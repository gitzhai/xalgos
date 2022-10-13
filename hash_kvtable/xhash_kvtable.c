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
#include "../include/xlist_kvd.h"
#include "../include/xqueue_deque.h"
#include "../array_pointer/xarray_pointer_x.h"
#include "xhash_kvtable_x.h"

static
XKVHashtab_PT xkvhashtab_new_impl(int hint, int(*hash)(void *key), int(*cmp)(void *key1, void *key2, void *cl), void *cl, bool init_list) {
    int slot = xutils_hash_buckets_num(hint);

    XKVHashtab_PT table = XMEM_CALLOC(1, sizeof(*table));
    if (!table) {
        return NULL;
    }

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
            XKVDList_PT list = xkvdlist_new();
            if (!list) {
                xkvhashtab_free(&table);
                return NULL;
            }
            xparray_put_impl(table->buckets, i, (void*)list);
        }
    }

    return table;
}

XKVHashtab_PT xkvhashtab_new(int hint, int(*hash)(void *key), int (*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(0 <= hint);
    xassert(cmp);
    xassert(hash);

    if ((hint < 0) || !cmp || !hash) {
        return NULL;
    }

    return xkvhashtab_new_impl(hint, hash, cmp, cl, true);
}

// in order to support x2kvhashtab_copy, do not use xassert(table) here
/* note :
 *   the deep copy use the memcpy at last,
 *   so, the key_size and value_size should be treated carefully !
 */
XKVHashtab_PT xkvhashtab_copy_impl(XKVHashtab_PT table, int key_size, int value_size, bool deep) {
    if (!table) {
        return NULL;
    }

    {
        XKVHashtab_PT ntable = xkvhashtab_new_impl(table->slot, table->hash, table->cmp, table->cl, false);
        if (!ntable) {
            return NULL;
        }

        for (int i = 0; i < table->slot; i++) {
            XKVDList_PT list = deep ? xkvdlist_deep_copy((XKVDList_PT)xparray_get_impl(table->buckets, i), key_size, value_size) : xkvdlist_copy((XKVDList_PT)xparray_get_impl(table->buckets, i));
            if (!list) {
                deep ? xkvhashtab_deep_free(&ntable) : xkvhashtab_free(&ntable);
                return NULL;
            }
            xparray_put_impl(ntable->buckets, i, (void*)list);
        }

        ntable->size = table->size;

        return ntable;
    }
}

XKVHashtab_PT xkvhashtab_copy(XKVHashtab_PT table) {
    return xkvhashtab_copy_impl(table, 0, 0, false);
}

XKVHashtab_PT xkvhashtab_deep_copy(XKVHashtab_PT table, int key_size, int value_size) {
    xassert(0 < key_size);
    xassert(0 <= value_size);

    if ((key_size <= 0) || (value_size < 0)) {
        return NULL;
    }

    return xkvhashtab_copy_impl(table, key_size, value_size, true);
}

// in order to support x2kvhashtab_put_repeat, do not use xassert(table) here
int xkvhashtab_put_repeat(XKVHashtab_PT table, void *key, void *value) {
    xassert(key);

    if (!table || !key) {
        return -1;
    }

    {
        int i = (*table->hash)(key) % table->slot;

        if (xkvdlist_push_front_repeat((XKVDList_PT)xparray_get_impl(table->buckets, i), key, value)) {
            table->size++;
            return 1;
        }
    }

    return 0;
}

// in order to support x2kvhashtab_put_unique, do not use xassert(table) here
int xkvhashtab_put_unique(XKVHashtab_PT table, void *key, void *value) {
    xassert(key);

    if (!table || !key) {
        return -1;
    }

    {
        int i = (*table->hash)(key) % table->slot;

        int count = xkvdlist_push_front_unique_if((XKVDList_PT)xparray_get_impl(table->buckets, i), key, value, table->cmp, table->cl);
        if (0 < count) {
            table->size++;
        }

        return count;
    }
}

// in order to support x2kvhashtab_put_replace, do not use xassert(table) here
static 
int xkvhashtab_put_replace_impl(XKVHashtab_PT table, void *key, void *value, bool deep) {
    xassert(key);

    if (!table || !key) {
        return -1;
    }

    {
        int i = (*table->hash)(key) % table->slot;
        int count = 0;

        if (deep) {
            count = xkvdlist_push_front_deep_replace_if((XKVDList_PT)xparray_get_impl(table->buckets, i), key, value, table->cmp, table->cl);
        }
        else {
            count = xkvdlist_push_front_replace_if((XKVDList_PT)xparray_get_impl(table->buckets, i), key, value, table->cmp, table->cl);
        }

        if (0 < count) {
            table->size++;
        }

        return count;
    }
}

int xkvhashtab_put_replace(XKVHashtab_PT table, void *key, void *value) {
    return xkvhashtab_put_replace_impl(table, key, value, false);
}

int xkvhashtab_put_deep_replace(XKVHashtab_PT table, void *key, void *value) {
    return xkvhashtab_put_replace_impl(table, key, value, true);
}

// in order to support x2kvhashtab_get, do not use xassert(table) here
XRDList_PT xkvhashtab_get(XKVHashtab_PT table, void *key) {
    xassert(key);

    if (!table || !key) {
        return NULL;
    }

    return xkvdlist_find_equal_if((XKVDList_PT)xparray_get_impl(table->buckets, (*table->hash)(key) % table->slot), key, table->cmp, table->cl);
}

static
bool xkvhashtab_get_all_apply(void *key, void *cl) {
    XKVHashtab_Cmp_Keys_Paras_PT paras = (XKVHashtab_Cmp_Keys_Paras_PT)cl;
    XDeque_PT deque = (XDeque_PT)paras->key2;

    if ((paras->cmp)(key, paras->key1, paras->cl) == 0) {
        if (!xdeque_push_back_no_limit(deque, key)) {
            return false;
        }
    }

    return true;
}

// in order to support x2kvhashtab_get_all, do not use xassert(table) here
XDeque_PT xkvhashtab_get_all(XKVHashtab_PT table, void *key) {
    xassert(key);

    if (!table || !key) {
        return NULL;
    }

    {
        int i = (*table->hash)(key) % table->slot;

        XDeque_PT deque = xdeque_new(XUTILS_DEQUE_LAYER2_DEFAULT_LENGTH);
        if (!deque) {
            return NULL;
        }

        {
            XKVHashtab_Cmp_Keys_Paras_T paras = { table->cmp, key, (void*)deque, table->cl };
            bool ret = xkvdlist_map_key_break_if_false((XKVDList_PT)xparray_get_impl(table->buckets, i), xkvhashtab_get_all_apply, &paras);
            if (ret) {
                xdeque_free(&deque);
                return NULL;
            }

            return deque;
        }
    }
}

bool xkvhashtab_find(XKVHashtab_PT table, void *key) {
    return (NULL != xkvhashtab_get(table, key));
}

/* in order to support x2kvhashtab_remove :
*   1. have to return bool value here.
*   2. do not use xassert(table) here
*/
bool xkvhashtab_remove(XKVHashtab_PT table, void *key) {
    xassert(key);

    if (!table || !key) {
        return false;
    }

    {
        int i = (*table->hash)(key) % table->slot;
        int count = xkvdlist_size((XKVDList_PT)xparray_get_impl(table->buckets, i));

        bool ret = xkvdlist_remove_equal_break_if((XKVDList_PT)xparray_get_impl(table->buckets, i), key, table->cmp, table->cl);
        if (ret) {
            table->size -= count - xkvdlist_size((XKVDList_PT)xparray_get_impl(table->buckets, i));
        }

        return ret;
    }
}

/* in order to support x2kvhashtab_remove_all :
*   1. have to return removed key numbers.
*   2. do not use xassert(table) here
*/
int xkvhashtab_remove_all(XKVHashtab_PT table, void *key) {
    xassert(key);

    if (!table || !key) {
        return -1;
    }

    {
        int i = (*table->hash)(key) % table->slot;

        int count = xkvdlist_remove_equal_if((XKVDList_PT)xparray_get_impl(table->buckets, i), key, table->cmp, table->cl);
        if (0 < count) {
            table->size -= count;
        }

        return count;
    }
}

// in order to support x2kvhashtab_clear, do not use xassert(table) here
static 
void xkvhashtab_clear_impl(XKVHashtab_PT table, bool deep, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!table) {
        return;
    }

    if (0 < table->size) {
        for (int i = 0; i < table->slot; i++) {
            XKVDList_PT list = (XKVDList_PT)xparray_get_impl(table->buckets, i);
            if (list) {
                deep ? xkvdlist_deep_clear(list) : (apply ? xkvdlist_clear_apply(list, apply, cl) : xkvdlist_clear(list));
            }
        }
    }

    table->size = 0;
}

