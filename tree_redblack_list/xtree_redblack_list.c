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
#include "../list_s_raw/xlist_s_raw_x.h"
#include "../list_s/xlist_s_x.h"
#include "../include/xqueue_fifo.h"
#include "xtree_redblack_list_x.h"

/* Note :
 *    1. only one key exist when they are equal ("cmp" return 0)
 *       so, when keys are different, but "cmp" return 0, only the first key will be saved in the tree
 *
 *    2. if "value" is NULL, 
 *       put_repeat operation may not increase the size since nothing will be saved in the tree if "values" has elements already
 *       put_replace operation may decrease the size since one value will be erased if "values" has elements already
*/
static const bool xlistrbtree_color_red = false;
static const bool xlistrbtree_color_black = true;

static
bool xlistrbtree_node_color_red(XListRBTree_Node_PT node) {
    return node ? (node->color == xlistrbtree_color_red) : false;
}

static
bool xlistrbtree_node_color_black(XListRBTree_Node_PT node) {    
    return node ? (node->color == xlistrbtree_color_black) : true;  /* NULL node is black */
}

XListRBTree_Node_PT xlistrbtree_min_impl(XListRBTree_PT tree, XListRBTree_Node_PT node) {
    if (!node) {
        return NULL;
    }

    while (node->left) {
        node = node->left;
    }

    return node;
}

XListRBTree_Node_PT xlistrbtree_max_impl(XListRBTree_PT tree, XListRBTree_Node_PT node) {
    if (!node) {
        return NULL;
    }

    while (node->right) {
        node = node->right;
    }

    return node;
}

XListRBTree_Node_PT xlistrbtree_get_impl(XListRBTree_PT tree, XListRBTree_Node_PT node, void *key) {
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

XListRBTree_Node_PT xlistrbtree_prev_node(XListRBTree_PT tree, XListRBTree_Node_PT node) {
    if (node->left) {
        return xlistrbtree_max_impl(tree, node->left);
    }

    {
        /* H's pre node is E :
        *      E
        *       \
        *         R
        *        / \
        *       H   S
        */
        XListRBTree_Node_PT parent = node->parent;
        while (parent && (node == parent->left)) {
            node = parent;
            parent = parent->parent;
        }

        return parent;
    }
}

XListRBTree_Node_PT xlistrbtree_next_node(XListRBTree_PT tree, XListRBTree_Node_PT node) {
    if (node->right) {
        return xlistrbtree_min_impl(tree, node->right);
    }

    {
        /* R's next node is S :
        *         S
        *       /   \
        *     E       T
        *      \
        *       R
        */
        XListRBTree_Node_PT parent = node->parent;
        while (parent && (node == parent->right)) {
            node = parent;
            parent = parent->parent;
        }

        return parent;
    }
}

/* xlistrbtree_flip_color :
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
void xlistrbtree_flip_color(XListRBTree_Node_PT node_N) {
    node_N->color = !node_N->color;
    if (node_N->left) {
        node_N->left->color = !node_N->left->color;
    }
    if (node_N->right) {
        node_N->right->color = !node_N->right->color;
    }
}

/* xlistrbtree_rotate_left :
*
*      |                   |
*      N                   X
*    /   \       -->     /   \
*   D    X(R)          N(R)    T
*        /  \          /  \
*       F    T        D    F
*/
static
XListRBTree_Node_PT xlistrbtree_rotate_left(XListRBTree_Node_PT node_N) {
    XListRBTree_Node_PT x = node_N->right;

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
    node_N->color = xlistrbtree_color_red;
    x->size = node_N->size;
    node_N->size = node_N->node_size + (node_N->left ? node_N->left->size : 0) + (node_N->right ? node_N->right->size : 0);

    return x;
}

/* xlistrbtree_rotate_right :
*
*         |                 |
*         N                 X
*       /   \     -->     /   \
*     X(R)   T           D    N(R)
*     /  \                    /  \
*    D    F                  F    T
*/
static
XListRBTree_Node_PT xlistrbtree_rotate_right(XListRBTree_Node_PT node_N) {
    XListRBTree_Node_PT x = node_N->left;

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
    node_N->color = xlistrbtree_color_red;
    x->size = node_N->size;
    node_N->size = node_N->node_size + (node_N->left ? node_N->left->size : 0) + (node_N->right ? node_N->right->size : 0);

    return x;
}

static
XListRBTree_Node_PT xlistrbtree_balance(XListRBTree_Node_PT node_N) {
    /* 1. node_N->left is red, node_N->right is black */
    if (xlistrbtree_node_color_red(node_N->left) && xlistrbtree_node_color_black(node_N->right)) {
        /* 1.1 node_N->left->right is red, left rotate at first, then right rotate as step 1.2 :
        *         |                  |
        *         N                  N
        *       /   \     -->      /   \
        *     D(R)   X           F(R)   X
        *       \                /
        *       F(R)           D(R)
        */
        if (xlistrbtree_node_color_black(node_N->left->left) && xlistrbtree_node_color_red(node_N->left->right)) {
            node_N->left = xlistrbtree_rotate_left(node_N->left);
        }

        /* 1.2 node_N->left->left is red, right rotate :
        *         |                  |
        *         N                  F
        *       /   \     -->      /   \
        *     F(R)   X           D(R)  N(R)
        *     /                          \
        *   D(R)                          X
        */
        if (xlistrbtree_node_color_red(node_N->left->left) && xlistrbtree_node_color_black(node_N->left->right)) {
            node_N = xlistrbtree_rotate_right(node_N);
        }
    }

    /* 2. node_N->left is black, node_N->right is red */
    if (xlistrbtree_node_color_black(node_N->left) && xlistrbtree_node_color_red(node_N->right)) {
        /* 2.1 node_N->right->left is red, right rotate at first, then left rotate as step 2.2 :
        *         |                  |
        *         N                  N
        *       /   \     -->      /   \
        *      D   X(R)           D    U(R)
        *           /                    \
        *         U(R)                   X(R)
        */
        if (xlistrbtree_node_color_red(node_N->right->left) && xlistrbtree_node_color_black(node_N->right->right)) {
            node_N->right = xlistrbtree_rotate_right(node_N->right);
        }

        /* 2.2 node_N->right->right is red, left rotate :
        *         |                  |
        *         N                  U
        *       /   \     -->      /   \
        *      D    U(R)         N(R)  X(R)
        *             \          /
        *             X(R)      D
        */
        if (xlistrbtree_node_color_black(node_N->right->left) && xlistrbtree_node_color_red(node_N->right->right)) {
            node_N = xlistrbtree_rotate_left(node_N);
        }
    }

    /* 3. node_N->left and node_N->right are all red, make the node_N red to pop up it to its parent layer nodes,
    *         |                  |
    *         N                 N(R)
    *       /   \     -->      /   \
    *     D(R)  X(R)          D     X
    */
    if (xlistrbtree_node_color_red(node_N->left) && xlistrbtree_node_color_red(node_N->right)) {
        xlistrbtree_flip_color(node_N);
    }

    node_N->size = node_N->node_size + (node_N->left ? node_N->left->size : 0) + (node_N->right ? node_N->right->size : 0);

    return node_N;
}

static
XListRBTree_Node_PT xlistrbtree_new_node(void *key, void *value, bool color) {
    XListRBTree_Node_PT node = XMEM_CALLOC(1, sizeof(*node));
    if (!node) {
        return NULL;
    }

    node->key = key;
    if (value) {
        node->values = xrslist_new(value);
        if (!node->values) {
            XMEM_FREE(node);
            return NULL;
        }
    }

    node->node_size = 1;
    node->size = 1;
    node->color = color;

    //node->parent = NULL;
    //node->left = NULL;
    //node->right = NULL;

    return node;
}

static
XListRBTree_Node_PT xlistrbtree_new_node_with_list(void *key, XRSList_PT values, int size, bool color) {
    XListRBTree_Node_PT node = XMEM_CALLOC(1, sizeof(*node));
    if (!node) {
        return NULL;
    }

    node->key = key;
    node->values = values;

    node->node_size = size;
    node->size = size;
    node->color = color;

    //node->parent = NULL;
    //node->left = NULL;
    //node->right = NULL;

    return node;
}

XListRBTree_PT xlistrbtree_new(int (*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(cmp);
    
    if (!cmp) {
        return NULL;
    }

    {
        XListRBTree_PT tree = XMEM_CALLOC(1, sizeof(*tree));
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
XListRBTree_Node_PT xlistrbtree_copy_node(XListRBTree_Node_PT node, XListRBTree_Node_PT nparent, bool *false_found, void *cl) {
    XListRBTree_Node_PT nnode = XMEM_CALLOC(1, sizeof(*nnode));
    if (!nnode) {
        *false_found = true;
        return NULL;
    }

    nnode->key = node->key;
    nnode->values = xrslist_copy(node->values);
    if (node->values && !nnode->values) {
        *false_found = true;
        XMEM_FREE(nnode);
        return NULL;
    }

    nnode->node_size = node->node_size;
    nnode->size = node->size;
    nnode->color = node->color;

    nnode->parent = nparent;
    //nnode->left = NULL;
    //nnode->right = NULL;

    return nnode;
}

void xlistrbtree_copy_break_if_false_impl(XListRBTree_PT tree, XListRBTree_Node_PT node, XListRBTree_PT ntree, XListRBTree_Node_PT nparent, bool root, bool left, bool *false_found, XListRBTree_Node_PT (*apply)(XListRBTree_Node_PT node, XListRBTree_Node_PT nparent, bool *false_found, void *cl), void *cl) {
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

    xlistrbtree_copy_break_if_false_impl(tree, node->left, ntree, nparent, false, true, false_found, apply, cl);
    if (*false_found) {
        return;
    }

    xlistrbtree_copy_break_if_false_impl(tree, node->right, ntree, nparent, false, false, false_found, apply, cl);
}

XListRBTree_PT xlistrbtree_copy(XListRBTree_PT tree) {
    xassert(tree);

    if (!tree) {
        return NULL;
    }

    {
        bool false_found = false;

        XListRBTree_PT ntree = xlistrbtree_new(tree->cmp, tree->cl);
        if (!ntree) {
            return NULL;
        }

        xlistrbtree_copy_break_if_false_impl(tree, tree->root, ntree, NULL, true, false, &false_found, xlistrbtree_copy_node, NULL);
        if (false_found) {
            xlistrbtree_free(&ntree);
            return NULL;
        }

        xassert(xlistrbtree_size(tree) == xlistrbtree_size(ntree));

        return ntree;
    }
}

static
XListRBTree_Node_PT xlistrbtree_deep_copy_node(XListRBTree_Node_PT node, XListRBTree_Node_PT nparent, bool *false_found, void *cl) {
    XListRBTree_3Paras_PT paras = (XListRBTree_3Paras_PT)cl;

    XListRBTree_Node_PT nnode = XMEM_CALLOC(1, sizeof(*nnode));
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

    nnode->values = xrslist_deep_copy(node->values, *((int*)paras->para2));
    if (node->values && !nnode->values) {
        *false_found = true;
        XMEM_FREE(nnode->key);
        XMEM_FREE(nnode);
        return NULL;
    }

    nnode->node_size = node->node_size;
    nnode->size = node->size;
    nnode->color = node->color;

    nnode->parent = nparent;
    //nnode->left = NULL;
    //nnode->right = NULL;

    return nnode;
}

XListRBTree_PT xlistrbtree_deep_copy(XListRBTree_PT tree, int key_size, int value_size) {
    xassert(tree);
    xassert(0 < key_size);
    xassert(0 < value_size);

    if (!tree || (key_size <= 0) || (value_size <= 0)) {
        return NULL;
    }

    {
        XListRBTree_PT ntree = xlistrbtree_new(tree->cmp, tree->cl);
        if (!ntree) {
            return NULL;
        }

        {
            XListRBTree_3Paras_T paras = { ntree, (void*)&key_size, (void*)&value_size, NULL };

            bool false_found = false;
            xlistrbtree_copy_break_if_false_impl(tree, tree->root, ntree, NULL, true, false, &false_found, xlistrbtree_deep_copy_node, (void*)&paras);
            if (false_found) {
                xlistrbtree_deep_free(&ntree);
                return NULL;
            }
        }

        xassert(xlistrbtree_size(tree) == xlistrbtree_size(ntree));

        return ntree;
    }
}

static
XListRBTree_Node_PT xlistrbtree_put_repeat_impl(XListRBTree_PT tree, XListRBTree_Node_PT parent, XListRBTree_Node_PT node, void *key, void *value, bool *false_found, bool unique) {
    /* reach the leaf node, return the new created new_node */
    if (!node) {
        XListRBTree_Node_PT nnode = xlistrbtree_new_node(key, value, xlistrbtree_color_red);
        if (!nnode) {
            *false_found = true;
        }
        else {
            nnode->parent = parent;
        }
        return nnode;
    }

    {
        int ret = tree->cmp(key, node->key, tree->cl);
        if (ret == 0) {
            if (unique) {
                return node;
            }

            /* if value is NULL, it will be ignored here since node->values has elements already */

            if (value) {
                if (node->values) {
                    if (!xrslist_push_front_repeat(&node->values, value)) {
                        *false_found = true;
                        return node;
                    }
                    ++node->node_size;
                    ++node->size;
                }
                else {
                    node->values = xrslist_new(value);
                    if (!node->values) {
                        *false_found = true;
                    }
                    /* no need to increase node_size since it's 1 already */
                }
            }

            return node;
        }

        /* find the branch to insert the new node */
        if (ret < 0) {
            node->left = xlistrbtree_put_repeat_impl(tree, node, node->left, key, value, false_found, unique);
        }
        else {
            node->right = xlistrbtree_put_repeat_impl(tree, node, node->right, key, value, false_found, unique);
        }

        /* make the tree balance again */
        return xlistrbtree_balance(node);
    }
}

bool xlistrbtree_put_repeat(XListRBTree_PT tree, void *key, void *value) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return false;
    }

    {
        bool false_found = false;

        tree->root = xlistrbtree_put_repeat_impl(tree, NULL, tree->root, key, value, &false_found, false);
        if (tree->root) {
            tree->root->color = xlistrbtree_color_black;
        }

        return !false_found;
    }
}

bool xlistrbtree_put_unique(XListRBTree_PT tree, void *key, void *value) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return false;
    }

    {
        bool false_found = false;

        tree->root = xlistrbtree_put_repeat_impl(tree, NULL, tree->root, key, value, &false_found, true);
        if (tree->root) {
            tree->root->color = xlistrbtree_color_black;
        }

        return !false_found;
    }
}


