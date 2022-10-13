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

#ifndef XLIFO_INCLUDED
#define XLIFO_INCLUDED

#include <stdbool.h>
#include "xqueue_deque.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XDeque_PT XLifo_PT;

/* O(1) */
extern XLifo_PT xlifo_new                   (int capacity);

/* O(N) */
extern XLifo_PT xlifo_copy                  (XLifo_PT lifo);
extern XLifo_PT xlifo_copyn                 (XLifo_PT lifo, int count);

/* O(1) */
extern XLifo_PT xlifo_deep_copy             (XLifo_PT lifo, int elem_size);
extern XLifo_PT xlifo_deep_copyn            (XLifo_PT lifo, int count, int elem_size);

/* O(1) */
extern bool     xlifo_push                  (XLifo_PT lifo, void *x);
extern void*    xlifo_pop                   (XLifo_PT lifo);
extern bool     xlifo_push_no_limit         (XLifo_PT lifo, void *x);

/* O(1) */
extern void*    xlifo_peek                  (XLifo_PT lifo);

/* O(1) */
extern void     xlifo_free                  (XLifo_PT *plifo);
/* O(N) */
extern void     xlifo_free_apply            (XLifo_PT *plifo, bool (*apply)(void *x, void *cl), void *cl);
extern void     xlifo_deep_free             (XLifo_PT *plifo);

/* O(1) */
extern void     xlifo_clear                 (XLifo_PT lifo);
/* O(N) */
extern void     xlifo_clear_apply           (XLifo_PT lifo, bool (*apply)(void *x, void *cl), void *cl);
extern void     xlifo_deep_clear            (XLifo_PT lifo);

/* O(1) */
extern int      xlifo_size                  (XLifo_PT lifo);
extern bool     xlifo_is_empty              (XLifo_PT lifo);

/* O(1) */
extern int      xlifo_capacity              (XLifo_PT lifo);
extern bool     xlifo_set_capacity_no_limit (XLifo_PT lifo);

/* O(1) */
extern bool     xlifo_swap                  (XLifo_PT lifo1, XLifo_PT lifo2);

/* O(1) */
extern bool     xlifo_set_strategy_discard_new    (XLifo_PT lifo);
extern bool     xlifo_set_strategy_discard_top    (XLifo_PT lifo);
extern bool     xlifo_set_strategy_discard_bottom (XLifo_PT lifo);

#ifdef __cplusplus
}
#endif

#endif
