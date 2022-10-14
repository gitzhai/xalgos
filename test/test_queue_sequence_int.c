
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/xutils.h"
#include "../array_int/xarray_int_x.h"
#include "../queue_sequence_int/xqueue_sequence_int_x.h"
#include "../include/xalgos.h"

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static
XISeq_PT xiseq_random_int(int size) {
    XISeq_PT seq = xiseq_new(size);
    if (!seq) {
        return NULL;
    }

    for (int i = 0; i < size; i++) {
        xiseq_push_back(seq, rand() % 100);
    }

    return seq;
}

void test_xiseq() {
    /* xiseq_new */
    {
        XISeq_PT seq = xiseq_new(10);

        xassert(seq->size == 0);
        xassert(seq->head == 0);
        xassert(xiarray_size(seq->array) == 10);

        xiseq_free(&seq);
    }

    /* xiseq_copy */
    /* xiseq_copyn */
    {
        /* sequence is empty, 0 < count */
        {
            XISeq_PT seq = xiseq_new(20);
            XISeq_PT nseq = xiseq_copyn(seq, 10);

            xassert(nseq->size == 0);
            xassert(nseq->head == 0);

            xiseq_free(&seq);
            xiseq_free(&nseq);
        }

        /* sequence is not empty, count == 0, head == 0 */
        {
            XISeq_PT seq = xiseq_new(10);
            xiseq_push_back(seq, 1);
            xiseq_push_back(seq, 2);
            xiseq_push_back(seq, 3);

            XISeq_PT nseq = xiseq_copyn(seq, 0);

            xassert(nseq->size == 0);
            xassert(nseq->head == 0);
            
            xiseq_free(&seq);
            xiseq_free(&nseq);
        }

        /* sequence is not empty, count == 0, 0 < head */
        {
            XISeq_PT seq = xiseq_new(10);
            xiseq_push_back(seq, 1);
            xiseq_push_back(seq, 2);
            xiseq_push_back(seq, 3);
            xiseq_pop_front(seq);

            XISeq_PT nseq = xiseq_copyn(seq, 0);

            xassert(nseq->size == 0);
            xassert(nseq->head == 0);
            
            xiseq_free(&seq);
            xiseq_free(&nseq);
        }

        /* sequence is not empty, 0 < count <= size, head == 0 */
        {
            XISeq_PT seq = xiseq_new(10);
            xiseq_push_back(seq, 1);
            xiseq_push_back(seq, 2);
            xiseq_push_back(seq, 3);

            XISeq_PT nseq = xiseq_copyn(seq, 2);

            xassert(nseq->size == 2);
            xassert(nseq->head == 0);

            xassert(xiseq_get_impl(nseq, 0) == 1);
            xassert(xiseq_get_impl(nseq, 1) == 2);

            xiseq_free(&seq);
            xiseq_free(&nseq);
        }

        /* sequence is not empty, 0 < count <= size, 0 < head, count <= (size - head + 1) */
        {
            XISeq_PT seq = xiseq_new(10);
            xiseq_push_back(seq, 4);
            xiseq_push_back(seq, 1);
            xiseq_push_back(seq, 2);
            xiseq_push_back(seq, 3);
            xiseq_pop_front(seq);

            XISeq_PT nseq = xiseq_copyn(seq, 2);

            xassert(nseq->size == 2);
            xassert(nseq->head == 0);

            xassert(xiseq_get_impl(nseq, 0) == 1);
            xassert(xiseq_get_impl(nseq, 1) == 2);

            xiseq_free(&seq);
            xiseq_free(&nseq);
        }

        /* sequence is not empty, 0 < count <= size, 0 < head, (size - head + 1) < count */
        {
            XISeq_PT seq = xiseq_new(5);
            xiseq_push_back(seq, 4);
            xiseq_push_back(seq, 4);
            xiseq_push_back(seq, 1);
            xiseq_push_back(seq, 2);
            xiseq_push_back(seq, 3);
            xiseq_pop_front(seq);
            xiseq_pop_front(seq);
            xiseq_push_back(seq, 4);

            XISeq_PT nseq = xiseq_copyn(seq, 4);

            xassert(nseq->size == 4);
            xassert(nseq->head == 0);

            xassert(xiseq_get_impl(nseq, 0) == 1);
            xassert(xiseq_get_impl(nseq, 1) == 2);
            xassert(xiseq_get_impl(nseq, 2) == 3);
            xassert(xiseq_get_impl(nseq, 3) == 4);

            xiseq_free(&seq);
            xiseq_free(&nseq);
        }
    }

    /* xiseq_push_front */
    {
        XISeq_PT seq = xiseq_new(7);

        xiseq_push_front(seq, 5);
        xassert(seq->size == 1);
        xassert(seq->head == 6);

        xiseq_push_front(seq, 4);
        xassert(seq->size == 2);
        xassert(seq->head == 5);

        xiseq_push_front(seq, 3);
        xassert(seq->size == 3);
        xassert(seq->head == 4);

        xiseq_push_back(seq, 6);
        xassert(seq->size == 4);
        xassert(seq->head == 4);

        xiseq_push_back(seq, 7);
        xassert(seq->size == 5);
        xassert(seq->head == 4);

        xiseq_push_front(seq, 2);
        xassert(seq->size == 6);
        xassert(seq->head == 3);

        xiseq_push_front(seq, 1);
        xassert(seq->size == 7);
        xassert(seq->head == 2);

        xassert_false(xiseq_push_front(seq, 8));
        xassert(seq->size == 7);
        xassert(seq->head == 2);

        xassert(xiseq_get_impl(seq, 0) == 1);
        xassert(xiseq_get_impl(seq, 1) == 2);
        xassert(xiseq_get_impl(seq, 2) == 3);
        xassert(xiseq_get_impl(seq, 3) == 4);
        xassert(xiseq_get_impl(seq, 4) == 5);
        xassert(xiseq_get_impl(seq, 5) == 6);
        xassert(xiseq_get_impl(seq, 6) == 7);

        xassert(xiseq_pop_front(seq) == 1);
        xassert(seq->size == 6);
        xassert(seq->head == 3);
        xassert(xiseq_pop_back(seq) == 7);
        xassert(seq->size == 5);
        xassert(seq->head == 3);
        xassert(xiseq_pop_front(seq) == 2);
        xassert(seq->size == 4);
        xassert(seq->head == 4);
        xassert(xiseq_pop_back(seq) == 6);
        xassert(seq->size == 3);
        xassert(seq->head == 4);
        xassert(xiseq_pop_front(seq) == 3);
        xassert(seq->size == 2);
        xassert(seq->head == 5);
        xassert(xiseq_pop_back(seq) == 5);
        xassert(seq->size == 1);
        xassert(seq->head == 5);
        xassert(xiseq_pop_front(seq) == 4);
        xassert(seq->size == 0);
        xassert(seq->head == 6);

        xiseq_free(&seq);
    }

    /* xiseq_pop_front*/
    {
        /* head == 0 */
        {
            XISeq_PT seq = xiseq_new(5);

            xiseq_push_back(seq, 1);
            xiseq_push_back(seq, 2);
            xiseq_push_back(seq, 3);
            xiseq_push_back(seq, 4);
            xiseq_push_back(seq, 5);

            xassert(xiseq_pop_front(seq) == 1);
            xassert(seq->size == 4);
            xassert(seq->head == 1);

            xassert(xiseq_pop_front(seq) == 2);
            xassert(seq->size == 3);
            xassert(seq->head == 2);

            xassert(xiseq_pop_front(seq) == 3);
            xassert(seq->size == 2);
            xassert(seq->head == 3);

            xiseq_free(&seq);
        }

        /* 0 < head */
        {
            XISeq_PT seq = xiseq_new(5);

            xiseq_push_front(seq, 1);
            xiseq_push_front(seq, 2);
            xiseq_push_front(seq, 3);

            xassert(xiseq_pop_front(seq) == 3);
            xassert(seq->size == 2);
            xassert(xiseq_pop_front(seq) == 2);
            xassert(seq->size == 1);
            xassert(xiseq_pop_front(seq) == 1);
            xassert(seq->size == 0);

            xiseq_free(&seq);
        }
    }

    /* xiseq_push_back */
    {
        XISeq_PT seq = xiseq_new(7);

        xiseq_push_back(seq, 3);
        xassert(seq->size == 1);
        xassert(seq->head == 0);

        xiseq_push_back(seq, 4);
        xassert(seq->size == 2);
        xassert(seq->head == 0);

        xiseq_push_back(seq, 5);
        xassert(seq->size == 3);
        xassert(seq->head == 0);

        xiseq_push_front(seq, 2);
        xassert(seq->size == 4);
        xassert(seq->head == 6);

        xiseq_push_front(seq, 1);
        xassert(seq->size == 5);
        xassert(seq->head == 5);

        xiseq_push_back(seq, 6);
        xassert(seq->size == 6);
        xassert(seq->head == 5);

        xiseq_push_back(seq, 7);
        xassert(seq->size == 7);
        xassert(seq->head == 5);

        xassert_false(xiseq_push_front(seq, 8));
        xassert(seq->size == 7);
        xassert(seq->head == 5);

        xassert(xiseq_get_impl(seq, 0) == 1);
        xassert(xiseq_get_impl(seq, 1) == 2);
        xassert(xiseq_get_impl(seq, 2) == 3);
        xassert(xiseq_get_impl(seq, 3) == 4);
        xassert(xiseq_get_impl(seq, 4) == 5);
        xassert(xiseq_get_impl(seq, 5) == 6);
        xassert(xiseq_get_impl(seq, 6) == 7);

        xassert(xiseq_pop_front(seq) == 1);
        xassert(seq->size == 6);
        xassert(seq->head == 6);
        xassert(xiseq_pop_back(seq) == 7);
        xassert(seq->size == 5);
        xassert(seq->head == 6);
        xassert(xiseq_pop_front(seq) == 2);
        xassert(seq->size == 4);
        xassert(seq->head == 0);
        xassert(xiseq_pop_back(seq) == 6);
        xassert(seq->size == 3);
        xassert(seq->head == 0);
        xassert(xiseq_pop_front(seq) == 3);
        xassert(seq->size == 2);
        xassert(seq->head == 1);
        xassert(xiseq_pop_back(seq) == 5);
        xassert(seq->size == 1);
        xassert(seq->head == 1);
        xassert(xiseq_pop_front(seq) == 4);
        xassert(seq->size == 0);
        xassert(seq->head == 2);

        xiseq_free(&seq);
    }

    /* xiseq_pop_back */
    {
        /* head == 0 */
        {
            XISeq_PT seq = xiseq_new(5);

            xiseq_push_back(seq, 1);
            xiseq_push_back(seq, 2);
            xiseq_push_back(seq, 3);
            xiseq_push_back(seq, 4);
            xiseq_push_back(seq, 5);

            xassert(xiseq_pop_back(seq) == 5);
            xassert(xiseq_pop_back(seq) == 4);
            xassert(xiseq_pop_back(seq) == 3);

            xiseq_free(&seq);
        }

        /* 0 < head */
        {
            XISeq_PT seq = xiseq_new(5);

            xiseq_push_front(seq, 1);
            xiseq_push_front(seq, 2);
            xiseq_push_front(seq, 3);

            xassert(xiseq_pop_back(seq) == 1);
            xassert(xiseq_pop_back(seq) == 2);
            xassert(xiseq_pop_back(seq) == 3);

            xiseq_free(&seq);
        }
    }

    /* xiseq_front */
    {
        xassert_false(xiseq_front(NULL));

        {
            XISeq_PT seq = xiseq_new(5);
            xassert_false(xiseq_front(NULL));
            xiseq_free(&seq);
        }

        {
            XISeq_PT seq = xiseq_new(5);
            xiseq_push_back(seq, 3);
            xiseq_push_back(seq, 3);
            xiseq_push_back(seq, 1);
            xiseq_push_back(seq, 2);
            xiseq_push_back(seq, 5);
            xiseq_pop_front(seq);
            xiseq_pop_front(seq);
            xiseq_push_back(seq, 3);

            xassert(xiseq_front(seq) == 1);

            xiseq_free(&seq);
        }
    }

    /* xiseq_back */
    {
        xassert_false(xiseq_back(NULL));

        {
            XISeq_PT seq = xiseq_new(5);
            xassert_false(xiseq_back(NULL));
            xiseq_free(&seq);
        }

        {
            XISeq_PT seq = xiseq_new(5);
            xiseq_push_back(seq, 3);
            xiseq_push_back(seq, 3);
            xiseq_push_back(seq, 1);
            xiseq_push_back(seq, 2);
            xiseq_push_back(seq, 5);
            xiseq_pop_front(seq);
            xiseq_pop_front(seq);

            xassert(xiseq_back(seq) == 5);

            xiseq_push_back(seq, 3);

            xassert(xiseq_back(seq) == 3);

            xiseq_free(&seq);
        }
    }

    /* xiseq_put */
    {
        {
            XISeq_PT seq = xiseq_new(10000);
            xiseq_put_impl(seq, XUTILS_SEQ_DEFAULT_LENGTH + 30, 1);
            xiseq_put_impl(seq, XUTILS_SEQ_DEFAULT_LENGTH + 800, 2);
            xassert(xiseq_get_impl(seq, XUTILS_SEQ_DEFAULT_LENGTH + 30) == 1);
            xassert(xiseq_get_impl(seq, XUTILS_SEQ_DEFAULT_LENGTH + 800) == 2);
            xiseq_free(&seq);
        }

        {
            XISeq_PT seq = xiseq_new(10000);
            xiseq_push_back(seq, 3);
            xiseq_push_back(seq, 3);
            xiseq_push_back(seq, 1);
            xiseq_push_back(seq, 2);
            xiseq_push_back(seq, 5);
            xiseq_pop_front(seq);
            xiseq_pop_front(seq);

            xiseq_put_impl(seq, XUTILS_SEQ_DEFAULT_LENGTH + 30, 1);
            xiseq_put_impl(seq, XUTILS_SEQ_DEFAULT_LENGTH + 800, 2);
            xassert(xiseq_get_impl(seq, XUTILS_SEQ_DEFAULT_LENGTH + 30) == 1);
            xassert(xiseq_get_impl(seq, XUTILS_SEQ_DEFAULT_LENGTH + 800) == 2);
            xiseq_free(&seq);
        }

        {
            XISeq_PT seq = xiseq_new(10000);
            xiseq_push_front(seq, 3);
            xiseq_push_front(seq, 3);
            xiseq_push_front(seq, 1);

            xiseq_put_impl(seq, XUTILS_SEQ_DEFAULT_LENGTH + 30, 1);
            xiseq_put_impl(seq, XUTILS_SEQ_DEFAULT_LENGTH + 800, 2);
            xassert(xiseq_get_impl(seq, XUTILS_SEQ_DEFAULT_LENGTH + 30) == 1);
            xassert(xiseq_get_impl(seq, XUTILS_SEQ_DEFAULT_LENGTH + 800) == 2);
            xiseq_free(&seq);
        }

        /* 0 < capacity, i < seq->array->size */
        {
            XISeq_PT seq = xiseq_new(11);
            xiseq_put_impl(seq, 10, 1);
            xassert(xiseq_get_impl(seq, 10) == 1);
            xassert(xiarray_size(seq->array) == 11);
            xiseq_free(&seq);
        }
    }

    /* xiseq_get */

    /* xiseq_expand */
    {
        /* |5|6|7|0|1|2|3|4|   ->   |5|6|7|-|-|-|0|1|2|3|4| */
        XISeq_PT seq = xiseq_new(8);
        xiseq_push_front(seq, 4);
        xiseq_push_front(seq, 3);
        xiseq_push_front(seq, 2);
        xiseq_push_front(seq, 1);
        xiseq_push_front(seq, 0);
        xiseq_push_back(seq, 5);
        xiseq_push_back(seq, 6);
        xiseq_push_back(seq, 7);

        xiseq_expand(seq, 3);
        xassert(seq->size == 8);
        xassert(seq->array->size == 11);
        xassert(seq->head == 6);

        xassert(xiseq_get_impl(seq, 0) == 0);
        xassert(xiseq_get_impl(seq, 1) == 1);
        xassert(xiseq_get_impl(seq, 2) == 2);
        xassert(xiseq_get_impl(seq, 3) == 3);
        xassert(xiseq_get_impl(seq, 4) == 4);
        xassert(xiseq_get_impl(seq, 5) == 5);
        xassert(xiseq_get_impl(seq, 6) == 6);
        xassert(xiseq_get_impl(seq, 7) == 7);
        xassert_false(xiseq_get_impl(seq, 8));
        xassert_false(xiseq_get_impl(seq, 9));
        xassert_false(xiseq_get_impl(seq, 10));

        xiseq_clear(seq);
        xassert(seq->size == 0);
        xassert(seq->head == 0);

        xiseq_free(&seq);
    }

    /* xiseq_free */
    /* xiseq_free */
    /* xiseq_clear */
    /* xiseq_deep_clear */
    {
        XISeq_PT seq = xiseq_random_int(10);
        int t = xiseq_pop_front(seq);
        t = xiseq_pop_front(seq);
        xiseq_clear(seq);
        xassert(seq->size == 0);
        xassert(seq->head == 0);
        xiseq_free(&seq);
    }

    /* xiseq_size */
    /* xiseq_is_empty */
    /* xiseq_swap */

    /* xiseq_heapify_impl */
    {
        {
            XISeq_PT seq = xiseq_random_int(1000);
            xassert(xiseq_heapify_impl(seq, 0, 999, true));
            xiseq_free(&seq);
        }

        {
            for (int i = 100; i < 200; ++i) {
                XISeq_PT seq = xiseq_random_int(i);
                xassert(xiseq_heapify_impl(seq, 0, i / 3, true));
                xassert(xiseq_heapify_impl(seq, i / 3, i / 3 * 2, true));
                xassert(xiseq_heapify_impl(seq, i / 3 * 2, seq->size - 1, true));
                xiseq_free(&seq);
            }
        }

        {
            XISeq_PT seq = xiseq_random_int(1000);
            xassert(xiseq_heapify_impl(seq, 0, 999, false));
            xiseq_free(&seq);
        }

        {
            for (int i = 100; i < 200; ++i) {
                XISeq_PT seq = xiseq_random_int(i);
                xassert(xiseq_heapify_impl(seq, 0, i / 3, false));
                xassert(xiseq_heapify_impl(seq, i / 3, i / 3 * 2, false));
                xassert(xiseq_heapify_impl(seq, i / 3 * 2, seq->size - 1, false));
                xiseq_free(&seq);
            }
        }
    }

    /* xiseq_heapify_min */
    {
        {
            XISeq_PT seq = xiseq_random_int(1000);
            xassert(xiseq_heapify_min(seq));
            xiseq_free(&seq);
        }

        {
            for (int i = 1; i < 200; ++i) {
                XISeq_PT seq = xiseq_random_int(i);
                xassert(xiseq_heapify_min(seq));
                xiseq_free(&seq);
            }
        }
    }

    /* xiseq_heapify_max */
    {
        {
            XISeq_PT seq = xiseq_random_int(1000);
            xassert(xiseq_heapify_max(seq));
            xiseq_free(&seq);
        }

        {
            for (int i = 1; i < 200; ++i) {
                XISeq_PT seq = xiseq_random_int(i);
                xassert(xiseq_heapify_max(seq));
                xiseq_free(&seq);
            }
        }
    }

    /* xiseq_heap_sort_impl */
    {
        for (int i = 100; i < 200; ++i) {
            XISeq_PT seq = xiseq_random_int(i);
            xassert(xiseq_heap_sort_impl(seq, 0, i / 3, true));
            xassert(xiseq_heap_sort_impl(seq, i / 3, i / 3 * 2, true));
            xassert(xiseq_heap_sort_impl(seq, i / 3 * 2, seq->size - 1, true));
            xiseq_free(&seq);
        }

        for (int i = 100; i < 200; ++i) {
            XISeq_PT seq = xiseq_random_int(i);
            xassert(xiseq_heap_sort_impl(seq, 0, i / 3, false));
            xassert(xiseq_heap_sort_impl(seq, i / 3, i / 3 * 2, false));
            xassert(xiseq_heap_sort_impl(seq, i / 3 * 2, seq->size - 1, false));
            xiseq_free(&seq);
        }
    }

    /* xiseq_heap_sort */
    {
        {
            XISeq_PT seq = xiseq_random_int(1000);
            xassert(xiseq_heap_sort(seq));
            xiseq_free(&seq);
        }

        {
            for (int i = 1; i < 200; ++i) {
                XISeq_PT seq = xiseq_random_int(i);
                xassert(xiseq_heap_sort(seq));
                xiseq_free(&seq);
            }
        }
    }

    /* xiseq_quick_sort */
    {
        {
            XISeq_PT seq = xiseq_random_int(1000);
            {
                int x = xiseq_pop_front(seq);
            }
            {
                int x = xiseq_pop_front(seq);
            }
            {
                int x = xiseq_pop_front(seq);
            }
            xassert(xiseq_quick_sort(seq));
            xassert(xiseq_quick_sort(seq));
            xassert(xiseq_quick_sort(seq));
            xiseq_free(&seq);
        }

        {
            for (int i = 1; i < 200; ++i) {
                XISeq_PT seq = xiseq_random_int(i);
                xassert(xiseq_quick_sort(seq));
                xassert(xiseq_quick_sort(seq));
                xiseq_free(&seq);
            }
        }
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}


