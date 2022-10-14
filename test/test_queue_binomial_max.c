
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../queue_binomial_min/xqueue_binomial_min_x.h"
#include "../queue_sequence/xqueue_sequence_x.h"
#include "../include/xalgos.h"

static
int test_xmaxbinque_cmp(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static
bool test_xmaxbinque_print(void *x, void *cl) {
    printf("%s\n", (char*)x);
    return true;
}

static
bool test_xmaxbinque_find_str_true(void* ptr, void* cl) {
    if (strcmp(ptr, (char*)cl) == 0) {
        return true;
    }
    return false;
}

static
bool test_xmaxbinque_find_str_false(void* ptr, void* cl) {
    if (strcmp(ptr, (char*)cl) == 0) {
        return false;
    }
    return true;
}

static
bool test_xmaxbinque_apply_true(void *atom_str, void *cl) {
    return true;
}

static
bool test_xmaxbinque_apply_false(void *atom_str, void *cl) {
    return false;
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static
XMaxBinQue_PT xmaxbinque_random_string(int size) {
    XMaxBinQue_PT queue = xmaxbinque_new(size, test_xmaxbinque_cmp, NULL);
    if (!queue) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xmaxbinque_deep_free(&queue);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            str[str_size - 1] = '\0';

            xmaxbinque_push(queue, str);
        }
    }

    return queue;
}

