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

#include "../include/xqueue_binomial_min.h"
#include "../include/xheap_min.h"

XMinHeap_PT xminheap_new(int capacity, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    return xminbinque_new(capacity, cmp, cl);
}

XMinHeap_PT xminheap_copy(XMinHeap_PT heap) {
    return xminbinque_copy(heap);
}

XMinHeap_PT xminheap_deep_copy(XMinHeap_PT heap, int elem_size) {
    return xminbinque_deep_copy(heap, elem_size);
}

bool xminheap_push(XMinHeap_PT heap, void *data) {
    return xminbinque_push(heap, data);
}

void* xminheap_pop(XMinHeap_PT heap) {
    return xminbinque_pop(heap);
}

void* xminheap_peek(XMinHeap_PT heap) {
    return xminbinque_peek(heap);
}

bool xminheap_merge(XMinHeap_PT heap1, XMinHeap_PT *pheap2) {
    return xminbinque_merge(heap1, pheap2);
}

int xminheap_map(XMinHeap_PT heap, bool(*apply)(void *x, void *cl), void *cl) {
    return xminbinque_map(heap, apply, cl);
}

bool xminheap_map_break_if_true(XMinHeap_PT heap, bool(*apply)(void *x, void *cl), void *cl) {
    return xminbinque_map_break_if_true(heap, apply, cl);
}

bool xminheap_map_break_if_false(XMinHeap_PT heap, bool(*apply)(void *x, void *cl), void *cl) {
    return xminbinque_map_break_if_false(heap, apply, cl);
}

void xminheap_free(XMinHeap_PT *pheap) {
    xminbinque_free(pheap);
}

void xminheap_free_apply(XMinHeap_PT *pheap, bool(*apply)(void **data, void *cl), void *cl) {
    xminbinque_free_apply(pheap, apply, cl);
}

void xminheap_deep_free(XMinHeap_PT *pheap) {
    xminbinque_deep_free(pheap);
}

void xminheap_clear(XMinHeap_PT heap) {
    xminbinque_clear(heap);
}

void xminheap_clear_apply(XMinHeap_PT heap, bool(*apply)(void **data, void *cl), void *cl) {
    xminbinque_clear_apply(heap, apply, cl);
}

void xminheap_deep_clear(XMinHeap_PT heap) {
    xminbinque_deep_clear(heap);
}

int xminheap_size(XMinHeap_PT heap) {
    return xminbinque_size(heap);
}

bool xminheap_is_empty(XMinHeap_PT heap) {
    return xminbinque_is_empty(heap);
}

bool xminheap_swap(XMinHeap_PT heap1, XMinHeap_PT heap2) {
    return xminbinque_swap(heap1, heap2);
}

/* Note : make sure the capacity of heap is "M" which is the wanted limitation at first */
bool xminheap_keep_max_values(XMinHeap_PT heap, void *data, void **odata) {
    return xminbinque_keep_max_values(heap, data, odata);
}

bool xminheap_set_strategy_discard_new(XMinHeap_PT heap) {
    return xminbinque_set_strategy_discard_new(heap);
}

bool xminheap_set_strategy_discard_top(XMinHeap_PT heap) {
    return xminbinque_set_strategy_discard_top(heap);
}
