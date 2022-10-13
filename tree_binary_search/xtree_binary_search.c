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
#include "../include/xqueue_fifo.h"
#include "xtree_binary_search_x.h"

static
XBSTree_Node_PT xbstree_find_impl(XBSTree_PT tree, XBSTree_Node_PT node, void *key) {
    if (!key) {
        return NULL;
    }

    while (node) {
        int ret = tree->cmp(key, node->key, tree->cl);
        if (ret == 0) {
            return node;
        }
        else if (ret < 0) {
            node = node->left;
        }
        else {
            node = node->right;
        }
    }

    return NULL;
}

static
XBSTree_Node_PT xbstree_min_impl(XBSTree_PT tree, XBSTree_Node_PT node) {
    if (!node) {
        return NULL;
    }

    while (node->left) {
        node = node->left;
    }

    return node;
}

static
XBSTree_Node_PT xbstree_max_impl(XBSTree_PT tree, XBSTree_Node_PT node) {
    if (!node) {
        return NULL;
    }

    while (node->right) {
        node = node->right;
    }

    return node;
}

static
XBSTree_Node_PT xbstree_prev_node(XBSTree_PT tree, XBSTree_Node_PT node) {
    if (node->left) {
        return xbstree_max_impl(tree, node->left);
    }

    {
        /* H's pre node is E :
        *      E
        *       \
        *         R
        *        / \
        *       H   S
        */
        XBSTree_Node_PT parent = node->parent;
        while (parent && (node == parent->left)) {
            node = parent;
            parent = parent->parent;
        }

        return parent;
    }
}

static
XBSTree_Node_PT xbstree_next_node(XBSTree_PT tree, XBSTree_Node_PT node) {
    if (node->right) {
        return xbstree_min_impl(tree, node->right);
    }

    {
        /* R's next node is S :
        *         S
        *       /   \
        *     E       T
        *      \
        *       R
        */
        XBSTree_Node_PT parent = node->parent;
        while (parent && (node == parent->right)) {
            node = parent;
            parent = parent->parent;
        }

        return parent;
    }
}

