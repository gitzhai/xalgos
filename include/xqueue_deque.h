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

#ifndef XDEQUE_INCLUDED
#define XDEQUE_INCLUDED

#include <stdbool.h>
#include "xarray_pointer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XDeque* XDeque_PT;

/* O(1) */
extern XDeque_PT xdeque_new                   (int capacity);    /* capacity = 0 means no limitation */

/* O(N) */
extern XDeque_PT xdeque_copy                  (XDeque_PT deque);
extern XDeque_PT xdeque_copyn                 (XDeque_PT deque, int count);

/* O(N) */
extern XDeque_PT xdeque_deep_copy             (XDeque_PT deque, int elem_size);
extern XDeque_PT xdeque_deep_copyn            (XDeque_PT deque, int count, int elem_size);

/* O(N) */
extern int       xdeque_vload                 (XDeque_PT deque, void *x, ...);
extern int       xdeque_aload                 (XDeque_PT deque, XPArray_PT xs);

/* O(1) */
extern bool      xdeque_push_front            (XDeque_PT deque, void *x);
extern void*     xdeque_pop_front             (XDeque_PT deque);
extern bool      xdeque_push_front_no_limit   (XDeque_PT deque, void *x);

/* O(1) */
extern bool      xdeque_push_back             (XDeque_PT deque, void *x);
extern void*     xdeque_pop_back              (XDeque_PT deque);
extern bool      xdeque_push_back_no_limit    (XDeque_PT deque, void *x);

/* O(1) */
extern void*     xdeque_front                 (XDeque_PT deque);
extern void*     xdeque_back                  (XDeque_PT deque);

/* O(1) */
extern bool      xdeque_put                   (XDeque_PT deque, int i, void *x, void **old_x);
extern void*     xdeque_get                   (XDeque_PT deque, int i);

/* O(N) */
extern int       xdeque_map                   (XDeque_PT deque, bool (*apply)(void *x, void *cl), void *cl);
extern bool      xdeque_map_break_if_true     (XDeque_PT deque, bool (*apply)(void *x, void *cl), void *cl);
extern bool      xdeque_map_break_if_false    (XDeque_PT deque, bool (*apply)(void *x, void *cl), void *cl);

/* O(1) */
extern void      xdeque_free                  (XDeque_PT *pdeque);
/* O(N) */
extern void      xdeque_free_apply            (XDeque_PT *pdeque, bool (*apply)(void *x, void *cl), void *cl);
extern void      xdeque_deep_free             (XDeque_PT *pdeque);

/* O(1) */
extern void      xdeque_clear                 (XDeque_PT deque);
/* O(N) */
extern void      xdeque_clear_apply           (XDeque_PT deque, bool (*apply)(void *x, void *cl), void *cl);
extern void      xdeque_deep_clear            (XDeque_PT deque);

/* O(1) */
extern int       xdeque_size                  (XDeque_PT deque);
extern bool      xdeque_is_empty              (XDeque_PT deque);
extern int       xdeque_capacity              (XDeque_PT deque);
extern bool      xdeque_set_capacity_no_limit (XDeque_PT deque);

/* O(1) */
extern bool      xdeque_swap                  (XDeque_PT deque1, XDeque_PT deque2);

/* O(N) */
extern bool      xdeque_is_sorted             (XDeque_PT deque, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(NlgN) */
extern bool      xdeque_quick_sort            (XDeque_PT deque, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(NlgN) */
extern bool      xdeque_heap_sort             (XDeque_PT deque, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(N) */
extern bool      xdeque_heapify_min           (XDeque_PT deque, int (*cmp)(void *x, void *y, void *cl), void *cl);
extern bool      xdeque_heapify_max           (XDeque_PT deque, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(NlgN) */
extern bool      xdeque_vmultiway_sort        (XDeque_PT deque, int (*cmp)(void *x, void *y, void *cl), void *cl, XDeque_PT deque1, ...);
extern bool      xdeque_amultiway_sort        (XDeque_PT deque, int (*cmp)(void *x, void *y, void *cl), void *cl, XPArray_PT deques);

/* O(1) */
extern bool      xdeque_set_strategy_discard_new   (XDeque_PT deque);
extern bool      xdeque_set_strategy_discard_front (XDeque_PT deque);
extern bool      xdeque_set_strategy_discard_back  (XDeque_PT deque);

#ifdef __cplusplus
}
#endif

#endif
