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
*
*   Refer to :
*       <<C Interfaces and Implementations>> David R. Hanson, chapter 10
*/

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../utils/xutils.h"
#include "../include/xarith_int.h"
#include "../array_int/xarray_int_x.h"
#include "../array_pointer/xarray_pointer_x.h"
#include "xarray_x.h"

/* Note :
*    make sure the elements to be NULL if elements removed/deleted/not used !!!
*/

XArray_PT xarray_new(int size, int elem_size) {
    xassert(0 <= size);
    xassert(0 < elem_size);

    if ((size < 0) || (elem_size <= 0)) {
        return NULL;
    }

    {
        XArray_PT array = XMEM_CALLOC(1, sizeof(*array));
        if (!array) {
            return NULL;
        }

        {
            char *memory = NULL;

            if (0 < size) {
                memory = XMEM_CALLOC(size, elem_size);
                if (!memory) {
                    XMEM_FREE(array);
                    return NULL;
                }
            }

            array->size = size;
            array->elem_size = elem_size;
            array->datas = memory;
        }

        return array;
    }
}

XArray_PT xarray_copyn_impl(XArray_PT array, int start, int count, bool (*apply)(void *x, int elem_size, void *cl), void *cl) {
    XArray_PT narray = xarray_new(array->size, array->elem_size);
    if (!narray) {
        return NULL;
    }

    if (count == 0) {
        return narray;
    }

    if (array->size < count) {
        count = array->size;
    }

    if (array->size < start + count) {
        count = array->size - start;
    }

    if (0 < array->size) {
        if (apply) {
            int end = start + count;
            for (int i = start; i < end; i++) {
                /* just copy the valid elements */
                if (apply(array->datas + i * array->elem_size, array->elem_size, cl)) {
                    memcpy(narray->datas + (i - start) * array->elem_size, (array->datas + i * array->elem_size), array->elem_size);
                }
            }
        }
        else {
            memcpy(narray->datas, array->datas + start * array->elem_size, (count * array->elem_size));
        }
    }

    return narray;
}

XArray_PT xarray_copy(XArray_PT array) {
    return xarray_copyn(array, array ? array->size : 0);
}

XArray_PT xarray_copyn(XArray_PT array, int count) {
    xassert(array);
    xassert(0 <= count);

    if (!array || (count < 0)) {
        return NULL;
    }

    return xarray_copyn_impl(array, 0, count, NULL, NULL);
}

int xarray_vload(XArray_PT array, void *x, ...) {
    xassert(array);

    if (!array) {
        return -1;
    }

    if (!x) {
        return 0;
    }

    {
        int count = 0;

        va_list ap;
        va_start(ap, x);
        for (; x; x = va_arg(ap, void*)) {
            if (!xarray_put_expand(array, count, x, NULL)) {
                break;
            }
            count++;
        }
        va_end(ap);

        return count;
    }
}

int xarray_aload(XArray_PT array, XPArray_PT xs) {
    xassert(array);

    if (!array) {
        return -1;
    }

    if (!xs) {
        return 0;
    }

    {
        int count = 0;
        int total = xparray_size(xs);

        for (; count < total; count++) {
            if (!xarray_put_expand(array, count, xparray_get_impl(xs, count), NULL)) {
                break;
            }
        }

        return count;
    }
}

bool xarray_put(XArray_PT array, int i, void *data, void *old_data) {
    xassert(array);
    xassert(0 <= i);
    xassert(i < array->size);

    if (!array || (i < 0) || (array->size <= i)) {
        return false;
    }

    if (old_data) {
        memcpy(old_data, (array->datas + i * array->elem_size), array->elem_size);
    }

    if (data) {
        memcpy((array->datas + i * array->elem_size), data, array->elem_size);
    }
    else {
        memset((array->datas + i * array->elem_size), 0, array->elem_size);
    }

    return true;
}

static
bool xarray_put_expand_impl(XArray_PT array, int i, void *data, void *old_data, int new_size) {
    if (array->size <= i) {
        if (!xarray_resize(array, new_size)) {
            return false;
        }
    }

    return xarray_put(array, i, data, old_data);
}

bool xarray_put_expand(XArray_PT array, int i, void *data, void *old_data) {
    xassert(array);
    xassert(0 <= i);

    if (!array || (i < 0)) {
        return false;
    }

    return xarray_put_expand_impl(array, i, data, old_data, (i + XUTILS_ARRAY_EXPAND_DEFAULT_LENGTH));
}

