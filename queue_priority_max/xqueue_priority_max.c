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
#include "../include/xqueue_binomial_max.h"
#include "../include/xqueue_priority_max.h"

XMaxPQ_PT xmaxpq_new(int capacity, int (*cmp)(void *x, void *y, void *cl), void *cl) {
    return xmaxbinque_new(capacity, cmp, cl);
}

XMaxPQ_PT xmaxpq_copy(XMaxPQ_PT queue) {
    return xmaxbinque_copy(queue);
}

XMaxPQ_PT xmaxpq_deep_copy(XMaxPQ_PT queue, int elem_size) {
    return xmaxbinque_deep_copy(queue, elem_size);
}

bool xmaxpq_push(XMaxPQ_PT queue, void *data) {
    return xmaxbinque_push(queue, data);
}

void* xmaxpq_pop(XMaxPQ_PT queue) {
    return xmaxbinque_pop(queue);
}

void* xmaxpq_peek(XMaxPQ_PT queue) {
    return xmaxbinque_peek(queue);
}

bool xmaxpq_merge(XMaxPQ_PT queue1, XMaxPQ_PT *pqueue2) {
    return xmaxbinque_merge(queue1, pqueue2);
}

int xmaxpq_map(XMaxPQ_PT queue, bool (*apply)(void *x, void *cl), void *cl) {
    return xmaxbinque_map(queue, apply, cl);
}

bool xmaxpq_map_break_if_true(XMaxPQ_PT queue, bool (*apply)(void *x, void *cl), void *cl) {
    return xmaxbinque_map_break_if_true(queue, apply, cl);
}

bool xmaxpq_map_break_if_false(XMaxPQ_PT queue, bool (*apply)(void *x, void *cl), void *cl) {
    return xmaxbinque_map_break_if_false(queue, apply, cl);
}

void xmaxpq_free(XMaxPQ_PT *pqueue) {
    xmaxbinque_free(pqueue);
}

void xmaxpq_free_apply(XMaxPQ_PT *pqueue, bool (*apply)(void **x, void *cl), void *cl) {
    xmaxbinque_free_apply(pqueue, apply, cl);
}

void xmaxpq_deep_free(XMaxPQ_PT *pqueue) {
    xmaxbinque_deep_free(pqueue);
}

void xmaxpq_clear(XMaxPQ_PT queue) {
    xmaxbinque_clear(queue);
}

void xmaxpq_clear_apply(XMaxPQ_PT queue, bool (*apply)(void **x, void *cl), void *cl) {
    xmaxbinque_clear_apply(queue, apply, cl);
}

void xmaxpq_deep_clear(XMaxPQ_PT queue) {
    xmaxbinque_deep_clear(queue);
}

int xmaxpq_size(XMaxPQ_PT queue) {
    return xmaxbinque_size(queue);
}

bool xmaxpq_is_empty(XMaxPQ_PT queue) {
    return xmaxbinque_is_empty(queue);
}

/* Note : make sure the capacity of queue is "M" which is the wanted limitation at first */
bool xmaxpq_keep_min_values(XMaxPQ_PT queue, void *data, void **odata) {
    return xmaxbinque_keep_min_values(queue, data, odata);
}

bool xmaxpq_set_strategy_discard_new(XMaxPQ_PT queue) {
    return xmaxbinque_set_strategy_discard_new(queue);
}

bool xmaxpq_set_strategy_discard_top(XMaxPQ_PT queue) {
    return xmaxbinque_set_strategy_discard_top(queue);
}
