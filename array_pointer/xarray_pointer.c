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

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../utils/xutils.h"
#include "../include/xarith_int.h"
#include "xarray_pointer_x.h"

/* Note :
*    make sure the elements to be NULL if elements removed/deleted/not used !!!
*/

XPArray_PT xparray_new(int size) {
    xassert(0 <= size);

    if (size < 0) {
        return NULL;
    }

    {
        XPArray_PT array = XMEM_CALLOC(1, sizeof(*array));
        if (!array) {
            return NULL;
        }

        if (0 < size) {
            array->datas = XMEM_CALLOC(size, sizeof(void*));
            if (!array->datas) {
                XMEM_FREE(array);
                return NULL;
            }
        }

        array->size = size;

        return array;
    }
}

XPArray_PT xparray_copyn_impl(XPArray_PT array, int start, int count, int elem_size, bool deep) {
    /* keep the new array the same size as source array */
    XPArray_PT narray = xparray_new(array->size);
    if (!narray) {
        return NULL;
    }

    if ((count == 0) || (array->size <= 0) || (array->size <= start)) {
        return narray;
    }

    if (start < 0) {
        start = 0;
    }

    if (array->size < count) {
        count = array->size;
    }

    if (array->size < start + count) {
        count = array->size - start;
    }

    if (deep) {
        int end = start + count;
        for (int i = start; i < end; ++i) {
            int k = i - start;
            if (array->datas[i]) {
                narray->datas[k] = XMEM_CALLOC(1, elem_size);
                if (!narray->datas[k]) {
                    xparray_deep_free(&narray);
                    return NULL;
                }
                memcpy(narray->datas[k], array->datas[i], elem_size);
            }
            else {
                narray->datas[k] = NULL;
            }
        }
    }
    else {
        memcpy(narray->datas, (array->datas + start), (count * sizeof(void*)));
    }

    return narray;
}

XPArray_PT xparray_copy(XPArray_PT array) {
    return xparray_copyn(array, (array ? array->size : 0));
}

XPArray_PT xparray_copyn(XPArray_PT array, int count) {
    xassert(array);
    xassert(0 <= count);

    if (!array || (count < 0)) {
        return NULL;
    }

    return xparray_copyn_impl(array, 0, count, 0, false);
}

XPArray_PT xparray_deep_copy(XPArray_PT array, int elem_size) {
    return xparray_deep_copyn(array, (array ? array->size : 0), elem_size);
}

XPArray_PT xparray_deep_copyn(XPArray_PT array, int count, int elem_size) {
    xassert(array);
    xassert(0 <= count);
    xassert(0 < elem_size);

    if (!array || (count < 0) || (elem_size <= 0)) {
        return NULL;
    }

    return xparray_copyn_impl(array, 0, count, elem_size, true);
}

XPArray_PT xparray_scope_copy(XPArray_PT array, int start, int end) {
    xassert(array);
    xassert(0 <= start);
    xassert(start <= end);
    xassert(end < array->size);

    if (!array || (start < 0) || (end < start) || (array->size <= end)) {
        return NULL;
    }

    return xparray_copyn_impl(array, start, (end - start + 1), 0, false);
}

XPArray_PT xparray_scope_deep_copy(XPArray_PT array, int start, int end, int elem_size) {
    xassert(array);
    xassert(0 <= start);
    xassert(start <= end);
    xassert(end < array->size);
    xassert(0 < elem_size);

    if (!array || (start < 0) || (end < start) || (array->size <= end) || (elem_size <= 0)) {
        return NULL;
    }

    return xparray_copyn_impl(array, start, (end - start + 1), elem_size, true);
}

static 
bool xparray_index_copyn_impl(XPArray_PT array, int start, int end, XPArray_PT darray, int dstart, int elem_size, bool deep) {
    if (deep) {
        for (int i = start; i <= end; i++) {
            int k = dstart + (i - start);
            if (array->datas[i]) {
                darray->datas[k] = XMEM_CALLOC(1, elem_size);
                if (!darray->datas[k]) {
                    xparray_scope_deep_remove(darray, dstart, dstart + end - start);
                    return false;
                }

                memcpy(darray->datas[k], array->datas[i], elem_size);
            }
            else {
                darray->datas[k] = NULL;
            }
        }
    }
    else {
        memcpy((darray->datas + dstart), (array->datas + start), ((end - start + 1) * sizeof(void*)));
    }

    return true;
}

bool xparray_scope_index_copy(XPArray_PT array, int start, int end, XPArray_PT darray, int dstart) {
    xassert(array);
    xassert(0 <= start);
    xassert(start <= end);
    xassert(end < array->size);

    xassert(darray);
    xassert(0 <= dstart);
    xassert(dstart < darray->size);
    xassert(dstart + end - start < darray->size);

    if (!array || (start < 0) || (end < start) || (array->size <= end)) {
        return false;
    }

    if (!darray || (dstart < 0) || (darray->size <= dstart) || (darray->size <= dstart + end - start)) {
        return false;
    }

    return xparray_index_copyn_impl(array, start, end, darray, dstart, 0, false);
}

bool xparray_scope_index_copy_resize(XPArray_PT array, int start, int end, XPArray_PT darray, int dstart) {
    xassert(array);
    xassert(0 <= start);
    xassert(start <= end);
    xassert(end < array->size);

    xassert(darray);
    xassert(0 <= dstart);

    if (!array || (start < 0) || (end < start) || (array->size <= end)) {
        return false;
    }

    if (!darray || (dstart < 0)) {
        return false;
    }

    /* expand the size if needed */
    {
        int new_size = dstart + end - start + 1;
        if (darray->size < new_size) {
            xparray_resize(darray, new_size);
        }
    }

    return xparray_index_copyn_impl(array, start, end, darray, dstart, 0, false);
}

bool xparray_scope_deep_index_copy(XPArray_PT array, int start, int end, XPArray_PT darray, int dstart, int elem_size) {
    xassert(array);
    xassert(0 <= start);
    xassert(start <= end);
    xassert(end < array->size);

    xassert(darray);
    xassert(0 <= dstart);
    xassert(dstart < darray->size);
    xassert(dstart + end - start < darray->size);

    xassert(0 < elem_size);

    if (!array || (start < 0) || (end < start) || (array->size <= end) || (elem_size <= 0)) {
        return false;
    }

    if (!darray || (dstart < 0) || (darray->size <= dstart) || (darray->size <= dstart + end - start)) {
        return false;
    }

    return xparray_index_copyn_impl(array, start, end, darray, dstart, elem_size, true);
}

