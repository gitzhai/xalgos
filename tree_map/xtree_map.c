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

#include "../tree_redblack/xtree_redblack_x.h"
#include "xtree_map_x.h"

XMap_PT xmap_new(int (*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xrbtree_new(cmp, cl);
}

XMap_PT xmap_copy(XMap_PT map) {
    return xrbtree_copy(map);
}

XMap_PT xmap_deep_copy(XMap_PT map, int key_size, int value_size) {
    return xrbtree_deep_copy(map, key_size, value_size);
}

bool xmap_put_repeat(XMap_PT map, void *key, void *value) {
    return xrbtree_put_repeat(map, key, value);
}

bool xmap_put_unique(XMap_PT map, void *key, void *value) {
    return xrbtree_put_unique(map, key, value);
}

bool xmap_put_replace(XMap_PT map, void *key, void *value, void **old_value) {
    return xrbtree_put_replace(map, key, value, old_value);
}

bool xmap_put_deep_replace(XMap_PT map, void *key, void *value) {
    return xrbtree_put_deep_replace(map, key, value);
}

void* xmap_select(XMap_PT map, int k) {
    return xrbtree_select(map, k);
}

void* xmap_get(XMap_PT map, void *key) {
    return xrbtree_get(map, key);
}

bool xmap_find(XMap_PT map, void *key) {
    return xrbtree_find(map, key);
}

bool xmap_find_replace(XMap_PT map, void *key, void *value, void **old_value) {
    return xrbtree_find_replace(map, key, value, old_value);
}

bool xmap_find_deep_replace(XMap_PT map, void *key, void *value) {
    return xrbtree_find_deep_replace(map, key, value);
}

void xmap_replace_key(XMap_PT map, void *old_key, void *new_key) {
    xrbtree_replace_key(map, old_key, new_key);
}

void xmap_clear(XMap_PT map) {
    xrbtree_clear(map);
}

void xmap_clear_apply(XMap_PT map, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xrbtree_clear_apply(map, apply, cl);
}

void xmap_deep_clear(XMap_PT map) {
    xrbtree_deep_clear(map);
}

void xmap_free(XMap_PT *pmap) {
    xrbtree_free(pmap);
}

void xmap_free_apply(XMap_PT *pmap, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xrbtree_free_apply(pmap, apply, cl);
}

void xmap_deep_free(XMap_PT *pmap) {
    xrbtree_deep_free(pmap);
}

void xmap_remove(XMap_PT map, void *key) {
    xrbtree_remove(map, key);
}

void xmap_remove_save(XMap_PT map, void *key, void **value) {
    xrbtree_remove_save(map, key, value);
}

void xmap_deep_remove(XMap_PT map, void *key) {
    xrbtree_deep_remove(map, key);
}

int xmap_map(XMap_PT map, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return xrbtree_map_min_to_max(map, apply, cl);
}

bool xmap_map_break_if_true(XMap_PT map, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return xrbtree_map_min_to_max_break_if_true(map, apply, cl);
}

bool xmap_map_break_if_false(XMap_PT map, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return xrbtree_map_min_to_max_break_if_false(map, apply, cl);
}

int xmap_map_min_to_max(XMap_PT map, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return xrbtree_map_min_to_max(map, apply, cl);
}

int xmap_map_max_to_min(XMap_PT map, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return xrbtree_map_max_to_min(map, apply, cl);
}

bool xmap_swap(XMap_PT map1, XMap_PT map2) {
    return xrbtree_swap(map1, map2);
}

int xmap_size(XMap_PT map) {
    return xrbtree_size(map);
}

bool xmap_is_empty(XMap_PT map) {
    return xrbtree_is_empty(map);
}