bool xarray_put_fix_expand(XArray_PT array, int i, void *data, void *old_data, int new_size) {
    xassert(array);
    xassert(0 <= i);
    xassert(i < new_size);
    xassert(array->size <= new_size);

    if (!array || (i < 0) || (new_size <= i) || (new_size < array->size)) {
        return false;
    }

    return xarray_put_expand_impl(array, i, data, old_data, new_size);
}

bool xarray_fill(XArray_PT array, void *data) {
    if (xarray_size(array) == 0) {
        return false;
    }

    return xarray_scope_fill(array, 0, xarray_size(array) - 1, data);
}

bool xarray_scope_fill(XArray_PT array, int start, int end, void *data) {
    xassert(array);
    xassert(0 <= start);
    xassert(start <= end);
    xassert(end < array->size);

    if (!array || (start < 0) || (end < start) || (array->size <= end) || (array->size == 0)) {
        return false;
    }

    if (data) {
        for (int i = start; i <= end; i++) {
            memcpy((array->datas + i * array->elem_size), data, array->elem_size);
        }
    }
    else {
        memset((array->datas + start * array->elem_size), 0, (end - start + 1) * array->elem_size);
    }

    return true;
}

void* xarray_get(XArray_PT array, int i) {
    xassert(array);
    xassert(0 <= i);
    xassert(i < array->size);

    if (!array || (i < 0) || (array->size <= i)) {
        return NULL;
    }

    return array->datas + i * array->elem_size;
}

void* xarray_front(XArray_PT array) {
    return xarray_get(array, 0);
}

void* xarray_back(XArray_PT array) {
    if (xarray_size(array) == 0) {
        return NULL;
    }

    return xarray_get(array, (array->size - 1));
}

static
int xarray_map_impl(XArray_PT array, int m, int n, bool break_first, bool break_true, bool (*apply)(void *x, int elem_size, void *cl), void *cl) {
    xassert(array);
    xassert(apply);

    xassert(0 <= m);
    xassert(m <= n);
    xassert(n < array->size);

    if (!array || !apply) {
        return -1;
    }

    if ((m < 0) || (n < m) || (array->size <= n) || (array->size == 0)) {
        return 0;
    }

    {
        int count = 0;

        for (int i = m; i <= n; i++) {
            bool ret = apply(array->datas + i * array->elem_size, array->elem_size, cl);

            if (break_first) {
                if (ret && break_true) {
                    count++;
                    break;
                }

                if (!ret && !break_true) {
                    count++;
                    break;
                }
            }
            else {
                if (ret) {
                    count++;
                }
            }
        }

        return count;
    }
}

int xarray_map(XArray_PT array, bool (*apply)(void *x, int elem_size, void *cl), void *cl) {
    if (xarray_size(array) == 0) {
        return 0;
    }

    return xarray_map_impl(array, 0, array->size - 1, false, false, apply, cl);
}

bool xarray_map_break_if_true(XArray_PT array, bool (*apply)(void *x, int elem_size, void *cl), void *cl) {
    if (xarray_size(array) == 0) {
        return false;
    }

    return (0 < xarray_map_impl(array, 0, array->size - 1, true, true, apply, cl));
}

bool xarray_map_break_if_false(XArray_PT array, bool (*apply)(void *x, int elem_size, void *cl), void *cl) {
    if (xarray_size(array) == 0) {
        return false;
    }

    return (0 < xarray_map_impl(array, 0, array->size - 1, true, false, apply, cl));
}

static 
void xarray_free_datas(XArray_PT array) {
    if (0 < array->size) {
        XMEM_FREE(array->datas);
        array->size = 0;
        //should not reset elem_size to 0 here, or the resize function will failed
        //array->elem_size = 0;
    }

    return;
}

void xarray_free(XArray_PT *parray) {
    if (!parray || !*parray) {
        return;
    }

    xarray_free_datas(*parray);

    XMEM_FREE(*parray);

    return;
}

void xarray_clear(XArray_PT array) {
    xassert(array);

    if (!array) {
        return;
    }

    if (0 < array->size) {
        memset(array->datas, 0, (array->size * array->elem_size));
    }

    return;
}

static
bool xarray_remove_impl(XArray_PT array, int start, int end, void *old_data) {
    xassert(array);
    xassert(0 <= start);
    xassert(start <= end);
    xassert(end < array->size);

    if (!array || (start < 0) || (end < start) || (array->size <= end)) {
        return false;
    }

    /* just save the first one */
    if (old_data) {
        memcpy(old_data, array->datas + start * array->elem_size, array->elem_size);
    }

    memset(array->datas + start * array->elem_size, 0, (end - start + 1) * array->elem_size);

    return true;
}

