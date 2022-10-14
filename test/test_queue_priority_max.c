
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../include/xalgos.h"

static
int test_xmaxpq_cmp(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static
bool test_xmaxpq_print(void *x, void *cl) {
    printf("%s\n", (char*)x);
    return true;
}

static
bool test_xmaxpq_find_str_true(void* ptr, void* cl) {
    if (strcmp(ptr, (char*)cl) == 0) {
        return true;
    }
    return false;
}

static
bool test_xmaxpq_find_str_false(void* ptr, void* cl) {
    if (strcmp(ptr, (char*)cl) == 0) {
        return false;
    }
    return true;
}

static
bool test_xmaxpq_apply_true(void *atom_str, void *cl) {
    return true;
}

static
bool test_xmaxpq_apply_false(void *atom_str, void *cl) {
    return false;
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static 
XMaxPQ_PT xmaxpq_random_string(int size) {
    XMaxPQ_PT heap = xmaxpq_new(size, test_xmaxpq_cmp, NULL);
    if (!heap) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xmaxpq_deep_free(&heap);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            str[str_size - 1] = '\0';

            xmaxpq_push(heap, str);
        }
    }

    return heap;
}

void test_xmaxpq() {

    /* xmaxpq_new */
    {
        XMaxPQ_PT heap = xmaxpq_new(5, test_xmaxpq_cmp, NULL);
        xassert(heap);
        xassert(xmaxpq_size(heap) == 0);
        xmaxpq_free(&heap);
    }

    /* xmaxpq_push */
    {
        XMaxPQ_PT heap = xmaxpq_new(5, test_xmaxpq_cmp, NULL);
        xassert(xmaxpq_push(heap, "a"));
        xassert(xmaxpq_push(heap, "b"));
        xassert(xmaxpq_push(heap, "c"));
        xassert(xmaxpq_push(heap, "d"));
        xassert(xmaxpq_push(heap, "e"));
        xassert(xmaxpq_size(heap) == 5);

        xassert_false(xmaxpq_push(heap, "f"));
        xassert(xmaxpq_size(heap) == 5);
        xassert_false(xmaxpq_push(heap, "g"));
        xassert(xmaxpq_size(heap) == 5);
        xassert_false(xmaxpq_push(heap, "h"));
        xassert(xmaxpq_size(heap) == 5);

        //xmaxpq_map(heap, test_xmaxpq_print, NULL);
        xmaxpq_free(&heap);
    }

    /* xmaxpq_pop */
    {
        char* str = NULL;
        XMaxPQ_PT heap = xmaxpq_new(5, test_xmaxpq_cmp, NULL);
        xassert(xmaxpq_push(heap, "c"));
        xassert(xmaxpq_push(heap, "a"));
        xassert(xmaxpq_push(heap, "e"));
        xassert(xmaxpq_push(heap, "d"));
        xassert(xmaxpq_push(heap, "b"));
        xassert(xmaxpq_size(heap) == 5);

        xassert(str = xmaxpq_pop(heap));
        xassert(strcmp(str, "e") == 0);
        xassert(xmaxpq_size(heap) == 4);
        xassert(str = xmaxpq_pop(heap));
        xassert(strcmp(str, "d") == 0);
        xassert(xmaxpq_size(heap) == 3);
        xassert(str = xmaxpq_pop(heap));
        xassert(strcmp(str, "c") == 0);
        xassert(xmaxpq_size(heap) == 2);
        xassert(str = xmaxpq_pop(heap));
        xassert(strcmp(str, "b") == 0);
        xassert(xmaxpq_size(heap) == 1);
        xassert(str = xmaxpq_pop(heap));
        xassert(strcmp(str, "a") == 0);
        xassert(xmaxpq_size(heap) == 0);

        xmaxpq_free(&heap);
    }

    /* xmaxpq_peek */
    {
        XMaxPQ_PT heap = xmaxpq_new(5, test_xmaxpq_cmp, NULL);
        xassert(xmaxpq_push(heap, "a"));
        xassert(xmaxpq_push(heap, "b"));
        xassert(xmaxpq_push(heap, "c"));
        xassert(xmaxpq_push(heap, "d"));
        xassert(xmaxpq_push(heap, "e"));
        xassert(xmaxpq_size(heap) == 5);

        xassert(strcmp((char*)xmaxpq_peek(heap), "e") == 0);
        xassert(strcmp((char*)xmaxpq_peek(heap), "e") == 0);
        xassert(xmaxpq_size(heap) == 5);
        xmaxpq_free(&heap);
    }

    /* xmaxpq_map */
    {
        XMaxPQ_PT heap = xmaxpq_new(5, test_xmaxpq_cmp, NULL);
        xassert(xmaxpq_push(heap, "a"));
        xassert(xmaxpq_push(heap, "b"));
        xassert(xmaxpq_push(heap, "c"));
        xassert(xmaxpq_push(heap, "d"));
        xassert(xmaxpq_push(heap, "e"));
        xassert(xmaxpq_size(heap) == 5);

        xassert(xmaxpq_map(heap, test_xmaxpq_apply_true, NULL) == 5);
        xassert(xmaxpq_map(heap, test_xmaxpq_apply_false, NULL) == 0);

        //xassert(xmaxpq_map(heap, test_xmaxpq_print, NULL) == 5);
        xassert(xmaxpq_size(heap) == 5);
        xmaxpq_free(&heap);
    }

    /* xmaxpq_map_break_if_true */
    {
        XMaxPQ_PT heap = xmaxpq_new(5, test_xmaxpq_cmp, NULL);
        xassert(xmaxpq_push(heap, "a"));
        xassert(xmaxpq_push(heap, "b"));
        xassert(xmaxpq_push(heap, "c"));
        xassert(xmaxpq_push(heap, "d"));
        xassert(xmaxpq_push(heap, "e"));
        xassert(xmaxpq_size(heap) == 5);

        xassert(xmaxpq_map_break_if_true(heap, test_xmaxpq_apply_true, NULL));
        xassert_false(xmaxpq_map_break_if_true(heap, test_xmaxpq_apply_false, NULL));

        xassert(xmaxpq_map_break_if_true(heap, test_xmaxpq_find_str_true, "c"));
        xassert_false(xmaxpq_map_break_if_true(heap, test_xmaxpq_find_str_true, "x"));

        xassert(xmaxpq_size(heap) == 5);
        xmaxpq_free(&heap);
    }

    /* xmaxpq_map_break_if_false */
    {
        XMaxPQ_PT heap = xmaxpq_new(5, test_xmaxpq_cmp, NULL);
        xassert(xmaxpq_push(heap, "a"));
        xassert(xmaxpq_push(heap, "b"));
        xassert(xmaxpq_push(heap, "c"));
        xassert(xmaxpq_push(heap, "d"));
        xassert(xmaxpq_push(heap, "e"));
        xassert(xmaxpq_size(heap) == 5);

        xassert(xmaxpq_map_break_if_false(heap, test_xmaxpq_apply_false, NULL));
        xassert_false(xmaxpq_map_break_if_false(heap, test_xmaxpq_apply_true, NULL));

        xassert(xmaxpq_map_break_if_false(heap, test_xmaxpq_find_str_false, "c"));
        xassert_false(xmaxpq_map_break_if_false(heap, test_xmaxpq_find_str_false, "x"));

        xassert(xmaxpq_size(heap) == 5);
        xmaxpq_free(&heap);
    }

    /* xmaxpq_free : testef already by other cases */

    /* xmaxpq_deep_free */
    {
        XMaxPQ_PT heap = xmaxpq_random_string(100);
        xmaxpq_deep_free(&heap);
    }

    /* xmaxpq_size */
    /* xmaxpq_is_empty */
    {
        XMaxPQ_PT heap = xmaxpq_new(5, test_xmaxpq_cmp, NULL);
        xassert(xmaxpq_size(heap) == 0);
        xassert(xmaxpq_is_empty(heap));

        xassert(xmaxpq_push(heap, "c"));
        xassert(xmaxpq_push(heap, "d"));
        xassert(xmaxpq_size(heap) == 2);
        xassert_false(xmaxpq_is_empty(heap));

        xmaxpq_free(&heap);
    }

    /* xmaxpq_keep_min_values */
    {
        XMaxPQ_PT heap = xmaxpq_new(5, test_xmaxpq_cmp, NULL);
        xmaxpq_keep_min_values(heap, "1", NULL);
        xmaxpq_keep_min_values(heap, "8", NULL);
        xmaxpq_keep_min_values(heap, "2", NULL);
        xmaxpq_keep_min_values(heap, "4", NULL);
        xmaxpq_keep_min_values(heap, "3", NULL);
        xmaxpq_keep_min_values(heap, "7", NULL);
        xmaxpq_keep_min_values(heap, "5", NULL);
        xmaxpq_keep_min_values(heap, "6", NULL);
        xmaxpq_keep_min_values(heap, "9", NULL);

        //xmaxpq_map(heap, test_xmaxpq_print, NULL);

        xmaxpq_free(&heap);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
