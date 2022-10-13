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
*       << Algorithms in C >> Third Edition.chapter 9.6
*/

#include <stddef.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../utils/xutils.h"
#include "../array_int/xarray_int_x.h"
#include "../array_pointer/xarray_pointer_x.h"
#include "../queue_sequence_int/xqueue_sequence_int_x.h"
#include "xheap_index_min_x.h"

/* Note : all invalid value in heap->index should be set to "-1" */

XIndexMinHeap_PT xindexminheap_new(int capacity, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(0 < capacity);
    xassert(cmp);

    if ((capacity <= 0) || !cmp) {
        return NULL;
    }

    {
        XIndexMinHeap_PT heap = XMEM_CALLOC(1, sizeof(*heap));
        if (!heap) {
            return NULL;
        }

        heap->buckets = xparray_new(capacity);
        if (!heap->buckets) {
            XMEM_FREE(heap);
            return NULL;
        }

        heap->index = xiarray_new(capacity);
        if (!heap->index) {
            xparray_free(&heap->buckets);
            XMEM_FREE(heap);
            return NULL;
        }
        xiarray_fill(heap->index, -1);

        heap->heap = xiseq_new(capacity);
        if (!heap->heap) {
            xparray_free(&heap->buckets);
            xiarray_free(&heap->index);
            XMEM_FREE(heap);
            return NULL;
        }

        heap->cmp = cmp;
        heap->cl = cl;

        return heap;
    }
}

static
XIndexMinHeap_PT xindexminheap_copy_impl(XIndexMinHeap_PT heap, int elem_size, bool deep) {
    xassert(heap);

    if (!heap) {
        return NULL;
    }

    {
        XIndexMinHeap_PT nheap = XMEM_CALLOC(1, sizeof(*nheap));
        if (!nheap) {
            return NULL;
        }

        nheap->buckets = deep ? xparray_deep_copy(heap->buckets, elem_size) : xparray_copy(heap->buckets);
        if (!nheap->buckets) {
            XMEM_FREE(nheap);
            return NULL;
        }

        nheap->index = xiarray_copy(heap->index);
        if (!nheap->index) {
            xparray_free(&nheap->buckets);
            XMEM_FREE(nheap);
            return NULL;
        }

        nheap->heap = xiseq_copy(heap->heap);
        if (!nheap->heap) {
            xparray_free(&nheap->buckets);
            xiarray_free(&nheap->index);
            XMEM_FREE(nheap);
            return NULL;
        }

        nheap->cmp = heap->cmp;
        nheap->cl = heap->cl;

        return nheap;
    }
}

XIndexMinHeap_PT xindexminheap_copy(XIndexMinHeap_PT heap) {
    return xindexminheap_copy_impl(heap, 0, false);
}

XIndexMinHeap_PT xindexminheap_deep_copy(XIndexMinHeap_PT heap, int elem_size) {
    return xindexminheap_copy_impl(heap, elem_size, true);
}

#define xindexminheap_heap_parent(npos) ((int)(((npos) - 1) / 2))
#define xindexminheap_heap_left(npos)   (((npos) * 2) + 1)
#define xindexminheap_heap_right(npos)  (((npos) * 2) + 2)

static inline
void xindexminheap_exch_impl(XIndexMinHeap_PT heap, int i, int j) {
    /* exchange the elements in heap */
    xiseq_exch_impl(heap->heap, i, j);

    /* change the index saved in heap */
    xiarray_put_impl(heap->index, xiseq_get_impl(heap->heap, i), i);
    xiarray_put_impl(heap->index, xiseq_get_impl(heap->heap, j), j);
}