bool xarray_remove(XArray_PT array, int i, void *old_data) {
    return xarray_remove_impl(array, i, i, old_data);
}

int xarray_size(XArray_PT array) {
    return (array ? array->size : 0);
}

int xarray_elem_size(XArray_PT array) {
    return (array ? array->elem_size : 0);
}

bool xarray_is_empty(XArray_PT array) {
    return (array ? (array->size == 0) : true);
}

static 
bool xarray_resize_impl_apply(XArray_PT array, int new_size) {
    xassert(array);
    xassert(0 <= new_size);
    xassert(0 < array->elem_size);

    if (!array || (new_size < 0)) {
        return false;
    }

    if (new_size == array->size) {
        return true;
    }

    if (new_size == 0) {
        xarray_free_datas(array);
    }
    else if (array->size == 0) {
        array->datas = XMEM_CALLOC(new_size, array->elem_size);
        if (!array->datas) {
            return false;
        }
    }
    else {
        char* ndatas = xmem_resize(array->datas, (new_size * array->elem_size), __FILE__, __LINE__);
        if (!ndatas) {
            return false;
        }
        array->datas = ndatas;
    }

    array->size = new_size;

    return true;
}

static 
bool xarray_resize_impl(XArray_PT array, int new_size) {
    int old_len = xarray_size(array);

    if (xarray_resize_impl_apply(array, new_size)) {
        if (old_len < new_size) {
            memset((array->datas + old_len * array->elem_size), 0, (new_size - old_len) * array->elem_size);
        }

        return true;
    }

    return false;
}

bool xarray_resize(XArray_PT array, int new_size) {
    return xarray_resize_impl(array, new_size);
}

static 
bool xarray_remove_resize_impl(XArray_PT array, int i, void *old_data) {
    xassert(array);
    xassert(0 <= i);
    xassert(i < array->size);

    if (!array || (i < 0) || (array->size <= i)) {
        return false;
    }

    xarray_remove_impl(array, i, i, old_data);

    if (i != array->size - 1) {
        memmove(array->datas + i * array->elem_size, array->datas + (i + 1) * array->elem_size, array->size - i - 1);
    }

    xarray_resize_impl_apply(array, array->size - 1);

    return true;
}

bool xarray_remove_resize(XArray_PT array, int i, void *old_data) {
    return xarray_remove_resize_impl(array, i, old_data);
}

bool xarray_swap(XArray_PT array1, XArray_PT array2) {
    xassert(array1);
    xassert(array2);

    if (!array1 || !array2) {
        return false;
    }

    {
        int   size = array1->size;
        int   elem_size = array1->elem_size;
        char *datas = array1->datas;

        array1->size = array2->size;
        array1->elem_size = array2->elem_size;
        array1->datas = array2->datas;

        array2->size = size;
        array2->elem_size = elem_size;
        array2->datas = datas;
    }

    return true;
}

static inline
void xarray_exch_impl(XArray_PT array, int i, int j, void *buffer) {
    /* works fine for i == j, so ignore the judgement for i != j since most of the cases are i != j */
    memcpy(buffer, array->datas + i * array->elem_size, array->elem_size);
    memcpy(array->datas + i * array->elem_size, array->datas + j * array->elem_size, array->elem_size);
    memcpy(array->datas + j * array->elem_size, buffer, array->elem_size);
}

bool xarray_exch(XArray_PT array, int i, int j) {
    xassert(array);
    xassert(0 <= i);
    xassert(i < array->size);
    xassert(0 <= j);
    xassert(j < array->size);

    if (!array || (i < 0) || (array->size <= i) || (j < 0) || (array->size <= j)) {
        return false;
    }

    {
        char *buffer = XMEM_CALLOC(1, array->elem_size);
        if (!buffer) {
            return false;
        }

        xarray_exch_impl(array, i, j, buffer);

        XMEM_FREE(buffer);
    }

    return true;
}

static
bool xarray_is_sorted_impl(XArray_PT array, int lo, int hi, int(*cmp)(void *x, void *y, int elem_size, void *cl), void *cl) {
    for (int i = lo + 1; i <= hi; ++i) {
        if (cmp(array->datas + i * array->elem_size, array->datas + (i - 1) * array->elem_size, array->elem_size, cl) < 0) {
            return false;
        }
    }

    return true;
}

