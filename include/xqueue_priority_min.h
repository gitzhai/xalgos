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

#ifndef XMINPQ_INCLUDED
#define XMINPQ_INCLUDED

#include "xqueue_binomial_min.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XMinBinQue_PT XMinPQ_PT;

 /* O(1) */
extern XMinPQ_PT  xminpq_new                (int capacity, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(lgN) */
extern bool       xminpq_push               (XMinPQ_PT queue, void *data);
extern void*      xminpq_pop                (XMinPQ_PT queue);

/* O(lgN) */
extern void*      xminpq_peek               (XMinPQ_PT queue);
extern bool       xminpq_merge              (XMinPQ_PT queue1, XMinPQ_PT *pqueue2);

/* O(NlgN) */
extern int        xminpq_map                (XMinPQ_PT queue, bool (*apply)(void *x, void *cl), void *cl);
extern bool       xminpq_map_break_if_true  (XMinPQ_PT queue, bool (*apply)(void *x, void *cl), void *cl);
extern bool       xminpq_map_break_if_false (XMinPQ_PT queue, bool (*apply)(void *x, void *cl), void *cl);

/* O(NlgN) */
extern void       xminpq_free               (XMinPQ_PT *pqueue);
extern void       xminpq_free_apply         (XMinPQ_PT *pqueue, bool (*apply)(void **x, void *cl), void *cl);
extern void       xminpq_deep_free          (XMinPQ_PT *pqueue);

/* O(NlgN) */
extern void       xminpq_clear              (XMinPQ_PT queue);
extern void       xminpq_clear_apply        (XMinPQ_PT queue, bool (*apply)(void **x, void *cl), void *cl);
extern void       xminpq_deep_clear         (XMinPQ_PT queue);

/* O(1) */
extern int        xminpq_size               (XMinPQ_PT queue);
extern bool       xminpq_is_empty           (XMinPQ_PT queue);

/* O(lgN) */
extern bool       xminpq_keep_max_values    (XMinPQ_PT queue, void *data, void **odata);

/* O(1) */
extern bool       xminpq_set_strategy_discard_new (XMinPQ_PT queue);
extern bool       xminpq_set_strategy_discard_top (XMinPQ_PT queue);

#ifdef __cplusplus
}
#endif

#endif
