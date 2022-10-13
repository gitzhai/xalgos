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
#include <stdlib.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../utils/xutils.h"
#include "../array_pointer/xarray_pointer_x.h"
#include "../include/xtree_redblack.h"
#include "xhash_rbtree_x.h"

static
XRBTreeHash_PT xrbtreehash_new_impl(int hint, int(*hash)(void *key), int(*cmp)(void *key1, void *key2, void *cl), void *cl, bool init_list) {
    int slot = xutils_hash_buckets_num(hint);

    XRBTreeHash_PT table = XMEM_CALLOC(1, sizeof(*table));
    if (!table) {
        return NULL;
    }

    {
        table->buckets = xparray_new(slot);
        if (!table->buckets) {
            XMEM_FREE(table);
            return NULL;
        }

        table->slot = slot;
        table->size = 0;
        table->cmp = cmp;
        table->cl = cl;
        table->hash = hash;

        if (init_list) {
            for (int i = 0; i < table->slot; i++) {
                XRBTree_PT tree = xrbtree_new(cmp, cl);
                if (!tree) {
                    xrbtreehash_free(&table);
                    return NULL;
                }

                xparray_put_impl(table->buckets, i, (void*)tree);
            }
        }
    }

    return table;
}

XRBTreeHash_PT xrbtreehash_new(int hint, int(*hash)(void *key), int (*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(0 <= hint);
    xassert(cmp);
    xassert(hash);

    if ((hint < 0) || !cmp || !hash) {
        return NULL;
    }

    return xrbtreehash_new_impl(hint, hash, cmp, cl, true);
}

static 
XRBTreeHash_PT xrbtreehash_copy_impl(XRBTreeHash_PT table, int key_size, int value_size, bool deep) {
    xassert(table);

    if (!table) {
        return NULL;
    }

    {
        XRBTreeHash_PT ntable = xrbtreehash_new_impl(table->slot, table->hash, table->cmp, table->cl, false);
        if (!ntable) {
            return NULL;
        }

        for (int i = 0; i < table->slot; i++) {
            XRBTree_PT tree = deep ? xrbtree_deep_copy((XRBTree_PT)xparray_get_impl(table->buckets, i), key_size, value_size) : xrbtree_copy((XRBTree_PT)xparray_get_impl(table->buckets, i));
            if (!tree) {
                deep ? xrbtreehash_deep_free(&ntable) : xrbtreehash_free(&ntable);
                return NULL;
            }
            xparray_put_impl(ntable->buckets, i, (void*)tree);
        }

        ntable->size = table->size;

        return ntable;
    }
}

XRBTreeHash_PT xrbtreehash_copy(XRBTreeHash_PT table) {
    return xrbtreehash_copy_impl(table, 0, 0, false);
}

XRBTreeHash_PT xrbtreehash_deep_copy(XRBTreeHash_PT table, int key_size, int value_size) {
    xassert(0 < key_size);
    xassert(0 <= value_size);

    if ((key_size <= 0) || (value_size < 0)) {
        return NULL;
    }

    return xrbtreehash_copy_impl(table, key_size, value_size, true);
}

bool xrbtreehash_put_repeat(XRBTreeHash_PT table, void *key, void *value) {
    xassert(table);
    xassert(key);

    if (!table || !key) {
        return false;
    }

    {
        int i = (*table->hash)(key) % table->slot;

        if (xrbtree_put_repeat((XRBTree_PT)xparray_get_impl(table->buckets, i), key, value)) {
            ++table->size;
            return true;
        }
    }

    return false;
}

bool xrbtreehash_put_unique(XRBTreeHash_PT table, void *key, void *value) {
    xassert(table);
    xassert(key);

    if (!table || !key) {
        return false;
    }

    {
        int i = (*table->hash)(key) % table->slot;
        int size = xrbtree_size((XRBTree_PT)xparray_get_impl(table->buckets, i));

        if(xrbtree_put_unique((XRBTree_PT)xparray_get_impl(table->buckets, i), key, value)) {
            table->size += xrbtree_size((XRBTree_PT)xparray_get_impl(table->buckets, i)) - size;
            return true;
        }

        return false;
    }
}

static 
bool xrbtreehash_put_replace_impl(XRBTreeHash_PT table, void *key, void *value, bool deep) {
    xassert(table);
    xassert(key);

    if (!table || !key) {
        return false;
    }

    {
        int i = (*table->hash)(key) % table->slot;
        int size = xrbtree_size((XRBTree_PT)xparray_get_impl(table->buckets, i));
        bool ret = false;

        if (deep) {
            ret = xrbtree_put_deep_replace((XRBTree_PT)xparray_get_impl(table->buckets, i), key, value);
        }
        else {
            ret = xrbtree_put_replace((XRBTree_PT)xparray_get_impl(table->buckets, i), key, value, NULL);
        }

        if (ret) {
            table->size += xrbtree_size((XRBTree_PT)xparray_get_impl(table->buckets, i)) - size;
            return ret;
        }

        return ret;
    }
}

bool xrbtreehash_put_replace(XRBTreeHash_PT table, void *key, void *value) {
    return xrbtreehash_put_replace_impl(table, key, value, false);
}

bool xrbtreehash_put_deep_replace(XRBTreeHash_PT table, void *key, void *value) {
    return xrbtreehash_put_replace_impl(table, key, value, true);
}

void* xrbtreehash_get(XRBTreeHash_PT table, void *key) {
    xassert(table);
    xassert(key);

    if (!table || !key) {
        return NULL;
    }

    return xrbtree_get((XRBTree_PT)xparray_get_impl(table->buckets, (*table->hash)(key) % table->slot), key);
}

XSList_PT xrbtreehash_get_all(XRBTreeHash_PT table, void *key) {
    xassert(table);
    xassert(key);

    if (!table || !key) {
        return NULL;
    }

    return xrbtree_get_all((XRBTree_PT)xparray_get_impl(table->buckets, (*table->hash)(key) % table->slot), key);
}

bool xrbtreehash_find(XRBTreeHash_PT table, void *key) {
    return xrbtree_find((XRBTree_PT)xparray_get_impl(table->buckets, (*table->hash)(key) % table->slot), key);
}

bool xrbtreehash_remove(XRBTreeHash_PT table, void *key) {
    xassert(table);
    xassert(key);

    if (!table || !key) {
        return false;
    }

    {
        int i = (*table->hash)(key) % table->slot;

        int count = xrbtree_remove((XRBTree_PT)xparray_get_impl(table->buckets, i), key);

        table->size -= count;

        return true;
    }
}

bool xrbtreehash_deep_remove(XRBTreeHash_PT table, void *key) {
    xassert(table);
    xassert(key);

    if (!table || !key) {
        return false;
    }

    {
        int i = (*table->hash)(key) % table->slot;

        int count = xrbtree_deep_remove((XRBTree_PT)xparray_get_impl(table->buckets, i), key);

        table->size -= count;

        return true;
    }
}

int xrbtreehash_remove_all(XRBTreeHash_PT table, void *key) {
    xassert(table);
    xassert(key);

    if (!table || !key) {
        return 0;
    }

    {
        int i = (*table->hash)(key) % table->slot;

        int count = xrbtree_remove_all((XRBTree_PT)xparray_get_impl(table->buckets, i), key);

        table->size -= count;

        return count;
    }
}

int xrbtreehash_deep_remove_all(XRBTreeHash_PT table, void *key) {
    xassert(table);
    xassert(key);

    if (!table || !key) {
        return 0;
    }

    {
        int i = (*table->hash)(key) % table->slot;

        int count = xrbtree_deep_remove_all((XRBTree_PT)xparray_get_impl(table->buckets, i), key);

        table->size -= count;

        return count;
    }
}

static 
void xrbtreehash_clear_impl(XRBTreeHash_PT table, bool deep, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(table);

    if (!table) {
        return;
    }

    if (0 < table->size) {
        for (int i = 0; i < table->slot; i++) {
            XRBTree_PT tree = (XRBTree_PT)xparray_get_impl(table->buckets, i);
            deep ? xrbtree_deep_clear(tree) : (apply ? xrbtree_clear_apply(tree, apply, cl) : xrbtree_clear(tree));
        }
    }

    table->size = 0;
}

void xrbtreehash_clear(XRBTreeHash_PT table) {
    xrbtreehash_clear_impl(table, false, NULL, NULL);
}

void xrbtreehash_clear_apply(XRBTreeHash_PT table, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xrbtreehash_clear_impl(table, false, apply, cl);
}

void xrbtreehash_deep_clear(XRBTreeHash_PT table) {
    xrbtreehash_clear_impl(table, true, NULL, NULL);
}

static 
void xrbtreehash_free_impl(XRBTreeHash_PT *table, bool deep, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(table);
    xassert(*table);

    if (!table || !(*table)) {
        return;
    }

    for (int i = 0; i < (*table)->slot; i++) {
        XRBTree_PT tree = (XRBTree_PT)xparray_get_impl((*table)->buckets, i);
        deep ? xrbtree_deep_free(&tree) : (apply ? xrbtree_free_apply(&tree, apply, cl) : xrbtree_free(&tree));
    }

    xparray_free(&((*table)->buckets));
    XMEM_FREE(*table);
}

void xrbtreehash_free(XRBTreeHash_PT *table) {
    xrbtreehash_free_impl(table, false, NULL, NULL);
}

void xrbtreehash_free_apply(XRBTreeHash_PT *table, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xrbtreehash_free_impl(table, false, apply, cl);
}

void xrbtreehash_deep_free(XRBTreeHash_PT *table) {
    xrbtreehash_free_impl(table, true, NULL, NULL);
}

int xrbtreehash_map(XRBTreeHash_PT table, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(table);
    xassert(apply);

    if (!table || !apply) {
        return -1;
    }

    {
        int total = 0;

        for (int i = 0; i < table->slot; i++) {
            total += xrbtree_map_min_to_max((XRBTree_PT)xparray_get_impl(table->buckets, i), apply, cl);
        }

        return total;
    }
}

bool xrbtreehash_map_break_if_true(XRBTreeHash_PT table, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(table);
    xassert(apply);

    if (!table || !apply) {
        return false;
    }

    {
        for (int i = 0; i < table->slot; i++) {
            if (xrbtree_map_min_to_max_break_if_true((XRBTree_PT)xparray_get_impl(table->buckets, i), apply, cl)) {
                return true;
            }
        }
    }

    return false;
}

bool xrbtreehash_map_break_if_false(XRBTreeHash_PT table, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!table || !apply) {
        return true;
    }

    {
        for (int i = 0; i < table->slot; i++) {
            if (xrbtree_map_min_to_max_break_if_false((XRBTree_PT)xparray_get_impl(table->buckets, i), apply, cl)) {
                return true;
            }
        }
    }

    return false;
}