XBSTree_PT xbstree_new(int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(cmp);
    
    if (!cmp) {
        return NULL;
    }

    {
        XBSTree_PT tree = XMEM_CALLOC(1, sizeof(*tree));
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
XBSTree_Node_PT xbstree_copy_node(XBSTree_Node_PT node, XBSTree_Node_PT nparent, bool *false_found, void *cl) {
    XBSTree_Node_PT nnode = XMEM_CALLOC(1, sizeof(*nnode));
    if (!nnode) {
        *false_found = true;
        return NULL;
    }

    nnode->key = node->key;
    nnode->value = node->value;

    nnode->size = node->size;

    nnode->parent = nparent;
    //nnode->left = NULL;
    //nnode->right = NULL;

    return nnode;
}

static
void xbstree_copy_break_if_false_impl(XBSTree_PT tree, XBSTree_Node_PT node, XBSTree_PT ntree, XBSTree_Node_PT nparent, bool root, bool left, bool *false_found, XBSTree_Node_PT(*apply)(XBSTree_Node_PT node, XBSTree_Node_PT nparent, bool *false_found, void *cl), void *cl) {
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

    xbstree_copy_break_if_false_impl(tree, node->left, ntree, nparent, false, true, false_found, apply, cl);
    if (*false_found) {
        return;
    }

    xbstree_copy_break_if_false_impl(tree, node->right, ntree, nparent, false, false, false_found, apply, cl);
}

XBSTree_PT xbstree_copy(XBSTree_PT tree) {
    xassert(tree);

    if (!tree) {
        return NULL;
    }

    {
        bool false_found = false;

        XBSTree_PT ntree = xbstree_new(tree->cmp, tree->cl);
        if (!ntree) {
            return NULL;
        }

        xbstree_copy_break_if_false_impl(tree, tree->root, ntree, NULL, true, false, &false_found, xbstree_copy_node, NULL);
        if (false_found) {
            xbstree_free(&ntree);
            return NULL;
        }

        xassert(xbstree_size(tree) == xbstree_size(ntree));

        return ntree;
    }
}

static
XBSTree_Node_PT xbstree_deep_copy_node(XBSTree_Node_PT node, XBSTree_Node_PT nparent, bool *false_found, void *cl) {
    XBSTree_3Paras_PT paras = (XBSTree_3Paras_PT)cl;

    XBSTree_Node_PT nnode = XMEM_CALLOC(1, sizeof(*nnode));
    if (!nnode) {
        *false_found = true;
        return NULL;
    }

    nnode->key = xutils_deep_copy(node->key, *((int*)paras->para1));
    if (!nnode->key) {
        *false_found = true;
        XMEM_FREE(nnode);
        return NULL;
    }

    if (0 < *((int*)paras->para2)) {
        nnode->value = xutils_deep_copy(node->value, *((int*)paras->para2));
        if (node->value && !nnode->value) {
            *false_found = true;
            XMEM_FREE(nnode->key);
            XMEM_FREE(nnode);
            return NULL;
        }
    }
    else {
        nnode->value = NULL;
    }

    nnode->size = node->size;

    nnode->parent = nparent;
    //nnode->left = NULL;
    //nnode->right = NULL;

    return nnode;
}

XBSTree_PT xbstree_deep_copy(XBSTree_PT tree, int key_size, int value_size) {
    xassert(tree);
    xassert(0 < key_size);
    xassert(0 <= value_size);

    if (!tree || (key_size <= 0) || (value_size < 0)) {
        return NULL;
    }

    {
        XBSTree_PT ntree = xbstree_new(tree->cmp, tree->cl);
        if (!ntree) {
            return NULL;
        }

        {
            XBSTree_3Paras_T paras = { ntree, (void*)&key_size, (void*)&value_size, NULL };

            bool false_found = false;
            xbstree_copy_break_if_false_impl(tree, tree->root, ntree, NULL, true, false, &false_found, xbstree_deep_copy_node, (void*)&paras);
            if (false_found) {
                xbstree_free(&ntree);
                return NULL;
            }
        }

        xassert(xbstree_size(tree) == xbstree_size(ntree));

        return ntree;
    }
}

static 
XBSTree_Node_PT xbstree_new_node(void *key, void *value) {
    XBSTree_Node_PT node = XMEM_CALLOC(1, sizeof(*node));
    if (!node) {
        return NULL;
    }

    node->key = key;
    node->value = value;

    node->size = 1;

    //node->parent = NULL;
    //node->left = NULL;
    //node->right = NULL;

    return node;
}

static
bool xbstree_put_repeat_impl(XBSTree_PT tree, void* key, void* value, bool unique) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return false;
    }

    {
        XBSTree_Node_PT parent = NULL;
        XBSTree_Node_PT* node = &tree->root;

        while (*node) {
            int ret = tree->cmp(key, (*node)->key, tree->cl);
            if ((ret == 0) && unique) {
                return true;
            }

            parent = *node;
            if (ret < 0) {
                node = &(*node)->left;
            }
            /* the equal key will be inserted into right branch too */
            else {
                node = &(*node)->right;
            }
        }

        xassert_false(*node);

        /* reach the leaf node */
        *node = xbstree_new_node(key, value);
        if (!*node) {
            return false;
        }
        (*node)->parent = parent;

        /* increase the parent node size */
        while (parent) {
            ++parent->size;
            parent = parent->parent;
        }

        return true;
    }
}

bool xbstree_put_repeat(XBSTree_PT tree, void *key, void *value) {
    return xbstree_put_repeat_impl(tree, key, value, false);
}

bool xbstree_put_unique(XBSTree_PT tree, void *key, void *value) {
    return xbstree_put_repeat_impl(tree, key, value, true);
}

static
bool xbstree_put_replace_impl(XBSTree_PT tree, void* key, void* value, void** old_value, bool deep) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return false;
    }

    {
        XBSTree_Node_PT parent = NULL;
        XBSTree_Node_PT* node = &tree->root;

        while (*node) {
            int ret = tree->cmp(key, (*node)->key, tree->cl);
            /* find the equal key, replace the value */
            if (ret == 0) {
                if (deep) {
                    XMEM_FREE((*node)->value);
                }
                else if (old_value) {
                    *old_value = (*node)->value;
                }

                (*node)->value = value;
                return true;
            }

            parent = *node;
            if (ret < 0) {
                node = &(*node)->left;
            }
            else {
                node = &(*node)->right;
            }
        }

        xassert_false(*node);

        /* reach the leaf node */
        *node = xbstree_new_node(key, value);
        if (!*node) {
            return false;
        }
        (*node)->parent = parent;

        /* increase the parent node size */
        while (parent) {
            ++parent->size;
            parent = parent->parent;
        }

        return true;
    }
}

