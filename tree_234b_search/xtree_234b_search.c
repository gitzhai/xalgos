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
#include "xtree_234b_search_x.h"

static
T234BSTree_Node_PT t234bstree_get_impl(T234BSTree_PT tree, T234BSTree_Node_PT node, void *key) {
    if (!key) {
        return NULL;
    }

    while (node) {
        int ret1 = tree->cmp(key, node->key1, tree->cl);
        if (ret1 == 0) {
            node->target = 1;
            return node;
        }
        else if (ret1 < 0) {
            node = node->link1;
        }
        else if (node->key2) {
            int ret2 = tree->cmp(key, node->key2, tree->cl);
            if (ret2 == 0) {
                node->target = 2;
                return node;
            }
            else if (ret2 < 0) {
                node = node->link2;
            }
            else if (node->key3) {
                int ret3 = tree->cmp(key, node->key3, tree->cl);
                if (ret3 == 0) {
                    node->target = 3;
                    return node;
                }
                else if (ret3 < 0) {
                    node = node->link3;
                }
                else {
                    node = node->link4;
                }
            }
            else {
                node = node->link3;
            }
        }
        else {
            node = node->link2;
        }
    }

    return NULL;
}

T234BSTree_PT t234bstree_new(int (*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(cmp);

    if (!cmp) {
        return NULL;
    }

    {
        T234BSTree_PT tree = XMEM_CALLOC(1, sizeof(*tree));
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
T234BSTree_Node_PT t234bstree_copy_node(T234BSTree_Node_PT node, T234BSTree_Node_PT nparent, bool *false_found, void *cl) {
    T234BSTree_Node_PT nnode = XMEM_CALLOC(1, sizeof(*nnode));
    if (!nnode) {
        *false_found = true;
        return NULL;
    }

    nnode->key1 = node->key1;
    nnode->value1 = node->value1;

    nnode->key2 = node->key2;
    nnode->value2 = node->value2;

    nnode->key3 = node->key3;
    nnode->value3 = node->value3;

    nnode->size = node->size;

    //nnode->link1 = NULL;
    //nnode->link2 = NULL;
    //nnode->link3 = NULL;
    //nnode->link4 = NULL;
    // target = 0

    return nnode;
}

static
void t234bstree_copy_break_if_false_impl(T234BSTree_PT tree, T234BSTree_Node_PT parent, T234BSTree_Node_PT node, T234BSTree_PT ntree, T234BSTree_Node_PT nparent, bool *false_found, T234BSTree_Node_PT(*apply)(T234BSTree_Node_PT node, T234BSTree_Node_PT nparent, bool *false_found, void *cl), void *cl) {
    if (*false_found || !node) {
        return;
    }

    if (!parent) {
        ntree->root = apply(tree->root, NULL, false_found, cl);
        nparent = ntree->root;
    }
    else {
        if (node == parent->link1) {
            nparent->link1 = apply(node, nparent, false_found, cl);
            nparent = nparent->link1;
        }
        else if (node == parent->link2) {
            nparent->link2 = apply(node, nparent, false_found, cl);
            nparent = nparent->link2;
        }
        else if (node == parent->link3) {
            nparent->link3 = apply(node, nparent, false_found, cl);
            nparent = nparent->link3;
        }
        else {
            nparent->link4 = apply(node, nparent, false_found, cl);
            nparent = nparent->link4;
        }
    }

    if (*false_found) {
        return;
    }

    t234bstree_copy_break_if_false_impl(tree, node, node->link1, ntree, nparent, false_found, apply, cl);
    if (*false_found) {
        return;
    }

    t234bstree_copy_break_if_false_impl(tree, node, node->link2, ntree, nparent, false_found, apply, cl);
    if (*false_found) {
        return;
    }

    t234bstree_copy_break_if_false_impl(tree, node, node->link3, ntree, nparent, false_found, apply, cl);
    if (*false_found) {
        return;
    }

    t234bstree_copy_break_if_false_impl(tree, node, node->link4, ntree, nparent, false_found, apply, cl);
}

T234BSTree_PT t234bstree_copy(T234BSTree_PT tree) {
    xassert(tree);

    if (!tree) {
        return NULL;
    }

    {
        bool false_found = false;

        T234BSTree_PT ntree = t234bstree_new(tree->cmp, tree->cl);
        if (!ntree) {
            return NULL;
        }

        t234bstree_copy_break_if_false_impl(tree, NULL, tree->root, ntree, NULL, &false_found, t234bstree_copy_node, NULL);
        if (false_found) {
            t234bstree_free(&ntree);
            return NULL;
        }

        xassert(t234bstree_size(tree) == t234bstree_size(ntree));

        return ntree;
    }
}

void* t234bstree_get(T234BSTree_PT tree, void *key) {
    T234BSTree_Node_PT node = t234bstree_get_impl(tree, (tree ? tree->root : NULL), key);
    if (node) {
        if (node->target == 1) {
            return node->value1;
        }
        else if (node->target == 2) {
            return node->value2;
        }
        else if (node->target == 3) {
            return node->value3;
        }
    }

    return NULL;
}

bool t234bstree_find(T234BSTree_PT tree, void *key) {
    return t234bstree_get_impl(tree, (tree ? tree->root : NULL), key) ? true : false;
}

static
T234BSTree_Node_PT t234bstree_new_node(void *key, void *value) {
    T234BSTree_Node_PT node = XMEM_CALLOC(1, sizeof(*node));
    if (!node) {
        return NULL;
    }

    node->key1 = key;
    node->value1 = value;
    node->size = 1;

    // node->key2 = NULL;
    // node->value2 = NULL;
    // node->key3 = NULL;
    // node->value3 = NULL;

    // node->link1 = NULL;
    // node->link2 = NULL;
    // node->link3 = NULL;
    // node->link4 = NULL;

    // node->target = 0;

    return node;
}

static
void t234bstree_count_node_size(T234BSTree_PT tree, T234BSTree_Node_PT node) {
    xassert(node);

    if (node->key3) {
        node->size = 3 + (node->link1 ? (node->link1->size + node->link2->size + node->link3->size + node->link4->size) : 0);
    }
    else if (node->key2) {
        node->size = 2 + (node->link1 ? (node->link1->size + node->link2->size + node->link3->size) : 0);
    }
    else {
        node->size = 1 + (node->link1 ? (node->link1->size + node->link2->size) : 0);
    }

    return;
}

/* root is 3 keys node, split it into two layers */
static
bool t234bstree_split_3keys_root(T234BSTree_PT tree) {
    xassert(tree->root->key3);
    if (!(tree->root->key3)) {
        return false;
    }

    {
        T234BSTree_Node_PT nroot = t234bstree_new_node(tree->root->key2, tree->root->value2);
        T234BSTree_Node_PT left = t234bstree_new_node(tree->root->key1, tree->root->value1);
        T234BSTree_Node_PT right = t234bstree_new_node(tree->root->key3, tree->root->value3);
        if (!nroot || !left || !right) {
            XMEM_FREE(nroot);
            XMEM_FREE(left);
            XMEM_FREE(right);
            return false;
        }

        left->link1 = tree->root->link1;
        left->link2 = tree->root->link2;
        left->size = 1 + (left->link1 ? (left->link1->size + left->link2->size) : 0);

        right->link1 = tree->root->link3;
        right->link2 = tree->root->link4;
        right->size = 1 + (right->link1 ? (right->link1->size + right->link2->size) : 0);

        nroot->link1 = left;
        nroot->link2 = right;
        nroot->size = tree->root->size;

        /* release the old root */
        XMEM_FREE(tree->root);

        /* reset the root node */
        tree->root = nroot;
    }

    return true;
}

/* parent is 1 or 2 keys node, child is 3 keys node, move key2 of child into parent */
static
bool t234bstree_split_3keys_child(T234BSTree_PT tree, T234BSTree_Node_PT parent, T234BSTree_Node_PT child) {
    xassert(!parent->key3);
    xassert(child->key3);

    if (parent->key3 || !(child->key3)) {
        return false;
    }

    {
        /* create one new node to split the child */
        T234BSTree_Node_PT new_node = t234bstree_new_node(child->key3, child->value3);
        if (!new_node) {
            return false;
        }

        new_node->link1 = child->link3;
        new_node->link2 = child->link4;
        new_node->size = 1 + (new_node->link1 ? (new_node->link1->size + new_node->link2->size) : 0);

        /* parent is 2 keys node */
        if (parent->key2) {
            /* child is link1 of parent */
            if (child == parent->link1) {
                parent->key3 = parent->key2;
                parent->key2 = parent->key1;
                parent->key1 = child->key2;

                parent->value3 = parent->value2;
                parent->value2 = parent->value1;
                parent->value1 = child->value2;

                parent->link4 = parent->link3;
                parent->link3 = parent->link2;
                parent->link2 = new_node;
            }
            /* child is link2 of parent */
            else if (child == parent->link2) {
                parent->key3 = parent->key2;
                parent->key2 = child->key2;

                parent->value3 = parent->value2;
                parent->value2 = child->value2;

                parent->link4 = parent->link3;
                parent->link3 = new_node;
            }
            /* child is link3 of parent */
            else {
                parent->key3 = child->key2;
                parent->value3 = child->value2;
                parent->link4 = new_node;
            }
        }
        /* parent is 1 keys node */
        else {
            /* child is link1 of parent */
            if (child == parent->link1) {
                parent->key2 = parent->key1;
                parent->key1 = child->key2;

                parent->value2 = parent->value1;
                parent->value1 = child->value2;

                parent->link3 = parent->link2;
                parent->link2 = new_node;
            }
            /* child is link2 of parent */
            else {
                parent->key2 = child->key2;
                parent->value2 = child->value2;
                parent->link3 = new_node;
            }
        }

        /* reset the child */
        child->key2 = NULL;
        child->key3 = NULL;

        child->value2 = NULL;
        child->value3 = NULL;

        child->link3 = NULL;
        child->link4 = NULL;

        child->size = 1 + (child->link1 ? (child->link1->size + child->link2->size) : 0);
    }

    return true;
}

static
bool t234bstree_create_root(T234BSTree_PT tree, void *key, void *value) {
    xassert(!tree->root);
    if (tree->root) {
        return false;
    }

    tree->root = t234bstree_new_node(key, value);

    return (tree->root != NULL);
}

/* node is the leaf node now, it has 1 or 2 keys, insert the (key, value) to node directly */
static
bool t234bstree_node_insert(T234BSTree_PT tree, T234BSTree_Node_PT node, void *key, void *value, bool unique, bool replace, bool deep) {
    xassert(!node->key3);
    if (node->key3) {
        return false;
    }

    int ret1 = tree->cmp(key, node->key1, tree->cl);

    if (ret1 == 0) {
        if (unique) {
            return true;
        }

        if (replace) {
            if (deep) {
                XMEM_FREE(node->value1);
            }
            node->value1 = value;
            return true;
        }
    }

    if (ret1 < 0) {
        if (node->key2) {
            node->key3 = node->key2;
            node->value3 = node->value2;
        }

        node->key2 = node->key1;
        node->value2 = node->value1;

        node->key1 = key;
        node->value1 = value;
    }
    else if (node->key2) {
        int ret2 = tree->cmp(key, node->key2, tree->cl);

        if (ret2 == 0) {
            if (unique) {
                return true;
            }

            if (replace) {
                if (deep) {
                    XMEM_FREE(node->value2);
                }
                node->value2 = value;
                return true;
            }
        }

        if (ret2 < 0) {
            node->key3 = node->key2;
            node->value3 = node->value2;

            node->key2 = key;
            node->value2 = value;
        }
        else {
            node->key3 = key;
            node->value3 = value;
        }
    }
    else {
        node->key2 = key;
        node->value2 = value;
    }

    ++node->size;
    return true;
}

bool t234bstree_put_impl(T234BSTree_PT tree, T234BSTree_Node_PT parent, T234BSTree_Node_PT child, void *key, void *value, bool unique, bool replace, bool deep);

bool t234bstree_put_search(T234BSTree_PT tree, T234BSTree_Node_PT node, void *key, void *value, bool unique, bool replace, bool deep) {
    xassert(node);
    if (!node) {
        return false;
    }

    {
        bool result = true;

        /* search the right position to insert the new (key, value) */
        int ret1 = tree->cmp(key, node->key1, tree->cl);
        if (ret1 == 0) {
            if (unique) {
                return true;
            }

            if (replace) {
                if (deep) {
                    XMEM_FREE(node->value1);
                }
                node->value1 = value;
                return true;
            }
        }

        if (ret1 < 0) {
            result = t234bstree_put_impl(tree, node, node->link1, key, value, unique, replace, deep);
        }
        else if (node->key2) {
            int ret2 = tree->cmp(key, node->key2, tree->cl);
            if (ret2 == 0) {
                if (unique) {
                    return true;
                }

                if (replace) {
                    if (deep) {
                        XMEM_FREE(node->value2);
                    }
                    node->value2 = value;
                    return true;
                }
            }

            if (ret2 < 0) {
                result = t234bstree_put_impl(tree, node, node->link2, key, value, unique, replace, deep);
            }
            else if (node->key3) {
                int ret3 = tree->cmp(key, node->key3, tree->cl);
                if (ret3 == 0) {
                    if (unique) {
                        return true;
                    }

                    if (replace) {
                        if (deep) {
                            XMEM_FREE(node->value3);
                        }
                        node->value3 = value;
                        return true;
                    }
                }

                if (ret3 < 0) {
                    result = t234bstree_put_impl(tree, node, node->link3, key, value, unique, replace, deep);
                }
                else {
                    result = t234bstree_put_impl(tree, node, node->link4, key, value, unique, replace, deep);
                }
            }
            else {
                result = t234bstree_put_impl(tree, node, node->link3, key, value, unique, replace, deep);
            }
        }
        else {
            result = t234bstree_put_impl(tree, node, node->link2, key, value, unique, replace, deep);
        }

        t234bstree_count_node_size(tree, node);

        return result;
    }
}

bool t234bstree_put_impl(T234BSTree_PT tree, T234BSTree_Node_PT parent, T234BSTree_Node_PT child, void *key, void *value, bool unique, bool replace, bool deep) {
    /* search to the end, insert the (key, value) directly now */
    if (!child) {
        /* 1 : parent is NULL, tree is empty */
        if (!parent) {
            return t234bstree_create_root(tree, key, value);
        }

        /* 2 : insert the (key, value) to parent directly since it's 1 or 2 keys node now */
        return t234bstree_node_insert(tree, parent, key, value, unique, replace, deep);
    }

    /* child is 3 keys node, split it and put the (key2, value2) to its parent, the parent must be NULL, 1 or 2 keys node now */
    if (child->key3) {
        /* 1 : parent is NULL, child is root node */
        if (!parent) {
            if (!t234bstree_split_3keys_root(tree)) {
                return false;
            }

            /* continue the put operation */
            return t234bstree_put_search(tree, tree->root, key, value, unique, replace, deep);
        }

        /* 2 : child is 3 keys node, parent is 1 or 2 keys node, put the key2 of child to its parent */
        if (!t234bstree_split_3keys_child(tree, parent, child)) {
            return false;
        }

        /* 3 : search from the parent again */
        return t234bstree_put_search(tree, parent, key, value, unique, replace, deep);
    }
    /* child is 1 or 2 keys node, find the right branch to insert the (key, value) */
    else {
        return t234bstree_put_search(tree, child, key, value, unique, replace, deep);
    }
}

bool t234bstree_put_repeat(T234BSTree_PT tree, void *key, void *value) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return false;
    }

    return t234bstree_put_impl(tree, NULL, tree->root, key, value, false, false, false);
}

bool t234bstree_put_unique(T234BSTree_PT tree, void *key, void *value) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return false;
    }

    return t234bstree_put_impl(tree, NULL, tree->root, key, value, true, false, false);
}