int xrbtreehash_map_key(XRBTreeHash_PT table, bool (*apply)(void *key, void *cl), void *cl) {
    xassert(table);
    xassert(apply);

    if (!table || !apply) {
        return -1;
    }

    {
        int total = 0;

        for (int i = 0; i < table->slot; i++) {
            total += xrbtree_map_key_min_to_max((XRBTree_PT)xparray_get_impl(table->buckets, i), apply, cl);
        }

        return total;
    }
}

bool xrbtreehash_map_key_break_if_true(XRBTreeHash_PT table, bool (*apply)(void *key, void *cl), void *cl) {
    xassert(table);
    xassert(apply);

    if (!table || !apply) {
        return false;
    }

    {
        for (int i = 0; i < table->slot; i++) {
            if (xrbtree_map_key_min_to_max_break_if_true((XRBTree_PT)xparray_get_impl(table->buckets, i), apply, cl)) {
                return true;
            }
        }
    }

    return false;
}

bool xrbtreehash_map_key_break_if_false(XRBTreeHash_PT table, bool (*apply)(void *key, void *cl), void *cl) {
    xassert(apply);

    if (!table || !apply) {
        return true;
    }

    {
        for (int i = 0; i < table->slot; i++) {
            if (xrbtree_map_key_min_to_max_break_if_false((XRBTree_PT)xparray_get_impl(table->buckets, i), apply, cl)) {
                return true;
            }
        }
    }

    return false;
}

