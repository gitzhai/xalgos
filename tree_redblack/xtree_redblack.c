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
#include "xtree_redblack_x.h"

static const bool xrbtree_color_red = false;
static const bool xrbtree_color_black = true;

static
bool xrbtree_node_color_red(XRBTree_Node_PT node) {
    return node ? (node->color == xrbtree_color_red) : false;
}

static
bool xrbtree_node_color_black(XRBTree_Node_PT node) {
    return node ? (node->color == xrbtree_color_black) : true;
}

static
XRBTree_Node_PT xrbtree_min_impl(XRBTree_PT tree, XRBTree_Node_PT node) {
    if (!node) {
        return NULL;
    }

    while (node->left) {
        node = node->left;
    }

    return node;
}

static
XRBTree_Node_PT xrbtree_max_impl(XRBTree_PT tree, XRBTree_Node_PT node) {
    if (!node) {
        return NULL;
    }

    while (node->right) {
        node = node->right;
    }

    return node;
}

static
XRBTree_Node_PT xrbtree_get_impl(XRBTree_PT tree, XRBTree_Node_PT node, void *key) {
    if (!tree || !key) {
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
XRBTree_Node_PT xrbtree_prev_node(XRBTree_PT tree, XRBTree_Node_PT node) {
    if (node->left) {
        return xrbtree_max_impl(tree, node->left);
    }

    {
        /* H's pre node is E :
        *      E
        *       \
        *         R
        *        / \
        *       H   S
        */
        XRBTree_Node_PT parent = node->parent;
        while (parent && (node == parent->left)) {
            node = parent;
            parent = parent->parent;
        }

        return parent;
    }
}

static
XRBTree_Node_PT xrbtree_next_node(XRBTree_PT tree, XRBTree_Node_PT node) {
    if (node->right) {
        return xrbtree_min_impl(tree, node->right);
    }

    {
        /* R's next node is S :
        *         S
        *       /   \
        *     E       T
        *      \
        *       R
        */
        XRBTree_Node_PT parent = node->parent;
        while (parent && (node == parent->right)) {
            node = parent;
            parent = parent->parent;
        }

        return parent;
    }
}

/* xrbtree_flip_color :
*         |                  |
*         N                 N(R)
*       /   \     -->      /   \
*     D(R)  X(R)          D     X
*
*    or
*
*         |                  |
*        N(R)                N
*       /   \     -->      /   \
*      D     X          D(R)     X(R)
*/
static
void xrbtree_flip_color(XRBTree_Node_PT node_N) {
    node_N->color = !node_N->color;
    if (node_N->left) {
        node_N->left->color = !node_N->left->color;
    }
    if (node_N->right) {
        node_N->right->color = !node_N->right->color;
    }
}

/* xrbtree_rotate_left :
*
*      |                   |
*      N                   X
*    /   \       -->     /   \
*   D    X(R)          N(R)    T
*        /  \          /  \
*       F    T        D    F
*/
static
XRBTree_Node_PT xrbtree_rotate_left(XRBTree_Node_PT node_N) {
    XRBTree_Node_PT x = node_N->right;

    node_N->right = x->left;
    if (x->left) {
        x->left->parent = node_N;
    }

    x->parent = node_N->parent;

    if (node_N->parent) {
        if (node_N == node_N->parent->left) {
            node_N->parent->left = x;
        }
        else {
            node_N->parent->right = x;
        }
    }

    x->left = node_N;
    node_N->parent = x;

    x->color = node_N->color;
    node_N->color = xrbtree_color_red;
    x->size = node_N->size;
    node_N->size = 1 + (node_N->left ? node_N->left->size : 0) + (node_N->right ? node_N->right->size : 0);

    return x;
}

/* xrbtree_rotate_right :
*
*         |                 |
*         N                 X
*       /   \     -->     /   \
*     X(R)   T           D    N(R)
*     /  \                    /  \
*    D    F                  F    T
*/
static
XRBTree_Node_PT xrbtree_rotate_right(XRBTree_Node_PT node_N) {
    XRBTree_Node_PT x = node_N->left;

    node_N->left = x->right;
    if (x->right) {
        x->right->parent = node_N;
    }

    x->parent = node_N->parent;

    if (node_N->parent) {
        if (node_N == node_N->parent->left) {
            node_N->parent->left = x;
        }
        else {
            node_N->parent->right = x;
        }
    }

    x->right = node_N;
    node_N->parent = x;

    x->color = node_N->color;
    node_N->color = xrbtree_color_red;
    x->size = node_N->size;
    node_N->size = 1 + (node_N->left ? node_N->left->size : 0) + (node_N->right ? node_N->right->size : 0);
     
    return x;
}

static
XRBTree_Node_PT xrbtree_balance(XRBTree_Node_PT node_N) {
    /* 1. node_N->left is red, node_N->right is black */
    if (xrbtree_node_color_red(node_N->left) && xrbtree_node_color_black(node_N->right)) {
        /* 1.1 node_N->left->right is red, left rotate at first, then right rotate as step 1.2 :
        *         |                  |
        *         N                  N
        *       /   \     -->      /   \
        *     D(R)   X           F(R)   X
        *       \                /
        *       F(R)           D(R)
        */
        if (xrbtree_node_color_black(node_N->left->left) && xrbtree_node_color_red(node_N->left->right)) {
            node_N->left = xrbtree_rotate_left(node_N->left);
        }

        /* 1.2 node_N->left->left is red, right rotate :
        *         |                  |
        *         N                  F
        *       /   \     -->      /   \
        *     F(R)   X           D(R)  N(R)
        *     /                          \
        *   D(R)                          X
        */
        if (xrbtree_node_color_red(node_N->left->left) && xrbtree_node_color_black(node_N->left->right)) {
            node_N = xrbtree_rotate_right(node_N);
        }
    }

    /* 2. node_N->left is black, node_N->right is red */
    if (xrbtree_node_color_black(node_N->left) && xrbtree_node_color_red(node_N->right)) {
        /* 2.1 node_N->right->left is red, right rotate at first, then left rotate as step 2.2 :
        *         |                  |
        *         N                  N
        *       /   \     -->      /   \
        *      D   X(R)           D    U(R)
        *           /                    \
        *         U(R)                   X(R)
        */
        if (xrbtree_node_color_red(node_N->right->left) && xrbtree_node_color_black(node_N->right->right)) {
            node_N->right = xrbtree_rotate_right(node_N->right);
        }

        /* 2.2 node_N->right->right is red, left rotate :
        *         |                  |
        *         N                  U
        *       /   \     -->      /   \
        *      D    U(R)         N(R)  X(R)
        *             \          /
        *             X(R)      D
        */
        if (xrbtree_node_color_black(node_N->right->left) && xrbtree_node_color_red(node_N->right->right)) {
            node_N = xrbtree_rotate_left(node_N);
        }
    }

    /* 3. node_N->left and node_N->right are all red, make the node_N red to pop up it to its parent layer nodes,
    *         |                  |
    *         N                 N(R)
    *       /   \     -->      /   \
    *     D(R)  X(R)          D     X
    */
    if (xrbtree_node_color_red(node_N->left) && xrbtree_node_color_red(node_N->right)) {
        xrbtree_flip_color(node_N);
    }

    node_N->size = 1 + (node_N->left ? node_N->left->size : 0) + (node_N->right ? node_N->right->size : 0);

    return node_N;
}

static
XRBTree_Node_PT xrbtree_new_node(void *key, void *value, bool color) {
    XRBTree_Node_PT node = XMEM_CALLOC(1, sizeof(*node));
    if (!node) {
        return NULL;
    }

    node->key = key;
    node->value = value;

    node->size = 1;
    node->color = color;

    //node->parent = NULL;
    //node->left = NULL;
    //node->right = NULL;

    return node;
}

XRBTree_PT xrbtree_new(int (*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(cmp);
    
    if (!cmp) {
        return NULL;
    }

    {
        XRBTree_PT tree = XMEM_CALLOC(1, sizeof(*tree));
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
XRBTree_Node_PT xrbtree_copy_node(XRBTree_Node_PT node, XRBTree_Node_PT nparent, bool *false_found, void *cl) {
    XRBTree_Node_PT nnode = XMEM_CALLOC(1, sizeof(*nnode));
    if (!nnode) {
        *false_found = true;
        return NULL;
    }

    nnode->key = node->key;
    nnode->value = node->value;

    nnode->size = node->size;
    nnode->color = node->color;

    nnode->parent = nparent;
    //nnode->left = NULL;
    //nnode->right = NULL;

    return nnode;
}

static
void xrbtree_copy_break_if_false_impl(XRBTree_PT tree, XRBTree_Node_PT node, XRBTree_PT ntree, XRBTree_Node_PT nparent, bool root, bool left, bool *false_found, XRBTree_Node_PT(*apply)(XRBTree_Node_PT node, XRBTree_Node_PT nparent, bool *false_found, void *cl), void *cl) {
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

    xrbtree_copy_break_if_false_impl(tree, node->left, ntree, nparent, false, true, false_found, apply, cl);
    if (*false_found) {
        return;
    }

    xrbtree_copy_break_if_false_impl(tree, node->right, ntree, nparent, false, false, false_found, apply, cl);
}

XRBTree_PT xrbtree_copy(XRBTree_PT tree) {
    xassert(tree);

    if (!tree) {
        return NULL;
    }

    {
        bool false_found = false;

        XRBTree_PT ntree = xrbtree_new(tree->cmp, tree->cl);
        if (!ntree) {
            return NULL;
        }

        xrbtree_copy_break_if_false_impl(tree, tree->root, ntree, NULL, true, false, &false_found, xrbtree_copy_node, NULL);
        if (false_found) {
            xrbtree_free(&ntree);
            return NULL;
        }

        xassert(xrbtree_size(tree) == xrbtree_size(ntree));

        return ntree;
    }
}

static
XRBTree_Node_PT xrbtree_deep_copy_node(XRBTree_Node_PT node, XRBTree_Node_PT nparent, bool *false_found, void *cl) {
    XRBTree_3Paras_PT paras = (XRBTree_3Paras_PT)cl;

    XRBTree_Node_PT nnode = XMEM_CALLOC(1, sizeof(*nnode));
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
    nnode->color = node->color;

    nnode->parent = nparent;
    //nnode->left = NULL;
    //nnode->right = NULL;

    return nnode;
}

XRBTree_PT xrbtree_deep_copy(XRBTree_PT tree, int key_size, int value_size) {
    xassert(tree);
    xassert(0 < key_size);
    xassert(0 <= value_size);

    if (!tree || (key_size <= 0) || (value_size < 0)) {
        return NULL;
    }

    {
        XRBTree_PT ntree = xrbtree_new(tree->cmp, tree->cl);
        if (!ntree) {
            return NULL;
        }

        {
            XRBTree_3Paras_T paras = { ntree, (void*)&key_size, (void*)&value_size, NULL };

            bool false_found = false;
            xrbtree_copy_break_if_false_impl(tree, tree->root, ntree, NULL, true, false, &false_found, xrbtree_deep_copy_node, (void*)&paras);
            if (false_found) {
                xrbtree_free(&ntree);
                return NULL;
            }
        }

        xassert(xrbtree_size(tree) == xrbtree_size(ntree));

        return ntree;
    }
}

static
XRBTree_Node_PT xrbtree_put_repeat_impl(XRBTree_PT tree, XRBTree_Node_PT parent, XRBTree_Node_PT node, XRBTree_Node_PT new_node, void *key, bool unique) {
    /* reach the leaf node, return the new created new_node */
    if (!node) {
        new_node->parent = parent;
        return new_node;
    }

    {
        int ret = tree->cmp(key, node->key, tree->cl);
        if (unique && (ret == 0)) {
            XMEM_FREE(new_node);
            return node;
        }

        /* find the branch to insert the new node */
        if (ret < 0) {
            node->left = xrbtree_put_repeat_impl(tree, node, node->left, new_node, key, unique);
        }
        else {
            node->right = xrbtree_put_repeat_impl(tree, node, node->right, new_node, key, unique);
        }

        /* make the tree balance again */
        return xrbtree_balance(node);
    }
}

bool xrbtree_put_repeat(XRBTree_PT tree, void *key, void *value) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return false;
    }

    {
        /* always insert the new node with red color */
        XRBTree_Node_PT nnode = xrbtree_new_node(key, value, xrbtree_color_red);
        if (!nnode) {
            return false;
        }

        tree->root = xrbtree_put_repeat_impl(tree, NULL, tree->root, nnode, key, false);
        tree->root->color = xrbtree_color_black;
    }

    return true;
}

bool xrbtree_put_unique(XRBTree_PT tree, void *key, void *value) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return false;
    }

    {
        XRBTree_Node_PT nnode = xrbtree_new_node(key, value, xrbtree_color_red);
        if (!nnode) {
            return false;
        }

        tree->root = xrbtree_put_repeat_impl(tree, NULL, tree->root, nnode, key, true);
        tree->root->color = xrbtree_color_black;
    }

    return true;
}

static 
XRBTree_Node_PT xrbtree_put_replace_impl(XRBTree_PT tree, XRBTree_Node_PT parent, XRBTree_Node_PT node, XRBTree_Node_PT new_node, void *key, void *value, void **old_value, bool deep) {
    /* reach the leaf node, return the new created new_node */
    if (!node) {
        new_node->parent = parent;
        return new_node;
    }

    {
        int ret = tree->cmp(key, node->key, tree->cl);
        /* find the equal key, replace the value */
        if (ret == 0) {
            if (deep) {
                XMEM_FREE(node->value);
            }
            else if (old_value) {
                *old_value = node->value;
            }

            node->value = value;
            XMEM_FREE(new_node);
            return node;
        }
        /* find the right branch to insert the new node */
        else if (ret < 0) {
            node->left = xrbtree_put_replace_impl(tree, node, node->left, new_node, key, value, old_value, deep);
        }
        else {
            node->right = xrbtree_put_replace_impl(tree, node, node->right, new_node, key, value, old_value, deep);
        }

        /* make the tree balance again */
        return xrbtree_balance(node);
    }
}

bool xrbtree_put_replace(XRBTree_PT tree, void *key, void *value, void **old_value) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return false;
    }

    {
        XRBTree_Node_PT nnode = xrbtree_new_node(key, value, xrbtree_color_red);
        if (!nnode) {
            return false;
        }

        tree->root = xrbtree_put_replace_impl(tree, NULL, tree->root, nnode, key, value, old_value, false);
        tree->root->color = xrbtree_color_black;
    }

    return true;
}

