/*
*   Copyright (C) 2022, Xiaosan Zhai(tom.zhai@aliyun.com)
*   This file is part of the xalgos library.
*
*   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
*   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*   See the MIT License for more details.
*
*   You should have received a copy of the MIT License along with this program.
*   If not, see <https://mit-license.org/>.
*/

#include <stdio.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../tree_set/xtree_set_x.h"
#include "../include/xqueue_fifo.h"
#include "xtree_multiple_branch_x.h"

XMTree_PT xmtree_new(int(*cmp) (void *key1, void *key2, void *cl), void *cl) {
    xassert(cmp);

    if (!cmp) {
        return NULL;
    }

    {
        XMTree_PT tree = (XMTree_PT)XMEM_CALLOC(1, sizeof(*tree));
        if (!tree) {
            return NULL;
        }

        tree->cmp = cmp;
        tree->cl = cl;
        //tree->root = NULL;

        return tree;
    }
}

static
void xmtree_free_impl(XMTree_PT tree, XMTree_Node_PT node, bool deep);

static 
bool xmtree_free_impl_apply(void *key, void *cl) {
    XMTree_3Paras_PT paras = (XMTree_3Paras_PT)cl;
    xmtree_free_impl(paras->tree, (XMTree_Node_PT)key, *((bool*)paras->para1));
    return true;
}

static
void xmtree_free_impl(XMTree_PT tree, XMTree_Node_PT node, bool deep) {
    if (!node) {
        return;
    }

    {
        XMTree_3Paras_T paras = { tree, (void*)&deep, NULL, NULL };
        xset_map(node->children, xmtree_free_impl_apply, (void*)&paras);
        xset_free(&node->children);
    }

    if (deep) {
        XMEM_FREE(node->key);
        XMEM_FREE(node->value);
    }
    XMEM_FREE(node);

    return;
}

void xmtree_clear(XMTree_PT tree) {
    xassert(tree);

    if (!tree) {
        return;
    }

    xmtree_free_impl(tree, tree->root, false);
    tree->root = NULL;

    return;
}

void xmtree_deep_clear(XMTree_PT tree) {
    xassert(tree);

    if (!tree) {
        return;
    }

    xmtree_free_impl(tree, tree->root, true);
    tree->root = NULL;

    return;
}

void xmtree_free(XMTree_PT *ptree) {
    if (!ptree || !*ptree) {
        return;
    }

    xmtree_free_impl(*ptree, (*ptree)->root, false);
    XMEM_FREE(*ptree);

    return;
}

void xmtree_deep_free(XMTree_PT *ptree) {
    if (!ptree || !*ptree) {
        return;
    }

    xmtree_free_impl(*ptree, (*ptree)->root, true);
    XMEM_FREE(*ptree);

    return;
}

static
XMTree_Node_PT xmtree_new_node(XMTree_PT tree, XMTree_Node_PT parent, void *key, void *value) {
    XMTree_Node_PT node = (XMTree_Node_PT)XMEM_CALLOC(1, sizeof(*node));
    if (!node) {
        return NULL;
    }

    node->parent = parent;
    node->children = xset_new(tree->cmp, tree->cl);
    if (!node->children) {
        XMEM_FREE(node);
        return NULL;
    }

    node->key = key;
    node->value = value;

    //node->size = 1;

    return node;
}

static
void xmtree_free_node(XMTree_Node_PT *node) {
    xset_free(&(*node)->children);
    XMEM_FREE(*node);
}

XMTree_Node_PT xmtree_put_repeat(XMTree_PT tree, XMTree_Node_PT parent, void *key, void *value) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return NULL;
    }

    {
        XMTree_Node_PT nnode = xmtree_new_node(tree, parent, key, value);
        if (!nnode) {
            return NULL;
        }

        if (parent) {
            if (!xset_put_repeat(parent->children, nnode)) {
                xmtree_free_node(&nnode);
                return NULL;
            }
        }
        else {
            tree->root = nnode;
        }

        return nnode;
    }
}