bool t234bstree_put_replace(T234BSTree_PT tree, void *key, void *value) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return false;
    }

    return t234bstree_put_impl(tree, NULL, tree->root, key, value, false, true, false);
}

bool t234bstree_put_deep_replace(T234BSTree_PT tree, void *key, void *value) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return false;
    }

    return t234bstree_put_impl(tree, NULL, tree->root, key, value, false, true, true);
}

static
T234BSTree_Node_PT t234bstree_min_impl(T234BSTree_PT tree, T234BSTree_Node_PT node) {
    if (!node) {
        return NULL;
    }

    while (node->link1) {
        node = node->link1;
    }

    return node;
}

void* t234bstree_min(T234BSTree_PT tree) {
    xassert(tree);

    if (!tree) {
        return NULL;
    }

    {
        T234BSTree_Node_PT node = t234bstree_min_impl(tree, tree->root);
        if (node) {
            return node->key1;
        }
        return NULL;
    }
}

static
T234BSTree_Node_PT t234bstree_max_impl(T234BSTree_PT tree, T234BSTree_Node_PT node) {
    if (!node) {
        return NULL;
    }

    while (node->link1) {
        node = node->link4 ? node->link4 : (node->link3 ? node->link3 : node->link2);
    }

    return node;
}

void* t234bstree_max(T234BSTree_PT tree) {
    xassert(tree);

    if (!tree) {
        return NULL;
    }

    {
        T234BSTree_Node_PT node = t234bstree_max_impl(tree, tree->root);
        if (node) {
            return node->key3 ? node->key3 : (node->key2 ? node->key2 : node->key1);
        }

        return NULL;
    }
}

static
T234BSTree_Node_PT t234bstree_floor_impl(T234BSTree_PT tree, T234BSTree_Node_PT node, void *key) {
    if (!node) {
        return NULL;
    }

    int ret1 = tree->cmp(key, node->key1, tree->cl);
    if (ret1 == 0) {
        node->target = 1;
        return node;
    }
    else if (ret1 < 0) {
        return t234bstree_floor_impl(tree, node->link1, key);
    }
    else if (node->key2) {
        int ret2 = tree->cmp(key, node->key2, tree->cl);
        if (ret2 == 0) {
            node->target = 2;
            return node;
        }
        else if (ret2 < 0) {
            T234BSTree_Node_PT floor = t234bstree_floor_impl(tree, node->link2, key);
            if (floor) {
                return floor;
            }
            else {
                node->target = 1;
                return node;
            }
        }
        else if (node->key3) {
            int ret3 = tree->cmp(key, node->key3, tree->cl);
            if (ret3 == 0) {
                node->target = 3;
                return node;
            }
            else if (ret3 < 0) {
                T234BSTree_Node_PT floor = t234bstree_floor_impl(tree, node->link3, key);
                if (floor) {
                    return floor;
                }
                else {
                    node->target = 2;
                    return node;
                }
            }
            else {
                T234BSTree_Node_PT floor = t234bstree_floor_impl(tree, node->link4, key);
                if (floor) {
                    return floor;
                }
                else {
                    node->target = 3;
                    return node;
                }
            }
        }
        else {
            T234BSTree_Node_PT floor = t234bstree_floor_impl(tree, node->link3, key);
            if (floor) {
                return floor;
            }
            else {
                node->target = 2;
                return node;
            }
        }
    }
    else {
        T234BSTree_Node_PT floor = t234bstree_floor_impl(tree, node->link2, key);
        if (floor) {
            return floor;
        }
        else {
            node->target = 1;
            return node;
        }
    }
}