bool xbstree_put_replace(XBSTree_PT tree, void* key, void* value, void** old_value) {
    return xbstree_put_replace_impl(tree, key, value, old_value, false);
}

bool xbstree_put_deep_replace(XBSTree_PT tree, void *key, void *value) {
    return xbstree_put_replace_impl(tree, key, value, NULL, true);
}

void* xbstree_min(XBSTree_PT tree) {
    XBSTree_Node_PT node = xbstree_min_impl(tree, tree ? tree->root : NULL);
    return node ? node->key : NULL;
}

void* xbstree_max(XBSTree_PT tree) {
    XBSTree_Node_PT node = xbstree_max_impl(tree, tree ? tree->root : NULL);
    return node ? node->key : NULL;
}

static
XBSTree_Node_PT xbstree_floor_impl(XBSTree_PT tree, XBSTree_Node_PT node, void* key) {
    if (!key) {
        return NULL;
    }

    {
        XBSTree_Node_PT result = NULL;

        while (node) {
            int ret = tree->cmp(node->key, key, tree->cl);
            if (ret == 0) {
                return node;
            }
            else if (ret < 0) {
                result = node;
                node = node->right;
            }
            else {
                node = node->left;
            }
        }

        return result;
    }
}

/* the maximum element which is <= key */
void* xbstree_floor(XBSTree_PT tree, void *key) {
    XBSTree_Node_PT floor = xbstree_floor_impl(tree, (tree ? tree->root : NULL), key);
    return floor ? floor->key : NULL;
}

static
XBSTree_Node_PT xbstree_ceiling_impl(XBSTree_PT tree, XBSTree_Node_PT node, void* key) {
    if (!key) {
        return NULL;
    }

    {
        XBSTree_Node_PT result = NULL;

        while (node) {
            int ret = tree->cmp(node->key, key, tree->cl);
            if (ret == 0) {
                return node;
            }
            else if (ret < 0) {
                node = node->right;
            }
            else {
                result = node;
                node = node->left;
            }
        }

        return result;
    }
}

/* the minimum element which is >= key */
void* xbstree_ceiling(XBSTree_PT tree, void *key) {
    XBSTree_Node_PT ceiling = xbstree_ceiling_impl(tree, (tree ? tree->root : NULL), key);
    return ceiling ? ceiling->key : NULL;
}

static
XBSTree_Node_PT xbstree_select_impl(XBSTree_PT tree, XBSTree_Node_PT node, int k) {
    while (node) {
        int t = node->left ? node->left->size : 0;
        if (t == k) {
            return node;
        }
        else if (t < k) {
            k -= t + 1;
            node = node->right;
        }
        else {
            node = node->left;
        }
    }

    xassert(false); // should never reach here
    return NULL;
}

/* element number is : 0, 1, 2, ... */
void* xbstree_select(XBSTree_PT tree, int k) {
    xassert(tree);
    xassert(0 <= k);
    xassert(k < xbstree_size(tree));

    if (!tree || (k < 0) || (xbstree_size(tree) <= k)) {
        return NULL;
    }

    return xbstree_select_impl(tree, tree->root, k)->key;
}

static
int xbstree_rank_impl(XBSTree_PT tree, XBSTree_Node_PT node, void *key) {
    if (!key) {
        return -1;
    }

    {
        int k = 0;
        while (node) {
            int ret = tree->cmp(key, node->key, tree->cl);
            if (ret == 0) {
                return k + (node->left ? node->left->size : 0);
            }
            else if (ret < 0) {
                node = node->left;
            }
            else {
                k += 1 + (node->left ? node->left->size : 0);
                node = node->right;
            }
        }
    }

    return -1;
}

/* element number of the key : 0, 1, 2, ... */
int xbstree_rank(XBSTree_PT tree, void *key) {
    return xbstree_rank_impl(tree, (tree ? tree->root : NULL), key);
}