bool xparray_scope_deep_index_copy_resize(XPArray_PT array, int start, int end, XPArray_PT darray, int dstart, int elem_size) {
    xassert(array);
    xassert(0 <= start);
    xassert(start <= end);
    xassert(end < array->size);

    xassert(darray);
    xassert(0 <= dstart);

    xassert(0 < elem_size);

    if (!array || (start < 0) || (end < start) || (array->size <= end) || (elem_size <= 0)) {
        return false;
    }

    if (!darray || (dstart < 0)) {
        return false;
    }

    /* expand the size if needed */
    {
        int new_size = dstart + end - start + 1;
        if (darray->size < new_size) {
            xparray_deep_resize(darray, new_size);
        }
    }

    return xparray_index_copyn_impl(array, start, end, darray, dstart, elem_size, true);
}

int xparray_vload(XPArray_PT array, void *x, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, x);
    for (; x; x = va_arg(ap, void*)) {
        if (!xparray_put_expand(array, count, x, NULL)) {
            break;
        }
        count++;
    }
    va_end(ap);

    return count;
}

int xparray_aload(XPArray_PT array, void **xs, int len) {
    xassert(xs);
    xassert(0 <= len);

    if (!xs || (len < 0)) {
        return 0;
    }

    {
        int count = 0;

        for (; count < len; ++count) {
            if (!xparray_put_expand(array, count, xs[count], NULL)) {
                break;
            }
        }

        return count;
    }
}

bool xparray_save_and_put_impl(XPArray_PT array, int i, void *data, void **old_data) {
    if (old_data) {
        *old_data = array->datas[i];
    }

    xparray_put_impl(array, i, data);

    return true;
}

bool xparray_put(XPArray_PT array, int i, void *data, void **old_data) {
    xassert(array);
    xassert(0 <= i);
    xassert(i < array->size);

    if (!array || (i < 0) || (array->size <= i)) {
        return false;
    }
    
    return xparray_save_and_put_impl(array, i, data, old_data);
}

static
bool xparray_resize_impl(XPArray_PT array, int new_size, bool deep);

static
bool xparray_put_expand_impl(XPArray_PT array, int i, void *data, void **old_data, int new_size) {
    if (array->size <= i) {
        if (!xparray_resize_impl(array, new_size, false)) {
            return false;
        }
    }

    return xparray_save_and_put_impl(array, i, data, old_data);
}

bool xparray_put_expand(XPArray_PT array, int i, void *data, void **old_data) {
    xassert(array);
    xassert(0 <= i);

    if (!array || (i < 0)) {
        return false;
    }

    return xparray_put_expand_impl(array, i, data, old_data, (i + XUTILS_ARRAY_EXPAND_DEFAULT_LENGTH));
}

bool xparray_put_fix_expand(XPArray_PT array, int i, void *data, void **old_data, int new_size) {
    xassert(array);
    xassert(0 <= i);
    xassert(i < new_size);
    xassert(array->size <= new_size);

    if (!array || (i < 0) || (new_size <= i) || (new_size < array->size)) {
        return false;
    }

    return xparray_put_expand_impl(array, i, data, old_data, new_size);
}

bool xparray_scope_fill(XPArray_PT array, int start, int end, void *data) {
    xassert(array);
    xassert(0 <= start);
    xassert(start <= end);
    xassert(end < array->size);

    if (!array || (start < 0) || (end < start) || (array->size <= end) || (array->size == 0)) {
        return false;
    }

    for (int i = start; i <= end; i++) {
        array->datas[i] = data;
    }

    return true;
}

bool xparray_fill(XPArray_PT array, void *data) {
    return xparray_scope_fill(array, 0, (xparray_size(array) - 1), data);
}

void* xparray_get(XPArray_PT array, int i) {
    xassert(array);
    xassert(0 <= i);
    xassert(i < array->size);

    if (!array || (i < 0) || (array->size <= i)) {
        return NULL;
    }

    return xparray_get_impl(array, i);
}

void* xparray_front(XPArray_PT array) {
    return xparray_get(array, 0);
}

void* xparray_back(XPArray_PT array) {
    return xparray_get(array, (xparray_size(array) - 1));
}

void* xparray_min(XPArray_PT array, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(array);
    xassert(0 < array->size);

    if (!array || (array->size <= 0)) {
        return NULL;
    }

    {
        void *min = array->datas[0];

        for (int i = 1; i < array->size; ++i) {
            if (cmp(array->datas[i], min, cl) < 0) {
                min = array->datas[i];
            }
        }

        return min;
    }
}

void* xparray_max(XPArray_PT array, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(array);
    xassert(0 < array->size);

    if (!array || (array->size <= 0)) {
        return NULL;
    }

    {
        void *max = array->datas[0];

        for (int i = 1; i < array->size; ++i) {
            if (cmp(max, array->datas[i], cl) < 0) {
                max = array->datas[i];
            }
        }

        return max;
    }
}

static
int xparray_map_impl(XPArray_PT array, int m, int n, bool break_first, bool break_true, bool (*apply)(void *x, void *cl), void *cl) {
    xassert(array);
    xassert(apply);

    xassert(0 <= m);
    xassert(m <= n);
    xassert(n < array->size);

    if (!array || !apply) {
        return 0;
    }

    if ((m < 0) || (n < m) || (array->size <= n) || (array->size == 0)) {
        return 0;
    }

    {
        int count = 0;

        for (int i = m; i <= n; i++) {
            bool ret = apply(array->datas[i], cl);

            if (break_first) {
                if (ret && break_true) {
                    ++count;
                    break;
                }

                if (!ret && !break_true) {
                    ++count;
                    break;
                }
            }
            else {
                if (ret) {
                    ++count;
                }
            }
        }

        return count;
    }
}

int xparray_map(XPArray_PT array, bool (*apply)(void *x, void *cl), void *cl) {
    return xparray_map_impl(array, 0, (xparray_size(array) - 1), false, false, apply, cl);
}

bool xparray_map_break_if_true(XPArray_PT array, bool (*apply)(void *x, void *cl), void *cl) {
    return (0 < xparray_map_impl(array, 0, (xparray_size(array) - 1), true, true, apply, cl));
}

bool xparray_map_break_if_false(XPArray_PT array, bool (*apply)(void *x, void *cl), void *cl) {
    return (0 < xparray_map_impl(array, 0, (xparray_size(array) - 1), true, false, apply, cl));
}

static 
void xparray_free_datas_impl(XPArray_PT array, bool deep, bool (*apply)(void *x, void *cl), void *cl) {
    if (0 < array->size) {
        if (deep || apply) {
            for (int i = 0; i < array->size; i++) {
                if (array->datas[i]) {                    
                    deep ? XMEM_FREE(array->datas[i]) : apply(array->datas[i], cl);
                }
            }
        }

        XMEM_FREE(array->datas);
        array->size = 0;
    }
}

void xparray_free(XPArray_PT *parray) {
    if (!parray || !*parray) {
        return;
    }
    xparray_free_datas_impl(*parray, false, NULL, NULL);
    XMEM_FREE(*parray);
}

