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
#include <limits.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../utils/xutils.h"
#include "../include/xarith_int.h"
#include "../array_pointer/xarray_pointer_x.h"
#include "../queue_sequence_int/xqueue_sequence_int_x.h"
#include "../utils/xutils.h"
#include "xarray_int_x.h"

XIArray_PT xiarray_new(int size) {
    xassert(0 <= size);

    if (size < 0) {
        return NULL;
    }

    {
        XIArray_PT array = XMEM_CALLOC(1, sizeof(*array));
        if (!array) {
            return NULL;
        }

        if (0 < size) {
            array->datas = XMEM_CALLOC(size, sizeof(int));
            if (!array->datas) {
                XMEM_FREE(array);
                return NULL;
            }
        }

        array->size = size;

        return array;
    }
}

XIArray_PT xiarray_copyn_impl(XIArray_PT array, int start, int count) {
    XIArray_PT narray = xiarray_new(array->size);
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

    memcpy(narray->datas, array->datas + start, (count * sizeof(int)));

    return narray;
}

XIArray_PT xiarray_copy(XIArray_PT array) {
    return xiarray_copyn(array, (array ? array->size : 0));
}

XIArray_PT xiarray_copyn(XIArray_PT array, int count) {
    xassert(array);
    xassert(0 <= count);

    if (!array || (count < 0)) {
        return NULL;
    }

    return xiarray_copyn_impl(array, 0, count);
}

XIArray_PT xiarray_scope_copy(XIArray_PT array, int start, int end) {
    xassert(array);
    xassert(0 <= start);
    xassert(start <= end);
    xassert(end < array->size);

    if (!array || (start < 0) || (end < start) || (array->size <= end)) {
        return NULL;
    }

    return xiarray_copyn_impl(array, start, (end - start + 1));
}

bool xiarray_scope_index_copy(XIArray_PT array, int start, int end, XIArray_PT darray, int dstart) {
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

    memcpy(darray->datas + dstart, (array->datas + start), ((end - start + 1) * sizeof(int)));

    return true;
}

bool xiarray_scope_index_copy_resize(XIArray_PT array, int start, int end, XIArray_PT darray, int dstart) {
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
            xiarray_resize(darray, new_size);
        }
    }

    memcpy(darray->datas + dstart, (array->datas + start), ((end - start + 1) * sizeof(int)));

    return true;
}

int xiarray_aload(XIArray_PT array, int *xs, int len) {
    xassert(xs);
    xassert(0 <= len);

    if (!xs || (len < 0)) {
        return 0;
    }

    {
        int count = 0;

        for (; count < len; count++) {
            if (!xiarray_put_expand(array, count, xs[count], NULL)) {
                break;
            }
        }

        return count;
    }
}

bool xiarray_save_and_put_impl(XIArray_PT array, int i, int data, int *old_data) {
    if (old_data) {
        *old_data = array->datas[i];
    }

    xiarray_put_impl(array, i, data);

    return true;
}

bool xiarray_put(XIArray_PT array, int i, int data, int *old_data) {
    xassert(array);
    xassert(0 <= i);
    xassert(i < array->size);

    if (!array || (i < 0) || (array->size <= i)) {
        return false;
    }

    return xiarray_save_and_put_impl(array, i, data, old_data);
}

void xiarray_free_datas(XIArray_PT array) {
    if (0 < array->size) {
        XMEM_FREE(array->datas);
        array->size = 0;
    }
}

static
bool xiarray_resize_impl_apply(XIArray_PT array, int new_size) {
    xassert(array);
    xassert(0 <= new_size);

    if (!array || (new_size < 0)) {
        return false;
    }

    if (new_size == array->size) {
        return true;
    }

    if (new_size == 0) {
        xiarray_free_datas(array);
    }
    else if (array->size == 0) {
        array->datas = XMEM_CALLOC(new_size, sizeof(int));
        if (!array->datas) {
            return false;
        }
    }
    else {
        void* ndatas = xmem_resize(array->datas, (new_size * sizeof(int)), __FILE__, __LINE__);
        if (!ndatas) {
            return false;
        }
        array->datas = ndatas;
    }

    array->size = new_size;

    return true;
}

static
bool xiarray_resize_impl(XIArray_PT array, int new_size) {
    int old_len = array ? array->size : 0;

    if (xiarray_resize_impl_apply(array, new_size)) {
        if (old_len < new_size) {
            memset((array->datas + old_len), 0, (new_size - old_len) * sizeof(int));
        }

        return true;
    }

    return false;
}

