
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../include/xalgos.h"

static
int test_xminpq_cmp(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static
bool test_xminpq_print(void *x, void *cl) {
    printf("%s\n", (char*)x);
    return true;
}

static
bool test_xminpq_find_str_true(void* ptr, void* cl) {
    if (strcmp(ptr, (char*)cl) == 0) {
        return true;
    }
    return false;
}

static
bool test_xminpq_find_str_false(void* ptr, void* cl) {
    if (strcmp(ptr, (char*)cl) == 0) {
        return false;
    }
    return true;
}

static
bool test_xminpq_apply_true(void *atom_str, void *cl) {
    return true;
}

static
bool test_xminpq_apply_false(void *atom_str, void *cl) {
    return false;
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static
XMinPQ_PT xminpq_random_string(int size) {
    XMinPQ_PT heap = xminpq_new(size, test_xminpq_cmp, NULL);
    if (!heap) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xminpq_deep_free(&heap);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            str[str_size - 1] = '\0';

            xminpq_push(heap, str);
        }
    }

    return heap;
}

void test_xminpq() {

    /* xminpq_new */
    {
        XMinPQ_PT heap = xminpq_new(5, test_xminpq_cmp, NULL);
        xassert(heap);
        xassert(xminpq_size(heap) == 0);
        xminpq_free(&heap);
    }

    /* xminpq_push */
    {
        XMinPQ_PT heap = xminpq_new(5, test_xminpq_cmp, NULL);
        xassert(xminpq_push(heap, "a"));
        xassert(xminpq_push(heap, "b"));
        xassert(xminpq_push(heap, "c"));
        xassert(xminpq_push(heap, "d"));
        xassert(xminpq_push(heap, "e"));
        xassert(xminpq_size(heap) == 5);

        xassert_false(xminpq_push(heap, "f"));
        xassert(xminpq_size(heap) == 5);
        xassert_false(xminpq_push(heap, "g"));
        xassert(xminpq_size(heap) == 5);
        xassert_false(xminpq_push(heap, "h"));
        xassert(xminpq_size(heap) == 5);

        //xminpq_map(heap, test_xminpq_print, NULL);
        xminpq_free(&heap);
    }

    /* xminpq_pop */
    {
        char* str = NULL;
        XMinPQ_PT heap = xminpq_new(5, test_xminpq_cmp, NULL);
        xassert(xminpq_push(heap, "b"));
        xassert(xminpq_push(heap, "a"));
        xassert(xminpq_push(heap, "d"));
        xassert(xminpq_push(heap, "e"));
        xassert(xminpq_push(heap, "c"));
        xassert(xminpq_size(heap) == 5);

        xassert(str = xminpq_pop(heap));
        xassert(strcmp(str, "a") == 0);
        xassert(xminpq_size(heap) == 4);
        xassert(str = xminpq_pop(heap));
        xassert(strcmp(str, "b") == 0);
        xassert(xminpq_size(heap) == 3);
        xassert(str = xminpq_pop(heap));
        xassert(strcmp(str, "c") == 0);
        xassert(xminpq_size(heap) == 2);
        xassert(str = xminpq_pop(heap));
        xassert(strcmp(str, "d") == 0);
        xassert(xminpq_size(heap) == 1);
        xassert(str = xminpq_pop(heap));
        xassert(strcmp(str, "e") == 0);
        xassert(xminpq_size(heap) == 0);

        xminpq_free(&heap);
    }

    /* xminpq_peek */
    {
        XMinPQ_PT heap = xminpq_new(5, test_xminpq_cmp, NULL);
        xassert(xminpq_push(heap, "a"));
        xassert(xminpq_push(heap, "b"));
        xassert(xminpq_push(heap, "c"));
        xassert(xminpq_push(heap, "d"));
        xassert(xminpq_push(heap, "e"));
        xassert(xminpq_size(heap) == 5);

        xassert(strcmp((char*)xminpq_peek(heap), "a") == 0);
        xassert(strcmp((char*)xminpq_peek(heap), "a") == 0);
        xassert(xminpq_size(heap) == 5);
        xminpq_free(&heap);
    }

    /* xminpq_map */
    {
        XMinPQ_PT heap = xminpq_new(5, test_xminpq_cmp, NULL);
        xassert(xminpq_push(heap, "a"));
        xassert(xminpq_push(heap, "e"));
        xassert(xminpq_push(heap, "c"));
        xassert(xminpq_push(heap, "b"));
        xassert(xminpq_push(heap, "d"));
        xassert(xminpq_size(heap) == 5);

        xassert(xminpq_map(heap, test_xminpq_apply_true, NULL) == 5);
        xassert(xminpq_map(heap, test_xminpq_apply_false, NULL) == 0);

        //xassert(xminpq_map(heap, test_xminpq_print, NULL) == 5);
        xassert(xminpq_size(heap) == 5);
        xminpq_free(&heap);
    }

    /* xminpq_map_break_if_true */
    {
        XMinPQ_PT heap = xminpq_new(5, test_xminpq_cmp, NULL);
        xassert(xminpq_push(heap, "a"));
        xassert(xminpq_push(heap, "b"));
        xassert(xminpq_push(heap, "c"));
        xassert(xminpq_push(heap, "d"));
        xassert(xminpq_push(heap, "e"));
        xassert(xminpq_size(heap) == 5);

        xassert(xminpq_map_break_if_true(heap, test_xminpq_apply_true, NULL));
        xassert_false(xminpq_map_break_if_true(heap, test_xminpq_apply_false, NULL));

        xassert(xminpq_map_break_if_true(heap, test_xminpq_find_str_true, "c"));
        xassert_false(xminpq_map_break_if_true(heap, test_xminpq_find_str_true, "x"));

        xassert(xminpq_size(heap) == 5);
        xminpq_free(&heap);
    }

    /* xminpq_map_break_if_false */
    {
        XMinPQ_PT heap = xminpq_new(5, test_xminpq_cmp, NULL);
        xassert(xminpq_push(heap, "a"));
        xassert(xminpq_push(heap, "b"));
        xassert(xminpq_push(heap, "c"));
        xassert(xminpq_push(heap, "d"));
        xassert(xminpq_push(heap, "e"));
        xassert(xminpq_size(heap) == 5);

        xassert(xminpq_map_break_if_false(heap, test_xminpq_apply_false, NULL));
        xassert_false(xminpq_map_break_if_false(heap, test_xminpq_apply_true, NULL));

        xassert(xminpq_map_break_if_false(heap, test_xminpq_find_str_false, "c"));
        xassert_false(xminpq_map_break_if_false(heap, test_xminpq_find_str_false, "x"));

        xassert(xminpq_size(heap) == 5);
        xminpq_free(&heap);
    }

    /* xminpq_free : testef already by other cases */

    /* xminpq_deep_free */
    {
        XMinPQ_PT heap = xminpq_random_string(100);
        xminpq_deep_free(&heap);
    }

    /* xminpq_size */
    /* xminpq_is_empty */
    {
        XMinPQ_PT heap = xminpq_new(5, test_xminpq_cmp, NULL);
        xassert(xminpq_size(heap) == 0);
        xassert(xminpq_is_empty(heap));

        xassert(xminpq_push(heap, "c"));
        xassert(xminpq_push(heap, "d"));
        xassert(xminpq_size(heap) == 2);
        xassert_false(xminpq_is_empty(heap));

        xminpq_free(&heap);
    }

    /* xminpq_is_heap */
    {
        XMinPQ_PT heap = xminpq_random_string(100);
        xminpq_deep_free(&heap);
    }

    /* xminpq_keep_max_values */
    {
        XMinPQ_PT heap = xminpq_new(5, test_xminpq_cmp, NULL);
        xminpq_keep_max_values(heap, "1", NULL);
        xminpq_keep_max_values(heap, "8", NULL);
        xminpq_keep_max_values(heap, "2", NULL);
        xminpq_keep_max_values(heap, "4", NULL);
        xminpq_keep_max_values(heap, "3", NULL);
        xminpq_keep_max_values(heap, "7", NULL);
        xminpq_keep_max_values(heap, "5", NULL);
        xminpq_keep_max_values(heap, "6", NULL);
        xminpq_keep_max_values(heap, "9", NULL);

        //xminpq_map(heap, test_xminpq_print, NULL);

        xminpq_free(&heap);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