void xparray_free_apply(XPArray_PT *parray, bool (*apply)(void *x, void *cl), void *cl) {
    if (!parray || !*parray) {
        return;
    }
    xparray_free_datas_impl(*parray, false, apply, cl);
    XMEM_FREE(*parray);
}

void xparray_deep_free(XPArray_PT *parray) {
    if (!parray || !*parray) {
        return;
    }
    xparray_free_datas_impl(*parray, true, NULL, NULL);
    XMEM_FREE(*parray);
}

static
void xparray_clear_impl(XPArray_PT array, bool deep, bool (*apply)(void *x, void *cl), void *cl) {
    if (!array) {
        return;
    }

    if (0 < array->size) {
        if (deep || apply) {
            for (int i = 0; i < array->size; i++) {
                if (array->datas[i]) {
                    deep ? XMEM_FREE(array->datas[i]) : apply(array->datas[i], cl);
                }
            }
        }
        else {
            memset(array->datas, 0, (array->size * sizeof(void*)));
        }
    }
}

void xparray_clear(XPArray_PT array) {
    xparray_clear_impl(array, false, NULL, NULL);
}

void xparray_clear_apply(XPArray_PT array, bool (*apply)(void *x, void *cl), void *cl) {
    xparray_clear_impl(array, false, apply, cl);
}

void xparray_deep_clear(XPArray_PT array) {
    xparray_clear_impl(array, true, NULL, NULL);
}

static
bool xparray_remove_impl(XPArray_PT array, int start, int end, bool deep, void **old_data, bool (*apply)(void *x, void *cl), void *cl) {
    xassert(array);
    xassert(0 <= start);
    xassert(start <= end);
    xassert(end < array->size);

    if (!array || (start < 0) || (end < start) || (array->size <= end)) {
        return false;
    }

    /* just save the first one */
    if (old_data) {
        *old_data = array->datas[start];
    }

    if (deep || apply) {
        for (int i = start; i <= end; i++) {
            if (array->datas[i]) {
                deep ? XMEM_FREE(array->datas[i]) : apply(array->datas[i], cl);
            }
        }
    }
    else {
        memset(array->datas + start, 0, (end - start + 1) * sizeof(void*));
    }

    return true;
}

bool xparray_remove(XPArray_PT array, int i, void **old_data) {
    return xparray_remove_impl(array, i, i, false, old_data, NULL, NULL);
}

bool xparray_remove_apply(XPArray_PT array, int i, bool (*apply)(void *x, void *cl), void *cl) {
    return xparray_remove_impl(array, i, i, false, NULL, apply, cl);
}

bool xparray_deep_remove(XPArray_PT array, int i) {
    return xparray_remove_impl(array, i, i, true, NULL, NULL, NULL);
}

bool xparray_scope_remove(XPArray_PT array, int start, int end) {
    return xparray_remove_impl(array, start, end, false, NULL, NULL, NULL);
}

bool xparray_scope_deep_remove(XPArray_PT array, int start, int end) {
    return xparray_remove_impl(array, start, end, true, NULL, NULL, NULL);
}

static 
bool xparray_resize_impl_apply(XPArray_PT array, int new_size, bool deep) {
    xassert(array);
    xassert(0 <= new_size);

    if (!array || (new_size < 0)) {
        return false;
    }

    if (new_size == array->size) {
        return true;
    }

    if (new_size == 0) {
        xparray_free_datas_impl(array, deep, NULL, NULL);
    }
    else if (array->size == 0) {
        array->datas = XMEM_CALLOC(new_size, sizeof(void*));
        if (!array->datas) {
            return false;
        }
    }
    else {
        XPArray_PT tarray = NULL;

        if (deep && (new_size < array->size)) {
            /* memory will be released from new_size to array->size - 1 after resize
            *  so, temprary save the pointers here for later release
            */
            tarray = xparray_scope_copy(array, new_size, array->size - 1);
            if (!tarray) {
                return false;
            }
        }

        {
            void* ndatas = xmem_resize(array->datas, (new_size * sizeof(void*)), __FILE__, __LINE__);
            if (!ndatas) {
                return false;
            }
            array->datas = ndatas;

            /* free the pointers which will not be used any more */
            if (deep && (new_size < array->size)) {
                xparray_deep_free(&tarray);
            }
        }
    }

    array->size = new_size;

    return true;
}

static 
bool xparray_resize_impl(XPArray_PT array, int new_size, bool deep) {
    int old_len = array ? array->size : 0;

    if (xparray_resize_impl_apply(array, new_size, deep)) {
        if (old_len < new_size) {
            memset((array->datas + old_len), 0, (new_size - old_len) * sizeof(void*));
        }

        return true;
    }

    return false;
}

bool xparray_resize(XPArray_PT array, int new_size) {
    return xparray_resize_impl(array, new_size, false);
}

bool xparray_deep_resize(XPArray_PT array, int new_size) {
    return xparray_resize_impl(array, new_size, true);
}

static 
bool xparray_remove_resize_impl(XPArray_PT array, int i, void **old_data, bool deep) {
    if (!xparray_remove_impl(array, i, i, deep, old_data, NULL, NULL)) {
        return false;
    }

    if (i != array->size - 1) {
        for (int k = i; k < array->size - 1; ++k) {
            array->datas[k] = array->datas[k + 1];
        }
        array->datas[array->size - 1] = NULL;
    }

    /* the last element has been removed already, so, parameter "deep" should be false here */
    return xparray_resize_impl_apply(array, array->size - 1, false);
}

bool xparray_remove_resize(XPArray_PT array, int i, void **old_data) {
    return xparray_remove_resize_impl(array, i, old_data, false);
}

bool xparray_deep_remove_resize(XPArray_PT array, int i) {
    return xparray_remove_resize_impl(array, i, NULL, true);
}

int xparray_size(XPArray_PT array) {
    return (array ? array->size : 0);
}

bool xparray_is_empty(XPArray_PT array) {
    return (array ? (array->size == 0) : true);
}

bool xparray_swap(XPArray_PT array1, XPArray_PT array2) {
    xassert(array1);
    xassert(array2);

    if (!array1 || !array2) {
        return false;
    }

    {
        int size = array1->size;
        void *datas = array1->datas;

        array1->size = array2->size;
        array1->datas = array2->datas;

        array2->size = size;
        array2->datas = datas;
    }

    return true;
}

static inline
void xparray_exch_impl(XPArray_PT array, int i, int j) {
    /* works fine for i == j, so ignore the judgement for i != j since most of the cases are i != j */
    void *x = array->datas[i];
    array->datas[i] = array->datas[j];
    array->datas[j] = x;
}

