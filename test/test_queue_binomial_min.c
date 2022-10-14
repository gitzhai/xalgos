
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../queue_binomial_min/xqueue_binomial_min_x.h"
#include "../queue_sequence/xqueue_sequence_x.h"
#include "../include/xalgos.h"

static
int test_xminbinque_cmp(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static
bool test_xminbinque_print(void *x, void *cl) {
    printf("%s\n", (char*)x);
    return true;
}

static
bool test_xminbinque_find_str_true(void* ptr, void* cl) {
    if (strcmp(ptr, (char*)cl) == 0) {
        return true;
    }
    return false;
}

static
bool test_xminbinque_find_str_false(void* ptr, void* cl) {
    if (strcmp(ptr, (char*)cl) == 0) {
        return false;
    }
    return true;
}

static
bool test_xminbinque_apply_true(void *atom_str, void *cl) {
    return true;
}

static
bool test_xminbinque_apply_false(void *atom_str, void *cl) {
    return false;
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static 
XMinBinQue_PT xminbinque_random_string(int size) {
    XMinBinQue_PT queue = xminbinque_new(size, test_xminbinque_cmp, NULL);
    if (!queue) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xminbinque_deep_free(&queue);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            str[str_size - 1] = '\0';

            xminbinque_push(queue, str);
        }
    }

    return queue;
}

static
int xminbinque_pow2(int n) {
    int k = 1;
    for (; 1 <= n; --n) {
        k <<= 1;
    }
    return k;
}

