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
#include <stdlib.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../utils/xutils.h"
#include "../include/xqueue_stack.h"
#include "../include/xqueue_fifo.h"
#include "xtree_binary_x.h"

XBinTree_PT xbintree_new(void) {
    XBinTree_PT tree = XMEM_CALLOC(1, sizeof(*tree));
    if (!tree) {
        return NULL;
    }

    //tree->root = NULL;
    //tree->size = 0;

    return tree;
}

XBinTree_Node_PT xbintree_new_node(void *data) {
    XBinTree_Node_PT node = XMEM_CALLOC(1, sizeof(*node));
    if (!node) {
        return NULL;
    }

    node->data = data;

    //node->parent = NULL;
    //node->left = NULL;
    //node->right = NULL;

    return node;
}

static
XBinTree_Node_PT xbintree_copy_apply(XBinTree_Node_PT node, XBinTree_Node_PT nparent, bool *false_found, void *cl) {
    XBinTree_Node_PT nnode = XMEM_CALLOC(1, sizeof(*nnode));
    if (!nnode) {
        *false_found = true;
        return NULL;
    }

    nnode->data = node->data;

    nnode->parent = nparent;
    //nnode->left = NULL;
    //nnode->right = NULL;

    return nnode;
}

static
void xbintree_copy_break_if_false_impl(XBinTree_PT tree, XBinTree_Node_PT node, XBinTree_PT ntree, XBinTree_Node_PT nparent, bool root, bool left, bool *false_found, XBinTree_Node_PT(*apply)(XBinTree_Node_PT node, XBinTree_Node_PT nparent, bool *false_found, void *cl), void *cl) {
    if (*false_found || !node) {
        return;
    }

    if (root) {
        ntree->root = apply(tree->root, NULL, false_found, cl);
        nparent = ntree->root;
    }
    else {
        if (left) {
            nparent->left = apply(node, nparent, false_found, cl);
            nparent = nparent->left;
        }
        else {
            nparent->right = apply(node, nparent, false_found, cl);
            nparent = nparent->right;
        }
    }

    if (*false_found) {
        return;
    }

    ++ntree->size;

    xbintree_copy_break_if_false_impl(tree, node->left, ntree, nparent, false, true, false_found, apply, cl);
    if (*false_found) {
        return;
    }

    xbintree_copy_break_if_false_impl(tree, node->right, ntree, nparent, false, false, false_found, apply, cl);
}

XBinTree_PT xbintree_copy(XBinTree_PT tree) {
    xassert(tree);

    if (!tree) {
        return NULL;
    }

    {
        bool false_found = false;

        XBinTree_PT ntree = xbintree_new();
        if (!ntree) {
            return NULL;
        }

        xbintree_copy_break_if_false_impl(tree, tree->root, ntree, NULL, true, false, &false_found, xbintree_copy_apply, NULL);
        if (false_found) {
            xbintree_free(&ntree);
            return NULL;
        }

        xassert(xbintree_size(tree) == xbintree_size(ntree));

        return ntree;
    }
}

static
XBinTree_Node_PT xbintree_deep_copy_apply(XBinTree_Node_PT node, XBinTree_Node_PT nparent, bool *false_found, void *cl) {
    XBinTree_3Paras_PT paras = (XBinTree_3Paras_PT)cl;

    XBinTree_Node_PT nnode = XMEM_CALLOC(1, sizeof(*nnode));
    if (!nnode) {
        *false_found = true;
        return NULL;
    }

    nnode->data = xutils_deep_copy(node->data, *((int*)paras->para1));
    if (!nnode->data) {
        *false_found = true;
        XMEM_FREE(nnode);
        return NULL;
    }

    nnode->parent = nparent;
    //nnode->left = NULL;
    //nnode->right = NULL;

    return nnode;
}

XBinTree_PT xbintree_deep_copy(XBinTree_PT tree, int elem_size) {
    xassert(tree);
    xassert(0 < elem_size);

    if (!tree || (elem_size <= 0)) {
        return NULL;
    }

    {
        XBinTree_PT ntree = xbintree_new();
        if (!ntree) {
            return NULL;
        }

        {
            XBinTree_3Paras_T paras = { ntree, (void*)&elem_size, NULL, NULL };

            bool false_found = false;
            xbintree_copy_break_if_false_impl(tree, tree->root, ntree, NULL, true, false, &false_found, xbintree_deep_copy_apply, (void*)&paras);
            if (false_found) {
                xbintree_free(&ntree);
                return NULL;
            }
        }

        xassert(xbintree_size(tree) == xbintree_size(ntree));

        return ntree;
    }
}