void test_xmaxbinque() {

    /* xmaxbinque_new */
    {
        XMaxBinQue_PT queue = xmaxbinque_new(5, test_xmaxbinque_cmp, NULL);
        xassert(queue->buckets);
        xassert(queue->size == 0);
        xassert(queue->capacity == 5);
        xassert(queue->cmp == test_xmaxbinque_cmp);
        xmaxbinque_free(&queue);
    }

    /* xmaxbinque_copy */
    /* xmaxbinque_deep_copy */
    {
        XMaxBinQue_PT queue = xmaxbinque_random_string(5000);
        XMaxBinQue_PT queue2 = xmaxbinque_deep_copy(queue, 5);
        xmaxbinque_deep_free(&queue);
        xmaxbinque_deep_free(&queue2);
    }

    /* xmaxbinque_push */
    {
        XMaxBinQue_PT queue = xmaxbinque_new(8, test_xmaxbinque_cmp, NULL);
        xassert(xmaxbinque_push(queue, "a"));
        xassert(queue->size == 1);
        xassert(queue->buckets->size == 1);

        xassert(xmaxbinque_push(queue, "b"));
        xassert(queue->size == 2);
        xassert(queue->buckets->size == 2);

        xassert(xmaxbinque_push(queue, "c"));
        xassert(queue->size == 3);
        xassert(queue->buckets->size == 2);

        xassert(xmaxbinque_push(queue, "d"));
        xassert(queue->size == 4);
        xassert(queue->buckets->size == 3);

        xassert(xmaxbinque_push(queue, "e"));
        xassert(queue->size == 5);
        xassert(queue->buckets->size == 3);

        xassert(xmaxbinque_push(queue, "f"));
        xassert(queue->size == 6);
        xassert(queue->buckets->size == 3);

        xassert(xmaxbinque_push(queue, "g"));
        xassert(queue->size == 7);
        xassert(queue->buckets->size == 3);

        xassert(xmaxbinque_push(queue, "h"));
        xassert(queue->size == 8);
        xassert(queue->buckets->size == 4);

        xassert_false(xmaxbinque_push(queue, "i"));
        xassert(queue->size == 8);
        xassert(queue->buckets->size == 4);

        xmaxbinque_set_strategy_discard_top(queue);

        xassert(xmaxbinque_push(queue, "a"));
        xassert(queue->size == 8);
        xassert(queue->buckets->size == 4);
        xassert(strcmp((char*)xmaxbinque_peek(queue), "g") == 0);

        xassert(xmaxbinque_push(queue, "b"));
        xassert(queue->size == 8);
        xassert(queue->buckets->size == 4);
        xassert(strcmp((char*)xmaxbinque_peek(queue), "f") == 0);

        //xmaxbinque_map(queue, test_xmaxbinque_print, NULL);
        xmaxbinque_free(&queue);
    }

    /* xmaxbinque_pop */
    {
        char* str1 = NULL;
        char* str2 = NULL;
        XMaxBinQue_PT queue = xmaxbinque_random_string(5000);
        for (int i = 1; i <= 2500; ++i) {
            xassert(str1 = xmaxbinque_pop(queue));
            xassert(str2 = xmaxbinque_pop(queue));
            xassert(strcmp(str2, str1) <= 0);
            xassert(queue->size == 5000 - 2 * i);
            XMEM_FREE(str1);
            XMEM_FREE(str2);
        }

        xassert(xpseq_size(queue->buckets) == 0);
        xmaxbinque_free(&queue);
    }

    /* xmaxbinque_peek */
    {
        XMaxBinQue_PT queue = xmaxbinque_new(5, test_xmaxbinque_cmp, NULL);
        xassert(xmaxbinque_push(queue, "a"));
        xassert(xmaxbinque_push(queue, "b"));
        xassert(xmaxbinque_push(queue, "c"));
        xassert(xmaxbinque_push(queue, "d"));
        xassert(xmaxbinque_push(queue, "e"));
        xassert(xmaxbinque_size(queue) == 5);

        xassert(strcmp((char*)xmaxbinque_peek(queue), "e") == 0);
        xassert(strcmp((char*)xmaxbinque_peek(queue), "e") == 0);
        xassert(xmaxbinque_size(queue) == 5);
        xmaxbinque_free(&queue);
    }

    /* xmaxbinque_merge */
    {
        XMaxBinQue_PT queue1 = xmaxbinque_random_string(5000);
        XMaxBinQue_PT queue2 = xmaxbinque_random_string(5000);

        xmaxbinque_merge(queue1, &queue2);
        xassert_false(queue2);
        xassert(queue1->size == 10000);

        char* str1 = NULL;
        char* str2 = NULL;
        for (int i = 1; i <= 5000; ++i) {
            xassert(str1 = xmaxbinque_pop(queue1));
            xassert(str2 = xmaxbinque_pop(queue1));
            xassert(strcmp(str2, str1) <= 0);
            xassert(queue1->size == 10000 - 2 * i);
            XMEM_FREE(str1);
            XMEM_FREE(str2);
        }

        xassert(xpseq_size(queue1->buckets) == 0);
        xmaxbinque_free(&queue1);
    }

    /* xmaxbinque_map */
    {
        XMaxBinQue_PT queue = xmaxbinque_new(5, test_xmaxbinque_cmp, NULL);
        xassert(xmaxbinque_push(queue, "a"));
        xassert(xmaxbinque_push(queue, "e"));
        xassert(xmaxbinque_push(queue, "c"));
        xassert(xmaxbinque_push(queue, "b"));
        xassert(xmaxbinque_push(queue, "d"));
        xassert(xmaxbinque_size(queue) == 5);

        xassert(xmaxbinque_map(queue, test_xmaxbinque_apply_true, NULL) == 5);
        xassert(xmaxbinque_map(queue, test_xmaxbinque_apply_false, NULL) == 0);

        //xassert(xmaxbinque_map(queue, test_xmaxbinque_print, NULL) == 5);
        xassert(xmaxbinque_size(queue) == 5);
        xmaxbinque_free(&queue);
    }

    /* xmaxbinque_map_break_if_true */
    {
        XMaxBinQue_PT queue = xmaxbinque_new(5, test_xmaxbinque_cmp, NULL);
        xassert(xmaxbinque_push(queue, "a"));
        xassert(xmaxbinque_push(queue, "b"));
        xassert(xmaxbinque_push(queue, "c"));
        xassert(xmaxbinque_push(queue, "d"));
        xassert(xmaxbinque_push(queue, "e"));
        xassert(xmaxbinque_size(queue) == 5);

        xassert(xmaxbinque_map_break_if_true(queue, test_xmaxbinque_apply_true, NULL));
        xassert_false(xmaxbinque_map_break_if_true(queue, test_xmaxbinque_apply_false, NULL));

        xassert(xmaxbinque_map_break_if_true(queue, test_xmaxbinque_find_str_true, "c"));
        xassert_false(xmaxbinque_map_break_if_true(queue, test_xmaxbinque_find_str_true, "x"));

        xassert(xmaxbinque_size(queue) == 5);
        xmaxbinque_free(&queue);
    }

    /* xmaxbinque_map_break_if_false */
    {
        XMaxBinQue_PT queue = xmaxbinque_new(5, test_xmaxbinque_cmp, NULL);
        xassert(xmaxbinque_push(queue, "a"));
        xassert(xmaxbinque_push(queue, "b"));
        xassert(xmaxbinque_push(queue, "c"));
        xassert(xmaxbinque_push(queue, "d"));
        xassert(xmaxbinque_push(queue, "e"));
        xassert(xmaxbinque_size(queue) == 5);

        xassert(xmaxbinque_map_break_if_false(queue, test_xmaxbinque_apply_false, NULL));
        xassert_false(xmaxbinque_map_break_if_false(queue, test_xmaxbinque_apply_true, NULL));

        xassert(xmaxbinque_map_break_if_false(queue, test_xmaxbinque_find_str_false, "c"));
        xassert_false(xmaxbinque_map_break_if_false(queue, test_xmaxbinque_find_str_false, "x"));

        xassert(xmaxbinque_size(queue) == 5);
        xmaxbinque_free(&queue);
    }

    /* xmaxbinque_free : testef already by other cases */

    /* xmaxbinque_deep_free */
    {
        XMaxBinQue_PT queue = xmaxbinque_random_string(100);
        xmaxbinque_deep_free(&queue);
    }

    /* xmaxbinque_clear */
    /* xmaxbinque_deep_clear */
    {
        XMaxBinQue_PT queue = xmaxbinque_random_string(100);
        xmaxbinque_deep_clear(queue);
        xassert(queue->size == 0);
        xassert(queue->buckets->size == 0);
        xmaxbinque_free(&queue);
    }

    /* xmaxbinque_size */
    /* xmaxbinque_is_empty */
    {
        XMaxBinQue_PT queue = xmaxbinque_new(5, test_xmaxbinque_cmp, NULL);
        xassert(xmaxbinque_size(queue) == 0);
        xassert(xmaxbinque_is_empty(queue));

        xassert(xmaxbinque_push(queue, "c"));
        xassert(xmaxbinque_push(queue, "d"));
        xassert(xmaxbinque_size(queue) == 2);
        xassert_false(xmaxbinque_is_empty(queue));

        xmaxbinque_free(&queue);
    }

    /* xmaxbinque_keep_max_values */
    {
        XMaxBinQue_PT queue = xmaxbinque_new(7, test_xmaxbinque_cmp, NULL);
        xmaxbinque_keep_min_values(queue, "1", NULL);
        xmaxbinque_keep_min_values(queue, "8", NULL);
        xmaxbinque_keep_min_values(queue, "2", NULL);
        xmaxbinque_keep_min_values(queue, "4", NULL);
        xmaxbinque_keep_min_values(queue, "3", NULL);
        xmaxbinque_keep_min_values(queue, "7", NULL);
        xmaxbinque_keep_min_values(queue, "5", NULL);
        xmaxbinque_keep_min_values(queue, "6", NULL);
        xmaxbinque_keep_min_values(queue, "9", NULL);

        xassert(strcmp((char*)xmaxbinque_peek(queue), "1") != 0);
        xassert(strcmp((char*)xmaxbinque_peek(queue), "2") != 0);
        //xmaxbinque_map(queue, test_xmaxbinque_print, NULL);

        xmaxbinque_free(&queue);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
