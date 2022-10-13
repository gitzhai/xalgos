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
#include "../include/xmem.h"
#include "../queue_deque/xqueue_deque_x.h"
#include "theap_min_x.h"

TMinHeap_PT tminheap_new(int capacity, int (*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(0 <= capacity);
    xassert(cmp);

    if ((capacity < 0) || !cmp) {
        return NULL;
    }

    {
        TMinHeap_PT heap = XMEM_CALLOC(1, sizeof(*heap));
        if (!heap) {
            return NULL;
        }

        heap->tree = xdeque_new(capacity);
        if (!heap->tree) {
            XMEM_FREE(heap);
            return NULL;
        }

        heap->cmp = cmp;
        heap->cl = cl;

        return heap;
    }
}

bool tminheap_push_impl(TMinHeap_PT heap, void *data, bool minheap) {
    /* Insert the node after the last node */
    if(!xdeque_push_back(heap ? heap->tree : NULL, data)) {
        return false;
    }

    xdeque_heapify_swim_elem(heap->tree, heap->tree->size - 1, 0, heap->tree->size - 1, minheap, heap->cmp, heap->cl);

    return true;
}

bool tminheap_push(TMinHeap_PT heap, void *data) {
    return tminheap_push_impl(heap, data, true);
}

void* tminheap_pop_impl(TMinHeap_PT heap, bool minheap) {
    xassert(heap);

    if (!heap || (heap->tree->size <= 0)) {
        return NULL;
    }

    if (heap->tree->size == 1) {
        return xdeque_pop_back(heap->tree);
    }

    {
        /* 1. Extract the node at the top of the heap */
        void *data = xdeque_get_impl(heap->tree, 0);

        /* 2. move the last node to the top */
        void *save = xdeque_pop_back(heap->tree);
        xdeque_put_impl(heap->tree, 0, save, NULL);

        /* 3. Heapify the tree by pushing the contents of the new top downward */
        xdeque_heapify_sink_elem(heap->tree, 0, 0, heap->tree->size - 1, minheap, heap->cmp, heap->cl);

        return data;
    }
}

void* tminheap_pop(TMinHeap_PT heap) {
    return tminheap_pop_impl(heap, true);
}

void* tminheap_peek(TMinHeap_PT heap) {
    return xdeque_get(heap ? heap->tree : NULL, 0);
}

int tminheap_map(TMinHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl) {
    return xdeque_map(heap ? heap->tree : NULL, apply, cl);
}

bool tminheap_map_break_if_true(TMinHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl) {
    return xdeque_map_break_if_true(heap ? heap->tree : NULL, apply, cl);
}

bool tminheap_map_break_if_false(TMinHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl) {
    return xdeque_map_break_if_false(heap ? heap->tree : NULL, apply, cl);
}

void tminheap_free(TMinHeap_PT *pheap) {
    xdeque_free((pheap && *pheap) ? &((*pheap)->tree) : NULL);
    XMEM_FREE(*pheap);
}

void tminheap_free_apply(TMinHeap_PT *pheap, bool (*apply)(void *x, void *cl), void *cl) {
    xdeque_free_apply((pheap && *pheap) ? &((*pheap)->tree) : NULL, apply, cl);
    XMEM_FREE(*pheap);
}

void tminheap_deep_free(TMinHeap_PT *pheap) {
    xdeque_deep_free((pheap && *pheap) ? &((*pheap)->tree) : NULL);
    XMEM_FREE(*pheap);
}

void tminheap_clear(TMinHeap_PT heap) {
    xdeque_clear(heap ? heap->tree : NULL);
}

void tminheap_clear_apply(TMinHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl) {
    xdeque_clear_apply(heap ? heap->tree : NULL, apply, cl);
}

void tminheap_deep_clear(TMinHeap_PT heap) {
    xdeque_deep_clear(heap ? heap->tree : NULL);
}

int tminheap_size(TMinHeap_PT heap) {
    return (heap ? xdeque_size(heap->tree) : 0);
}

bool tminheap_is_empty(TMinHeap_PT heap) {
    return (heap ? (xdeque_size(heap->tree) == 0) : true);
}

bool tminheap_is_minheap(TMinHeap_PT heap) {
    xassert(heap);

    if (!heap || (heap->tree->size <= 1)) {
        return true;
    }

    return xdeque_is_heap_sorted_impl(heap->tree, 0, 0, heap->tree->size - 1, true, heap->cmp, heap->cl);
}

bool tminheap_sort(TMinHeap_PT heap) {
    xassert(heap);

    if (!heap) {
        return false;
    }

    xdeque_heapify_impl(heap->tree, 0, heap->tree->size - 1, false, heap->cmp, heap->cl);
    return xdeque_sort_after_heapify_impl(heap->tree, 0, heap->tree->size - 1, true, heap->cmp, heap->cl);
}