bool xrbtree_put_deep_replace(XRBTree_PT tree, void *key, void *value) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return false;
    }

    {
        XRBTree_Node_PT nnode = xrbtree_new_node(key, value, xrbtree_color_red);
        if (!nnode) {
            return false;
        }

        tree->root = xrbtree_put_replace_impl(tree, NULL, tree->root, nnode, key, value, NULL, true);
        tree->root->color = xrbtree_color_black;
    }

    return true;
}

void* xrbtree_min(XRBTree_PT tree) {
    XRBTree_Node_PT node = xrbtree_min_impl(tree, (tree ? tree->root : NULL));
    return node ? node->key : NULL;
}

void* xrbtree_max(XRBTree_PT tree) {
    XRBTree_Node_PT node = xrbtree_max_impl(tree, (tree ? tree->root : NULL));
    return node ? node->key : NULL;
}

static
XRBTree_Node_PT xrbtree_floor_impl(XRBTree_PT tree, XRBTree_Node_PT node, void *key) {
    if (!key) {
        return NULL;
    }

    {
        XRBTree_Node_PT result = NULL;

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
void* xrbtree_floor(XRBTree_PT tree, void *key) {
    XRBTree_Node_PT floor = xrbtree_floor_impl(tree, (tree ? tree->root : NULL), key);
    return floor ? floor->key : NULL;
}

static
XRBTree_Node_PT xrbtree_ceiling_impl(XRBTree_PT tree, XRBTree_Node_PT node, void *key) {
    if (!key) {
        return NULL;
    }

    {
        XRBTree_Node_PT result = NULL;

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
void* xrbtree_ceiling(XRBTree_PT tree, void *key) {
    XRBTree_Node_PT ceiling = xrbtree_ceiling_impl(tree, (tree ? tree->root : NULL), key);
    return ceiling ? ceiling->key : NULL;
}

static
XRBTree_Node_PT xrbtree_select_impl(XRBTree_PT tree, XRBTree_Node_PT node, int k) {
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
void* xrbtree_select(XRBTree_PT tree, int k) {
    xassert(tree);
    xassert(0 <= k);
    xassert(k < xrbtree_size(tree));

    if (!tree || (k < 0) || (xrbtree_size(tree) <= k)) {
        return NULL;
    }

    return xrbtree_select_impl(tree, tree->root, k)->key;
}

static 
void xrbtree_index_replace_impl(XRBTree_PT tree, int k, void *value, void **old_value, bool deep) {
    XRBTree_Node_PT node = xrbtree_select_impl(tree, tree->root, k);
    xassert(node);

    if (deep) {
        XMEM_FREE(node->value);
    }
    else if (old_value) {
        *old_value = node->value;
    }
    node->value = value;

    return;
}

bool xrbtree_index_replace(XRBTree_PT tree, int k, void *value, void **old_value) {
    xassert(tree);
    xassert(0 <= k);
    xassert(k < xrbtree_size(tree));

    if (!tree || (k < 0) || (xrbtree_size(tree) <= k)) {
        return false;
    }

    xrbtree_index_replace_impl(tree, k, value, old_value, false);

    return true;
}

bool xrbtree_index_deep_replace(XRBTree_PT tree, int k, void *value) {
    xassert(tree);
    xassert(0 <= k);
    xassert(k < xrbtree_size(tree));

    if (!tree || (k < 0) || (xrbtree_size(tree) <= k)) {
        return false;
    }

    xrbtree_index_replace_impl(tree, k, value, NULL, true);

    return true;
}

static
int xrbtree_rank_impl(XRBTree_PT tree, XRBTree_Node_PT node, void *key) {
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
                k += node->size - (node->right ? node->right->size : 0);
                node = node->right;
            }
        }

        return -1;
    }
}

/* element number of the key : 0, 1, 2, ... */
int xrbtree_rank(XRBTree_PT tree, void *key) {
    return xrbtree_rank_impl(tree, (tree ? tree->root : NULL), key);
}

void* xrbtree_get(XRBTree_PT tree, void *key) {
    XRBTree_Node_PT node = xrbtree_get_impl(tree, (tree ? tree->root : NULL), key);
    return node ? node->value : NULL;
}

XSList_PT xrbtree_get_all(XRBTree_PT tree, void *key) {
    XSList_PT list = xslist_new();
    if (!list) {
        return NULL;
    }

    {
        XRBTree_Node_PT node = xrbtree_get_impl(tree, tree->root, key);
        if (node) {
            /* the equal keys may be saved in pre nodes */
            XRBTree_Node_PT tnode = xrbtree_prev_node(tree, node);
            while (tnode) {
                if (tree->cmp(node->key, tnode->key, tree->cl) == 0) {
                    if (!xslist_push_back_repeat(list, tnode->value)) {
                        xslist_free(&list);
                        return NULL;
                    }
                }
                tnode = xrbtree_prev_node(tree, tnode);
            }
        }

        while (node && (tree->cmp(node->key, key, tree->cl) <= 0)) {
            if (!xslist_push_back_repeat(list, node->value)) {
                xslist_free(&list);
                return NULL;
            }
            node = xrbtree_next_node(tree, node);
        }
    }

    return list;
}

bool xrbtree_find(XRBTree_PT tree, void *key) {
    return xrbtree_get_impl(tree, (tree ? tree->root : NULL), key) ? true : false;
}

bool xrbtree_find_replace(XRBTree_PT tree, void *key, void *value, void **old_value) {
    XRBTree_Node_PT node = xrbtree_get_impl(tree, (tree ? tree->root : NULL), key);
    if (node) {
        if (old_value) {
            *old_value = node->value;
        }
        node->value = value;
        return true;
    }

    return false;
}

bool xrbtree_find_deep_replace(XRBTree_PT tree, void *key, void *value) {
    XRBTree_Node_PT node = xrbtree_get_impl(tree, (tree ? tree->root : NULL), key);
    if (node) {
        XMEM_FREE(node->value);
        node->value = value;
        return true;
    }

    return false;
}

static 
void xrbtree_switch_key_value(XRBTree_Node_PT node1, XRBTree_Node_PT node2) {
    void *key = node1->key;
    void *value = node1->value;

    node1->key = node2->key;
    node1->value = node2->value;

    node2->key = key;
    node2->value = value;
}

void xrbtree_replace_key(XRBTree_PT tree, void *old_key, void *new_key) {
    XRBTree_Node_PT node = xrbtree_get_impl(tree, (tree ? tree->root : NULL), old_key);
    if (node) {
        node->key = new_key;

        {
            bool balanced = false;

            XRBTree_Node_PT prev = xrbtree_prev_node(tree, node);
            while (prev && (tree->cmp(node->key, prev->key, tree->cl) < 0)) {
                xrbtree_switch_key_value(prev, node);
                balanced = true;

                node = prev;
                prev = xrbtree_prev_node(tree, node);
            }

            if (!balanced) {
                XRBTree_Node_PT next = xrbtree_next_node(tree, node);
                while (next && (tree->cmp(next->key, node->key, tree->cl) < 0)) {
                    xrbtree_switch_key_value(next, node);

                    node = next;
                    next = xrbtree_next_node(tree, node);
                }
            }
        }
    }
}

XSList_PT xrbtree_keys(XRBTree_PT tree, void *low, void *high) {
    xassert(tree);
    xassert(low);
    xassert(high);

    if (!tree || !low || !high) {
        return NULL;
    }

    {
        XSList_PT list = xslist_new();
        if (!list) {
            return NULL;
        }

        if (0 < tree->cmp(low, high, tree->cl)) {
            void *tmp = low;
            low = high;
            high = tmp;
        }

        {
            XRBTree_Node_PT node = xrbtree_get_impl(tree, tree->root, low);
            if (node) {
                /* the equal keys may be saved in pre nodes */
                XRBTree_Node_PT tnode = xrbtree_prev_node(tree, node);
                while (tnode) {
                    if (tree->cmp(node->key, tnode->key, tree->cl) == 0) {
                        if (!xslist_push_back_repeat(list, tnode->key)) {
                            xslist_free(&list);
                            return NULL;
                        }
                    }
                    tnode = xrbtree_prev_node(tree, tnode);
                }
            }

            while (node && (tree->cmp(node->key, high, tree->cl) <= 0)) {
                if (!xslist_push_back_repeat(list, node->key)) {
                    xslist_free(&list);
                    return NULL;
                }
                node = xrbtree_next_node(tree, node);
            }
        }

        return list;
    }
}

static
void xrbtree_free_impl(XRBTree_PT tree, XRBTree_Node_PT node, bool deep, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return;
    }

    xrbtree_free_impl(tree, node->left, deep, apply, cl);
    xrbtree_free_impl(tree, node->right, deep, apply, cl);

    if (deep) {
        XMEM_FREE(node->key);
        XMEM_FREE(node->value);
    }
    else if (apply) {
        apply(node->key, &node->value, cl);
    }
    XMEM_FREE(node);
}

void xrbtree_clear(XRBTree_PT tree) {
    xrbtree_free_impl(tree, (tree ? tree->root : NULL), false, NULL, NULL);
    if (tree) {
        tree->root = NULL;
    }
}

void xrbtree_clear_apply(XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xrbtree_free_impl(tree, (tree ? tree->root : NULL), false, apply, cl);
    if (tree) {
        tree->root = NULL;
    }
}

void xrbtree_deep_clear(XRBTree_PT tree) {
    xrbtree_free_impl(tree, (tree ? tree->root : NULL), true, NULL, NULL);
    if (tree) {
        tree->root = NULL;
    }
}

void xrbtree_free(XRBTree_PT *ptree) {
    if (!ptree || !*ptree) {
        return;
    }

    xrbtree_free_impl(*ptree, (*ptree)->root, false, NULL, NULL);
    XMEM_FREE(*ptree);
}

void xrbtree_free_apply(XRBTree_PT *ptree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!ptree || !*ptree) {
        return;
    }

    xrbtree_free_impl(*ptree, (*ptree)->root, false, apply, cl);
    XMEM_FREE(*ptree);
}

