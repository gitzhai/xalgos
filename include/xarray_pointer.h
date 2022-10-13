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

#ifndef XPARRAY_INCLUDED
#define XPARRAY_INCLUDED

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XPArray* XPArray_PT;

/* O(1) */
extern XPArray_PT  xparray_new                   (int size);

/* O(N) */
extern XPArray_PT  xparray_copy                  (XPArray_PT array);
extern XPArray_PT  xparray_copyn                 (XPArray_PT array, int count);

/* O(N) */
extern XPArray_PT  xparray_deep_copy             (XPArray_PT array, int elem_size);
extern XPArray_PT  xparray_deep_copyn            (XPArray_PT array, int count, int elem_size);

/* O(N) */
extern int         xparray_vload                 (XPArray_PT array, void *x, ...);
/* O(N) : ( O(1) if no expand happened ) */
extern int         xparray_aload                 (XPArray_PT array, void **xs, int len);

/* O(1) */
extern bool        xparray_put                   (XPArray_PT array, int i, void *data, void **old_data);

/* O(N) : ( O(1) if no expand happened ) */
extern bool        xparray_put_expand            (XPArray_PT array, int i, void *data, void **old_data);
extern bool        xparray_put_fix_expand        (XPArray_PT array, int i, void *data, void **old_data, int new_size);

/* O(N) */
extern bool        xparray_fill                  (XPArray_PT array, void *data);
extern bool        xparray_scope_fill            (XPArray_PT array, int start, int end, void *data);

/* O(1) */
extern void*       xparray_get                   (XPArray_PT array, int i);

/* O(N) : quick partition (O(NlgN) if worst case happened or heap sort method choosed) */
extern void*       xparray_get_kth_element       (XPArray_PT array, int k, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(lgN) */
extern int         xparray_binary_search         (XPArray_PT array, void *data, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(1) */
extern void*       xparray_front                 (XPArray_PT array);
extern void*       xparray_back                  (XPArray_PT array);

/* O(N) */
extern void*       xparray_min                   (XPArray_PT array, int (*cmp)(void *x, void *y, void *cl), void *cl);
extern void*       xparray_max                   (XPArray_PT array, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(N) */
extern int         xparray_map                   (XPArray_PT array, bool (*apply)(void *x, void *cl), void *cl);
extern bool        xparray_map_break_if_true     (XPArray_PT array, bool (*apply)(void *x, void *cl), void *cl);
extern bool        xparray_map_break_if_false    (XPArray_PT array, bool (*apply)(void *x, void *cl), void *cl);

/* O(1) */
extern void        xparray_free                  (XPArray_PT *parray);

/* O(N) */
extern void        xparray_free_apply            (XPArray_PT *parray, bool (*apply)(void *x, void *cl), void *cl);
extern void        xparray_deep_free             (XPArray_PT *parray);

/* O(N) */
extern void        xparray_clear                 (XPArray_PT array);
extern void        xparray_clear_apply           (XPArray_PT array, bool (*apply)(void *x, void *cl), void *cl);
extern void        xparray_deep_clear            (XPArray_PT array);

/* O(1) */
extern bool        xparray_remove                (XPArray_PT array, int i, void **old_data);
extern bool        xparray_remove_apply          (XPArray_PT array, int i, bool (*apply)(void *x, void *cl), void *cl);
extern bool        xparray_deep_remove           (XPArray_PT array, int i);

/* O(N) */
extern bool        xparray_resize                (XPArray_PT array, int new_size);
extern bool        xparray_deep_resize           (XPArray_PT array, int new_size);

/* O(N) */
extern bool        xparray_remove_resize         (XPArray_PT array, int i, void **old_data);
extern bool        xparray_deep_remove_resize    (XPArray_PT array, int i);

/* O(1) */
extern int         xparray_size                  (XPArray_PT array);
extern bool        xparray_is_empty              (XPArray_PT array);

/* O(1) */
extern bool        xparray_swap                  (XPArray_PT array1, XPArray_PT array2);

/* O(1) */
extern bool        xparray_exch                  (XPArray_PT array, int i, int j);

/* O(N) */
extern bool        xparray_is_sorted             (XPArray_PT array, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(N^2) */
extern bool        xparray_bubble_sort           (XPArray_PT array, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(N^2) : take almost the same efforts for the sorted array */
extern bool        xparray_select_sort           (XPArray_PT array, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(N)   : best  - for sorted array
 * O(N^2) : worst - for reverse order array
 */
extern bool        xparray_insert_sort           (XPArray_PT array, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(N^(3/2)) : very fast for the sorted array, efficient for big random array too */
extern bool        xparray_shell_sort            (XPArray_PT array, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(NlgN) : need additional space to temporarily save intermediate data */
extern bool        xparray_merge_sort            (XPArray_PT array, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(NlgN) : very fast for the random array */
extern bool        xparray_quick_sort            (XPArray_PT array, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(NlgN) : very fast for the sorted array, efficient for big random array too, can't use memory cache may slow the speed */
extern bool        xparray_heap_sort             (XPArray_PT array, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(N) */
extern bool        xparray_is_heap_sorted        (XPArray_PT array, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(N) : <<Introduction to Algorithms>> Third Edtion, chapter 6.3 */
extern bool        xparray_heapify_min           (XPArray_PT array, int (*cmp)(void *x, void *y, void *cl), void *cl);
extern bool        xparray_heapify_max           (XPArray_PT array, int (*cmp)(void *x, void *y, void *cl), void *cl);

#ifdef __cplusplus
}
#endif

#endif