bool xparray_exch(XPArray_PT array, int i, int j) {
    xassert(array);
    xassert(0 <= i);
    xassert(i < array->size);
    xassert(0 <= j);
    xassert(j < array->size);

    if (!array || (i < 0) || (array->size <= i) || (j < 0) || (array->size <= j)) {
        return false;
    }

    xparray_exch_impl(array, i, j);

    return true;
}

static
bool xparray_is_sorted_impl(XPArray_PT array, int lo, int hi, bool min_to_max, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    if (min_to_max) {
        for (int i = lo + 1; i <= hi; ++i) {
            if (cmp((array->datas)[i], (array->datas)[i - 1], cl) < 0) {
                return false;
            }
        }
    }
    else {
        for (int i = lo + 1; i <= hi; ++i) {
            if (cmp((array->datas)[i - 1], (array->datas)[i], cl) < 0) {
                return false;
            }
        }
    }

    return true;
}

bool xparray_is_sorted(XPArray_PT array, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(array);
    xassert(cmp);

    if (!array || !cmp) {
        return false;
    }

    return xparray_is_sorted_impl(array, 0, array->size - 1, true, cmp, cl);
}

bool xparray_bubble_sort(XPArray_PT array, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(array);
    xassert(cmp);

    if (!array || !cmp) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    for (int i = 0; i < array->size - 1; ++i) {
        for (int j = array->size - 1; i < j; --j) {
            if (cmp((array->datas)[j], (array->datas)[j - 1], cl) < 0) {
                xparray_exch_impl(array, j, (j - 1));
            }
        }
    }

    xassert(xparray_is_sorted(array, cmp, cl));

    return true;
}

/* <<Algorithms>> Fourth Edition, Chapter 2.1.2 */
bool xparray_select_sort(XPArray_PT array, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(array);
    xassert(cmp);

    if (!array || !cmp) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    for (int i = 0; i < array->size; ++i) {
        int min = i;
        for (int j = i + 1; j < array->size; ++j) {
            if (cmp(array->datas[j], array->datas[min], cl) < 0) {
                min = j;
            }
        }

        if (i != min) {
            xparray_exch_impl(array, i, min);
        }
    }

    xassert(xparray_is_sorted(array, cmp, cl));

    return true;
}

static
void xparray_insert_sort_impl(XPArray_PT array, int lo, int step, int hi, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    /*  lo     x                 hi
    *    |-|-|-|-|-|-|-|-|-|-|-|-|
    *          i-->
    *     <--j
    */
    for (int i = lo + step; i <= hi; ++i) {
        void *x = array->datas[i];

        int j = i - step;
        for (; lo <= j; j -= step) {
            /* sorted already */
            if (cmp(array->datas[j], x, cl) <= 0) {
                break;
            }
            array->datas[j + step] = array->datas[j];
        }

        j += step;
        if (j < i) {
            array->datas[j] = x;
        }
    }
}

/* <<Algorithms>> Fourth Edition, Chapter 2.1.3 */
bool xparray_insert_sort(XPArray_PT array, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(array);
    xassert(cmp);

    if (!array || !cmp) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    xparray_insert_sort_impl(array, 0, 1, (array->size - 1), cmp, cl);

    xassert(xparray_is_sorted(array, cmp, cl));

    return true;
}

static
void xparray_shell_sort_impl(XPArray_PT array, int lo, int hi, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    int h = 1;
    int num = hi - lo + 1;

    while (h < num / 3) {
        h = 3 * h + 1;  /* 1, 4, 13, 40, 121, 364, 1093, ... */
    }

    while (1 <= h) {
        xparray_insert_sort_impl(array, lo, h, hi, cmp, cl);
        h = h / 3;
    }
}

/* <<Algorithms>> Fourth Edition, Chapter 2.1.6 */
bool xparray_shell_sort(XPArray_PT array, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(array);
    xassert(cmp);

    if (!array || !cmp) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    xparray_shell_sort_impl(array, 0, array->size - 1, cmp, cl);

    xassert(xparray_is_sorted(array, cmp, cl));

    return true;
}

static
void xparray_in_place_merge_impl(XPArray_PT array, XPArray_PT tarray, int lo, int mid, int hi, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    /*  lo          mid          hi
    *    |-|-|-|-|-|-|-|-|-|-|-|-|
    *    i-->          j--->
    */
    int i = lo, j = mid + 1;
    for (int k = lo; k <= hi; ++k) {
        if (mid < i) {      /* left parts are all in array already */
            /* array->datas[k] = tarray->datas[j++]; */
            memcpy(array->datas + k, tarray->datas + j, ((hi - k + 1) * sizeof(void*)));
            break;
        }
        else if (hi < j) {  /* right parts are all in array already */
            /* array->datas[k] = tarray->datas[i++]; */
            memcpy(array->datas + k, tarray->datas + i, ((hi - k + 1) * sizeof(void*)));
            break;
        }
        else if (cmp(tarray->datas[j], tarray->datas[i], cl) < 0) {
            array->datas[k] = tarray->datas[j++];
        }
        else {
            array->datas[k] = tarray->datas[i++];
        }
    }
}

static
void xparray_merge_impl(XPArray_PT array, XPArray_PT tarray, int lo, int mid, int hi, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    /* check if in order already */
    if (cmp(array->datas[mid], array->datas[mid + 1], cl) <= 0) {
        return;
    }

    /* backup the [lo, hi] at first */
    memcpy(tarray->datas + lo, array->datas + lo, ((hi - lo + 1) * sizeof(void*)));

    xparray_in_place_merge_impl(array, tarray, lo, mid, hi, cmp, cl);
}