/* the maximum element which is <= key */
void* t234bstree_floor(T234BSTree_PT tree, void *key) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return NULL;
    }

    T234BSTree_Node_PT floor = t234bstree_floor_impl(tree, tree->root, key);
    if (floor) {
        if (floor->target == 1) {
            return floor->key1;
        }
        else if (floor->target == 2) {
            return floor->key2;
        }
        else {
            return floor->key3;
        }
    }

    return NULL;
}

static
T234BSTree_Node_PT t234bstree_ceiling_impl(T234BSTree_PT tree, T234BSTree_Node_PT node, void *key) {
    if (!node) {
        return NULL;
    }

    int ret1 = tree->cmp(key, node->key1, tree->cl);
    if (ret1 == 0) {
        node->target = 1;
        return node;
    }
    else if (ret1 < 0) {
        T234BSTree_Node_PT ceiling = t234bstree_ceiling_impl(tree, node->link1, key);
        if (ceiling) {
            return ceiling;
        }
        else {
            node->target = 1;
            return node;
        }
    }
    else if (node->key2) {
        int ret2 = tree->cmp(key, node->key2, tree->cl);
        if (ret2 == 0) {
            node->target = 2;
            return node;
        }
        else if (ret2 < 0) {
            T234BSTree_Node_PT ceiling = t234bstree_ceiling_impl(tree, node->link2, key);
            if (ceiling) {
                return ceiling;
            }
            else {
                node->target = 2;
                return node;
            }
        }
        else if (node->key3) {
            int ret3 = tree->cmp(key, node->key3, tree->cl);
            if (ret3 == 0) {
                node->target = 3;
                return node;
            }
            else if (ret3 < 0) {
                T234BSTree_Node_PT ceiling = t234bstree_ceiling_impl(tree, node->link3, key);
                if (ceiling) {
                    return ceiling;
                }
                else {
                    node->target = 3;
                    return node;
                }
            }
            else {
                return t234bstree_ceiling_impl(tree, node->link4, key);
            }
        }
        else {
            return t234bstree_ceiling_impl(tree, node->link3, key);
        }
    }
    else {
        return t234bstree_ceiling_impl(tree, node->link2, key);
    }
}

/* the minimum element which is >= key */
void* t234bstree_ceiling(T234BSTree_PT tree, void *key) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return NULL;
    }

    T234BSTree_Node_PT ceiling = t234bstree_ceiling_impl(tree, tree->root, key);
    if (ceiling) {
        if (ceiling->target == 1) {
            return ceiling->key1;
        }
        else if (ceiling->target == 2) {
            return ceiling->key2;
        }
        else {
            return ceiling->key3;
        }
    }

    return NULL;
}

static
T234BSTree_Node_PT t234bstree_select_impl(T234BSTree_PT tree, T234BSTree_Node_PT node, int k) {
    if (!node) {
        return NULL;
    }

    int t1 = node->link1 ? node->link1->size : 0;
    if (t1 == k) {
        node->target = 1;
        return node;
    }
    else if (t1 < k) {
        int t2 = node->link2 ? node->link2->size : 0;
        int total2 = t1 + t2 + 1;
        if (total2 == k) {
            node->target = 2;
            return node;
        }
        else if (total2 < k) {
            int t3 = node->link3 ? node->link3->size : 0;
            int total3 = t1 + t2 + t3 + 2;
            if (total3 == k) {
                node->target = 3;
                return node;
            }
            else if (total3 < k) {
                return t234bstree_select_impl(tree, node->link4, (k - total3 - 1));
            }
            else {
                return t234bstree_select_impl(tree, node->link3, (k - total2 - 1));
            }
        }
        else {
            return t234bstree_select_impl(tree, node->link2, (k - t1 - 1));
        }
    }
    else {
        return t234bstree_select_impl(tree, node->link1, k);
    }
}

/* element number is : 0, 1, 2, ... */
void* t234bstree_select(T234BSTree_PT tree, int k) {
    xassert(tree);
    xassert(0 <= k);
    xassert(k < t234bstree_size(tree));

    if (!tree || (k < 0) || (t234bstree_size(tree) <= k)) {
        return NULL;
    }

    T234BSTree_Node_PT node = t234bstree_select_impl(tree, tree->root, k);
    xassert(node);
    if (node->target == 1) {
        return node->key1;
    }
    else if (node->target == 2) {
        return node->key2;
    }
    else {
        return node->key3;
    }
}

static
int t234bstree_rank_impl(T234BSTree_PT tree, T234BSTree_Node_PT node, void *key) {
    xassert(node);
    if (!node) {
        return -1;
    }

    {
        int ret1 = tree->cmp(key, node->key1, tree->cl);
        if (ret1 == 0) {
            return node->link1 ? node->link1->size : 0;
        }
        else if (ret1 < 0) {
            return t234bstree_rank_impl(tree, node->link1, key);
        }
        else if (node->key2) {
            int ret2 = tree->cmp(key, node->key2, tree->cl);
            if (ret2 == 0) {
                return 1 + (node->link1 ? (node->link1->size + node->link2->size) : 0);
            }
            else if (ret2 < 0) {
                int rank = t234bstree_rank_impl(tree, node->link2, key);
                if (rank == -1) {
                    return rank;
                }

                return 1 + node->link1->size + rank;
            }
            else if (node->key3) {
                int ret3 = tree->cmp(key, node->key3, tree->cl);
                if (ret3 == 0) {
                    return 2 + (node->link1 ? (node->link1->size + node->link2->size + node->link3->size) : 0);
                }
                else if (ret3 < 0) {
                    int rank = t234bstree_rank_impl(tree, node->link3, key);
                    if (rank == -1) {
                        return rank;
                    }

                    return 2 + node->link1->size + node->link2->size + rank;
                }
                else {
                    int rank = t234bstree_rank_impl(tree, node->link4, key);
                    if (rank == -1) {
                        return rank;
                    }

                    return 3 + node->link1->size + node->link2->size + node->link3->size + rank;
                }
            }
            else {
                int rank = t234bstree_rank_impl(tree, node->link3, key);
                if (rank == -1) {
                    return rank;
                }

                return 2 + node->link1->size + node->link2->size + rank;
            }
        }
        else {
            int rank = t234bstree_rank_impl(tree, node->link2, key);
            if (rank == -1) {
                return rank;
            }

            return 1 + node->link1->size + rank;
        }
    }
}

/* element number of the key : 0, 1, 2, ... */
int t234bstree_rank(T234BSTree_PT tree, void *key) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return -1;
    }

    return t234bstree_rank_impl(tree, tree->root, key);
}

void t234bstree_keys_impl(T234BSTree_PT tree, XFifo_PT fifo, T234BSTree_Node_PT node, void *low, void *high) {
    if (!node) {
        return;
    }

    int retlow1 = tree->cmp(low, node->key1, tree->cl);
    int rethigh1 = tree->cmp(high, node->key1, tree->cl);

    if (retlow1 <= 0) {
        t234bstree_keys_impl(tree, fifo, node->link1, low, high);
    }
    if ((retlow1 <= 0) && (0 <= rethigh1)) {
        xfifo_push(fifo, node->key1);
    }
    if (0 <= rethigh1) {
        if (node->key2) {
            int retlow2 = tree->cmp(low, node->key2, tree->cl);
            int rethigh2 = tree->cmp(high, node->key2, tree->cl);

            if (retlow2 <= 0) {
                t234bstree_keys_impl(tree, fifo, node->link2, low, high);
            }
            if ((retlow2 <= 0) && (0 <= rethigh2)) {
                xfifo_push(fifo, node->key2);
            }
            if (0 <= rethigh2) {
                if (node->key3) {
                    int retlow3 = tree->cmp(low, node->key3, tree->cl);
                    int rethigh3 = tree->cmp(high, node->key3, tree->cl);

                    if (retlow3 <= 0) {
                        t234bstree_keys_impl(tree, fifo, node->link3, low, high);
                    }
                    if ((retlow3 <= 0) && (0 <= rethigh3)) {
                        xfifo_push(fifo, node->key3);
                    }
                    if (0 <= rethigh3) {
                        t234bstree_keys_impl(tree, fifo, node->link4, low, high);
                    }
                }
                else {
                    t234bstree_keys_impl(tree, fifo, node->link3, low, high);
                }
            }
        }
        else {
            t234bstree_keys_impl(tree, fifo, node->link2, low, high);
        }
    }
}

XFifo_PT t234bstree_keys(T234BSTree_PT tree, void *low, void *high) {
    xassert(tree);
    xassert(low);
    xassert(high);
    xassert((tree->cmp(low, high, tree->cl) <= 0));

    if (!tree || !low || !high || (0 < tree->cmp(low, high, tree->cl))) {
        return NULL;
    }

    XFifo_PT fifo = xfifo_new(0);
    if (!fifo) {
        return NULL;
    }

    t234bstree_keys_impl(tree, fifo, tree->root, low, high);

    return fifo;
}

static
void t234bstree_free_impl(T234BSTree_PT tree, T234BSTree_Node_PT node, bool deep) {
    if (!node) {
        return;
    }

    {
        t234bstree_free_impl(tree, node->link1, deep);
        t234bstree_free_impl(tree, node->link2, deep);

        if (deep) {
            XMEM_FREE(node->key1);
            XMEM_FREE(node->value1);
        }

        if (node->key2) {
            t234bstree_free_impl(tree, node->link3, deep);

            if (deep) {
                XMEM_FREE(node->key2);
                XMEM_FREE(node->value2);
            }

            if (node->key3) {
                t234bstree_free_impl(tree, node->link4, deep);

                if (deep) {
                    XMEM_FREE(node->key3);
                    XMEM_FREE(node->value3);
                }
            }
        }

        XMEM_FREE(node);

        return;
    }
}

