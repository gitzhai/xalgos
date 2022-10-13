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

#include "../include/xqueue_binomial_max.h"
#include "../include/xheap_max.h"

XMaxHeap_PT xmaxheap_new(int capacity, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    return xmaxbinque_new(capacity, cmp, cl);
}

XMaxHeap_PT xmaxheap_copy(XMaxHeap_PT queue) {
    return xmaxbinque_copy(queue);
}

XMaxHeap_PT xmaxheap_deep_copy(XMaxHeap_PT queue, int elem_size) {
    return xmaxbinque_deep_copy(queue, elem_size);
}

bool xmaxheap_push(XMaxHeap_PT queue, void *data) {
    return xmaxbinque_push(queue, data);
}

void* xmaxheap_pop(XMaxHeap_PT queue) {
    return xmaxbinque_pop(queue);
}

void* xmaxheap_peek(XMaxHeap_PT queue) {
    return xmaxbinque_peek(queue);
}

int xmaxheap_map(XMaxHeap_PT queue, bool(*apply)(void *x, void *cl), void *cl) {
    return xmaxbinque_map(queue, apply, cl);
}

bool xmaxheap_map_break_if_true(XMaxHeap_PT queue, bool(*apply)(void *x, void *cl), void *cl) {
    return xmaxbinque_map_break_if_true(queue, apply, cl);
}

bool xmaxheap_map_break_if_false(XMaxHeap_PT queue, bool(*apply)(void *x, void *cl), void *cl) {
    return xmaxbinque_map_break_if_false(queue, apply, cl);
}

void xmaxheap_free(XMaxHeap_PT *pqueue) {
    xmaxbinque_free(pqueue);
}

void xmaxheap_free_apply(XMaxHeap_PT *pqueue, bool(*apply)(void **x, void *cl), void *cl) {
    xmaxbinque_free_apply(pqueue, apply, cl);
}

void xmaxheap_deep_free(XMaxHeap_PT *pqueue) {
    xmaxbinque_deep_free(pqueue);
}

void xmaxheap_clear(XMaxHeap_PT queue) {
    xmaxbinque_clear(queue);
}

void xmaxheap_clear_apply(XMaxHeap_PT queue, bool(*apply)(void **x, void *cl), void *cl) {
    xmaxbinque_clear_apply(queue, apply, cl);
}

void xmaxheap_deep_clear(XMaxHeap_PT queue) {
    xmaxbinque_deep_clear(queue);
}

int xmaxheap_size(XMaxHeap_PT queue) {
    return xmaxbinque_size(queue);
}

bool xmaxheap_is_empty(XMaxHeap_PT queue) {
    return xmaxbinque_is_empty(queue);
}

/* Note : make sure the capacity of queue is "M" which is the wanted limitation at first */
bool xmaxheap_keep_min_values(XMaxHeap_PT queue, void *data, void **odata) {
    return xmaxbinque_keep_min_values(queue, data, odata);
}

bool xmaxheap_set_strategy_discard_new(XMaxHeap_PT queue) {
    return xmaxbinque_set_strategy_discard_new(queue);
}

bool xmaxheap_set_strategy_discard_top(XMaxHeap_PT queue) {
    return xmaxbinque_set_strategy_discard_top(queue);
}