void xkvhashtab_clear(XKVHashtab_PT table) {
    xkvhashtab_clear_impl(table, false, NULL, NULL);
}

void xkvhashtab_clear_apply(XKVHashtab_PT table, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xkvhashtab_clear_impl(table, false, apply, cl);
}

// in order to support x2kvhashtab_clear, do not use xassert(table) here
void xkvhashtab_deep_clear(XKVHashtab_PT table) {
    xkvhashtab_clear_impl(table, true, NULL, NULL);
}

// in order to support x2kvhashtab_free, do not use xassert(table) here
static 
void xkvhashtab_free_impl(XKVHashtab_PT *table, bool deep, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!table || !(*table)) {
        return;
    }

    for (int i = 0; i < (*table)->slot; i++) {
        XKVDList_PT list = (XKVDList_PT)xparray_get_impl((*table)->buckets, i);
        if (list) {
            deep ? xkvdlist_deep_free(&list) : (apply ? xkvdlist_free_apply(&list, apply, cl) : xkvdlist_free(&list));
        }
    }

    xparray_free(&((*table)->buckets));
    XMEM_FREE(*table);
}

void xkvhashtab_free(XKVHashtab_PT *table) {
    xkvhashtab_free_impl(table, false, NULL, NULL);
}

void xkvhashtab_free_apply(XKVHashtab_PT *table, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xkvhashtab_free_impl(table, false, apply, cl);
}