/* <<Algorithms in C>> Third Edition, Chapter 8.4
*
* call stack example :
*
* xparray_merge_sort_impl_no_copy(array, tarray, 0, 50)
*     xparray_merge_sort_impl_no_copy(tarray, array, 0, 25)
*
*         xparray_merge_sort_impl_no_copy(array, tarray, 0, 12)
*
*             xparray_merge_sort_impl_no_copy(tarray, array, 0, 6)
*                 xparray_insert_sort_impl(tarray, 0, 1, 6)
*             xparray_merge_sort_impl_no_copy(tarray, array, 7, 12)
*                 xparray_insert_sort_impl(tarray, 7, 1, 12)
*             xparray_in_place_merge_impl(array, tarray, 0, 6, 12)
*
*         xparray_merge_sort_impl_no_copy(array, tarray, 13, 25)
*
*             xparray_merge_sort_impl_no_copy(tarray, array, 13, 19)
*                 xparray_insert_sort_impl(tarray, 13, 1, 19)
*             xparray_merge_sort_impl_no_copy(tarray, array, 19, 25)
*                 xparray_insert_sort_impl(tarray, 19, 1, 25)
*             xparray_in_place_merge_impl(array, tarray, 13, 19, 25)
*
*         xparray_in_place_merge_impl(tarray, array, 0, 12, 25)
*
*     xparray_merge_sort_impl_no_copy(tarray, array, 26, 50)
*
*         xparray_merge_sort_impl_no_copy(array, tarray, 26, 38)
*
*             xparray_merge_sort_impl_no_copy(tarray, array, 26, 32)
*                 xparray_insert_sort_impl(tarray, 26, 1, 32)
*             xparray_merge_sort_impl_no_copy(tarray, array, 33, 38)
*                 xparray_insert_sort_impl(tarray, 33, 1, 38)
*             xparray_in_place_merge_impl(array, tarray, 26, 32, 38)
*
*         xparray_merge_sort_impl_no_copy(array, tarray, 39, 50)
*
*             xparray_merge_sort_impl_no_copy(tarray, array, 39, 44)
*                 xparray_insert_sort_impl(tarray, 39, 1, 44)
*             xparray_merge_sort_impl_no_copy(tarray, array, 45, 50)
*                 xparray_insert_sort_impl(tarray, 45, 1, 50)
*             xparray_in_place_merge_impl(array, tarray, 39, 44, 50)
*
*         xparray_in_place_merge_impl(tarray, array, 26, 32, 50)
*
*     xparray_in_place_merge_impl(array, tarray, 0, 25, 50)
*/
void xparray_merge_sort_impl_no_copy(XPArray_PT array, XPArray_PT tarray, int lo, int hi, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    /* <<Algorithms>> Fourth Edition, Chapter 2.2.2.1
    *  for short array, insert sort is faster, so use insert sort here instead of :
    *   if (hi <= lo) {
    *       return;
    *   }
    */
    if (hi <= lo + 10) {
        xparray_insert_sort_impl(array, lo, 1, hi, cmp, cl);
        return;
    }

    {
        int mid = (lo + hi) / 2;

        xparray_merge_sort_impl_no_copy(tarray, array, lo, mid, cmp, cl);       /* sort the [lo, mid] */
        xparray_merge_sort_impl_no_copy(tarray, array, mid + 1, hi, cmp, cl);   /* sort the [mid+1, hi] */

        xparray_in_place_merge_impl(array, tarray, lo, mid, hi, cmp, cl);        /* merge the sorted [lo, mid] and [mid+1, hi] */
    }
}

/* <<Algorithms>> Fourth Edition, Chapter 2.2.2 */
void xparray_merge_sort_impl_up_bottom(XPArray_PT array, XPArray_PT tarray, int lo, int hi, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    /* <<Algorithms>> Fourth Edition, Chapter 2.2.2.1
    *  for short array, insert sort is faster, so use insert sort here instead of :
    *   if (hi <= lo) {
    *       return;
    *   }
    */
    if (hi <= lo + 10) {
        xparray_insert_sort_impl(array, lo, 1, hi, cmp, cl);
        return;
    }

    {
        int mid = (lo + hi) / 2;

        xparray_merge_sort_impl_up_bottom(array, tarray, lo, mid, cmp, cl);       /* sort the [lo, mid] */
        xparray_merge_sort_impl_up_bottom(array, tarray, mid + 1, hi, cmp, cl);   /* sort the [mid+1, hi] */

        xparray_merge_impl(array, tarray, lo, mid, hi, cmp, cl);        /* merge the sorted [lo, mid] and [mid+1, hi] */
    }
}

/* <<Algorithms>> Fourth Edition, Chapter 2.2.3 */
void xparray_merge_sort_impl_bottom_up(XPArray_PT array, XPArray_PT tarray, int lo, int hi, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    int num = hi - lo + 1;
    /* i = 1, 2, 4, 8, 16, 32 ... : the subarray size used for merging every time from the bottom */
    for (int i = 1; i < num; i *= 2) {
        /* j = 0, j += 2*i... : merge the two subarray sequentially */
        for (int j = 0; j < num - i; j += 2 * i) {
            xparray_merge_impl(array, tarray, lo+j, lo+j+i-1, xiarith_min(lo+j+2*i-1, lo+num-1), cmp, cl);
        }
    }
}

/* <<Algorithms>> Fourth Edition, Chapter 2.2 */
bool xparray_merge_sort(XPArray_PT array, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(array);
    xassert(cmp);

    if (!array || !cmp) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    {
        /* for temporary backup */
        XPArray_PT tarray = xparray_new(array->size);
        if (!tarray) {
            return false;
        }

#if defined(MERGE_SORT_BOTTOM_UP)
        xparray_merge_sort_impl_bottom_up(array, tarray, 0, array->size - 1, cmp, cl);
#elif defined(MERGE_SORT_NO_COPY_MERGE)
        memcpy(tarray->datas, array->datas, (array->size * sizeof(void*)));
        xparray_merge_sort_impl_no_copy(array, tarray, 0, array->size - 1, cmp, cl);
#else
        xparray_merge_sort_impl_up_bottom(array, tarray, 0, array->size - 1, cmp, cl);
#endif
        xparray_free(&tarray);
    }

    xassert(xparray_is_sorted(array, cmp, cl));

    return true;
}

/*  standard quick sort method : <<Algorithms>> Fourth Edition, Chapter 2.3.1
*
 *  lo                       hi  
 *  |v|  <=v  |--?--|  >=v   |       -->   |  <=val  | val |  >= val |
 *    i-->                <--j
 *
 */
static
int xparray_quick_sort_impl_basic_split_impl(XPArray_PT array, int lo, int hi, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    int i = lo, j = hi + 1;

    void *val = array->datas[lo];
    while (true) {
        /* find one bigger(>=) element from left index :
        *    it's better to stop moving while "datas[++i] >= val", the same principle for next step "datas[--j] <= val",
        *    it may exchange the equal value sometimes, but it can avoid N^2 performance in extreme scenarios :
        *       it can decrease the change to let "val" to stay in index "lo" always
        */
        while (cmp(array->datas[++i], val, cl) < 0) {
            if (i == hi) {
                break;
            }
        }

        /* find one smaller(<=) element from right index */
        while (cmp(val, array->datas[--j], cl) < 0);

        /* all elements are right splitted already */
        if (j <= i) {
            break;
        }

        /* exchange the left side bigger element with the right side smaller element */
        xparray_exch_impl(array, i, j);
    }

    /* move the split element to the target postion to make its left side elements smaller and right side elements bigger */
    xparray_exch_impl(array, lo, j);

    return j;
}

void xparray_quick_sort_impl_basic_split(XPArray_PT array, int lo, int hi, int depth_limit, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    if (hi <= lo) {
        return;
    }

    /* for short array, insert sort is faster than quick sort, so use insert sort instead,
     * <<Algorithms in C>> Third Edition, Chapter 7.4
     */
    if (hi <= lo + 10) {
        xparray_insert_sort_impl(array, lo, 1, hi, cmp, cl);
        return;
    }

    /* limit the call stack depth to avoid bad performance */
    if (depth_limit <= 0) {
        xparray_heap_sort_impl(array, lo, hi, true, cmp, cl);
        return;
    }

    --depth_limit;

    {
        int j = xparray_quick_sort_impl_basic_split_impl(array, lo, hi, cmp, cl);

        xparray_quick_sort_impl_basic_split(array, lo, j - 1, depth_limit, cmp, cl);
        xparray_quick_sort_impl_basic_split(array, j + 1, hi, depth_limit, cmp, cl);
    }
}