bool xbintree_insert_left(XBinTree_PT tree, XBinTree_Node_PT node, void *data) {
    xassert(tree);
    xassert(data);

    if (!tree || !data) {
        return false;
    }

    {
        XBinTree_Node_PT nnode = xbintree_new_node(data);
        if (!nnode) {
            return false;
        }

        if (node) {
            nnode->left = node->left;
            if (nnode->left) {
                nnode->left->parent = nnode;
            }
            nnode->parent = node;
            node->left = nnode;
        }
        else {
            if (0 < tree->size) {
                XMEM_FREE(nnode);
                return false;
            }

            tree->root = nnode;
        }

        tree->size++;
    }

    return true;
}

bool xbintree_insert_right(XBinTree_PT tree, XBinTree_Node_PT node, void *data) {
    xassert(tree);
    xassert(data);

    if (!tree || !data) {
        return false;
    }

    {
        XBinTree_Node_PT nnode = xbintree_new_node(data);
        if (!nnode) {
            return false;
        }

        if (node) {
            nnode->right = node->right;
            if (nnode->right) {
                nnode->right->parent = nnode;
            }
            nnode->parent = node;
            node->right = nnode;
        }
        else {
            if (0 < tree->size) {
                XMEM_FREE(nnode);
                return false;
            }

            tree->root = nnode;
        }

        tree->size++;
    }

    return true;
}

void* xbintree_root(XBinTree_PT tree) {
    return tree ? (tree->root ? tree->root->data : NULL) : NULL;
}

void xbintree_free_node_impl_apply(XBinTree_Node_PT node, bool (*apply)(void **data, void *cl), void *cl) {
    if (!node) {
        return;
    }

    xbintree_free_node_impl_apply(node->left, apply, cl);
    xbintree_free_node_impl_apply(node->right, apply, cl);

    apply(&node->data, cl);
    XMEM_FREE(node);
}

void xbintree_free_node_impl(XBinTree_Node_PT node, bool deep) {
    if (!node) {
        return;
    }

    xbintree_free_node_impl(node->left, deep);
    xbintree_free_node_impl(node->right, deep);

    if (deep) {
        XMEM_FREE(node->data);
    }
    XMEM_FREE(node);
}

static
void xbintree_free_impl(XBinTree_PT tree, XBinTree_Node_PT node, bool deep, bool (*apply)(void *data, void *cl), void *cl) {
    if (!node) {
        return;
    }

    xbintree_free_impl(tree, node->left, deep, apply, cl);
    xbintree_free_impl(tree, node->right, deep, apply, cl);

    if (deep) {
        XMEM_FREE(node->data);
    }
    else if (apply) {
        apply(node->data, cl);
    }
    XMEM_FREE(node);
    tree->size--;
}

static 
bool xbintree_remove_left_impl(XBinTree_PT tree, XBinTree_Node_PT node, bool deep) {
    xassert(tree);
    xassert(node);

    if (!tree || !node) {
        return false;
    }

    xbintree_free_impl(tree, node->left, deep, NULL, NULL);
    node->left = NULL;

    return true;
}

bool xbintree_remove_left(XBinTree_PT tree, XBinTree_Node_PT node) {
    return xbintree_remove_left_impl(tree, node, false);
}

bool xbintree_deep_remove_left(XBinTree_PT tree, XBinTree_Node_PT node) {
    return xbintree_remove_left_impl(tree, node, true);
}

static 
bool xbintree_remove_right_impl(XBinTree_PT tree, XBinTree_Node_PT node, bool deep) {
    xassert(tree);
    xassert(node);

    if (!tree || !node) {
        return false;
    }

    xbintree_free_impl(tree, node->right, deep, NULL, NULL);
    node->right = NULL;

    return true;
}

bool xbintree_remove_right(XBinTree_PT tree, XBinTree_Node_PT node) {
    return xbintree_remove_right_impl(tree, node, false);
}

bool xbintree_deep_remove_right(XBinTree_PT tree, XBinTree_Node_PT node) {
    return xbintree_remove_right_impl(tree, node, true);
}

void xbintree_clear(XBinTree_PT tree) {
    xbintree_free_impl(tree, (tree ? tree->root : NULL), false, NULL, NULL);
    tree->root = NULL;
}

