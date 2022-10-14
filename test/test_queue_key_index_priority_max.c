
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../queue_key_index_priority_min/xqueue_key_index_priority_min_x.h"
#include "../include/xalgos.h"

static 
int test_xkeyindexmaxpq_cmp(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static 
bool test_xkeyindexmaxpq_print(void *value, void **key, void *cl) {
    printf("%s:%s\n", (char*)*key, (char*)value);
    return true;
}

static
bool test_xkeyindexmaxpq_find_str_true(void *value, void **key, void *cl) {
    if (strcmp(*key, (char*)cl) == 0) {
        return true;
    }
    return false;
}

static
bool test_xkeyindexmaxpq_find_str_false(void *value, void **key, void *cl) {
    if (strcmp(*key, (char*)cl) == 0) {
        return false;
    }
    return true;
}

static
bool test_xkeyindexmaxpq_apply_true(void *value, void **key, void *cl) {
    return true;
}

static
bool test_xkeyindexmaxpq_apply_false(void *value, void **key, void *cl) {
    return false;
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static 
XKeyIndexMaxPQ_PT xkeyindexmaxpq_random_string(int size) {
    XKeyIndexMaxPQ_PT queue = xkeyindexmaxpq_new(test_xkeyindexmaxpq_cmp, test_xkeyindexmaxpq_cmp, NULL);
    if (!queue) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* key = XMEM_CALLOC(1, str_size);
            if (!key) {
                xkeyindexmaxpq_deep_free(&queue);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                key[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            key[str_size - 1] = '\0';

            char* value = XMEM_CALLOC(1, str_size);
            if (!value) {
                xkeyindexmaxpq_deep_free(&queue);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                value[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            value[str_size - 1] = '\0';

            if (xkeyindexmaxpq_get(queue, key)) {
                XMEM_FREE(key);
                XMEM_FREE(value);
                continue;
            }

            xkeyindexmaxpq_push(queue, key, value, NULL);
        }
    }

    return queue;
}

void test_xkeyindexmaxpq() {

    /* xkeyindexmaxpq_new */
    {
        XKeyIndexMaxPQ_PT queue = xkeyindexmaxpq_new(test_xkeyindexmaxpq_cmp, test_xkeyindexmaxpq_cmp, NULL);
        xassert(queue);
        xassert(queue->key_map);
        xassert(queue->value_map);
        xassert(xkeyindexmaxpq_size(queue) == 0);
        xkeyindexmaxpq_free(&queue);
    }

    /* xkeyindexmaxpq_push */
    {
        XKeyIndexMaxPQ_PT queue = xkeyindexmaxpq_new(test_xkeyindexmaxpq_cmp, test_xkeyindexmaxpq_cmp, NULL);
        xassert(xkeyindexmaxpq_push(queue, "a", "a0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "b", "b0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "c", "c0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "d", "d0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "e", "e0", NULL));
        xassert(xkeyindexmaxpq_size(queue) == 5);
        xassert(xmap_size(queue->key_map) == 5);
        xassert(xmap_size(queue->value_map) == 5);

        xassert(xkeyindexmaxpq_push(queue, "b", "b1", NULL));
        xassert(xkeyindexmaxpq_push(queue, "c", "c1", NULL));
        xassert(xkeyindexmaxpq_size(queue) == 5);
        xassert(xmap_size(queue->key_map) == 5);
        xassert(xmap_size(queue->value_map) == 5);

        xassert(strcmp((char*)xkeyindexmaxpq_get(queue, "b"), "b1") == 0);
        xassert(strcmp((char*)xkeyindexmaxpq_get(queue, "c"), "c1") == 0);

        xkeyindexmaxpq_free(&queue);
    }

    /* xkeyindexmaxpq_pop */
    {
        char* key = NULL;
        char* value = NULL;
        XKeyIndexMaxPQ_PT queue = xkeyindexmaxpq_new(test_xkeyindexmaxpq_cmp, test_xkeyindexmaxpq_cmp, NULL);
        xassert(xkeyindexmaxpq_push(queue, "a", "a0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "b", "b0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "c", "c0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "d", "d0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "e", "e0", NULL));
        xassert(xkeyindexmaxpq_size(queue) == 5);

        xassert(xkeyindexmaxpq_pop(queue, (void**)&key, (void**)&value));
        xassert(strcmp(key, "e") == 0);
        xassert(strcmp(value, "e0") == 0);
        xassert(xkeyindexmaxpq_size(queue) == 4);

        xassert(xkeyindexmaxpq_pop(queue, (void**)&key, (void**)&value));
        xassert(strcmp(key, "d") == 0);
        xassert(strcmp(value, "d0") == 0);
        xassert(xkeyindexmaxpq_size(queue) == 3);

        xassert(xkeyindexmaxpq_pop(queue, (void**)&key, (void**)&value));
        xassert(strcmp(key, "c") == 0);
        xassert(strcmp(value, "c0") == 0);
        xassert(xkeyindexmaxpq_size(queue) == 2);

        xassert(xkeyindexmaxpq_pop(queue, (void**)&key, (void**)&value));
        xassert(strcmp(key, "b") == 0);
        xassert(strcmp(value, "b0") == 0);
        xassert(xkeyindexmaxpq_size(queue) == 1);

        xassert(xkeyindexmaxpq_pop(queue, (void**)&key, (void**)&value));
        xassert(strcmp(key, "a") == 0);
        xassert(strcmp(value, "a0") == 0);
        xassert(xkeyindexmaxpq_size(queue) == 0);

        xkeyindexmaxpq_free(&queue);
    }

    /* xkeyindexmaxpq_peek */
    {
        XKeyIndexMaxPQ_PT queue = xkeyindexmaxpq_new(test_xkeyindexmaxpq_cmp, test_xkeyindexmaxpq_cmp, NULL);
        xassert(xkeyindexmaxpq_push(queue, "a", "a0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "b", "b0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "c", "c0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "d", "d0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "e", "e0", NULL));
        xassert(xkeyindexmaxpq_size(queue) == 5);

        char* key = NULL;
        char* value = NULL;
        xassert(xkeyindexmaxpq_peek(queue, (void**)&key, (void**)&value));
        xassert(strcmp(key, "e") == 0);
        xassert(strcmp(value, "e0") == 0);
        xassert(xkeyindexmaxpq_peek(queue, (void**)&key, (void**)&value));
        xassert(strcmp(key, "e") == 0);
        xassert(strcmp(value, "e0") == 0);
        xassert(xkeyindexmaxpq_size(queue) == 5);
        xkeyindexmaxpq_free(&queue);
    }

    /* xkeyindexmaxpq_map */
    {
        XKeyIndexMaxPQ_PT queue = xkeyindexmaxpq_new(test_xkeyindexmaxpq_cmp, test_xkeyindexmaxpq_cmp, NULL);
        xassert(xkeyindexmaxpq_push(queue, "a", "a0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "b", "b0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "c", "c0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "d", "d0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "e", "e0", NULL));
        xassert(xkeyindexmaxpq_size(queue) == 5);

        xassert(xkeyindexmaxpq_map(queue, test_xkeyindexmaxpq_apply_true, NULL) == 5);
        xassert(xkeyindexmaxpq_map(queue, test_xkeyindexmaxpq_apply_false, NULL) == 0);

        //xassert(xkeyindexmaxpq_map(queue, test_xkeyindexmaxpq_print, NULL) == 5);
        xassert(xkeyindexmaxpq_size(queue) == 5);
        xkeyindexmaxpq_free(&queue);
    }

    /* xkeyindexmaxpq_map_break_if_true */
    {
        XKeyIndexMaxPQ_PT queue = xkeyindexmaxpq_new(test_xkeyindexmaxpq_cmp, test_xkeyindexmaxpq_cmp, NULL);
        xassert(xkeyindexmaxpq_push(queue, "a", "a0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "b", "b0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "c", "c0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "d", "d0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "e", "e0", NULL));
        xassert(xkeyindexmaxpq_size(queue) == 5);

        xassert(xkeyindexmaxpq_map_break_if_true(queue, test_xkeyindexmaxpq_apply_true, NULL));
        xassert_false(xkeyindexmaxpq_map_break_if_true(queue, test_xkeyindexmaxpq_apply_false, NULL));

        xassert(xkeyindexmaxpq_map_break_if_true(queue, test_xkeyindexmaxpq_find_str_true, "c"));
        xassert_false(xkeyindexmaxpq_map_break_if_true(queue, test_xkeyindexmaxpq_find_str_true, "x"));

        xassert(xkeyindexmaxpq_size(queue) == 5);
        xkeyindexmaxpq_free(&queue);
    }

    /* xkeyindexmaxpq_map_break_if_false */
    {
        XKeyIndexMaxPQ_PT queue = xkeyindexmaxpq_new(test_xkeyindexmaxpq_cmp, test_xkeyindexmaxpq_cmp, NULL);
        xassert(xkeyindexmaxpq_push(queue, "a", "a0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "b", "b0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "c", "c0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "d", "d0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "e", "e0", NULL));
        xassert(xkeyindexmaxpq_size(queue) == 5);

        xassert(xkeyindexmaxpq_map_break_if_false(queue, test_xkeyindexmaxpq_apply_false, NULL));
        xassert_false(xkeyindexmaxpq_map_break_if_false(queue, test_xkeyindexmaxpq_apply_true, NULL));

        xassert(xkeyindexmaxpq_map_break_if_false(queue, test_xkeyindexmaxpq_find_str_false, "c"));
        xassert_false(xkeyindexmaxpq_map_break_if_false(queue, test_xkeyindexmaxpq_find_str_false, "x"));

        xassert(xkeyindexmaxpq_size(queue) == 5);
        xkeyindexmaxpq_free(&queue);
    }

    /* xkeyindexmaxpq_free : testef already by other cases */
    /* xkeyindexmaxpq_deep_free */
    {
        XKeyIndexMaxPQ_PT queue = xkeyindexmaxpq_random_string(20);
        xkeyindexmaxpq_deep_free(&queue);
    }

    /* xkeyindexmaxpq_clear */
    {
        XKeyIndexMaxPQ_PT queue = xkeyindexmaxpq_new(test_xkeyindexmaxpq_cmp, test_xkeyindexmaxpq_cmp, NULL);
        xassert(xkeyindexmaxpq_push(queue, "a", "a0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "b", "b0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "c", "c0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "d", "d0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "e", "e0", NULL));
        xassert(xkeyindexmaxpq_size(queue) == 5);

        xkeyindexmaxpq_clear(queue);
        xkeyindexmaxpq_deep_free(&queue);
    }

    /* xkeyindexmaxpq_deep_clear */
    {
        XKeyIndexMaxPQ_PT queue = xkeyindexmaxpq_random_string(20);
        xkeyindexmaxpq_deep_clear(queue);
        xkeyindexmaxpq_free(&queue);
    }

    /* xkeyindexmaxpq_size */
    /* xkeyindexmaxpq_is_empty */
    {
        XKeyIndexMaxPQ_PT queue = xkeyindexmaxpq_new(test_xkeyindexmaxpq_cmp, test_xkeyindexmaxpq_cmp, NULL);
        xassert(xkeyindexmaxpq_size(queue) == 0);
        xassert(xkeyindexmaxpq_is_empty(queue));

        xassert(xkeyindexmaxpq_push(queue, "a", "a0", NULL));
        xassert(xkeyindexmaxpq_push(queue, "b", "b0", NULL));
        xassert(xkeyindexmaxpq_size(queue) == 2);
        xassert_false(xkeyindexmaxpq_is_empty(queue));

        xkeyindexmaxpq_free(&queue);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
