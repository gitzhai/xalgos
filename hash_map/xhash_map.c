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

#include "../include/xhash_rbtree.h"
#include "../include/xhash_map.h"

XHashMap_PT xhashmap_new(int hint, int(*hash)(void *key), int (*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xrbtreehash_new(hint, hash, cmp, cl);
}

XHashMap_PT xhashmap_copy(XHashMap_PT map) {
    return xrbtreehash_copy(map);
}

XHashMap_PT xhashmap_deep_copy(XHashMap_PT map, int key_size, int value_size) {
    return xrbtreehash_deep_copy(map, key_size, value_size);
}

bool xhashmap_put_repeat(XHashMap_PT map, void *key, void *value) {
    return xrbtreehash_put_repeat(map, key, value);
}

bool xhashmap_put_unique(XHashMap_PT map, void *key, void *value) {
    return xrbtreehash_put_unique(map, key, value);
}

bool xhashmap_put_replace(XHashMap_PT map, void *key, void *value) {
    return xrbtreehash_put_replace(map, key, value);
}

bool xhashmap_put_deep_replace(XHashMap_PT map, void *key, void *value) {
    return xrbtreehash_put_deep_replace(map, key, value);
}

bool xhashmap_remove(XHashMap_PT map, void *key) {
    return xrbtreehash_remove(map, key);
}

bool xhashmap_remove_all(XHashMap_PT map, void *key) {
    return 0 <= xrbtreehash_remove_all(map, key);
}

void xhashmap_clear(XHashMap_PT map) {
    xrbtreehash_clear(map);
}

void xhashmap_clear_apply(XHashMap_PT map, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xrbtreehash_clear_apply(map, apply, cl);
}

void xhashmap_deep_clear(XHashMap_PT map) {
    xrbtreehash_deep_clear(map);
}

void xhashmap_free(XHashMap_PT *map) {
    xrbtreehash_free(map);
}

void xhashmap_free_apply(XHashMap_PT *map, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xrbtreehash_free_apply(map, apply, cl);
}

void xhashmap_deep_free(XHashMap_PT *map) {
    xrbtreehash_deep_free(map);
}

bool xhashmap_swap(XHashMap_PT map1, XHashMap_PT map2) {
    return xrbtreehash_swap(map1, map2);
}

int xhashmap_map(XHashMap_PT map, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return xrbtreehash_map(map, apply, cl);
}

bool xhashmap_map_break_if_true(XHashMap_PT map, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return xrbtreehash_map_break_if_true(map, apply, cl);
}

bool xhashmap_map_break_if_false(XHashMap_PT map, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return xrbtreehash_map_break_if_false(map, apply, cl);
}

void* xhashmap_get(XHashMap_PT map, void *key) {
    return xrbtreehash_get(map, key);
}

XSList_PT xhashmap_get_all(XHashMap_PT map, void *key) {
    return xrbtreehash_get_all(map, key);
}

bool xhashmap_find(XHashMap_PT map, void *key) {
    return xrbtreehash_find(map, key);
}

int xhashmap_size(XHashMap_PT map) {
    return xrbtreehash_size(map);
}

bool xhashmap_is_empty(XHashMap_PT map) {
    return xrbtreehash_is_empty(map);
}

double xhashmap_loading_factor(XHashMap_PT map) {
    return xrbtreehash_loading_factor(map);
}

int xhashmap_key_size(XHashMap_PT map, void *key) {
    return xrbtreehash_key_size(map, key);
}