static
int xmtree_map_preorder_impl(XMTree_PT tree, XMTree_Node_PT node, bool (*apply)(void *key, void **value, void *cl), void *cl);

static 
bool xmtree_map_preorder_impl_apply(void *key, void *cl) {
    XMTree_3Paras_PT paras = (XMTree_3Paras_PT)cl;
    int *count = (int*)paras->para1/*count*/;
    *count += xmtree_map_preorder_impl(paras->tree, (XMTree_Node_PT)key, (bool (*)(void *key, void **value, void *cl))paras->para2/*apply*/, paras->para3/*cl*/);
    return true;
}

static
int xmtree_map_preorder_impl(XMTree_PT tree, XMTree_Node_PT node, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        if (apply(node->key, &(node->value), cl)) {
            count++;
        }

        {
            XMTree_3Paras_T paras = { tree, (void*)&count, (void*)apply, cl};
            xset_map(node->children, xmtree_map_preorder_impl_apply, (void*)&paras);
        }

        return count;
    }
}

int xmtree_map_preorder(XMTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(apply);

    if (!tree || !apply) {
        return 0;
    }

    return xmtree_map_preorder_impl(tree, tree->root, apply, cl);
}

static
void xmtree_map_preorder_break_if_true_impl(XMTree_PT tree, XMTree_Node_PT node, XMTree_3Paras_PT paras);

static
bool xmtree_map_preorder_break_if_true_impl_apply(void *key, void *cl) {
    XMTree_3Paras_PT paras = (XMTree_3Paras_PT)cl;
    xmtree_map_preorder_break_if_true_impl(paras->tree, (XMTree_Node_PT)key, paras);
    return *(bool*)paras->para1/*true_found*/;
}

static
void xmtree_map_preorder_break_if_true_impl(XMTree_PT tree, XMTree_Node_PT node, XMTree_3Paras_PT paras) {
    bool *true_found = (bool*)paras->para1/*true_found*/;
    if (*true_found || !node) {
        return;
    }

    if ((*(bool (*)(void *key, void **value, void *cl))paras->para2/*apply*/)(node->key, &(node->value), paras->para3/*cl*/)) {
        *true_found = true;
        return;
    }

    xset_map_break_if_true(node->children, xmtree_map_preorder_break_if_true_impl_apply, (void*)paras);
}

bool xmtree_map_preorder_break_if_true(XMTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(apply);

    if (!tree || !apply) {
        return false;
    }

    {
        bool true_found = false;

        XMTree_3Paras_T paras = { tree, (void*)&true_found, (void*)apply, cl };
        xmtree_map_preorder_break_if_true_impl(tree, tree->root, &paras);

        return true_found;
    }
}

static
void xmtree_map_preorder_break_if_false_impl(XMTree_PT tree, XMTree_Node_PT node, XMTree_3Paras_PT paras);

static
bool xmtree_map_preorder_break_if_false_impl_apply(void *key, void *cl) {
    XMTree_3Paras_PT paras = (XMTree_3Paras_PT)cl;
    xmtree_map_preorder_break_if_false_impl(paras->tree, (XMTree_Node_PT)key, paras);
    return *(bool*)paras->para1/*false_found*/;
}

static
void xmtree_map_preorder_break_if_false_impl(XMTree_PT tree, XMTree_Node_PT node, XMTree_3Paras_PT paras) {
    bool *false_found = (bool*)paras->para1/*false_found*/;
    if (*false_found || !node) {
        return;
    }

    if (!(*(bool (*)(void *key, void **value, void *cl))paras->para2/*apply*/)(node->key, &(node->value), paras->para3/*cl*/)) {
        *false_found = true;
        return;
    }

    xset_map_break_if_true(node->children, xmtree_map_preorder_break_if_false_impl_apply, (void*)paras);
}