bool xiarray_resize(XIArray_PT array, int new_size) {
    return xiarray_resize_impl(array, new_size);
}

static
bool xiarray_put_expand_impl(XIArray_PT array, int i, int data, int *old_data, int new_size) {
    if (array->size <= i) {
        if (!xiarray_resize_impl(array, new_size)) {
            return false;
        }
    }

    return xiarray_save_and_put_impl(array, i, data, old_data);
}

bool xiarray_put_expand(XIArray_PT array, int i, int data, int *old_data) {
    xassert(array);
    xassert(0 <= i);

    if (!array || (i < 0)) {
        return false;
    }

    return xiarray_put_expand_impl(array, i, data, old_data, (i + XUTILS_ARRAY_EXPAND_DEFAULT_LENGTH));
}

bool xiarray_put_fix_expand(XIArray_PT array, int i, int data, int *old_data, int new_size) {
    xassert(array);
    xassert(0 <= i);
    xassert(i < new_size);
    xassert(array->size <= new_size);

    if (!array || (i < 0) || (new_size <= i) || (new_size < array->size)) {
        return false;
    }

    return xiarray_put_expand_impl(array, i, data, old_data, new_size);
}

bool xiarray_scope_fill(XIArray_PT array, int start, int end, int data) {
    xassert(array);
    xassert(0 <= start);
    xassert(start <= end);
    xassert(end < array->size);

    if (!array || (start < 0) || (end < start) || (array->size <= end) || (array->size == 0)) {
        return false;
    }

    if (data == 0) {
        memset(array->datas, 0, (end - start + 1) * sizeof(int));
    }
    else {
        for (int i = start; i <= end; i++) {
            array->datas[i] = data;
        }
    }

    return true;
}

bool xiarray_fill(XIArray_PT array, int data) {
    return xiarray_scope_fill(array, 0, xiarray_size(array) - 1, data);
}

int xiarray_get(XIArray_PT array, int i) {
    xassert(array);
    xassert(0 <= i);
    xassert(i < array->size);

    if (!array || (i < 0) || (array->size <= i)) {
        return 0;
    }

    return array->datas[i];
}

int  xiarray_front(XIArray_PT array) {
    return xiarray_get(array, 0);
}

int xiarray_back(XIArray_PT array) {
    return xiarray_get(array, (xiarray_size(array) - 1));
}

/* << Introduction to Algorithms >> Third Edition, Chapter 9.1 */
int xiarray_min(XIArray_PT array) {
    xassert(array);
    xassert(0 < array->size);

    if (!array || (array->size <= 0)) {
        return INT_MIN;
    }

    {
        int min = array->datas[0];

        for (int i = 1; i < array->size; ++i) {
            if (array->datas[i] < min) {
                min = array->datas[i];
            }
        }

        return min;
    }
}

/* << Introduction to Algorithms >> Third Edition, Chapter 9.1 */
int xiarray_max(XIArray_PT array) {
    xassert(array);
    xassert(0 < array->size);

    if (!array || (array->size <= 0)) {
        return INT_MAX;
    }

    {
        int max = array->datas[0];

        for (int i = 1; i < array->size; ++i) {
            if (max < array->datas[i]) {
                max = array->datas[i];
            }
        }

        return max;
    }
}

/* << Introduction to Algorithms >> Third Edition, Chapter 9.1 */
int xiarray_min_max(XIArray_PT array, int *min, int *max) {
    xassert(array);
    xassert(0 < array->size);
    xassert(min);
    xassert(max);

    if (!array || (array->size <= 0) || !min || !max) {
        return -1;
    }

    if (array->size == 1) {
        *min = array->datas[0];
        *max = array->datas[0];
        return 0;
    }

    {
        int i = 0;

        /* when array has even number of elements, use index 0 & 1 as the min/max elements */
        if (array->size % 2 == 0) {
            if (array->datas[0] <= array->datas[1]) {
                *min = array->datas[0];
                *max = array->datas[1];
            }
            else {
                *min = array->datas[1];
                *max = array->datas[0];
            }
            i = 2;
        }
        /* when array has odd number of elements, use index 0 as the min/max elements */
        else {
            *min = array->datas[0];
            *max = array->datas[0];
            i = 1;
        }
        
        /* compare the pair of elements at first, then compare the smaller element with min,
         * and compare the bigger element with max
         */
        for (; i < array->size; i += 2) {
            if (array->datas[i] <= array->datas[i + 1]) {
                if (array->datas[i] < *min) {
                    *min = array->datas[i];
                }
                if (*max < array->datas[i + 1]) {
                    *max = array->datas[i + 1];
                }
            }
            else {
                if (array->datas[i + 1] < *min) {
                    *min = array->datas[i + 1];
                }
                if (*max < array->datas[i]) {
                    *max = array->datas[i];
                }
            }
        }

        return 0;
    }
}