static 
void xindexminheap_heapify_swim_elem(XIndexMinHeap_PT heap, int k, int lo, int hi, bool minheap) {
    int parent = lo + xindexminheap_heap_parent(k);

    while (lo <= parent) {
        if (minheap) {
            /* parent is smaller than k, do nothing */
            if (heap->cmp(xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, parent)), xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, lo + k)), heap->cl) <= 0) {
                break;
            }
        }
        else {
            /* parent is bigger than k, do nothing */
            if (heap->cmp(xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, lo + k)), xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, parent)), heap->cl) <= 0) {
                break;
            }
        }

        /* exchange the parent with k */
        xindexminheap_exch_impl(heap, (lo + k), parent);

        /* prepare the next round compare */
        k = parent - lo;
        parent = lo + xindexminheap_heap_parent(k);
    }
}

static 
void xindexminheap_heapify_sink_elem(XIndexMinHeap_PT heap, int k, int lo, int hi, bool minheap) {
    int left_child = lo + xindexminheap_heap_left(k);

    /* sink to left branch by default */
    int child = left_child;

    while (child <= hi) {
        int right_child = left_child + 1;

        if (minheap) {
            /* right child is bigger than left child */
            if ((right_child <= hi) && (heap->cmp(xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, right_child)), xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, left_child)), heap->cl) < 0)) {
                child = right_child;
            }

            /* k is smaller than child, do noting */
            if (heap->cmp(xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, lo + k)), xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, child)), heap->cl) <= 0) {
                break;
            }
        }
        else {
            /* right child is bigger than left child */
            if ((right_child <= hi) && (heap->cmp(xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, left_child)), xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, right_child)), heap->cl) < 0)) {
                child = right_child;
            }

            /* big child is smaller than k, do noting */
            if (heap->cmp(xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, child)), xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, lo + k)), heap->cl) <= 0) {
                break;
            }
        }

        /* change the big child with k */
        xindexminheap_exch_impl(heap, (lo + k), child);

        /* prepare the next round compare */
        k = child - lo;
        left_child = lo + xindexminheap_heap_left(k);
        child = left_child;
    }
}

bool xindexminheap_push_impl(XIndexMinHeap_PT heap, int i, void *data, void **old_data, bool minheap) {
    xassert(heap);
    xassert(0 <= i);
    xassert(i < heap->buckets->size);
    xassert(data);

    if (!heap || !data || (i < 0) || (heap->buckets->size <= i)) {
        return false;
    }

    {
        void *odata = xparray_get_impl(heap->buckets, i);

        /* 1. save the input data */
        xparray_put_impl(heap->buckets, i, data);

        if (odata) {
            if (old_data) {
                *old_data = odata;
            }

            /* 2. update the heap */
            xindexminheap_heapify_swim_elem(heap, xiarray_get(heap->index, i), 0, heap->heap->size - 1, minheap);
            xindexminheap_heapify_sink_elem(heap, xiarray_get(heap->index, i), 0, heap->heap->size - 1, minheap);
        }
        else {
            /* 2. save the index "i" into the tail of heap */
            xiseq_push_back_impl(heap->heap, i);

            /* 3. track the position of "i" in heap */
            xiarray_put_impl(heap->index, i, heap->heap->size - 1);

            /* 4. heapify the heap */
            xindexminheap_heapify_swim_elem(heap, heap->heap->size - 1, 0, heap->heap->size - 1, minheap);
        }
    }

    return true;
}

bool xindexminheap_push(XIndexMinHeap_PT heap, int i, void *data, void **old_data) {
    return xindexminheap_push_impl(heap, i, data, old_data, true);
}