bool xmtree_map_preorder_break_if_false(XMTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(apply);

    if (!tree || !apply) {
        return true;
    }

    {
        bool false_found = false;

        XMTree_3Paras_T paras = { tree, (void*)&false_found, (void*)apply, cl };
        xmtree_map_preorder_break_if_false_impl(tree, tree->root, &paras);

        return false_found;
    }
}

static
int xmtree_map_postorder_impl(XMTree_PT tree, XMTree_Node_PT node, bool (*apply)(void *key, void **value, void *cl), void *cl);

static
bool xmtree_map_postorder_impl_apply(void *key, void *cl) {
    XMTree_3Paras_PT paras = (XMTree_3Paras_PT)cl;
    int *count = (int*)paras->para1/*count*/;
    *count += xmtree_map_postorder_impl(paras->tree, (XMTree_Node_PT)key, (bool (*)(void *key, void **value, void *cl))paras->para2/*apply*/, paras->para3/*cl*/);
    return true;
}

static
int xmtree_map_postorder_impl(XMTree_PT tree, XMTree_Node_PT node, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        {
            XMTree_3Paras_T paras = { tree, (void*)&count, (void*)apply, cl };
            xset_map(node->children, xmtree_map_postorder_impl_apply, (void*)&paras);
        }

        if (apply(node->key, &(node->value), cl)) {
            count++;
        }

        return count;
    }
}

int xmtree_map_postorder(XMTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(apply);

    if (!tree || !apply) {
        return 0;
    }

    return xmtree_map_postorder_impl(tree, tree->root, apply, cl);
}

static
bool xmtree_map_levelorder_apply(void *key, void *cl) {
    XMTree_3Paras_PT paras = (XMTree_3Paras_PT)cl;
    if (!xfifo_push((XFifo_PT)paras->para1/*fifo*/, key)) {
        return false;
    }
    return true;
}

int xmtree_map_levelorder(XMTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(apply);

    if (!tree || !apply) {
        return 0;
    }

    {
        int count = 0;

        XFifo_PT fifo = xfifo_new(0);
        if (!fifo) {
            return 0;
        }

        if (!xfifo_push(fifo, (void*)tree->root)) {
            xfifo_free(&fifo);
            return -1;
        }

        while (!xfifo_is_empty(fifo)) {
            XMTree_Node_PT node = (XMTree_Node_PT)xfifo_pop(fifo);
            if (apply(node->key, &(node->value), cl)) {
                count++;
            }

            {
                XMTree_3Paras_T paras = { tree, (void*)&fifo, NULL, NULL };
                if (xset_map_break_if_false(node->children, xmtree_map_levelorder_apply, (void*)&paras)) {
                    xfifo_free(&fifo);
                    return -1;
                }
            }
        }

        xfifo_free(&fifo);

        return count;
    }
}

static
bool xmtree_to_string_apply(void *key, void *cl) {
    if (!key) {
        return true;
    }

    {
        XMTree_3Paras_PT paras = (XMTree_3Paras_PT)cl;
        if (!xfifo_push((XFifo_PT)paras->para1/*fifo*/, key)) {
            return false;
        }

        XMTree_Node_PT node = (XMTree_Node_PT)key;
        printf("%s ", (char*)node->key);
    }

    return true;
}

bool xmtree_to_string(XMTree_PT tree) {
    XFifo_PT fifo = xfifo_new(0);
    if (!fifo) {
        return false;
    }

    if (!xfifo_push(fifo, (void*)tree->root)) {
        xfifo_free(&fifo);
        return false;
    }

    while (!xfifo_is_empty(fifo)) {
        XMTree_Node_PT node = (XMTree_Node_PT)xfifo_pop(fifo);
        printf("%s : { ", (char*)node->key);

        {
            XMTree_3Paras_T paras = { tree, (void*)fifo, NULL, NULL };
            if (xset_map_break_if_false(node->children, xmtree_to_string_apply, (void*)&paras)) {
                xfifo_free(&fifo);
                return false;
            }
        }

        printf("}\n");
    }

    xfifo_free(&fifo);

    return true;
}
