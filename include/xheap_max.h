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

#ifndef XMAXHEAP_INCLUDED
#define XMAXHEAP_INCLUDED

#include <stdbool.h>
#include "xqueue_binomial_max.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XMaxBinQue_PT XMaxHeap_PT;

 /* O(1) */
extern XMaxHeap_PT  xmaxheap_new                (int capacity, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(N) */
extern XMaxHeap_PT  xmaxheap_copy               (XMaxHeap_PT heap);
extern XMaxHeap_PT  xmaxheap_deep_copy          (XMaxHeap_PT heap, int elem_size);

/* O(lgN) */
extern bool         xmaxheap_push               (XMaxHeap_PT heap, void *data);
extern void*        xmaxheap_pop                (XMaxHeap_PT heap);
extern void*        xmaxheap_peek               (XMaxHeap_PT heap);

/* O(NlgN) */
extern int          xmaxheap_map                (XMaxHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl);
extern bool         xmaxheap_map_break_if_true  (XMaxHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl);
extern bool         xmaxheap_map_break_if_false (XMaxHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl);

/* O(NlgN) */
extern void         xmaxheap_free               (XMaxHeap_PT *pheap);
extern void         xmaxheap_free_apply         (XMaxHeap_PT *pheap, bool (*apply)(void **x, void *cl), void *cl);
extern void         xmaxheap_deep_free          (XMaxHeap_PT *pheap);

/* O(NlgN) */
extern void         xmaxheap_clear              (XMaxHeap_PT heap);
extern void         xmaxheap_clear_apply        (XMaxHeap_PT heap, bool (*apply)(void **x, void *cl), void *cl);
extern void         xmaxheap_deep_clear         (XMaxHeap_PT heap);

/* O(1) */
extern int          xmaxheap_size               (XMaxHeap_PT heap);
extern bool         xmaxheap_is_empty           (XMaxHeap_PT heap);

/* O(lgN) */
extern bool         xmaxheap_keep_min_values    (XMaxHeap_PT heap, void *data, void **odata);

/* O(1) */
extern bool         xmaxheap_set_strategy_discard_new (XMaxHeap_PT heap);
extern bool         xmaxheap_set_strategy_discard_top (XMaxHeap_PT heap);


#ifdef __cplusplus
}
#endif

#endif