void t234bstree_free(T234BSTree_PT *ptree) {
    xassert(ptree);
    xassert(*ptree);

    if (!ptree || !*ptree) {
        return;
    }

    t234bstree_free_impl(*ptree, (*ptree)->root, false);
    XMEM_FREE(*ptree);

    return;
}

void t234bstree_deep_free(T234BSTree_PT *ptree) {
    xassert(ptree);
    xassert(*ptree);

    if (!ptree || !*ptree) {
        return;
    }

    t234bstree_free_impl(*ptree, (*ptree)->root, true);
    XMEM_FREE(*ptree);

    return;
}

void t234bstree_clear(T234BSTree_PT tree) {
    xassert(tree);

    if (!tree) {
        return;
    }

    t234bstree_free_impl(tree, tree->root, false);
    tree->root = NULL;

    return;
}

void t234bstree_deep_clear(T234BSTree_PT tree) {
    xassert(tree);

    if (!tree) {
        return;
    }

    t234bstree_free_impl(tree, tree->root, true);
    tree->root = NULL;

    return;
}

static
void t234bstree_move_key_from_link1_to_link2(T234BSTree_PT tree, T234BSTree_Node_PT node) {
    xassert(node->key1);
    xassert(node->link1->key2);
    xassert(!node->link2->key2);

    node->link2->key2 = node->link2->key1;
    node->link2->value2 = node->link2->value1;

    node->link2->key1 = node->key1;
    node->link2->value1 = node->value1;

    node->link2->link3 = node->link2->link2;
    node->link2->link2 = node->link2->link1;

    if (node->link1->key3) {
        node->link2->link1 = node->link1->link4;

        node->key1 = node->link1->key3;
        node->value1 = node->link1->value3;

        node->link1->key3 = NULL;
        node->link1->value3 = NULL;
        node->link1->link4 = NULL;

        t234bstree_count_node_size(tree, node->link1);
    }
    else {
        node->link2->link1 = node->link1->link3;

        node->key1 = node->link1->key2;
        node->value1 = node->link1->value2;

        node->link1->key2 = NULL;
        node->link1->value2 = NULL;
        node->link1->link3 = NULL;

        t234bstree_count_node_size(tree, node->link1);
    }

    t234bstree_count_node_size(tree, node->link2);

    return;
}

static
void t234bstree_move_key_from_link2_to_link1(T234BSTree_PT tree, T234BSTree_Node_PT node) {
    xassert(node->key1);
    xassert(node->link2->key2);
    xassert(!node->link1->key2);

    node->link1->key2 = node->key1;
    node->link1->value2 = node->value1;

    node->link1->link3 = node->link2->link1;

    node->key1 = node->link2->key1;
    node->value1 = node->link2->value1;

    node->link2->key1 = node->link2->key2;
    node->link2->key2 = node->link2->key3;
    node->link2->key3 = NULL;

    node->link2->value1 = node->link2->value2;
    node->link2->value2 = node->link2->value3;
    node->link2->value3 = NULL;

    node->link2->link1 = node->link2->link2;
    node->link2->link2 = node->link2->link3;
    node->link2->link3 = node->link2->link4;
    node->link2->link4 = NULL;

    t234bstree_count_node_size(tree, node->link1);
    t234bstree_count_node_size(tree, node->link2);
    return;
}

static
void t234bstree_move_key_from_link2_to_link3(T234BSTree_PT tree, T234BSTree_Node_PT node) {
    xassert(node->key2);
    xassert(node->link2->key2);
    xassert(!node->link3->key2);

    node->link3->key2 = node->link3->key1;
    node->link3->value2 = node->link3->value1;

    node->link3->key1 = node->key2;
    node->link3->value1 = node->value2;

    node->link3->link3 = node->link3->link2;
    node->link3->link2 = node->link3->link1;

    if (node->link2->key3) {
        node->link3->link1 = node->link2->link4;

        node->key2 = node->link2->key3;
        node->value2 = node->link2->value3;

        node->link2->key3 = NULL;
        node->link2->value3 = NULL;
        node->link2->link4 = NULL;

        t234bstree_count_node_size(tree, node->link2);
    }
    else {
        node->link3->link1 = node->link2->link3;

        node->key2 = node->link2->key2;
        node->value2 = node->link2->value2;

        node->link2->key2 = NULL;
        node->link2->value2 = NULL;
        node->link2->link3 = NULL;

        t234bstree_count_node_size(tree, node->link2);
    }

    t234bstree_count_node_size(tree, node->link3);

    return;
}

static
void t234bstree_move_key_from_link3_to_link2(T234BSTree_PT tree, T234BSTree_Node_PT node) {
    xassert(node->key2);
    xassert(node->link3->key2);
    xassert(!node->link2->key2);

    node->link2->key2 = node->key2;
    node->link2->value2 = node->value2;

    node->link2->link3 = node->link3->link1;

    node->key2 = node->link3->key1;
    node->value2 = node->link3->value1;

    node->link3->key1 = node->link3->key2;
    node->link3->key2 = node->link3->key3;
    node->link3->key3 = NULL;

    node->link3->value1 = node->link3->value2;
    node->link3->value2 = node->link3->value3;
    node->link3->value3 = NULL;

    node->link3->link1 = node->link3->link2;
    node->link3->link2 = node->link3->link3;
    node->link3->link3 = node->link3->link4;
    node->link3->link4 = NULL;

    t234bstree_count_node_size(tree, node->link2);
    t234bstree_count_node_size(tree, node->link3);

    return;
}

static
void t234bstree_move_key_from_link3_to_link4(T234BSTree_PT tree, T234BSTree_Node_PT node) {
    xassert(node->key3);
    xassert(node->link3->key2);
    xassert(!node->link4->key2);

    node->link4->key2 = node->link4->key1;
    node->link4->value2 = node->link4->value1;

    node->link4->key1 = node->key3;
    node->link4->value1 = node->value3;

    node->link4->link3 = node->link4->link2;
    node->link4->link2 = node->link4->link1;

    if (node->link3->key3) {
        node->link4->link1 = node->link3->link4;

        node->key3 = node->link3->key3;
        node->value3 = node->link3->value3;

        node->link3->key3 = NULL;
        node->link3->value3 = NULL;
        node->link3->link4 = NULL;

        t234bstree_count_node_size(tree, node->link3);
    }
    else {
        node->link4->link1 = node->link3->link3;

        node->key3 = node->link3->key2;
        node->value3 = node->link3->value2;

        node->link3->key2 = NULL;
        node->link3->value2 = NULL;
        node->link3->link3 = NULL;

        t234bstree_count_node_size(tree, node->link3);
    }

    t234bstree_count_node_size(tree, node->link4);    

    return;
}

static
void t234bstree_move_key_from_link4_to_link3(T234BSTree_PT tree, T234BSTree_Node_PT node) {
    xassert(node->key3);
    xassert(node->link4->key2);
    xassert(!node->link3->key2);

    node->link3->key2 = node->key3;
    node->link3->value2 = node->value3;

    node->link3->link3 = node->link4->link1;

    node->key3 = node->link4->key1;
    node->value3 = node->link4->value1;

    node->link4->key1 = node->link4->key2;
    node->link4->key2 = node->link4->key3;
    node->link4->key3 = NULL;

    node->link4->value1 = node->link4->value2;
    node->link4->value2 = node->link4->value3;
    node->link4->value3 = NULL;

    node->link4->link1 = node->link4->link2;
    node->link4->link2 = node->link4->link3;
    node->link4->link3 = node->link4->link4;
    node->link4->link4 = NULL;

    t234bstree_count_node_size(tree, node->link3);
    t234bstree_count_node_size(tree, node->link4);

    return;
}

static
void t234bstree_merge_left_key_links(T234BSTree_PT tree, T234BSTree_Node_PT node) {
    /* this scenarios is just for root node */
    if (!node->key2) {
        xassert(node == tree->root);

        T234BSTree_Node_PT left  = node->link1;
        T234BSTree_Node_PT right = node->link2;

        /* node's link1 and link2 are all 1 key node, merge then together */
        xassert(!left->key2);
        xassert(!right->key2);

        node->key2 = node->key1;
        node->value2 = node->value1;
        node->key1 = left->key1;
        node->value1 = left->value1;
        node->key3 = right->key1;
        node->value3 = right->value3;

        node->link1 = left->link1;
        node->link2 = left->link2;
        node->link3 = right->link1;
        node->link4 = right->link2;

        XMEM_FREE(left);
        XMEM_FREE(right);

        return;
    }

    /* node is 2 or 3 keys node */
    xassert(node->key2);

    /* node's link1 and link2 are all 1 key node, merge then together */
    xassert(!node->link1->key2);
    xassert(!node->link2->key2);

    node->link1->key2 = node->key1;
    node->link1->value2 = node->value1;
    node->link1->key3 = node->link2->key1;
    node->link1->value3 = node->link2->value1;
    node->link1->link3 = node->link2->link1;
    node->link1->link4 = node->link2->link2;

    XMEM_FREE(node->link2);

    node->key1 = node->key2;
    node->value1 = node->value2;
    node->key2 = node->key3;
    node->value2 = node->value3;
    node->key3 = NULL;
    node->value3 = NULL;
    node->link2 = node->link3;
    node->link3 = node->link4;
    node->link4 = NULL;

    t234bstree_count_node_size(tree, node->link1);

    return;
}