bool xarray_is_sorted(XArray_PT array, int(*cmp)(void *x, void *y, int elem_size, void *cl), void *cl) {
    xassert(array);
    xassert(cmp);

    if (!array || !cmp) {
        return false;
    }

    return xarray_is_sorted_impl(array, 0, array->size - 1, cmp, cl);
}

static
void xarray_insert_sort_impl(XArray_PT array, int lo, int step, int hi, char *buffer, int(*cmp)(void *x, void *y, int elem_size, void *cl), void *cl) {
    for (int i = lo + step; i <= hi; ++i) {
        memcpy(buffer, array->datas + i * array->elem_size, array->elem_size);

        int j = i - step;
        for (; lo <= j; j -= step) {
            /* sorted already */
            if (cmp(array->datas + j * array->elem_size, buffer, array->elem_size, cl) <= 0) {
                break;
            }
            memcpy(array->datas + (j + step) * array->elem_size, array->datas + j * array->elem_size, array->elem_size);
        }

        j += step;
        if (j < i) {
            memcpy(array->datas + j * array->elem_size, buffer, array->elem_size);
        }
    }
}

static
void xarray_shell_sort_impl(XArray_PT array, int lo, int hi, char *buffer, int(*cmp)(void *x, void *y, int elem_size, void *cl), void *cl) {
    int h = 1;
    while (h < (hi - lo) / 3) {
        h = 3 * h + 1;  /* 1, 4, 13, 40, 121, 364, 1093, ... */
    }

    while (1 <= h) {
        xarray_insert_sort_impl(array, lo, h, hi, buffer, cmp, cl);
        h = h / 3;
    }
}

static
int xarray_lg(int n) {
    int k = 0;
    for (; n != 1; n >>= 1) {
        ++k;
    }
    return k;
}

static
void xarray_quick_sort_impl(XArray_PT array, int lo, int hi, int depth_limit, char *value, char *exchange, int(*cmp)(void *x, void *y, int elem_size, void *cl), void *cl) {
    /* for short array, insert sort is faster than quick sort, so use insert sort instead */
    if (hi <= lo + 10) {
        xarray_insert_sort_impl(array, lo, 1, hi, exchange, cmp, cl);
        return;
    }

    if (depth_limit == 0) {
        xarray_shell_sort_impl(array, lo, hi, exchange, cmp, cl);
        return;
    }

    --depth_limit;

    {
        int lt = lo, i = lo + 1, gt = hi;

        memcpy(value, array->datas + lt * array->elem_size, array->elem_size);
        while (i <= gt) {
            int ret = cmp(array->datas + i * array->elem_size, value, array->elem_size, cl);
            if (ret < 0) {
                xarray_exch_impl(array, lt, i, exchange);
                ++lt;
                ++i;
            }
            else if (0 < ret) {
                xarray_exch_impl(array, i, gt, exchange);
                --gt;
            }
            else {
                ++i;
            }
        }

        xarray_quick_sort_impl(array, lo, lt - 1, depth_limit, value, exchange, cmp, cl);
        xarray_quick_sort_impl(array, gt + 1, hi, depth_limit, value, exchange, cmp, cl);
    }
}

bool xarray_quick_sort(XArray_PT array, int(*cmp)(void *x, void *y, int elem_size, void *cl), void *cl) {
    xassert(array);
    xassert(cmp);

    if (!array || !cmp) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    {
        void *exchange = XMEM_CALLOC(1, array->elem_size);
        void *value = XMEM_CALLOC(1, array->elem_size);
        if (!exchange || !value) {
            XMEM_FREE(exchange);
            XMEM_FREE(value);
            return false;
        }

        xarray_quick_sort_impl(array, 0, array->size - 1, xarray_lg(array->size - 1) * 2, value, exchange, cmp, cl);

        XMEM_FREE(exchange);
        XMEM_FREE(value);
    }

    xassert(xarray_is_sorted(array, cmp, cl));

    return true;
}

static
int xarray_pointer_sort_apply(void *x, void *y, void *cl) {
    XArray_Apply_Paras_PT paras = (XArray_Apply_Paras_PT)cl;
    XArray_PT array = paras->array;

    return (paras->apply)(x, y, array->elem_size, paras->cl);
}

