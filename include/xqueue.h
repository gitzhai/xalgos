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

#ifndef XQUEUE_INCLUDED
#define XQUEUE_INCLUDED

#include <stdbool.h>
#include "xqueue_deque.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XDeque_PT XQueue_PT;

/* O(1) */
extern XQueue_PT xqueue_new                   (int capacity);

/* O(N) */
extern XQueue_PT xqueue_copy                  (XQueue_PT queue);
extern XQueue_PT xqueue_copyn                 (XQueue_PT queue, int count);

/* O(N) */
extern XQueue_PT xqueue_deep_copy             (XQueue_PT queue, int elem_size);
extern XQueue_PT xqueue_deep_copyn            (XQueue_PT queue, int count, int elem_size);

/* O(1) */
extern bool      xqueue_push                  (XQueue_PT queue, void *x);
extern void*     xqueue_pop                   (XQueue_PT queue);
extern bool      xqueue_push_no_limit         (XQueue_PT queue, void *x);

/* O(1) */
extern void*     xqueue_peek                  (XQueue_PT queue);

/* O(1) */
extern void      xqueue_free                  (XQueue_PT *pqueue);
/* O(N) */
extern void      xqueue_free_apply            (XQueue_PT *pqueue, bool (*apply)(void *x, void *cl), void *cl);
extern void      xqueue_deep_free             (XQueue_PT *pqueue);

/* O(1) */
extern void      xqueue_clear                 (XQueue_PT queue);
/* O(N) */
extern void      xqueue_clear_apply           (XQueue_PT queue, bool (*apply)(void *x, void *cl), void *cl);
extern void      xqueue_deep_clear            (XQueue_PT queue);

/* O(1) */
extern int       xqueue_size                  (XQueue_PT queue);
extern bool      xqueue_is_empty              (XQueue_PT queue);

/* O(1) */
extern int       xqueue_capacity              (XQueue_PT queue);
extern bool      xqueue_set_capacity_no_limit (XQueue_PT queue);

/* O(1) */
extern bool      xqueue_swap                  (XQueue_PT queue1, XQueue_PT queue2);

/* O(1) */
extern bool      xqueue_set_strategy_discard_new   (XQueue_PT queue);
extern bool      xqueue_set_strategy_discard_front (XQueue_PT queue);
extern bool      xqueue_set_strategy_discard_back  (XQueue_PT queue);

#ifdef __cplusplus
}
#endif

#endif