static
void t234bstree_merge_middle_key_links(T234BSTree_PT tree, T234BSTree_Node_PT node) {
    /* node is 2 or 3 keys node */
    xassert(node->key2);

    /* node's link2 and link3 are all 1 key node, merge then together */
    xassert(!node->link2->key2);
    xassert(!node->link3->key2);

    node->link2->key2 = node->key2;
    node->link2->value2 = node->value2;
    node->link2->key3 = node->link3->key1;
    node->link2->value3 = node->link3->value1;
    node->link2->link3 = node->link3->link1;
    node->link2->link4 = node->link3->link2;

    XMEM_FREE(node->link3);

    node->key2 = node->key3;
    node->key3 = NULL;
    node->link3 = node->link4;
    node->link4 = NULL;

    t234bstree_count_node_size(tree, node->link2);

    return;
}

static
void t234bstree_merge_right_key_links(T234BSTree_PT tree, T234BSTree_Node_PT node) {
    /* node is 3 keys node */
    xassert(node->key3);

    /* node's link3 and link4 are all 1 key node, merge then together */
    xassert(!node->link3->key2);
    xassert(!node->link4->key2);

    node->link3->key2 = node->key3;
    node->link3->value2 = node->value3;
    node->link3->key3 = node->link4->key1;
    node->link3->value3 = node->link4->value1;
    node->link3->link3 = node->link4->link1;
    node->link3->link4 = node->link4->link2;

    XMEM_FREE(node->link4);

    node->key3 = NULL;
    node->link4 = NULL;

    t234bstree_count_node_size(tree, node->link3);

    return;
}

static
void t234bstree_merge_root_min(T234BSTree_PT tree) {
    /* empty tree, or root has no children, or root has at least 2 keys, or root's link1 has at least 2 keys */
    if (!tree->root || !tree->root->link1 || tree->root->key2 || tree->root->link1->key2) {
        return;
    }

    /* root is 1 key node and root's link1 child is 1 key node too */
    {
        T234BSTree_Node_PT child = tree->root;

        xassert(!child->key2);
        xassert(!child->link1->key2);

        /* root's link1 brother link2 has at least 2 keys, borrow 1 key from it */
        if (child->link2->key2) {
            t234bstree_move_key_from_link2_to_link1(tree, child);
        }
        /* root's link1 brother has only 1 key, merge them together */
        else {
            t234bstree_merge_left_key_links(tree, child);
        }
    }

    return;
}

/* Note :
 *   this function will decrease 1 for all parent node (child is not NULL)
 *   since the result pmin will be used to remove min key from it.
 */
static
void t234bstree_merge_to_min(T234BSTree_PT tree, T234BSTree_Node_PT parent, T234BSTree_Node_PT child, T234BSTree_Node_PT *pmin) {
    /* child is the root node */
    if (!parent) {
        /* empty tree, or root has no children */
        if (!child || !child->link1) {
            if (pmin) {
                *pmin = child;
            }
            return;
        }

        /* merge the root if needed */
        t234bstree_merge_root_min(tree);

        /* root has no children after merge */
        if (!child->link1) {
            if (pmin) {
                *pmin = child;
            }
            return;
        }

        /* continue to merge the root's link1 */
        if (child->link1->key2) {
            t234bstree_merge_to_min(tree, child->link1, child->link1->link1, pmin);
        }
        else {
            t234bstree_merge_to_min(tree, child, child->link1, pmin);
        }

        t234bstree_count_node_size(tree, child);
        return;
    }

    /* to the end now, parent must has at least 2 keys */
    if (!child) {
        xassert(parent->key2);

        if (pmin) {
            *pmin = parent;
        }

        return;
    }

    /* parent is 2 or 3 keys node */
    xassert(parent->key2);
    xassert(child == parent->link1);

    /* child is 1 key node, get one key from parent */
    if (!child->key2) {
        /* brother has at least 2 keys, borrow 1 key from it */
        if (parent->link2->key2) {
            t234bstree_move_key_from_link2_to_link1(tree, parent);
        }
        /* brother has only 1 key, merge them together */
        else {
            t234bstree_merge_left_key_links(tree, parent);
        }
    }

    /* Note :
    *    This is special processing for later (key, value) remove action,
    *    just in order to increase efficiency ( bad design ) !
    *    If we don't do it here, we will have to count the size again
    *    after the remove action.
    */
    parent->size -= 1;

    /* child is 2 or 3 keys node now */
    xassert(child->key2);

    t234bstree_merge_to_min(tree, child, child->link1, pmin);

    return;
}

static
void t234bstree_merge_root_max(T234BSTree_PT tree) {
    /* empty tree, or root has no children, or root has at least 2 keys */
    if (!tree->root || !tree->root->link1 || tree->root->key2) {
        return;
    }

    /* root's link2 has at least 2 keys */
    if (tree->root->link2->key2) {
        return;
    }

    /* root is 1 key node and root's link2 is 1 key node too */
    {
        T234BSTree_Node_PT child = tree->root;

        xassert(!child->key2);
        xassert(!child->link2->key2);

        /* root's link2 brother link1 has at least 2 keys, borrow 1 key from it */
        if (child->link1->key2) {
            t234bstree_move_key_from_link1_to_link2(tree, child);
        }
        /* root's link2 brother link1 has only 1 key, merge them together */
        else {
            t234bstree_merge_left_key_links(tree, child);
        }
    }

    return;
}

/* Note :
 *   this function will decrease 1 for all parent node (child is not NULL)
 *   since the result pmax will be used to remove max key from it.
 */
static
void t234bstree_merge_to_max(T234BSTree_PT tree, T234BSTree_Node_PT parent, T234BSTree_Node_PT child, T234BSTree_Node_PT *pmax) {
    /* child is the root node */
    if (!parent) {
        /* empty tree, or root has no children */
        if (!child || !child->link1) {
            if (pmax) {
                *pmax = child;
            }
            return;
        }

        /* merge the root if needed */
        t234bstree_merge_root_max(tree);

        /* root has no children */
        if (!child->link1) {
            if (pmax) {
                *pmax = child;
            }
            return;
        }

        /* continue to merge the root's link2 or link3 or link4 */
        if (child->link4) {
            /* child's link3 has more than one keys, ingore the child itself, search it's link direcly */
            if (child->link4->key2) {
                t234bstree_merge_to_max(tree, child->link4, (child->link4->link4 ? child->link4->link4 : (child->link4->link3 ? child->link4->link3 : child->link4->link2)), pmax);
            }
            else {
                t234bstree_merge_to_max(tree, child, child->link4, pmax);
            }
        }
        else if (child->link3) {
            /* child's link3 has more than one keys, ingore the child itself, search it's link direcly */
            if (child->link3->key2) {
                t234bstree_merge_to_max(tree, child->link3, (child->link3->link4 ? child->link3->link4 : (child->link3->link3 ? child->link3->link3 : child->link3->link2)), pmax);
            }
            else {
                t234bstree_merge_to_max(tree, child, child->link3, pmax);
            }
        }
        else {
            /* child's link2 has more than one keys, ingore the child itself, search it's link direcly */
            if (child->link2->key2) {
                t234bstree_merge_to_max(tree, child->link2, (child->link2->link4 ? child->link2->link4 : (child->link2->link3 ? child->link2->link3 : child->link2->link2)), pmax);
            }
            else {
                t234bstree_merge_to_max(tree, child, child->link2, pmax);
            }
        }

        t234bstree_count_node_size(tree, child);
        return;
    }

    /* merge to the end now, parent must has at least 2 keys */
    if (!child) {
        xassert(parent->key2);

        if (pmax) {
            *pmax = parent;
        }
        return;
    }

    /* parent is 2 or 3 keys node */
    xassert(parent->key2);
    xassert(child == (parent->link4 ? parent->link4 : (parent->link3 ? parent->link3 : parent->link2)));

    /* child is 1 key node, get one key from parent */
    if (!child->key2) {
        /* parent is 3 keys node */
        if (parent->key3) {
            /* brother has at least 2 keys, borrow 1 key from it */
            if (parent->link3->key2) {
                t234bstree_move_key_from_link3_to_link4(tree, parent);
            }
            /* brother has only 1 key, merge them together */
            else {
                t234bstree_merge_right_key_links(tree, parent);
                child = parent->link3;
            }
        }
        /* parent is 2 keys node */
        else {
            /* brother has at least 2 keys, borrow 1 key from it */
            if (parent->link2->key2) {
                t234bstree_move_key_from_link2_to_link3(tree, parent);
            }
            /* brother has only 1 key, merge them together */
            else {
                t234bstree_merge_middle_key_links(tree, parent);
                child = parent->link2;
            }
        }
    }

    /* Note : 
     *    This is special processing for later (key, value) remove action, 
     *    just in order to increase efficiency ( bad design ) !
     *    If we don't do it here, we will have to count the size again 
     *    after the remove action.
     */
    parent->size -= 1;

    /* child is 2 or 3 keys node now */
    xassert(child->key2);

    t234bstree_merge_to_max(tree, child, (child->link4 ? child->link4 : (child->link3 ? child->link3 : child->link2)), pmax);

    return;
}

/* node has 2 or 3 keys, remove the (key, value) from node directly */
static
void t234bstree_node_remove_min(T234BSTree_PT tree, T234BSTree_Node_PT node, void **key, void **value, bool deep) {
    if (!node) {
        return;
    }

    xassert(node->key2);

    if (deep) {
        XMEM_FREE(node->key1);
        XMEM_FREE(node->value1);
    }
    else if (key) {
        *key = node->key1;
        if (value) {
            *value = node->value1;
        }
    }

    node->key1 = node->key2;
    node->key2 = node->key3;
    node->key3 = NULL;

    node->value1 = node->value2;
    node->value2 = node->value3;
    node->value3 = NULL;

    --node->size;

    return;
}