// in order to support x2kvhashtab_deep_free, do not use xassert(table) here
void xkvhashtab_deep_free(XKVHashtab_PT *table) {
    xkvhashtab_free_impl(table, true, NULL, NULL);
}

// in order to support x2kvhashtab_map, do not use xassert(table) here
int xkvhashtab_map(XKVHashtab_PT table, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!table || !apply) {
        return -1;
    }

    {
        int total = 0;

        for (int i = 0; i < table->slot; i++) {
            total += xkvdlist_map((XKVDList_PT)xparray_get_impl(table->buckets, i), apply, cl);
        }

        return total;
    }
}

/* in order to support x2kvhashtab_map_break_if:
*   1. have to return bool value
*   2. do not use xassert(table) here
*/
bool xkvhashtab_map_break_if_true(XKVHashtab_PT table, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!table || !apply) {
        return false;
    }

    {
        for (int i = 0; i < table->slot; i++) {
            if (xkvdlist_map_break_if_true((XKVDList_PT)xparray_get_impl(table->buckets, i), apply, cl)) {
                return true;
            }
        }
    }

    return false;
}

bool xkvhashtab_map_break_if_false(XKVHashtab_PT table, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xassert(apply);

    if (!table || !apply) {
        return true;
    }

    {
        for (int i = 0; i < table->slot; i++) {
            if (xkvdlist_map_break_if_false((XKVDList_PT)xparray_get_impl(table->buckets, i), apply, cl)) {
                return true;
            }
        }
    }

    return false;
}

// in order to support x2kvhashtab_map, do not use xassert(table) here
int xkvhashtab_map_key(XKVHashtab_PT table, bool (*apply)(void *key, void *cl), void *cl) {
    xassert(apply);

    if (!table || !apply) {
        return -1;
    }

    {
        int total = 0;

        for (int i = 0; i < table->slot; i++) {
            total += xkvdlist_map_key((XKVDList_PT)xparray_get_impl(table->buckets, i), apply, cl);
        }

        return total;
    }
}

