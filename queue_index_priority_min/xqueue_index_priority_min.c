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
#include "../include/xqueue_index_priority_min.h"

XIndexMinPQ_PT xindexminpq_new(int capacity, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    return xindexminheap_new(capacity, cmp, cl);
}

XIndexMinPQ_PT  xindexminpq_copy(XIndexMinPQ_PT queue) {
    return xindexminheap_copy(queue);
}

XIndexMinPQ_PT  xindexminpq_deep_copy(XIndexMinPQ_PT queue, int elem_size) {
    return xindexminheap_deep_copy(queue, elem_size);
}

bool xindexminpq_push(XIndexMinPQ_PT queue, int i, void *data, void **old_data) {
    return xindexminheap_push(queue, i, data, old_data);
}

void* xindexminpq_pop(XIndexMinPQ_PT queue, int *i) {
    return xindexminheap_pop(queue, i);
}

void* xindexminpq_peek(XIndexMinPQ_PT queue, int *i) {
    return xindexminheap_peek(queue, i);
}

void* xindexminpq_get(XIndexMinPQ_PT queue, int i) {
    return xindexminheap_get(queue, i);
}

bool xindexminpq_remove(XIndexMinPQ_PT queue, int i, void **old_data) {
    return xindexminheap_remove(queue, i, old_data);
}

int xindexminpq_map(XIndexMinPQ_PT queue, bool (*apply)(int i, void *x, void *cl), void *cl) {
    return xindexminheap_map(queue, apply, cl);
}

bool xindexminpq_map_break_if_true(XIndexMinPQ_PT queue, bool (*apply)(int i, void *x, void *cl), void *cl) {
    return xindexminheap_map_break_if_true(queue, apply, cl);
}

bool xindexminpq_map_break_if_false(XIndexMinPQ_PT queue, bool (*apply)(int i, void *x, void *cl), void *cl) {
    return xindexminheap_map_break_if_false(queue, apply, cl);
}

void xindexminpq_free(XIndexMinPQ_PT *pqueue) {
    xindexminheap_free(pqueue);
}

void xindexminpq_deep_free(XIndexMinPQ_PT *pqueue) {
    xindexminheap_deep_free(pqueue);
}

void xindexminpq_clear(XIndexMinPQ_PT queue) {
    xindexminheap_clear(queue);
}

void xindexminpq_deep_clear(XIndexMinPQ_PT queue) {
    xindexminheap_deep_clear(queue);
}

int xindexminpq_size(XIndexMinPQ_PT queue) {
    return xindexminheap_size(queue);
}

bool xindexminpq_is_empty(XIndexMinPQ_PT queue) {
    return xindexminheap_is_empty(queue);
}

bool xindexminpq_is_minpq(XIndexMinPQ_PT queue) {
    return xindexminheap_is_minheap(queue);
}