/* node has 2 or 3 keys, remove the max (key, value) from node directly */
static
void t234bstree_node_remove_max(T234BSTree_PT tree, T234BSTree_Node_PT node, void **key, void **value, bool deep) {
    if (!node) {
        return;
    }

    xassert(node->key2);

    if (node->key3) {
        if (deep) {
            XMEM_FREE(node->key3);
            XMEM_FREE(node->value3);
        }
        else if (key) {
            *key = node->key3;
            *value = node->value3;
        }

        node->key3 = NULL;
        node->value3 = NULL;
    }
    else {
        if (deep) {
            XMEM_FREE(node->key2);
            XMEM_FREE(node->value2);
        }
        else if (key) {
            *key = node->key2;
            *value = node->value2;
        }

        node->key2 = NULL;
        node->value2 = NULL;
    }

    --node->size;

    return;
}

void t234bstree_remove_min(T234BSTree_PT tree) {
    xassert(tree);

    if (!tree) {
        return;
    }

    T234BSTree_Node_PT min = NULL;
    t234bstree_merge_to_min(tree, NULL, tree->root, &min);
    xassert(min);
    t234bstree_node_remove_min(tree, min, NULL, NULL, false);

    return;
}

void t234bstree_remove_max(T234BSTree_PT tree) {
    xassert(tree);

    if (!tree) {
        return;
    }

    T234BSTree_Node_PT max = NULL;
    t234bstree_merge_to_max(tree, NULL, tree->root, &max);
    xassert(max);
    t234bstree_node_remove_max(tree, max, NULL, NULL, false);

    return;
}

static
bool t234bstree_node_remove_key(T234BSTree_PT tree, T234BSTree_Node_PT node, void *key, void **value, bool deep) {
    xassert(node->key1);
    /* node has no children */
    xassert(!node->link1);

    if (tree->cmp(key, node->key1, tree->cl) == 0) {
        if (deep) {
            XMEM_FREE(node->key1);
            XMEM_FREE(node->value1);
        }
        else if (value) {
            *value = node->value1;
        }

        node->key1 = node->key2;
        node->key2 = node->key3;
        node->key3 = NULL;
        node->value1 = node->value2;
        node->value2 = node->value3;
        node->value3 = NULL;

        --node->size;
        return true;
    }
    else if (node->key2 && (tree->cmp(key, node->key2, tree->cl) == 0)) {
        if (deep) {
            XMEM_FREE(node->key2);
            XMEM_FREE(node->value2);
        }
        else if (value) {
            *value = node->value2;
        }

        node->key2 = node->key3;
        node->key3 = NULL;
        node->value2 = node->value3;
        node->value3 = NULL;

        --node->size;
        return true;
    }
    else if (node->key3 && (tree->cmp(key, node->key3, tree->cl) == 0)) {
        if (deep) {
            XMEM_FREE(node->key3);
            XMEM_FREE(node->value3);
        }
        else if (value) {
            *value = node->value3;
        }

        node->key3 = NULL;
        node->value3 = NULL;

        --node->size;
        return true;
    }

    return false;
}

static
void t234bstree_remove_search(T234BSTree_PT tree, T234BSTree_Node_PT node, void *key, void **value, bool deep) {
    /* node has no children */
    if (!node->link1) {
        t234bstree_node_remove_key(tree, node, key, value, deep);
        return;
    }

    int ret1 = tree->cmp(key, node->key1, tree->cl);
    if (ret1 == 0) {
        /* link1 has 2 or 3 keys */
        if (node->link1->key2) {
            T234BSTree_Node_PT max = NULL;
            t234bstree_merge_to_max(tree, node->link1, (node->link1->link4 ? node->link1->link4 : node->link1->link3), &max);
            /* use the (max_key, max_value) to replace the (key, value) */
            if (deep) {
                XMEM_FREE(node->key1);
                XMEM_FREE(node->value1);
            }
            else if (value) {
                *value = node->value1;
            }

            if (max->key3) {
                node->key1 = max->key3;
                node->value1 = max->value3;
            }
            else {
                node->key1 = max->key2;
                node->value1 = max->value2;
            }

            /* delete the (max_key, max_value) now */
            t234bstree_node_remove_max(tree, max, NULL, NULL, false);
            return;
        }
        /* link2 has 2 or 3 keys */
        else if (node->link2->key2) {
            T234BSTree_Node_PT min = NULL;
            t234bstree_merge_to_min(tree, node->link2, node->link2->link1, &min);
            /* use the (max_key, max_value) to replace the (key, value) */
            if (deep) {
                XMEM_FREE(node->key1);
                XMEM_FREE(node->value1);
            }
            else if (value) {
                *value = node->value1;
            }

            node->key1 = min->key1;
            node->value1 = min->value1;

            /* delete the (min_key, min_value) now */
            t234bstree_node_remove_min(tree, min, NULL, NULL, false);
            return;
        }

        /* link1 and link2 all have 1 key */
        t234bstree_merge_left_key_links(tree, node);
        if (node == tree->root) {
            t234bstree_remove_search(tree, node, key, value, deep);
        }
        else {
            t234bstree_remove_search(tree, node->link1, key, value, deep);
        }

        t234bstree_count_node_size(tree, node);
        return;
    }
    else if (ret1 < 0) {
        /* link1 2 or 3 keys */
        if (node->link1->key2) {
            t234bstree_remove_search(tree, node->link1, key, value, deep);
            t234bstree_count_node_size(tree, node);
            return;
        }
        /* link2 2 or 3 keys */
        else if (node->link2->key2) {
            t234bstree_move_key_from_link2_to_link1(tree, node);
            t234bstree_remove_search(tree, node->link1, key, value, deep);
            t234bstree_count_node_size(tree, node);
            return;
        }

        /* link1 and link2 all have 1 key */
         t234bstree_merge_left_key_links(tree, node);
        if (node == tree->root) {
            t234bstree_remove_search(tree, node, key, value, deep);
        }
        else {
            t234bstree_remove_search(tree, node->link1, key, value, deep);
        }
        t234bstree_count_node_size(tree, node);

        return;
    }
    else if (node->key2) {
        int ret2 = tree->cmp(key, node->key2, tree->cl);
        if (ret2 == 0) {
            /* link2 has 2 or 3 keys */
            if (node->link2->key2) {
                T234BSTree_Node_PT max = NULL;
                t234bstree_merge_to_max(tree, node->link2, (node->link2->link4 ? node->link2->link4 : node->link2->link3), &max);
                /* use the (max_key, max_value) to replace the (key, value) */
                if (deep) {
                    XMEM_FREE(node->key2);
                    XMEM_FREE(node->value2);
                }
                else if (value) {
                    *value = node->value2;
                }

                if (max->key3) {
                    node->key2 = max->key3;
                    node->value2 = max->value3;
                }
                else {
                    node->key2 = max->key2;
                    node->value2 = max->value2;
                }

                /* delete the (max_key, max_value) now */
                t234bstree_node_remove_max(tree, max, NULL, NULL, false);
                return;
            }
            /* link3 has 2 or 3 keys */
            else if (node->link3->key2) {
                T234BSTree_Node_PT min = NULL;
                t234bstree_merge_to_min(tree, node->link3, node->link3->link1, &min);
                /* use the (max_key, max_value) to replace the (key, value) */
                if (deep) {
                    XMEM_FREE(node->key2);
                    XMEM_FREE(node->value2);
                }
                else if (value) {
                    *value = node->value2;
                }
                node->key2 = min->key1;
                node->value2 = min->value1;

                /* delete the (min_key, min_value) now */
                t234bstree_node_remove_min(tree, min, NULL, NULL, false);
                return;
            }

            /* link2 and link3 all have only 1 key */
            t234bstree_merge_middle_key_links(tree, node);
            t234bstree_remove_search(tree, node->link2, key, value, deep);
            t234bstree_count_node_size(tree, node);
            return;
        }
        else if (ret2 < 0) {
            /* link2 has 2 or 3 keys */
            if (node->link2->key2) {
                t234bstree_remove_search(tree, node->link2, key, value, deep);
                t234bstree_count_node_size(tree, node);
                return;
            }
            /* link3 has 2 or 3 keys */
            else if (node->link3->key2) {
                t234bstree_move_key_from_link3_to_link2(tree, node);
                t234bstree_remove_search(tree, node->link2, key, value, deep);
                t234bstree_count_node_size(tree, node);
                return;
            }

            /* link2 and link3 all have only 1 key */
            t234bstree_merge_middle_key_links(tree, node);
            t234bstree_remove_search(tree, node->link2, key, value, deep);
            t234bstree_count_node_size(tree, node);
            return;
        }
        else if (node->key3) {
            int ret3 = tree->cmp(key, node->key3, tree->cl);
            if (ret3 == 0) {
                /* link3 has 2 or 3 keys */
                if (node->link3->key2) {
                    T234BSTree_Node_PT max = NULL;
                    t234bstree_merge_to_max(tree, node->link3, (node->link3->link4 ? node->link3->link4 : node->link3->link3), &max);
                    /* use the (max_key, max_value) to replace the (key, value) */
                    if (deep) {
                        XMEM_FREE(node->key3);
                        XMEM_FREE(node->value3);
                    }
                    else if (value) {
                        *value = node->value3;
                    }

                    if (max->key3) {
                        node->key3 = max->key3;
                        node->value3 = max->value3;
                    }
                    else {
                        node->key3 = max->key2;
                        node->value3 = max->value2;
                    }

                    /* delete the (max_key, max_value) now */
                    t234bstree_node_remove_max(tree, max, NULL, NULL, false);
                    return;
                }
                /* link4 has 2 or 3 keys */
                else if (node->link4->key2) {
                    T234BSTree_Node_PT min = NULL;
                    t234bstree_merge_to_min(tree, node->link4, node->link4->link1, &min);
                    /* use the (max_key, max_value) to replace the (key, value) */
                    if (deep) {
                        XMEM_FREE(node->key3);
                        XMEM_FREE(node->value3);
                    }
                    else if (value) {
                        *value = node->value3;
                    }

                    node->key3 = min->key1;
                    node->value3 = min->value1;

                    /* delete the (min_key, min_value) now */
                    t234bstree_node_remove_min(tree, min, NULL, NULL, false);
                    return;
                }

                /* link3 and link4 all have only 1 key */
                t234bstree_merge_right_key_links(tree, node);
                t234bstree_remove_search(tree, node->link3, key, value, deep);
                t234bstree_count_node_size(tree, node);
                return;
            }
            else if (ret3 < 0) {
                /* link3 has 2 or 3 keys */
                if (node->link3->key2) {
                    t234bstree_remove_search(tree, node->link3, key, value, deep);
                    t234bstree_count_node_size(tree, node);
                    return;
                }
                /* link4 has 2 or 3 keys */
                else if (node->link4->key2) {
                    t234bstree_move_key_from_link4_to_link3(tree, node);
                    t234bstree_remove_search(tree, node->link3, key, value, deep);
                    t234bstree_count_node_size(tree, node);
                    return;
                }

                /* link3 and link4 all have only 1 key */
                t234bstree_merge_right_key_links(tree, node);
                t234bstree_remove_search(tree, node->link3, key, value, deep);
                t234bstree_count_node_size(tree, node);
                return;
            }
            else {
                /* link4 has 2 or 3 keys */
                if (node->link4->key2) {
                    t234bstree_remove_search(tree, node->link4, key, value, deep);
                    t234bstree_count_node_size(tree, node);
                    return;
                }
                /* link3 has 2 or 3 keys */
                else if (node->link3->key2) {
                    t234bstree_move_key_from_link3_to_link4(tree, node);
                    t234bstree_remove_search(tree, node->link4, key, value, deep);
                    t234bstree_count_node_size(tree, node);
                    return;
                }

                /* link3 and link4 all have only 1 key */
                t234bstree_merge_right_key_links(tree, node);
                t234bstree_remove_search(tree, node->link3, key, value, deep);
                t234bstree_count_node_size(tree, node);
                return;
            }
        }
        /* node has no key3 exist */
        else {
            /* link3 has 2 or 3 keys */
            if (node->link3->key2) {
                t234bstree_remove_search(tree, node->link3, key, value, deep);
                t234bstree_count_node_size(tree, node);
                return;
            }
            /* link2 has 2 or 3 keys */
            else if (node->link2->key2) {
                t234bstree_move_key_from_link2_to_link3(tree, node);
                t234bstree_remove_search(tree, node->link3, key, value, deep);
                t234bstree_count_node_size(tree, node);
                return;
            }

            /* link2 and link3 all have only 1 key */
            t234bstree_merge_middle_key_links(tree, node);
            t234bstree_remove_search(tree, node->link2, key, value, deep);
            t234bstree_count_node_size(tree, node);
            return;
        }
    }
    /* node has only key1 exist */
    else {
        /* link2 2 or 3 keys */
        if (node->link2->key2) {
            t234bstree_remove_search(tree, node->link2, key, value, deep);
            t234bstree_count_node_size(tree, node);
            return;
        }
        /* link1 has 2 or 3 keys */
        else if (node->link1->key2) {
            t234bstree_move_key_from_link1_to_link2(tree, node);
            t234bstree_remove_search(tree, node->link2, key, value, deep);
            t234bstree_count_node_size(tree, node);
            return;
        }

        /* link1 and link2 all have only 1 key */
        t234bstree_merge_left_key_links(tree, node);
        if (node == tree->root) {
            t234bstree_remove_search(tree, node, key, value, deep);
        }
        else {
            t234bstree_remove_search(tree, node->link1, key, value, deep);
        }
        t234bstree_count_node_size(tree, node);
        return;
    }
}

