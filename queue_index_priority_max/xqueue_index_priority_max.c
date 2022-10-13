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
#include "../include/xqueue_index_priority_max.h"

XIndexMaxPQ_PT xindexmaxpq_new(int capacity, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    return xindexmaxheap_new(capacity, cmp, cl);
}

XIndexMaxPQ_PT  xindexmaxpq_copy(XIndexMaxPQ_PT queue) {
    return xindexmaxheap_copy(queue);
}

XIndexMaxPQ_PT  xindexmaxpq_deep_copy(XIndexMaxPQ_PT queue, int elem_size) {
    return xindexmaxheap_deep_copy(queue, elem_size);
}

bool xindexmaxpq_push(XIndexMaxPQ_PT queue, int i, void *data, void **old_data) {
    return xindexmaxheap_push(queue, i, data, old_data);
}

void* xindexmaxpq_pop(XIndexMaxPQ_PT queue, int *i) {
    return xindexmaxheap_pop(queue, i);
}

void* xindexmaxpq_peek(XIndexMaxPQ_PT queue, int *i) {
    return xindexmaxheap_peek(queue, i);
}

void* xindexmaxpq_get(XIndexMaxPQ_PT queue, int i) {
    return xindexmaxheap_get(queue, i);
}

bool xindexmaxpq_remove(XIndexMaxPQ_PT queue, int i, void **old_data) {
    return xindexmaxheap_remove(queue, i, old_data);
}

int xindexmaxpq_map(XIndexMaxPQ_PT queue, bool (*apply)(int i, void *x, void *cl), void *cl) {
    return xindexmaxheap_map(queue, apply, cl);
}

bool xindexmaxpq_map_break_if_true(XIndexMaxPQ_PT queue, bool (*apply)(int i, void *x, void *cl), void *cl) {
    return xindexmaxheap_map_break_if_true(queue, apply, cl);
}

bool xindexmaxpq_map_break_if_false(XIndexMaxPQ_PT queue, bool (*apply)(int i, void *x, void *cl), void *cl) {
    return xindexmaxheap_map_break_if_false(queue, apply, cl);
}

void xindexmaxpq_free(XIndexMaxPQ_PT *pqueue) {
    xindexmaxheap_free(pqueue);
}

void xindexmaxpq_deep_free(XIndexMaxPQ_PT *pqueue) {
    xindexmaxheap_deep_free(pqueue);
}

void xindexmaxpq_clear(XIndexMaxPQ_PT queue) {
    xindexmaxheap_clear(queue);
}

void xindexmaxpq_deep_clear(XIndexMaxPQ_PT queue) {
    xindexmaxheap_deep_clear(queue);
}

int xindexmaxpq_size(XIndexMaxPQ_PT queue) {
    return xindexmaxheap_size(queue);
}

bool xindexmaxpq_is_empty(XIndexMaxPQ_PT queue) {
    return xindexmaxheap_is_empty(queue);
}

bool xindexmaxpq_is_maxpq(XIndexMaxPQ_PT queue) {
    return xindexmaxheap_is_maxheap(queue);
}