void* xbstree_get(XBSTree_PT tree, void *key) {
    XBSTree_Node_PT node = xbstree_find_impl(tree, (tree ? tree->root : NULL), key);
    return node ? node->value : NULL;
}

XSList_PT xbstree_get_all(XBSTree_PT tree, void *key) {
    XSList_PT list = xslist_new();
    if (!list) {
        return NULL;
    }

    {
        XBSTree_Node_PT node = xbstree_find_impl(tree, (tree ? tree->root : NULL), key);
        if (node) {
            /* the equal keys may be saved in pre nodes */
            XBSTree_Node_PT tnode = xbstree_prev_node(tree, node);
            while (tnode) {
                if (tree->cmp(node->key, tnode->key, tree->cl) == 0) {
                    if (!xslist_push_back_repeat(list, tnode->value)) {
                        xslist_free(&list);
                        return NULL;
                    }
                }
                tnode = xbstree_prev_node(tree, tnode);
            }
        }

        while (node && (tree->cmp(node->key, key, tree->cl) <= 0)) {
            if (!xslist_push_back_repeat(list, node->value)) {
                xslist_free(&list);
                return NULL;
            }
            node = xbstree_next_node(tree, node);
        }
    }

    return list;
}

bool xbstree_find(XBSTree_PT tree, void *key) {
    return xbstree_find_impl(tree, (tree ? tree->root : NULL), key) ? true : false;
}

bool xbstree_find_replace(XBSTree_PT tree, void *key, void *value, void **old_value) {
    XBSTree_Node_PT node = xbstree_find_impl(tree, (tree ? tree->root : NULL), key);
    if (node) {
        if (old_value) {
            *old_value = node->value;
        }
        node->value = value;
        return true;
    }

    return false;
}

bool xbstree_find_deep_replace(XBSTree_PT tree, void *key, void *value) {
    XBSTree_Node_PT node = xbstree_find_impl(tree, (tree ? tree->root : NULL), key);
    if (node) {
        XMEM_FREE(node->value);
        node->value = value;
        return true;
    }

    return false;
}

bool xbstree_find_put_if_not(XBSTree_PT tree, void *key, void *value) {
    return xbstree_put_unique(tree, key, value);
}

XSList_PT xbstree_keys(XBSTree_PT tree, void *low_key, void *high_key) {
    xassert(tree);
    xassert(low_key);
    xassert(high_key);

    if (!tree || !low_key || !high_key) {
        return NULL;
    }

    // exchange the key if needed
    if (tree->cmp(high_key, low_key, tree->cl) < 0) {
        void* tmp = high_key;
        high_key = low_key;
        low_key = tmp;
    }

    {
        XBSTree_Node_PT node = xbstree_ceiling_impl(tree, tree->root, low_key);
        if (!node) {
            return NULL;
        }

        {
            XSList_PT list = xslist_new();
            if (!list) {
                return NULL;
            }

            while (node && tree->cmp(node->key, high_key, tree->cl) <= 0) {
                if (!xslist_push_back_repeat(list, node->key)) {
                    xslist_free(&list);
                    return NULL;
                }
                node = xbstree_next_node(tree, node);
            }

            return list;
        }
    }
}

static
void xbstree_free_impl(XBSTree_PT tree, XBSTree_Node_PT node, bool deep, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return;
    }

    xbstree_free_impl(tree, node->left, deep, apply, cl);
    xbstree_free_impl(tree, node->right, deep, apply, cl);

    if (deep) {
        XMEM_FREE(node->key);
        XMEM_FREE(node->value);
    }
    else if (apply) {
        apply(node->key, &node->value, cl);
    }
    XMEM_FREE(node);

    return;
}

void xbstree_clear(XBSTree_PT tree) {
    xbstree_free_impl(tree, (tree ? tree->root : NULL), false, NULL, NULL);
    tree->root = NULL;
}

void xbstree_clear_apply(XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xbstree_free_impl(tree, (tree ? tree->root : NULL), false, apply, cl);
    tree->root = NULL;
}

void xbstree_deep_clear(XBSTree_PT tree) {
    xbstree_free_impl(tree, (tree ? tree->root : NULL), true, NULL, NULL);
    tree->root = NULL;
}

