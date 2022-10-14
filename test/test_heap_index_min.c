
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/xutils.h"
#include "../queue_sequence_int/xqueue_sequence_int_x.h"
#include "../array_pointer/xarray_pointer_x.h"
#include "../heap_index_min/xheap_index_min_x.h"
#include "../include/xalgos.h"

static 
int test_xindexminheap_cmp(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static 
bool test_xindexminheap_print(int i, void *x, void *cl) {
    printf("%d:%s\n", i, (char*)x);
    return true;
}

static
bool test_xindexminheap_find_str_true(int i, void* ptr, void* cl) {
    if (strcmp(ptr, (char*)cl) == 0) {
        return true;
    }
    return false;
}

static
bool test_xindexminheap_find_str_false(int i, void* ptr, void* cl) {
    if (strcmp(ptr, (char*)cl) == 0) {
        return false;
    }
    return true;
}

static
bool test_xindexminheap_apply_true(int i, void *atom_str, void *cl) {
    return true;
}

static
bool test_xindexminheap_apply_false(int i, void *atom_str, void *cl) {
    return false;
}

static
bool test_xindexminheap_xiseq_sorted_impl(XIndexMinHeap_PT heap, int lo, int hi, bool min_to_max) {
    if (min_to_max) {
        for (int i = lo + 1; i <= hi; ++i) {
            if (heap->cmp(xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, i)), xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, i - 1)), heap->cl) < 0) {
                return false;
            }
        }
    }
    else {
        for (int i = lo + 1; i <= hi; ++i) {
            if (heap->cmp(xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, i - 1)), xparray_get_impl(heap->buckets, xiseq_get_impl(heap->heap, i)), heap->cl) < 0) {
                return false;
            }
        }
    }

    return true;
}

static
bool test_xindexminheap_xiseq_is_sorted(XIndexMinHeap_PT heap) {
    return test_xindexminheap_xiseq_sorted_impl(heap, 0, heap->heap->size - 1, true);
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static 
XIndexMinHeap_PT xindexminheap_random_string(int size) {
    XIndexMinHeap_PT heap = xindexminheap_new(size, test_xindexminheap_cmp, NULL);
    if (!heap) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xindexminheap_deep_free(&heap);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            str[str_size - 1] = '\0';

            xindexminheap_push(heap, i, str, NULL);
        }
    }

    return heap;
}