void xrbtree_deep_free(XRBTree_PT *ptree) {
    if (!ptree || !*ptree) {
        return;
    }

    xrbtree_free_impl(*ptree, (*ptree)->root, true, NULL, NULL);
    XMEM_FREE(*ptree);
}

/* move one key from node_N's right branch to left branch :
*        |                    |
*       N(R)                 Q(R)
*     /     \              /     \
*    E      Q T     -->   E N     T
*   / \    / | \         / | \   / \
*  D   F  P  S  U       D  F  P S   U
*/
static
XRBTree_Node_PT xrbtree_move_red_left(XRBTree_Node_PT node_N) {
    if (node_N->right && (xrbtree_node_color_red(node_N->right->left) || xrbtree_node_color_red(node_N->right->right))) {
        /*
        *          |                  |                     |                  |
        *         N(R)                N                    N(R)                N
        *       /     \    -->      /   \         or     /     \    -->      /   \
        *      E       T          E(R)  T(R)            E       Q          E(R)  Q(R)
        *     / \     / \         / \    / \           / \     / \         / \    / \
        *    D   F  Q(R) U       D   F Q(R) U         D   F   P  T(R)     D   F  P  T(R)
        *           / \                / \                       / \                / \
        *          P   S              P   S                     S   U              S   U
        */
        xassert(xrbtree_node_color_red(node_N) && xrbtree_node_color_black(node_N->left) && xrbtree_node_color_black(node_N->right));
        xrbtree_flip_color(node_N);

        if (xrbtree_node_color_red(node_N->right->left)) {
            /*
            *          |                  |
            *          N                  N
            *       /    \             /     \
            *    E(R)    T(R)   ->   E(R)    Q(R)
            *    /  \    / \         / \    /   \
            *   D    F Q(R) U       D   F  P     T(R)
            *          / \                       / \
            *         P   S                     S   U
            */
            node_N->right = xrbtree_rotate_right(node_N->right);
        }

        if (xrbtree_node_color_red(node_N->right->right)) {
            /*
            *        |                         |
            *        N                         Q
            *     /     \                   /     \
            *   E(R)    Q(R)        ->     N(R)   T(R)
            *   / \    /   \              /  \    /  \
            *  D   F  P     T(R)        E(R)  P  S    U
            *               / \         /  \
            *              S   U       D    F
            */
            node_N = xrbtree_rotate_left(node_N);
        }

        /*
        *          |                         |
        *          Q                        Q(R)
        *       /     \                    /    \
        *     N(R)    T(R)       ->       N      T
        *     /  \    /  \              /  \    /  \
        *   E(R)  P  S   U            E(R)  P  S    U
        *   /  \                      /  \
        *  D    F                    D    F
        */
        xassert(xrbtree_node_color_black(node_N) && xrbtree_node_color_red(node_N->left) && xrbtree_node_color_red(node_N->right));
        xrbtree_flip_color(node_N);
    }

    return node_N;
}