void xbintree_clear_apply(XBinTree_PT tree, bool (*apply)(void *data, void *cl), void *cl) {
    xbintree_free_impl(tree, (tree ? tree->root : NULL), false, apply, cl);
    tree->root = NULL;
}

void xbintree_deep_clear(XBinTree_PT tree) {
    xbintree_free_impl(tree, (tree ? tree->root : NULL), true, NULL, NULL);
    tree->root = NULL;
}

void xbintree_free(XBinTree_PT *ptree) {
    if (!ptree || !*ptree) {
        return;
    }

    xbintree_free_impl(*ptree, (*ptree)->root, false, NULL, NULL);
    XMEM_FREE(*ptree);
}

void xbintree_free_apply(XBinTree_PT *ptree, bool (*apply)(void *data, void *cl), void *cl) {
    if (!ptree || !*ptree) {
        return;
    }

    xbintree_free_impl(*ptree, (*ptree)->root, false, apply, cl);
    XMEM_FREE(*ptree);
}

void xbintree_deep_free(XBinTree_PT *ptree) {
    if (!ptree || !*ptree) {
        return;
    }

    xbintree_free_impl(*ptree, (*ptree)->root, true, NULL, NULL);
    XMEM_FREE(*ptree);
}

static
void xbintree_remove_left_most_impl(XBinTree_PT tree, XBinTree_Node_PT node, bool deep, void **data) {
    if (!node) {
        return;
    }

    while (node->left) {
        node = node->left;
    }

    if (node == tree->root) {  // root has no left branch
        tree->root = node->right;
        if (tree->root) {
            tree->root->parent = NULL;
        }
    }
    else {
        node->parent->left = node->right;
        if (node->right) {
            node->right->parent = node->parent;
        }
    }

    if (deep) {
        XMEM_FREE(node->data);
    }
    else if (data) {
        *data = node->data;
    }

    XMEM_FREE(node);
    tree->size--;
}

void xbintree_remove_left_most(XBinTree_PT tree) {
    xbintree_remove_left_most_impl(tree, (tree ? tree->root : NULL), false, NULL);
}

void xbintree_remove_save_left_most(XBinTree_PT tree, void **data) {
    xbintree_remove_left_most_impl(tree, (tree ? tree->root : NULL), false, data);
}

void xbintree_deep_remove_left_most(XBinTree_PT tree) {
    xbintree_remove_left_most_impl(tree, (tree ? tree->root : NULL), true, NULL);
}

static
void xbintree_remove_right_most_impl(XBinTree_PT tree, XBinTree_Node_PT node, bool deep, void **data) {
    if (!node) {
        return;
    }

    while (node->right) {
        node = node->right;
    }

    {
        if (node == tree->root) {  // root has no right branch
            tree->root = node->left;
            if (tree->root) {
                tree->root->parent = NULL;
            }
        }
        else {
            node->parent->right = node->left;
            if (node->left) {
                node->left->parent = node->parent;
            }
        }

        if (deep) {
            XMEM_FREE(node->data);
        }
        else if (data) {
            *data = node->data;
        }

        XMEM_FREE(node);
        tree->size--;
    }
}

void xbintree_remove_right_most(XBinTree_PT tree) {
    xbintree_remove_right_most_impl(tree, (tree ? tree->root : NULL), false, NULL);
}

void xbintree_remove_save_right_most(XBinTree_PT tree, void **data) {
    xbintree_remove_right_most_impl(tree, (tree ? tree->root : NULL), false, data);
}

void xbintree_deep_remove_right_most(XBinTree_PT tree) {
    xbintree_remove_right_most_impl(tree, (tree ? tree->root : NULL), true, NULL);
}

static 
int xbintree_map_preorder_impl(XBinTree_Node_PT node, bool (*apply)(void *data, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        if (apply(node->data, cl)) {
            count++;
        }

        count += xbintree_map_preorder_impl(node->left, apply, cl);
        count += xbintree_map_preorder_impl(node->right, apply, cl);

        return count;
    }
}