static 
void t234bstree_remove_impl(T234BSTree_PT tree, void *key, void **value, bool deep) {
    /* empty tree */
    if (!tree->root) {
        return;
    }

    /* root has no children */
    if (!tree->root->link1) {
        t234bstree_node_remove_key(tree, tree->root, key, value, deep);
        if (tree->root->size == 0) {
            XMEM_FREE(tree->root);
        }
        return;
    }

    t234bstree_remove_search(tree, tree->root, key, value, deep);
    t234bstree_count_node_size(tree, tree->root);

    return;
}

void t234bstree_remove(T234BSTree_PT tree, void *key) {
    xassert(tree);
    xassert(key);

    if (!tree || !key) {
        return;
    }

    t234bstree_remove_impl(tree, key, NULL, false);
}

static
int t234bstree_map_preorder_impl(T234BSTree_PT tree, T234BSTree_Node_PT node, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        if (apply(node->key1, &(node->value1), cl)) {
            count++;
        }
        if (node->key2 && apply(node->key2, &(node->value2), cl)) {
            count++;
        }
        if (node->key3 && apply(node->key3, &(node->value3), cl)) {
            count++;
        }

        count += t234bstree_map_preorder_impl(tree, node->link1, apply, cl);
        count += t234bstree_map_preorder_impl(tree, node->link2, apply, cl);
        if (node->key2) {
            count += t234bstree_map_preorder_impl(tree, node->link3, apply, cl);
        }
        if (node->key3) {
            count += t234bstree_map_preorder_impl(tree, node->link4, apply, cl);
        }

        return count;
    }
}

int t234bstree_map_preorder(T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(apply);

    if (!tree || !apply) {
        return 0;
    }

    return t234bstree_map_preorder_impl(tree, tree->root, apply, cl);
}

static
void t234bstree_map_preorder_break_if_true_impl(T234BSTree_PT tree, T234BSTree_Node_PT node, bool *true_found, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (*true_found || !node) {
        return;
    }

    if (apply(node->key1, &(node->value1), cl)) {
        *true_found = true;
        return;
    }
    if (node->key2 && apply(node->key2, &(node->value2), cl)) {
        *true_found = true;
        return;
    }
    if (node->key3 && apply(node->key3, &(node->value3), cl)) {
        *true_found = true;
        return;
    }

    t234bstree_map_preorder_break_if_true_impl(tree, node->link1, true_found, apply, cl);
    if (*true_found) {
        return;
    }
    t234bstree_map_preorder_break_if_true_impl(tree, node->link2, true_found, apply, cl);
    if (*true_found) {
        return;
    }
    if (node->key2) {
        t234bstree_map_preorder_break_if_true_impl(tree, node->link3, true_found, apply, cl);
        if (*true_found) {
            return;
        }
    }
    if (node->key3) {
        t234bstree_map_preorder_break_if_true_impl(tree, node->link4, true_found, apply, cl);
        if (*true_found) {
            return;
        }
    }
}

bool t234bstree_map_preorder_break_if_true(T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(apply);

    if (!tree || !apply) {
        return false;
    }

    {
        bool true_found = false;
        t234bstree_map_preorder_break_if_true_impl(tree, tree->root, &true_found, apply, cl);
        return true_found;
    }
}

static
void t234bstree_map_preorder_break_if_false_impl(T234BSTree_PT tree, T234BSTree_Node_PT node, bool *false_found, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (*false_found || !node) {
        return;
    }

    if (!apply(node->key1, &(node->value1), cl)) {
        *false_found = true;
        return;
    }
    if (node->key2 && !apply(node->key2, &(node->value2), cl)) {
        *false_found = true;
        return;
    }
    if (node->key3 && !apply(node->key3, &(node->value3), cl)) {
        *false_found = true;
        return;
    }

    t234bstree_map_preorder_break_if_false_impl(tree, node->link1, false_found, apply, cl);
    if (*false_found) {
        return;
    }
    t234bstree_map_preorder_break_if_false_impl(tree, node->link2, false_found, apply, cl);
    if (*false_found) {
        return;
    }
    if (node->key2) {
        t234bstree_map_preorder_break_if_false_impl(tree, node->link3, false_found, apply, cl);
        if (*false_found) {
            return;
        }
    }
    if (node->key3) {
        t234bstree_map_preorder_break_if_false_impl(tree, node->link4, false_found, apply, cl);
        if (*false_found) {
            return;
        }
    }
}

bool t234bstree_map_preorder_break_if_false(T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(apply);

    if (!tree || !apply) {
        return true;
    }

    {
        bool false_found = false;
        t234bstree_map_preorder_break_if_false_impl(tree, tree->root, &false_found, apply, cl);
        return false_found;
    }
}

static
int t234bstree_map_inorder_impl(T234BSTree_PT tree, T234BSTree_Node_PT node, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        count += t234bstree_map_inorder_impl(tree, node->link1, apply, cl);

        if (apply(node->key1, &(node->value1), cl)) {
            count++;
        }

        count += t234bstree_map_inorder_impl(tree, node->link2, apply, cl);

        if (node->key2) {
            if (apply(node->key2, &(node->value2), cl)) {
                count++;
            }

            count += t234bstree_map_inorder_impl(tree, node->link3, apply, cl);
        }

        if (node->key3) {
            if (apply(node->key3, &(node->value3), cl)) {
                count++;
            }

            count += t234bstree_map_inorder_impl(tree, node->link4, apply, cl);
        }

        return count;
    }
}