/* move one key from node_N's left branch to right branch :
*         |                   |
*        N(R)                K(R)
*      /      \      -->    /    \
*    G K       T           G     N T
*   / | \     / \         / \   / | \
*  D  H  M   Q   U       D   H M  Q  U
*/
static
XRBTree_Node_PT xrbtree_move_red_right(XRBTree_Node_PT node_N) {
    if (node_N->left && (xrbtree_node_color_red(node_N->left->right) || xrbtree_node_color_red(node_N->left->left))) {
        /*
        *          |                   |                         |                   |
        *         N(R)                 N                        N(R)                 N
        *       /     \     -->     /     \        or         /     \     -->     /     \
        *      G       T          G(R)    T(R)               K       T          K(R)    T(R)
        *     /  \    /  \        /  \     / \              /  \    /  \        /  \     / \
        *    D  K(R) Q    U      D  K(R)  Q   U           G(R)  M  Q    U     G(R)  M   Q   U
        *        / \                 / \                  / \                 / \
        *       H   M               H   M                D   H               D   H
        */
        xassert(xrbtree_node_color_red(node_N) && xrbtree_node_color_black(node_N->left) && xrbtree_node_color_black(node_N->right));
        xrbtree_flip_color(node_N);

        if (xrbtree_node_color_red(node_N->left->right)) {
            /*
            *          |                   |
            *          N                   N
            *       /     \     -->     /     \
            *     G(R)    T(R)        K(R)    T(R)
            *     /  \    /  \        /  \     / \
            *    D  K(R) Q    U     G(R)  M   Q   U
            *        / \            / \
            *       H   M          D   H
            */
            node_N->left = xrbtree_rotate_left(node_N->left);
        }

        if (xrbtree_node_color_red(node_N->left->left)) {
            /*
            *          |                   |
            *          N                   K
            *       /     \     -->     /     \
            *     K(R)   T(R)         G(R)   N(R)
            *     /  \    / \         / \     / \
            *   G(R)  M  Q   U       D   H   M  T(R)
            *   / \                             / \
            *  D   H                           Q   U
            */
            node_N = xrbtree_rotate_right(node_N);
        }

        /*
         *         |                   |
         *         G                  G(R)
         *      /     \     -->     /     \
         *    G(R)   N(R)          G       N
         *    /  \    / \         / \     / \
         *   D    H  M  T(R)     D   H   M  T(R)
         *             / \                  / \
         *            Q   U                Q   U
         */
        xassert(xrbtree_node_color_black(node_N) && xrbtree_node_color_red(node_N->left) && xrbtree_node_color_red(node_N->right));
        xrbtree_flip_color(node_N);
    }

    return node_N;
}

