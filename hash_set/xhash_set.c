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

#include <stddef.h>

#include "../include/xhash_rbtree.h"
#include "../include/xhash_set.h"

XHashSet_PT xhashset_new(int hint, int(*hash)(void *elem), int (*cmp)(void *elem1, void *elem2, void *cl), void *cl) {
    return xrbtreehash_new(hint, hash, cmp, cl);
}

XHashSet_PT xhashset_copy(XHashSet_PT set) {
    return xrbtreehash_copy(set);
}

XHashSet_PT xhashset_deep_copy(XHashSet_PT set, int elem_size) {
    return xrbtreehash_deep_copy(set, elem_size, 0);
}

bool xhashset_put_repeat(XHashSet_PT set, void *elem) {
    return xrbtreehash_put_repeat(set, elem, NULL);
}

int xhashset_put_unique(XHashSet_PT set, void *elem) {
    return xrbtreehash_put_unique(set, elem, NULL);
}

bool xhashset_find(XHashSet_PT set, void *elem) {
    return xrbtreehash_find(set, elem);
}

bool xhashset_remove(XHashSet_PT set, void *elem) {
    return xrbtreehash_remove(set, elem);
}

bool xhashset_deep_remove(XHashSet_PT set, void *elem) {
    return xrbtreehash_deep_remove(set, elem);
}

int xhashset_remove_all(XHashSet_PT set, void *elem) {
    return xrbtreehash_remove_all(set, elem);
}

int xhashset_deep_remove_all(XHashSet_PT set, void *elem) {
    return xrbtreehash_deep_remove_all(set, elem);
}

void xhashset_clear(XHashSet_PT set) {
    xrbtreehash_clear(set);
}

void xhashset_deep_clear(XHashSet_PT set) {
    xrbtreehash_deep_clear(set);
}

void xhashset_free(XHashSet_PT *set) {
    xrbtreehash_free(set);
}

void xhashset_deep_free(XHashSet_PT *set) {
    xrbtreehash_deep_free(set);
}

int xhashset_map(XHashSet_PT set, bool (*apply)(void *elem, void *cl), void *cl) {
    return xrbtreehash_map_key(set, apply, cl);
}

bool xhashset_map_break_if_true(XHashSet_PT set, bool (*apply)(void *elem, void *cl), void *cl) {
    return xrbtreehash_map_key_break_if_true(set, apply, cl);
}

bool xhashset_map_break_if_false(XHashSet_PT set, bool (*apply)(void *elem, void *cl), void *cl) {
    return xrbtreehash_map_key_break_if_false(set, apply, cl);
}

bool xhashset_swap(XHashSet_PT set1, XHashSet_PT set2) {
    return xrbtreehash_swap(set1, set2);
}

int xhashset_size(XHashSet_PT set) {
    return xrbtreehash_size(set);
}

bool xhashset_is_empty(XHashSet_PT set) {
    return xrbtreehash_is_empty(set);
}

int xhashset_count(XHashSet_PT set, void *elem) {
    return xrbtreehash_key_size(set, elem);
}

double xhashset_loading_factor(XHashSet_PT set) {
    return xrbtreehash_loading_factor(set);
}

int xhashset_elem_size(XHashSet_PT set, void *elem) {
    return xrbtreehash_key_size(set, elem);
}