void test_xindexminheap() {

    /* xindexminheap_new */
    {
        XIndexMinHeap_PT heap = xindexminheap_new(5, test_xindexminheap_cmp, NULL);
        xassert(heap);
        xassert(heap->buckets);
        xassert(heap->heap);
        xassert(heap->index);
        xassert(xindexminheap_size(heap) == 0);
        xindexminheap_free(&heap);
    }

    /* xindexminheap_push */
    {
        XIndexMinHeap_PT heap = xindexminheap_new(10, test_xindexminheap_cmp, NULL);
        xassert(xindexminheap_push(heap, 0, "a", NULL));
        xassert(xindexminheap_push(heap, 1, "b", NULL));
        xassert(xindexminheap_push(heap, 2, "c", NULL));
        xassert(xindexminheap_push(heap, 3, "d", NULL));
        xassert(xindexminheap_push(heap, 4, "e", NULL));
        xassert(xindexminheap_size(heap) == 5);
        xassert(xindexminheap_is_minheap(heap));
        xindexminheap_free(&heap);
    }

    /* xindexminheap_pop */
    {
        {
            char* str = NULL;
            int  index = 0;
            XIndexMinHeap_PT heap = xindexminheap_new(5, test_xindexminheap_cmp, NULL);
            xassert(xindexminheap_push(heap, 0, "a", NULL));
            xassert(xindexminheap_push(heap, 1, "b", NULL));
            xassert(xindexminheap_push(heap, 2, "c", NULL));
            xassert(xindexminheap_push(heap, 3, "d", NULL));
            xassert(xindexminheap_push(heap, 4, "e", NULL));
            xassert(xindexminheap_size(heap) == 5);
            xassert(xindexminheap_is_minheap(heap));

            xassert(str = xindexminheap_pop(heap, &index));
            xassert(index == 0);
            xassert(strcmp(str, "a") == 0);
            xassert(xindexminheap_size(heap) == 4);

            xassert(str = xindexminheap_pop(heap, &index));
            xassert(index == 1);
            xassert(strcmp(str, "b") == 0);
            xassert(xindexminheap_size(heap) == 3);

            xassert(str = xindexminheap_pop(heap, &index));
            xassert(index == 2);
            xassert(strcmp(str, "c") == 0);
            xassert(xindexminheap_size(heap) == 2);

            xassert(str = xindexminheap_pop(heap, &index));
            xassert(index == 3);
            xassert(strcmp(str, "d") == 0);
            xassert(xindexminheap_size(heap) == 1);

            xassert(str = xindexminheap_pop(heap, &index));
            xassert(index == 4);
            xassert(strcmp(str, "e") == 0);
            xassert(xindexminheap_size(heap) == 0);

            xindexminheap_free(&heap);
        }

        {
            XIndexMinHeap_PT heap = xindexminheap_new(50000, test_xindexminheap_cmp, NULL);
            for (int i = -1; i < XUTILS_DEQUE_LAYER2_DEFAULT_LENGTH;) {
                xassert(xindexminheap_push(heap, ++i, "a", NULL));
                xassert(xindexminheap_push(heap, ++i, "b", NULL));
                xassert(xindexminheap_push(heap, ++i, "c", NULL));
                xassert(xindexminheap_push(heap, ++i, "d", NULL));
                xassert(xindexminheap_push(heap, ++i, "e", NULL));
            }
            xassert(xindexminheap_is_minheap(heap));
            //xindexminheap_map(heap, test_xindexminheap_print, NULL);

            void *data = NULL;
            int index = 0;
            int count = 0;
            while (!xindexminheap_is_empty(heap)) {
                ++count;
                xassert(data = xindexminheap_pop(heap, &index));
                xassert(index != -1);
                if (count % 5000 == 0) {
                    xassert(xindexminheap_is_minheap(heap));
                }
            }
            xassert(xindexminheap_size(heap) == 0);
            xindexminheap_free(&heap);
        }
    }

    /* xindexminheap_peek */
    {
        XIndexMinHeap_PT heap = xindexminheap_new(5, test_xindexminheap_cmp, NULL);
        xassert(xindexminheap_push(heap, 0, "a", NULL));
        xassert(xindexminheap_push(heap, 1, "b", NULL));
        xassert(xindexminheap_push(heap, 2, "c", NULL));
        xassert(xindexminheap_push(heap, 3, "d", NULL));
        xassert(xindexminheap_push(heap, 4, "e", NULL));
        xassert(xindexminheap_size(heap) == 5);
        xassert(xindexminheap_is_minheap(heap));

        void *data = NULL;
        int index = 0;
        data = xindexminheap_peek(heap, &index);
        xassert(strcmp((char*)data, "a") == 0);
        xassert(index == 0);
        data = xindexminheap_peek(heap, &index);
        xassert(strcmp((char*)data, "a") == 0);
        xassert(index == 0);
        xassert(xindexminheap_size(heap) == 5);
        xindexminheap_free(&heap);
    }

    /* xindexminheap_map */
    {
        XIndexMinHeap_PT heap = xindexminheap_new(5, test_xindexminheap_cmp, NULL);
        xassert(xindexminheap_push(heap, 0, "a", NULL));
        xassert(xindexminheap_push(heap, 1, "b", NULL));
        xassert(xindexminheap_push(heap, 2, "c", NULL));
        xassert(xindexminheap_push(heap, 3, "d", NULL));
        xassert(xindexminheap_push(heap, 4, "e", NULL));
        xassert(xindexminheap_size(heap) == 5);
        xassert(xindexminheap_is_minheap(heap));

        xassert(xindexminheap_map(heap, test_xindexminheap_apply_true, NULL) == 5);
        xassert(xindexminheap_map(heap, test_xindexminheap_apply_false, NULL) == 0);

        //xassert(xindexminheap_map(heap, test_xindexminheap_print, NULL) == 5);
        xassert(xindexminheap_size(heap) == 5);
        xindexminheap_free(&heap);
    }

    /* xindexminheap_map_break_if_true */
    {
        XIndexMinHeap_PT heap = xindexminheap_new(5, test_xindexminheap_cmp, NULL);
        xassert(xindexminheap_push(heap, 0, "a", NULL));
        xassert(xindexminheap_push(heap, 1, "b", NULL));
        xassert(xindexminheap_push(heap, 2, "c", NULL));
        xassert(xindexminheap_push(heap, 3, "d", NULL));
        xassert(xindexminheap_push(heap, 4, "e", NULL));
        xassert(xindexminheap_size(heap) == 5);
        xassert(xindexminheap_is_minheap(heap));

        xassert(xindexminheap_map_break_if_true(heap, test_xindexminheap_apply_true, NULL));
        xassert_false(xindexminheap_map_break_if_true(heap, test_xindexminheap_apply_false, NULL));

        xassert(xindexminheap_map_break_if_true(heap, test_xindexminheap_find_str_true, "c"));
        xassert_false(xindexminheap_map_break_if_true(heap, test_xindexminheap_find_str_true, "x"));

        xassert(xindexminheap_size(heap) == 5);
        xindexminheap_free(&heap);
    }

    /* xindexminheap_map_break_if_false */
    {
        XIndexMinHeap_PT heap = xindexminheap_new(5, test_xindexminheap_cmp, NULL);
        xassert(xindexminheap_push(heap, 0, "a", NULL));
        xassert(xindexminheap_push(heap, 1, "b", NULL));
        xassert(xindexminheap_push(heap, 2, "c", NULL));
        xassert(xindexminheap_push(heap, 3, "d", NULL));
        xassert(xindexminheap_push(heap, 4, "e", NULL));
        xassert(xindexminheap_size(heap) == 5);
        xassert(xindexminheap_is_minheap(heap));

        xassert(xindexminheap_map_break_if_false(heap, test_xindexminheap_apply_false, NULL));
        xassert_false(xindexminheap_map_break_if_false(heap, test_xindexminheap_apply_true, NULL));

        xassert(xindexminheap_map_break_if_false(heap, test_xindexminheap_find_str_false, "c"));
        xassert_false(xindexminheap_map_break_if_false(heap, test_xindexminheap_find_str_false, "x"));

        xassert(xindexminheap_size(heap) == 5);
        xindexminheap_free(&heap);
    }

    /* xindexminheap_free : testef already by other cases */
    /* xindexminheap_deep_free */
    {
        XIndexMinHeap_PT heap = xindexminheap_random_string(100);
        xassert(xindexminheap_is_minheap(heap));
        xindexminheap_deep_free(&heap);
    }

    /* xindexminheap_clear */
    {
        XIndexMinHeap_PT heap = xindexminheap_new(5, test_xindexminheap_cmp, NULL);
        xassert(xindexminheap_push(heap, 0, "a", NULL));
        xassert(xindexminheap_push(heap, 1, "b", NULL));
        xassert(xindexminheap_push(heap, 2, "c", NULL));
        xassert(xindexminheap_push(heap, 3, "d", NULL));
        xassert(xindexminheap_push(heap, 4, "e", NULL));
        xassert(xindexminheap_size(heap) == 5);
        xassert(xindexminheap_is_minheap(heap));

        xindexminheap_clear(heap);
        xindexminheap_deep_free(&heap);
    }

    /* xindexminheap_deep_clear */
    {
        XIndexMinHeap_PT heap = xindexminheap_random_string(100);
        xindexminheap_deep_clear(heap);
        xindexminheap_free(&heap);
    }

    /* xindexminheap_size */
    /* xindexminheap_is_empty */
    {
        XIndexMinHeap_PT heap = xindexminheap_new(5, test_xindexminheap_cmp, NULL);
        xassert(xindexminheap_size(heap) == 0);
        xassert(xindexminheap_is_empty(heap));

        xassert(xindexminheap_push(heap, 0, "c", NULL));
        xassert(xindexminheap_push(heap, 1, "d", NULL));
        xassert(xindexminheap_size(heap) == 2);
        xassert_false(xindexminheap_is_empty(heap));

        xindexminheap_free(&heap);
    }

    /* xindexminheap_is_minheap */
    {
        /* tested already */
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