static 
XRBTree_Node_PT xrbtree_remove_min_impl(XRBTree_PT tree, XRBTree_Node_PT node_N, void **old_key, void **old_value, bool deep) {
    /* if node has no left branch, node is the mimimum one */
    if (!node_N->left) {
        XRBTree_Node_PT right = node_N->right;
        if (right) {
            right->parent = node_N->parent;
            /* keep the black node numbers not changed */
            if (xrbtree_node_color_black(node_N)) {
                right->color = node_N->color;
            }
        }

        if (deep) {
            XMEM_FREE(node_N->key);
            XMEM_FREE(node_N->value);
        }
        else {
            if (old_key) {
                *old_key = node_N->key;
            }
            if (old_value) {
                *old_value = node_N->value;
            }
        }
        XMEM_FREE(node_N);

        return right;
    }

    /* node->left has no 2 or 3 keys
    *          |
    *         N(R)
    *       /      \
    *     E(B)      T
    */
    if (xrbtree_node_color_black(node_N->left) && xrbtree_node_color_black(node_N->left->left) && xrbtree_node_color_black(node_N->left->right)) {
        /* node->right is red
        *          |                       |
        *         N(R)                    T(R)
        *       /      \       ->       /      \
        *     E(B)     T(R)            N(R)     U
        *     /        /  \           /   \
        *   D(B)      S    U        E(B)   S
        *                           /
        *                         D(B)
        */
        if (xrbtree_node_color_red(node_N->right)) {
            node_N = xrbtree_rotate_left(node_N);
        }
        else {
            /* node->right has 2 or 3 keys, move one to left
            *           |                         |
            *          N(R)                      Q(R)
            *       /       \                   /    \
            *     E(B)       T        -->      N      T
            *     / \      /   \              / \    / \
            *   D(B) F   Q(R)   U           E(R) P  S   U
            *           / \                 / \
            *          P   S             D(B)  F
            */
            if (node_N->right && (xrbtree_node_color_red(node_N->right->left) || xrbtree_node_color_red(node_N->right->right))) {
                node_N = xrbtree_move_red_left(node_N);
            }
            else {
                /* node->right is black
                *           |                         |
                *          N(R)                       N
                *       /       \         -->       /    \
                *     E(B)       T                E(R)   T(R)
                *     /                           /
                *   D(B)                        D(B)
                */
                xassert(xrbtree_node_color_red(node_N) && xrbtree_node_color_black(node_N->left) && xrbtree_node_color_black(node_N->right));
                xrbtree_flip_color(node_N);
            }
        }
    }

    /* node->left has 2 or 3 keys now */
    node_N->left = xrbtree_remove_min_impl(tree, node_N->left, old_key, old_value, deep);

    return xrbtree_balance(node_N);
}

void xrbtree_remove_min(XRBTree_PT tree) {
    xassert(tree);

    if (!tree || !tree->root) {
        return ;
    }

    if (xrbtree_node_color_black(tree->root->left) && xrbtree_node_color_black(tree->root->right)) {
        tree->root->color = xrbtree_color_red;
    }

    tree->root = xrbtree_remove_min_impl(tree, tree->root, NULL, NULL, false);
    if (tree->root) {
        tree->root->color = xrbtree_color_black;
    }
}

void xrbtree_remove_save_min(XRBTree_PT tree, void **key, void **value) {
    xassert(tree);

    if (!tree || !tree->root) {
        return;
    }

    if (xrbtree_node_color_black(tree->root->left) && xrbtree_node_color_black(tree->root->right)) {
        tree->root->color = xrbtree_color_red;
    }

    tree->root = xrbtree_remove_min_impl(tree, tree->root, key, value, false);
    if (tree->root) {
        tree->root->color = xrbtree_color_black;
    }
}

void xrbtree_deep_remove_min(XRBTree_PT tree) {
    xassert(tree);

    if (!tree || !tree->root) {
        return;
    }

    if (xrbtree_node_color_black(tree->root->left) && xrbtree_node_color_black(tree->root->right)) {
        tree->root->color = xrbtree_color_red;
    }

    tree->root = xrbtree_remove_min_impl(tree, tree->root, NULL, NULL, true);
    if (tree->root) {
        tree->root->color = xrbtree_color_black;
    }
}

static
XRBTree_Node_PT xrbtree_remove_max_impl(XRBTree_PT tree, XRBTree_Node_PT node_N, void **old_key, void **old_value, bool deep) {
    /* if node has no right branch, node is the maximum one */
    if (!node_N->right) {
        XRBTree_Node_PT left = node_N->left;
        if (left) {
            left->parent = node_N->parent;
            /* keep the black node numbers not changed */
            if (xrbtree_node_color_black(node_N)) {
                left->color = node_N->color;
            }
        }

        if (deep) {
            XMEM_FREE(node_N->key);
            XMEM_FREE(node_N->value);
        }
        else {
            if (old_key) {
                *old_key = node_N->key;
            }
            if (old_value) {
                *old_value = node_N->value;
            }
        }
        XMEM_FREE(node_N);

        return left;
    }

    /* node->right has no 2 or 3 keys
    *          |
    *         N(R)
    *       /      \
    *      C       T(B)
    */
    if (xrbtree_node_color_black(node_N->right) && xrbtree_node_color_black(node_N->right->left) && xrbtree_node_color_black(node_N->right->right)) {
        /* node->left is red
        *          |                       |
        *         N(R)                    C(R)
        *       /      \       ->       /      \
        *     C(R)     T(B)            A       N(R)
        *     /  \       \                    /    \
        *    A    E      U(B)                E     T(B)
        *                                            \
        *                                            U(B)
        */
        if (xrbtree_node_color_red(node_N->left)) {
            node_N = xrbtree_rotate_right(node_N);
        }
        else {
            /* node->left has 2 or 3 keys, move one to right
            *           |                        |
            *          N(R)                     E(R)
            *       /       \                  /    \
            *     C(B)      T(B)     -->     C(B)    N
            *     / \         \              / \    /  \
            *    A  E(R)      U(B)          A   D  F   T(R)
            *       / \                                  \
            *      D   F                                 U(B)
            */
            if (node_N->left && (xrbtree_node_color_red(node_N->left->left) || xrbtree_node_color_red(node_N->left->right))) {
                node_N = xrbtree_move_red_right(node_N);
            }
            else {
                /* node->left is black
                *
                *           |                         |
                *          N(R)                       N
                *       /       \         -->       /    \
                *     C(B)      T(B)              C(R)   T(R)
                *                 \                        \
                *                 U(B)                     U(B)
                */
                xassert(xrbtree_node_color_red(node_N) && xrbtree_node_color_black(node_N->left) && xrbtree_node_color_black(node_N->right));
                xrbtree_flip_color(node_N);
            }
        }
    }

    /* node->right has 2 or 3 keys now */
    node_N->right = xrbtree_remove_max_impl(tree, node_N->right, old_key, old_value, deep);

    return xrbtree_balance(node_N);
}

void xrbtree_remove_max(XRBTree_PT tree) {
    xassert(tree);

    if (!tree || !tree->root) {
        return;
    }

    if (xrbtree_node_color_black(tree->root->left) && xrbtree_node_color_black(tree->root->right)) {
        tree->root->color = xrbtree_color_red;
    }

    tree->root = xrbtree_remove_max_impl(tree, tree->root, NULL, NULL, false);
    if (tree->root) {
        tree->root->color = xrbtree_color_black;
    }
}

void xrbtree_remove_save_max(XRBTree_PT tree, void **key, void **value) {
    xassert(tree);

    if (!tree || !tree->root) {
        return;
    }

    if (xrbtree_node_color_black(tree->root->left) && xrbtree_node_color_black(tree->root->right)) {
        tree->root->color = xrbtree_color_red;
    }

    tree->root = xrbtree_remove_max_impl(tree, tree->root, key, value, false);
    if (tree->root) {
        tree->root->color = xrbtree_color_black;
    }
}

void xrbtree_deep_remove_max(XRBTree_PT tree) {
    xassert(tree);

    if (!tree || !tree->root) {
        return;
    }

    if (xrbtree_node_color_black(tree->root->left) && xrbtree_node_color_black(tree->root->right)) {
        tree->root->color = xrbtree_color_red;
    }

    tree->root = xrbtree_remove_max_impl(tree, tree->root, NULL, NULL, true);
    if (tree->root) {
        tree->root->color = xrbtree_color_black;
    }
}

