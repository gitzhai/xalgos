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

#ifndef XDEQUEX_INCLUDED
#define XDEQUEX_INCLUDED

#include "../include/xqueue_sequence.h"
#include "../include/xqueue_deque.h"

/* 1. when capacity != 0:
 *    layer1_seq will be used directly as the deque container
 *
 * 2. when capacity == 0 :
 *    XDeque_PT has two layers XPSeq_PT : (idea is from SGI STL : stl_deque.h)
 *    a. the size of the first layer XPSeq_PT can be expanded
 *    b. the elements of the first layer XPSeq_PT are still XPSeq_PT
 *       ----------------------------------------------------------
 *       | XPSeq_PT1 | XPSeq_PT2 | XPSeq_PT3 | ...... | XPSeq_PTn |
 *       ----------------------------------------------------------
 *    c. the size of the second layer XPSeq_PT is a fix value
 *
 * 3. capacity can be transfered from !=0 to ==0 by calling xdeque_set_no_capacity
 *    then, the original layer1_seq will be saved in the second layer XPSeq_PT
 *
 * 4. we can call xdeque_new with capacity != 0, then use 
 *    xdeque_push_front_no_limit / xdeque_push_back_no_limit
 *    to save the elements without capacity limit
 */

struct XDeque {
    int      size;             /* number of valid elements */
    int      capacity;         /* number of elements can be filled into the Deque, 0 means no limitation */

    int      discard_strategy; /* how to discard element when no capacity left :
                                *   0 : discard new (default)
                                *   1 : discard front
                                *   2 : discard back
                                */
    XPSeq_PT layer1_seq;
};

/* used to transfer one kind of interface to another */
typedef struct XDeque_Apply_Paras  XDeque_Apply_Paras_T;
typedef struct XDeque_Apply_Paras* XDeque_Apply_Paras_PT;

struct XDeque_Apply_Paras {
    int    (*apply)(void *x, void *y, void *cl);
    void    *cl;
};

/* O(1) */
extern void* xdeque_get_impl                (XDeque_PT deque, int i);
extern bool  xdeque_put_impl                (XDeque_PT deque, int i, void *x, void **old_x);

/* O(1) */
extern void  xdeque_exch_impl               (XDeque_PT deque, int i, int j, void *cl);

/* O(NlgN) */
extern bool  xdeque_heap_sort_impl          (XDeque_PT deque, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl), void *cl);
extern bool  xdeque_sort_after_heapify_impl (XDeque_PT deque, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(NlgN) */
extern bool  xdeque_heapify_impl            (XDeque_PT deque, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(lgN) */
extern void  xdeque_heapify_swim_elem       (XDeque_PT deque, int k, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl2), void *cl2);
extern void  xdeque_heapify_sink_elem       (XDeque_PT deque, int k, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl2), void *cl2);

/* O(N) */
extern bool  xdeque_is_heap_sorted_impl     (XDeque_PT deque, int k, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl), void *cl);

#endif