static
XListRBTree_Node_PT xlistrbtree_put_replace_impl(XListRBTree_PT tree, XListRBTree_Node_PT parent, XListRBTree_Node_PT node, void *key, void *value, void **old_value, bool *false_found, bool deep) {
    /* reach the leaf node, return the new created new_node */
    if (!node) {
        XListRBTree_Node_PT nnode = xlistrbtree_new_node(key, value, xlistrbtree_color_red);
        if (!nnode) {
            *false_found = true;
        }
        else {
            nnode->parent = parent;
        }
        return nnode;
    }

    {
        int ret = tree->cmp(key, node->key, tree->cl);
        if (ret == 0) {
            if (node->values) {
                if (value) {
                    if (deep) {
                        XMEM_FREE(node->values->value);
                    }
                    else if (old_value) {
                        *old_value = node->values->value;
                    }
                    node->values->value = value;
                }
                else {                    
                    void* t = xrslist_pop_front(&node->values);
                    if (old_value) {
                        *old_value = t;
                    }

                    if (node->values) {
                        --node->node_size;
                        --node->size;
                    }
                }
            }
            else {
                if (value) {
                    node->values = xrslist_new(value);
                    if (!node->values) {
                        *false_found = true;
                    }
                }
            }

            return node;
        }

        /* find the branch to insert the new node */
        if (ret < 0) {
            node->left = xlistrbtree_put_replace_impl(tree, node, node->left, key, value, old_value, false_found, deep);
        }
        else {
            node->right = xlistrbtree_put_replace_impl(tree, node, node->right, key, value, old_value, false_found, deep);
        }

        /* make the tree balance again */
        return xlistrbtree_balance(node);
    }
}

bool xlistrbtree_put_replace(XListRBTree_PT tree, void *key, void *value, void **old_value) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return false;
    }

    {
        bool false_found = false;

        tree->root = xlistrbtree_put_replace_impl(tree, NULL, tree->root, key, value, old_value, &false_found, false);
        if (tree->root) {
            tree->root->color = xlistrbtree_color_black;
        }

        return !false_found;
    }
}