void test_xminbinque() {

    /* xminbinque_pow2 */
    {
        xassert(xminbinque_pow2(0) == 1);
        xassert(xminbinque_pow2(1) == 2);
        xassert(xminbinque_pow2(2) == 4);
        xassert(xminbinque_pow2(3) == 8);
        xassert(xminbinque_pow2(4) == 16);
        xassert(xminbinque_pow2(5) == 32);
        xassert(xminbinque_pow2(6) == 64);
        xassert(xminbinque_pow2(7) == 128);
        xassert(xminbinque_pow2(8) == 256);
        xassert(xminbinque_pow2(9) == 512);
        xassert(xminbinque_pow2(10) == 1024);
        xassert(xminbinque_pow2(11) == 2048);
        xassert(xminbinque_pow2(12) == 4096);
        xassert(xminbinque_pow2(13) == 8192);
    }

    /* xminbinque_new */
    {
        XMinBinQue_PT queue = xminbinque_new(5, test_xminbinque_cmp, NULL);
        xassert(queue->buckets);
        xassert(queue->size == 0);
        xassert(queue->capacity == 5);
        xassert(queue->cmp == test_xminbinque_cmp);
        xminbinque_free(&queue);
    }

    /* xminbinque_copy */
    /* xminbinque_deep_copy */
    {
        XMinBinQue_PT queue = xminbinque_random_string(5000);
        XMinBinQue_PT queue2 = xminbinque_deep_copy(queue, 5);
        xminbinque_deep_free(&queue);
        xminbinque_deep_free(&queue2);
    }

    /* xminbinque_push */
    {
        XMinBinQue_PT queue = xminbinque_new(8, test_xminbinque_cmp, NULL);
        xassert(xminbinque_push(queue, "a"));
        xassert(queue->size == 1);
        xassert(queue->buckets->size == 1);

        xassert(xminbinque_push(queue, "b"));
        xassert(queue->size == 2);
        xassert(queue->buckets->size == 2);

        xassert(xminbinque_push(queue, "c"));
        xassert(queue->size == 3);
        xassert(queue->buckets->size == 2);

        xassert(xminbinque_push(queue, "d"));
        xassert(queue->size == 4);
        xassert(queue->buckets->size == 3);

        xassert(xminbinque_push(queue, "e"));
        xassert(queue->size == 5);
        xassert(queue->buckets->size == 3);

        xassert(xminbinque_push(queue, "f"));
        xassert(queue->size == 6);
        xassert(queue->buckets->size == 3);

        xassert(xminbinque_push(queue, "g"));
        xassert(queue->size == 7);
        xassert(queue->buckets->size == 3);

        xassert(xminbinque_push(queue, "h"));
        xassert(queue->size == 8);
        xassert(queue->buckets->size == 4);

        xassert_false(xminbinque_push(queue, "i"));
        xassert(queue->size == 8);
        xassert(queue->buckets->size == 4);

        xminbinque_set_strategy_discard_top(queue);

        xassert(xminbinque_push(queue, "h"));
        xassert(queue->size == 8);
        xassert(queue->buckets->size == 4);
        xassert(strcmp((char*)xminbinque_peek(queue), "b") == 0);

        xassert(xminbinque_push(queue, "i"));
        xassert(queue->size == 8);
        xassert(queue->buckets->size == 4);
        xassert(strcmp((char*)xminbinque_peek(queue), "c") == 0);

        //xminbinque_map(queue, test_xminbinque_print, NULL);
        xminbinque_free(&queue);
    }

    /* xminbinque_pop */
    {
        char* str1 = NULL;
        char* str2 = NULL;
        XMinBinQue_PT queue = xminbinque_random_string(5000);
        for (int i = 1; i <= 2500; ++i) {
            xassert(str1 = xminbinque_pop(queue));
            xassert(str2 = xminbinque_pop(queue));
            xassert(0 <= strcmp(str2, str1));
            xassert(queue->size == 5000 - 2*i);
            XMEM_FREE(str1);
            XMEM_FREE(str2);
        }

        xassert(xpseq_size(queue->buckets) == 0);
        xminbinque_free(&queue);
    }

    /* xminbinque_peek */
    {
        XMinBinQue_PT queue = xminbinque_new(5, test_xminbinque_cmp, NULL);
        xassert(xminbinque_push(queue, "a"));
        xassert(xminbinque_push(queue, "b"));
        xassert(xminbinque_push(queue, "c"));
        xassert(xminbinque_push(queue, "d"));
        xassert(xminbinque_push(queue, "e"));
        xassert(xminbinque_size(queue) == 5);

        xassert(strcmp((char*)xminbinque_peek(queue), "a") == 0);
        xassert(strcmp((char*)xminbinque_peek(queue), "a") == 0);
        xassert(queue->size == 5);
        xminbinque_free(&queue);
    }

    /* xminbinque_merge */
    {
        XMinBinQue_PT queue1 = xminbinque_random_string(5000);
        XMinBinQue_PT queue2 = xminbinque_random_string(5000);

        xminbinque_merge(queue1, &queue2);
        xassert_false(queue2);
        xassert(queue1->size == 10000);

        char* str1 = NULL;
        char* str2 = NULL;
        for (int i = 1; i <= 5000; ++i) {
            xassert(str1 = xminbinque_pop(queue1));
            xassert(str2 = xminbinque_pop(queue1));
            xassert(0 <= strcmp(str2, str1));
            xassert(queue1->size == 10000 - 2 * i);
            XMEM_FREE(str1);
            XMEM_FREE(str2);
        }

        xassert(xpseq_size(queue1->buckets) == 0);
        xminbinque_free(&queue1);
    }

    /* xminbinque_map */
    {
        XMinBinQue_PT queue = xminbinque_new(5, test_xminbinque_cmp, NULL);
        xassert(xminbinque_push(queue, "a"));
        xassert(xminbinque_push(queue, "b"));
        xassert(xminbinque_push(queue, "c"));
        xassert(xminbinque_push(queue, "d"));
        xassert(xminbinque_push(queue, "e"));
        xassert(queue->size == 5);

        xassert(xminbinque_map(queue, test_xminbinque_apply_true, NULL) == 5);
        xassert(xminbinque_map(queue, test_xminbinque_apply_false, NULL) == 0);

        //xassert(xminbinque_map(queue, test_xminbinque_print, NULL) == 5);
        xassert(queue->size == 5);
        xminbinque_free(&queue);
    }

    /* xminbinque_map_break_if_true */
    {
        XMinBinQue_PT queue = xminbinque_new(5, test_xminbinque_cmp, NULL);
        xassert(xminbinque_push(queue, "a"));
        xassert(xminbinque_push(queue, "b"));
        xassert(xminbinque_push(queue, "c"));
        xassert(xminbinque_push(queue, "d"));
        xassert(xminbinque_push(queue, "e"));
        xassert(queue->size == 5);

        xassert(xminbinque_map_break_if_true(queue, test_xminbinque_apply_true, NULL));
        xassert_false(xminbinque_map_break_if_true(queue, test_xminbinque_apply_false, NULL));

        xassert(xminbinque_map_break_if_true(queue, test_xminbinque_find_str_true, "c"));
        xassert_false(xminbinque_map_break_if_true(queue, test_xminbinque_find_str_true, "x"));

        xassert(queue->size == 5);
        xminbinque_free(&queue);
    }

    /* xminbinque_map_break_if_false */
    {
        XMinBinQue_PT queue = xminbinque_new(5, test_xminbinque_cmp, NULL);
        xassert(xminbinque_push(queue, "a"));
        xassert(xminbinque_push(queue, "b"));
        xassert(xminbinque_push(queue, "c"));
        xassert(xminbinque_push(queue, "d"));
        xassert(xminbinque_push(queue, "e"));
        xassert(queue->size == 5);

        xassert(xminbinque_map_break_if_false(queue, test_xminbinque_apply_false, NULL));
        xassert_false(xminbinque_map_break_if_false(queue, test_xminbinque_apply_true, NULL));

        xassert(xminbinque_map_break_if_false(queue, test_xminbinque_find_str_false, "c"));
        xassert_false(xminbinque_map_break_if_false(queue, test_xminbinque_find_str_false, "x"));

        xassert(queue->size == 5);
        xminbinque_free(&queue);
    }

    /* xminbinque_free : testef already by other cases */

    /* xminbinque_deep_free */
    {
        XMinBinQue_PT queue = xminbinque_random_string(100);
        xminbinque_deep_free(&queue);
    }

    /* xminbinque_clear */
    /* xminbinque_deep_clear */
    {
        XMinBinQue_PT queue = xminbinque_random_string(100);
        xminbinque_deep_clear(queue);
        xassert(queue->size == 0);
        xassert(queue->buckets->size == 0);
        xminbinque_free(&queue);
    }

    /* xminbinque_size */
    /* xminbinque_is_empty */
    {
        XMinBinQue_PT queue = xminbinque_new(5, test_xminbinque_cmp, NULL);
        xassert(queue->size == 0);
        xassert(xminbinque_is_empty(queue));

        xassert(xminbinque_push(queue, "c"));
        xassert(xminbinque_push(queue, "d"));
        xassert(queue->size == 2);
        xassert_false(xminbinque_is_empty(queue));

        xminbinque_free(&queue);
    }

    /* xminbinque_keep_min_values */
    {
        XMinBinQue_PT queue = xminbinque_new(7, test_xminbinque_cmp, NULL);
        xminbinque_keep_max_values(queue, "1", NULL);
        xminbinque_keep_max_values(queue, "8", NULL);
        xminbinque_keep_max_values(queue, "2", NULL);
        xminbinque_keep_max_values(queue, "4", NULL);
        xminbinque_keep_max_values(queue, "3", NULL);
        xminbinque_keep_max_values(queue, "7", NULL);
        xminbinque_keep_max_values(queue, "5", NULL);
        xminbinque_keep_max_values(queue, "6", NULL);
        xminbinque_keep_max_values(queue, "9", NULL);

        xassert(strcmp((char*)xminbinque_peek(queue), "8") != 0);
        xassert(strcmp((char*)xminbinque_peek(queue), "9") != 0);
        //xminbinque_map(queue, test_xminbinque_print, NULL);

        xminbinque_free(&queue);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