XPArray_PT xarray_pointer_sort(XArray_PT array, int(*cmp)(void *x, void *y, int elem_size, void *cl), void *cl) {
    xassert(array);
    xassert(cmp);

    if (!array || !cmp) {
        return NULL;
    }

    {
        XPArray_PT parray = xparray_new(array->size);
        if (!parray) {
            return NULL;
        }

        for (int i = 0; i < array->size; ++i) {
            xparray_put_impl(parray, i, (void*)(array->datas + i * array->elem_size));
        }

        {
            XArray_Apply_Paras_T paras = { array, cmp, cl };
            xparray_quick_sort(parray, xarray_pointer_sort_apply, (void*)&paras);

            xassert(xparray_is_sorted(parray, xarray_pointer_sort_apply, (void*)&paras));
        }

        return parray;
    }
}

/* Note : if we use pointer2 - pointer1 to get the array index, we can use the same method as xarray_index_inplace_sort */
bool xarray_pointer_inplace_sort(XArray_PT array, XPArray_PT pointer_array) {
    xassert(array);
    xassert(pointer_array);
    xassert(array->size == pointer_array->size);

    if (!array || !pointer_array || (array->size != pointer_array->size)) {
        return false;
    }

    {
        XArray_PT narray = xarray_new(array->size, array->elem_size);
        if (!narray) {
            return false;
        }

        for (int i = 0; i < array->size; i++) {
            xarray_put(narray, i, xparray_get_impl(pointer_array, i), NULL);
        }

        xarray_swap(array, narray);

        xarray_free(&narray);

        return true;
    }
}

static
int xarray_index_sort_apply(int x, int y, void *cl) {
    XArray_Apply_Paras_PT paras = (XArray_Apply_Paras_PT)cl;
    XArray_PT array = paras->array;

    return (paras->apply)((void*)(array->datas + x * array->elem_size), (void*)(array->datas + y * array->elem_size), array->elem_size, paras->cl);
}

/* <<Algorithms in C>> Third Edition : Chapter 6.8 */
XIArray_PT xarray_index_sort(XArray_PT array, int(*cmp)(void *x, void *y, int elem_size, void *cl), void *cl) {
    xassert(array);
    xassert(cmp);

    if (!array || !cmp) {
        return NULL;
    }

    {
        XIArray_PT iarray = xiarray_new(array->size);
        if (!iarray) {
            return NULL;
        }

        for (int i = 0; i < array->size; ++i) {
            xiarray_put(iarray, i, i, NULL);
        }

        {
            XArray_Apply_Paras_T paras = { array, cmp, cl };
            xiarray_quick_sort_if(iarray, xarray_index_sort_apply, (void*)&paras);
        }

        return iarray;
    }
}

/* <<Algorithms in C>> Third Edition : Chapter 6.8 */
bool xarray_index_inplace_sort(XArray_PT array, XIArray_PT index_array) {
    xassert(array);
    xassert(index_array);
    xassert(array->size == index_array->size);

    if (!array || !index_array || (array->size != index_array->size)) {
        return false;
    }

    {
        void *x = XMEM_CALLOC(1, array->elem_size);
        if (!x) {
            return false;
        }

        for (int i = 0; i < array->size; i++) {
            memcpy(x, (void*)(array->datas + i * array->elem_size), array->elem_size);

            {
                int k = i;
                int j;
                while (xiarray_get(index_array, k) != i) {
                    j = k;
                    xarray_put(array, k, xarray_get(array, xiarray_get(index_array, k)), NULL);
                    k = xiarray_get(index_array, j);
                    xiarray_put(index_array, j, j, NULL);
                }

                if (i != k) {
                    xarray_put(array, k, x, NULL);
                    xiarray_put(index_array, k, k, NULL);
                }
            }
        }

        XMEM_FREE(x);
    }

    return true;
}

static
int xarray_binary_search_impl(XArray_PT array, void *data, int lo, int hi, int(*cmp)(void *x, void *y, int elem_size, void *cl), void *cl) {
    if (hi < lo) {
        return -1;
    }

    {
        int k = (lo + hi) / 2;

        int result = cmp(data, array->datas + k * array->elem_size, array->elem_size, cl);

        if (result == 0) {
            return k;
        }
        else if (result < 0) {
            return xarray_binary_search_impl(array, data, lo, k - 1, cmp, cl);
        }
        else {
            return xarray_binary_search_impl(array, data, k + 1, hi, cmp, cl);
        }
    }
}

int xarray_binary_search(XArray_PT array, void *data, int(*cmp)(void *x, void *y, int elem_size, void *cl), void *cl) {
    xassert(array);
    xassert(data);
    xassert(cmp);

    if (!array || !data || !cmp || (array->size <= 0)) {
        return -1;
    }

    xassert(xarray_is_sorted(array, cmp, cl));

    return xarray_binary_search_impl(array, data, 0, array->size - 1, cmp, cl);
}
