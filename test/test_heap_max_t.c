
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/xutils.h"
#include "../queue_deque/xqueue_deque_x.h"
#include "../heap_min_t/theap_min_x.h"
#include "../heap_max_t/theap_max.h"
#include "../include/xalgos.h"

static
int test_tmaxheap_cmp(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static
int test_tmaxheap_cmp_reverse(void *x, void *y, void *cl) {
    return strcmp((char*)y, (char*)x);
}

static
bool test_tmaxheap_print(void *x, void *cl) {
    printf("%s\n", (char*)x);
    return true;
}

static
bool test_tmaxheap_find_str_true(void* ptr, void* cl) {
    if (strcmp(ptr, (char*)cl) == 0) {
        return true;
    }
    return false;
}

static
bool test_tmaxheap_find_str_false(void* ptr, void* cl) {
    if (strcmp(ptr, (char*)cl) == 0) {
        return false;
    }
    return true;
}

static
bool test_tmaxheap_apply_true(void *atom_str, void *cl) {
    return true;
}

static
bool test_tmaxheap_apply_false(void *atom_str, void *cl) {
    return false;
}


static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static
TMaxHeap_PT tmaxheap_random_string(int size) {
    TMaxHeap_PT heap = tmaxheap_new(size, test_tmaxheap_cmp, NULL);
    if (!heap) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                tmaxheap_deep_free(&heap);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            str[str_size - 1] = '\0';

            tmaxheap_push(heap, str);
        }
    }

    return heap;
}

static
TMaxHeap_PT tmaxheap_random_string_no_limit(int size) {
    TMaxHeap_PT heap = tmaxheap_new(100, test_tmaxheap_cmp, NULL);
    if (!heap) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                tmaxheap_deep_free(&heap);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            str[str_size - 1] = '\0';

            xdeque_push_back_no_limit(heap->tree, str);

            xdeque_heapify_swim_elem(heap->tree, heap->tree->size - 1, 0, heap->tree->size - 1, false, heap->cmp, heap->cl);
        }
    }

    return heap;
}

