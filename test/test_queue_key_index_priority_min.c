
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../queue_key_index_priority_min/xqueue_key_index_priority_min_x.h"
#include "../include/xalgos.h"

static 
int test_xkeyindexminpq_cmp(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static 
bool test_xkeyindexminpq_print(void *value, void **key, void *cl) {
    printf("%s:%s\n", (char*)*key, (char*)value);
    return true;
}

static
bool test_xkeyindexminpq_find_str_true(void *value, void **key, void *cl) {
    if (strcmp(*key, (char*)cl) == 0) {
        return true;
    }
    return false;
}

static
bool test_xkeyindexminpq_find_str_false(void *value, void **key, void *cl) {
    if (strcmp(*key, (char*)cl) == 0) {
        return false;
    }
    return true;
}

static
bool test_xkeyindexminpq_apply_true(void *value, void **key, void *cl) {
    return true;
}

static
bool test_xkeyindexminpq_apply_false(void *value, void **key, void *cl) {
    return false;
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static 
XKeyIndexMinPQ_PT xkeyindexminpq_random_string(int size) {
    XKeyIndexMinPQ_PT queue = xkeyindexminpq_new(test_xkeyindexminpq_cmp, test_xkeyindexminpq_cmp, NULL);
    if (!queue) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* key = XMEM_CALLOC(1, str_size);
            if (!key) {
                xkeyindexminpq_deep_free(&queue);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                key[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            key[str_size - 1] = '\0';

            char* value = XMEM_CALLOC(1, str_size);
            if (!value) {
                xkeyindexminpq_deep_free(&queue);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                value[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            value[str_size - 1] = '\0';

            if (xkeyindexminpq_get(queue, key)) {
                XMEM_FREE(key);
                XMEM_FREE(value);
                continue;
            }

            xkeyindexminpq_push(queue, key, value, NULL);
        }
    }

    return queue;
}

void test_xkeyindexminpq() {

    /* xkeyindexminpq_new */
    {
        XKeyIndexMinPQ_PT queue = xkeyindexminpq_new(test_xkeyindexminpq_cmp, test_xkeyindexminpq_cmp, NULL);
        xassert(queue);
        xassert(queue->key_map);
        xassert(queue->value_map);
        xassert(xkeyindexminpq_size(queue) == 0);
        xkeyindexminpq_free(&queue);
    }

    /* xkeyindexminpq_push */
    {
        XKeyIndexMinPQ_PT queue = xkeyindexminpq_new(test_xkeyindexminpq_cmp, test_xkeyindexminpq_cmp, NULL);
        xassert(xkeyindexminpq_push(queue, "a", "a0", NULL));
        xassert(xkeyindexminpq_push(queue, "b", "b0", NULL));
        xassert(xkeyindexminpq_push(queue, "c", "c0", NULL));
        xassert(xkeyindexminpq_push(queue, "d", "d0", NULL));
        xassert(xkeyindexminpq_push(queue, "e", "e0", NULL));
        xassert(xkeyindexminpq_size(queue) == 5);

        xassert(xkeyindexminpq_push(queue, "b", "b1", NULL));
        xassert(xkeyindexminpq_push(queue, "c", "c1", NULL));

        xassert(xkeyindexminpq_size(queue) == 5);
        xassert(strcmp((char*)xkeyindexminpq_get(queue, "b"), "b1") == 0);
        xassert(strcmp((char*)xkeyindexminpq_get(queue, "c"), "c1") == 0);

        xkeyindexminpq_free(&queue);
    }

    /* xkeyindexminpq_pop */
    {
        char* key = NULL;
        char* value = NULL;
        XKeyIndexMinPQ_PT queue = xkeyindexminpq_new(test_xkeyindexminpq_cmp, test_xkeyindexminpq_cmp, NULL);
        xassert(xkeyindexminpq_push(queue, "a", "a0", NULL));
        xassert(xkeyindexminpq_push(queue, "b", "b0", NULL));
        xassert(xkeyindexminpq_push(queue, "c", "c0", NULL));
        xassert(xkeyindexminpq_push(queue, "d", "d0", NULL));
        xassert(xkeyindexminpq_push(queue, "e", "e0", NULL));
        xassert(xkeyindexminpq_size(queue) == 5);

        xassert(xkeyindexminpq_pop(queue, (void**)&key, (void**)&value));
        xassert(strcmp(key, "a") == 0);
        xassert(strcmp(value, "a0") == 0);
        xassert(xkeyindexminpq_size(queue) == 4);

        xassert(xkeyindexminpq_pop(queue, (void**)&key, (void**)&value));
        xassert(strcmp(key, "b") == 0);
        xassert(strcmp(value, "b0") == 0);
        xassert(xkeyindexminpq_size(queue) == 3);

        xassert(xkeyindexminpq_pop(queue, (void**)&key, (void**)&value));
        xassert(strcmp(key, "c") == 0);
        xassert(strcmp(value, "c0") == 0);
        xassert(xkeyindexminpq_size(queue) == 2);

        xassert(xkeyindexminpq_pop(queue, (void**)&key, (void**)&value));
        xassert(strcmp(key, "d") == 0);
        xassert(strcmp(value, "d0") == 0);
        xassert(xkeyindexminpq_size(queue) == 1);

        xassert(xkeyindexminpq_pop(queue, (void**)&key, (void**)&value));
        xassert(strcmp(key, "e") == 0);
        xassert(strcmp(value, "e0") == 0);
        xassert(xkeyindexminpq_size(queue) == 0);

        xkeyindexminpq_free(&queue);
    }

    /* xkeyindexminpq_peek */
    {
        XKeyIndexMinPQ_PT queue = xkeyindexminpq_new(test_xkeyindexminpq_cmp, test_xkeyindexminpq_cmp, NULL);
        xassert(xkeyindexminpq_push(queue, "a", "a0", NULL));
        xassert(xkeyindexminpq_push(queue, "b", "b0", NULL));
        xassert(xkeyindexminpq_push(queue, "c", "c0", NULL));
        xassert(xkeyindexminpq_push(queue, "d", "d0", NULL));
        xassert(xkeyindexminpq_push(queue, "e", "e0", NULL));
        xassert(xkeyindexminpq_size(queue) == 5);

        char* key = NULL;
        char* value = NULL;
        xassert(xkeyindexminpq_peek(queue, (void**)&key, (void**)&value));
        xassert(strcmp(key, "a") == 0);
        xassert(strcmp(value, "a0") == 0);
        xassert(xkeyindexminpq_peek(queue, (void**)&key, (void**)&value));
        xassert(strcmp(key, "a") == 0);
        xassert(strcmp(value, "a0") == 0);
        xassert(xkeyindexminpq_size(queue) == 5);
        xkeyindexminpq_free(&queue);
    }

    /* xkeyindexminpq_map */
    {
        XKeyIndexMinPQ_PT queue = xkeyindexminpq_new(test_xkeyindexminpq_cmp, test_xkeyindexminpq_cmp, NULL);
        xassert(xkeyindexminpq_push(queue, "a", "a0", NULL));
        xassert(xkeyindexminpq_push(queue, "b", "b0", NULL));
        xassert(xkeyindexminpq_push(queue, "c", "c0", NULL));
        xassert(xkeyindexminpq_push(queue, "d", "d0", NULL));
        xassert(xkeyindexminpq_push(queue, "e", "e0", NULL));
        xassert(xkeyindexminpq_size(queue) == 5);

        xassert(xkeyindexminpq_map(queue, test_xkeyindexminpq_apply_true, NULL) == 5);
        xassert(xkeyindexminpq_map(queue, test_xkeyindexminpq_apply_false, NULL) == 0);

        //xassert(xkeyindexminpq_map(queue, test_xkeyindexminpq_print, NULL) == 5);
        xassert(xkeyindexminpq_size(queue) == 5);
        xkeyindexminpq_free(&queue);
    }

    /* xkeyindexminpq_map_break_if_true */
    {
        XKeyIndexMinPQ_PT queue = xkeyindexminpq_new(test_xkeyindexminpq_cmp, test_xkeyindexminpq_cmp, NULL);
        xassert(xkeyindexminpq_push(queue, "a", "a0", NULL));
        xassert(xkeyindexminpq_push(queue, "b", "b0", NULL));
        xassert(xkeyindexminpq_push(queue, "c", "c0", NULL));
        xassert(xkeyindexminpq_push(queue, "d", "d0", NULL));
        xassert(xkeyindexminpq_push(queue, "e", "e0", NULL));
        xassert(xkeyindexminpq_size(queue) == 5);

        xassert(xkeyindexminpq_map_break_if_true(queue, test_xkeyindexminpq_apply_true, NULL));
        xassert_false(xkeyindexminpq_map_break_if_true(queue, test_xkeyindexminpq_apply_false, NULL));

        xassert(xkeyindexminpq_map_break_if_true(queue, test_xkeyindexminpq_find_str_true, "c"));
        xassert_false(xkeyindexminpq_map_break_if_true(queue, test_xkeyindexminpq_find_str_true, "x"));

        xassert(xkeyindexminpq_size(queue) == 5);
        xkeyindexminpq_free(&queue);
    }

    /* xkeyindexminpq_map_break_if_false */
    {
        XKeyIndexMinPQ_PT queue = xkeyindexminpq_new(test_xkeyindexminpq_cmp, test_xkeyindexminpq_cmp, NULL);
        xassert(xkeyindexminpq_push(queue, "a", "a0", NULL));
        xassert(xkeyindexminpq_push(queue, "b", "b0", NULL));
        xassert(xkeyindexminpq_push(queue, "c", "c0", NULL));
        xassert(xkeyindexminpq_push(queue, "d", "d0", NULL));
        xassert(xkeyindexminpq_push(queue, "e", "e0", NULL));
        xassert(xkeyindexminpq_size(queue) == 5);

        xassert(xkeyindexminpq_map_break_if_false(queue, test_xkeyindexminpq_apply_false, NULL));
        xassert_false(xkeyindexminpq_map_break_if_false(queue, test_xkeyindexminpq_apply_true, NULL));

        xassert(xkeyindexminpq_map_break_if_false(queue, test_xkeyindexminpq_find_str_false, "c"));
        xassert_false(xkeyindexminpq_map_break_if_false(queue, test_xkeyindexminpq_find_str_false, "x"));

        xassert(xkeyindexminpq_size(queue) == 5);
        xkeyindexminpq_free(&queue);
    }

    /* xkeyindexminpq_free : testef already by other cases */
    /* xkeyindexminpq_deep_free */
    {
        XKeyIndexMinPQ_PT queue = xkeyindexminpq_random_string(20);
        xkeyindexminpq_deep_free(&queue);
    }

    /* xkeyindexminpq_clear */
    {
        XKeyIndexMinPQ_PT queue = xkeyindexminpq_new(test_xkeyindexminpq_cmp, test_xkeyindexminpq_cmp, NULL);
        xassert(xkeyindexminpq_push(queue, "a", "a0", NULL));
        xassert(xkeyindexminpq_push(queue, "b", "b0", NULL));
        xassert(xkeyindexminpq_push(queue, "c", "c0", NULL));
        xassert(xkeyindexminpq_push(queue, "d", "d0", NULL));
        xassert(xkeyindexminpq_push(queue, "e", "e0", NULL));
        xassert(xkeyindexminpq_size(queue) == 5);

        xkeyindexminpq_clear(queue);
        xkeyindexminpq_deep_free(&queue);
    }

    /* xkeyindexminpq_deep_clear */
    {
        XKeyIndexMinPQ_PT queue = xkeyindexminpq_random_string(20);
        xkeyindexminpq_deep_clear(queue);
        xkeyindexminpq_free(&queue);
    }

    /* xkeyindexminpq_size */
    /* xkeyindexminpq_is_empty */
    {
        XKeyIndexMinPQ_PT queue = xkeyindexminpq_new(test_xkeyindexminpq_cmp, test_xkeyindexminpq_cmp, NULL);
        xassert(xkeyindexminpq_size(queue) == 0);
        xassert(xkeyindexminpq_is_empty(queue));

        xassert(xkeyindexminpq_push(queue, "a", "a0", NULL));
        xassert(xkeyindexminpq_push(queue, "b", "b0", NULL));
        xassert(xkeyindexminpq_size(queue) == 2);
        xassert_false(xkeyindexminpq_is_empty(queue));

        xkeyindexminpq_free(&queue);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