bool xlistrbtree_put_deep_replace(XListRBTree_PT tree, void *key, void *value) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return false;
    }

    {
        bool false_found = false;

        tree->root = xlistrbtree_put_replace_impl(tree, NULL, tree->root, key, value, NULL, &false_found, true);
        if (tree->root) {
            tree->root->color = xlistrbtree_color_black;
        }

        return !false_found;
    }
}

void xlistrbtree_key_unique(XListRBTree_PT tree, void *key) {
    XListRBTree_Node_PT node = xlistrbtree_get_impl(tree, (tree ? tree->root : NULL), key);
    if (node && node->values) {
        int count = xrslist_free_except_front(node->values);
        if (0 < count) {
            node->node_size = 1;

            while (node) {
                node->size -= count;
                node = node->parent;
            }
        }
    }
}

static
void xlistrbtree_unique_correct_size_impl(XListRBTree_PT tree, XListRBTree_Node_PT node) {
    if (!node) {
        return;
    }

    {
        xlistrbtree_unique_correct_size_impl(tree, node->left);
        xlistrbtree_unique_correct_size_impl(tree, node->right);
        node->size = node->node_size + (node->left ? node->left->size : 0) + (node->right ? node->right->size : 0);
    }
}

static
bool xlistrbtree_unique_impl(XListRBTree_PT tree, XListRBTree_PT set) {
    if (!tree) {
        return false;
    }

    {
        bool updated = false;

        //uniq the keys at first
        XListRBTree_Node_PT node = xlistrbtree_min_impl(tree, tree->root);
        while (node) {
            if (node->values) {
                if (1 <= xrslist_free_except_front(node->values)) {
                    node->node_size = 1;
                    updated = true;

                    if (set && !xlistrbtree_put_repeat(set, xrslist_front(node->values), xrslist_front(node->values))) {
                        xlistrbtree_unique_correct_size_impl(tree, tree->root);
                        return false;
                    }
                }
            }

            node = xlistrbtree_next_node(tree, node);
        }

        //correct the total node size
        if (updated) {
            xlistrbtree_unique_correct_size_impl(tree, tree->root);
        }

        return true;
    }
}

void xlistrbtree_unique(XListRBTree_PT tree) {
    xlistrbtree_unique_impl(tree, NULL);
}

XListRBTree_PT xlistrbtree_unique_return_uniqued(XListRBTree_PT tree) {
    if (!tree) {
        return NULL;
    }

    {
        XListRBTree_PT set = xlistrbtree_new(tree->cmp, tree->cl);
        if (!set) {
            return NULL;
        }

        if (!xlistrbtree_unique_impl(tree, set)) {
            xlistrbtree_free(&set);
            return NULL;
        }

        return set;
    }
}

static
bool xlistrbtree_unique_except_impl(XListRBTree_PT tree, void *key, XListRBTree_PT set) {
    if (!tree || !key) {
        return false;
    }

    {
        bool updated = false;

        //uniq the keys at first
        XListRBTree_Node_PT node = xlistrbtree_min_impl(tree, tree->root);
        while (node) {
            if (node->values && (0 != tree->cmp(key, xrslist_front(node->values), tree->cl))) {
                if (1 <= xrslist_free_except_front(node->values)) {
                    node->node_size = 1;
                    updated = true;

                    if (set && !xlistrbtree_put_repeat(set, xrslist_front(node->values), xrslist_front(node->values))) {
                        xlistrbtree_unique_correct_size_impl(tree, tree->root);
                        return false;
                    }
                }
            }

            node = xlistrbtree_next_node(tree, node);
        }

        //correct the total node size
        if (updated) {
            xlistrbtree_unique_correct_size_impl(tree, tree->root);
        }

        return true;
    }
}

void xlistrbtree_unique_except(XListRBTree_PT tree, void *key) {
    xlistrbtree_unique_except_impl(tree, key, NULL);
}

XListRBTree_PT xlistrbtree_unique_except_return_uniqued(XListRBTree_PT tree, void *key) {
    if (!tree || !key) {
        return NULL;
    }

    {
        XListRBTree_PT set = xlistrbtree_new(tree->cmp, tree->cl);
        if (!set) {
            return NULL;
        }

        if (!xlistrbtree_unique_except_impl(tree, key, set)) {
            xlistrbtree_free(&set);
            return NULL;
        }

        return set;
    }
}

bool xlistrbtree_has_repeat_keys_except(XListRBTree_PT tree, void *key) {
    XListRBTree_Node_PT node = xlistrbtree_min_impl(tree, (tree ? tree->root : NULL));
    while (node) {
        if (node->values && (0 != tree->cmp(key, xrslist_front(node->values), tree->cl))) {
            if (1 < node->node_size) {
                return true;
            }
        }

        node = xlistrbtree_next_node(tree, node);
    }

    return false;
}

int xlistrbtree_repeat_keys_size_except(XListRBTree_PT tree, void *key) {
    int count = 0;

    XListRBTree_Node_PT node = xlistrbtree_min_impl(tree, (tree ? tree->root : NULL));
    while (node) {
        if (node->values && (0 != tree->cmp(key, xrslist_front(node->values), tree->cl))) {
            if (1 < node->node_size) {
                count += node->node_size;
            }
        }

        node = xlistrbtree_next_node(tree, node);
    }

    return count;
}

void* xlistrbtree_min(XListRBTree_PT tree) {
    XListRBTree_Node_PT node = xlistrbtree_min_impl(tree, (tree ? tree->root : NULL));
    return node ? node->key : NULL;
}

void* xlistrbtree_max(XListRBTree_PT tree) {
    XListRBTree_Node_PT node = xlistrbtree_max_impl(tree, (tree ? tree->root : NULL));
    return node ? node->key : NULL;
}