static
XRBTree_Node_PT xrbtree_remove_impl(XRBTree_PT tree, XRBTree_Node_PT node_N, void *key, void **old_value, bool deep) {
    /* can't find the node to remove */
    if (!node_N) {
        return NULL;
    }

    {
        int ret = tree->cmp(key, node_N->key, tree->cl);
        if (ret == 0) {
            /* no left or right branch */
            if (!node_N->left || !node_N->right) {
                XRBTree_Node_PT ret_node = node_N->left ? node_N->left : node_N->right;
                if (ret_node) {
                    ret_node->parent = node_N->parent;
                    /* keep the black node numbers not changed */
                    if (xrbtree_node_color_black(node_N)) {
                        ret_node->color = node_N->color;
                    }
                }

                if (deep) {
                    XMEM_FREE(node_N->key);
                    XMEM_FREE(node_N->value);
                }
                else if (old_value) {
                    *old_value = node_N->value;
                }

                XMEM_FREE(node_N);
                return ret_node;
            }

            /* 3. both branches exist, use the min key of node_N->right branch to replace node_N :
            *          |
            *         N(R)
            *        /   \
            *       L     P
            */

            {
                /* node_N->right has no 2 or 3 keys
                *          |
                *         N(R)
                *       /      \
                *      C       T(B)
                */
                if (xrbtree_node_color_black(node_N->right) && xrbtree_node_color_black(node_N->right->left) && xrbtree_node_color_black(node_N->right->right)) {
                    /* node->left is red
                    *          |                       |
                    *         N(R)                    C(R)
                    *       /      \       ->       /      \
                    *     C(R)     T(B)            A       N(R)
                    *     /  \                            /    \
                    *    A    E                          E     T(B)
                    */
                    if (xrbtree_node_color_red(node_N->left)) {
                        node_N = xrbtree_rotate_right(node_N);
                    }
                    else {
                        /* node_N->left has 2 or 3 keys, move one to right
                        *           |                        |
                        *          N(R)                     E(R)
                        *       /       \                  /    \
                        *     C(B)      T(B)     -->     C(B)    N
                        *     / \                        / \    /  \
                        *    A  E(R)                    A   D  F   T(R)
                        *       / \
                        *      D   F
                        */
                        if (node_N->left && (xrbtree_node_color_red(node_N->left->left) || xrbtree_node_color_red(node_N->left->right))) {
                            node_N = xrbtree_move_red_right(node_N);
                        }
                        else {
                            /* node_N->left is black
                            *
                            *           |                         |
                            *          N(R)                       N
                            *       /       \         -->       /    \
                            *     C(B)      T(B)              C(R)   T(R)
                            */
                            xassert(xrbtree_node_color_red(node_N) && xrbtree_node_color_black(node_N->left) && xrbtree_node_color_black(node_N->right));
                            xrbtree_flip_color(node_N);
                        }
                    }
                }

                /* node->right has 2 or 3 keys now */

                if (0 == tree->cmp(key, node_N->key, tree->cl)) {
                    /* delete the min key of node_N->right */
                    void *min_key = NULL, *min_value = NULL;
                    node_N->right = xrbtree_remove_min_impl(tree, node_N->right, &min_key, &min_value, false);

                    if (deep) {
                        XMEM_FREE(node_N->key);
                        XMEM_FREE(node_N->value);
                    }
                    else if (old_value) {
                        *old_value = node_N->value;
                    }

                    node_N->key = min_key;
                    node_N->value = min_value;
                }
                else {
                    node_N->right = xrbtree_remove_impl(tree, node_N->right, key, old_value, deep);
                }
            }
        }
        else if (ret < 0) {
            /* key should be in left branch, but left branch is empty */
            if (!node_N->left) {
                return node_N;
            }

            /* node_N->left has no 2 or 3 keys
            *          |
            *         N(R)
            *       /      \
            *     E(B)      T
            */
            if (xrbtree_node_color_black(node_N->left) && xrbtree_node_color_black(node_N->left->left) && xrbtree_node_color_black(node_N->left->right)) {
                /* node_N->right is red
                *          |                       |
                *         N(R)                    T(R)
                *       /      \       ->       /      \
                *     E(B)     T(R)            N(R)     U
                *              /  \           /   \
                *             S    U        E(B)   S
                */
                if (xrbtree_node_color_red(node_N->right)) {
                    node_N = xrbtree_rotate_left(node_N);
                }
                else {
                    /* node_N->right has 2 or 3 keys, move one to left
                    *           |                         |
                    *          N(R)                      Q(R)
                    *       /       \                   /    \
                    *     E(B)       T        -->      N      T
                    *              /   \              / \    / \
                    *           Q(R)   U            E(R) P  S   U
                    *           / \
                    *          P   S
                    */
                    if (node_N->right && (xrbtree_node_color_red(node_N->right->left) || xrbtree_node_color_red(node_N->right->right))) {
                        node_N = xrbtree_move_red_left(node_N);
                    }
                    else {
                        /* node_N->right is black
                        *           |                         |
                        *          N(R)                       N
                        *       /       \         -->       /    \
                        *     E(B)       T                E(R)   T(R)
                        */
                        xassert(xrbtree_node_color_red(node_N) && xrbtree_node_color_black(node_N->left) && xrbtree_node_color_black(node_N->right));
                        xrbtree_flip_color(node_N);
                    }
                }
            }

            /* node_N->left has 2 or 3 keys now */
            node_N->left = xrbtree_remove_impl(tree, node_N->left, key, old_value, deep);
        }
        else {
            /* key should be in right branch, but right branch is empty */
            if (!node_N->right) {
                return node_N;
            }

            /* node_N->right has no 2 or 3 keys
            *          |
            *         N(R)
            *       /      \
            *      C       T(B)
            */
            if (xrbtree_node_color_black(node_N->right) && xrbtree_node_color_black(node_N->right->left) && xrbtree_node_color_black(node_N->right->right)) {
                /* node->left is red
                *          |                       |
                *         N(R)                    C(R)
                *       /      \       ->       /      \
                *     C(R)     T(B)            A       N(R)
                *     /  \                            /    \
                *    A    E                          E     T(B)
                */
                if (xrbtree_node_color_red(node_N->left)) {
                    node_N = xrbtree_rotate_right(node_N);
                }
                else {
                    /* node_N->left has 2 or 3 keys, move one to right
                    *           |                        |
                    *          N(R)                     E(R)
                    *       /       \                  /    \
                    *     C(B)      T(B)     -->     C(B)    N
                    *     / \                        / \    /  \
                    *    A  E(R)                    A   D  F   T(R)
                    *       / \
                    *      D   F
                    */
                    if (node_N->left && (xrbtree_node_color_red(node_N->left->left) || xrbtree_node_color_red(node_N->left->right))) {
                        node_N = xrbtree_move_red_right(node_N);
                    }
                    else {
                        /* node_N->left is black
                        *
                        *           |                         |
                        *          N(R)                       N
                        *       /       \         -->       /    \
                        *     C(B)      T(B)              C(R)   T(R)
                        */
                        xassert(xrbtree_node_color_red(node_N) && xrbtree_node_color_black(node_N->left) && xrbtree_node_color_black(node_N->right));
                        xrbtree_flip_color(node_N);
                    }
                }
            }

            /* node->right has 2 or 3 keys now */
            node_N->right = xrbtree_remove_impl(tree, node_N->right, key, old_value, deep);
        }

        return xrbtree_balance(node_N);
    }
}

int xrbtree_remove(XRBTree_PT tree, void *key) {
    if (!tree || !key || !tree->root) {
        return 0;
    }

    {
        int total = tree->root->size;

        if (xrbtree_node_color_black(tree->root->left) && xrbtree_node_color_black(tree->root->right)) {
            tree->root->color = xrbtree_color_red;
        }

        tree->root = xrbtree_remove_impl(tree, tree->root, key, NULL, false);
        if (tree->root) {
            tree->root->color = xrbtree_color_black;
        }

        return (total - xrbtree_size(tree));
    }
}