void* xindexminheap_pop_impl(XIndexMinHeap_PT heap, int *i, bool minheap) {
    xassert(heap);

    if (!heap || (heap->heap->size <= 0)) {
        return NULL;
    }

    {
        void *data = NULL;

        /* 1. get the right index and value */
        int j = xiseq_get_impl(heap->heap, 0);
        if (i) {
            *i = j;
        }

        data = xparray_get_impl(heap->buckets, j);

        /* 2. remove the info in xparray and xiarray */
        xparray_put_impl(heap->buckets, j, NULL);
        xiarray_put_impl(heap->index, j, -1);

        /* 3. move the last element in heap to the top */
        {
            int last = xiseq_pop_back_impl(heap->heap);

            /* no more element left */
            if (xiseq_is_empty(heap->heap)) {
                return data;
            }

            xiseq_put_impl(heap->heap, 0, last);
            xiarray_put_impl(heap->index, last, 0);
        }

        /* 4. Heapify the heap */
        xindexminheap_heapify_sink_elem(heap, 0, 0, heap->heap->size - 1, minheap);

        return data;
    }
}

void* xindexminheap_pop(XIndexMinHeap_PT heap, int *i) {
    return xindexminheap_pop_impl(heap, i, true);
}

void* xindexminheap_peek(XIndexMinHeap_PT heap, int *i) {
    xassert(heap);

    if (!heap || (heap->heap->size <= 0)) {
        return NULL;
    }

    if (i) {
        *i = xiseq_get_impl(heap->heap, 0);
    }

    return xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, 0));
}

void* xindexminheap_get(XIndexMinHeap_PT heap, int i) {
    return xparray_get(heap ? heap->buckets : NULL, i);
}

bool xindexminheap_remove_impl(XIndexMinHeap_PT heap, int i, void **old_data, bool minheap) {
    xassert(heap);
    xassert(0 <= i);
    xassert(i < heap->buckets->size);

    if (!heap || (i < 0) || (heap->buckets->size <= i)) {
        return false;
    }

    {
        /* 1. get the heap index */
        int m = xiarray_get(heap->index, i);
        if (m == -1) {
            /* can't find old value */
            return true;
        }

        /* 2. remove the info in xparray and xiarray */
        xparray_save_and_put_impl(heap->buckets, i, NULL, old_data);
        xiarray_put_impl(heap->index, i, -1);

        /* 3. move the last element to index m */
        {
            int last = xiseq_pop_back_impl(heap->heap);

            /* no more element left */
            if (xiseq_is_empty(heap->heap)) {
                return true;
            }

            xiseq_put_impl(heap->heap, m, last);
            xiarray_put_impl(heap->index, last, m);
        }

        /* 4. Heapify the heap again */
        xindexminheap_heapify_swim_elem(heap, m, 0, heap->heap->size - 1, minheap);
        xindexminheap_heapify_sink_elem(heap, m, 0, heap->heap->size - 1, minheap);

        return true;
    }
}

bool xindexminheap_remove(XIndexMinHeap_PT heap, int i, void **old_data) {
    return xindexminheap_remove_impl(heap, i, old_data, true);
}

int xindexminheap_map_impl(XIndexMinHeap_PT heap, bool break_first, bool break_true, bool (*apply)(int i, void *x, void *cl), void *cl) {
    xassert(heap);
    xassert(apply);

    if (!heap || !apply) {
        return 0;
    }

    {
        int count = 0;

        for (int m = 0; m < heap->heap->size; ++m) {
            int i = xiseq_get_impl(heap->heap, m);
            bool ret = apply(i, xparray_get_impl(heap->buckets, i), cl);

            if (break_first) {
                if (ret && break_true) {
                    count++;
                    break;
                }

                if (!ret && !break_true) {
                    count++;
                    break;
                }
            }
            else {
                ret ? count++ : 0;
            }
        }

        return count;
    }
}

int xindexminheap_map(XIndexMinHeap_PT heap, bool (*apply)(int i, void *x, void *cl), void *cl) {
    return xindexminheap_map_impl(heap, false, false, apply, cl);
}

bool xindexminheap_map_break_if_true(XIndexMinHeap_PT heap, bool (*apply)(int i, void *x, void *cl), void *cl) {
    return xindexminheap_map_impl(heap, true, true, apply, cl);
}

