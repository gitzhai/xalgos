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
#include "../include/xhash_table.h"

XHashtab_PT xhashtab_new(int hint, int(*hash)(void *key), int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xrbtreehash_new(hint, hash, cmp, cl);
}

XHashtab_PT xhashtab_copy(XHashtab_PT table) {
    return xrbtreehash_copy(table);
}

XHashtab_PT xhashtab_deep_copy(XHashtab_PT table, int key_size) {
    return xrbtreehash_deep_copy(table, key_size, 0);
}

bool xhashtab_put_repeat(XHashtab_PT table, void *key) {
    return xrbtreehash_put_repeat(table, key, NULL);
}

bool xhashtab_put_unique(XHashtab_PT table, void *key) {
    return xrbtreehash_put_unique(table, key, NULL);
}

bool xhashtab_remove(XHashtab_PT table, void *key) {
    return xrbtreehash_remove(table, key);
}

bool xhashtab_deep_remove(XHashtab_PT table, void *key) {
    return xrbtreehash_deep_remove(table, key);
}

int xhashtab_remove_all(XHashtab_PT table, void *key) {
    return xrbtreehash_remove_all(table, key);
}

int xhashtab_deep_remove_all(XHashtab_PT table, void *key) {
    return xrbtreehash_deep_remove_all(table, key);
}

void xhashtab_clear(XHashtab_PT table) {
    xrbtreehash_clear(table);
}

void xhashtab_deep_clear(XHashtab_PT table) {
    xrbtreehash_deep_clear(table);
}

void xhashtab_free(XHashtab_PT *table) {
    xrbtreehash_free(table);
}

void xhashtab_deep_free(XHashtab_PT *table) {
    xrbtreehash_deep_free(table);
}

bool xhashtab_swap(XHashtab_PT table1, XHashtab_PT table2) {
    return xrbtreehash_swap(table1, table2);
}

int xhashtab_map(XHashtab_PT table, bool (*apply)(void *key, void *cl), void *cl) {
    return xrbtreehash_map_key(table, apply, cl);
}

bool xhashtab_map_break_if_true(XHashtab_PT table, bool (*apply)(void *key, void *cl), void *cl) {
    return xrbtreehash_map_key_break_if_true(table, apply, cl);
}

bool xhashtab_map_break_if_false(XHashtab_PT table, bool (*apply)(void *key, void *cl), void *cl) {
    return xrbtreehash_map_key_break_if_false(table, apply, cl);
}

void* xhashtab_get(XHashtab_PT table, void *key) {
    return xrbtreehash_get(table, key);
}

XSList_PT xhashtab_get_all(XHashtab_PT table, void *key) {
    return xrbtreehash_get_all(table, key);
}

bool xhashtab_find(XHashtab_PT table, void *key) {
    return xrbtreehash_find(table, key);
}

int xhashtab_size(XHashtab_PT table) {
    return xrbtreehash_size(table);
}

bool xhashtab_is_empty(XHashtab_PT table) {
    return xrbtreehash_is_empty(table);
}

double xhashtab_loading_factor(XHashtab_PT table) {
    return xrbtreehash_loading_factor(table);
}

int xhashtab_bucket_size(XHashtab_PT table) {
    return xrbtreehash_bucket_size(table);
}

int xhashtab_elems_in_bucket(XHashtab_PT table, int bucket) {
    return xrbtreehash_elems_in_bucket(table, bucket);
}

int xhashtab_key_size(XHashtab_PT table, void *key) {
    return xrbtreehash_key_size(table, key);
}