int xrbtree_remove_all(XRBTree_PT tree, void *key) {
    int count = 0;

    while (0 < xrbtree_remove(tree, key)) {
        ++count;
    }

    return count;
}

int xrbtree_remove_save(XRBTree_PT tree, void *key, void **value) {
    xassert(tree);
    xassert(key);

    if (!tree || !key || !tree->root) {
        return 0;
    }

    {
        int total = tree->root->size;

        if (xrbtree_node_color_black(tree->root->left) && xrbtree_node_color_black(tree->root->right)) {
            tree->root->color = xrbtree_color_red;
        }

        tree->root = xrbtree_remove_impl(tree, tree->root, key, value, false);
        if (tree->root) {
            tree->root->color = xrbtree_color_black;
        }

        return (total - xrbtree_size(tree));
    }
}

int xrbtree_deep_remove(XRBTree_PT tree, void *key) {
    xassert(tree);
    xassert(key);

    if (!tree || !key || !tree->root) {
        return 0;
    }

    {
        int total = tree->root->size;

        if (xrbtree_node_color_black(tree->root->left) && xrbtree_node_color_black(tree->root->right)) {
            tree->root->color = xrbtree_color_red;
        }

        tree->root = xrbtree_remove_impl(tree, tree->root, key, NULL, true);
        if (tree->root) {
            tree->root->color = xrbtree_color_black;
        }

        return (total - xrbtree_size(tree));
    }
}

int xrbtree_deep_remove_all(XRBTree_PT tree, void *key) {
    int count = 0;

    while (0 < xrbtree_deep_remove(tree, key)) {
        ++count;
    }

    return count;
}

static 
int xrbtree_map_preorder_impl(XRBTree_PT tree, XRBTree_Node_PT node, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        if (apply(node->key, &(node->value), cl)) {
            count++;
        }

        count += xrbtree_map_preorder_impl(tree, node->left, apply, cl);
        count += xrbtree_map_preorder_impl(tree, node->right, apply, cl);

        return count;
    }
}

int xrbtree_map_preorder(XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xrbtree_map_preorder_impl(tree, (tree ? tree->root : NULL), apply, cl);
}

static 
int xrbtree_map_inorder_impl(XRBTree_PT tree, XRBTree_Node_PT node, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        count += xrbtree_map_inorder_impl(tree, node->left, apply, cl);

        if (apply(node->key, &(node->value), cl)) {
            count++;
        }

        count += xrbtree_map_inorder_impl(tree, node->right, apply, cl);

        return count;
    }
}

int xrbtree_map_inorder(XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xrbtree_map_inorder_impl(tree, (tree ? tree->root : NULL), apply, cl);
}

static
int xrbtree_map_postorder_impl(XRBTree_PT tree, XRBTree_Node_PT node, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        count += xrbtree_map_postorder_impl(tree, node->left, apply, cl);
        count += xrbtree_map_postorder_impl(tree, node->right, apply, cl);

        if (apply(node->key, &(node->value), cl)) {
            count++;
        }

        return count;
    }
}

int xrbtree_map_postorder(XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xrbtree_map_postorder_impl(tree, (tree ? tree->root : NULL), apply, cl);
}

int xrbtree_map_levelorder(XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
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
                XRBTree_Node_PT node = (XRBTree_Node_PT)xfifo_pop(fifo);
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

int xrbtree_map_min_to_max_impl(XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    int count = 0;

    XRBTree_Node_PT node = xrbtree_min_impl(tree, tree ? tree->root : NULL);
    while (node) {
        if (apply(node->key, &node->value, cl)) {
            ++count;
        }

        node = xrbtree_next_node(tree, node);
    }

    return count;
}

bool xrbtree_map_min_to_max_break_if_impl(XRBTree_PT tree, bool break_true, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    XRBTree_Node_PT node = xrbtree_min_impl(tree, tree ? tree->root : NULL);
    while (node) {
        bool ret = apply(node->key, &node->value, cl);
        if (ret && break_true) {
            return true;
        }
        if (!ret && !break_true) {
            return true;
        }

        node = xrbtree_next_node(tree, node);
    }

    return false;
}

int xrbtree_map_max_to_min_impl(XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    int count = 0;

    XRBTree_Node_PT node = xrbtree_max_impl(tree, tree ? tree->root : NULL);
    while (node) {
        if (apply(node->key, &node->value, cl)) {
            ++count;
        }

        node = xrbtree_prev_node(tree, node);
    }

    return count;
}

bool xrbtree_map_max_to_min_break_if_impl(XRBTree_PT tree, bool break_true, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    XRBTree_Node_PT node = xrbtree_max_impl(tree, tree ? tree->root : NULL);
    while (node) {
        bool ret = apply(node->key, &node->value, cl);
        if (ret && break_true) {
            return true;
        }
        if (!ret && !break_true) {
            return true;
        }

        node = xrbtree_prev_node(tree, node);
    }

    return false;
}

int xrbtree_map_min_to_max(XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xrbtree_map_min_to_max_impl(tree, apply, cl);
}

bool xrbtree_map_min_to_max_break_if_true(XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return false;
    }

    return xrbtree_map_min_to_max_break_if_impl(tree, true, apply, cl);
}

bool xrbtree_map_min_to_max_break_if_false(XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return true;
    }

    return xrbtree_map_min_to_max_break_if_impl(tree, false, apply, cl);
}

int xrbtree_map_max_to_min(XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xrbtree_map_max_to_min_impl(tree, apply, cl);
}

bool xrbtree_map_max_to_min_break_if_true(XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return false;
    }

    return xrbtree_map_max_to_min_break_if_impl(tree, true, apply, cl);
}

bool xrbtree_map_max_to_min_break_if_false(XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return true;
    }

    return xrbtree_map_max_to_min_break_if_impl(tree, false, apply, cl);
}

int xrbtree_map_key_min_to_max_impl(XRBTree_PT tree, bool (*apply)(void *key, void *cl), void *cl) {
    int count = 0;

    XRBTree_Node_PT node = xrbtree_min_impl(tree, tree ? tree->root : NULL);
    while (node) {
        if (apply(node->key, cl)) {
            ++count;
        }

        node = xrbtree_next_node(tree, node);
    }

    return count;
}

bool xrbtree_map_key_min_to_max_break_if_impl(XRBTree_PT tree, bool break_true, bool (*apply)(void *key, void *cl), void *cl) {
    XRBTree_Node_PT node = xrbtree_min_impl(tree, tree ? tree->root : NULL);
    while (node) {
        bool ret = apply(node->key, cl);
        if (ret && break_true) {
            return true;
        }
        if (!ret && !break_true) {
            return true;
        }

        node = xrbtree_next_node(tree, node);
    }

    return false;
}

int xrbtree_map_key_max_to_min_impl(XRBTree_PT tree, bool (*apply)(void *key, void *cl), void *cl) {
    int count = 0;

    XRBTree_Node_PT node = xrbtree_max_impl(tree, tree ? tree->root : NULL);
    while (node) {
        if (apply(node->key, cl)) {
            ++count;
        }

        node = xrbtree_prev_node(tree, node);
    }

    return count;
}

bool xrbtree_map_key_max_to_min_break_if_impl(XRBTree_PT tree, bool break_true, bool (*apply)(void *key, void *cl), void *cl) {
    XRBTree_Node_PT node = xrbtree_max_impl(tree, tree ? tree->root : NULL);
    while (node) {
        bool ret = apply(node->key, cl);
        if (ret && break_true) {
            return true;
        }
        if (!ret && !break_true) {
            return true;
        }

        node = xrbtree_prev_node(tree, node);
    }

    return false;
}

