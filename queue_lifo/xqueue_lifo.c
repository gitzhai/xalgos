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
#include "../include/xqueue_lifo.h"

XLifo_PT xlifo_new(int capacity) {
    return xdeque_new(capacity);
}

XLifo_PT xlifo_copy(XLifo_PT lifo) {
    return xdeque_copy(lifo);
}

XLifo_PT xlifo_copyn(XLifo_PT lifo, int count) {
    return xdeque_copyn(lifo, count);
}

XLifo_PT xlifo_deep_copy(XLifo_PT lifo, int elem_size) {
    return xdeque_deep_copy(lifo, elem_size);
}

XLifo_PT xlifo_deep_copyn(XLifo_PT lifo, int count, int elem_size) {
    return xdeque_deep_copyn(lifo, count, elem_size);
}

bool xlifo_push(XLifo_PT lifo, void *x) {
    return xdeque_push_back(lifo, x);
}

bool xlifo_push_no_limit(XLifo_PT lifo, void *x) {
    return xdeque_push_back_no_limit(lifo, x);
}

void* xlifo_pop(XLifo_PT lifo) {
    return xdeque_pop_back(lifo);
}

void* xlifo_peek(XLifo_PT lifo) {
    return xdeque_back(lifo);
}

void xlifo_free(XLifo_PT *plifo) {
    xdeque_free(plifo);
}

void xlifo_free_apply(XLifo_PT *plifo, bool (*apply)(void *x, void *cl), void *cl) {
    xdeque_free_apply(plifo, apply, cl);
}

void xlifo_deep_free(XLifo_PT *plifo) {
    xdeque_deep_free(plifo);
}

void xlifo_clear(XLifo_PT lifo) {
    xdeque_clear(lifo);
}

void xlifo_clear_apply(XLifo_PT lifo, bool (*apply)(void *x, void *cl), void *cl) {
    xdeque_clear_apply(lifo, apply, cl);
}

void xlifo_deep_clear(XLifo_PT lifo) {
    xdeque_deep_clear(lifo);
}

int xlifo_size(XLifo_PT lifo) {
    return xdeque_size(lifo);
}

bool xlifo_is_empty(XLifo_PT lifo) {
    return xdeque_is_empty(lifo);
}

int xlifo_capacity(XLifo_PT lifo) {
    return xdeque_capacity(lifo);
}

bool xlifo_set_capacity_no_limit(XLifo_PT lifo) {
    return xdeque_set_capacity_no_limit(lifo);
}

bool xlifo_swap(XLifo_PT lifo1, XLifo_PT lifo2) {
    return xdeque_swap(lifo1, lifo2);
}

bool xlifo_set_strategy_discard_new(XLifo_PT lifo) {
    return xdeque_set_strategy_discard_new(lifo);
}

bool xlifo_set_strategy_discard_top(XLifo_PT lifo) {
    return xdeque_set_strategy_discard_back(lifo);
}

bool xlifo_set_strategy_discard_bottom(XLifo_PT lifo) {
    return xdeque_set_strategy_discard_front(lifo);
}