/* in order to support x2kvhashtab_map_break_if:
*   1. have to return bool value
*   2. do not use xassert(table) here
*/
bool xkvhashtab_map_key_break_if_true(XKVHashtab_PT table, bool (*apply)(void *key, void *cl), void *cl) {
    xassert(apply);

    if (!table || !apply) {
        return false;
    }

    {
        for (int i = 0; i < table->slot; i++) {
            if (xkvdlist_map_key_break_if_true((XKVDList_PT)xparray_get_impl(table->buckets, i), apply, cl)) {
                return true;
            }
        }
    }

    return false;
}

bool xkvhashtab_map_key_break_if_false(XKVHashtab_PT table, bool (*apply)(void *key, void *cl), void *cl) {
    xassert(apply);

    if (!table || !apply) {
        return true;
    }

    {
        for (int i = 0; i < table->slot; i++) {
            if (xkvdlist_map_key_break_if_false((XKVDList_PT)xparray_get_impl(table->buckets, i), apply, cl)) {
                return true;
            }
        }
    }

    return false;
}

bool xkvhashtab_swap(XKVHashtab_PT table1, XKVHashtab_PT table2) {
    xassert(table1);
    xassert(table2);

    if (!table1 || !table2) {
        return false;
    }

    {
        int slot = table1->slot;
        int size = table1->size;
        int(*cmp) (void *x, void *y, void *cl) = table1->cmp;
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

int xkvhashtab_size(XKVHashtab_PT table) {
    return (table ? table->size : 0);
}

bool xkvhashtab_is_empty(XKVHashtab_PT table) {
    return (table ? (table->size == 0) : true);
}

double xkvhashtab_loading_factor(XKVHashtab_PT table) {
    return (table ? (double)table->size / table->slot : 0.0);
}

int xkvhashtab_bucket_size(XKVHashtab_PT table) {
    return (table ? table->slot : 0);
}

int xkvhashtab_max_bucket_size(XKVHashtab_PT table) {
    return xutils_max_hash_buckets_size();
}

int xkvhashtab_elems_in_bucket(XKVHashtab_PT table, int bucket) {
    xassert(table);
    xassert(0 <= bucket);
    xassert(bucket < table->slot);

    if (!table || (bucket < 0) || (table->slot <= bucket)) {
        return 0;
    }

    return xkvdlist_size((XKVDList_PT)xparray_get_impl(table->buckets, bucket)) ;
}

static
bool xkvhashtab_key_size_apply(void *key, void *cl) {
    XKVHashtab_Cmp_Keys_Paras_PT paras = (XKVHashtab_Cmp_Keys_Paras_PT)cl;
    return (paras->cmp)(key, paras->key1, paras->cl) == 0;
}

int xkvhashtab_key_size(XKVHashtab_PT table, void *key) {
    xassert(table);
    xassert(key);

    if (!table || !key) {
        return 0;
    }

    {
        int i = (*table->hash)(key) % table->slot;
        XKVHashtab_Cmp_Keys_Paras_T paras = { table->cmp, key, NULL, table->cl };

        return xkvdlist_map_key((XKVDList_PT)xparray_get_impl(table->buckets, i), xkvhashtab_key_size_apply, &paras);
    }
}

static 
bool xkvhashtab_resize_map_apply(void *key, void **value, void *cl) {
    XKVHashtab_PT ntable = (XKVHashtab_PT)cl;

    if (!xkvhashtab_put_repeat(ntable, key, *value)) {
        return false;
    }

    return true;
}

/* if use rbtree instead, we will not need this function */
bool xkvhashtab_resize(XKVHashtab_PT table, int new_hint) {
    xassert(table);
    xassert(0 < new_hint);

    if (!table || (new_hint <= 0)) {
        return false;
    }

    {
        XKVHashtab_PT ntable = xkvhashtab_new(new_hint, table->hash, table->cmp, table->cl);
        if (!ntable) {
            return false;
        }

        bool ret = false;
        for (int i = 0; i < table->slot; i++) {
            ret = xkvdlist_map_break_if_false((XKVDList_PT)xparray_get_impl(table->buckets, i), xkvhashtab_resize_map_apply, ntable);
            if (ret) {
                break;
            }
        }

        if (!ret) {
            xkvhashtab_swap(table, ntable);
        }

        xkvhashtab_free(&ntable);

        return !ret;
    }
}
