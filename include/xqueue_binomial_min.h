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

#ifndef XMINBINQUE_INCLUDED
#define XMINBINQUE_INCLUDED

#include <stdbool.h>
#include "xarray_pointer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XMinBinQue* XMinBinQue_PT;

/* O(1) */
extern XMinBinQue_PT xminbinque_new                (int capacity, int (*cmp)(void *x, void *y, void *cl), void *cl);    /* capacity = 0 means no limitation */

/* O(N) */
extern XMinBinQue_PT xminbinque_copy               (XMinBinQue_PT queue);
extern XMinBinQue_PT xminbinque_deep_copy          (XMinBinQue_PT queue, int elem_size);

/* O(lgN) */
extern int           xminbinque_vload              (XMinBinQue_PT queue, void *x, ...);
extern int           xminbinque_aload              (XMinBinQue_PT queue, XPArray_PT xs);

/* O(lgN) */
extern bool          xminbinque_push               (XMinBinQue_PT queue, void *x);
extern void*         xminbinque_pop                (XMinBinQue_PT queue);

/* O(lgN) */
extern void*         xminbinque_peek               (XMinBinQue_PT queue);
extern bool          xminbinque_merge              (XMinBinQue_PT queue1, XMinBinQue_PT *pqueue2);

/* O(NlgN) */
extern int           xminbinque_map                (XMinBinQue_PT queue, bool (*apply)(void *x, void *cl), void *cl);
extern bool          xminbinque_map_break_if_true  (XMinBinQue_PT queue, bool (*apply)(void *x, void *cl), void *cl);
extern bool          xminbinque_map_break_if_false (XMinBinQue_PT queue, bool (*apply)(void *x, void *cl), void *cl);

/* O(NlgN) */
extern void          xminbinque_free               (XMinBinQue_PT *pqueue);
extern void          xminbinque_free_apply         (XMinBinQue_PT *pqueue, bool (*apply)(void **data, void *cl), void *cl);
extern void          xminbinque_deep_free          (XMinBinQue_PT *pqueue);

/* O(NlgN) */
extern void          xminbinque_clear              (XMinBinQue_PT queue);
extern void          xminbinque_clear_apply        (XMinBinQue_PT queue, bool (*apply)(void **data, void *cl), void *cl);
extern void          xminbinque_deep_clear         (XMinBinQue_PT queue);

/* O(1) */ 
extern int           xminbinque_size               (XMinBinQue_PT queue);
extern bool          xminbinque_is_empty           (XMinBinQue_PT queue);

/* O(1) */
extern bool          xminbinque_swap               (XMinBinQue_PT queue1, XMinBinQue_PT queue2);

/* O(lgN) */
extern bool          xminbinque_keep_max_values    (XMinBinQue_PT queue, void *data, void **odata);

/* O(1) */
extern bool          xminbinque_set_strategy_discard_new (XMinBinQue_PT queue);
extern bool          xminbinque_set_strategy_discard_top (XMinBinQue_PT queue);

#ifdef __cplusplus
}
#endif

#endif
