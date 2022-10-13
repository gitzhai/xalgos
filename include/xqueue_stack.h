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

#ifndef XSTACK_INCLUDED
#define XSTACK_INCLUDED

#include <stdbool.h>
#include "xqueue_deque.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XDeque_PT XStack_PT;

/* O(1) */
extern XStack_PT xstack_new                   (int capacity);

/* O(N) */
extern XStack_PT xstack_copy                  (XStack_PT stack);
extern XStack_PT xstack_copyn                 (XStack_PT stack, int count);

/* O(N) */
extern XStack_PT xstack_deep_copy             (XStack_PT stack, int elem_size);
extern XStack_PT xstack_deep_copyn            (XStack_PT stack, int count, int elem_size);

/* O(1) */
extern bool      xstack_push                  (XStack_PT stack, void *x);
extern void*     xstack_pop                   (XStack_PT stack);
extern bool      xstack_push_no_limit         (XStack_PT stack, void *x);

/* O(1) */
extern void*     xstack_peek                  (XStack_PT stack);

/* O(1) */
extern void      xstack_free                  (XStack_PT *pstack);
/* O(N) */
extern void      xstack_free_apply            (XStack_PT *pstack, bool (*apply)(void *x, void *cl), void *cl);
extern void      xstack_deep_free             (XStack_PT *pstack);

/* O(1) */
extern void      xstack_clear                 (XStack_PT stack);
/* O(N) */
extern void      xstack_clear_apply           (XStack_PT stack, bool (*apply)(void *x, void *cl), void *cl);
extern void      xstack_deep_clear            (XStack_PT stack);

/* O(1) */
extern int       xstack_size                  (XStack_PT stack);
extern bool      xstack_is_empty              (XStack_PT stack);

/* O(1) */
extern int       xstack_capacity              (XStack_PT stack);
extern bool      xstack_set_capacity_no_limit (XStack_PT stack);

/* O(1) */
extern bool      xstack_swap                  (XStack_PT stack1, XStack_PT stack);

/* O(1) */
extern bool      xstack_set_strategy_discard_new    (XStack_PT stack);
extern bool      xstack_set_strategy_discard_top    (XStack_PT stack);
extern bool      xstack_set_strategy_discard_bottom (XStack_PT stack);

#ifdef __cplusplus
}
#endif

#endif