int t234bstree_map_inorder(T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(apply);

    if (!tree || !apply) {
        return 0;
    }

    return t234bstree_map_inorder_impl(tree, tree->root, apply, cl);
}


static
void t234bstree_map_inorder_break_if_true_impl(T234BSTree_PT tree, T234BSTree_Node_PT node, bool *true_found, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (*true_found || !node) {
        return;
    }

    t234bstree_map_inorder_break_if_true_impl(tree, node->link1, true_found, apply, cl);
    if (*true_found) {
        return;
    }

    if (apply(node->key1, &(node->value1), cl)) {
        *true_found = true;
        return;
    }

    t234bstree_map_inorder_break_if_true_impl(tree, node->link2, true_found, apply, cl);
    if (*true_found) {
        return;
    }

    if (node->key2) {
        if (apply(node->key2, &(node->value2), cl)) {
            *true_found = true;
            return;
        }

        t234bstree_map_inorder_break_if_true_impl(tree, node->link3, true_found, apply, cl);
        if (*true_found) {
            return;
        }
    }

    if (node->key3) {
        if (apply(node->key3, &(node->value3), cl)) {
            *true_found = true;
            return;
        }

        t234bstree_map_inorder_break_if_true_impl(tree, node->link4, true_found, apply, cl);
        if (*true_found) {
            return;
        }
    }
}

bool t234bstree_map_inorder_break_if_true(T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(apply);

    if (!tree || !apply) {
        return false;
    }

    {
        bool true_found = false;
        t234bstree_map_inorder_break_if_true_impl(tree, tree->root, &true_found, apply, cl);
        return true_found;
    }
}

static
void t234bstree_map_inorder_break_if_false_impl(T234BSTree_PT tree, T234BSTree_Node_PT node, bool *false_found, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (*false_found || !node) {
        return;
    }

    t234bstree_map_inorder_break_if_false_impl(tree, node->link1, false_found, apply, cl);
    if (*false_found) {
        return;
    }

    if (!apply(node->key1, &(node->value1), cl)) {
        *false_found = true;
        return;
    }

    t234bstree_map_inorder_break_if_false_impl(tree, node->link2, false_found, apply, cl);
    if (*false_found) {
        return;
    }

    if (node->key2) {
        if (!apply(node->key2, &(node->value2), cl)) {
            *false_found = true;
            return;
        }

        t234bstree_map_inorder_break_if_false_impl(tree, node->link3, false_found, apply, cl);
        if (*false_found) {
            return;
        }
    }

    if (node->key3) {
        if (!apply(node->key3, &(node->value3), cl)) {
            *false_found = true;
            return;
        }

        t234bstree_map_inorder_break_if_false_impl(tree, node->link4, false_found, apply, cl);
        if (*false_found) {
            return;
        }
    }
}

bool t234bstree_map_inorder_break_if_false(T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(apply);

    if (!tree || !apply) {
        return true;
    }

    {
        bool false_found = false;
        t234bstree_map_inorder_break_if_false_impl(tree, tree->root, &false_found, apply, cl);
        return false_found;
    }
}

static
int t234bstree_map_postorder_impl(T234BSTree_PT tree, T234BSTree_Node_PT node, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        count += t234bstree_map_postorder_impl(tree, node->link1, apply, cl);
        count += t234bstree_map_postorder_impl(tree, node->link2, apply, cl);
        if (apply(node->key1, &(node->value1), cl)) {
            count++;
        }

        if (node->key2) {
            count += t234bstree_map_postorder_impl(tree, node->link3, apply, cl);
            if (apply(node->key2, &(node->value2), cl)) {
                count++;
            }

            if (node->key3) {
                count += t234bstree_map_postorder_impl(tree, node->link4, apply, cl);
                if (apply(node->key3, &(node->value3), cl)) {
                    count++;
                }
            }
        }
        
        return count;
    }
}

int t234bstree_map_postorder(T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(apply);

    if (!tree || !apply) {
        return 0;
    }

    return t234bstree_map_postorder_impl(tree, tree->root, apply, cl);
}

int t234bstree_map_levelorder(T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
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
                T234BSTree_Node_PT node = (T234BSTree_Node_PT)xfifo_pop(fifo);
                if (apply(node->key1, &(node->value1), cl)) {
                    count++;
                }
                if (node->key2 && apply(node->key2, &(node->value2), cl)) {
                    count++;
                }
                if (node->key3 && apply(node->key3, &(node->value3), cl)) {
                    count++;
                }

                if (node->link1) {
                    if (!xfifo_push(fifo, (void*)node->link1)) {
                        return -1;
                    }
                }
                if (node->link2) {
                    if (!xfifo_push(fifo, (void*)node->link2)) {
                        return -1;
                    }
                }
                if (node->link3) {
                    if (!xfifo_push(fifo, (void*)node->link3)) {
                        return -1;
                    }
                }
                if (node->link4) {
                    if (!xfifo_push(fifo, (void*)node->link4)) {
                        return -1;
                    }
                }
            }

            xfifo_free(&fifo);
        }

        return count;
    }
}

int t234bstree_map_min_to_max(T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return t234bstree_map_inorder(tree, apply, cl);
}

static
int t234bstree_map_max_to_min_impl(T234BSTree_PT tree, T234BSTree_Node_PT node, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!node) {
        return 0;
    }

    {
        int count = 0;

        if (node->key3) {
            count += t234bstree_map_max_to_min_impl(tree, node->link4, apply, cl);
            if (apply(node->key3, &(node->value3), cl)) {
                count++;
            }
        }

        if (node->key2) {
            count += t234bstree_map_max_to_min_impl(tree, node->link3, apply, cl);
            if (apply(node->key2, &(node->value2), cl)) {
                count++;
            }
        }

        count += t234bstree_map_max_to_min_impl(tree, node->link2, apply, cl);

        if (apply(node->key1, &(node->value1), cl)) {
            count++;
        }

        count += t234bstree_map_max_to_min_impl(tree, node->link1, apply, cl);

        return count;
    }
}

int t234bstree_map_max_to_min(T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(tree);
    xassert(apply);

    if (!tree || !apply) {
        return 0;
    }

    return t234bstree_map_max_to_min_impl(tree, tree->root, apply, cl);
}

bool t234bstree_swap(T234BSTree_PT tree1, T234BSTree_PT tree2) {
    xassert(tree1);
    xassert(tree2);

    if (!tree1 || !tree2) {
        return false;
    }

    {
        T234BSTree_Node_PT root = tree1->root;
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

    return true;
}

static
int t234bstree_height_impl(T234BSTree_Node_PT node) {
    if (!node) {
        return 0;
    }

    {
        int u = t234bstree_height_impl(node->link1);
        int v = t234bstree_height_impl(node->link2);
        int w = t234bstree_height_impl(node->link3);
        int x = t234bstree_height_impl(node->link4);

        return xiarith_max(xiarith_max(u, v), xiarith_max(w, x)) + 1;
    }
}

int t234bstree_height(T234BSTree_PT tree) {
    xassert(tree);

    if (!tree) {
        return 0;
    }

    return t234bstree_height_impl(tree->root);
}

static
void t234bstree_keys_size_impl(T234BSTree_PT tree, T234BSTree_Node_PT node, void *low, void *high, int *count) {
    if (!node) {
        return;
    }

    int retlow1 = tree->cmp(low, node->key1, tree->cl);
    int rethigh1 = tree->cmp(high, node->key1, tree->cl);

    if (retlow1 <= 0) {
        t234bstree_keys_size_impl(tree, node->link1, low, high, count);
    }
    if ((retlow1 <= 0) && (0 <= rethigh1)) {
        (*count)++;
    }
    if (0 <= rethigh1) {
        if (node->key2) {
            int retlow2 = tree->cmp(low, node->key2, tree->cl);
            int rethigh2 = tree->cmp(high, node->key2, tree->cl);

            if (retlow2 <= 0) {
                t234bstree_keys_size_impl(tree, node->link2, low, high, count);
            }
            if ((retlow2 <= 0) && (0 <= rethigh2)) {
                (*count)++;
            }
            if (0 <= rethigh2) {
                if (node->key3) {
                    int retlow3 = tree->cmp(low, node->key3, tree->cl);
                    int rethigh3 = tree->cmp(high, node->key3, tree->cl);

                    if (retlow3 <= 0) {
                        t234bstree_keys_size_impl(tree, node->link3, low, high, count);
                    }
                    if ((retlow3 <= 0) && (0 <= rethigh3)) {
                        (*count)++;
                    }
                    if (0 <= rethigh3) {
                        t234bstree_keys_size_impl(tree, node->link4, low, high, count);
                    }
                }
                else {
                    t234bstree_keys_size_impl(tree, node->link3, low, high, count);
                }
            }
        }
        else {
            t234bstree_keys_size_impl(tree, node->link2, low, high, count);
        }
    }
}

int t234bstree_keys_size(T234BSTree_PT tree, void *low, void *high) {
    xassert(tree);
    xassert(low);
    xassert(high);
    xassert((tree->cmp(low, high, tree->cl) <= 0));

    if (!tree || !(tree->root) || !low || !high || (0 < tree->cmp(low, high, tree->cl))) {
        return 0;
    }

    int count = 0;
    t234bstree_keys_size_impl(tree, tree->root, low, high, &count);
    return count;
}

int t234bstree_size(T234BSTree_PT tree) {
    return (tree ? (tree->root ? tree->root->size : 0) : 0);
}

bool t234bstree_is_empty(T234BSTree_PT tree) {
    return (tree ? (tree->root ? (tree->root->size == 0) : true) : true);
}
