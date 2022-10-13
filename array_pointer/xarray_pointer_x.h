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

#ifndef XPARRAYX_INCLUDED
#define XPARRAYX_INCLUDED

#include <stdbool.h>

#include "../include/xarray_pointer.h"

struct XPArray {
    void **datas;        /* memory to save the pointers */

    int    size;         /* capacity : how many pointers can be saved */
};

/* O(N) */
extern XPArray_PT    xparray_copyn_impl                      (XPArray_PT array, int start, int count, int elem_size, bool deep);

/* O(N) */
extern XPArray_PT    xparray_scope_copy                      (XPArray_PT array, int start, int end);
extern XPArray_PT    xparray_scope_deep_copy                 (XPArray_PT array, int start, int end, int elem_size);
extern bool          xparray_scope_index_copy                (XPArray_PT array, int start, int end, XPArray_PT darray, int dstart);
extern bool          xparray_scope_index_copy_resize         (XPArray_PT array, int start, int end, XPArray_PT darray, int dstart);
extern bool          xparray_scope_deep_index_copy           (XPArray_PT array, int start, int end, XPArray_PT darray, int dstart, int elem_size);
extern bool          xparray_scope_deep_index_copy_resize    (XPArray_PT array, int start, int end, XPArray_PT darray, int dstart, int elem_size);

/* O(N) */
extern bool          xparray_scope_remove                    (XPArray_PT array, int start, int end);
extern bool          xparray_scope_deep_remove               (XPArray_PT array, int start, int end);

/* O(NlgN) */
extern void          xparray_merge_sort_impl_bottom_up       (XPArray_PT array, XPArray_PT tarray, int lo, int hi, int (*cmp)(void *x, void *y, void *cl), void *cl);
extern void          xparray_merge_sort_impl_no_copy         (XPArray_PT array, XPArray_PT tarray, int lo, int hi, int (*cmp)(void *x, void *y, void *cl), void *cl);
extern void          xparray_merge_sort_impl_up_bottom       (XPArray_PT array, XPArray_PT tarray, int lo, int hi, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(NlgN) */
extern void          xparray_quick_sort_impl_basic_split           (XPArray_PT array, int lo, int hi, int depth_limit, int (*cmp)(void *x, void *y, void *cl), void *cl);
extern void          xparray_quick_sort_impl_random_split          (XPArray_PT array, int lo, int hi, int depth_limit, int (*cmp)(void *x, void *y, void *cl), void *cl);
extern void          xparray_quick_sort_impl_median_of_three_split (XPArray_PT array, int lo, int hi, int depth_limit, int (*cmp)(void *x, void *y, void *cl), void *cl);
extern void          xparray_quick_sort_impl_3_way_split           (XPArray_PT array, int lo, int hi, int depth_limit, int (*cmp)(void *x, void *y, void *cl), void *cl);
extern void          xparray_quick_sort_impl_quick_3_way_split     (XPArray_PT array, int lo, int hi, int depth_limit, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(lgN) */
extern void*         xparray_get_kth_element_impl_quick_sort (XPArray_PT array, int k, int (*cmp)(void *x, void *y, void *cl), void *cl);
extern void*         xparray_get_kth_element_impl_heap_sort  (XPArray_PT array, int k, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(NlgN) */
extern bool          xparray_heapify_impl                    (XPArray_PT array, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl), void *cl);
extern bool          xparray_heap_sort_impl                  (XPArray_PT array, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(N) */
extern bool          xparray_section_is_heap_sorted          (XPArray_PT array, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(1) */
extern bool          xparray_save_and_put_impl(XPArray_PT array, int i, void *data, void **old_data);

/* O(1) */
static inline
void xparray_put_impl(XPArray_PT array, int i, void *data) {
    array->datas[i] = data;
}

/* O(1) */
static inline
void* xparray_get_impl(XPArray_PT array, int i) {
    return array->datas[i];
}

#endif