void xbstree_free(XBSTree_PT *ptree) {
    if (!ptree || !*ptree) {
        return;
    }

    xbstree_free_impl(*ptree, (*ptree)->root, false, NULL, NULL);
    XMEM_FREE(*ptree);
}

void xbstree_free_apply(XBSTree_PT *ptree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!ptree || !*ptree) {
        return;
    }

    xbstree_free_impl(*ptree, (*ptree)->root, false, apply, cl);
    XMEM_FREE(*ptree);
}

void xbstree_deep_free(XBSTree_PT *ptree) {
    if (!ptree || !*ptree) {
        return;
    }

    xbstree_free_impl(*ptree, (*ptree)->root, true, NULL, NULL);
    XMEM_FREE(*ptree);
}

static
void xbstree_remove_min_impl(XBSTree_PT tree, XBSTree_Node_PT node, bool deep, void **key, void **value) {
    if (!node) {
        return ;
    }

    while (node->left) {
        node = node->left;
    }

    {
        XBSTree_Node_PT parent = node->parent;
        XBSTree_Node_PT right = node->right;

        if (deep) {
            XMEM_FREE(node->key);
            XMEM_FREE(node->value);
        }
        else {
            if (key) {
                *key = node->key;
            }
            if (value) {
                *value = node->value;
            }
        }

        if (right) {
            right->parent = parent;
        }
        if (parent) {
            if (parent->left == node) {
                parent->left = right;
            }
            else {
                parent->right = right;
            }            
        }
        XMEM_FREE(node);

        while (parent) {
            parent->size -= 1;
            parent = parent->parent;
        }
    }
}

void xbstree_remove_min(XBSTree_PT tree) {
    xbstree_remove_min_impl(tree, (tree ? tree->root : NULL), false, NULL, NULL);
}

static
void xbstree_remove_max_impl(XBSTree_PT tree, XBSTree_Node_PT node, bool deep, void **key, void **value) {
    if (!node) {
        return;
    }

    while (node->right) {
        node = node->right;
    }

    {
        XBSTree_Node_PT parent = node->parent;
        XBSTree_Node_PT left = node->left;

        if (deep) {
            XMEM_FREE(node->key);
            XMEM_FREE(node->value);
        }
        else {
            if (key) {
                *key = node->key;
            }
            if (value) {
                *value = node->value;
            }
        }

        if (left) {
            left->parent = parent;
        }
        if (parent) {
            if (parent->left == node) {
                parent->left = left;
            }
            else {
                parent->right = left;
            }
        }
        XMEM_FREE(node);

        while (parent) {
            parent->size -= 1;
            parent = parent->parent;
        }
    }
}

void xbstree_remove_max(XBSTree_PT tree) {
    xbstree_remove_max_impl(tree, (tree ? tree->root : NULL), false, NULL, NULL);
}

void xbstree_remove_impl(XBSTree_PT tree, XBSTree_Node_PT node, void* key, bool deep, void** value) {
    node = xbstree_find_impl(tree, node, key);
    if (!node) {
        return;
    }

    if (!node->left || !node->right) {
        XBSTree_Node_PT parent = node->parent;
        XBSTree_Node_PT child = node->left ? node->left : node->right;

        if (deep) {
            XMEM_FREE(node->key);
            XMEM_FREE(node->value);
        }
        else if (value) {
            *value = node->value;
        }

        if (child) {
            child->parent = parent;
        }
        if (parent) {
            if (parent->left == node) {
                parent->left = child;
            }
            else {
                parent->right = child;
            }
        }
        XMEM_FREE(node);

        while (parent) {
            parent->size -= 1;
            parent = parent->parent;
        }
    }
    /* node has both left and right branch now, in order to make it more balance after delete,
     * choose the bigger size branch to do replace, but not always choose one side to do replace
     */
    else {
        void *save_key = NULL, *save_value = NULL;
        if (node->left->size <= node->right->size) {
            xbstree_remove_min_impl(tree, node->right, false, &save_key, &save_value);
        }
        else {
            xbstree_remove_max_impl(tree, node->left, false, &save_key, &save_value);
        }

        if (deep) {
            XMEM_FREE(node->key);
            XMEM_FREE(node->value);
        }
        else if (value) {
            *value = node->value;
        }

        node->key = save_key;
        node->value = save_value;

        /* size of each node has been updated in xbstree_remove_min_impl or xbstree_remove_max_impl */
    }
}

