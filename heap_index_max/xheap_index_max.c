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
*       <<Algorithms in C>> Third Edition. chapter 9.6
*/

#include <stddef.h>

#include "../include/xassert.h"
#include "../heap_index_min/xheap_index_min_x.h"
#include "../include/xheap_index_max.h"

XIndexMaxHeap_PT xindexmaxheap_new(int capacity, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    return xindexminheap_new(capacity, cmp, cl);
}

XIndexMaxHeap_PT xindexmaxheap_copy(XIndexMaxHeap_PT heap) {
    return xindexminheap_copy(heap);
}

XIndexMaxHeap_PT xindexmaxheap_deep_copy(XIndexMaxHeap_PT heap, int elem_size) {
    return xindexminheap_deep_copy(heap, elem_size);
}

bool xindexmaxheap_push(XIndexMaxHeap_PT heap, int i, void *data, void **old_data) {
    return xindexminheap_push_impl(heap, i, data, old_data, false);
}

void* xindexmaxheap_pop(XIndexMaxHeap_PT heap, int *i) {
    return xindexminheap_pop_impl(heap, i, false);
}

void* xindexmaxheap_peek(XIndexMaxHeap_PT heap, int *i) {
    return xindexminheap_peek(heap, i);
}

void* xindexmaxheap_get(XIndexMaxHeap_PT heap, int i) {
    return xindexminheap_get(heap, i);
}

bool xindexmaxheap_remove(XIndexMaxHeap_PT heap, int i, void **old_data) {
    return xindexminheap_remove_impl(heap, i, old_data, false);
}

int xindexmaxheap_map(XIndexMaxHeap_PT heap, bool (*apply)(int i, void *x, void *cl), void *cl) {
    return xindexminheap_map(heap, apply, cl);
}

bool xindexmaxheap_map_break_if_true(XIndexMaxHeap_PT heap, bool (*apply)(int i, void *x, void *cl), void *cl) {
    return xindexminheap_map_break_if_true(heap, apply, cl);
}

bool xindexmaxheap_map_break_if_false(XIndexMaxHeap_PT heap, bool (*apply)(int i, void *x, void *cl), void *cl) {
    return xindexminheap_map_break_if_false(heap, apply, cl);
}

void xindexmaxheap_free(XIndexMaxHeap_PT *pheap) {
    xindexminheap_free(pheap);
}

void xindexmaxheap_deep_free(XIndexMaxHeap_PT *pheap) {
    xindexminheap_deep_free(pheap);
}

void xindexmaxheap_clear(XIndexMaxHeap_PT heap) {
    xindexminheap_clear(heap);
}

void xindexmaxheap_deep_clear(XIndexMaxHeap_PT heap) {
    xindexminheap_deep_clear(heap);
}

int xindexmaxheap_size(XIndexMaxHeap_PT heap) {
    return xindexminheap_size(heap);
}

bool xindexmaxheap_is_empty(XIndexMaxHeap_PT heap) {
    return xindexminheap_is_empty(heap);
}

bool xindexmaxheap_is_maxheap(XIndexMaxHeap_PT heap) {
    xassert(heap);

    if (!heap || (xiseq_size(heap->heap) <= 1)) {
        return true;
    }

    return xindexminheap_is_heap_sorted_impl(heap, 0, 0, xiseq_size(heap->heap) - 1, false);
}

