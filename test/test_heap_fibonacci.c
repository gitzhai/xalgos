
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/xutils.h"
#include "../include/xalgos.h"

static
int test_xfibheap_cmp(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static
int test_xfibheap_cmp_reverse(void *x, void *y, void *cl) {
    return strcmp((char*)y, (char*)x);
}

static
bool test_xfibheap_print(void *x, void *cl) {
    printf("%s\n", (char*)x);
    return true;
}

static
bool test_xfibheap_find_str_true(void* ptr, void* cl) {
    if (strcmp(ptr, (char*)cl) == 0) {
        return true;
    }
    return false;
}

static
bool test_xfibheap_find_str_false(void* ptr, void* cl) {
    if (strcmp(ptr, (char*)cl) == 0) {
        return false;
    }
    return true;
}

static
bool test_xfibheap_apply_true(void *atom_str, void *cl) {
    return true;
}

static
bool test_xfibheap_apply_false(void *atom_str, void *cl) {
    return false;
}


static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static
XFibHeap_PT xfibheap_random_string(int size) {
    XFibHeap_PT heap = xfibheap_new(test_xfibheap_cmp, NULL);
    if (!heap) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xfibheap_deep_free(&heap);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            str[str_size - 1] = '\0';

            xfibheap_push(heap, str);
        }
    }

    return heap;
}