void xbstree_remove(XBSTree_PT tree, void *key) {
    xbstree_remove_impl(tree, (tree ? tree->root : NULL), key, false, NULL);
}

void xbstree_deep_remove(XBSTree_PT tree, void *key) {
    xbstree_remove_impl(tree, (tree ? tree->root : NULL), key, true, NULL);
}

static 
int xbstree_map_preorder_impl(XBSTree_PT tree, XBSTree_Node_PT node, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        if (apply(node->key, &(node->value), cl)) {
            count++;
        }

        count += xbstree_map_preorder_impl(tree, node->left, apply, cl);
        count += xbstree_map_preorder_impl(tree, node->right, apply, cl);

        return count;
    }
}

int xbstree_map_preorder(XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xbstree_map_preorder_impl(tree, (tree ? tree->root : NULL), apply, cl);
}

static 
int xbstree_map_inorder_impl(XBSTree_PT tree, XBSTree_Node_PT node, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        count += xbstree_map_inorder_impl(tree, node->left, apply, cl);

        if (apply(node->key, &(node->value), cl)) {
            count++;
        }

        count += xbstree_map_inorder_impl(tree, node->right, apply, cl);

        return count;
    }
}

int xbstree_map_inorder(XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xbstree_map_inorder_impl(tree, (tree ? tree->root : NULL), apply, cl);
}

static
int xbstree_map_postorder_impl(XBSTree_PT tree, XBSTree_Node_PT node, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        count += xbstree_map_postorder_impl(tree, node->left, apply, cl);
        count += xbstree_map_postorder_impl(tree, node->right, apply, cl);

        if (apply(node->key, &(node->value), cl)) {
            count++;
        }

        return count;
    }
}

int xbstree_map_postorder(XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xbstree_map_postorder_impl(tree, (tree ? tree->root : NULL), apply, cl);
}