void xparray_quick_sort_impl_random_split(XPArray_PT array, int lo, int hi, int depth_limit, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    if (hi <= lo) {
        return;
    }

    /* for short array, insert sort is faster than quick sort, so use insert sort instead,
     * <<Algorithms in C>> Third Edition, Chapter 7.4
     */
    if (hi <= lo + 10) {
        xparray_insert_sort_impl(array, lo, 1, hi, cmp, cl);
        return;
    }

    /* limit the call stack depth to avoid bad performance */
    if (depth_limit <= 0) {
        xparray_heap_sort_impl(array, lo, hi, true, cmp, cl);
        return;
    }

    --depth_limit;

    {
        /* create a random index i between lo and hi, exchange the value at position lo and i at first */
        int i = rand() % (hi-lo) + lo;
        xparray_exch_impl(array, i, lo);

        int j = xparray_quick_sort_impl_basic_split_impl(array, lo, hi, cmp, cl);

        xparray_quick_sort_impl_random_split(array, lo, j - 1, depth_limit, cmp, cl);
        xparray_quick_sort_impl_random_split(array, j + 1, hi, depth_limit, cmp, cl);
    }
}

/* median of three : <<Algorithms in C>> Third Edition, Chapter 7.5
 *  |  <=val  | val |  >= val |
 */
static
int xparray_quick_sort_impl_median_of_three_split_impl(XPArray_PT array, int lo, int hi, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    /* find the median value of three samples, move it to index lo */
    int m = (lo + hi) / 2;

    if (cmp(array->datas[lo], array->datas[m], cl) <= 0) {
        if (cmp(array->datas[lo], array->datas[hi], cl) < 0) {
            if (cmp(array->datas[m], array->datas[hi], cl) <= 0) {
                xparray_exch_impl(array, lo, m);
            }
            else {
                xparray_exch_impl(array, lo, hi);
            }
        }
        /* lo is median value already */
    }
    else {
        if (cmp(array->datas[hi], array->datas[lo], cl) < 0) {
            if (cmp(array->datas[hi], array->datas[m], cl) <= 0) {
                xparray_exch_impl(array, lo, m);
            }
            else {
                xparray_exch_impl(array, lo, hi);
            }
        }
        /* lo is median value already */
    }

    return xparray_quick_sort_impl_basic_split_impl(array, lo, hi, cmp, cl);
}

void xparray_quick_sort_impl_median_of_three_split(XPArray_PT array, int lo, int hi, int depth_limit, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    /* for short array, insert sort is faster than quick sort, so use insert sort instead,
     * <<Algorithms in C>> Third Edition, Chapter 7.4
     */
    if (hi <= lo + 10) {
        xparray_insert_sort_impl(array, lo, 1, hi, cmp, cl);
        return;
    }

    /* limit the call stack depth to avoid bad performance */
    if (depth_limit <= 0) {
        xparray_heap_sort_impl(array, lo, hi, true, cmp, cl);
        return;
    }

    --depth_limit;

    {
        int j = xparray_quick_sort_impl_median_of_three_split_impl(array, lo, hi, cmp, cl);

        xparray_quick_sort_impl_median_of_three_split(array, lo, j - 1, depth_limit, cmp, cl);
        xparray_quick_sort_impl_median_of_three_split(array, j + 1, hi, depth_limit, cmp, cl);
    }
}

/*  3 way split method : <<Algorithms>> Fourth Edition, Chapter 2.3.3.3
 *
 *  lo                        hi          lo                              hi
 *  |v| --------------------- |     -->   |  <val  | =val | ---- |  > val |     -->   |  <val  |  == val  |  > val |
 *  lt-->                  <--gt                   lt-->      <--gt
 *    i-->                                                i--> 
 *  
 */
void xparray_quick_sort_impl_3_way_split(XPArray_PT array, int lo, int hi, int depth_limit, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    /* for short array, insert sort is faster than quick sort, so use insert sort instead,
     * <<Algorithms in C>> Third Edition, Chapter 7.4
     */
    if (hi <= lo + 10) {
        xparray_insert_sort_impl(array, lo, 1, hi, cmp, cl);
        return;
    }

    /* limit the call stack depth to avoid bad performance */
    if (depth_limit <= 0) {
        xparray_heap_sort_impl(array, lo, hi, true, cmp, cl);
        return;
    }

    --depth_limit;

    {
        int lt = lo, i = lo + 1, gt = hi;

        void *val = array->datas[lt];
        while (i <= gt) {
            int ret = cmp(array->datas[i], val, cl);
            if (ret < 0) {
                /* move the smaller element to left side */
                xparray_exch_impl(array, lt, i);
                ++lt;
                ++i;
            }
            else if (0 < ret) {
                /* move the bigger element to right side */
                xparray_exch_impl(array, i, gt);
                --gt;
            }
            else {
                /* find the equal element */
                ++i;
            }
        }

        xparray_quick_sort_impl_3_way_split(array, lo, lt - 1, depth_limit, cmp, cl);
        xparray_quick_sort_impl_3_way_split(array, gt + 1, hi, depth_limit, cmp, cl);
    }
}

/* Bentley McIlroy Method : <<Algorithms in C>> Third Edition, Chapter 7.6
 *
 *  lo                        hi          lo                                 hi
 *  | --------------------- |v|     -->   | =val | <val | ------- | >val | =val |    -->   | =val  | <val | >val | =val |  -->  |  <val  |  = val  |  > val |
 *  p-->                   <--q                  p-->                 <--q
 *  i-->                 <--j                           i-->   <--j
 * 
 */