bool xindexminheap_map_break_if_false(XIndexMinHeap_PT heap, bool (*apply)(int i, void *x, void *cl), void *cl) {
    return xindexminheap_map_impl(heap, true, false, apply, cl);
}

void xindexminheap_free_impl(XIndexMinHeap_PT *pheap, bool deep, bool (*apply)(int i, void *x, void *cl), void *cl) {
    xassert(pheap);
    xassert(*pheap);

    if (!pheap || !*pheap) {
        return;
    }

    if(deep) {
        int size = xiseq_size((*pheap)->heap);
        for (int m = 0; m < size; ++m) {
            int i = xiseq_get_impl((*pheap)->heap, m);
            xparray_deep_remove((*pheap)->buckets, i);
        }
    }

    xiseq_free(&((*pheap)->heap));
    xiarray_free(&((*pheap)->index));
    xparray_free(&((*pheap)->buckets));

    XMEM_FREE(*pheap);
}

void xindexminheap_free(XIndexMinHeap_PT *pheap) {
    xindexminheap_free_impl(pheap, false, NULL, NULL);
}

void xindexminheap_deep_free(XIndexMinHeap_PT *pheap) {
    xindexminheap_free_impl(pheap, true, NULL, NULL);
}

void xindexminheap_clear_impl(XIndexMinHeap_PT heap, bool deep, bool (*apply)(int i, void *x, void *cl), void *cl) {
    if (!heap) {
        return;
    }

    if(deep) {
        for (int m = 0; m < heap->heap->size; ++m) {
            int i = xiseq_get_impl(heap->heap, m);
            xparray_deep_remove(heap->buckets, i);
        }
    }
    else {
        xparray_clear(heap->buckets);
    }

    xiseq_clear(heap->heap);
    xiarray_fill(heap->index, -1);
}

void xindexminheap_clear(XIndexMinHeap_PT heap) {
    xindexminheap_clear_impl(heap, false, NULL, NULL);
}

void xindexminheap_deep_clear(XIndexMinHeap_PT heap) {
    xindexminheap_clear_impl(heap, true, NULL, NULL);
}

int xindexminheap_size(XIndexMinHeap_PT heap) {
    return (heap ? heap->heap->size : 0);
}

bool xindexminheap_is_empty(XIndexMinHeap_PT heap) {
    return (heap ? (heap->heap->size == 0) : true);
}

bool xindexminheap_is_heap_sorted_impl(XIndexMinHeap_PT heap, int k, int lo, int hi, bool minheap) {
    if (hi <= lo + k) {
        return true;
    }

    {
        int lpos = xindexminheap_heap_left(k);
        int rpos = xindexminheap_heap_right(k);

        /* check left branch */
        if (hi < lo + lpos) {
            return true;
        }
        if (minheap) {
            if (heap->cmp(xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, lo + lpos)), xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, lo + k)), heap->cl) < 0) {
                return false;
            }
        }
        else {
            if (heap->cmp(xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, lo + k)), xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, lo + lpos)), heap->cl) < 0) {
                return false;
            }
        }

        /* check right branch */
        if (hi < lo + rpos) {
            return true;
        }
        if (minheap) {
            if (heap->cmp(xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, lo + rpos)), xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, lo + k)), heap->cl) < 0) {
                return false;
            }
        }
        else {
            if (heap->cmp(xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, lo + k)), xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, lo + rpos)), heap->cl) < 0) {
                return false;
            }
        }

        /* continue to check the children of left and right branches */
        return xindexminheap_is_heap_sorted_impl(heap, lpos, lo, hi, minheap) && xindexminheap_is_heap_sorted_impl(heap, rpos, lo, hi, minheap);
    }
}

bool xindexminheap_is_minheap(XIndexMinHeap_PT heap) {
    xassert(heap);

    if (!heap || (heap->heap->size <= 1)) {
        return true;
    }

    return xindexminheap_is_heap_sorted_impl(heap, 0, 0, xiseq_size(heap->heap) - 1, true);
}
