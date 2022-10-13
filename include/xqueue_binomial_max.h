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
*       <<Algorithms in C>> Third Edition : chapter 9.7
*/

#ifndef XMAXBINQUE_INCLUDED
#define XMAXBINQUE_INCLUDED

#include <stdbool.h>
#include "xqueue_binomial_min.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XMinBinQue_PT XMaxBinQue_PT;

/* O(1) */
extern XMaxBinQue_PT xmaxbinque_new                (int capacity, int (*cmp)(void *x, void *y, void *cl), void *cl);    /* capacity = 0 means no limitation */

/* O(N) */
extern XMaxBinQue_PT xmaxbinque_copy               (XMaxBinQue_PT queue);
extern XMaxBinQue_PT xmaxbinque_deep_copy          (XMaxBinQue_PT queue, int elem_size);

/* O(lgN) */
extern int           xmaxbinque_vload              (XMaxBinQue_PT queue, void *x, ...);
extern int           xmaxbinque_aload              (XMaxBinQue_PT queue, XPArray_PT xs);

/* O(lgN) */
extern bool          xmaxbinque_push               (XMaxBinQue_PT queue, void *x);
extern void*         xmaxbinque_pop                (XMaxBinQue_PT queue);

/* O(lgN) */
extern void*         xmaxbinque_peek               (XMaxBinQue_PT queue);
extern bool          xmaxbinque_merge              (XMaxBinQue_PT queue1, XMaxBinQue_PT *pqueue2);

/* O(NlgN) */
extern int           xmaxbinque_map                (XMaxBinQue_PT queue, bool (*apply)(void *x, void *cl), void *cl);
extern bool          xmaxbinque_map_break_if_true  (XMaxBinQue_PT queue, bool (*apply)(void *x, void *cl), void *cl);
extern bool          xmaxbinque_map_break_if_false (XMaxBinQue_PT queue, bool (*apply)(void *x, void *cl), void *cl);

/* O(NlgN) */
extern void          xmaxbinque_free               (XMaxBinQue_PT *pqueue);
extern void          xmaxbinque_free_apply         (XMaxBinQue_PT *pqueue, bool (*apply)(void **x, void *cl), void *cl);
extern void          xmaxbinque_deep_free          (XMaxBinQue_PT *pqueue);

/* O(NlgN) */
extern void          xmaxbinque_clear              (XMaxBinQue_PT queue);
extern void          xmaxbinque_clear_apply        (XMaxBinQue_PT queue, bool (*apply)(void **x, void *cl), void *cl);
extern void          xmaxbinque_deep_clear         (XMaxBinQue_PT queue);

/* O(1) */ 
extern int           xmaxbinque_size               (XMaxBinQue_PT queue);
extern bool          xmaxbinque_is_empty           (XMaxBinQue_PT queue);

/* O(1) */
extern bool          xmaxbinque_swap               (XMaxBinQue_PT queue1, XMaxBinQue_PT queue2);

/* O(lgN) */
extern bool          xmaxbinque_keep_min_values    (XMaxBinQue_PT queue, void *data, void **odata);

/* O(1) */
extern bool          xmaxbinque_set_strategy_discard_new (XMaxBinQue_PT queue);
extern bool          xmaxbinque_set_strategy_discard_top (XMaxBinQue_PT queue);

#ifdef __cplusplus
}
#endif

#endif