static
XListRBTree_Node_PT xlistrbtree_floor_impl(XListRBTree_PT tree, XListRBTree_Node_PT node, void *key) {
    if (!key) {
        return NULL;
    }

    {
        XListRBTree_Node_PT result = NULL;

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
void* xlistrbtree_floor(XListRBTree_PT tree, void *key) {
    XListRBTree_Node_PT floor = xlistrbtree_floor_impl(tree, (tree ? tree->root : NULL), key);
    return floor ? floor->key : NULL;
}

static
XListRBTree_Node_PT xlistrbtree_ceiling_impl(XListRBTree_PT tree, XListRBTree_Node_PT node, void *key) {
    if (!key) {
        return NULL;
    }

    {
        XListRBTree_Node_PT result = NULL;

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
void* xlistrbtree_ceiling(XListRBTree_PT tree, void *key) {
    XListRBTree_Node_PT ceiling = xlistrbtree_ceiling_impl(tree, (tree ? tree->root : NULL), key);
    return ceiling ? ceiling->key : NULL;
}

static 
XListRBTree_Node_PT xlistrbtree_select_impl(XListRBTree_PT tree, XListRBTree_Node_PT node, int k) {
    while (node) {
        int t = node->left ? node->left->size : 0;
        if (t == k) {
            return node;
        }
        else if (t < k) {
            /* the kth value is in "node" because it has some repeat values :
            *       (3,4,5)
            *       /    \
            *   (0,1,2)  (6,7)
            *  for above tree, t = 3, if k = 4 or 5, it should still return "node"
            */
            if (k < t + node->node_size) {
                return node;
            }

            k -= t + node->node_size;
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
void* xlistrbtree_select(XListRBTree_PT tree, int k) {
    xassert(0 <= k);
    xassert(k < xlistrbtree_size(tree));

    if ((k < 0) || (xlistrbtree_size(tree) <= k)) {
        return NULL;
    }

    return xlistrbtree_select_impl(tree, (tree ? tree->root : NULL), k)->key;
}

static
bool xlistrbtree_node_value_replace_impl(XListRBTree_Node_PT node, int k, void *value, void **old_value, bool deep) {
    xassert(node);

    if (!node) {
        return false;
    }

    {
        bool size_change = false;

        if (node->values) {
            XRSList_PT tnode = xrslist_get_kth(node->values, k);
            if (value) {
                if (deep) {
                    XMEM_FREE(tnode->value);
                }
                else if (old_value) {
                    *old_value = tnode->value;
                }
                tnode->value = value;
            }
            else {
                void* t = xrslist_pop_kth(&node->values, k);
                if (old_value) {
                    *old_value = t;
                }

                if (deep) {
                    XMEM_FREE(*old_value);
                }
                if (node->values) {
                    --node->node_size;
                    --node->size;
                    size_change = true;
                }
            }
        }
        else {
            if (value) {
                node->values = xrslist_new(value);
                if (!node->values) {
                    return false;
                }
            }
        }

        /* correct the node size */
        if (size_change) {
            node = node->parent;
            while (node) {
                --node->size;
                node = node->parent;
            }
        }
    }

    return true;
}

bool xlistrbtree_index_replace(XListRBTree_PT tree, int k, void *value, void **old_value) {
    xassert(0 <= k);
    xassert(k < xlistrbtree_size(tree));

    if ((k < 0) || (xlistrbtree_size(tree) <= k)) {
        return false;
    }

    XListRBTree_Node_PT node = xlistrbtree_select_impl(tree, (tree ? tree->root : NULL), k);
    return xlistrbtree_node_value_replace_impl(node, (k - (node->left ? node->left->size : 0)), value, old_value, false);
}

bool xlistrbtree_index_deep_replace(XListRBTree_PT tree, int k, void *value) {
    xassert(0 <= k);
    xassert(k < xlistrbtree_size(tree));

    if ((k < 0) || (xlistrbtree_size(tree) <= k)) {
        return false;
    }

    XListRBTree_Node_PT node = xlistrbtree_select_impl(tree, (tree ? tree->root : NULL), k);
    return xlistrbtree_node_value_replace_impl(node, (k - (node->left ? node->left->size : 0)), value, NULL, true);
}

static
int xlistrbtree_rank_impl(XListRBTree_PT tree, XListRBTree_Node_PT node, void *key) {
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
int xlistrbtree_rank(XListRBTree_PT tree, void *key) {
    return xlistrbtree_rank_impl(tree, (tree ? tree->root : NULL), key);
}

void* xlistrbtree_get(XListRBTree_PT tree, void *key) {
    XListRBTree_Node_PT node = xlistrbtree_get_impl(tree, (tree ? tree->root : NULL), key);
    return node ? (node->values ? node->values->value : NULL) : NULL;
}

XSList_PT xlistrbtree_get_all(XListRBTree_PT tree, void *key) {
    XListRBTree_Node_PT node = xlistrbtree_get_impl(tree, (tree ? tree->root : NULL), key);
    if (node) {
        XRSList_PT list = xrslist_copy(node->values);
        if (!list) {
            return NULL;
        }
        return xslist_new_with_rslist(list);
    }

    return NULL;
}

bool xlistrbtree_find(XListRBTree_PT tree, void *key) {
    return xlistrbtree_get_impl(tree, (tree ? tree->root : NULL), key) ? true : false;
}

bool xlistrbtree_find_replace(XListRBTree_PT tree, void *key, void *value, void **old_value) {
    XListRBTree_Node_PT node = xlistrbtree_get_impl(tree, (tree ? tree->root : NULL), key);
    return xlistrbtree_node_value_replace_impl(node, 0, value, old_value, false);
}

bool xlistrbtree_find_deep_replace(XListRBTree_PT tree, void *key, void *value) {
    XListRBTree_Node_PT node = xlistrbtree_get_impl(tree, (tree ? tree->root : NULL), key);
    return xlistrbtree_node_value_replace_impl(node, 0, value, NULL, true);
}

XSList_PT xlistrbtree_keys_impl(XListRBTree_PT tree, void *low, void *high) {
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
        XListRBTree_Node_PT node = xlistrbtree_get_impl(tree, tree->root, low);
        while (node && (tree->cmp(node->key, high, tree->cl) <= 0)) {
            if (!xslist_push_back_repeat(list, node->key)) {
                xslist_free(&list);
                return NULL;
            }

            node = xlistrbtree_next_node(tree, node);
        }
    }

    return list;
}

XSList_PT xlistrbtree_keys(XListRBTree_PT tree, void *low, void *high) {
    xassert(tree);
    xassert(low);
    xassert(high);

    if (!tree || !low || !high) {
        return NULL;
    }

    return xlistrbtree_keys_impl(tree, low, high);
}

static
void xlistrbtree_free_impl(XListRBTree_PT tree, XListRBTree_Node_PT node, bool deep, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return;
    }

    xlistrbtree_free_impl(tree, node->left, deep, apply, cl);
    xlistrbtree_free_impl(tree, node->right, deep, apply, cl);

    if (deep) {
        XMEM_FREE(node->key);
        xrslist_deep_free(&node->values);
    }
    else {
        xrslist_free(&node->values);
    }
    XMEM_FREE(node);
}

void xlistrbtree_clear(XListRBTree_PT tree) {
    xlistrbtree_free_impl(tree, (tree ? tree->root : NULL), false, NULL, NULL);
    if (tree) {
        tree->root = NULL;
    }
}

void xlistrbtree_deep_clear(XListRBTree_PT tree) {
    xlistrbtree_free_impl(tree, (tree ? tree->root : NULL), true, NULL, NULL);
    if (tree) {
        tree->root = NULL;
    }
}

void xlistrbtree_free(XListRBTree_PT *ptree) {
    if (!ptree || !*ptree) {
        return;
    }

    xlistrbtree_free_impl(*ptree, (*ptree)->root, false, NULL, NULL);
    XMEM_FREE(*ptree);
}

void xlistrbtree_deep_free(XListRBTree_PT *ptree) {
    if (!ptree || !*ptree) {
        return;
    }

    xlistrbtree_free_impl(*ptree, (*ptree)->root, true, NULL, NULL);
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
XListRBTree_Node_PT xlistrbtree_move_red_left(XListRBTree_Node_PT node_N) {
    if (node_N->right && (xlistrbtree_node_color_red(node_N->right->left) || xlistrbtree_node_color_red(node_N->right->right))) {
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
        xassert(xlistrbtree_node_color_red(node_N) && xlistrbtree_node_color_black(node_N->left) && xlistrbtree_node_color_black(node_N->right));
        xlistrbtree_flip_color(node_N);

        if (xlistrbtree_node_color_red(node_N->right->left)) {
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
            node_N->right = xlistrbtree_rotate_right(node_N->right);
        }

        if (xlistrbtree_node_color_red(node_N->right->right)) {
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
            node_N = xlistrbtree_rotate_left(node_N);
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
        xassert(xlistrbtree_node_color_black(node_N) && xlistrbtree_node_color_red(node_N->left) && xlistrbtree_node_color_red(node_N->right));
        xlistrbtree_flip_color(node_N);
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
XListRBTree_Node_PT xlistrbtree_move_red_right(XListRBTree_Node_PT node_N) {
    if (node_N->left && (xlistrbtree_node_color_red(node_N->left->right) || xlistrbtree_node_color_red(node_N->left->left))) {
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
        xassert(xlistrbtree_node_color_red(node_N) && xlistrbtree_node_color_black(node_N->left) && xlistrbtree_node_color_black(node_N->right));
        xlistrbtree_flip_color(node_N);

        if (xlistrbtree_node_color_red(node_N->left->right)) {
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
            node_N->left = xlistrbtree_rotate_left(node_N->left);
        }

        if (xlistrbtree_node_color_red(node_N->left->left)) {
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
            node_N = xlistrbtree_rotate_right(node_N);
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
        xassert(xlistrbtree_node_color_black(node_N) && xlistrbtree_node_color_red(node_N->left) && xlistrbtree_node_color_red(node_N->right));
        xlistrbtree_flip_color(node_N);
    }

    return node_N;
}

static 
XListRBTree_Node_PT xlistrbtree_remove_min_impl(XListRBTree_PT tree, XListRBTree_Node_PT node_N, void **old_key, void **old_value, bool deep) {
    /* if node_N has no left branch, node_N is the mimimum one */
    if (!node_N->left) {
        XListRBTree_Node_PT right = node_N->right;

        if (node_N->values) {
            void* ovalue = xrslist_pop_front(&node_N->values);
            --node_N->node_size;
            --node_N->size;

            if (deep) {
                XMEM_FREE(ovalue);
                if (!node_N->values) {
                    XMEM_FREE(node_N->key);
                }
            }
            else {
                if (old_key) {
                    *old_key = node_N->key;
                }
                if (old_value) {
                    *old_value = ovalue;
                }
            }
        }
        else {
            if (deep) {
                XMEM_FREE(node_N->key);
            }
            else if (old_key) {
                *old_key = node_N->key;
            }
        }

        if (!node_N->values) {
            if (right) {
                right->parent = node_N->parent;
                /* keep the black node numbers not changed */
                if (xlistrbtree_node_color_black(node_N)) {
                    right->color = node_N->color;
                }
            }

            XMEM_FREE(node_N);
            return right;
        }

        return node_N;
    }

    /* node->left has no 2 or 3 keys
    *          |
    *         N(R)
    *       /      \
    *     E(B)      T
    */
    if (xlistrbtree_node_color_black(node_N->left) && xlistrbtree_node_color_black(node_N->left->left) && xlistrbtree_node_color_black(node_N->left->right)) {
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
        if (xlistrbtree_node_color_red(node_N->right)) {
            node_N = xlistrbtree_rotate_left(node_N);
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
            if (node_N->right && (xlistrbtree_node_color_red(node_N->right->left) || xlistrbtree_node_color_red(node_N->right->right))) {
                node_N = xlistrbtree_move_red_left(node_N);
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
                xassert(xlistrbtree_node_color_red(node_N) && xlistrbtree_node_color_black(node_N->left) && xlistrbtree_node_color_black(node_N->right));
                xlistrbtree_flip_color(node_N);
            }
        }
    }

    /* node->left has 2 or 3 keys now */
    node_N->left = xlistrbtree_remove_min_impl(tree, node_N->left, old_key, old_value, deep);

    return xlistrbtree_balance(node_N);
}

void xlistrbtree_remove_min(XListRBTree_PT tree) {
    xassert(tree);

    if (!tree || !tree->root) {
        return ;
    }

    if (xlistrbtree_node_color_black(tree->root->left) && xlistrbtree_node_color_black(tree->root->right)) {
        tree->root->color = xlistrbtree_color_red;
    }

    tree->root = xlistrbtree_remove_min_impl(tree, tree->root, NULL, NULL, false);
    if (tree->root) {
        tree->root->color = xlistrbtree_color_black;
    }
}

void xlistrbtree_remove_save_min(XListRBTree_PT tree, void **key, void **value) {
    xassert(tree);

    if (!tree || !tree->root) {
        return;
    }

    if (xlistrbtree_node_color_black(tree->root->left) && xlistrbtree_node_color_black(tree->root->right)) {
        tree->root->color = xlistrbtree_color_red;
    }

    tree->root = xlistrbtree_remove_min_impl(tree, tree->root, key, value, false);
    if (tree->root) {
        tree->root->color = xlistrbtree_color_black;
    }
}

void xlistrbtree_deep_remove_min(XListRBTree_PT tree) {
    xassert(tree);

    if (!tree || !tree->root) {
        return;
    }

    if (xlistrbtree_node_color_black(tree->root->left) && xlistrbtree_node_color_black(tree->root->right)) {
        tree->root->color = xlistrbtree_color_red;
    }

    tree->root = xlistrbtree_remove_min_impl(tree, tree->root, NULL, NULL, true);
    if (tree->root) {
        tree->root->color = xlistrbtree_color_black;
    }
}

static
XListRBTree_Node_PT xlistrbtree_remove_max_impl(XListRBTree_PT tree, XListRBTree_Node_PT node_N, void **old_key, void **old_value, bool deep) {
    /* if node has no right branch, node is the maximum one */
    if (!node_N->right) {
        XListRBTree_Node_PT left = node_N->left;

        if (node_N->values) {
            void* ovalue = xrslist_pop_front(&node_N->values);
            --node_N->node_size;
            --node_N->size;

            if (deep) {
                XMEM_FREE(ovalue);
                if (!node_N->values) {
                    XMEM_FREE(node_N->key);
                }
            }
            else {
                if (old_key) {
                    *old_key = node_N->key;
                }
                if (old_value) {
                    *old_value = ovalue;
                }
            }
        }
        else {
            if (deep) {
                XMEM_FREE(node_N->key);
            }
            else if (old_key) {
                *old_key = node_N->key;
            }
        }

        if (!node_N->values) {
            if (left) {
                left->parent = node_N->parent;
                /* keep the black node numbers not changed */
                if (xlistrbtree_node_color_black(node_N)) {
                    left->color = node_N->color;
                }
            }

            XMEM_FREE(node_N);
            return left;
        }

        return node_N;
    }

    /* node->right has no 2 or 3 keys
    *          |
    *         N(R)
    *       /      \
    *      C       T(B)
    */
    if (xlistrbtree_node_color_black(node_N->right) && xlistrbtree_node_color_black(node_N->right->left) && xlistrbtree_node_color_black(node_N->right->right)) {
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
        if (xlistrbtree_node_color_red(node_N->left)) {
            node_N = xlistrbtree_rotate_right(node_N);
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
            if (node_N->left && (xlistrbtree_node_color_red(node_N->left->left) || xlistrbtree_node_color_red(node_N->left->right))) {
                node_N = xlistrbtree_move_red_right(node_N);
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
                xassert(xlistrbtree_node_color_red(node_N) && xlistrbtree_node_color_black(node_N->left) && xlistrbtree_node_color_black(node_N->right));
                xlistrbtree_flip_color(node_N);
            }
        }
    }

    /* node->right has 2 or 3 keys now */
    node_N->right = xlistrbtree_remove_max_impl(tree, node_N->right, old_key, old_value, deep);

    return xlistrbtree_balance(node_N);
}

void xlistrbtree_remove_max(XListRBTree_PT tree) {
    xassert(tree);

    if (!tree || !tree->root) {
        return;
    }

    if (xlistrbtree_node_color_black(tree->root->left) && xlistrbtree_node_color_black(tree->root->right)) {
        tree->root->color = xlistrbtree_color_red;
    }

    tree->root = xlistrbtree_remove_max_impl(tree, tree->root, NULL, NULL, false);
    if (tree->root) {
        tree->root->color = xlistrbtree_color_black;
    }
}

void xlistrbtree_remove_save_max(XListRBTree_PT tree, void **key, void **value) {
    xassert(tree);

    if (!tree || !tree->root) {
        return;
    }

    if (xlistrbtree_node_color_black(tree->root->left) && xlistrbtree_node_color_black(tree->root->right)) {
        tree->root->color = xlistrbtree_color_red;
    }

    tree->root = xlistrbtree_remove_max_impl(tree, tree->root, key, value, false);
    if (tree->root) {
        tree->root->color = xlistrbtree_color_black;
    }
}

void xlistrbtree_deep_remove_max(XListRBTree_PT tree) {
    xassert(tree);

    if (!tree || !tree->root) {
        return;
    }

    if (xlistrbtree_node_color_black(tree->root->left) && xlistrbtree_node_color_black(tree->root->right)) {
        tree->root->color = xlistrbtree_color_red;
    }

    tree->root = xlistrbtree_remove_max_impl(tree, tree->root, NULL, NULL, true);
    if (tree->root) {
        tree->root->color = xlistrbtree_color_black;
    }
}

static
XListRBTree_Node_PT xlistrbtree_remove_min_node_impl(XListRBTree_PT tree, XListRBTree_Node_PT node_N, void **old_key, XRSList_PT* old_values, int *size) {
    /* if node has no left branch, node is the mimimum one */
    if (!node_N->left) {
        XListRBTree_Node_PT right = node_N->right;

        *old_key = node_N->key;
        *old_values = node_N->values;
        *size = node_N->node_size;

        if (right) {
            right->parent = node_N->parent;
            /* keep the black node numbers not changed */
            if (xlistrbtree_node_color_black(node_N)) {
                right->color = node_N->color;
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
    if (xlistrbtree_node_color_black(node_N->left) && xlistrbtree_node_color_black(node_N->left->left) && xlistrbtree_node_color_black(node_N->left->right)) {
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
        if (xlistrbtree_node_color_red(node_N->right)) {
            node_N = xlistrbtree_rotate_left(node_N);
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
            if (node_N->right && (xlistrbtree_node_color_red(node_N->right->left) || xlistrbtree_node_color_red(node_N->right->right))) {
                node_N = xlistrbtree_move_red_left(node_N);
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
                xassert(xlistrbtree_node_color_red(node_N) && xlistrbtree_node_color_black(node_N->left) && xlistrbtree_node_color_black(node_N->right));
                xlistrbtree_flip_color(node_N);
            }
        }
    }

    /* node->left has 2 or 3 keys now */
    node_N->left = xlistrbtree_remove_min_node_impl(tree, node_N->left, old_key, old_values, size);

    return xlistrbtree_balance(node_N);
}

/* for_set : "true" only for deep remove the XSet_PT */
static
XListRBTree_Node_PT xlistrbtree_remove_impl(XListRBTree_PT tree, XListRBTree_Node_PT node_N, void *key, void **old_value, bool deep, bool remove_all, bool for_set) {
    /* can't find the node to remove */
    if (!node_N) {
        return NULL;
    }

    {
        int ret = tree->cmp(key, node_N->key, tree->cl);
        if (ret == 0) {
            /* 1. more than one value exist */
            if (node_N->values && node_N->values->next && !remove_all) {
                void* ovalue = xrslist_pop_front(&node_N->values);
                --node_N->node_size;
                --node_N->size;

                if (deep) {
                    XMEM_FREE(ovalue);
                }
                else if (old_value) {
                    *old_value = ovalue;
                }

                return node_N;
            }

            /* 2. no left or right branch */
            if (!node_N->left || !node_N->right) {
                XListRBTree_Node_PT ret_node = node_N->left ? node_N->left : node_N->right;
                if (ret_node) {
                    ret_node->parent = node_N->parent;
                    /* keep the black node numbers not changed */
                    if (xlistrbtree_node_color_black(node_N)) {
                        ret_node->color = node_N->color;
                    }
                }

                if (remove_all) {
                    if (node_N->values) {
                        deep ? xrslist_deep_free(&node_N->values) : xrslist_free(&node_N->values);
                    }
                }
                else {
                    /* one value exist */
                    if (node_N->values) {
                        void* ovalue = xrslist_pop_front(&node_N->values);

                        if (deep) {
                            XMEM_FREE(ovalue);
                        }
                        else if (old_value) {
                            *old_value = ovalue;
                        }
                    }
                }

                if (deep && !for_set) {
                    XMEM_FREE(node_N->key);
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
                if (xlistrbtree_node_color_black(node_N->right) && xlistrbtree_node_color_black(node_N->right->left) && xlistrbtree_node_color_black(node_N->right->right)) {
                    /* node->left is red
                    *          |                       |
                    *         N(R)                    C(R)
                    *       /      \       ->       /      \
                    *     C(R)     T(B)            A       N(R)
                    *     /  \                            /    \
                    *    A    E                          E     T(B)
                    */
                    if (xlistrbtree_node_color_red(node_N->left)) {
                        node_N = xlistrbtree_rotate_right(node_N);
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
                        if (node_N->left && (xlistrbtree_node_color_red(node_N->left->left) || xlistrbtree_node_color_red(node_N->left->right))) {
                            node_N = xlistrbtree_move_red_right(node_N);
                        }
                        else {
                            /* node_N->left is black
                            *
                            *           |                         |
                            *          N(R)                       N
                            *       /       \         -->       /    \
                            *     C(B)      T(B)              C(R)   T(R)
                            */
                            xassert(xlistrbtree_node_color_red(node_N) && xlistrbtree_node_color_black(node_N->left) && xlistrbtree_node_color_black(node_N->right));
                            xlistrbtree_flip_color(node_N);
                        }
                    }
                }

                /* node->right has 2 or 3 keys now */
                if (0 == tree->cmp(key, node_N->key, tree->cl)) {
                    /* delete the min key of node_N->right */
                    int node_size = 0;
                    void *min_key = NULL;
                    XRSList_PT min_values = NULL;
                    node_N->right = xlistrbtree_remove_min_node_impl(tree, node_N->right, &min_key, &min_values, &node_size);

                    if (remove_all) {
                        if (node_N->values) {
                            deep ? xrslist_deep_free(&node_N->values) : xrslist_free(&node_N->values);
                        }
                    }
                    else {
                        /* one value exist */
                        if (node_N->values) {
                            void* ovalue = xrslist_pop_front(&node_N->values);

                            if (deep) {
                                XMEM_FREE(ovalue);
                            }
                            else if (old_value) {
                                *old_value = ovalue;
                            }
                        }
                    }

                    if (deep && !for_set) {
                        XMEM_FREE(node_N->key);
                    }

                    node_N->key = min_key;
                    node_N->values = min_values;
                    node_N->node_size = node_size;
                    --node_N->size;

                    return node_N;
                }
                else {
                    node_N->right = xlistrbtree_remove_impl(tree, node_N->right, key, old_value, deep, remove_all, for_set);
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
            if (xlistrbtree_node_color_black(node_N->left) && xlistrbtree_node_color_black(node_N->left->left) && xlistrbtree_node_color_black(node_N->left->right)) {
                /* node_N->right is red
                *          |                       |
                *         N(R)                    T(R)
                *       /      \       ->       /      \
                *     E(B)     T(R)            N(R)     U
                *              /  \           /   \
                *             S    U        E(B)   S
                */
                if (xlistrbtree_node_color_red(node_N->right)) {
                    node_N = xlistrbtree_rotate_left(node_N);
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
                    if (node_N->right && (xlistrbtree_node_color_red(node_N->right->left) || xlistrbtree_node_color_red(node_N->right->right))) {
                        node_N = xlistrbtree_move_red_left(node_N);
                    }
                    else {
                        /* node_N->right is black
                        *           |                         |
                        *          N(R)                       N
                        *       /       \         -->       /    \
                        *     E(B)       T                E(R)   T(R)
                        */
                        xassert(xlistrbtree_node_color_red(node_N) && xlistrbtree_node_color_black(node_N->left) && xlistrbtree_node_color_black(node_N->right));
                        xlistrbtree_flip_color(node_N);
                    }
                }
            }

            /* node_N->left has 2 or 3 keys now */
            node_N->left = xlistrbtree_remove_impl(tree, node_N->left, key, old_value, deep, remove_all, for_set);
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
            if (xlistrbtree_node_color_black(node_N->right) && xlistrbtree_node_color_black(node_N->right->left) && xlistrbtree_node_color_black(node_N->right->right)) {
                /* node->left is red
                *          |                       |
                *         N(R)                    C(R)
                *       /      \       ->       /      \
                *     C(R)     T(B)            A       N(R)
                *     /  \                            /    \
                *    A    E                          E     T(B)
                */
                if (xlistrbtree_node_color_red(node_N->left)) {
                    node_N = xlistrbtree_rotate_right(node_N);
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
                    if (node_N->left && (xlistrbtree_node_color_red(node_N->left->left) || xlistrbtree_node_color_red(node_N->left->right))) {
                        node_N = xlistrbtree_move_red_right(node_N);
                    }
                    else {
                        /* node_N->left is black
                        *
                        *           |                         |
                        *          N(R)                       N
                        *       /       \         -->       /    \
                        *     C(B)      T(B)              C(R)   T(R)
                        */
                        xassert(xlistrbtree_node_color_red(node_N) && xlistrbtree_node_color_black(node_N->left) && xlistrbtree_node_color_black(node_N->right));
                        xlistrbtree_flip_color(node_N);
                    }
                }
            }

            /* node->right has 2 or 3 keys now */
            node_N->right = xlistrbtree_remove_impl(tree, node_N->right, key, old_value, deep, remove_all, for_set);
        }

        return xlistrbtree_balance(node_N);
    }
}

int xlistrbtree_remove(XListRBTree_PT tree, void *key) {
    if (!tree || !key || !tree->root) {
        return 0;
    }

    {
        int total = tree->root->size;

        if (xlistrbtree_node_color_black(tree->root->left) && xlistrbtree_node_color_black(tree->root->right)) {
            tree->root->color = xlistrbtree_color_red;
        }

        tree->root = xlistrbtree_remove_impl(tree, tree->root, key, NULL, false, false, false);
        if (tree->root) {
            tree->root->color = xlistrbtree_color_black;
        }

        return (total - xlistrbtree_size(tree));
    }
}

int xlistrbtree_remove_all(XListRBTree_PT tree, void *key) {
    if (!tree || !key || !tree->root) {
        return 0;
    }

    {
        int total = tree->root->size;

        if (xlistrbtree_node_color_black(tree->root->left) && xlistrbtree_node_color_black(tree->root->right)) {
            tree->root->color = xlistrbtree_color_red;
        }

        tree->root = xlistrbtree_remove_impl(tree, tree->root, key, NULL, false, true, false);
        if (tree->root) {
            tree->root->color = xlistrbtree_color_black;
        }

        return (total - xlistrbtree_size(tree));
    }
}

int xlistrbtree_remove_save(XListRBTree_PT tree, void *key, void **value) {
    if (!tree || !key || !tree->root) {
        return 0;
    }

    {
        int total = tree->root->size;

        if (xlistrbtree_node_color_black(tree->root->left) && xlistrbtree_node_color_black(tree->root->right)) {
            tree->root->color = xlistrbtree_color_red;
        }

        tree->root = xlistrbtree_remove_impl(tree, tree->root, key, value, false, false, false);
        if (tree->root) {
            tree->root->color = xlistrbtree_color_black;
        }

        return (total - xlistrbtree_size(tree));
    }
}

int xlistrbtree_deep_remove_impl(XListRBTree_PT tree, void *key, bool remove_all, bool for_set) {
    if (!tree || !key || !tree->root) {
        return 0;
    }

    {
        int total = tree->root->size;

        if (xlistrbtree_node_color_black(tree->root->left) && xlistrbtree_node_color_black(tree->root->right)) {
            tree->root->color = xlistrbtree_color_red;
        }

        tree->root = xlistrbtree_remove_impl(tree, tree->root, key, NULL, true, remove_all, for_set);
        if (tree->root) {
            tree->root->color = xlistrbtree_color_black;
        }

        return (total - xlistrbtree_size(tree));
    }
}

int xlistrbtree_deep_remove(XListRBTree_PT tree, void *key) {
    return xlistrbtree_deep_remove_impl(tree, key, false, false);
}

int xlistrbtree_deep_remove_all(XListRBTree_PT tree, void *key) {
    return xlistrbtree_deep_remove_impl(tree, key, true, false);
}

static 
int xlistrbtree_map_preorder_impl(XListRBTree_PT tree, XListRBTree_Node_PT node, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        if (node->values) {
            for (XRSList_PT step = node->values; step; step = step->next) {
                if (apply(node->key, &(step->value), cl)) {
                    count++;
                }
            }
        }
        else {
            if (apply(node->key, NULL, cl)) {
                count++;
            }
        }

        count += xlistrbtree_map_preorder_impl(tree, node->left, apply, cl);
        count += xlistrbtree_map_preorder_impl(tree, node->right, apply, cl);

        return count;
    }
}

int xlistrbtree_map_preorder(XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xlistrbtree_map_preorder_impl(tree, (tree ? tree->root : NULL), apply, cl);
}

static 
int xlistrbtree_map_inorder_impl(XListRBTree_PT tree, XListRBTree_Node_PT node, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        count += xlistrbtree_map_inorder_impl(tree, node->left, apply, cl);

        if (node->values) {
            for (XRSList_PT step = node->values; step; step = step->next) {
                if (apply(node->key, &(step->value), cl)) {
                    count++;
                }
            }
        }
        else {
            if (apply(node->key, NULL, cl)) {
                count++;
            }
        }

        count += xlistrbtree_map_inorder_impl(tree, node->right, apply, cl);

        return count;
    }
}

int xlistrbtree_map_inorder(XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xlistrbtree_map_inorder_impl(tree, (tree ? tree->root : NULL), apply, cl);
}

static
int xlistrbtree_map_postorder_impl(XListRBTree_PT tree, XListRBTree_Node_PT node, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        count += xlistrbtree_map_postorder_impl(tree, node->left, apply, cl);
        count += xlistrbtree_map_postorder_impl(tree, node->right, apply, cl);

        if (node->values) {
            for (XRSList_PT step = node->values; step; step = step->next) {
                if (apply(node->key, &(step->value), cl)) {
                    count++;
                }
            }
        }
        else {
            if (apply(node->key, NULL, cl)) {
                count++;
            }
        }

        return count;
    }
}

int xlistrbtree_map_postorder(XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xlistrbtree_map_postorder_impl(tree, (tree ? tree->root : NULL), apply, cl);
}

int xlistrbtree_map_levelorder(XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
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
                XListRBTree_Node_PT node = (XListRBTree_Node_PT)xfifo_pop(fifo);
                if (node->values) {
                    for (XRSList_PT step = node->values; step; step = step->next) {
                        if (apply(node->key, &(step->value), cl)) {
                            count++;
                        }
                    }
                }
                else {
                    if (apply(node->key, NULL, cl)) {
                        count++;
                    }
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

int xlistrbtree_map_min_to_max_impl(XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    int count = 0;

    XListRBTree_Node_PT node = xlistrbtree_min_impl(tree, tree ? tree->root : NULL);
    while (node) {
        if (node->values) {
            for (XRSList_PT step = node->values; step; step = step->next) {
                if (apply(node->key, &(step->value), cl)) {
                    ++count;
                }
            }
        }
        else {
            if (apply(node->key, NULL, cl)) {
                ++count;
            }
        }

        node = xlistrbtree_next_node(tree, node);
    }

    return count;
}

int xlistrbtree_map_min_to_max(XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xlistrbtree_map_min_to_max_impl(tree, apply, cl);
}

bool xlistrbtree_map_min_to_max_break_if_impl(XListRBTree_PT tree, bool break_true, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    XListRBTree_Node_PT node = xlistrbtree_min_impl(tree, tree ? tree->root : NULL);
    while (node) {
        if (node->values) {
            for (XRSList_PT step = node->values; step; step = step->next) {
                bool ret = apply(node->key, &(step->value), cl);
                if (ret && break_true) {
                    return true;
                }
                if (!ret && !break_true) {
                    return true;
                }
            }
        }
        else {
            bool ret = apply(node->key, NULL, cl);
            if (ret && break_true) {
                return true;
            }
            if (!ret && !break_true) {
                return true;
            }
        }

        node = xlistrbtree_next_node(tree, node);
    }

    return false;
}

bool xlistrbtree_map_min_to_max_break_if_true(XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return false;
    }

    return xlistrbtree_map_min_to_max_break_if_impl(tree, true, apply, cl);
}

bool xlistrbtree_map_min_to_max_break_if_false(XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return true;
    }

    return xlistrbtree_map_min_to_max_break_if_impl(tree, false, apply, cl);
}

int xlistrbtree_map_max_to_min_impl(XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    int count = 0;

    XListRBTree_Node_PT node = xlistrbtree_max_impl(tree, tree ? tree->root : NULL);
    while (node) {
        if (node->values) {
            for (XRSList_PT step = node->values; step; step = step->next) {
                if (apply(node->key, &(step->value), cl)) {
                    ++count;
                }
            }
        }
        else {
            if (apply(node->key, NULL, cl)) {
                ++count;
            }
        }

        node = xlistrbtree_prev_node(tree, node);
    }

    return count;
}

int xlistrbtree_map_max_to_min(XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xlistrbtree_map_max_to_min_impl(tree, apply, cl);
}

bool xlistrbtree_map_max_to_min_break_if_impl(XListRBTree_PT tree, bool break_true, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    XListRBTree_Node_PT node = xlistrbtree_max_impl(tree, tree ? tree->root : NULL);
    while (node) {
        if (node->values) {
            for (XRSList_PT step = node->values; step; step = step->next) {
                bool ret = apply(node->key, &(step->value), cl);
                if (ret && break_true) {
                    return true;
                }
                if (!ret && !break_true) {
                    return true;
                }
            }
        }
        else {
            bool ret = apply(node->key, NULL, cl);
            if (ret && break_true) {
                return true;
            }
            if (!ret && !break_true) {
                return true;
            }
        }

        node = xlistrbtree_prev_node(tree, node);
    }

    return false;
}

bool xlistrbtree_map_max_to_min_break_if_true(XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return false;
    }

    return xlistrbtree_map_max_to_min_break_if_impl(tree, true, apply, cl);
}

bool xlistrbtree_map_max_to_min_break_if_false(XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return true;
    }

    return xlistrbtree_map_max_to_min_break_if_impl(tree, false, apply, cl);
}

int xlistrbtree_scope_map_min_to_max(XListRBTree_PT tree, void *low, void *high, bool (*apply)(void *key, void **value, void *cl), void *cl) {
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
            XListRBTree_Node_PT node = xlistrbtree_get_impl(tree, tree->root, low);
            while (node && (tree->cmp(node->key, high, tree->cl) <= 0)) {
                if (node->values) {
                    for (XRSList_PT step = node->values; step; step = step->next) {
                        if (apply(node->key, &(step->value), cl)) {
                            ++count;
                        }
                    }
                }
                else {
                    if (apply(node->key, NULL, cl)) {
                        ++count;
                    }
                }
                node = xlistrbtree_next_node(tree, node);
            }
        }

        return count;
    }
}

bool xlistrbtree_scope_map_min_to_max_break_if_true(XListRBTree_PT tree, void *low, void *high, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(low);
    xassert(high);
    xassert(apply);

    if (!tree || !(tree->root) || !low || !high || !apply) {
        return false;
    }

    {
        if (0 < tree->cmp(low, high, tree->cl)) {
            void *tmp = low;
            low = high;
            high = tmp;
        }

        {
            XListRBTree_Node_PT node = xlistrbtree_get_impl(tree, tree->root, low);
            while (node && (tree->cmp(node->key, high, tree->cl) <= 0)) {
                if (node->values) {
                    for (XRSList_PT step = node->values; step; step = step->next) {
                        if (apply(node->key, &(step->value), cl)) {
                            return true;
                        }
                    }
                }
                else {
                    if (apply(node->key, NULL, cl)) {
                        return true;
                    }
                }
                node = xlistrbtree_next_node(tree, node);
            }
        }

        return false;
    }
}

bool xlistrbtree_scope_map_min_to_max_break_if_false(XListRBTree_PT tree, void *low, void *high, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(low);
    xassert(high);
    xassert(apply);

    if (!tree || !(tree->root) || !low || !high || !apply) {
        return true;
    }

    {
        if (0 < tree->cmp(low, high, tree->cl)) {
            void *tmp = low;
            low = high;
            high = tmp;
        }

        {
            XListRBTree_Node_PT node = xlistrbtree_get_impl(tree, tree->root, low);
            while (node && (tree->cmp(node->key, high, tree->cl) <= 0)) {
                if (node->values) {
                    for (XRSList_PT step = node->values; step; step = step->next) {
                        if (!apply(node->key, &(step->value), cl)) {
                            return true;
                        }
                    }
                }
                else {
                    if (!apply(node->key, NULL, cl)) {
                        return true;
                    }
                }
                node = xlistrbtree_next_node(tree, node);
            }
        }

        return false;
    }
}

bool xlistrbtree_swap(XListRBTree_PT tree1, XListRBTree_PT tree2) {
    xassert(tree1);
    xassert(tree2);

    if (!tree1 || !tree2) {
        return false;
    }

    {
        {
            XListRBTree_Node_PT root = tree1->root;
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

static
XListRBTree_Node_PT xlistrbtree_merge_repeat_impl(XListRBTree_PT tree, XListRBTree_Node_PT parent, XListRBTree_Node_PT node, void *key, XRSList_PT values, int size, bool *false_found, bool unique) {
    /* reach the leaf node, return the new created new_node */
    if (!node) {
        XRSList_PT nvalues = xrslist_copy(values);
        if (!nvalues) {
            *false_found = true;
            return NULL;
        }

        {
            XListRBTree_Node_PT nnode = xlistrbtree_new_node_with_list(key, nvalues, size, xlistrbtree_color_red);
            if (!nnode) {
                xrslist_free(&nvalues);
                *false_found = true;
            }
            else {
                nnode->parent = parent;
            }
            return nnode;
        }
    }

    {
        int ret = tree->cmp(key, node->key, tree->cl);
        if (ret == 0) {
            if (unique) {
                return node;
            }

            if (values) {
                XRSList_PT nvalues = xrslist_copy(values);
                if (!nvalues) {
                    *false_found = true;
                    return node;
                }
                if (node->values) {
                    xrslist_merge(node->values, &nvalues);
                }
                else {
                    node->values = nvalues;
                }
                node->node_size += size;
                node->size += size;
            }

            return node;
        }

        /* find the branch to insert the new node */
        if (ret < 0) {
            node->left = xlistrbtree_merge_repeat_impl(tree, node, node->left, key, values, size, false_found, unique);
        }
        else {
            node->right = xlistrbtree_merge_repeat_impl(tree, node, node->right, key, values, size, false_found, unique);
        }

        /* make the tree balance again */
        return xlistrbtree_balance(node);
    }
}

bool xlistrbtree_merge_repeat(XListRBTree_PT tree1, XListRBTree_PT *ptree2) {
    if (!tree1 || !ptree2 || !*ptree2 || (tree1->cmp != (*ptree2)->cmp)) {
        return false;
    }

    {
        XListRBTree_Node_PT node = xlistrbtree_min_impl(*ptree2, (*ptree2)->root);

        while (node) {
            bool false_found = false;
            tree1->root = xlistrbtree_merge_repeat_impl(tree1, NULL, tree1->root, node->key, node->values, node->node_size, &false_found, false);
            if (false_found) {
                return false;
            }
            node = xlistrbtree_next_node(*ptree2, node);
        }

        xlistrbtree_free(ptree2);
        return true;
    }
}

bool xlistrbtree_merge_unique(XListRBTree_PT tree1, XListRBTree_PT *ptree2) {
    if (!tree1 || !ptree2 || !*ptree2 || (tree1->cmp != (*ptree2)->cmp)) {
        return false;
    }

    {
        XListRBTree_Node_PT node = xlistrbtree_min_impl(*ptree2, (*ptree2)->root);

        while (node) {
            bool false_found = false;
            tree1->root = xlistrbtree_merge_repeat_impl(tree1, NULL, tree1->root, node->key, node->values, node->node_size, &false_found, true);
            if (false_found) {
                return false;
            }
            node = xlistrbtree_next_node(*ptree2, node);
        }

        xlistrbtree_free(ptree2);
        return true;
    }
}

int xlistrbtree_keys_size(XListRBTree_PT tree, void *low, void *high) {
    xassert(tree);
    xassert(low);
    xassert(high);

    if (!tree || !(tree->root) || !low || !high) {
        return -1;
    }

    if (0 < tree->cmp(low, high, tree->cl)) {
        void *tmp = low;
        low = high;
        high = tmp;
    }

    {
        int count = 0;

        XListRBTree_Node_PT node = xlistrbtree_get_impl(tree, tree->root, low);
        while (node && (tree->cmp(node->key, high, tree->cl) <= 0)) {
            ++count;
            node = xlistrbtree_next_node(tree, node);
        }

        return count;
    }
}

int xlistrbtree_size(XListRBTree_PT tree) {
    return (tree ? (tree->root ? tree->root->size : 0) : 0);
}

bool xlistrbtree_is_empty(XListRBTree_PT tree) {
    return (tree ? (tree->root ? (tree->root->size == 0) : true) : true);
}

static
int xlistrbtree_height_impl(XListRBTree_Node_PT node) {
    if (!node) {
        return 0;
    }

    {
        int u = xlistrbtree_height_impl(node->left);
        int v = xlistrbtree_height_impl(node->right);

        return u < v ? (v + 1) : (u + 1);
    }
}

int xlistrbtree_height(XListRBTree_PT tree) {
    return xlistrbtree_height_impl(tree ? tree->root : NULL);
}

static 
int xlistrbtree_count_black_to_root(XListRBTree_PT tree, XListRBTree_Node_PT node) {
    int count = 0;

    while (node != tree->root) {
        if (xlistrbtree_node_color_black(node)) {
            ++count;
        }
        node = node->parent;
    }

    return count + 1;  /* 1 means tree->root is black */
}

bool xlistrbtree_is_rbtree(XListRBTree_PT tree) {
    if (!tree) {
        return false;
    }

    if (tree->root) {
        /* root is black */
        if (xlistrbtree_node_color_red(tree->root)) {
            return false;
        }
    }
    else {
        /* no root */
        return true;
    }

    {
        int count = 0;

        XListRBTree_Node_PT node = xlistrbtree_min_impl(tree, tree->root);
        int black = xlistrbtree_count_black_to_root(tree, node);

        while (node) {
            count += node->node_size;

            /* node is red, it's children must be black */
            if (xlistrbtree_node_color_red(node) && (xlistrbtree_node_color_red(node->left) || xlistrbtree_node_color_red(node->right)))
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

            /* node->size = node->node_size + node->left->size + node->right->size */
            if (node->size != (node->node_size + (node->left ? node->left->size : 0) + (node->right ? node->right->size : 0))) {
                return false;
            }

            /* the black node number from every leaf node to root must be the same */
            if (!node->left && !node->right && (black != xlistrbtree_count_black_to_root(tree, node))) {
                return false;
            }

            node = xlistrbtree_next_node(tree, node);
        }

        /* total node number is the same with the size saved in root */
        if (count != tree->root->size) {
            return false;
        }
    }

    return true;
}