int xbintree_map_preorder(XBinTree_PT tree, bool (*apply)(void *data, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xbintree_map_preorder_impl((tree ? tree->root : NULL), apply, cl);
}

int xbintree_map_inorder_impl(XBinTree_Node_PT node, bool (*apply)(void *data, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        count += xbintree_map_inorder_impl(node->left, apply, cl);

        if (apply(node->data, cl)) {
            count++;
        }

        count += xbintree_map_inorder_impl(node->right, apply, cl);

        return count;
    }
}

int xbintree_map_inorder(XBinTree_PT tree, bool (*apply)(void *data, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xbintree_map_inorder_impl((tree ? tree->root : NULL), apply, cl);
}

static
int xbintree_map_postorder_impl(XBinTree_Node_PT node, bool (*apply)(void *data, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        count += xbintree_map_postorder_impl(node->left, apply, cl);
        count += xbintree_map_postorder_impl(node->right, apply, cl);

        if (apply(node->data, cl)) {
            count++;
        }

        return count;
    }
}

int xbintree_map_postorder(XBinTree_PT tree, bool (*apply)(void *data, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xbintree_map_postorder_impl((tree ? tree->root : NULL), apply, cl);
}

int xbintree_map_levelorder(XBinTree_PT tree, bool (*apply)(void *data, void *cl), void *cl) {
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

        if (fifo) {
            if (!xfifo_push(fifo, (void*)tree->root)) {
                return -1;
            }

            while (!xfifo_is_empty(fifo)) {
                XBinTree_Node_PT node = (XBinTree_Node_PT)xfifo_pop(fifo);
                if (apply(node->data, cl)) {
                    count++;
                }

                if (node->left) {
                    if (!xfifo_push(fifo, (void*)node->left)) {
                        return -1;
                    }
                }
                if (node->right) {
                    if (!xfifo_push(fifo, (void*)node->right)) {
                        return -1;
                    }
                }
            }

            xfifo_free(&fifo);
        }

        return count;
    }
}

void xbintree_map_inorder_break_if_true_impl(XBinTree_Node_PT node, bool *true_found, bool (*apply)(void *data, void *cl), void *cl) {
    if (*true_found || !node) {
        return;
    }

    xbintree_map_inorder_break_if_true_impl(node->left, true_found, apply, cl);
    if (*true_found) {
        return;
    }

    if (apply(node->data, cl)) {
        *true_found = true;
        return;
    }

    xbintree_map_inorder_break_if_true_impl(node->right, true_found, apply, cl);
}

bool xbintree_map_inorder_break_if_true(XBinTree_PT tree, bool (*apply)(void *data, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return false;
    }

    {
        bool true_found = false;
        xbintree_map_inorder_break_if_true_impl((tree ? tree->root : NULL), &true_found, apply, cl);
        return true_found;
    }
}

void xbintree_map_inorder_break_if_false_impl(XBinTree_Node_PT node, bool *false_found, bool (*apply)(void *data, void *cl), void *cl) {
    if (*false_found || !node) {
        return;
    }

    xbintree_map_inorder_break_if_false_impl(node->left, false_found, apply, cl);
    if (*false_found) {
        return;
    }

    if (!apply(node->data, cl)) {
        *false_found = true;
        return;
    }

    xbintree_map_inorder_break_if_false_impl(node->right, false_found, apply, cl);
}

bool xbintree_map_inorder_break_if_false(XBinTree_PT tree, bool (*apply)(void *data, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return true;
    }

    {
        bool false_found = false;
        xbintree_map_inorder_break_if_false_impl((tree ? tree->root : NULL), &false_found, apply, cl);
        return false_found;
    }
}

int xbintree_map_preorder_stack(XBinTree_PT tree, bool (*apply)(void *data, void *cl), void *cl) {
    xassert(tree);
    xassert(apply);

    if (!tree || !apply) {
        return -1;
    }

    if (tree->size == 0) {
        return 0;
    }

    {
        int count = 0;

        XStack_PT stack = xstack_new(0);
        if (!stack) {
            return -1;
        }

        if (!xstack_push(stack, (void*)tree->root)) {
            return -1;
        }

        while (!xstack_is_empty(stack)) {
            XBinTree_Node_PT node = (XBinTree_Node_PT)xstack_pop(stack);
            if (apply(node->data, cl)) {
                count++;
            }

            if (node->right) {
                if (!xstack_push(stack, (void*)node->right))
                {
                    return -1;
                }
            }
            if (node->left) {
                if (!xstack_push(stack, (void*)node->left)) {
                    return -1;
                }
            }
        }

        xstack_free(&stack);

        return count;
    }
}

int xbintree_map_inorder_stack(XBinTree_PT tree, bool (*apply)(void *data, void *cl), void *cl) {
    xassert(tree);
    xassert(apply);

    if (!tree || !apply || (tree->size <= 0)) {
        return 0;
    }

    {
        int count = 0;

        XStack_PT stack = xstack_new(0);
        if (!stack) {
            return -1;
        }

        {
            XBinTree_PT ntree = xbintree_copy(tree);
            if (!ntree) {
                xstack_free(&stack);
                return -1;
            }

            {
                XBinTree_Node_PT left, right, node = ntree->root;

                while (node) {
                    left = node->left;
                    right = node->right;

                    if ((!left) && (!right)) {
                        if (apply(node->data, cl)) {
                            count++;
                        }

                        XMEM_FREE(node);

                        if (!xstack_is_empty(stack)) {
                            node = (XBinTree_Node_PT)xstack_pop(stack);
                            continue;
                        }
                        else {
                            break;
                        }
                    }

                    if (right) {
                        if (!xstack_push(stack, (void*)right)) {
                            return -1;
                        }
                    }

                    node->parent = node->left = node->right = NULL;
                    if (!xstack_push(stack, (void*)node)) {
                        return -1;
                    }

                    if (left) {
                        if (!xstack_push(stack, (void*)left)) {
                            return -1;
                        }
                    }

                    node = (XBinTree_Node_PT)xstack_pop(stack);
                }
            }

            xbintree_free(&ntree);
        }

        xstack_free(&stack);

        return count;
    }
}

int xbintree_map_postorder_stack(XBinTree_PT tree, bool (*apply)(void *data, void *cl), void *cl) {
    xassert(tree);
    xassert(apply);

    if (!tree || !apply || (tree->size <= 0)) {
        return 0;
    }

    {
        int count = 0;

        XStack_PT stack = xstack_new(0);
        if (!stack) {
            return -1;
        }

        {
            XBinTree_PT ntree = xbintree_copy(tree);
            if (!ntree) {
                xstack_free(&stack);
                return -1;
            }

            {
                XBinTree_Node_PT left, right, node = ntree->root;

                while (node) {
                    left = node->left;
                    right = node->right;

                    if ((!left) && (!right)) {
                        if (apply(node->data, cl)) {
                            count++;
                        }

                        XMEM_FREE(node);

                        if (!xstack_is_empty(stack)) {
                            node = (XBinTree_Node_PT)xstack_pop(stack);
                            continue;
                        }
                        else {
                            break;
                        }
                    }

                    node->parent = node->left = node->right = NULL;
                    if (!xstack_push(stack, (void*)node)) {
                        return -1;
                    }

                    if (right) {
                        if (!xstack_push(stack, right)) {
                            return -1;
                        }
                    }

                    if (left) {
                        if (!xstack_push(stack, left)) {
                            return -1;
                        }
                    }

                    node = (XBinTree_Node_PT)xstack_pop(stack);
                }
            }

            xbintree_free(&ntree);
        }

        xstack_free(&stack);

        return count;
    }
}

bool xbintree_swap(XBinTree_PT tree1, XBinTree_PT tree2) {
    xassert(tree1);
    xassert(tree2);

    if (!tree1 || !tree2) {
        return false;
    }

    {
        XBinTree_Node_PT root = tree1->root;
        int size = tree1->size;

        tree1->root = tree2->root;
        tree1->size = tree2->size;
    
        tree2->root = root;
        tree2->size = size;
    }

    return true;
}

int xbintree_size(XBinTree_PT tree) {
    return (tree ? tree->size : 0);
}

bool xbintree_is_empty(XBinTree_PT tree) {
    return (tree ? (tree->size == 0) : true);
}

static
int xbintree_height_impl(XBinTree_Node_PT node) {
    if (!node) {
        return 0;
    }

    {
        int u = xbintree_height_impl(node->left);
        int v = xbintree_height_impl(node->right);

        return u < v ? (v + 1) : (u + 1);
    }
}

int xbintree_height(XBinTree_PT tree) {
    xassert(tree);

    if (!tree) {
        return 0;
    }

    return xbintree_height_impl(tree->root);
}

XBinTree_PT xbintree_new_with_root(XBinTree_Node_PT root, int size) {
    XBinTree_PT tree = XMEM_CALLOC(1, sizeof(*tree));
    if (!tree) {
        return NULL;
    }

    tree->root = root;
    tree->size = size;

    return tree;
}

XBinTree_Node_PT xbintree_reset(XBinTree_PT tree) {
    XBinTree_Node_PT root = tree->root;
    tree->root = NULL;
    tree->size = 0;

    return root;
}

void* xbintree_node_data(XBinTree_Node_PT node) {
    return node->data;
}
