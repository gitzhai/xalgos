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

#include "../include/xqueue_deque.h"
#include "../include/xqueue.h"

XQueue_PT xqueue_new(int capacity) {
    return xdeque_new(capacity);
}

XQueue_PT xqueue_copy(XQueue_PT queue) {
    return xdeque_copy(queue);
}

XQueue_PT xqueue_copyn(XQueue_PT queue, int count) {
    return xdeque_copyn(queue, count);
}

XQueue_PT xqueue_deep_copy(XQueue_PT queue, int elem_size) {
    return xdeque_deep_copy(queue, elem_size);
}

XQueue_PT xqueue_deep_copyn(XQueue_PT queue, int count, int elem_size) {
    return xdeque_deep_copyn(queue, count, elem_size);
}

bool xqueue_push(XQueue_PT queue, void *x) {
    return xdeque_push_back(queue, x);
}

bool xqueue_push_no_limit(XQueue_PT queue, void *x) {
    return xdeque_push_back_no_limit(queue, x);
}

void* xqueue_pop(XQueue_PT queue) {
    return xdeque_pop_front(queue);
}

void* xqueue_peek(XQueue_PT queue) {
    return xdeque_front(queue);
}

void xqueue_free(XQueue_PT *pqueue) {
    xdeque_free(pqueue);
}

void xqueue_free_apply(XQueue_PT *pqueue, bool (*apply)(void *x, void *cl), void *cl) {
    xdeque_free_apply(pqueue, apply, cl);
}

void xqueue_deep_free(XQueue_PT *pqueue) {
    xdeque_deep_free(pqueue);
}

void xqueue_clear(XQueue_PT queue) {
    xdeque_clear(queue);
}

void xqueue_clear_apply(XQueue_PT queue, bool (*apply)(void *x, void *cl), void *cl) {
    xdeque_clear_apply(queue, apply, cl);
}

void xqueue_deep_clear(XQueue_PT queue) {
    xdeque_deep_clear(queue);
}

int xqueue_size(XQueue_PT queue) {
    return xdeque_size(queue);
}

bool xqueue_is_empty(XQueue_PT queue) {
    return xdeque_is_empty(queue);
}

int xqueue_capacity(XQueue_PT queue) {
    return xdeque_capacity(queue);
}

bool xqueue_set_capacity_no_limit(XQueue_PT queue) {
    return xdeque_set_capacity_no_limit(queue);
}

bool xqueue_swap(XQueue_PT queue1, XQueue_PT queue2) {
    return xdeque_swap(queue1, queue2);
}

bool xqueue_set_strategy_discard_new(XQueue_PT queue) {
    return xdeque_set_strategy_discard_new(queue);
}

bool xqueue_set_strategy_discard_front(XQueue_PT queue) {
    return xdeque_set_strategy_discard_front(queue);
}

bool xqueue_set_strategy_discard_back(XQueue_PT queue) {
    return xdeque_set_strategy_discard_back(queue);
}