void test_tmaxheap() {

    /* tmaxheap_new */
    {
        TMaxHeap_PT heap = tmaxheap_new(5, test_tmaxheap_cmp, NULL);
        xassert(heap);
        xassert(tmaxheap_size(heap) == 0);
        tmaxheap_free(&heap);
    }

    /* tmaxheap_push */
    {
        TMaxHeap_PT heap = tmaxheap_new(10, test_tmaxheap_cmp, NULL);
        xassert(tmaxheap_push(heap, "a"));
        xassert(tmaxheap_push(heap, "b"));
        xassert(tmaxheap_push(heap, "c"));
        xassert(tmaxheap_push(heap, "d"));
        xassert(tmaxheap_push(heap, "e"));
        xassert(tmaxheap_push(heap, "a"));
        xassert(tmaxheap_push(heap, "b"));
        xassert(tmaxheap_push(heap, "c"));
        xassert(tmaxheap_push(heap, "d"));
        xassert(tmaxheap_push(heap, "e"));
        xassert(tmaxheap_size(heap) == 10);
        xassert(tmaxheap_is_maxheap(heap));

        xassert_false(tmaxheap_push(heap, "f"));
        xassert(tmaxheap_size(heap) == 10);
        xassert_false(tmaxheap_push(heap, "g"));
        xassert(tmaxheap_size(heap) == 10);
        xassert_false(tmaxheap_push(heap, "h"));
        xassert(tmaxheap_size(heap) == 10);
        xassert(tmaxheap_is_maxheap(heap));

        tmaxheap_free(&heap);
    }

    /* tmaxheap_pop */
    {
        {
            char* str = NULL;
            TMaxHeap_PT heap = tmaxheap_new(5, test_tmaxheap_cmp, NULL);
            xassert(tmaxheap_push(heap, "a"));
            xassert(tmaxheap_push(heap, "b"));
            xassert(tmaxheap_push(heap, "c"));
            xassert(tmaxheap_push(heap, "d"));
            xassert(tmaxheap_push(heap, "e"));
            xassert(tmaxheap_size(heap) == 5);
            xassert(tmaxheap_is_maxheap(heap));

            xassert(str = tmaxheap_pop(heap));
            xassert(strcmp(str, "e") == 0);
            xassert(tmaxheap_size(heap) == 4);
            xassert(str = tmaxheap_pop(heap));
            xassert(strcmp(str, "d") == 0);
            xassert(tmaxheap_size(heap) == 3);
            xassert(str = tmaxheap_pop(heap));
            xassert(strcmp(str, "c") == 0);
            xassert(tmaxheap_size(heap) == 2);
            xassert(str = tmaxheap_pop(heap));
            xassert(strcmp(str, "b") == 0);
            xassert(tmaxheap_size(heap) == 1);
            xassert(str = tmaxheap_pop(heap));
            xassert(strcmp(str, "a") == 0);
            xassert(tmaxheap_size(heap) == 0);

            tmaxheap_free(&heap);
        }

        {
            TMaxHeap_PT heap = tmaxheap_new(0, test_tmaxheap_cmp, NULL);
            for (int i = 0; i < XUTILS_DEQUE_LAYER2_DEFAULT_LENGTH; i++) {
                xassert(tmaxheap_push(heap, "a"));
                xassert(tmaxheap_push(heap, "b"));
                xassert(tmaxheap_push(heap, "c"));
                xassert(tmaxheap_push(heap, "d"));
                xassert(tmaxheap_push(heap, "e"));
            }
            xassert(tmaxheap_is_maxheap(heap));
            xassert(tmaxheap_size(heap) == XUTILS_DEQUE_LAYER2_DEFAULT_LENGTH * 5);

            void *data = NULL;
            int count = 0;
            while (!tmaxheap_is_empty(heap)) {
                ++count;
                xassert(data = tmaxheap_pop(heap));
                xassert(data);
                if (count % 5000 == 0) {
                    xassert(tmaxheap_is_maxheap(heap));
                }
            }
            xassert(tmaxheap_size(heap) == 0);
            xassert(count == XUTILS_DEQUE_LAYER2_DEFAULT_LENGTH * 5);
            tmaxheap_free(&heap);
        }
    }

    {
        TMaxHeap_PT heap = tmaxheap_random_string_no_limit(20000);

        void *data = NULL;
        int count = 0;
        while (!tmaxheap_is_empty(heap)) {
            ++count;
            xassert(data = tmaxheap_pop(heap));
            xassert(data);
            XMEM_FREE(data);
            if (count % 1000 == 0) {
                xassert(tmaxheap_is_maxheap(heap));
            }
        }
        xassert(tmaxheap_size(heap) == 0);
        xassert(count == 20000);
        tmaxheap_free(&heap);
    }

    /* tmaxheap_peek */
    {
        TMaxHeap_PT heap = tmaxheap_new(5, test_tmaxheap_cmp, NULL);
        xassert(tmaxheap_push(heap, "a"));
        xassert(tmaxheap_push(heap, "b"));
        xassert(tmaxheap_push(heap, "c"));
        xassert(tmaxheap_push(heap, "d"));
        xassert(tmaxheap_push(heap, "e"));
        xassert(tmaxheap_size(heap) == 5);
        xassert(tmaxheap_is_maxheap(heap));

        xassert(strcmp((char*)tmaxheap_peek(heap), "e") == 0);
        xassert(strcmp((char*)tmaxheap_peek(heap), "e") == 0);
        xassert(tmaxheap_size(heap) == 5);
        tmaxheap_free(&heap);
    }

    /* tmaxheap_map */
    {
        TMaxHeap_PT heap = tmaxheap_new(5, test_tmaxheap_cmp, NULL);
        xassert(tmaxheap_push(heap, "a"));
        xassert(tmaxheap_push(heap, "e"));
        xassert(tmaxheap_push(heap, "c"));
        xassert(tmaxheap_push(heap, "b"));
        xassert(tmaxheap_push(heap, "d"));
        xassert(tmaxheap_size(heap) == 5);
        xassert(tmaxheap_is_maxheap(heap));

        xassert(tmaxheap_map(heap, test_tmaxheap_apply_true, NULL) == 5);
        xassert(tmaxheap_map(heap, test_tmaxheap_apply_false, NULL) == 0);

        //xassert(tmaxheap_map(heap, test_tmaxheap_print, NULL) == 5);
        xassert(tmaxheap_size(heap) == 5);
        tmaxheap_free(&heap);
    }

    /* tmaxheap_map_break_if_true */
    {
        TMaxHeap_PT heap = tmaxheap_new(5, test_tmaxheap_cmp, NULL);
        xassert(tmaxheap_push(heap, "a"));
        xassert(tmaxheap_push(heap, "b"));
        xassert(tmaxheap_push(heap, "c"));
        xassert(tmaxheap_push(heap, "d"));
        xassert(tmaxheap_push(heap, "e"));
        xassert(tmaxheap_size(heap) == 5);
        xassert(tmaxheap_is_maxheap(heap));

        xassert(tmaxheap_map_break_if_true(heap, test_tmaxheap_apply_true, NULL));
        xassert_false(tmaxheap_map_break_if_true(heap, test_tmaxheap_apply_false, NULL));

        xassert(tmaxheap_map_break_if_true(heap, test_tmaxheap_find_str_true, "c"));
        xassert_false(tmaxheap_map_break_if_true(heap, test_tmaxheap_find_str_true, "x"));

        xassert(tmaxheap_size(heap) == 5);
        tmaxheap_free(&heap);
    }

    /* tmaxheap_map_break_if_false */
    {
        TMaxHeap_PT heap = tmaxheap_new(5, test_tmaxheap_cmp, NULL);
        xassert(tmaxheap_push(heap, "a"));
        xassert(tmaxheap_push(heap, "b"));
        xassert(tmaxheap_push(heap, "c"));
        xassert(tmaxheap_push(heap, "d"));
        xassert(tmaxheap_push(heap, "e"));
        xassert(tmaxheap_size(heap) == 5);
        xassert(tmaxheap_is_maxheap(heap));

        xassert(tmaxheap_map_break_if_false(heap, test_tmaxheap_apply_false, NULL));
        xassert_false(tmaxheap_map_break_if_false(heap, test_tmaxheap_apply_true, NULL));

        xassert(tmaxheap_map_break_if_false(heap, test_tmaxheap_find_str_false, "c"));
        xassert_false(tmaxheap_map_break_if_false(heap, test_tmaxheap_find_str_false, "x"));

        xassert(tmaxheap_size(heap) == 5);
        tmaxheap_free(&heap);
    }

    /* tmaxheap_free : testef already by other cases */

    /* tmaxheap_deep_free */
    {
        TMaxHeap_PT heap = tmaxheap_random_string(100);
        xassert(tmaxheap_is_maxheap(heap));
        tmaxheap_deep_free(&heap);
    }

    /* tmaxheap_clear */
    {
        TMaxHeap_PT heap = tmaxheap_new(5, test_tmaxheap_cmp, NULL);
        xassert(tmaxheap_push(heap, "a"));
        xassert(tmaxheap_push(heap, "b"));
        xassert(tmaxheap_push(heap, "c"));
        xassert(tmaxheap_push(heap, "d"));
        xassert(tmaxheap_push(heap, "e"));
        xassert(tmaxheap_size(heap) == 5);
        xassert(tmaxheap_is_maxheap(heap));

        tmaxheap_clear(heap);
        tmaxheap_deep_free(&heap);
    }

    /* tmaxheap_deep_clear */
    {
        TMaxHeap_PT heap = tmaxheap_random_string(100);
        tmaxheap_deep_clear(heap);
        tmaxheap_free(&heap);
    }

    /* tmaxheap_size */
    /* tmaxheap_is_empty */
    {
        TMaxHeap_PT heap = tmaxheap_new(5, test_tmaxheap_cmp, NULL);
        xassert(tmaxheap_size(heap) == 0);
        xassert(tmaxheap_is_empty(heap));

        xassert(tmaxheap_push(heap, "c"));
        xassert(tmaxheap_push(heap, "d"));
        xassert(tmaxheap_size(heap) == 2);
        xassert_false(tmaxheap_is_empty(heap));

        tmaxheap_free(&heap);
    }

    /* tmaxheap_is_maxheap */
    {
        /* tested already */
    }

    /* tmaxheap_sort */
    {
        TMaxHeap_PT heap = tmaxheap_random_string_no_limit(5000);
        tmaxheap_sort(heap);
        xassert(xdeque_is_sorted(heap->tree, test_tmaxheap_cmp_reverse, heap->cl));
        tmaxheap_deep_free(&heap);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