static
int xiarray_map_impl(XIArray_PT array, int m, int n, bool break_first, bool break_true, bool (*apply)(int x, void *cl), void *cl) {
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

int xiarray_map(XIArray_PT array, bool (*apply)(int x, void *cl), void *cl) {
    return xiarray_map_impl(array, 0, xiarray_size(array) - 1, false, false, apply, cl);
}

bool xiarray_map_break_if_true(XIArray_PT array, bool (*apply)(int x, void *cl), void *cl) {
    return (0 < xiarray_map_impl(array, 0, xiarray_size(array) - 1, true, true, apply, cl));
}

bool xiarray_map_break_if_false(XIArray_PT array, bool (*apply)(int x, void *cl), void *cl) {
    return (0 < xiarray_map_impl(array, 0, xiarray_size(array) - 1, true, false, apply, cl));
}

void xiarray_free(XIArray_PT *parray) {
    if (!parray || !*parray) {
        return;
    }

    xiarray_free_datas(*parray);
    XMEM_FREE(*parray);
}

void xiarray_clear(XIArray_PT array) {
    xassert(array);

    if (!array) {
        return;
    }

    if (0 < array->size) {
       memset(array->datas, 0, (array->size * sizeof(int)));
    }
}

static
bool xiarray_remove_impl(XIArray_PT array, int start, int end, int *old_data) {
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

    memset(array->datas + start, 0, ((end - start + 1) * sizeof(int)));

    return true;
}

bool xiarray_remove(XIArray_PT array, int i, int *old_data) {
    return xiarray_remove_impl(array, i, i, old_data);
}

bool xiarray_scope_remove(XIArray_PT array, int start, int end) {
    return xiarray_remove_impl(array, start, end, NULL);
}

int xiarray_size(XIArray_PT array) {
    return (array ? array->size : 0);
}

bool xiarray_is_empty(XIArray_PT array) {
    return (array ? (array->size == 0) : true);
}

bool xiarray_remove_resize(XIArray_PT array, int i, int *old_data) {
    xiarray_remove_impl(array, i, i, old_data);

    if (i != array->size - 1) {
        memmove(array->datas + i, array->datas + i + 1, array->size - i - 1);
    }

    if (i != array->size - 1) {
        for (int k = i; k < array->size - 1; ++k) {
            array->datas[k] = array->datas[k + 1];
        }
        array->datas[array->size - 1] = 0;
    }

    return xiarray_resize_impl_apply(array, array->size - 1);
}

bool xiarray_swap(XIArray_PT array1, XIArray_PT array2) {
    xassert(array1);
    xassert(array2);

    if (!array1 || !array2) {
        return false;
    }

    {
        int   size = array1->size;
        int  *datas = array1->datas;

        array1->size = array2->size;
        array1->datas = array2->datas;

        array2->size = size;
        array2->datas = datas;
    }

    return true;
}

static inline 
void xiarray_exch_impl(XIArray_PT array, int i, int j) {
    /* works fine for i == j, so ignore the judgement for i != j since most of the cases are i != j */
    int x = array->datas[i];
    array->datas[i] = array->datas[j];
    array->datas[j] = x;
}

bool xiarray_exch(XIArray_PT array, int i, int j) {
    xassert(array);
    xassert(0 <= i);
    xassert(i < array->size);
    xassert(0 <= j);
    xassert(j < array->size);

    if (!array || (i < 0) || (array->size <= i) || (j < 0) || (array->size <= j)) {
        return false;
    }

    xiarray_exch_impl(array, i, j);

    return true;
}

static
bool xiarray_is_sorted_impl(XIArray_PT array, int lo, int hi) {
    for (int i = lo + 1; i <= hi; ++i) {
        if ((array->datas)[i] < (array->datas)[i - 1]) {
            return false;
        }
    }

    return true;
}

static
bool xiarray_is_sorted_if_impl(XIArray_PT array, int lo, int hi, int(*cmp)(int x, int y, void *cl), void *cl) {
    for (int i = lo + 1; i <= hi; ++i) {
        if (cmp((array->datas)[i], (array->datas)[i - 1], cl) < 0) {
            return false;
        }
    }

    return true;
}

bool xiarray_is_sorted(XIArray_PT array) {
    xassert(array);

    if (!array) {
        return false;
    }

    return xiarray_is_sorted_impl(array, 0, array->size - 1);
}

static
void xiarray_insert_sort_impl(XIArray_PT array, int lo, int step, int hi) {
    /*  lo     x                 hi
    *    |-|-|-|-|-|-|-|-|-|-|-|-|
    *          i-->
    *     <--j
    */
    for (int i = lo + step; i <= hi; ++i) {
        int x = array->datas[i];

        int j = i - step;
        for (; lo <= j; j -= step) {
            /* sorted already */
            if (array->datas[j] <= x) {
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

static
void xiarray_shell_sort_impl(XIArray_PT array, int lo, int hi) {
    int h = 1;
    int num = hi - lo + 1;

    while (h < num / 3) {
        h = 3 * h + 1;  /* 1, 4, 13, 40, 121, 364, 1093, ... */
    }

    while (1 <= h) {
        xiarray_insert_sort_impl(array, lo, h, hi);
        h = h / 3;
    }
}

bool xiarray_shell_sort(XIArray_PT array) {
    xassert(array);

    if (!array) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    xiarray_shell_sort_impl(array, 0, array->size - 1);

    xassert(xiarray_is_sorted(array));

    return true;
}

static
void xiarray_merge_impl(XIArray_PT array, XIArray_PT tarray, int lo, int mid, int hi) {
    /* check if in order already */
    if (array->datas[mid] <= array->datas[mid + 1]) {
        return;
    }

    /* backup the [lo, hi] part at first */
    memcpy(tarray->datas + lo, array->datas + lo, ((hi - lo + 1) * sizeof(int)));

    {
        int i = lo, j = mid + 1;
        for (int k = lo; k <= hi; ++k) {
            if (mid < i) {      /* left parts are all in array already */
                /* array->datas[k] = tarray->datas[j++]; */
                memcpy(array->datas + k, tarray->datas + j, ((hi - k + 1) * sizeof(int)));
                break;
            }
            else if (hi < j) {  /* right parts are all in array already */
                /* array->datas[k] = tarray->datas[i++]; */
                memcpy(array->datas + k, tarray->datas + i, ((hi - k + 1) * sizeof(int)));
                break;
            }
            else if (tarray->datas[j] < tarray->datas[i]) {
                array->datas[k] = tarray->datas[j++];
            }
            else {
                array->datas[k] = tarray->datas[i++];
            }
        }
    }
}

static
void xiarray_merge_sort_impl(XIArray_PT array, XIArray_PT tarray, int lo, int hi) {
    /* for short array, insert sort is faster, so use insert sort here instead of :
    *   if (hi <= lo) {
    *       return;
    *   }
    */
    if (hi <= lo + 10) {
        xiarray_insert_sort_impl(array, lo, 1, hi);
        return;
    }

    {
        int mid = lo + (hi - lo) / 2;
        xiarray_merge_sort_impl(array, tarray, lo, mid);       /* sort the [lo, mid] part */
        xiarray_merge_sort_impl(array, tarray, mid + 1, hi);   /* sort the [mid+1, hi] part */

        xiarray_merge_impl(array, tarray, lo, mid, hi);        /* merge the sorted [lo, mid] and [mid+1, hi] part */
    }
}

bool xiarray_merge_sort(XIArray_PT array) {
    xassert(array);

    if (!array) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    {
        XIArray_PT narray = xiarray_new(array->size);
        if (!narray) {
            return false;
        }

        xiarray_merge_sort_impl(array, narray, 0, array->size - 1);

        xiarray_free(&narray);
    }

    xassert(xiarray_is_sorted(array));

    return true;
}

#define xiarray_heap_left(npos)   (((npos) << 1) + 1)
#define xiarray_heap_right(npos)  (((npos) << 1) + 2)

static
bool xiarray_section_is_heap_sorted_impl(XIArray_PT array, int k, int lo, int hi, bool minheap) {
    if (hi <= lo + k) {
        return true;
    }

    {
        int lpos = xiarray_heap_left(k);
        int rpos = xiarray_heap_right(k);

        /* check left branch */
        if (hi < lo + lpos) {
            return true;
        }

        if (minheap) {
            if (array->datas[lo + lpos] < array->datas[lo + k]) {
                return false;
            }
        }
        else {
            if (array->datas[lo + k] < array->datas[lo + lpos]) {
                return false;
            }
        }

        /* check right branch */
        if (hi < lo + rpos) {
            return true;
        }

        if (minheap) {
            if (array->datas[lo + rpos] < array->datas[lo + k]) {
                return false;
            }
        }
        else {
            if (array->datas[lo + k] < array->datas[lo + rpos]) {
                return false;
            }
        }

        /* continue to check the children of left and right branches */
        return xiarray_section_is_heap_sorted_impl(array, lpos, lo, hi, minheap) && xiarray_section_is_heap_sorted_impl(array, rpos, lo, hi, minheap);
    }
}

/* index lo is the maximum one */
bool xiarray_section_is_heap_sorted(XIArray_PT array, int lo, int hi, bool minheap) {
    xassert(array);
    xassert(0 <= lo);
    xassert(lo <= hi);
    xassert(hi < array->size);

    if (!array || (lo < 0) || (hi < lo) || (array->size <= hi)) {
        return false;
    }

    return xiarray_section_is_heap_sorted_impl(array, 0, lo, hi, minheap);
}

bool xiarray_is_heap_sorted(XIArray_PT array) {
    xassert(array);

    if (!array) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    return xiarray_section_is_heap_sorted_impl(array, 0, 0, array->size - 1, true);
}

static
void xiarray_heapify_sink_elem(XIArray_PT array, int k, int lo, int hi, bool minheap) {
    int left_child = 0;
    int right_child = 0;
    int child = 0;

    while (true) {
        left_child = lo + xiarray_heap_left(k);
        if (hi < left_child) {
            break;
        }

        right_child = left_child + 1;
        child = left_child;

        if (minheap) {
            /* right child is smaller than left child */
            if ((right_child <= hi) && (array->datas[right_child] < array->datas[left_child])) {
                child = right_child;
            }

            /* k is smaller than child, do nothing */
            if (array->datas[lo + k] <= array->datas[child]) {
                break;
            }
        }
        else {
            /* right child is bigger than left child */
            if ((right_child <= hi) && (array->datas[left_child] < array->datas[right_child])) {
                child = right_child;
            }

            /* bigger child is not bigger than k, do nothing */
            if (array->datas[child] <= array->datas[lo + k]) {
                break;
            }
        }

        /* change the child with k */
        xiarray_exch_impl(array, (lo + k), child);

        /* prepare the next round compare */
        k = child - lo;
    }
}

bool xiarray_heapify_impl(XIArray_PT array, int lo, int hi, bool minheap) {
    /* just need to scan half of the array */
    for (int k = (hi - lo) / 2; 0 <= k; --k) {
        xiarray_heapify_sink_elem(array, k, lo, hi, minheap);
    }

    xassert(xiarray_section_is_heap_sorted(array, lo, hi, minheap));

    return true;
}

/* min heap */
bool xiarray_heapify_min(XIArray_PT array) {
    xassert(array);

    if (!array) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    return xiarray_heapify_impl(array, 0, array->size - 1, true);
}

/* max heap */
bool xiarray_heapify_max(XIArray_PT array) {
    xassert(array);

    if (!array) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    return xiarray_heapify_impl(array, 0, array->size - 1, false);
}

static 
bool xiarray_heap_sort_impl(XIArray_PT array, int lo, int hi, bool minheap) {
    int ohi = hi;

    /* make the array a heap*/
    xiarray_heapify_impl(array, lo, hi, !minheap);

    /* make the scope sorted : save the min/max element one bye one at the end of the scope */
    {
        while (lo < hi) {
            xiarray_exch_impl(array, lo, hi);
            --hi;
            xiarray_heapify_sink_elem(array, 0, lo, hi, !minheap);
        }
    }

    xassert(xiarray_is_sorted_impl(array, lo, ohi));

    return true;
}

bool xiarray_heap_sort(XIArray_PT array) {
    xassert(array);

    if (!array) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    return xiarray_heap_sort_impl(array, 0, array->size - 1, true);
}

static
void xiarray_quick_sort_impl(XIArray_PT array, int lo, int hi, int depth_limit) {
    /* for short array, insert sort is faster than quick sort, so use insert sort instead */
    if (hi <= lo + 10) {
        xiarray_insert_sort_impl(array, lo, 1, hi);
        return;
    }

    if (depth_limit <= 0) {
        xiarray_heap_sort_impl(array, lo, hi, true);
        return;
    }

    --depth_limit;

    {
        int lt = lo, i = lo + 1, gt = hi;

        int val = array->datas[lt];
        while (i <= gt) {
            if (array->datas[i] < val) {
                xiarray_exch_impl(array, lt, i);
                ++lt;
                ++i;
            }
            else if (val < array->datas[i]) {
                xiarray_exch_impl(array, i, gt);
                --gt;
            }
            else {
                ++i;
            }
        }

        xiarray_quick_sort_impl(array, lo, lt - 1, depth_limit);
        xiarray_quick_sort_impl(array, gt + 1, hi, depth_limit);
    }
}

bool xiarray_quick_sort(XIArray_PT array) {
    xassert(array);

    if (!array) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    xiarray_quick_sort_impl(array, 0, array->size - 1, xiarith_lg(array->size - 1) * 2);

    xassert(xiarray_is_sorted(array));

    return true;
}

static
void xiarray_insert_sort_if_impl(XIArray_PT array, int lo, int step, int hi, int(*cmp)(int x, int y, void *cl), void *cl) {
    for (int i = lo + step; i <= hi; ++i) {
        int x = array->datas[i];

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

static
void xiarray_shell_sort_if_impl(XIArray_PT array, int lo, int hi, int(*cmp)(int x, int y, void *cl), void *cl) {
    int h = 1;
    int num = hi - lo + 1;

    while (h < num / 3) {
        h = 3 * h + 1;  /* 1, 4, 13, 40, 121, 364, 1093, ... */
    }

    while (1 <= h) {
        xiarray_insert_sort_if_impl(array, lo, h, hi, cmp, cl);
        h = h / 3;
    }
}

static
void xiarray_quick_sort_if_impl(XIArray_PT array, int lo, int hi, int depth_limit, int(*cmp)(int x, int y, void *cl), void *cl) {
    /* for short array, insert sort is faster than quick sort, so use insert sort instead */
    if (hi <= lo + 10) {
        xiarray_insert_sort_if_impl(array, lo, 1, hi, cmp, cl);
        return;
    }

    if (depth_limit <= 0) {
        xiarray_shell_sort_if_impl(array, lo, hi, cmp, cl);
        return;
    }

    --depth_limit;

    {
        int lt = lo, i = lo + 1, gt = hi;

        int val = array->datas[lt];
        while (i <= gt) {
            int ret = cmp(array->datas[i], val, cl);
            if (ret < 0) {
                xiarray_exch_impl(array, lt, i);
                ++lt;
                ++i;
            }
            else if (0 < ret) {
                xiarray_exch_impl(array, i, gt);
                --gt;
            }
            else {
                ++i;
            }
        }

        xiarray_quick_sort_if_impl(array, lo, lt - 1, depth_limit, cmp, cl);
        xiarray_quick_sort_if_impl(array, gt + 1, hi, depth_limit, cmp, cl);
    }
}

bool xiarray_quick_sort_if(XIArray_PT array, int(*cmp)(int x, int y, void *cl), void *cl) {
    xassert(array);
    xassert(cmp);

    if (!array || !cmp) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    xiarray_quick_sort_if_impl(array, 0, array->size - 1, xiarith_lg(array->size - 1) * 2, cmp, cl);

    xassert(xiarray_is_sorted_if_impl(array, 0, array->size - 1, cmp, cl));

    return true;
}

static
bool xiarray_no_negative_values(XIArray_PT array) {
    for (int i = 0; i < array->size; ++i) {
        if (array->datas[i] < 0) {
            return false;
        }
    }

    return true;
}

/* <<Algorithms in C>> Third Edition, Chapter 6.10 */
/* <<Introduction to Algorithms>> Third Edition, Chapter 8.1 */
bool xiarray_counting_sort(XIArray_PT array) {
    xassert(array);

    if (!array) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    /* negative values are not allowed to use this method*/
    if (!xiarray_no_negative_values(array)) {
        return false;
    }

    {
        /* get the max value */
        int max = xiarray_max(array);

        /* used to find the right copy start position for different values */
        XIArray_PT count = xiarray_new(max + 2);
        /* used to save the sorted values */
        XIArray_PT narray = xiarray_new(array->size);

        if (!count || !narray) {
            xiarray_free(count ? &count : NULL);
            xiarray_free(narray ? &narray : NULL);
            return false;
        }

        /* the example digist list is : 
        *  index : 0 1 2 3 4 5 6 7 8 9
        *  value : 1 0 2 1 2 3 1 4 1 4
        */

        /* 1. intialize to 0 */
        memset(count->datas, 0, count->size * sizeof(int));

        /* 2. count the numbers of different values : index 0 reserved
        *  count->datas[0] = 0
        *  count->datas[1] = "1 : total number of 0"
        *  count->datas[2] = "4 : total number of 1"
        *  count->datas[3] = "2 : total number of 2"
        *  count->datas[4] = "1 : total number of 3"
        *  count->datas[5] = "2 : total number of 4"
        *  ......
        */
        for (int i = 0; i < array->size; ++i) {
            count->datas[array->datas[i] + 1]++;
        }

        /* 3. get the start position for each different value
        *  count->datas[0] = 0
        *  count->datas[1] = "1  : total number of 0"
        *  count->datas[2] = "5  : total number of 0 + 1"
        *  count->datas[3] = "7  : total number of 0 + 1 + 2"
        *  count->datas[4] = "8  : total number of 0 + 1 + 2 + 3"
        *  count->datas[5] = "10 : total number of 0 + 1 + 2 + 3 + 4"
        *  ......
        */
        for (int i = 1; i < count->size; ++i) {
            count->datas[i] += count->datas[i - 1];
        }

        /* 4. put the value into right position 
        * 0 -> count->datas[0] = 0
        * 1 -> count->datas[1] = "1  : total number of 0"
        * 2 -> count->datas[2] = "5  : total number of 0 + 1"
        * 3 -> count->datas[3] = "7  : total number of 0 + 1 + 2"
        * 4 -> count->datas[4] = "8  : total number of 0 + 1 + 2 + 3"
        *  ......
        */
        for (int i = 0; i < array->size; ++i) {
            narray->datas[count->datas[array->datas[i]]++] = array->datas[i];
        }

        /* 5. exchange the contents of original array and sorted array */
        xiarray_swap(array, narray);

        xiarray_free(&narray);
        xiarray_free(&count);
    }

    xassert(xiarray_is_sorted(array));

    return true;
}

static 
void xiarray_bucket_sort_free(XPArray_PT array) {
    for (int j = 0; j < array->size; ++j) {
        XISeq_PT iseq = xparray_get_impl(array, j);
        if (iseq) {
            xiseq_free(&iseq);
        }
    }
    xparray_free(&array);
}

/* <<Introduction to Algorithms>> Third Edition, Chapter 8.4 */
bool xiarray_bucket_sort(XIArray_PT array, int bucket_num) {
    xassert(array);
    xassert(1 < bucket_num);

    if (!array || (bucket_num <= 1)) {
        return false;
    }

    if (array->size <= 1) {
        return true;
    }

    {
        int min = 0, max = 0;
        int section_size = 0;

        XPArray_PT narray = xparray_new(bucket_num + 1);
        if (!narray) {
            return false;
        }

        xiarray_min_max(array, &min, &max);
        section_size = (max - min + 1) / bucket_num + 1;

        /* 1. save all the data into the bucket */
        for (int i = 0; i < array->size; ++i) {
            int diff = array->datas[i] - min;
            int index = diff/section_size + ((section_size < diff) ? (diff % section_size == 0 ? 0 : 1) : 0);

            /* since there is no list for int in this library, we have to use int sequence here */
            XISeq_PT iseq = xparray_get_impl(narray, index);
            if (!iseq) {
                iseq = xiseq_new(2*array->size/bucket_num);
                if (!iseq) {
                    xiarray_bucket_sort_free(narray);
                    return false;
                }
                xparray_put_impl(narray, index, iseq);
            }
            if (xiseq_is_full(iseq)) {
                if (!xiseq_expand(iseq, XUTILS_SEQ_EXPAND_DEFAULT_LENGTH)) {
                    xiarray_bucket_sort_free(narray);
                    return false;
                }
            }
            xiseq_push_back_impl(iseq, array->datas[i]);
        }

        /* 2. sort each of the bucket */
        for (int i = 0; i < narray->size; ++i) {
            XISeq_PT iseq = xparray_get_impl(narray, i);
            if (iseq && !xiseq_quick_sort(iseq)) {
                xiarray_bucket_sort_free(narray);
                return false;
            }
        }

        /* 3. save all data in the bucket back to input array one by one */
        int k = 0;
        for (int i = 0; i < narray->size; ++i) {
            XISeq_PT iseq = xparray_get_impl(narray, i);
            int j = xiseq_size(iseq);
            for (; 0 < j; --j) {
                array->datas[k++] = xiseq_pop_front(iseq);
            }
        }

        xassert(k == array->size);
        xiarray_bucket_sort_free(narray);
    }

    xassert(xiarray_is_sorted(array));
    return true;
}

/*  standard quick sort method : <<Algorithms>> Fourth Edition, chapter 2.3.1
*
 *  lo                       hi
 *  |v|  <=v  |--?--|  >=v   |       -->   |  <=val  | val |  >= val |
 *    i-->                <--j
 *
 */
static
int xiarray_quick_sort_impl_basic_split_impl(XIArray_PT array, int lo, int hi) {
    int i = lo, j = hi + 1;

    int val = array->datas[lo];
    while (true) {
        /* find one bigger(>=) element from left index :
        *    it's better to stop moving while "datas[++i] >= val", the same principle for next step "datas[--j] <= val",
        *    it may exchange the equal value sometimes, but it can avoid N^2 performance in extreme scenarios :
        *       it can decrease the change to let "val" to stay in index "lo" always
        */
        while (array->datas[++i] < val) {
            if (i == hi) {
                break;
            }
        }

        /* find one smaller(<=) element from right index */
        while (val < array->datas[--j]);

        /* all elements are right splitted already */
        if (j <= i) {
            break;
        }

        /* exchange the left side bigger element with the right side smaller element */
        xiarray_exch_impl(array, i, j);
    }

    /* move the split element to the target postion to make its left side elements smaller and right side elements bigger */
    xiarray_exch_impl(array, lo, j);

    return j;
}

static 
int xiarray_get_kth_element_impl_quick_sort(XIArray_PT array, int k) {
    int lo = 0;
    int hi = array->size - 1;

    while (lo < hi) {
        int j = xiarray_quick_sort_impl_basic_split_impl(array, lo, hi);
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

    return array->datas[k];
}

static 
int xiarray_get_kth_element_impl_heap_sort(XIArray_PT array, int k) {
    if (array->size <= 2 * k) {
        /* make the array a max heap*/
        xiarray_heapify_impl(array, 0, array->size - 1, false);

        /* save the max element one bye one at the end of the scope */
        {
            int hi = array->size - 1;

            while (0 < hi) {
                xiarray_exch_impl(array, 0, hi);
                if (hi == k) {
                    return array->datas[k];
                }

                --hi;
                xiarray_heapify_sink_elem(array, 0, 0, hi, false);
            }
        }
    }
    else {
        /* make the array a min heap*/
        xiarray_heapify_impl(array, 0, array->size - 1, true);

        /* save the min element one bye one at the end of the scope */
        {
            int hi = array->size - 1;
            int count = 0;

            while (0 < hi) {
                xiarray_exch_impl(array, 0, hi);
                if (count == k) {
                    return array->datas[hi];
                }

                ++count;
                --hi;
                xiarray_heapify_sink_elem(array, 0, 0, hi, true);
            }
        }
    }

    /* not possible to be here */
    xassert(false);
    return 0;
}

int xiarray_get_kth_element(XIArray_PT array, int k) {
    xassert(array);
    xassert(0 <= k);
    xassert(k < array->size);

    if (!array || (k < 0) || (array->size <= k)) {
        return 0;
    }

#if defined(GET_KTH_ELEMENT_QUICK_SORT)
    return xiarray_get_kth_element_impl_quick_sort(array, k);
#else
    return xiarray_get_kth_element_impl_heap_sort(array, k);
#endif
}

static
int xiarray_binary_search_impl(XIArray_PT array, int data, int lo, int hi) {
    if (hi < lo) {
        return -1;
    }

    {
        int k = (lo + hi) / 2;

        if (data == array->datas[k]) {
            return k;
        }
        else if (data < array->datas[k]) {
            return xiarray_binary_search_impl(array, data, lo, k - 1);
        }
        else {
            return xiarray_binary_search_impl(array, data, k + 1, hi);
        }
    }
}

int xiarray_binary_search(XIArray_PT array, int data) {
    xassert(array);
    xassert(data);

    if (!array || !data || (array->size <= 0)) {
        return -1;
    }

    xassert(xiarray_is_sorted(array));

    return xiarray_binary_search_impl(array, data, 0, array->size - 1);
}
