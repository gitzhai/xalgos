
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
int test_xindexmaxpq_cmp(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static 
bool test_xindexmaxpq_print(int i, void *x, void *cl) {
    printf("%d:%s\n", i, (char*)x);
    return true;
}

static
bool test_xindexmaxpq_find_str_true(int i, void* ptr, void* cl) {
    if (strcmp(ptr, (char*)cl) == 0) {
        return true;
    }
    return false;
}

static
bool test_xindexmaxpq_find_str_false(int i, void* ptr, void* cl) {
    if (strcmp(ptr, (char*)cl) == 0) {
        return false;
    }
    return true;
}

static
bool test_xindexmaxpq_apply_true(int i, void *atom_str, void *cl) {
    return true;
}

static
bool test_xindexmaxpq_apply_false(int i, void *atom_str, void *cl) {
    return false;
}

static
bool test_xindexmaxpq_xiseq_sorted_impl(XIndexMaxPQ_PT heap, int lo, int hi, bool min_to_max) {
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
bool test_xindexmaxpq_xiseq_is_sorted(XIndexMaxPQ_PT heap) {
    return test_xindexmaxpq_xiseq_sorted_impl(heap, 0, heap->heap->size - 1, true);
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static 
XIndexMaxPQ_PT xindexmaxpq_random_string(int size) {
    XIndexMaxPQ_PT heap = xindexmaxpq_new(size, test_xindexmaxpq_cmp, NULL);
    if (!heap) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xindexmaxpq_deep_free(&heap);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            str[str_size - 1] = '\0';

            xindexmaxpq_push(heap, i, str, NULL);
        }
    }

    return heap;
}

void test_xindexmaxpq() {

    /* xindexmaxpq_new */
    {
        XIndexMaxPQ_PT heap = xindexmaxpq_new(5, test_xindexmaxpq_cmp, NULL);
        xassert(heap);
        xassert(heap->buckets);
        xassert(heap->heap);
        xassert(heap->index);
        xassert(xindexmaxpq_size(heap) == 0);
        xindexmaxpq_free(&heap);
    }

    /* xindexmaxpq_push */
    {
        XIndexMaxPQ_PT heap = xindexmaxpq_new(10, test_xindexmaxpq_cmp, NULL);
        xassert(xindexmaxpq_push(heap, 0, "a", NULL));
        xassert(xindexmaxpq_push(heap, 1, "b", NULL));
        xassert(xindexmaxpq_push(heap, 2, "c", NULL));
        xassert(xindexmaxpq_push(heap, 3, "d", NULL));
        xassert(xindexmaxpq_push(heap, 4, "e", NULL));
        xassert(xindexmaxpq_size(heap) == 5);
        xassert(xindexmaxpq_is_maxpq(heap));
        xindexmaxpq_free(&heap);
    }

    /* xindexmaxpq_pop */
    {
        {
            char* str = NULL;
            int  index = 0;
            XIndexMaxPQ_PT heap = xindexmaxpq_new(5, test_xindexmaxpq_cmp, NULL);
            xassert(xindexmaxpq_push(heap, 0, "a", NULL));
            xassert(xindexmaxpq_push(heap, 1, "b", NULL));
            xassert(xindexmaxpq_push(heap, 2, "c", NULL));
            xassert(xindexmaxpq_push(heap, 3, "d", NULL));
            xassert(xindexmaxpq_push(heap, 4, "e", NULL));
            xassert(xindexmaxpq_size(heap) == 5);
            xassert(xindexmaxpq_is_maxpq(heap));

            xassert(str = xindexmaxpq_pop(heap, &index));
            xassert(index == 4);
            xassert(strcmp(str, "e") == 0);
            xassert(xindexmaxpq_size(heap) == 4);

            xassert(str = xindexmaxpq_pop(heap, &index));
            xassert(index == 3);
            xassert(strcmp(str, "d") == 0);
            xassert(xindexmaxpq_size(heap) == 3);

            xassert(str = xindexmaxpq_pop(heap, &index));
            xassert(index == 2);
            xassert(strcmp(str, "c") == 0);
            xassert(xindexmaxpq_size(heap) == 2);

            xassert(str = xindexmaxpq_pop(heap, &index));
            xassert(index == 1);
            xassert(strcmp(str, "b") == 0);
            xassert(xindexmaxpq_size(heap) == 1);

            xassert(str = xindexmaxpq_pop(heap, &index));
            xassert(index == 0);
            xassert(strcmp(str, "a") == 0);
            xassert(xindexmaxpq_size(heap) == 0);

            xindexmaxpq_free(&heap);
        }

        {
            XIndexMaxPQ_PT heap = xindexmaxpq_new(50000, test_xindexmaxpq_cmp, NULL);
            for (int i = -1; i < XUTILS_DEQUE_LAYER2_DEFAULT_LENGTH;) {
                xassert(xindexmaxpq_push(heap, ++i, "a", NULL));
                xassert(xindexmaxpq_push(heap, ++i, "b", NULL));
                xassert(xindexmaxpq_push(heap, ++i, "c", NULL));
                xassert(xindexmaxpq_push(heap, ++i, "d", NULL));
                xassert(xindexmaxpq_push(heap, ++i, "e", NULL));
            }
            xassert(xindexmaxpq_is_maxpq(heap));
            //xindexmaxpq_map(heap, test_xindexmaxpq_print, NULL);

            void *data = NULL;
            int index = 0;
            int count = 0;
            while (!xindexmaxpq_is_empty(heap)) {
                ++count;
                xassert(data = xindexmaxpq_pop(heap, &index));
                xassert(data);
                xassert(index != -1);
                if (count % 5000 == 0) {
                    xassert(xindexmaxpq_is_maxpq(heap));
                }
            }
            xassert(xindexmaxpq_size(heap) == 0);
            xindexmaxpq_free(&heap);
        }
    }

    /* xindexmaxpq_peek */
    {
        XIndexMaxPQ_PT heap = xindexmaxpq_new(5, test_xindexmaxpq_cmp, NULL);
        xassert(xindexmaxpq_push(heap, 0, "a", NULL));
        xassert(xindexmaxpq_push(heap, 1, "b", NULL));
        xassert(xindexmaxpq_push(heap, 2, "c", NULL));
        xassert(xindexmaxpq_push(heap, 3, "d", NULL));
        xassert(xindexmaxpq_push(heap, 4, "e", NULL));
        xassert(xindexmaxpq_size(heap) == 5);
        xassert(xindexmaxpq_is_maxpq(heap));

        void *data = NULL;
        int index = 0;
        data = xindexmaxpq_peek(heap, &index);
        xassert(strcmp((char*)data, "e") == 0);
        xassert(index == 4);
        data = xindexmaxpq_peek(heap, &index);
        xassert(strcmp((char*)data, "e") == 0);
        xassert(index == 4);
        xassert(xindexmaxpq_size(heap) == 5);
        xindexmaxpq_free(&heap);
    }

    /* xindexmaxpq_map */
    {
        XIndexMaxPQ_PT heap = xindexmaxpq_new(5, test_xindexmaxpq_cmp, NULL);
        xassert(xindexmaxpq_push(heap, 0, "a", NULL));
        xassert(xindexmaxpq_push(heap, 1, "b", NULL));
        xassert(xindexmaxpq_push(heap, 2, "c", NULL));
        xassert(xindexmaxpq_push(heap, 3, "d", NULL));
        xassert(xindexmaxpq_push(heap, 4, "e", NULL));
        xassert(xindexmaxpq_size(heap) == 5);
        xassert(xindexmaxpq_is_maxpq(heap));

        xassert(xindexmaxpq_map(heap, test_xindexmaxpq_apply_true, NULL) == 5);
        xassert(xindexmaxpq_map(heap, test_xindexmaxpq_apply_false, NULL) == 0);

        //xassert(xindexmaxpq_map(heap, test_xindexmaxpq_print, NULL) == 5);
        xassert(xindexmaxpq_size(heap) == 5);
        xindexmaxpq_free(&heap);
    }

    /* xindexmaxpq_map_break_if_true */
    {
        XIndexMaxPQ_PT heap = xindexmaxpq_new(5, test_xindexmaxpq_cmp, NULL);
        xassert(xindexmaxpq_push(heap, 0, "a", NULL));
        xassert(xindexmaxpq_push(heap, 1, "b", NULL));
        xassert(xindexmaxpq_push(heap, 2, "c", NULL));
        xassert(xindexmaxpq_push(heap, 3, "d", NULL));
        xassert(xindexmaxpq_push(heap, 4, "e", NULL));
        xassert(xindexmaxpq_size(heap) == 5);
        xassert(xindexmaxpq_is_maxpq(heap));

        xassert(xindexmaxpq_map_break_if_true(heap, test_xindexmaxpq_apply_true, NULL));
        xassert_false(xindexmaxpq_map_break_if_true(heap, test_xindexmaxpq_apply_false, NULL));

        xassert(xindexmaxpq_map_break_if_true(heap, test_xindexmaxpq_find_str_true, "c"));
        xassert_false(xindexmaxpq_map_break_if_true(heap, test_xindexmaxpq_find_str_true, "x"));

        xassert(xindexmaxpq_size(heap) == 5);
        xindexmaxpq_free(&heap);
    }

    /* xindexmaxpq_map_break_if_false */
    {
        XIndexMaxPQ_PT heap = xindexmaxpq_new(5, test_xindexmaxpq_cmp, NULL);
        xassert(xindexmaxpq_push(heap, 0, "a", NULL));
        xassert(xindexmaxpq_push(heap, 1, "b", NULL));
        xassert(xindexmaxpq_push(heap, 2, "c", NULL));
        xassert(xindexmaxpq_push(heap, 3, "d", NULL));
        xassert(xindexmaxpq_push(heap, 4, "e", NULL));
        xassert(xindexmaxpq_size(heap) == 5);
        xassert(xindexmaxpq_is_maxpq(heap));

        xassert(xindexmaxpq_map_break_if_false(heap, test_xindexmaxpq_apply_false, NULL));
        xassert_false(xindexmaxpq_map_break_if_false(heap, test_xindexmaxpq_apply_true, NULL));

        xassert(xindexmaxpq_map_break_if_false(heap, test_xindexmaxpq_find_str_false, "c"));
        xassert_false(xindexmaxpq_map_break_if_false(heap, test_xindexmaxpq_find_str_false, "x"));

        xassert(xindexmaxpq_size(heap) == 5);
        xindexmaxpq_free(&heap);
    }

    /* xindexmaxpq_free : testef already by other cases */
    /* xindexmaxpq_deep_free */
    {
        XIndexMaxPQ_PT heap = xindexmaxpq_random_string(100);
        xassert(xindexmaxpq_is_maxpq(heap));
        xindexmaxpq_deep_free(&heap);
    }

    /* xindexmaxpq_clear */
    {
        XIndexMaxPQ_PT heap = xindexmaxpq_new(5, test_xindexmaxpq_cmp, NULL);
        xassert(xindexmaxpq_push(heap, 0, "a", NULL));
        xassert(xindexmaxpq_push(heap, 1, "b", NULL));
        xassert(xindexmaxpq_push(heap, 2, "c", NULL));
        xassert(xindexmaxpq_push(heap, 3, "d", NULL));
        xassert(xindexmaxpq_push(heap, 4, "e", NULL));
        xassert(xindexmaxpq_size(heap) == 5);
        xassert(xindexmaxpq_is_maxpq(heap));

        xindexmaxpq_clear(heap);
        xindexmaxpq_deep_free(&heap);
    }

    /* xindexmaxpq_deep_clear */
    {
        XIndexMaxPQ_PT heap = xindexmaxpq_random_string(100);
        xindexmaxpq_deep_clear(heap);
        xindexmaxpq_free(&heap);
    }

    /* xindexmaxpq_size */
    /* xindexmaxpq_is_empty */
    {
        XIndexMaxPQ_PT heap = xindexmaxpq_new(5, test_xindexmaxpq_cmp, NULL);
        xassert(xindexmaxpq_size(heap) == 0);
        xassert(xindexmaxpq_is_empty(heap));

        xassert(xindexmaxpq_push(heap, 0, "c", NULL));
        xassert(xindexmaxpq_push(heap, 1, "d", NULL));
        xassert(xindexmaxpq_size(heap) == 2);
        xassert_false(xindexmaxpq_is_empty(heap));

        xindexmaxpq_free(&heap);
    }

    /* xindexmaxpq_is_maxpq */
    {
        /* tested already */
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
