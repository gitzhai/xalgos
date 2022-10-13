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

#ifndef XIARRAY_INCLUDED
#define XIARRAY_INCLUDED

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XIArray* XIArray_PT;

/* O(1) */
extern XIArray_PT  xiarray_new                 (int size);

/* O(N) */
extern XIArray_PT  xiarray_copy                (XIArray_PT array);
extern XIArray_PT  xiarray_copyn               (XIArray_PT array, int count);

/* O(N) */
extern int         xiarray_aload               (XIArray_PT array, int *xs, int len);

/* O(1) */
extern bool        xiarray_put                 (XIArray_PT array, int i, int data, int *old_data);

/* O(N) */
extern bool        xiarray_put_expand          (XIArray_PT array, int i, int data, int *old_data);
extern bool        xiarray_put_fix_expand      (XIArray_PT array, int i, int data, int *old_data, int new_size);

/* O(N) */
extern bool        xiarray_fill                (XIArray_PT array, int data);
extern bool        xiarray_scope_fill          (XIArray_PT array, int start, int end, int data);

/* O(1) */
extern int         xiarray_get                 (XIArray_PT array, int i);

/* O(NlgN) */
extern int         xiarray_get_kth_element     (XIArray_PT array, int k);

/* O(1) */
extern int         xiarray_front               (XIArray_PT array);
extern int         xiarray_back                (XIArray_PT array);

/* O(N) */
extern int         xiarray_min                 (XIArray_PT array);
extern int         xiarray_max                 (XIArray_PT array);
extern int         xiarray_min_max             (XIArray_PT array, int *min, int *max);

/* O(N) */
extern int         xiarray_map                 (XIArray_PT array, bool (*apply)(int x, void *cl), void *cl);
extern bool        xiarray_map_break_if_true   (XIArray_PT array, bool (*apply)(int x, void *cl), void *cl);
extern bool        xiarray_map_break_if_false  (XIArray_PT array, bool (*apply)(int x, void *cl), void *cl);

/* O(1) */
extern void        xiarray_free                (XIArray_PT *parray);

/* O(N) */
extern void        xiarray_clear               (XIArray_PT array);

/* O(1) */
extern bool        xiarray_remove              (XIArray_PT array, int i, int *old_data);

/* O(1) */
extern int         xiarray_size                (XIArray_PT array);
extern bool        xiarray_is_empty            (XIArray_PT array);

/* O(N) */
extern bool        xiarray_resize              (XIArray_PT array, int new_size);
extern bool        xiarray_remove_resize       (XIArray_PT array, int i, int *old_data);

/* O(1) */
extern bool        xiarray_swap                (XIArray_PT array1, XIArray_PT array2);

/* O(1) */
extern bool        xiarray_exch                (XIArray_PT array, int i, int j);

/* O(N) */
extern bool        xiarray_is_sorted           (XIArray_PT array);

/* O(N^(3/2)) */
extern bool        xiarray_shell_sort          (XIArray_PT array);

/* O(NlgN) */
extern bool        xiarray_merge_sort          (XIArray_PT array);

/* O(NlgN) */
extern bool        xiarray_quick_sort          (XIArray_PT array);

/* O(NlgN) */
extern bool        xiarray_heap_sort           (XIArray_PT array);

/* O(N) */
extern bool        xiarray_counting_sort       (XIArray_PT array);

/* O(N) - O(NlgN) */
extern bool        xiarray_bucket_sort         (XIArray_PT array, int bucket_num);

/* O(N) */
extern bool        xiarray_heapify_min         (XIArray_PT array);
extern bool        xiarray_heapify_max         (XIArray_PT array);

/* O(N) */
extern bool        xiarray_is_heap_sorted      (XIArray_PT array);

/* O(lgN) */
extern int         xiarray_binary_search       (XIArray_PT array, int data);

#ifdef __cplusplus
}
#endif

#endif