int xrbtree_map_key_min_to_max(XRBTree_PT tree, bool (*apply)(void *key, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xrbtree_map_key_min_to_max_impl(tree, apply, cl);
}

bool xrbtree_map_key_min_to_max_break_if_true(XRBTree_PT tree, bool (*apply)(void *key, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return false;
    }

    return xrbtree_map_key_min_to_max_break_if_impl(tree, true, apply, cl);
}

bool xrbtree_map_key_min_to_max_break_if_false(XRBTree_PT tree, bool (*apply)(void *key, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return true;
    }

    return xrbtree_map_key_min_to_max_break_if_impl(tree, false, apply, cl);
}

int xrbtree_map_key_max_to_min(XRBTree_PT tree, bool (*apply)(void *key, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return false;
    }

    return xrbtree_map_key_max_to_min_impl(tree, apply, cl);
}

bool xrbtree_map_key_max_to_min_break_if_true(XRBTree_PT tree, bool (*apply)(void *key, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return false;
    }

    return xrbtree_map_key_max_to_min_break_if_impl(tree, true, apply, cl);
}

bool xrbtree_map_key_max_to_min_break_if_false(XRBTree_PT tree, bool (*apply)(void *key, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return true;
    }

    return xrbtree_map_key_max_to_min_break_if_impl(tree, false, apply, cl);
}

int xrbtree_scope_map_min_to_max(XRBTree_PT tree, void *low, void *high, bool (*apply)(void *key, void **value, void *cl), void *cl) {
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
            XRBTree_Node_PT node = xrbtree_get_impl(tree, tree->root, low);
            if (node) {
                /* the equal keys may be saved in pre nodes */
                XRBTree_Node_PT tnode = xrbtree_prev_node(tree, node);
                while (tnode) {
                    if (tree->cmp(node->key, tnode->key, tree->cl) == 0) {
                        if (apply(tnode->key, tnode->value, cl)) {
                            ++count;
                        }
                    }
                    tnode = xrbtree_prev_node(tree, tnode);
                }
            }

            while (node && (tree->cmp(node->key, high, tree->cl) <= 0)) {
                if (apply(node->key, node->value, cl)) {
                    ++count;
                }
                node = xrbtree_next_node(tree, node);
            }
        }

        return count;
    }
}

bool xrbtree_scope_map_min_to_max_break_if_true(XRBTree_PT tree, void *low, void *high, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(low);
    xassert(high);
    xassert(apply);

    if (!tree || !(tree->root) || !low || !high || !apply) {
        return false;
    }

    if (0 < tree->cmp(low, high, tree->cl)) {
        void *tmp = low;
        low = high;
        high = tmp;
    }

    {
        XRBTree_Node_PT node = xrbtree_get_impl(tree, tree->root, low);
        if (node) {
            /* the equal keys may be saved in pre nodes */
            XRBTree_Node_PT tnode = xrbtree_prev_node(tree, node);
            while (tnode) {
                if (tree->cmp(node->key, tnode->key, tree->cl) == 0) {
                    if (apply(tnode->key, tnode->value, cl)) {
                        return true;
                    }
                }
                tnode = xrbtree_prev_node(tree, tnode);
            }
        }

        while (node && (tree->cmp(node->key, high, tree->cl) <= 0)) {
            if (apply(node->key, node->value, cl)) {
                return true;
            }
            node = xrbtree_next_node(tree, node);
        }
    }

    return false;
}

bool xrbtree_scope_map_min_to_max_break_if_false(XRBTree_PT tree, void *low, void *high, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(low);
    xassert(high);
    xassert(apply);

    if (!tree || !(tree->root) || !low || !high || !apply) {
        return true;
    }

    if (0 < tree->cmp(low, high, tree->cl)) {
        void *tmp = low;
        low = high;
        high = tmp;
    }

    {
        XRBTree_Node_PT node = xrbtree_get_impl(tree, tree->root, low);
        if (node) {
            /* the equal keys may be saved in pre nodes */
            XRBTree_Node_PT tnode = xrbtree_prev_node(tree, node);
            while (tnode) {
                if (tree->cmp(node->key, tnode->key, tree->cl) == 0) {
                    if (!apply(tnode->key, tnode->value, cl)) {
                        return true;
                    }
                }
                tnode = xrbtree_prev_node(tree, tnode);
            }
        }

        while (node && (tree->cmp(node->key, high, tree->cl) <= 0)) {
            if (!apply(node->key, node->value, cl)) {
                return true;
            }
            node = xrbtree_next_node(tree, node);
        }
    }

    return false;
}

bool xrbtree_swap(XRBTree_PT tree1, XRBTree_PT tree2) {
    xassert(tree1);
    xassert(tree2);

    if (!tree1 || !tree2) {
        return false;
    }

    {
        {
            XRBTree_Node_PT root = tree1->root;
            tree1->root = tree2->root;
            tree2->root = root;
        }

        {
            int (*cmp)(void *key1, void *key2, void *cl) = tree1->cmp;
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

int xrbtree_keys_size(XRBTree_PT tree, void *low, void *high) {
    xassert(tree);
    xassert(low);
    xassert(high);

    if (!tree || !(tree->root) || !low || !high) {
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
            XRBTree_Node_PT node = xrbtree_get_impl(tree, tree->root, low);
            if (node) {
                /* the equal keys may be saved in pre nodes */
                XRBTree_Node_PT tnode = xrbtree_prev_node(tree, node);
                while (tnode) {
                    if (tree->cmp(node->key, tnode->key, tree->cl) == 0) {
                        ++count;
                    }
                    tnode = xrbtree_prev_node(tree, tnode);
                }
            }

            while (node && (tree->cmp(node->key, high, tree->cl) <= 0)) {
                ++count;
                node = xrbtree_next_node(tree, node);
            }
        }

        return count;
    }
}

int xrbtree_size(XRBTree_PT tree) {
    return (tree ? (tree->root ? tree->root->size : 0) : 0);
}

bool xrbtree_is_empty(XRBTree_PT tree) {
    return (tree ? (tree->root ? (tree->root->size == 0) : true) : true);
}

static
int xrbtree_height_impl(XRBTree_Node_PT node) {
    if (!node) {
        return 0;
    }

    {
        int u = xrbtree_height_impl(node->left);
        int v = xrbtree_height_impl(node->right);

        return u < v ? (v + 1) : (u + 1);
    }
}

int xrbtree_height(XRBTree_PT tree) {
    return xrbtree_height_impl(tree ? tree->root : NULL);
}

static 
int xrbtree_count_black_to_root(XRBTree_PT tree, XRBTree_Node_PT node) {
    int count = 0;

    while (node != tree->root) {
        if (xrbtree_node_color_black(node)) {
            ++count;
        }
        node = node->parent;
    }

    return count + 1;  /* 1 means tree->root is black */
}

bool xrbtree_is_rbtree(XRBTree_PT tree) {
    if (!tree) {
        return false;
    }

    if (tree->root) {
        /* root is black */
        if (xrbtree_node_color_red(tree->root)) {
            return false;
        }
    }
    else {
        /* no root */
        return true;
    }

    {
        int count = 0;

        XRBTree_Node_PT node = xrbtree_min_impl(tree, tree->root);
        int black = xrbtree_count_black_to_root(tree, node);

        while (node) {
            ++count;

            /* node is red, it's children must be black */
            if (xrbtree_node_color_red(node) && (xrbtree_node_color_red(node->left) || xrbtree_node_color_red(node->right)))
            {
                return false;
            }

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

            /* the black node number from every leaf node to root must be the same */
            if (!node->left && !node->right && (black != xrbtree_count_black_to_root(tree, node))) {
                return false;
            }

            node = xrbtree_next_node(tree, node);
        }

        /* total node number is the same with the size saved in root */
        if (count != tree->root->size) {
            return false;
        }
    }

    return true;
}