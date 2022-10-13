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
*       <<Algorithms>> Fourth Edition. chapter 2.4
*/

#ifndef XMINHEAP_INCLUDED
#define XMINHEAP_INCLUDED

#include <stdbool.h>
#include "xqueue_binomial_min.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XMinBinQue_PT XMinHeap_PT;

/* O(1) */
extern XMinHeap_PT   xminheap_new                (int capacity, int (*cmp)(void *x, void *y, void *cl), void *cl);    /* capacity = 0 means no limitation */

/* O(N) */
extern XMinHeap_PT   xminheap_copy               (XMinHeap_PT heap);
extern XMinHeap_PT   xminheap_deep_copy          (XMinHeap_PT heap, int elem_size);

/* O(lgN) */
extern bool          xminheap_push               (XMinHeap_PT heap, void *x);
extern void*         xminheap_pop                (XMinHeap_PT heap);

/* O(lgN) */
extern void*         xminheap_peek               (XMinHeap_PT heap);
extern bool          xminheap_merge              (XMinHeap_PT heap1, XMinHeap_PT *pheap2);

/* O(NlgN) */
extern int           xminheap_map                (XMinHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl);
extern bool          xminheap_map_break_if_true  (XMinHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl);
extern bool          xminheap_map_break_if_false (XMinHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl);

/* O(NlgN) */
extern void          xminheap_free               (XMinHeap_PT *pheap);
extern void          xminheap_free_apply         (XMinHeap_PT *pheap, bool (*apply)(void **data, void *cl), void *cl);
extern void          xminheap_deep_free          (XMinHeap_PT *pheap);

/* O(NlgN) */
extern void          xminheap_clear              (XMinHeap_PT heap);
extern void          xminheap_clear_apply        (XMinHeap_PT heap, bool (*apply)(void **data, void *cl), void *cl);
extern void          xminheap_deep_clear         (XMinHeap_PT heap);

/* O(1) */ 
extern int           xminheap_size               (XMinHeap_PT heap);
extern bool          xminheap_is_empty           (XMinHeap_PT heap);

/* O(1) */
extern bool          xminheap_swap               (XMinHeap_PT heap1, XMinHeap_PT heap2);

/* O(lgN) */
extern bool          xminheap_keep_max_values    (XMinHeap_PT heap, void *data, void **odata);

/* O(1) */
extern bool          xminheap_set_strategy_discard_new (XMinHeap_PT heap);
extern bool          xminheap_set_strategy_discard_top (XMinHeap_PT heap);

#ifdef __cplusplus
}
#endif

#endif