void xparray_quick_sort_impl_quick_3_way_split(XPArray_PT array, int lo, int hi, int depth_limit, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    /* for short array, insert sort is faster than quick sort, so use insert sort instead,
     * <<Algorithms in C>> Third Edition, Chapter 7.4
     */
    if (hi <= lo + 10) {
        xparray_insert_sort_impl(array, lo, 1, hi, cmp, cl);
        return;
    }

    /* limit the call stack depth to avoid bad performance */
    if (depth_limit <= 0) {
        xparray_heap_sort_impl(array, lo, hi, true, cmp, cl);
        return;
    }

    --depth_limit;

    {
        int i = lo - 1, j = hi, p = lo - 1, q = hi;

        void *val = array->datas[hi];
        while (true) {
            /* find one bigger(>=) element from left index */
            while (cmp(array->datas[++i], val, cl) < 0);

            /* find one smaller(<=) element from right index */
            while (cmp(val, array->datas[--j], cl) < 0) {
                if (j == lo) {
                    break;
                }
            }

            /* all elements are right splitted already */
            if (j <= i) {
                break;
            }

            /* exchange the bigger and smaller element */
            xparray_exch_impl(array, i, j);

            /* move the equal element to left side */
            if (cmp(array->datas[i], val, cl) == 0) {
                ++p;
                xparray_exch_impl(array, p, i);
            }

            /* move the equal element to right side */
            if (cmp(array->datas[j], val, cl) == 0) {
                --q;
                xparray_exch_impl(array, q, j);
            }
        }

        /* array->datas[hi] < array->datas[i], exchange it now */
        xparray_exch_impl(array, i, hi);

        /* index i saved the value "val" now, so, move j to one previous step of "val", move i to one next step of "val" */
        j = i - 1;
        i += 1;

        /* move the left side equal element to middle */
        for (int k = lo; k < p; ++k, --j) {
            xparray_exch_impl(array, k, j);
        }

        /* move the right side equal element to middle */
        for (int k = hi - 1; q < k; --k, ++i) {
            xparray_exch_impl(array, k, i);
        }

        xparray_quick_sort_impl_quick_3_way_split(array, lo, j, depth_limit, cmp, cl);
        xparray_quick_sort_impl_quick_3_way_split(array, i, hi, depth_limit, cmp, cl);
    }
}

/* <<Algorithms>> Fourth Edition, Chapter 2.3 */
bool xparray_quick_sort(XPArray_PT array, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(array);
    xassert(cmp);

    if (!array || !cmp) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

#if defined(QUICK_SORT_NORMAL_SPLIT)
    xparray_quick_sort_impl_basic_split(array, 0, array->size - 1, xiarith_lg(array->size - 1) * 2, cmp, cl);
#elif defined(QUICK_SORT_RANDOM_SPLIT)
    srand((unsigned)time(NULL));
    xparray_quick_sort_impl_random_split(array, 0, array->size - 1, xiarith_lg(array->size - 1) * 2, cmp, cl);
#elif defined(QUICK_SORT_MEDIAN_OF_THREE)
    xparray_quick_sort_impl_median_of_three_split(array, 0, array->size - 1, xiarith_lg(array->size - 1) * 2, cmp, cl);
#elif defined(QUICK_SORT_3_WAY_SPLIT) 
    xparray_quick_sort_impl_3_way_split(array, 0, array->size - 1, xiarith_lg(array->size - 1) * 2, cmp, cl);
#else
    xparray_quick_sort_impl_quick_3_way_split(array, 0, array->size - 1, xiarith_lg(array->size - 1) * 2, cmp, cl);
#endif

    xassert(xparray_is_sorted(array, cmp, cl));

    return true;
}

#define xparray_heap_parent(npos) ((int)(((npos) - 1) >> 1))  /* >> 1 means divided by 2 (/2) */
#define xparray_heap_left(npos)   (((npos) << 1) + 1)         /* << 1 means multiplied by 2 (*2) */
#define xparray_heap_right(npos)  (((npos) << 1) + 2)

static
bool xparray_section_is_heap_sorted_impl(XPArray_PT array, int k, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl), void *cl) {
    if (hi <= lo + k) {
        return true;
    }

    {
        int lpos = xparray_heap_left(k);
        int rpos = xparray_heap_right(k);

        /* check left branch */
        if (hi < lo + lpos) {
            return true;
        }

        if (minheap) {
            if (cmp(array->datas[lo + lpos], array->datas[lo + k], cl) < 0) {
                return false;
            }
        }
        else {
            if (cmp(array->datas[lo + k], array->datas[lo + lpos], cl) < 0) {
                return false;
            }
        }

        /* check right branch */
        if (hi < lo + rpos) {
            return true;
        }

        if (minheap) {
            if (cmp(array->datas[lo + rpos], array->datas[lo + k], cl) < 0) {
                return false;
            }
        }
        else {
            if (cmp(array->datas[lo + k], array->datas[lo + rpos], cl) < 0) {
                return false;
            }
        }

        /* continue to check the children of left and right branches */
        return xparray_section_is_heap_sorted_impl(array, lpos, lo, hi, minheap, cmp, cl) && xparray_section_is_heap_sorted_impl(array, rpos, lo, hi, minheap, cmp, cl);
    }
}

bool xparray_section_is_heap_sorted(XPArray_PT array, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(array);
    xassert(0 <= lo);
    xassert(lo <= hi);
    xassert(hi < array->size);
    xassert(cmp);

    if (!array || (lo < 0) || (hi < lo) || (array->size <= hi) || !cmp) {
        return false;
    }

    return xparray_section_is_heap_sorted_impl(array, 0, lo, hi, minheap, cmp, cl);
}

bool xparray_is_heap_sorted(XPArray_PT array, int (*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(array);
    xassert(cmp);

    if (!array || !cmp) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    return xparray_section_is_heap_sorted_impl(array, 0, 0, array->size - 1, true, cmp, cl);
}

/* << Algorithms >> Fourth Edition, Chapter 2.4.4.1 */
static
void xparray_heapify_swim_elem(XPArray_PT array, int k, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl), void *cl) {
    int parent = lo + xparray_heap_parent(k);

    while (lo <= parent) {
        if (minheap) {
            /* parent is equal or smaller than k, do nothing */
            if (cmp(array->datas[parent], array->datas[lo + k], cl) <= 0) {
                break;
            }
        }
        else {
            /* parent is equal or bigger than k, do nothing */
            if (cmp(array->datas[lo + k], array->datas[parent], cl) <= 0) {
                break;
            }
        }

        /* exchange the parent with k */
        xparray_exch_impl(array, (lo + k), parent);

        /* prepare the next round compare */
        k = parent - lo;
        parent = lo + xparray_heap_parent(k);
    }
}

/* << Algorithms >> Fourth Edition, Chapter 2.4.4.2 */
/* << Introduction to Algorithms >> Third Edition, Chapter 6.2 */
static
void xparray_heapify_sink_elem(XPArray_PT array, int k, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl), void *cl) {
    int left_child = 0;
    int right_child = 0;
    int child = 0;

    while (true) {
        left_child = lo + xparray_heap_left(k);
        if (hi < left_child) {
            break;
        }

        right_child = left_child + 1;
        child = left_child;

        if (minheap) {
            /* right child is smaller than left child */
            if ((right_child <= hi) && (cmp(array->datas[right_child], array->datas[left_child], cl) < 0)) {
                child = right_child;
            }

            /* k is equal or smaller than child, do nothing */
            if (cmp(array->datas[lo + k], array->datas[child], cl) <= 0) {
                break;
            }
        }
        else {
            /* right child is bigger than left child */
            if ((right_child <= hi) && (cmp(array->datas[left_child], array->datas[right_child], cl) < 0)) {
                child = right_child;
            }

            /* k is equal or bigger than child, do nothing */
            if (cmp(array->datas[child], array->datas[lo + k], cl) <= 0) {
                break;
            }
        }

        /* change the child with k */
        xparray_exch_impl(array, (lo + k), child);

        /* prepare the next round compare */
        k = child - lo;
    }
}

