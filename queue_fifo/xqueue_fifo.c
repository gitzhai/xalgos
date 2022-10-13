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
#include "../include/xqueue_fifo.h"

XFifo_PT xfifo_new(int capacity) {
    return xdeque_new(capacity);
}

XFifo_PT xfifo_copy(XFifo_PT fifo) {
    return xdeque_copy(fifo);
}

XFifo_PT xfifo_copyn(XFifo_PT fifo, int count) {
    return xdeque_copyn(fifo, count);
}

XFifo_PT xfifo_deep_copy(XFifo_PT fifo, int elem_size) {
    return xdeque_deep_copy(fifo, elem_size);
}

XFifo_PT xfifo_deep_copyn(XFifo_PT fifo, int count, int elem_size) {
    return xdeque_deep_copyn(fifo, count, elem_size);
}

bool xfifo_push(XFifo_PT fifo, void *x) {
    return xdeque_push_back(fifo, x);
}

bool xfifo_push_no_limit(XFifo_PT fifo, void *x) {
    return xdeque_push_back_no_limit(fifo, x);
}

void* xfifo_pop(XFifo_PT fifo) {
    return xdeque_pop_front(fifo);
}

void* xfifo_peek(XFifo_PT fifo) {
    return xdeque_front(fifo);
}

void xfifo_free(XFifo_PT *pfifo) {
    xdeque_free(pfifo);
}

void xfifo_free_apply(XFifo_PT *pfifo, bool (*apply)(void *x, void *cl), void *cl) {
    xdeque_free_apply(pfifo, apply, cl);
}

void xfifo_deep_free(XFifo_PT *pfifo) {
    xdeque_deep_free(pfifo);
}

void xfifo_clear(XFifo_PT fifo) {
    xdeque_clear(fifo);
}

void xfifo_clear_apply(XFifo_PT fifo, bool (*apply)(void *x, void *cl), void *cl) {
    xdeque_clear_apply(fifo, apply, cl);
}

void xfifo_deep_clear(XFifo_PT fifo) {
    xdeque_deep_clear(fifo);
}

int xfifo_size(XFifo_PT fifo) {
    return xdeque_size(fifo);
}

int xfifo_capacity(XFifo_PT fifo) {
    return xdeque_capacity(fifo);
}

bool xfifo_set_capacity_no_limit(XFifo_PT fifo) {
    return xdeque_set_capacity_no_limit(fifo);
}

bool xfifo_is_empty(XFifo_PT fifo) {
    return xdeque_is_empty(fifo);
}

bool xfifo_swap(XFifo_PT fifo1, XFifo_PT fifo2) {
    return xdeque_swap(fifo1, fifo2);
}

bool xfifo_set_strategy_discard_new(XFifo_PT fifo) {
    return xdeque_set_strategy_discard_new(fifo);
}

bool xfifo_set_strategy_discard_top(XFifo_PT fifo) {
    return xdeque_set_strategy_discard_front(fifo);
}

bool xfifo_set_strategy_discard_bottom(XFifo_PT fifo) {
    return xdeque_set_strategy_discard_back(fifo);
}