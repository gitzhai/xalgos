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
#include "../heap_min_t/theap_min_x.h"
#include "theap_max.h"

/* Note :
*    TMaxHeap_PT is defined to be the same as TMinHeap_PT,
*    but in order to reduce the number of call stack, we call xdeque_* directly but not tminheap_*
*/

TMaxHeap_PT tmaxheap_new(int capacity, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(0 <= capacity);
    xassert(cmp);

    if ((capacity < 0) || !cmp) {
        return NULL;
    }

    {
        TMaxHeap_PT heap = XMEM_CALLOC(1, sizeof(*heap));
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

bool tmaxheap_push(TMaxHeap_PT heap, void *data) {
    /* Insert the node after the last node */
    if (!xdeque_push_back(heap ? heap->tree : NULL, data)) {
        return false;
    }

    xdeque_heapify_swim_elem(heap->tree, heap->tree->size - 1, 0, heap->tree->size - 1, false, heap->cmp, heap->cl);

    return true;
}

void* tmaxheap_pop(TMaxHeap_PT heap) {
    xassert(heap);

    if (!heap || (heap->tree->size <= 0)) {
        return NULL;
    }

    if (heap->tree->size == 1) {
        return xdeque_pop_back(heap->tree);
    }

    {
        /* 1. Extract the node at the top of the heap */
        void* data = xdeque_get_impl(heap->tree, 0);

        /* 2. move the last node to the top */
        void *save = xdeque_pop_back(heap->tree);
        xdeque_put_impl(heap->tree, 0, save, NULL);

        /* 3. Heapify the tree by pushing the contents of the new top downward */
        xdeque_heapify_sink_elem(heap->tree, 0, 0, heap->tree->size - 1, false, heap->cmp, heap->cl);

        return data;
    }
}

void* tmaxheap_peek(TMaxHeap_PT heap) {
    return xdeque_get(heap ? heap->tree : NULL, 0);
}

int tmaxheap_map(TMaxHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl) {
    return xdeque_map(heap ? heap->tree : NULL, apply, cl);
}

bool tmaxheap_map_break_if_true(TMaxHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl) {
    return xdeque_map_break_if_true(heap ? heap->tree : NULL, apply, cl);
}

bool tmaxheap_map_break_if_false(TMaxHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl) {
    return xdeque_map_break_if_false(heap ? heap->tree : NULL, apply, cl);
}

void tmaxheap_free(TMaxHeap_PT *pheap) {
    xdeque_free((pheap && *pheap) ? &((*pheap)->tree) : NULL);
    XMEM_FREE(*pheap);
}

void tmaxheap_free_apply(TMaxHeap_PT *pheap, bool (*apply)(void *x, void *cl), void *cl) {
    xdeque_free_apply((pheap && *pheap) ? &((*pheap)->tree) : NULL, apply, cl);
    XMEM_FREE(*pheap);
}

void tmaxheap_deep_free(TMaxHeap_PT *pheap) {
    xdeque_deep_free((pheap && *pheap) ? &((*pheap)->tree) : NULL);
    XMEM_FREE(*pheap);
}

void tmaxheap_clear(TMaxHeap_PT heap) {
    xdeque_clear(heap ? heap->tree : NULL);
}

void tmaxheap_clear_apply(TMaxHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl) {
    xdeque_clear_apply(heap ? heap->tree : NULL, apply, cl);
}

void tmaxheap_deep_clear(TMaxHeap_PT heap) {
    xdeque_deep_clear(heap ? heap->tree : NULL);
}

int tmaxheap_size(TMaxHeap_PT heap) {
    return (heap ? xdeque_size(heap->tree) : 0);
}

bool tmaxheap_is_empty(TMaxHeap_PT heap) {
    return (heap ? (xdeque_size(heap->tree) == 0) : true);
}

bool tmaxheap_is_maxheap(TMaxHeap_PT heap) {
    xassert(heap);

    if (!heap || (heap->tree->size <= 1)) {
        return true;
    }

    return xdeque_is_heap_sorted_impl(heap->tree, 0, 0, heap->tree->size - 1, false, heap->cmp, heap->cl);
}

bool tmaxheap_sort(TMaxHeap_PT heap) {
    xassert(heap);

    if (!heap) {
        return false;
    }

    xdeque_heapify_impl(heap->tree, 0, heap->tree->size - 1, true, heap->cmp, heap->cl);
    return xdeque_sort_after_heapify_impl(heap->tree, 0, heap->tree->size - 1, false, heap->cmp, heap->cl);
}