/* << Algorithms >> Fourth Edition, Chapter 2.4.5.1 */
/* << Introduction to Algorithms >> Third Edition, Chapter 6.3 */
bool xparray_heapify_impl(XPArray_PT array, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl), void *cl) {
    /* just need to scan half of the array */
    for (int k = (hi - lo) / 2; 0 <= k; --k) {
        xparray_heapify_sink_elem(array, k, lo, hi, minheap, cmp, cl);
    }

    xassert(xparray_section_is_heap_sorted(array, lo, hi, minheap, cmp, cl));

    return true;
}

/* min heap */
bool xparray_heapify_min(XPArray_PT array, int (*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(array);
    xassert(cmp);

    if (!array || !cmp) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    return xparray_heapify_impl(array, 0, array->size - 1, true, cmp, cl);
}

/* max heap */
bool xparray_heapify_max(XPArray_PT array, int (*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(array);
    xassert(cmp);

    if (!array || !cmp) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    return xparray_heapify_impl(array, 0, array->size - 1, false, cmp, cl);
}

/* << Algorithms >> Fourth Edition, Chapter 2.4.5 */
/* << Introduction to Algorithms >> Third Edition, Chapter 6.4 */
bool xparray_heap_sort_impl(XPArray_PT array, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl), void *cl) {
    int ohi = hi;

    /* make the scope a heap */
    xparray_heapify_impl(array, lo, hi, !minheap, cmp, cl);

    /* make the scope sorted : save the min/max element one bye one at the end of the scope */
    {
        while (lo < hi) {
            xparray_exch_impl(array, lo, hi);
            --hi;
            xparray_heapify_sink_elem(array, 0, lo, hi, !minheap, cmp, cl);
        }
    }

    xassert(xparray_is_sorted_impl(array, lo, ohi, minheap, cmp, cl));

    return true;
}

bool xparray_heap_sort(XPArray_PT array, int (*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(array);
    xassert(cmp);

    if (!array || !cmp) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    return xparray_heap_sort_impl(array, 0, array->size - 1, true, cmp, cl);
}

/* <<Algorithms>> Fourth Edition, Chapter 2.5.3.4 */
/* <<Introduction to Algorithms>> Third Edition, Chapter 9.2 */
void* xparray_get_kth_element_impl_quick_sort(XPArray_PT array, int k, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    int lo = 0;
    int hi = array->size - 1;
    int depth_limit = xiarith_lg(array->size - 1) * 2;

    while (lo < hi) {
        /* for short array, insert sort is faster than quick sort, so use insert sort instead,
         * <<Algorithms in C>> Third Edition, Chapter 7.4
         */
        if (hi <= lo + 10) {
            xparray_insert_sort_impl(array, lo, 1, hi, cmp, cl);
            return array->datas[k];
        }

        /* limit the call stack depth to avoid bad performance */
        if (depth_limit <= 0) {
            xparray_heap_sort_impl(array, lo, hi, true, cmp, cl);
            return array->datas[k];
        }

        --depth_limit;

        /* create a random index i between lo and hi, exchange the value at position lo and i at first */
        {
            int i = rand() % (hi - lo) + lo;
            xparray_exch_impl(array, i, lo);
        }

        {
            int j = xparray_quick_sort_impl_basic_split_impl(array, lo, hi, cmp, cl);

            if (k == j) {
                return array->datas[k];
            }
            else if (k < j) {
                hi = j - 1;
            }
            else {
                lo = j + 1;
            }
        }
    }

    /* not possible to be here */
    xassert(false);
    return NULL;
}

/* <<Algorithms>> Fourth Edition, Chapter 2.5.3.4 */
void* xparray_get_kth_element_impl_heap_sort(XPArray_PT array, int k, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    if (array->size <= 2 * k) {
        /* make the array a max heap*/
        xparray_heapify_impl(array, 0, array->size - 1, false, cmp, cl);

        /* save the max element one bye one at the end of the scope */
        {
            int hi = array->size - 1;

            while (0 < hi) {
                xparray_exch_impl(array, 0, hi);
                if (hi == k) {
                    return array->datas[k];
                }

                --hi;
                xparray_heapify_sink_elem(array, 0, 0, hi, false, cmp, cl);
            }
        }
    }
    else {
        /* make the array a min heap*/
        xparray_heapify_impl(array, 0, array->size - 1, true, cmp, cl);

        /* save the min element one bye one at the end of the scope */
        {
            int hi = array->size - 1;
            int count = 0;

            while (0 < hi) {
                xparray_exch_impl(array, 0, hi);
                if (count == k) {
                    return array->datas[hi];
                }

                ++count;
                --hi;
                xparray_heapify_sink_elem(array, 0, 0, hi, true, cmp, cl);
            }
        }
    }

    /* not possible to be here */
    xassert(false);
    return NULL;
}

void* xparray_get_kth_element(XPArray_PT array, int k, int (*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(array);
    xassert(0 <= k);
    xassert(k < array->size);
    xassert(cmp);

    if (!array || (k < 0) || (array->size <= k) || !cmp) {
        return NULL;
    }

#if defined(GET_KTH_ELEMENT_HEAP_SORT)
    return xparray_get_kth_element_impl_heap_sort(array, k, cmp, cl);
#else
    srand((unsigned)time(NULL));
    return xparray_get_kth_element_impl_quick_sort(array, k, cmp, cl);
#endif
}

static
int xparray_binary_search_impl(XPArray_PT array, void *data, int lo, int hi, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    if (hi < lo) {
        return -1;
    }

    {
        int k = (lo + hi) / 2;

        int result = cmp(data, array->datas[k], cl);

        if (result == 0) {
            return k;
        }
        else if (result < 0) {
            return xparray_binary_search_impl(array, data, lo, k - 1, cmp, cl);
        }
        else {
            return xparray_binary_search_impl(array, data, k + 1, hi, cmp, cl);
        }
    }
}

/* <<Algorithms>> Fourth Edition, Chapter 3.1.5 */
int xparray_binary_search(XPArray_PT array, void *data, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(array);
    xassert(data);
    xassert(cmp);

    if (!array || !data || !cmp || (array->size <= 0)) {
        return -1;
    }

    xassert(xparray_is_sorted(array, cmp, cl));

    return xparray_binary_search_impl(array, data, 0, array->size - 1, cmp, cl);
}
