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

#ifndef XMAXPQ_INCLUDED
#define XMAXPQ_INCLUDED

#include "xqueue_binomial_max.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XMaxBinQue_PT XMaxPQ_PT;

 /* O(1) */
extern XMaxPQ_PT  xmaxpq_new                (int capacity, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(N) */
extern XMaxPQ_PT  xmaxpq_copy               (XMaxPQ_PT queue);
extern XMaxPQ_PT  xmaxpq_deep_copy          (XMaxPQ_PT queue, int elem_size);

/* O(lgN) */
extern bool       xmaxpq_push               (XMaxPQ_PT queue, void *data);
extern void*      xmaxpq_pop                (XMaxPQ_PT queue);

/* O(lgN) */
extern void*      xmaxpq_peek               (XMaxPQ_PT queue);
extern bool       xmaxpq_merge              (XMaxPQ_PT queue1, XMaxPQ_PT *pqueue2);

/* O(NlgN) */
extern int        xmaxpq_map                (XMaxPQ_PT queue, bool (*apply)(void *x, void *cl), void *cl);
extern bool       xmaxpq_map_break_if_true  (XMaxPQ_PT queue, bool (*apply)(void *x, void *cl), void *cl);
extern bool       xmaxpq_map_break_if_false (XMaxPQ_PT queue, bool (*apply)(void *x, void *cl), void *cl);

/* O(NlgN) */
extern void       xmaxpq_free               (XMaxPQ_PT *pqueue);
extern void       xmaxpq_free_apply         (XMaxPQ_PT *pqueue, bool (*apply)(void **x, void *cl), void *cl);
extern void       xmaxpq_deep_free          (XMaxPQ_PT *pqueue);

/* O(NlgN) */
extern void       xmaxpq_clear              (XMaxPQ_PT queue);
extern void       xmaxpq_clear_apply        (XMaxPQ_PT queue, bool (*apply)(void **x, void *cl), void *cl);
extern void       xmaxpq_deep_clear         (XMaxPQ_PT queue);

/* O(1) */
extern int        xmaxpq_size               (XMaxPQ_PT queue);
extern bool       xmaxpq_is_empty           (XMaxPQ_PT queue);

/* O(lgN) */
extern bool       xmaxpq_keep_min_values    (XMaxPQ_PT queue, void *data, void **odata);

/* O(1) */
extern bool       xmaxpq_set_strategy_discard_new (XMaxPQ_PT queue);
extern bool       xmaxpq_set_strategy_discard_top (XMaxPQ_PT queue);

#ifdef __cplusplus
}
#endif

#endif