void test_xfibheap() {

    /* xfibheap_new */
    {
        XFibHeap_PT heap = xfibheap_new(test_xfibheap_cmp, NULL);
        xassert(heap);
        xassert(xfibheap_size(heap) == 0);
        xfibheap_free(&heap);
    }

    /* xfibheap_push */
    {
        XFibHeap_PT heap = xfibheap_new(test_xfibheap_cmp, NULL);
        xassert(xfibheap_push(heap, "a"));
        xassert(xfibheap_push(heap, "b"));
        xassert(xfibheap_push(heap, "c"));
        xassert(xfibheap_push(heap, "d"));
        xassert(xfibheap_push(heap, "e"));
        xassert(xfibheap_push(heap, "a"));
        xassert(xfibheap_push(heap, "b"));
        xassert(xfibheap_push(heap, "c"));
        xassert(xfibheap_push(heap, "d"));
        xassert(xfibheap_push(heap, "e"));
        xassert(xfibheap_size(heap) == 10);
        xassert(xfibheap_is_fibheap(heap));

        xfibheap_free(&heap);
    }

    /* xfibheap_pop */
    {
        {
            char* str = NULL;
            XFibHeap_PT heap = xfibheap_new(test_xfibheap_cmp, NULL);
            xassert(xfibheap_push(heap, "a"));
            xassert(xfibheap_push(heap, "b"));
            xassert(xfibheap_push(heap, "c"));
            xassert(xfibheap_push(heap, "d"));
            xassert(xfibheap_push(heap, "e"));
            xassert(xfibheap_size(heap) == 5);
            xassert(xfibheap_is_fibheap(heap));

            xassert(str = xfibheap_pop(heap));
            xassert(strcmp(str, "a") == 0);
            xassert(xfibheap_size(heap) == 4);
            xassert(xfibheap_is_fibheap(heap));

            xassert(str = xfibheap_pop(heap));
            xassert(strcmp(str, "b") == 0);
            xassert(xfibheap_size(heap) == 3);
            xassert(xfibheap_is_fibheap(heap));

            xassert(str = xfibheap_pop(heap));
            xassert(strcmp(str, "c") == 0);
            xassert(xfibheap_size(heap) == 2);
            xassert(xfibheap_is_fibheap(heap));

            xassert(str = xfibheap_pop(heap));
            xassert(strcmp(str, "d") == 0);
            xassert(xfibheap_size(heap) == 1);
            xassert(xfibheap_is_fibheap(heap));

            xassert(str = xfibheap_pop(heap));
            xassert(strcmp(str, "e") == 0);
            xassert(xfibheap_size(heap) == 0);

            xfibheap_free(&heap);
        }

        {
            char* str = NULL;
            XFibHeap_PT heap = xfibheap_new(test_xfibheap_cmp, NULL);
            xassert(xfibheap_push(heap, "a"));
            xassert(xfibheap_push(heap, "e"));
            xassert(xfibheap_push(heap, "d"));
            xassert(xfibheap_push(heap, "c"));
            xassert(xfibheap_push(heap, "b"));
            xassert(xfibheap_size(heap) == 5);
            xassert(xfibheap_is_fibheap(heap));

            xassert(str = xfibheap_pop(heap));
            xassert(strcmp(str, "a") == 0);
            xassert(xfibheap_size(heap) == 4);
            xassert(xfibheap_is_fibheap(heap));

            xfibheap_free(&heap);
        }

        {
            char* str = NULL;
            XFibHeap_PT heap = xfibheap_new(test_xfibheap_cmp, NULL);
            xassert(xfibheap_push(heap, "a"));
            xassert(xfibheap_push(heap, "b"));
            xassert(xfibheap_push(heap, "c"));
            xassert(xfibheap_push(heap, "d"));
            xassert(xfibheap_push(heap, "e"));
            xassert(xfibheap_push(heap, "f"));
            xassert(xfibheap_push(heap, "g"));
            xassert(xfibheap_push(heap, "h"));
            xassert(xfibheap_push(heap, "i"));
            xassert(xfibheap_push(heap, "j"));
            xassert(xfibheap_push(heap, "a"));
            xassert(xfibheap_push(heap, "b"));
            xassert(xfibheap_push(heap, "c"));
            xassert(xfibheap_push(heap, "d"));
            xassert(xfibheap_push(heap, "e"));
            xassert(xfibheap_push(heap, "f"));
            xassert(xfibheap_push(heap, "g"));
            xassert(xfibheap_push(heap, "h"));
            xassert(xfibheap_push(heap, "i"));
            xassert(xfibheap_push(heap, "j"));
            xassert(xfibheap_push(heap, "a"));
            xassert(xfibheap_push(heap, "b"));
            xassert(xfibheap_push(heap, "c"));
            xassert(xfibheap_push(heap, "d"));
            xassert(xfibheap_push(heap, "e"));
            xassert(xfibheap_push(heap, "f"));
            xassert(xfibheap_push(heap, "g"));
            xassert(xfibheap_push(heap, "h"));
            xassert(xfibheap_push(heap, "i"));
            xassert(xfibheap_push(heap, "j"));
            xassert(xfibheap_push(heap, "a"));
            xassert(xfibheap_push(heap, "b"));
            xassert(xfibheap_push(heap, "c"));
            xassert(xfibheap_push(heap, "d"));
            xassert(xfibheap_push(heap, "e"));
            xassert(xfibheap_push(heap, "f"));
            xassert(xfibheap_push(heap, "g"));
            xassert(xfibheap_push(heap, "h"));
            xassert(xfibheap_push(heap, "i"));
            xassert(xfibheap_push(heap, "j"));
            xassert(xfibheap_push(heap, "a"));
            xassert(xfibheap_push(heap, "b"));
            xassert(xfibheap_push(heap, "c"));
            xassert(xfibheap_push(heap, "d"));
            xassert(xfibheap_push(heap, "e"));
            xassert(xfibheap_push(heap, "f"));
            xassert(xfibheap_push(heap, "g"));
            xassert(xfibheap_push(heap, "h"));
            xassert(xfibheap_push(heap, "i"));
            xassert(xfibheap_push(heap, "j"));
            xassert(xfibheap_push(heap, "a"));
            xassert(xfibheap_push(heap, "b"));
            xassert(xfibheap_push(heap, "c"));
            xassert(xfibheap_push(heap, "d"));
            xassert(xfibheap_push(heap, "e"));
            xassert(xfibheap_push(heap, "f"));
            xassert(xfibheap_push(heap, "g"));
            xassert(xfibheap_push(heap, "h"));
            xassert(xfibheap_push(heap, "i"));
            xassert(xfibheap_push(heap, "j"));
            xassert(xfibheap_size(heap) == 60);
            xassert(xfibheap_is_fibheap(heap));

            xassert(str = xfibheap_pop(heap));
            xassert(xfibheap_is_fibheap(heap));
            xassert(str = xfibheap_pop(heap));
            xassert(xfibheap_is_fibheap(heap));
            xassert(str = xfibheap_pop(heap));
            xassert(xfibheap_is_fibheap(heap));
            xassert(str = xfibheap_pop(heap));
            xassert(xfibheap_is_fibheap(heap));
            xassert(str = xfibheap_pop(heap));
            xassert(xfibheap_is_fibheap(heap));
            xassert(str = xfibheap_pop(heap));
            xassert(xfibheap_is_fibheap(heap));
            xassert(str = xfibheap_pop(heap));
            xassert(xfibheap_is_fibheap(heap));
            xassert(str = xfibheap_pop(heap));
            xassert(xfibheap_is_fibheap(heap));
            xassert(str = xfibheap_pop(heap));
            xassert(xfibheap_is_fibheap(heap));
            xassert(str = xfibheap_pop(heap));
            xassert(xfibheap_is_fibheap(heap));
            xassert(xfibheap_size(heap) == 50);
            xfibheap_free(&heap);
        }

        {
            XFibHeap_PT heap = xfibheap_random_string(2000);

            void *data = NULL;
            int count = 0;
            while (!xfibheap_is_empty(heap)) {
                ++count;
                xassert(data = xfibheap_pop(heap));
                XMEM_FREE(data);
                if (count % 500 == 0) {
                    xassert(xfibheap_is_fibheap(heap));
                }
            }
            xassert(xfibheap_size(heap) == 0);
            xassert(count == 2000);
            xfibheap_free(&heap);
        }
    }

    /* xfibheap_peek */
    {
        XFibHeap_PT heap = xfibheap_new(test_xfibheap_cmp, NULL);
        xassert(xfibheap_push(heap, "a"));
        xassert(xfibheap_push(heap, "b"));
        xassert(xfibheap_push(heap, "c"));
        xassert(xfibheap_push(heap, "d"));
        xassert(xfibheap_push(heap, "e"));
        xassert(xfibheap_size(heap) == 5);
        xassert(xfibheap_is_fibheap(heap));

        xassert(strcmp((char*)xfibheap_peek(heap), "a") == 0);
        xassert(strcmp((char*)xfibheap_peek(heap), "a") == 0);
        xassert(xfibheap_size(heap) == 5);
        xfibheap_free(&heap);
    }

    /* xfibheap_map */
    {
        XFibHeap_PT heap = xfibheap_new(test_xfibheap_cmp, NULL);
        xassert(xfibheap_push(heap, "a"));
        xassert(xfibheap_push(heap, "e"));
        xassert(xfibheap_push(heap, "c"));
        xassert(xfibheap_push(heap, "b"));
        xassert(xfibheap_push(heap, "d"));
        xassert(xfibheap_size(heap) == 5);
        xassert(xfibheap_is_fibheap(heap));

        xassert(xfibheap_map(heap, test_xfibheap_apply_true, NULL) == 5);
        xassert(xfibheap_map(heap, test_xfibheap_apply_false, NULL) == 0);

        //xassert(xfibheap_map(heap, test_xfibheap_print, NULL) == 5);
        xassert(xfibheap_size(heap) == 5);
        xfibheap_free(&heap);
    }

    /* xfibheap_map_break_if_true */
    {
        XFibHeap_PT heap = xfibheap_new(test_xfibheap_cmp, NULL);
        xassert(xfibheap_push(heap, "a"));
        xassert(xfibheap_push(heap, "b"));
        xassert(xfibheap_push(heap, "c"));
        xassert(xfibheap_push(heap, "d"));
        xassert(xfibheap_push(heap, "e"));
        xassert(xfibheap_size(heap) == 5);
        xassert(xfibheap_is_fibheap(heap));

        xassert(xfibheap_map_break_if_true(heap, test_xfibheap_apply_true, NULL));
        xassert_false(xfibheap_map_break_if_true(heap, test_xfibheap_apply_false, NULL));

        xassert(xfibheap_map_break_if_true(heap, test_xfibheap_find_str_true, "c"));
        xassert_false(xfibheap_map_break_if_true(heap, test_xfibheap_find_str_true, "x"));

        xassert(xfibheap_size(heap) == 5);
        xfibheap_free(&heap);
    }

    /* xfibheap_map_break_if_false */
    {
        XFibHeap_PT heap = xfibheap_new(test_xfibheap_cmp, NULL);
        xassert(xfibheap_push(heap, "a"));
        xassert(xfibheap_push(heap, "b"));
        xassert(xfibheap_push(heap, "c"));
        xassert(xfibheap_push(heap, "d"));
        xassert(xfibheap_push(heap, "e"));
        xassert(xfibheap_size(heap) == 5);
        xassert(xfibheap_is_fibheap(heap));

        xassert(xfibheap_map_break_if_false(heap, test_xfibheap_apply_false, NULL));
        xassert_false(xfibheap_map_break_if_false(heap, test_xfibheap_apply_true, NULL));

        xassert(xfibheap_map_break_if_false(heap, test_xfibheap_find_str_false, "c"));
        xassert_false(xfibheap_map_break_if_false(heap, test_xfibheap_find_str_false, "x"));

        xassert(xfibheap_size(heap) == 5);
        xfibheap_free(&heap);
    }

    /* xfibheap_free : testef already by other cases */

    /* xfibheap_deep_free */
    {
        XFibHeap_PT heap = xfibheap_random_string(100);
        xassert(xfibheap_is_fibheap(heap));
        xfibheap_deep_free(&heap);
    }

    /* xfibheap_clear */
    {
        XFibHeap_PT heap = xfibheap_new(test_xfibheap_cmp, NULL);
        xassert(xfibheap_push(heap, "a"));
        xassert(xfibheap_push(heap, "b"));
        xassert(xfibheap_push(heap, "c"));
        xassert(xfibheap_push(heap, "d"));
        xassert(xfibheap_push(heap, "e"));
        xassert(xfibheap_size(heap) == 5);
        xassert(xfibheap_is_fibheap(heap));

        xfibheap_clear(heap);
        xfibheap_deep_free(&heap);
    }

    /* xfibheap_deep_clear */
    {
        XFibHeap_PT heap = xfibheap_random_string(100);
        xfibheap_deep_clear(heap);
        xfibheap_free(&heap);
    }

    /* xfibheap_size */
    /* xfibheap_is_empty */
    {
        XFibHeap_PT heap = xfibheap_new(test_xfibheap_cmp, NULL);
        xassert(xfibheap_size(heap) == 0);
        xassert(xfibheap_is_empty(heap));

        xassert(xfibheap_push(heap, "c"));
        xassert(xfibheap_push(heap, "d"));
        xassert(xfibheap_size(heap) == 2);
        xassert_false(xfibheap_is_empty(heap));

        xfibheap_free(&heap);
    }

    /* xfibheap_is_fibheap */
    {
        /* tested already */
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