bool xrbtreehash_swap(XRBTreeHash_PT table1, XRBTreeHash_PT table2) {
    xassert(table1);
    xassert(table2);

    if (!table1 || !table2) {
        return false;
    }

    {
        int slot = table1->slot;
        int size = table1->size;
        int(*cmp)(void *x, void *y, void *cl) = table1->cmp;
        void *cl = table1->cl;
        int(*hash)(void *key) = table1->hash;
        XPArray_PT buckets = table1->buckets;

        table1->slot = table2->slot;
        table1->size = table2->size;
        table1->cmp = table2->cmp;
        table1->cl = table2->cl;
        table1->hash = table2->hash;
        table1->buckets = table2->buckets;

        table2->slot = slot;
        table2->size = size;
        table2->cmp = cmp;
        table2->cl = cl;
        table2->hash = hash;
        table2->buckets = buckets;
    }

    return true;
}

int xrbtreehash_size(XRBTreeHash_PT table) {
    return (table ? table->size : 0);
}

bool xrbtreehash_is_empty(XRBTreeHash_PT table) {
    return (table ? (table->size == 0) : true);
}

int xrbtreehash_key_size(XRBTreeHash_PT table, void *key) {
    xassert(table);
    xassert(key);

    if (!table || !key) {
        return 0;
    }

    return xrbtree_keys_size((XRBTree_PT)xparray_get_impl(table->buckets, (*table->hash)(key) % table->slot), key, key);
}

double xrbtreehash_loading_factor(XRBTreeHash_PT table) {
    return (table ? (double)table->size / table->slot : 0.0);
}

int xrbtreehash_bucket_size(XRBTreeHash_PT table) {
    return (table ? table->slot : 0);
}

int xrbtreehash_elems_in_bucket(XRBTreeHash_PT table, int bucket) {
    xassert(table);
    xassert(0 <= bucket);
    xassert(bucket < table->slot);

    if (!table || (bucket < 0) || (table->slot <= bucket)) {
        return 0;
    }

    return xrbtree_size((XRBTree_PT)xparray_get_impl(table->buckets, bucket)) ;
}
