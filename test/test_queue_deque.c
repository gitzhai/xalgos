
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/xutils.h"
#include "../queue_deque/xqueue_deque_x.h"
#include "../queue_sequence/xqueue_sequence_x.h"
#include "../include/xalgos.h"

static
bool deque_print(void *x, void *cl) {
    printf("%s\n", (char*)x);
    return true;
}

static 
int sort_compare(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static
XDeque_PT xdeque_random_string(int size) {
    XDeque_PT deque = xdeque_new(0);
    if (!deque) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xdeque_deep_free(&deque);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            str[str_size - 1] = '\0';

            xdeque_push_back(deque, str);
        }
    }

    return deque;
}

static
XDeque_PT xdeque_random_string_no_limit(int size) {
    XDeque_PT deque = xdeque_new(10);
    if (!deque) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xdeque_deep_free(&deque);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            str[str_size - 1] = '\0';

            xdeque_push_back_no_limit(deque, str);
        }
    }

    return deque;
}

void test_xdeque() {
    /* xdeque_new */
    {
        /* "capacity" < 0 */
        {
            bool except = false;

            XEXCEPT_TRY
                XDeque_PT deque = xdeque_new(-1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* "capacity" == 0 */
        {
            XDeque_PT deque = xdeque_new(0);

            xassert(deque->size == 0);
            xassert(deque->capacity == 0);
            xassert(xpseq_capacity(deque->layer1_seq) == XUTILS_DEQUE_LAYER1_DEFAULT_LENGTH);
            xassert(deque->discard_strategy == XUTILS_QUEUE_STRATEGY_DISCARD_NEW);

            xdeque_free(&deque);
        }

        /* 0 < "capacity" */
        {
            XDeque_PT deque = xdeque_new(10);

            xassert(deque->size == 0);
            xassert(deque->capacity == 10);
            xassert(xpseq_capacity(deque->layer1_seq) == 10);
            xassert(deque->discard_strategy == XUTILS_QUEUE_STRATEGY_DISCARD_NEW);

            xdeque_free(&deque);
        }
    }

    /* xdeque_copy */
    {
        /* deque is NULL */
        {
            bool except = false;

            XEXCEPT_TRY
                XDeque_PT deque = xdeque_copy(NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }
    }

    /* xdeque_copyn */
    {
        /* deque is NULL */
        {
            bool except = false;

            XEXCEPT_TRY
                XDeque_PT deque = xdeque_copyn(NULL, 10);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* count < 0 */
        {
            XDeque_PT deque = xdeque_new(10);
            bool except = false;

            XEXCEPT_TRY
                XDeque_PT deque = xdeque_copyn(deque, -1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xdeque_free(&deque);
        }

        /* deque is empty, count == 0 */
        {
            XDeque_PT deque = xdeque_new(0);
            XDeque_PT ndeque = xdeque_copyn(deque, 0);

            xassert(ndeque->size == 0);
            xassert(ndeque->capacity == 0);
            xassert(ndeque->discard_strategy == deque->discard_strategy);

            xdeque_free(&deque);
            xdeque_free(&ndeque);
        }

        /* deque is empty, 0 < count */
        {
            XDeque_PT deque = xdeque_new(20);
            XDeque_PT ndeque = xdeque_copyn(deque, 10);

            xassert(ndeque->size == 0);
            xassert(ndeque->capacity == 20);
            xassert(ndeque->discard_strategy == deque->discard_strategy);

            xdeque_free(&deque);
            xdeque_free(&ndeque);
        }

        /* deque is not empty, count == 0 */
        {
            XDeque_PT deque = xdeque_new(10);
            xdeque_push_back(deque, "abc");
            xdeque_push_back(deque, "def");
            xdeque_push_back(deque, "xyz");

            XDeque_PT ndeque = xdeque_copyn(deque, 0);

            xassert(ndeque->size == 0);
            xassert(ndeque->capacity == 10);
            xassert(ndeque->discard_strategy == deque->discard_strategy);
            
            xdeque_free(&deque);
            xdeque_free(&ndeque);
        }

        /* deque is not empty, count == 0 */
        {
            XDeque_PT deque = xdeque_new(10);
            xdeque_push_back(deque, "abc");
            xdeque_push_back(deque, "def");
            xdeque_push_back(deque, "xyz");
            xdeque_pop_front(deque);

            XDeque_PT ndeque = xdeque_copyn(deque, 0);

            xassert(ndeque->size == 0);
            xassert(ndeque->capacity == 10);
            xassert(ndeque->discard_strategy == deque->discard_strategy);
            
            xdeque_free(&deque);
            xdeque_free(&ndeque);
        }

        /* deque is not empty, 0 < count <= size, capacity != 0 */
        {
            XDeque_PT deque = xdeque_new(10);
            xdeque_push_back(deque, "abc");
            xdeque_push_back(deque, "def");
            xdeque_push_back(deque, "xyz");

            XDeque_PT ndeque = xdeque_copyn(deque, 2);

            xassert(ndeque->size == 2);
            xassert(ndeque->capacity == 2);
            xassert(ndeque->discard_strategy == deque->discard_strategy);

            xassert(strcmp((char*)xdeque_get(ndeque, 0), "abc") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 1), "def") == 0);

            xdeque_free(&deque);
            xdeque_free(&ndeque);
        }

        /* deque is not empty, 0 < count <= size, capacity == 0 */
        {
            XDeque_PT deque = xdeque_new(3);
            xdeque_push_back_no_limit(deque, "a1");
            xdeque_push_back_no_limit(deque, "a2");
            xdeque_push_back_no_limit(deque, "a3");
            xdeque_push_back_no_limit(deque, "a4");
            xdeque_push_back_no_limit(deque, "a5");
            xdeque_push_back_no_limit(deque, "a6");
            xdeque_push_back_no_limit(deque, "a7");
            xdeque_push_back_no_limit(deque, "a8");
            xdeque_push_back_no_limit(deque, "a9");
            xdeque_push_back_no_limit(deque, "a10");

            XDeque_PT ndeque = xdeque_copyn(deque, 10);

            xassert(ndeque->size == 10);
            xassert(ndeque->capacity == 0);
            xassert(ndeque->discard_strategy == deque->discard_strategy);

            xassert(strcmp((char*)xdeque_get(ndeque, 0), "a1") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 1), "a2") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 2), "a3") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 3), "a4") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 4), "a5") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 5), "a6") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 6), "a7") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 7), "a8") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 8), "a9") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 9), "a10") == 0);

            xdeque_free(&deque);
            xdeque_free(&ndeque);
        }

        /* deque is not empty, size < count, capacity != 0 */
        {
            XDeque_PT deque = xdeque_new(10);
            xdeque_push_back(deque, "abc");
            xdeque_push_back(deque, "def");
            xdeque_push_back(deque, "xyz");

            XDeque_PT ndeque = xdeque_copyn(deque, 50);

            xassert(ndeque->size == 3);
            xassert(ndeque->capacity == 3);
            xassert(ndeque->discard_strategy == deque->discard_strategy);

            xassert(strcmp((char*)xdeque_get(ndeque, 0), "abc") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 1), "def") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 2), "xyz") == 0);

            xdeque_free(&deque);
            xdeque_free(&ndeque);
        }

        /* deque is not empty, size < count, capacity == 0 */
        {
            XDeque_PT deque = xdeque_new(3);
            xdeque_push_back_no_limit(deque, "a1");
            xdeque_push_back_no_limit(deque, "a2");
            xdeque_push_back_no_limit(deque, "a3");
            xdeque_push_back_no_limit(deque, "a4");
            xdeque_push_back_no_limit(deque, "a5");
            xdeque_push_back_no_limit(deque, "a6");
            xdeque_push_back_no_limit(deque, "a7");
            xdeque_push_back_no_limit(deque, "a8");
            xdeque_push_back_no_limit(deque, "a9");
            xdeque_push_back_no_limit(deque, "a10");

            XDeque_PT ndeque = xdeque_copyn(deque, 50);

            xassert(ndeque->size == 10);
            xassert(ndeque->capacity == 0);
            xassert(ndeque->discard_strategy == deque->discard_strategy);

            xassert(strcmp((char*)xdeque_get(ndeque, 0), "a1") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 1), "a2") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 2), "a3") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 3), "a4") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 4), "a5") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 5), "a6") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 6), "a7") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 7), "a8") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 8), "a9") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 9), "a10") == 0);

            xdeque_free(&deque);
            xdeque_free(&ndeque);
        }
    }

    /* xdeque_deep_copy */
    {
        /* deque is NULL */
        {
            bool except = false;

            XEXCEPT_TRY
                XDeque_PT deque = xdeque_deep_copy(NULL, 4);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }
    }

    /* xdeque_deep_copyn */
    {
        /* deque is NULL */
        {
            bool except = false;

            XEXCEPT_TRY
                XDeque_PT deque = xdeque_deep_copyn(NULL, 10, 4);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* count < 0 */
        {
            XDeque_PT deque = xdeque_new(10);
            bool except = false;

            XEXCEPT_TRY
                XDeque_PT deque = xdeque_deep_copyn(deque, -1, 4);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xdeque_free(&deque);
        }

        /* elem_size == 0 */
        {
            XDeque_PT deque = xdeque_new(0);
            bool except = false;

            XEXCEPT_TRY
                XDeque_PT deque = xdeque_deep_copyn(deque, 10, 0);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xdeque_free(&deque);
        }

        /* elem_size < 0 */
        {
            XDeque_PT deque = xdeque_new(0);
            bool except = false;

            XEXCEPT_TRY
                XDeque_PT deque = xdeque_deep_copyn(deque, 10, -1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xdeque_free(&deque);
        }

        /* deque is empty, count == 0 */
        {
            XDeque_PT deque = xdeque_new(0);
            XDeque_PT ndeque = xdeque_deep_copyn(deque, 0, 4);

            xassert(ndeque->size == 0);
            xassert(ndeque->capacity == 0);
            xassert(ndeque->discard_strategy == deque->discard_strategy);

            xdeque_free(&deque);
            xdeque_deep_free(&ndeque);
        }

        /* deque is empty, 0 < count */
        {
            XDeque_PT deque = xdeque_new(20);
            XDeque_PT ndeque = xdeque_deep_copyn(deque, 10, 4);

            xassert(ndeque->size == 0);
            xassert(ndeque->capacity == 20);
            xassert(ndeque->discard_strategy == deque->discard_strategy);

            xdeque_free(&deque);
            xdeque_deep_free(&ndeque);
        }

        /* deque is not empty, count == 0 */
        {
            XDeque_PT deque = xdeque_new(10);
            xdeque_push_back(deque, "abc");
            xdeque_push_back(deque, "def");
            xdeque_push_back(deque, "xyz");

            XDeque_PT ndeque = xdeque_deep_copyn(deque, 0, 4);

            xassert(ndeque->size == 0);
            xassert(ndeque->capacity == 10);
            xassert(ndeque->discard_strategy == deque->discard_strategy);

            xdeque_free(&deque);
            xdeque_deep_free(&ndeque);
        }

        /* deque is not empty, 0 < count <= size */
        {
            XDeque_PT deque = xdeque_new(10);
            xdeque_push_back(deque, "abc");
            xdeque_push_back(deque, "def");
            xdeque_push_back(deque, "xyz");

            XDeque_PT ndeque = xdeque_deep_copyn(deque, 2, 4);

            xassert(ndeque->size == 2);
            xassert(ndeque->capacity == 2);
            xassert(ndeque->discard_strategy == deque->discard_strategy);

            xassert(strcmp((char*)xdeque_get(ndeque, 0), "abc") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 1), "def") == 0);

            xdeque_free(&deque);
            xdeque_deep_free(&ndeque);
        }

        /* deque is not empty, size < count, capacity == 0 */
        {
            XDeque_PT deque = xdeque_new(3);
            xdeque_push_back_no_limit(deque, "a1");
            xdeque_push_back_no_limit(deque, "a2");
            xdeque_push_back_no_limit(deque, "a3");
            xdeque_push_back_no_limit(deque, "a4");
            xdeque_push_back_no_limit(deque, "a5");
            xdeque_push_back_no_limit(deque, "a6");
            xdeque_push_back_no_limit(deque, "a7");
            xdeque_push_back_no_limit(deque, "a8");
            xdeque_push_back_no_limit(deque, "a9");

            XDeque_PT ndeque = xdeque_deep_copyn(deque, 50, 3);

            xassert(ndeque->size == 9);
            xassert(ndeque->capacity == 0);
            xassert(ndeque->discard_strategy == deque->discard_strategy);

            xassert(strcmp((char*)xdeque_get(ndeque, 0), "a1") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 1), "a2") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 2), "a3") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 3), "a4") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 4), "a5") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 5), "a6") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 6), "a7") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 7), "a8") == 0);
            xassert(strcmp((char*)xdeque_get(ndeque, 8), "a9") == 0);

            xdeque_free(&deque);
            xdeque_deep_free(&ndeque);
        }
    }

    /* xdeque_vload */
    {
        /* deque is NULL */
        {
            bool except = false;
            XEXCEPT_TRY
                xdeque_vload(NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* 0 < size */
        {
            XDeque_PT deque = xdeque_new(10);
            xdeque_push_back(deque, "abc");

            xdeque_vload(deque, "def", NULL);
            xassert(deque->size == 2);

            xdeque_free(&deque);
        }

        /* x == NULL */
        {
            XDeque_PT deque = xdeque_new(10);
            xdeque_vload(deque, NULL);
            xassert(deque->size == 0);
            xdeque_free(&deque);
        }

        /* load multiple values */
        {
            XDeque_PT deque = xdeque_new(10);
            xassert(xdeque_vload(deque, "abc", "def", "xyz", NULL) == 3);

            xassert(deque->size == 3);
            xassert(strcmp((char*)xdeque_get(deque, 0), "abc") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 1), "def") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 2), "xyz") == 0);

            xdeque_free(&deque);
        }
    }

    /* xdeque_aload */
    {
        /* deque is NULL */
        {
            XPArray_PT array = xparray_new(10);
            bool except = false;
            XEXCEPT_TRY
                xdeque_aload(NULL, array);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xparray_free(&array);
        }

        /* 0 < size */
        {
            XDeque_PT deque = xdeque_new(10);
            xdeque_push_back(deque, "abc");
            XPArray_PT array = xparray_new(10);

            xdeque_aload(deque, array);
            xassert(deque->size == 1);

            xdeque_free(&deque);
            xparray_free(&array);
        }

        /* xs == NULL */
        {
            XDeque_PT deque = xdeque_new(10);

            bool except = false;
            XEXCEPT_TRY
                xdeque_aload(deque, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xdeque_free(&deque);
        }

        /* xs is empty */
        {
            XDeque_PT deque = xdeque_new(10);
            XPArray_PT array = xparray_new(10);

            xassert(xdeque_aload(deque, array) == 0);

            xassert(deque->size == 0);

            xdeque_free(&deque);
            xparray_free(&array);
        }

        /* capacity < xs size */
        {
            XDeque_PT deque = xdeque_new(2);

            XPArray_PT array = xparray_new(3);
            xparray_vload(array, "abc", "def", "xyz", NULL);

            xassert(xdeque_aload(deque, array) == 2);

            xassert(deque->size == 2);
            xassert(strcmp((char*)xdeque_get(deque, 0), "abc") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 1), "def") == 0);

            xdeque_free(&deque);
            xparray_free(&array);
        }

        /* xs size < capacity */
        {
            XDeque_PT deque = xdeque_new(10);
            XPArray_PT array = xparray_new(3);
            xparray_vload(array, "abc", "def", "xyz", NULL);

            xassert(xdeque_aload(deque, array) == 3);

            xassert(deque->size == 3);
            xassert(strcmp((char*)xdeque_get(deque, 0), "abc") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 1), "def") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 2), "xyz") == 0);

            xdeque_free(&deque);
            xparray_free(&array);
        }
    }

    /* xdeque_push_front */
    /* xdeque_push_front_no_limit */
    {
        /* capacity != 0 */
        {
            /* layer1_seq is not full */
            {
                XDeque_PT deque = xdeque_new(10);

                xdeque_push_front(deque, "a1");
                xassert(strcmp((char*)xdeque_front(deque), "a1") == 0);
                xassert(deque->size == 1);
                xassert(deque->capacity == 10);

                xdeque_push_front(deque, "a2");
                xassert(strcmp((char*)xdeque_front(deque), "a2") == 0);
                xassert(deque->size == 2);
                xassert(deque->capacity == 10);

                xdeque_push_front(deque, "a3");
                xassert(strcmp((char*)xdeque_front(deque), "a3") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 10);

                xdeque_push_front(deque, "a4");
                xassert(strcmp((char*)xdeque_front(deque), "a4") == 0);
                xassert(deque->size == 4);
                xassert(deque->capacity == 10);

                xdeque_push_front(deque, "a5");
                xassert(strcmp((char*)xdeque_front(deque), "a5") == 0);
                xassert(deque->size == 5);
                xassert(deque->capacity == 10);

                xdeque_push_front(deque, "a6");
                xassert(strcmp((char*)xdeque_front(deque), "a6") == 0);
                xassert(deque->size == 6);
                xassert(deque->capacity == 10);

                xdeque_free(&deque);
            }

            /* layer1_seq is full, discard_strategy == XUTILS_QUEUE_STRATEGY_DISCARD_NEW */
            {
                XDeque_PT deque = xdeque_new(3);
                xdeque_set_strategy_discard_new(deque);

                xdeque_push_front(deque, "a1");
                xassert(strcmp((char*)xdeque_front(deque), "a1") == 0);
                xdeque_push_front(deque, "a2");
                xassert(strcmp((char*)xdeque_front(deque), "a2") == 0);
                xdeque_push_front(deque, "a3");
                xassert(strcmp((char*)xdeque_front(deque), "a3") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);

                xassert_false(xdeque_push_front(deque, "a4"));
                xassert(strcmp((char*)xdeque_front(deque), "a3") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);
                xassert_false(xdeque_push_front(deque, "a5"));
                xassert(strcmp((char*)xdeque_front(deque), "a3") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);
                xassert_false(xdeque_push_front(deque, "a6"));
                xassert(strcmp((char*)xdeque_front(deque), "a3") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);

                xassert(strcmp((char*)xdeque_get(deque, 1), "a2") == 0);
                xassert(strcmp((char*)xdeque_get(deque, 2), "a1") == 0);

                xdeque_free(&deque);
            }

            /* layer1_seq is full, discard_strategy == XUTILS_QUEUE_STRATEGY_DISCARD_FRONT */
            {
                XDeque_PT deque = xdeque_new(3);
                xdeque_set_strategy_discard_front(deque);

                xdeque_push_front(deque, "a1");
                xassert(strcmp((char*)xdeque_front(deque), "a1") == 0);
                xdeque_push_front(deque, "a2");
                xassert(strcmp((char*)xdeque_front(deque), "a2") == 0);
                xdeque_push_front(deque, "a3");
                xassert(strcmp((char*)xdeque_front(deque), "a3") == 0);

                xassert(xdeque_push_front(deque, "a4"));
                xassert(strcmp((char*)xdeque_front(deque), "a4") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);
                xassert(xdeque_push_front(deque, "a5"));
                xassert(strcmp((char*)xdeque_front(deque), "a5") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);
                xassert(xdeque_push_front(deque, "a6"));
                xassert(strcmp((char*)xdeque_front(deque), "a6") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);

                xassert(strcmp((char*)xdeque_get(deque, 1), "a2") == 0);
                xassert(strcmp((char*)xdeque_get(deque, 2), "a1") == 0);

                xdeque_free(&deque);
            }

            /* layer1_seq is full, discard_strategy == XUTILS_QUEUE_STRATEGY_DISCARD_BACK */
            {
                XDeque_PT deque = xdeque_new(3);
                xdeque_set_strategy_discard_back(deque);

                xdeque_push_front(deque, "a1");
                xassert(strcmp((char*)xdeque_front(deque), "a1") == 0);
                xdeque_push_front(deque, "a2");
                xassert(strcmp((char*)xdeque_front(deque), "a2") == 0);
                xdeque_push_front(deque, "a3");
                xassert(strcmp((char*)xdeque_front(deque), "a3") == 0);

                xassert(xdeque_push_front(deque, "a4"));
                xassert(strcmp((char*)xdeque_front(deque), "a4") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);
                xassert(xdeque_push_front(deque, "a5"));
                xassert(strcmp((char*)xdeque_front(deque), "a5") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);
                xassert(xdeque_push_front(deque, "a6"));
                xassert(strcmp((char*)xdeque_front(deque), "a6") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);

                xassert(strcmp((char*)xdeque_get(deque, 1), "a5") == 0);
                xassert(strcmp((char*)xdeque_get(deque, 2), "a4") == 0);

                xdeque_free(&deque);
            }
        }

        /* capacity == 0 directly */
        {
            XDeque_PT deque = xdeque_new(0);
            for (int i = 0; i < 20000; i++) {
                if (i % 2 == 0) {
                    xdeque_push_front(deque, "a");
                }
                else {
                    xdeque_push_front(deque, "b");
                }
            }
            xassert(deque->size == 20000);
            xassert(deque->capacity == 0);

            for (int i = 0; i < 20000; i++) {
                if (i % 2 == 0) {
                    xassert(strcmp((char*)xdeque_get_impl(deque, i), "b") == 0);
                }
                else {
                    xassert(strcmp((char*)xdeque_get_impl(deque, i), "a") == 0);
                }
            }

            xdeque_free(&deque);
        }

        /* capacity == 0 indirectly */
        {
            XDeque_PT deque = xdeque_new(3);
            xdeque_push_front(deque, "a1");
            xdeque_push_front(deque, "a2");
            xdeque_push_front(deque, "a3");
            xdeque_push_front_no_limit(deque, "a4");
            xdeque_push_front(deque, "a5");
            xdeque_push_front(deque, "a6");
            xdeque_push_front(deque, "a7");
            xdeque_push_front(deque, "a8");
            xdeque_push_front(deque, "a9");
            xdeque_push_front(deque, "a10");

            xassert(deque->size == 10);
            xassert(deque->capacity == 0);

            xassert(xpseq_size(deque->layer1_seq) == 4);
            xassert(xpseq_size(xpseq_get_impl(deque->layer1_seq, 0)) == 1);
            xassert(xpseq_is_full(xpseq_get_impl(deque->layer1_seq, 1)));
            xassert(xpseq_size(xpseq_get_impl(deque->layer1_seq, 1)) == 3);
            xassert(xpseq_is_full(xpseq_get_impl(deque->layer1_seq, 2)));
            xassert(xpseq_size(xpseq_get_impl(deque->layer1_seq, 2)) == 3);
            xassert(xpseq_is_full(xpseq_get_impl(deque->layer1_seq, 3)));
            xassert(xpseq_size(xpseq_get_impl(deque->layer1_seq, 3)) == 3);

            xassert(strcmp((char*)xdeque_get(deque, 0), "a10") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 1), "a9") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 2), "a8") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 3), "a7") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 4), "a6") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 5), "a5") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 6), "a4") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 7), "a3") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 8), "a2") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 9), "a1") == 0);

            xdeque_free(&deque);
        }
    }

    /* xdeque_push_back */
    /* xdeque_push_back_no_limit */
    {
        /* capacity != 0 */
        {
            /* layer1_seq is not full */
            {
                XDeque_PT deque = xdeque_new(10);

                xdeque_push_back(deque, "a1");
                xassert(strcmp((char*)xdeque_back(deque), "a1") == 0);
                xassert(deque->size == 1);
                xassert(deque->capacity == 10);

                xdeque_push_back(deque, "a2");
                xassert(strcmp((char*)xdeque_back(deque), "a2") == 0);
                xassert(deque->size == 2);
                xassert(deque->capacity == 10);

                xdeque_push_back(deque, "a3");
                xassert(strcmp((char*)xdeque_back(deque), "a3") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 10);
                
                xdeque_push_back(deque, "a4");
                xassert(strcmp((char*)xdeque_back(deque), "a4") == 0);
                xassert(deque->size == 4);
                xassert(deque->capacity == 10);
                
                xdeque_push_back(deque, "a5");
                xassert(strcmp((char*)xdeque_back(deque), "a5") == 0);
                xassert(deque->size == 5);
                xassert(deque->capacity == 10);
                
                xdeque_push_back(deque, "a6");
                xassert(strcmp((char*)xdeque_back(deque), "a6") == 0);
                xassert(deque->size == 6);
                xassert(deque->capacity == 10);

                xdeque_free(&deque);
            }

            /* layer1_seq is full, discard_strategy == XUTILS_QUEUE_STRATEGY_DISCARD_NEW */
            {
                XDeque_PT deque = xdeque_new(3);
                xdeque_set_strategy_discard_new(deque);

                xdeque_push_back(deque, "a1");
                xassert(strcmp((char*)xdeque_back(deque), "a1") == 0);
                xdeque_push_back(deque, "a2");
                xassert(strcmp((char*)xdeque_back(deque), "a2") == 0);
                xdeque_push_back(deque, "a3");
                xassert(strcmp((char*)xdeque_back(deque), "a3") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);

                xassert_false(xdeque_push_back(deque, "a4"));
                xassert(strcmp((char*)xdeque_back(deque), "a3") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);
                xassert_false(xdeque_push_back(deque, "a5"));
                xassert(strcmp((char*)xdeque_back(deque), "a3") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);
                xassert_false(xdeque_push_back(deque, "a6"));
                xassert(strcmp((char*)xdeque_back(deque), "a3") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);

                xassert(strcmp((char*)xdeque_get(deque, 0), "a1") == 0);
                xassert(strcmp((char*)xdeque_get(deque, 1), "a2") == 0);

                xdeque_free(&deque);
            }

            /* layer1_seq is full, discard_strategy == XUTILS_QUEUE_STRATEGY_DISCARD_FRONT */
            {
                XDeque_PT deque = xdeque_new(3);
                xdeque_set_strategy_discard_front(deque);

                xdeque_push_back(deque, "a1");
                xassert(strcmp((char*)xdeque_back(deque), "a1") == 0);
                xdeque_push_back(deque, "a2");
                xassert(strcmp((char*)xdeque_back(deque), "a2") == 0);
                xdeque_push_back(deque, "a3");
                xassert(strcmp((char*)xdeque_back(deque), "a3") == 0);

                xassert(xdeque_push_back(deque, "a4"));
                xassert(strcmp((char*)xdeque_back(deque), "a4") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);
                xassert(xdeque_push_back(deque, "a5"));
                xassert(strcmp((char*)xdeque_back(deque), "a5") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);
                xassert(xdeque_push_back(deque, "a6"));
                xassert(strcmp((char*)xdeque_back(deque), "a6") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);

                xassert(strcmp((char*)xdeque_get(deque, 0), "a4") == 0);
                xassert(strcmp((char*)xdeque_get(deque, 1), "a5") == 0);

                xdeque_free(&deque);
            }

            /* layer1_seq is full, discard_strategy == XUTILS_QUEUE_STRATEGY_DISCARD_BACK */
            {
                XDeque_PT deque = xdeque_new(3);
                xdeque_set_strategy_discard_back(deque);

                xdeque_push_back(deque, "a1");
                xassert(strcmp((char*)xdeque_back(deque), "a1") == 0);
                xdeque_push_back(deque, "a2");
                xassert(strcmp((char*)xdeque_back(deque), "a2") == 0);
                xdeque_push_back(deque, "a3");
                xassert(strcmp((char*)xdeque_back(deque), "a3") == 0);

                xassert(xdeque_push_back(deque, "a4"));
                xassert(strcmp((char*)xdeque_back(deque), "a4") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);
                xassert(xdeque_push_back(deque, "a5"));
                xassert(strcmp((char*)xdeque_back(deque), "a5") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);
                xassert(xdeque_push_back(deque, "a6"));
                xassert(strcmp((char*)xdeque_back(deque), "a6") == 0);
                xassert(deque->size == 3);
                xassert(deque->capacity == 3);

                xassert(strcmp((char*)xdeque_get(deque, 0), "a1") == 0);
                xassert(strcmp((char*)xdeque_get(deque, 1), "a2") == 0);

                xdeque_free(&deque);
            }
        }

        /* capacity == 0 directly */
        {
            XDeque_PT deque = xdeque_new(0);
            for (int i = 0; i < 20000; i++) {
                if (i % 2 == 0) {
                    xdeque_push_back(deque, "a");
                }
                else {
                    xdeque_push_back(deque, "b");
                }
            }
            xassert(deque->size == 20000);
            xassert(deque->capacity == 0);

            for (int i = 0; i < 20000; i++) {
                if (i % 2 == 0) {
                    xassert(strcmp((char*)xdeque_get_impl(deque, i), "a") == 0);
                }
                else {
                    xassert(strcmp((char*)xdeque_get_impl(deque, i), "b") == 0);
                }
            }

            xdeque_free(&deque);
        }

        /* capacity == 0 indirectly */
        {
            XDeque_PT deque = xdeque_new(3);
            xdeque_push_back(deque, "a1");
            xdeque_push_back(deque, "a2");
            xdeque_push_back(deque, "a3");
            xdeque_push_back_no_limit(deque, "a4");
            xdeque_push_back(deque, "a5");
            xdeque_push_back(deque, "a6");
            xdeque_push_back(deque, "a7");
            xdeque_push_back(deque, "a8");
            xdeque_push_back(deque, "a9");
            xdeque_push_back(deque, "a10");

            xassert(deque->size == 10);
            xassert(deque->capacity == 0);

            xassert(xpseq_size(deque->layer1_seq) == 4);
            xassert(xpseq_is_full(xpseq_get_impl(deque->layer1_seq, 0)));
            xassert(xpseq_size(xpseq_get_impl(deque->layer1_seq, 0)) == 3);
            xassert(xpseq_is_full(xpseq_get_impl(deque->layer1_seq, 1)));
            xassert(xpseq_size(xpseq_get_impl(deque->layer1_seq, 1)) == 3);
            xassert(xpseq_is_full(xpseq_get_impl(deque->layer1_seq, 2)));
            xassert(xpseq_size(xpseq_get_impl(deque->layer1_seq, 2)) == 3);
            xassert(xpseq_size(xpseq_get_impl(deque->layer1_seq, 3)) == 1);

            xassert(strcmp((char*)xdeque_get(deque, 0), "a1") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 1), "a2") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 2), "a3") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 3), "a4") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 4), "a5") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 5), "a6") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 6), "a7") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 7), "a8") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 8), "a9") == 0);
            xassert(strcmp((char*)xdeque_get(deque, 9), "a10") == 0);

            xdeque_free(&deque);
        }
    }

    /* xdeque_pop_back */
    {
        /* capacity != 0 */
        {
            XDeque_PT deque = xdeque_new(10);

            xdeque_push_back(deque, "a1");
            xdeque_push_back(deque, "a2");
            xdeque_push_back(deque, "a3");
            xdeque_push_back(deque, "a4");
            xdeque_push_back(deque, "a5");

            xassert(deque->size == 5);
            xassert(deque->layer1_seq->size == 5);
            xassert(strcmp((char*)xdeque_pop_back(deque), "a5") == 0);
            xassert(deque->size == 4);
            xassert(deque->layer1_seq->size == 4);
            xassert(strcmp((char*)xdeque_pop_back(deque), "a4") == 0);
            xassert(deque->size == 3);
            xassert(deque->layer1_seq->size == 3);
            xassert(strcmp((char*)xdeque_pop_back(deque), "a3") == 0);
            xassert(deque->size == 2);
            xassert(deque->layer1_seq->size == 2);
            xassert(strcmp((char*)xdeque_pop_back(deque), "a2") == 0);
            xassert(deque->size == 1);
            xassert(deque->layer1_seq->size == 1);
            xassert(strcmp((char*)xdeque_pop_back(deque), "a1") == 0);
            xassert(deque->size == 0);
            xassert(deque->layer1_seq->size == 0);

            xdeque_free(&deque);
        }

        /* capacity == 0 indirectly */
        {
            XDeque_PT deque = xdeque_new(3);

            xdeque_push_back(deque, "a1");
            xdeque_push_back(deque, "a2");
            xdeque_push_back(deque, "a3");
            xdeque_push_back_no_limit(deque, "a4");
            xdeque_push_back(deque, "a5");
            xdeque_push_back(deque, "a6");
            xdeque_push_back(deque, "a7");
            xdeque_push_back(deque, "a8");
            xdeque_push_back(deque, "a9");
            xdeque_push_back(deque, "a10");

            xassert(deque->size == 10);
            xassert(deque->capacity == 0);
            xassert(deque->layer1_seq->size == 4);

            xassert(strcmp((char*)xdeque_pop_back(deque), "a10") == 0);
            xassert(deque->size == 9);
            xassert(deque->layer1_seq->size == 3);
            xassert(strcmp((char*)xdeque_pop_back(deque), "a9") == 0);
            xassert(deque->size == 8);
            xassert(strcmp((char*)xdeque_pop_back(deque), "a8") == 0);
            xassert(deque->size == 7);
            xassert(strcmp((char*)xdeque_pop_back(deque), "a7") == 0);
            xassert(deque->size == 6);
            xassert(deque->layer1_seq->size == 2);
            xassert(strcmp((char*)xdeque_pop_back(deque), "a6") == 0);
            xassert(deque->size == 5);
            xassert(strcmp((char*)xdeque_pop_back(deque), "a5") == 0);
            xassert(deque->size == 4);
            xassert(strcmp((char*)xdeque_pop_back(deque), "a4") == 0);
            xassert(deque->size == 3);
            xassert(deque->layer1_seq->size == 1);
            xassert(strcmp((char*)xdeque_pop_back(deque), "a3") == 0);
            xassert(deque->size == 2);
            xassert(strcmp((char*)xdeque_pop_back(deque), "a2") == 0);
            xassert(deque->size == 1);
            xassert(strcmp((char*)xdeque_pop_back(deque), "a1") == 0);
            xassert(deque->size == 0);
            xassert(deque->layer1_seq->size == 0);

            xdeque_free(&deque);
        }

        /* capacity == 0 directly */
        {
            XDeque_PT deque = xdeque_new(0);
            for (int i = 0; i < 20000; i++) {
                if (i % 2 == 0) {
                    xdeque_push_back(deque, "a");
                }
                else {
                    xdeque_push_back(deque, "b");
                }
            }
            xassert(deque->size == 20000);
            xassert(deque->capacity == 0);

            for (int i = 0; i < 20000; i++) {
                if (i % 2 == 0) {
                    xassert(strcmp((char*)xdeque_pop_back(deque), "b") == 0);
                }
                else {
                    xassert(strcmp((char*)xdeque_pop_back(deque), "a") == 0);
                }
            }

            xdeque_free(&deque);
        }
    }

    /* xdeque_pop_front*/
    {
        /* capacity != 0 */
        {
            XDeque_PT deque = xdeque_new(10);

            xdeque_push_back(deque, "a1");
            xdeque_push_back(deque, "a2");
            xdeque_push_back(deque, "a3");
            xdeque_push_back(deque, "a4");
            xdeque_push_back(deque, "a5");

            xassert(deque->size == 5);
            xassert(strcmp((char*)xdeque_pop_front(deque), "a1") == 0);
            xassert(deque->size == 4);
            xassert(strcmp((char*)xdeque_pop_front(deque), "a2") == 0);
            xassert(deque->size == 3);
            xassert(strcmp((char*)xdeque_pop_front(deque), "a3") == 0);
            xassert(deque->size == 2);
            xassert(strcmp((char*)xdeque_pop_front(deque), "a4") == 0);
            xassert(deque->size == 1);
            xassert(strcmp((char*)xdeque_pop_front(deque), "a5") == 0);
            xassert(deque->size == 0);

            xdeque_free(&deque);
        }

        /* capacity == 0 indirectly */
        {
            XDeque_PT deque = xdeque_new(3);

            xdeque_push_back(deque, "a1");
            xdeque_push_back(deque, "a2");
            xdeque_push_back(deque, "a3");
            xdeque_push_back_no_limit(deque, "a4");
            xdeque_push_back(deque, "a5");
            xdeque_push_back(deque, "a6");
            xdeque_push_back(deque, "a7");
            xdeque_push_back(deque, "a8");
            xdeque_push_back(deque, "a9");
            xdeque_push_back(deque, "a10");

            xassert(deque->size == 10);
            xassert(deque->capacity == 0);
            xassert(deque->layer1_seq->size == 4);

            xassert(strcmp((char*)xdeque_pop_front(deque), "a1") == 0);
            xassert(deque->size == 9);
            xassert(strcmp((char*)xdeque_pop_front(deque), "a2") == 0);
            xassert(deque->size == 8);
            xassert(strcmp((char*)xdeque_pop_front(deque), "a3") == 0);
            xassert(deque->size == 7);
            xassert(deque->layer1_seq->size == 3);
            xassert(strcmp((char*)xdeque_pop_front(deque), "a4") == 0);
            xassert(deque->size == 6);
            xassert(strcmp((char*)xdeque_pop_front(deque), "a5") == 0);
            xassert(deque->size == 5);
            xassert(strcmp((char*)xdeque_pop_front(deque), "a6") == 0);
            xassert(deque->size == 4);
            xassert(deque->layer1_seq->size == 2);
            xassert(strcmp((char*)xdeque_pop_front(deque), "a7") == 0);
            xassert(deque->size == 3);
            xassert(strcmp((char*)xdeque_pop_front(deque), "a8") == 0);
            xassert(deque->size == 2);
            xassert(strcmp((char*)xdeque_pop_front(deque), "a9") == 0);
            xassert(deque->size == 1);
            xassert(deque->layer1_seq->size == 1);
            xassert(strcmp((char*)xdeque_pop_front(deque), "a10") == 0);
            xassert(deque->size == 0);
            xassert(deque->layer1_seq->size == 0);

            xdeque_free(&deque);
        }

        /* capacity == 0 directly */
        {
            XDeque_PT deque = xdeque_new(0);
            for (int i = 0; i < 20000; i++) {
                if (i % 2 == 0) {
                    xdeque_push_back(deque, "a");
                }
                else {
                    xdeque_push_back(deque, "b");
                }
            }
            xassert(deque->size == 20000);
            xassert(deque->capacity == 0);

            for (int i = 0; i < 20000; i++) {
                if (i % 2 == 0) {
                    xassert(strcmp((char*)xdeque_pop_front(deque), "a") == 0);
                }
                else {
                    xassert(strcmp((char*)xdeque_pop_front(deque), "b") == 0);
                }
            }

            xdeque_free(&deque);
        }
    }

    /* xdeque_front */
    /* xdeque_back */
    {
        /* capacity != 0 */
        {
            XDeque_PT deque = xdeque_new(5);
            xdeque_push_back(deque, "a1");
            xdeque_push_back(deque, "a2");
            xdeque_push_back(deque, "a3");
            xdeque_push_back(deque, "a4");
            xdeque_push_back(deque, "a5");

            xassert(strcmp((char*)xdeque_front(deque), "a1") == 0);
            xassert(strcmp((char*)xdeque_back(deque), "a5") == 0);
            xassert(deque->size == 5);

            xdeque_pop_front(deque);
            xassert(strcmp((char*)xdeque_front(deque), "a2") == 0);
            xassert(deque->size == 4);

            xdeque_pop_back(deque);
            xassert(strcmp((char*)xdeque_back(deque), "a4") == 0);
            xassert(deque->size == 3);

            xdeque_pop_front(deque);
            xassert(strcmp((char*)xdeque_front(deque), "a3") == 0);
            xassert(deque->size == 2);

            xdeque_pop_back(deque);
            xassert(strcmp((char*)xdeque_back(deque), "a3") == 0);
            xassert(deque->size == 1);

            xdeque_push_back(deque, "a4");
            xdeque_push_back(deque, "a5");
            xdeque_push_front(deque, "a2");
            xdeque_push_front(deque, "a1");
            xassert(deque->size == 5);

            xassert(strcmp((char*)xdeque_front(deque), "a1") == 0);
            xassert(strcmp((char*)xdeque_back(deque), "a5") == 0);

            xdeque_free(&deque);
        }

        /* capacity == 0 */
        {
            XDeque_PT deque = xdeque_new(2);
            xdeque_push_back(deque, "a1");
            xdeque_push_back(deque, "a2");
            xdeque_push_back_no_limit(deque, "a3");
            xdeque_push_back(deque, "a4");
            xdeque_push_back(deque, "a5");

            xassert(strcmp((char*)xdeque_front(deque), "a1") == 0);
            xassert(strcmp((char*)xdeque_back(deque), "a5") == 0);
            xassert(deque->size == 5);

            xdeque_pop_front(deque);
            xassert(strcmp((char*)xdeque_front(deque), "a2") == 0);
            xassert(deque->size == 4);

            xdeque_pop_back(deque);
            xassert(strcmp((char*)xdeque_back(deque), "a4") == 0);
            xassert(deque->size == 3);

            xdeque_pop_front(deque);
            xassert(strcmp((char*)xdeque_front(deque), "a3") == 0);
            xassert(deque->size == 2);

            xdeque_pop_back(deque);
            xassert(strcmp((char*)xdeque_back(deque), "a3") == 0);
            xassert(deque->size == 1);

            xdeque_push_back(deque, "a4");
            xdeque_push_back(deque, "a5");
            xdeque_push_front(deque, "a2");
            xdeque_push_front(deque, "a1");
            xassert(deque->size == 5);

            xassert(strcmp((char*)xdeque_front(deque), "a1") == 0);
            xassert(strcmp((char*)xdeque_back(deque), "a5") == 0);

            xdeque_clear(deque);
            xassert(deque->size == 0);
            xdeque_free(&deque);
        }
    }

    /* xdeque_put */
    /* xdeque_get */
    {
        /* deque is NULL */
        {
            bool except = false;
            XEXCEPT_TRY
                xdeque_put(NULL, 0, "abc", NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* i < 0 */
        {
            XDeque_PT deque = xdeque_new(10);
            bool except = false;
            XEXCEPT_TRY
                xdeque_put(deque, -1, "abc", NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xdeque_free(&deque);
        }

        /* size < i */
        {
            XDeque_PT deque = xdeque_new(10);
            xdeque_push_back(deque, "a");
            xdeque_push_back(deque, "a");

            bool except = false;
            XEXCEPT_TRY
                xdeque_put(deque, 2, "abc", NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xdeque_free(&deque);
        }

        /* capacity != 0 */
        {
            XDeque_PT deque = xdeque_new(5);
            xdeque_push_back(deque, "a1");
            xdeque_push_back(deque, "a2");
            xdeque_push_back(deque, "a3");
            xdeque_push_back(deque, "a4");
            xdeque_push_back(deque, "a5");

            void *old_x = NULL;
            xdeque_put(deque, 0, "a11", &old_x);
            xassert(strcmp((char*)xdeque_get(deque, 0), "a11") == 0);
            xassert(strcmp((char*)old_x, "a1") == 0);
            xassert(deque->size == 5);

            xdeque_put(deque, 1, "a21", &old_x);
            xassert(strcmp((char*)xdeque_get(deque, 1), "a21") == 0);
            xassert(strcmp((char*)old_x, "a2") == 0);
            xassert(deque->size == 5);

            xdeque_put(deque, 2, "a31", &old_x);
            xassert(strcmp((char*)xdeque_get(deque, 2), "a31") == 0);
            xassert(strcmp((char*)old_x, "a3") == 0);
            xassert(deque->size == 5);

            xdeque_put(deque, 3, "a41", &old_x);
            xassert(strcmp((char*)xdeque_get(deque, 3), "a41") == 0);
            xassert(strcmp((char*)old_x, "a4") == 0);
            xassert(deque->size == 5);

            xdeque_put(deque, 4, "a51", &old_x);
            xassert(strcmp((char*)xdeque_get(deque, 4), "a51") == 0);
            xassert(strcmp((char*)old_x, "a5") == 0);
            xassert(deque->size == 5);

            xdeque_clear(deque);
            xassert(deque->size == 0);
            xdeque_free(&deque);
        }

        /* capacity == 0 */
        {
            XDeque_PT deque = xdeque_new(2);
            xdeque_push_back(deque, "a1");
            xdeque_push_back(deque, "a2");
            xdeque_push_back_no_limit(deque, "a3");
            xdeque_push_back(deque, "a4");
            xdeque_push_back(deque, "a5");
            xdeque_push_back(deque, "a6");
            xdeque_push_back(deque, "a7");
            xdeque_push_back(deque, "a8");
            xdeque_push_back(deque, "a9");
            xdeque_push_back(deque, "a10");

            xassert(deque->capacity == 0);

            void *old_x = NULL;
            xdeque_put(deque, 0, "a11", &old_x);
            xassert(strcmp((char*)xdeque_get(deque, 0), "a11") == 0);
            xassert(strcmp((char*)old_x, "a1") == 0);
            xassert(deque->size == 10);

            xdeque_put(deque, 1, "a21", &old_x);
            xassert(strcmp((char*)xdeque_get(deque, 1), "a21") == 0);
            xassert(strcmp((char*)old_x, "a2") == 0);
            xassert(deque->size == 10);

            xdeque_put(deque, 2, "a31", &old_x);
            xassert(strcmp((char*)xdeque_get(deque, 2), "a31") == 0);
            xassert(strcmp((char*)old_x, "a3") == 0);
            xassert(deque->size == 10);

            xdeque_put(deque, 3, "a41", &old_x);
            xassert(strcmp((char*)xdeque_get(deque, 3), "a41") == 0);
            xassert(strcmp((char*)old_x, "a4") == 0);
            xassert(deque->size == 10);

            xdeque_put(deque, 4, "a51", &old_x);
            xassert(strcmp((char*)xdeque_get(deque, 4), "a51") == 0);
            xassert(strcmp((char*)old_x, "a5") == 0);
            xassert(deque->size == 10);

            xdeque_put(deque, 5, "a61", &old_x);
            xassert(strcmp((char*)xdeque_get(deque, 5), "a61") == 0);
            xassert(strcmp((char*)old_x, "a6") == 0);
            xassert(deque->size == 10);

            xdeque_put(deque, 6, "a71", &old_x);
            xassert(strcmp((char*)xdeque_get(deque, 6), "a71") == 0);
            xassert(strcmp((char*)old_x, "a7") == 0);
            xassert(deque->size == 10);

            xdeque_put(deque, 7, "a81", &old_x);
            xassert(strcmp((char*)xdeque_get(deque, 7), "a81") == 0);
            xassert(strcmp((char*)old_x, "a8") == 0);
            xassert(deque->size == 10);

            xdeque_put(deque, 8, "a91", &old_x);
            xassert(strcmp((char*)xdeque_get(deque, 8), "a91") == 0);
            xassert(strcmp((char*)old_x, "a9") == 0);
            xassert(deque->size == 10);

            xdeque_put(deque, 9, "a101", &old_x);
            xassert(strcmp((char*)xdeque_get(deque, 9), "a101") == 0);
            xassert(strcmp((char*)old_x, "a10") == 0);
            xassert(deque->size == 10);

            xdeque_clear(deque);
            xassert(deque->size == 0);
            xdeque_free(&deque);
        }
    }

    /* xdeque_get */
    {
        /* deque is NULL */
        {
            bool except = false;
            XEXCEPT_TRY
                xdeque_get(NULL, 0);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* i < 0 */
        {
            XDeque_PT deque = xdeque_new(10);
            bool except = false;
            XEXCEPT_TRY
                xdeque_get(deque, -1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xdeque_free(&deque);
        }

        /* deque->array->size < i */
        {
            XDeque_PT deque = xdeque_new(10);
            bool except = false;
            XEXCEPT_TRY
                xdeque_get(deque, 20);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xdeque_free(&deque);
        }

        /* 0 < capacity, capacity <= i */
        {
            XDeque_PT deque = xdeque_new(10);
            bool except = false;
            XEXCEPT_TRY
                xdeque_get(deque, 10);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xdeque_free(&deque);
        }
    }

    /* xdeque_map */
    /* xdeque_map_break_if_true */
    /* xdeque_map_break_if_false */

    /* xdeque_free */
    /* xdeque_deep_free */
    /* xdeque_clear */
    /* xdeque_deep_clear */

    /* xdeque_size */
    /* xdeque_is_empty */

    /* xdeque_swap */

    /* xdeque_quick_sort */
    {
        {
            XDeque_PT deque = xdeque_random_string_no_limit(1000);
            {
                void *x = xdeque_pop_front(deque);
                XMEM_FREE(x);
            }
            {
                void *x = xdeque_pop_front(deque);
                XMEM_FREE(x);
            }
            {
                void *x = xdeque_pop_front(deque);
                XMEM_FREE(x);
            }
            xassert(xdeque_quick_sort(deque, sort_compare, NULL));
            xassert(xdeque_quick_sort(deque, sort_compare, NULL));
            xassert(xdeque_quick_sort(deque, sort_compare, NULL));
            xdeque_deep_free(&deque);
        }

        {
            for (int i = 0; i < 200; ++i) {
                XDeque_PT deque = xdeque_random_string(i);
                xassert(xdeque_quick_sort(deque, sort_compare, NULL));
                xassert(xdeque_quick_sort(deque, sort_compare, NULL));
                xdeque_deep_free(&deque);
            }
        }
    }

    /* xdeque_heapify_impl */
    {
        {
            XDeque_PT deque = xdeque_random_string_no_limit(1000);
            xassert(xdeque_heapify_impl(deque, 0, 999, true, sort_compare, NULL));
            xdeque_deep_free(&deque);
        }

        {
            for (int i = 100; i < 200; ++i) {
                XDeque_PT deque = xdeque_random_string(i);
                xassert(xdeque_heapify_impl(deque, 0, i / 3, true, sort_compare, NULL));
                xassert(xdeque_heapify_impl(deque, i / 3, i / 3 * 2, true, sort_compare, NULL));
                xassert(xdeque_heapify_impl(deque, i / 3 * 2, deque->size - 1, true, sort_compare, NULL));
                xdeque_deep_free(&deque);
            }
        }

        {
            XDeque_PT deque = xdeque_random_string_no_limit(1000);
            xassert(xdeque_heapify_impl(deque, 0, 999, false, sort_compare, NULL));
            xdeque_deep_free(&deque);
        }

        {
            for (int i = 100; i < 200; ++i) {
                XDeque_PT deque = xdeque_random_string(i);
                xassert(xdeque_heapify_impl(deque, 0, i / 3, false, sort_compare, NULL));
                xassert(xdeque_heapify_impl(deque, i / 3, i / 3 * 2, false, sort_compare, NULL));
                xassert(xdeque_heapify_impl(deque, i / 3 * 2, deque->size - 1, false, sort_compare, NULL));
                xdeque_deep_free(&deque);
            }
        }
    }

    /* xdeque_heapify_min */
    {
        {
            XDeque_PT deque = xdeque_random_string_no_limit(1000);
            xassert(xdeque_heapify_min(deque, sort_compare, NULL));
            xdeque_deep_free(&deque);
        }

        {
            for (int i = 0; i < 200; ++i) {
                XDeque_PT deque = xdeque_random_string(i);
                xassert(xdeque_heapify_min(deque, sort_compare, NULL));
                xdeque_deep_free(&deque);
            }
        }
    }

    /* xdeque_heapify_max */
    {
        {
            XDeque_PT deque = xdeque_random_string_no_limit(1000);
            xassert(xdeque_heapify_max(deque, sort_compare, NULL));
            xdeque_deep_free(&deque);
        }

        {
            for (int i = 0; i < 200; ++i) {
                XDeque_PT deque = xdeque_random_string(i);
                xassert(xdeque_heapify_max(deque, sort_compare, NULL));
                xdeque_deep_free(&deque);
            }
        }
    }

    /* xdeque_heap_sort_impl */
    {
        for (int i = 100; i < 200; ++i) {
            XDeque_PT deque = xdeque_random_string_no_limit(i);
            xassert(xdeque_heap_sort_impl(deque, 0, i / 3, true, sort_compare, NULL));
            xassert(xdeque_heap_sort_impl(deque, i / 3, i / 3 * 2, true, sort_compare, NULL));
            xassert(xdeque_heap_sort_impl(deque, i / 3 * 2, deque->size - 1, true, sort_compare, NULL));
            xdeque_deep_free(&deque);
        }

        for (int i = 100; i < 200; ++i) {
            XDeque_PT deque = xdeque_random_string(i);
            xassert(xdeque_heap_sort_impl(deque, 0, i / 3, false, sort_compare, NULL));
            xassert(xdeque_heap_sort_impl(deque, i / 3, i / 3 * 2, false, sort_compare, NULL));
            xassert(xdeque_heap_sort_impl(deque, i / 3 * 2, deque->size - 1, false, sort_compare, NULL));
            xdeque_deep_free(&deque);
        }
    }

    /* xdeque_heap_sort */
    {
        {
            XDeque_PT deque = xdeque_random_string_no_limit(1000);
            {
                void *x = xdeque_pop_front(deque);
                XMEM_FREE(x);
            }
            {
                void *x = xdeque_pop_front(deque);
                XMEM_FREE(x);
            }
            {
                void *x = xdeque_pop_front(deque);
                XMEM_FREE(x);
            }
            xassert(xdeque_heap_sort(deque, sort_compare, NULL));
            //xdeque_map(deque, deque_print, NULL);
            xassert(xdeque_heap_sort(deque, sort_compare, NULL));
            xassert(xdeque_heap_sort(deque, sort_compare, NULL));
            xdeque_deep_free(&deque);
        }

        {
            for (int i = 0; i < 200; ++i) {
                XDeque_PT deque = xdeque_random_string(i);
                xassert(xdeque_heap_sort(deque, sort_compare, NULL));
                xassert(xdeque_heap_sort(deque, sort_compare, NULL));
                xdeque_deep_free(&deque);
            }
        }
    }

    /* xdeque_vmultiway_sort */
    {
        XDeque_PT deque1 = xdeque_random_string_no_limit(1000);
        XDeque_PT deque2 = xdeque_random_string_no_limit(1000);
        XDeque_PT deque3 = xdeque_random_string_no_limit(1000);

        XDeque_PT deque = xdeque_new(5000);

        xassert(xdeque_vmultiway_sort(deque, sort_compare, NULL, deque1, deque2, deque3, NULL));

        xdeque_deep_free(&deque1);
        xdeque_deep_free(&deque2);
        xdeque_deep_free(&deque3);
        xdeque_deep_free(&deque);
    }

    /* xdeque_amultiway_sort */
    {
        XDeque_PT deque1 = xdeque_random_string(10);
        XDeque_PT deque2 = xdeque_random_string(10);
        XDeque_PT deque3 = xdeque_random_string(10);

        XDeque_PT deque = xdeque_new(50);
        XPArray_PT array = xparray_new(10);
        xparray_put(array, 0, (void*)deque1, NULL);
        xparray_put(array, 1, (void*)deque2, NULL);
        xparray_put(array, 2, (void*)deque3, NULL);

        xassert(xdeque_amultiway_sort(deque, sort_compare, NULL, array));

        xparray_free(&array);
        xdeque_deep_free(&deque1);
        xdeque_deep_free(&deque2);
        xdeque_deep_free(&deque3);
        xdeque_deep_free(&deque);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}


