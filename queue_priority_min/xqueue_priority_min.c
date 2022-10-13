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

#include <stddef.h>

#include "../include/xassert.h"
#include "../include/xqueue_binomial_min.h"
#include "../include/xqueue_priority_min.h"

XMinPQ_PT xminpq_new(int capacity, int (*cmp)(void *x, void *y, void *cl), void *cl) {
    return xminbinque_new(capacity, cmp, cl);
}

XMinPQ_PT xminpq_copy(XMinPQ_PT queue) {
    return xminbinque_copy(queue);
}

XMinPQ_PT xminpq_deep_copy(XMinPQ_PT queue, int elem_size) {
    return xminbinque_deep_copy(queue, elem_size);
}

bool xminpq_push(XMinPQ_PT queue, void *data) {
    return xminbinque_push(queue, data);
}

void* xminpq_pop(XMinPQ_PT queue) {
    return xminbinque_pop(queue);
}

void* xminpq_peek(XMinPQ_PT queue) {
    return xminbinque_peek(queue);
}

bool xminpq_merge(XMinPQ_PT queue1, XMinPQ_PT *pqueue2) {
    return xminbinque_merge(queue1, pqueue2);
}

int xminpq_map(XMinPQ_PT queue, bool (*apply)(void *x, void *cl), void *cl) {
    return xminbinque_map(queue, apply, cl);
}

bool xminpq_map_break_if_true(XMinPQ_PT queue, bool (*apply)(void *x, void *cl), void *cl) {
    return xminbinque_map_break_if_true(queue, apply, cl);
}

bool xminpq_map_break_if_false(XMinPQ_PT queue, bool (*apply)(void *x, void *cl), void *cl) {
    return xminbinque_map_break_if_false(queue, apply, cl);
}

void xminpq_free(XMinPQ_PT *pqueue) {
    xminbinque_free(pqueue);
}

void xminpq_deep_free(XMinPQ_PT *pqueue) {
    xminbinque_deep_free(pqueue);
}

void xminpq_free_apply(XMinPQ_PT *pqueue, bool (*apply)(void **x, void *cl), void *cl) {
    xminbinque_free_apply(pqueue, apply, cl);
}

void xminpq_clear(XMinPQ_PT queue) {
    xminbinque_clear(queue);
}

void xminpq_clear_apply(XMinPQ_PT queue, bool(*apply)(void **x, void *cl), void *cl) {
    xminbinque_clear_apply(queue, apply, cl);
}

void xminpq_deep_clear(XMinPQ_PT queue) {
    xminbinque_deep_clear(queue);
}

int xminpq_size(XMinPQ_PT queue) {
    return xminbinque_size(queue);
}

bool xminpq_is_empty(XMinPQ_PT queue) {
    return xminbinque_is_empty(queue);
}

/* Note : make sure the capacity of queue is "M" which is the wanted limitation at first */
bool xminpq_keep_max_values(XMinPQ_PT queue, void *data, void **odata) {
    return xminbinque_keep_max_values(queue, data, odata);
}

bool xminpq_set_strategy_discard_new(XMinPQ_PT queue) {
    return xminbinque_set_strategy_discard_new(queue);
}

bool xminpq_set_strategy_discard_top(XMinPQ_PT queue) {
    return xminbinque_set_strategy_discard_top(queue);
}