int xbstree_map_levelorder(XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(apply);

    if (!tree || !apply) {
        return -1;
    }

    {
        int count = 0;

        XFifo_PT fifo = xfifo_new(0);
        if (!fifo) {
            return -1;
        }

        if (fifo) {
            if (!xfifo_push(fifo, (void*)tree->root)) {
                return -1;
            }

            while (!xfifo_is_empty(fifo)) {
                XBSTree_Node_PT node = (XBSTree_Node_PT)xfifo_pop(fifo);
                if (apply(node->key, &(node->value), cl)) {
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

static
int xbstree_map_min_to_max_impl(XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    int count = 0;

    XBSTree_Node_PT node = xbstree_min_impl(tree, tree ? tree->root : NULL);
    while (node) {
        if (apply(node->key, &node->value, cl)) {
            ++count;
        }

        node = xbstree_next_node(tree, node);
    }

    return count;
}

int xbstree_map_min_to_max(XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xbstree_map_min_to_max_impl(tree, apply, cl);
}

static
bool xbstree_map_min_to_max_break_if_impl(XBSTree_PT tree, bool break_true, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    XBSTree_Node_PT node = xbstree_min_impl(tree, tree ? tree->root : NULL);
    while (node) {
        bool ret = apply(node->key, &node->value, cl);
        if(ret && break_true) {
            return true;
        }
        if (!ret && !break_true) {
            return true;
        }

        node = xbstree_next_node(tree, node);
    }

    return false;
}

bool xbstree_map_min_to_max_break_if_true(XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return false;
    }

    return xbstree_map_min_to_max_break_if_impl(tree, true, apply, cl);
}

bool xbstree_map_min_to_max_break_if_false(XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return true;
    }

    return xbstree_map_min_to_max_break_if_impl(tree, false, apply, cl);
}

static
int xbstree_map_max_to_min_impl(XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    int count = 0;

    XBSTree_Node_PT node = xbstree_max_impl(tree, tree ? tree->root : NULL);
    while (node) {
        if (apply(node->key, &node->value, cl)) {
            ++count;
        }

        node = xbstree_prev_node(tree, node);
    }

    return count;
}

int xbstree_map_max_to_min(XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xbstree_map_max_to_min_impl(tree, apply, cl);
}

static
bool xbstree_map_max_to_min_break_if_impl(XBSTree_PT tree, bool break_true, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    XBSTree_Node_PT node = xbstree_max_impl(tree, tree ? tree->root : NULL);
    while (node) {
        bool ret = apply(node->key, &node->value, cl);
        if (ret && break_true) {
            return true;
        }
        if (!ret && !break_true) {
            return true;
        }

        node = xbstree_prev_node(tree, node);
    }

    return false;
}

bool xbstree_map_max_to_min_break_if_true(XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return false;
    }

    return xbstree_map_max_to_min_break_if_impl(tree, true, apply, cl);
}

bool xbstree_map_max_to_min_break_if_false(XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return true;
    }

    return xbstree_map_max_to_min_break_if_impl(tree, false, apply, cl);
}

int xbstree_scope_map_min_to_max(XBSTree_PT tree, void *low, void *high, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(low);
    xassert(high);
    xassert(apply);

    if (!tree || !(tree->root) || !low || !high || !apply) {
        return 0;
    }

    {
        int count = 0;

        if (0 < tree->cmp(low, high, tree->cl)) {
            void *tmp = low;
            low = high;
            high = tmp;
        }

        {
            XBSTree_Node_PT node = xbstree_find_impl(tree, tree->root, low);
            if (node) {
                /* the equal keys may be saved in pre nodes */
                XBSTree_Node_PT tnode = xbstree_prev_node(tree, node);
                while (tnode) {
                    if (tree->cmp(node->key, tnode->key, tree->cl) == 0) {
                        if (apply(tnode->key, tnode->value, cl)) {
                            ++count;
                        }
                    }
                    tnode = xbstree_prev_node(tree, tnode);
                }
            }

            while (node && (tree->cmp(node->key, high, tree->cl) <= 0)) {
                if (apply(node->key, node->value, cl)) {
                    ++count;
                }
                node = xbstree_next_node(tree, node);
            }
        }

        return count;
    }
}

bool xbstree_swap(XBSTree_PT tree1, XBSTree_PT tree2) {
    xassert(tree1);
    xassert(tree2);

    if (!tree1 || !tree2) {
        return false;
    }

    {
        {
            XBSTree_Node_PT root = tree1->root;
            tree1->root = tree2->root;
            tree2->root = root;
        }

        {
            int(*cmp) (void *key1, void *key2, void *cl) = tree1->cmp;
            tree1->cmp = tree2->cmp;
            tree2->cmp = cmp;
        }

        {
            void *cl = tree1->cl;
            tree1->cl = tree2->cl;
            tree2->cl = cl;
        }
    }

    return true;
}

int xbstree_size(XBSTree_PT tree) {
    return (tree ? (tree->root ? tree->root->size : 0) : 0);
}

bool xbstree_is_empty(XBSTree_PT tree) {
    return (tree ? (tree->root ? (tree->root->size == 0) : true) : true);
}

static
int xbstree_height_impl(XBSTree_Node_PT node) {
    if (!node) {
        return 0;
    }

    {
        int u = xbstree_height_impl(node->left);
        int v = xbstree_height_impl(node->right);

        return u < v ? (v + 1) : (u + 1);
    }
}

int xbstree_height(XBSTree_PT tree) {
    return xbstree_height_impl(tree ? tree->root : NULL);
}

bool xbstree_is_bstree(XBSTree_PT tree) {
    if (!tree) {
        return false;
    }

    if (!tree->root) {
        /* no root */
        return true;
    }

    {
        int count = 0;

        XBSTree_Node_PT node = xbstree_min_impl(tree, tree->root);
        while (node) {
            ++count;

            /* node->left->key <= node->key */
            if (node->left && (0 < tree->cmp(node->left->key, node->key, tree->cl))) {
                return false;
            }

            /* node->key <= node->right->key */
            if (node->right && (0 < tree->cmp(node->key, node->right->key, tree->cl))) {
                return false;
            }

            /* node->size = 1 + node->left->size + node->right->size */
            if (node->size != (1 + (node->left ? node->left->size : 0) + (node->right ? node->right->size : 0))) {
                return false;
            }

            node = xbstree_next_node(tree, node);
        }

        /* total node number is the same with the size saved in root */
        if (count != tree->root->size) {
            return false;
        }
    }

    return true;
}
