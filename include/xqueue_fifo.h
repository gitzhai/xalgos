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

#ifndef XFIFO_INCLUDED
#define XFIFO_INCLUDED

#include <stdbool.h>
#include "xqueue_deque.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XDeque_PT XFifo_PT;

/* O(1) */
extern XFifo_PT xfifo_new                   (int capacity);

/* O(N) */
extern XFifo_PT xfifo_copy                  (XFifo_PT fifo);
extern XFifo_PT xfifo_copyn                 (XFifo_PT fifo, int count);

/* O(N) */
extern XFifo_PT xfifo_deep_copy             (XFifo_PT fifo, int elem_size);
extern XFifo_PT xfifo_deep_copyn            (XFifo_PT fifo, int count, int elem_size);

/* O(1) */
extern bool     xfifo_push                  (XFifo_PT fifo, void *x);
extern void*    xfifo_pop                   (XFifo_PT fifo);
extern bool     xfifo_push_no_limit         (XFifo_PT fifo, void *x);

/* O(1) */
extern void*    xfifo_peek                  (XFifo_PT fifo);

/* O(1) */
extern void     xfifo_free                  (XFifo_PT *pfifo);
/* O(N) */
extern void     xfifo_free_apply            (XFifo_PT *pfifo, bool (*apply)(void *x, void *cl), void *cl);
extern void     xfifo_deep_free             (XFifo_PT *pfifo);

/* O(1) */
extern void     xfifo_clear                 (XFifo_PT fifo);
/* O(N) */
extern void     xfifo_clear_apply           (XFifo_PT fifo, bool (*apply)(void *x, void *cl), void *cl);
extern void     xfifo_deep_clear            (XFifo_PT fifo);

/* O(1) */
extern int      xfifo_size                  (XFifo_PT fifo);
extern bool     xfifo_is_empty              (XFifo_PT fifo);

/* O(1) */
extern int      xfifo_capacity              (XFifo_PT fifo);
extern bool     xfifo_set_capacity_no_limit (XFifo_PT fifo);

/* O(1) */
extern bool     xfifo_swap                  (XFifo_PT fifo1, XFifo_PT fifo2);

/* O(1) */
extern bool     xfifo_set_strategy_discard_new    (XFifo_PT fifo);
extern bool     xfifo_set_strategy_discard_top    (XFifo_PT fifo);
extern bool     xfifo_set_strategy_discard_bottom (XFifo_PT fifo);

#ifdef __cplusplus
}
#endif

#endif
