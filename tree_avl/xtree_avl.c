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
#include "../include/xarith_int.h"
#include "../include/xqueue_fifo.h"
#include "xtree_avl_x.h"

static
XAVLTree_Node_PT xavltree_min_impl(XAVLTree_PT tree, XAVLTree_Node_PT node) {
    if (!node) {
        return NULL;
    }

    while (node->left) {
        node = node->left;
    }

    return node;
}

static
XAVLTree_Node_PT xavltree_max_impl(XAVLTree_PT tree, XAVLTree_Node_PT node) {
    if (!node) {
        return NULL;
    }

    while (node->right) {
        node = node->right;
    }

    return node;
}

static
XAVLTree_Node_PT xavltree_get_impl(XAVLTree_PT tree, XAVLTree_Node_PT node, void *key) {
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
XAVLTree_Node_PT xavltree_prev_node(XAVLTree_PT tree, XAVLTree_Node_PT node) {
    if (node->left) {
        return xavltree_max_impl(tree, node->left);
    }

    {
        /* H's pre node is E :
        *      E
        *       \
        *         R
        *        / \
        *       H   S
        */
        XAVLTree_Node_PT parent = node->parent;
        while (parent && (node == parent->left)) {
            node = parent;
            parent = parent->parent;
        }

        return parent;
    }
}

static
XAVLTree_Node_PT xavltree_next_node(XAVLTree_PT tree, XAVLTree_Node_PT node) {
    if (node->right) {
        return xavltree_min_impl(tree, node->right);
    }

    {
        /* R's next node is S :
        *         S
        *       /   \
        *     E       T
        *      \
        *       R
        */
        XAVLTree_Node_PT parent = node->parent;
        while (parent && (node == parent->right)) {
            node = parent;
            parent = parent->parent;
        }

        return parent;
    }
}

/* xavltree_rotate_left :
*
*      |                   |
*      N                   X
*    /   \       -->     /   \
*   D     X             N     T
*        / \           /  \
*       F   T         D    F
*/
static
XAVLTree_Node_PT xavltree_rotate_left(XAVLTree_Node_PT node_N) {
    XAVLTree_Node_PT x = node_N->right;

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

    x->size = node_N->size;
    node_N->size = 1 + (node_N->left ? node_N->left->size : 0) + (node_N->right ? node_N->right->size : 0);

    node_N->height = 1 + xiarith_max(node_N->left ? node_N->left->height : 0, node_N->right ? node_N->right->height : 0);
    x->height = 1 + xiarith_max(x->left ? x->left->height : 0, x->right ? x->right->height : 0);

    return x;
}

/* xavltree_rotate_right :
*
*         |                 |
*         N                 X
*       /   \     -->     /   \
*      X     T           D     N
*     / \                     /  \
*    D   F                   F    T
*/
static
XAVLTree_Node_PT xavltree_rotate_right(XAVLTree_Node_PT node_N) {
    XAVLTree_Node_PT x = node_N->left;

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

    x->size = node_N->size;
    node_N->size = 1 + (node_N->left ? node_N->left->size : 0) + (node_N->right ? node_N->right->size : 0);

    node_N->height = 1 + xiarith_max(node_N->left ? node_N->left->height : 0, node_N->right ? node_N->right->height : 0);
    x->height = 1 + xiarith_max(x->left ? x->left->height : 0, x->right ? x->right->height : 0);

    return x;
}

static
bool xavltree_balance(XAVLTree_PT tree, XAVLTree_Node_PT node_N) {
    if (!node_N) {
        return true;
    }

    {
        int factor_node_N = 0;
        int factor_node_N_left = 0;
        int factor_node_N_right = 0;

        while (node_N) {
            node_N->size = 1 + (node_N->left ? node_N->left->size : 0) + (node_N->right ? node_N->right->size : 0);
            node_N->height = 1 + xiarith_max(node_N->left ? node_N->left->height : 0, node_N->right ? node_N->right->height : 0);

            factor_node_N = (node_N->left ? node_N->left->height : 0) - (node_N->right ? node_N->right->height : 0);

            /* need to rotate to make the tree balance again */
            if (factor_node_N == 2) {
                factor_node_N_left = (node_N->left->left ? node_N->left->left->height : 0) - (node_N->left->right ? node_N->left->right->height : 0);
                if (factor_node_N_left < 0) {
                    /* 1.1
                    *           |                 |
                    *           N                 N
                    *         /   \     -->     /   \
                    *        D     P           F     P
                    *      /  \              /
                    *     B    F            D
                    *         /            /  \
                    *        E            B    E
                    *
                    *  1.2
                    *           |                 |
                    *           N                 N
                    *         /   \     -->     /   \
                    *        D     P           F     P
                    *      /  \              /  \
                    *     B    F            D    G
                    *           \          /
                    *            G        B
                    */
                    node_N->left = xavltree_rotate_left(node_N->left);
                }

                /* 2.1
                *           |                  |
                *           N                  D
                *         /   \     -->      /  \
                *        D     P            B    N
                *      /  \                /    / \
                *     B    F              A    F   P
                *    /
                *   A
                *
                *  2.2
                *           |                    |
                *           N                    D
                *         /   \     -->       /     \
                *        D     P             B       N
                *      /  \                   \     / \
                *     B    F                   C   F   P
                *      \
                *       C
                */
                node_N = xavltree_rotate_right(node_N);
            }
            else if (factor_node_N == -2) {
                factor_node_N_right = (node_N->right->left ? node_N->right->left->height : 0) - (node_N->right->right ? node_N->right->right->height : 0);
                if (0 < factor_node_N_right) {
                    /* 3.1
                    *           |               |
                    *           N               N
                    *         /   \     -->   /   \
                    *        D     R         D     P
                    *             /  \            /  \
                    *            P    T          O    R
                    *           /                      \
                    *          O                        T
                    *
                    *  3.2
                    *           |               |
                    *           N               N
                    *         /   \     -->   /   \
                    *        D     R         D     P
                    *             /  \              \
                    *            P    T              R
                    *             \                 /  \
                    *              Q               Q    T
                    */
                    node_N->right = xavltree_rotate_right(node_N->right);
                }

                /* 4.1
                *           |                  |
                *           N                  R
                *         /   \     -->      /  \
                *        D     R            N    T
                *             /  \         / \    \
                *            P    T       D   P    W
                *                  \
                *                   W
                *
                *  4.2
                *           |                   |
                *           N                   R
                *         /   \     -->      /     \
                *        D     R            N       T
                *             /  \         / \     /
                *            P    T       D   P   S
                *                /
                *               S
                */
                node_N = xavltree_rotate_left(node_N);
            }

            /* reset the root */
            if (!node_N->parent) {
                tree->root = node_N;
                break;
            }

            node_N = node_N->parent;
        }
    }

    return true;
}

static
XAVLTree_Node_PT xavltree_new_node(XAVLTree_Node_PT parent, void *key, void *value) {
    XAVLTree_Node_PT node = XMEM_CALLOC(1, sizeof(*node));
    if (!node) {
        return NULL;
    }

    node->key = key;
    node->value = value;

    node->height = 1;
    node->size = 1;

    node->parent = parent;
    //node->left = NULL;
    //node->right = NULL;

    return node;
}

static
void xavltree_free_node(XAVLTree_Node_PT node, void **old_key, void **old_value, bool deep) {
    if (deep) {
        XMEM_FREE(node->key);
        XMEM_FREE(node->value);
    }
    else {
        if (old_key) {
            *old_key = node->key;
        }
        if (old_value) {
            *old_value = node->value;
        }
    }
    XMEM_FREE(node);
}

XAVLTree_PT xavltree_new(int (*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(cmp);

    if (!cmp) {
        return NULL;
    }

    {
        XAVLTree_PT tree = XMEM_CALLOC(1, sizeof(*tree));
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
bool xavltree_put_impl(XAVLTree_PT tree, void *key, void *value, bool unique, bool replace, bool deep, void **old_value) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return false;
    }

    /* tree->root is NULL */
    if (!tree->root) {
        tree->root = xavltree_new_node(NULL, key, value);
        return tree->root ? true : false;
    }

    {
        int ret = 0;
        XAVLTree_Node_PT parent = NULL;
        XAVLTree_Node_PT node = tree->root;

        /* find the branch to insert the new key */
        while (node) {
            ret = tree->cmp(key, node->key, tree->cl);
            if (unique && (ret == 0)) {
                return true;
            }
            if (replace && (ret == 0)) {
                if (deep) {
                    XMEM_FREE(node->value);
                }
                else if (old_value) {
                    *old_value = node->value;
                }

                node->value = value;
                return true;
            }

            parent = node;
            node = ret < 0 ? node->left : node->right;
        }

        /* reach the leaf node */
        if (ret < 0) {
            parent->left = xavltree_new_node(parent, key, value);
            if (!parent->left) {
                return false;
            }
        }
        else {
            parent->right = xavltree_new_node(parent, key, value);
            if (!parent->right) {
                return false;
            }
        }

        /* make the tree balance again */
        return xavltree_balance(tree, parent);
    }
}

bool xavltree_put_repeat(XAVLTree_PT tree, void *key, void *value) {
    return xavltree_put_impl(tree, key, value, false, false, false, NULL);
}

bool xavltree_put_unique(XAVLTree_PT tree, void *key, void *value) {
    return xavltree_put_impl(tree, key, value, true, false, false, NULL);
}

bool xavltree_put_replace(XAVLTree_PT tree, void *key, void *value, void **old_value) {
    return xavltree_put_impl(tree, key, value, false, true, false, old_value);
}

bool xavltree_put_deep_replace(XAVLTree_PT tree, void *key, void *value) {
    return xavltree_put_impl(tree, key, value, false, true, true, NULL);
}

void* xavltree_min(XAVLTree_PT tree) {
    XAVLTree_Node_PT node = xavltree_min_impl(tree, (tree ? tree->root : NULL));
    return node ? node->key : NULL;
}

void* xavltree_max(XAVLTree_PT tree) {
    XAVLTree_Node_PT node = xavltree_max_impl(tree, (tree ? tree->root : NULL));
    return node ? node->key : NULL;
}

static
XAVLTree_Node_PT xavltree_floor_impl(XAVLTree_PT tree, XAVLTree_Node_PT node, void *key) {
    if (!key) {
        return NULL;
    }

    {
        XAVLTree_Node_PT result = NULL;

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
void* xavltree_floor(XAVLTree_PT tree, void *key) {
    XAVLTree_Node_PT floor = xavltree_floor_impl(tree, (tree ? tree->root : NULL), key);
    return floor ? floor->key : NULL;
}

static
XAVLTree_Node_PT xavltree_ceiling_impl(XAVLTree_PT tree, XAVLTree_Node_PT node, void *key) {
    if (!key) {
        return NULL;
    }

    {
        XAVLTree_Node_PT result = NULL;

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
void* xavltree_ceiling(XAVLTree_PT tree, void *key) {
    XAVLTree_Node_PT ceiling = xavltree_ceiling_impl(tree, (tree ? tree->root : NULL), key);
    return ceiling ? ceiling->key : NULL;
}

static
XAVLTree_Node_PT xavltree_select_impl(XAVLTree_PT tree, XAVLTree_Node_PT node, int k) {
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
void* xavltree_select(XAVLTree_PT tree, int k) {
    xassert(tree);
    xassert(0 <= k);
    xassert(k < xavltree_size(tree));

    if (!tree || (k < 0) || (xavltree_size(tree) <= k)) {
        return NULL;
    }

    return xavltree_select_impl(tree, tree->root, k)->key;
}

static
int xavltree_rank_impl(XAVLTree_PT tree, XAVLTree_Node_PT node, void *key) {
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
int xavltree_rank(XAVLTree_PT tree, void *key) {
    return xavltree_rank_impl(tree, (tree ? tree->root : NULL), key);
}

void* xavltree_get(XAVLTree_PT tree, void *key) {
    XAVLTree_Node_PT node = xavltree_get_impl(tree, (tree ? tree->root : NULL), key);
    return node ? node->value : NULL;
}

bool xavltree_find(XAVLTree_PT tree, void *key) {
    return xavltree_get_impl(tree, (tree ? tree->root : NULL), key) ? true : false;
}

static
void xavltree_free_impl(XAVLTree_PT tree, XAVLTree_Node_PT node, bool deep, bool(*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return;
    }

    xavltree_free_impl(tree, node->left, deep, apply, cl);
    xavltree_free_impl(tree, node->right, deep, apply, cl);

    if (deep) {
        XMEM_FREE(node->key);
        XMEM_FREE(node->value);
    }
    else if (apply) {
        apply(node->key, &node->value, cl);
    }
    XMEM_FREE(node);
}

void xavltree_clear(XAVLTree_PT tree) {
    xavltree_free_impl(tree, (tree ? tree->root : NULL), false, NULL, NULL);
    if (tree) {
        tree->root = NULL;
    }
}

void xavltree_clear_apply(XAVLTree_PT tree, bool(*apply)(void *key, void **value, void *cl), void *cl) {
    xavltree_free_impl(tree, (tree ? tree->root : NULL), false, apply, cl);
    if (tree) {
        tree->root = NULL;
    }
}

void xavltree_deep_clear(XAVLTree_PT tree) {
    xavltree_free_impl(tree, (tree ? tree->root : NULL), true, NULL, NULL);
    if (tree) {
        tree->root = NULL;
    }
}

void xavltree_free(XAVLTree_PT *ptree) {
    if (!ptree || !*ptree) {
        return;
    }

    xavltree_free_impl(*ptree, (*ptree)->root, false, NULL, NULL);
    XMEM_FREE(*ptree);
}

void xavltree_free_apply(XAVLTree_PT *ptree, bool(*apply)(void *key, void **value, void *cl), void *cl) {
    if (!ptree || !*ptree) {
        return;
    }

    xavltree_free_impl(*ptree, (*ptree)->root, false, apply, cl);
    XMEM_FREE(*ptree);
}

void xavltree_deep_free(XAVLTree_PT *ptree) {
    if (!ptree || !*ptree) {
        return;
    }

    xavltree_free_impl(*ptree, (*ptree)->root, true, NULL, NULL);
    XMEM_FREE(*ptree);
}

static
XAVLTree_Node_PT xavltree_remove_min_impl(XAVLTree_PT tree, XAVLTree_Node_PT node) {
    XAVLTree_Node_PT min = NULL;

    if (!node->left) {
        XAVLTree_Node_PT parent = node->parent;
        min = node;

        if (parent) {
            if (node == parent->left) {
                parent->left = node->right;
            }
            else {
                parent->right = node->right;
            }
            if (node->right) {
                node->right->parent = parent;
            }
            xavltree_balance(tree, parent);
        }
        else {
            /* node is root */
            tree->root = node->right;
            if (node->right) {
                node->right->parent = NULL;
            }
        }
    }
    else {
        XAVLTree_Node_PT parent = NULL;

        min = node;
        while (min->left) {
            min = min->left;
        }

        parent = min->parent;
        parent->left = min->right;
        if (min->right) {
            min->right->parent = parent;
        }

        xavltree_balance(tree, parent);
    }

    return min;
}

void xavltree_remove_min(XAVLTree_PT tree) {
    xassert(tree);

    if (!tree || !tree->root) {
        return;
    }

    {
        XAVLTree_Node_PT min = xavltree_remove_min_impl(tree, tree->root);
        xavltree_free_node(min, NULL, NULL, false);
    }
}

static
XAVLTree_Node_PT xavltree_remove_max_impl(XAVLTree_PT tree, XAVLTree_Node_PT node) {
    XAVLTree_Node_PT max = NULL;

    if (!node->right) {
        XAVLTree_Node_PT parent = node->parent;
        max = node;

        if (parent) {
            if (node == parent->left) {
                parent->left = node->left;
            }
            else {
                parent->right = node->left;
            }
            if (node->left) {
                node->left->parent = parent;
            }
            xavltree_balance(tree, parent);
        }
        else {
            /* node is root */
            tree->root = node->left;
            if (node->left) {
                node->left->parent = NULL;
            }
        }
    }
    else {
        XAVLTree_Node_PT parent = NULL;

        max = node;
        while (max->right) {
            max = max->right;
        }

        parent = max->parent;
        parent->right = max->left;
        if (max->left) {
            max->left->parent = parent;
        }

        xavltree_balance(tree, parent);
    }

    return max;
}

void xavltree_remove_max(XAVLTree_PT tree) {
    xassert(tree);

    if (!tree || !tree->root) {
        return;
    }

    {
        XAVLTree_Node_PT max = xavltree_remove_max_impl(tree, tree->root);
        xavltree_free_node(max, NULL, NULL, false);
    }
}

static 
void xavltree_swap_node_key_value(XAVLTree_Node_PT node1, XAVLTree_Node_PT node2) {
    void *key = node1->key;
    void *value = node1->value;

    node1->key = node2->key;
    node1->value = node2->value;

    node2->key = key;
    node2->value = value;
}

static
int xavltree_free_node_and_value(XAVLTree_Node_PT node, void **old_value, bool deep) {
    if (deep) {
        XMEM_FREE(node->value);
    }
    else {
        if (old_value) {
            *old_value = node->value;
        }
    }
    XMEM_FREE(node);

    return 1;
}

int xavltree_remove_impl(XAVLTree_PT tree, void *key, void **old_value, bool deep) {
    XAVLTree_Node_PT node = xavltree_get_impl(tree, tree->root, key);

    /* key not found */
    if (!node) {
        return 0;
    }

    /* leaf node */
    if (!node->left && !node->right) {
        /* root */
        if (!node->parent) {
            tree->root = NULL;
            return xavltree_free_node_and_value(node, old_value, deep);
        }

        /* non-root */
        if (node == node->parent->left) {
            node->parent->left = NULL;
        }
        else {
            node->parent->right = NULL;
        }
        xavltree_balance(tree, node->parent);

        return xavltree_free_node_and_value(node, old_value, deep);
    }

    /* non-leaf node */
    XAVLTree_Node_PT swap = node->left ? xavltree_remove_max_impl(tree, node->left) : xavltree_remove_min_impl(tree, node->right);
    xavltree_swap_node_key_value(node, swap);
    return xavltree_free_node_and_value(swap, old_value, deep);
}

int xavltree_remove(XAVLTree_PT tree, void *key) {
    if (!tree || !key || !tree->root) {
        return 0;
    }

    return xavltree_remove_impl(tree, key, NULL, false);
}

static
int xavltree_map_preorder_impl(XAVLTree_PT tree, XAVLTree_Node_PT node, bool(*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        if (apply(node->key, &(node->value), cl)) {
            count++;
        }

        count += xavltree_map_preorder_impl(tree, node->left, apply, cl);
        count += xavltree_map_preorder_impl(tree, node->right, apply, cl);

        return count;
    }
}

int xavltree_map_preorder(XAVLTree_PT tree, bool(*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xavltree_map_preorder_impl(tree, (tree ? tree->root : NULL), apply, cl);
}

static
int xavltree_map_inorder_impl(XAVLTree_PT tree, XAVLTree_Node_PT node, bool(*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        count += xavltree_map_inorder_impl(tree, node->left, apply, cl);

        if (apply(node->key, &(node->value), cl)) {
            count++;
        }

        count += xavltree_map_inorder_impl(tree, node->right, apply, cl);

        return count;
    }
}

int xavltree_map_inorder(XAVLTree_PT tree, bool(*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xavltree_map_inorder_impl(tree, (tree ? tree->root : NULL), apply, cl);
}

static
int xavltree_map_postorder_impl(XAVLTree_PT tree, XAVLTree_Node_PT node, bool(*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        count += xavltree_map_postorder_impl(tree, node->left, apply, cl);
        count += xavltree_map_postorder_impl(tree, node->right, apply, cl);

        if (apply(node->key, &(node->value), cl)) {
            count++;
        }

        return count;
    }
}

int xavltree_map_postorder(XAVLTree_PT tree, bool(*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xavltree_map_postorder_impl(tree, (tree ? tree->root : NULL), apply, cl);
}

int xavltree_map_levelorder(XAVLTree_PT tree, bool(*apply)(void *key, void **value, void *cl), void *cl) {
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
                XAVLTree_Node_PT node = (XAVLTree_Node_PT)xfifo_pop(fifo);
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

int xavltree_map_min_to_max_impl(XAVLTree_PT tree, bool(*apply)(void *key, void **value, void *cl), void *cl) {
    int count = 0;

    XAVLTree_Node_PT node = xavltree_min_impl(tree, tree ? tree->root : NULL);
    while (node) {
        if (apply(node->key, &node->value, cl)) {
            ++count;
        }

        node = xavltree_next_node(tree, node);
    }

    return count;
}

bool xavltree_map_min_to_max_break_if_impl(XAVLTree_PT tree, bool break_true, bool(*apply)(void *key, void **value, void *cl), void *cl) {
    XAVLTree_Node_PT node = xavltree_min_impl(tree, tree ? tree->root : NULL);
    while (node) {
        bool ret = apply(node->key, &node->value, cl);
        if (ret && break_true) {
            return true;
        }
        if (!ret && !break_true) {
            return true;
        }

        node = xavltree_next_node(tree, node);
    }

    return false;
}

int xavltree_map_min_to_max(XAVLTree_PT tree, bool(*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xavltree_map_min_to_max_impl(tree, apply, cl);
}

bool xavltree_map_min_to_max_break_if_true(XAVLTree_PT tree, bool(*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return false;
    }

    return xavltree_map_min_to_max_break_if_impl(tree, true, apply, cl);
}

bool xavltree_map_min_to_max_break_if_false(XAVLTree_PT tree, bool(*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return true;
    }

    return xavltree_map_min_to_max_break_if_impl(tree, false, apply, cl);
}

bool xavltree_swap(XAVLTree_PT tree1, XAVLTree_PT tree2) {
    xassert(tree1);
    xassert(tree2);

    if (!tree1 || !tree2) {
        return false;
    }

    {
        {
            XAVLTree_Node_PT root = tree1->root;
            tree1->root = tree2->root;
            tree2->root = root;
        }

        {
            int(*cmp)(void *key1, void *key2, void *cl) = tree1->cmp;
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

int xavltree_size(XAVLTree_PT tree) {
    return ((tree && tree->root) ? tree->root->size : 0);
}

bool xavltree_is_empty(XAVLTree_PT tree) {
    return ((tree && tree->root) ? (tree->root->size == 0) : true);
}

int xavltree_height(XAVLTree_PT tree) {
    return ((tree && tree->root) ? tree->root->height : 0);
}

static
int xavltree_count_height_impl(XAVLTree_Node_PT node) {
    if (!node) {
        return 0;
    }

    {
        int u = xavltree_count_height_impl(node->left);
        int v = xavltree_count_height_impl(node->right);

        return u < v ? (v + 1) : (u + 1);
    }
}

bool xavltree_is_avltree(XAVLTree_PT tree) {
    if (!tree) {
        return false;
    }

    {
        int count = 0;

        XAVLTree_Node_PT node = xavltree_min_impl(tree, tree->root);

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

            /* height difference is only allowed : 1, 0, -1 */
            int diff = (node->left ? node->left->height : 0) - (node->right ? node->right->height : 0);
            if ((diff != 1) && (diff != 0) && (diff != -1)) {
                return false;
            }

            /* height is the correct value compared to the real height */
            if (node->height != xavltree_count_height_impl(node)) {
                return false;
            }

            node = xavltree_next_node(tree, node);
        }

        /* total node number is the same with the size saved in root */
        if (count != tree->root->size) {
            return false;
        }
    }

    return true;
}
