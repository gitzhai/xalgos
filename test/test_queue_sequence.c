
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/xutils.h"
#include "../array_pointer/xarray_pointer_x.h"
#include "../queue_sequence/xqueue_sequence_x.h"
#include "../include/xalgos.h"

static
bool apply_true(void *x, void *cl) {
    return true;
}

static
bool apply_false(void *x, void *cl) {
    return false;
}

static
bool apply_compare(void *x, void *cl) {
    return strcmp((char*)x, (char*)cl) == 0;
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
XPSeq_PT xpseq_random_string(int size) {
    XPSeq_PT seq = xpseq_new(size);
    if (!seq) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xpseq_deep_free(&seq);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            str[str_size - 1] = '\0';

            xpseq_push_back(seq, str);
        }
    }

    return seq;
}

void test_xpseq() {
    /* xpseq_new */
    {
        /* capacity == 0 */
        {
            bool except = false;

            XEXCEPT_TRY
                XPSeq_PT seq = xpseq_new(0);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* capacity < 0 */
        {
            bool except = false;

            XEXCEPT_TRY
                XPSeq_PT seq = xpseq_new(-1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* 0 < capacity */
        {
            XPSeq_PT seq = xpseq_new(10);

            xassert(seq->size == 0);
            xassert(seq->head == 0);
            xassert(xparray_size(seq->array) == 10);

            xpseq_free(&seq);
        }
    }

    /* xpseq_copy */
    {
        /* sequence is NULL */
        {
            bool except = false;

            XEXCEPT_TRY
                XPSeq_PT seq = xpseq_copy(NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }
    }

    /* xpseq_copyn */
    {
        /* sequence is NULL */
        {
            bool except = false;

            XEXCEPT_TRY
                XPSeq_PT seq = xpseq_copyn(NULL, 10);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* count < 0 */
        {
            XPSeq_PT seq = xpseq_new(10);
            bool except = false;

            XEXCEPT_TRY
                XPSeq_PT seq = xpseq_copyn(seq, -1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xpseq_free(&seq);
        }

        /* sequence is empty, 0 < count */
        {
            XPSeq_PT seq = xpseq_new(20);
            XPSeq_PT nseq = xpseq_copyn(seq, 10);

            xassert(nseq->size == 0);
            xassert(nseq->head == 0);

            xpseq_free(&seq);
            xpseq_free(&nseq);
        }

        /* sequence is not empty, count == 0, head == 0 */
        {
            XPSeq_PT seq = xpseq_new(10);
            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "def");
            xpseq_push_back(seq, "xyz");

            XPSeq_PT nseq = xpseq_copyn(seq, 0);

            xassert(nseq->size == 0);
            xassert(nseq->head == 0);
            
            xpseq_free(&seq);
            xpseq_free(&nseq);
        }

        /* sequence is not empty, count == 0, 0 < head */
        {
            XPSeq_PT seq = xpseq_new(10);
            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "def");
            xpseq_push_back(seq, "xyz");
            xpseq_pop_front(seq);

            XPSeq_PT nseq = xpseq_copyn(seq, 0);

            xassert(nseq->size == 0);
            xassert(nseq->head == 0);
            
            xpseq_free(&seq);
            xpseq_free(&nseq);
        }

        /* sequence is not empty, 0 < count <= size, head == 0 */
        {
            XPSeq_PT seq = xpseq_new(10);
            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "def");
            xpseq_push_back(seq, "xyz");

            XPSeq_PT nseq = xpseq_copyn(seq, 2);

            xassert(nseq->size == 2);
            xassert(nseq->head == 0);

            xassert(strcmp((char*)xpseq_get(nseq, 0), "abc") == 0);
            xassert(strcmp((char*)xpseq_get(nseq, 1), "def") == 0);

            xpseq_free(&seq);
            xpseq_free(&nseq);
        }

        /* sequence is not empty, 0 < count <= size, 0 < head, count <= (size - head + 1) */
        {
            XPSeq_PT seq = xpseq_new(10);
            xpseq_push_back(seq, "hij");
            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "def");
            xpseq_push_back(seq, "xyz");
            xpseq_pop_front(seq);

            XPSeq_PT nseq = xpseq_copyn(seq, 2);

            xassert(nseq->size == 2);
            xassert(nseq->head == 0);

            xassert(strcmp((char*)xpseq_get(nseq, 0), "abc") == 0);
            xassert(strcmp((char*)xpseq_get(nseq, 1), "def") == 0);

            xpseq_free(&seq);
            xpseq_free(&nseq);
        }

        /* sequence is not empty, 0 < count <= size, 0 < head, (size - head + 1) < count */
        {
            XPSeq_PT seq = xpseq_new(5);
            xpseq_push_back(seq, "hij");
            xpseq_push_back(seq, "hij");
            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "def");
            xpseq_push_back(seq, "xyz");
            xpseq_pop_front(seq);
            xpseq_pop_front(seq);
            xpseq_push_back(seq, "hij");

            XPSeq_PT nseq = xpseq_copyn(seq, 4);

            xassert(nseq->size == 4);
            xassert(nseq->head == 0);

            xassert(strcmp((char*)xpseq_get(nseq, 0), "abc") == 0);
            xassert(strcmp((char*)xpseq_get(nseq, 1), "def") == 0);
            xassert(strcmp((char*)xpseq_get(nseq, 2), "xyz") == 0);
            xassert(strcmp((char*)xpseq_get(nseq, 3), "hij") == 0);

            xpseq_free(&seq);
            xpseq_free(&nseq);
        }
    }

    /* xpseq_deep_copy */
    {
        /* sequence is NULL */
        {
            bool except = false;

            XEXCEPT_TRY
                XPSeq_PT seq = xpseq_deep_copy(NULL, 4);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }
    }

    /* xpseq_deep_copyn */
    {
        /* sequence is NULL */
        {
            bool except = false;

            XEXCEPT_TRY
                XPSeq_PT seq = xpseq_deep_copyn(NULL, 10, 4);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* count < 0 */
        {
            XPSeq_PT seq = xpseq_new(10);
            bool except = false;

            XEXCEPT_TRY
                XPSeq_PT seq = xpseq_deep_copyn(seq, -1, 4);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xpseq_free(&seq);
        }

        /* elem_size == 0 */
        {
            XPSeq_PT seq = xpseq_new(10);
            bool except = false;

            XEXCEPT_TRY
                XPSeq_PT seq = xpseq_deep_copyn(seq, 10, 0);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xpseq_free(&seq);
        }

        /* elem_size < 0 */
        {
            XPSeq_PT seq = xpseq_new(10);
            bool except = false;

            XEXCEPT_TRY
                XPSeq_PT seq = xpseq_deep_copyn(seq, 10, -1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xpseq_free(&seq);
        }

        /* sequence is empty, count == 0 */
        {
            XPSeq_PT seq = xpseq_new(10);
            XPSeq_PT nseq = xpseq_deep_copyn(seq, 0, 4);

            xassert(nseq->size == 0);
            xassert(nseq->head == 0);

            xpseq_free(&seq);
            xpseq_deep_free(&nseq);
        }

        /* sequence is empty, 0 < count */
        {
            XPSeq_PT seq = xpseq_new(20);
            XPSeq_PT nseq = xpseq_deep_copyn(seq, 10, 4);

            xassert(nseq->size == 0);
            xassert(nseq->head == 0);

            xpseq_free(&seq);
            xpseq_deep_free(&nseq);
        }

        /* sequence is not empty, count == 0, head == 0 */
        {
            XPSeq_PT seq = xpseq_new(10);
            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "def");
            xpseq_push_back(seq, "xyz");

            XPSeq_PT nseq = xpseq_deep_copyn(seq, 0, 4);

            xassert(nseq->size == 0);
            xassert(nseq->head == 0);

            xpseq_free(&seq);
            xpseq_deep_free(&nseq);
        }

        /* sequence is not empty, count == 0, 0 < head */
        {
            XPSeq_PT seq = xpseq_new(10);
            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "def");
            xpseq_push_back(seq, "xyz");
            xpseq_pop_front(seq);

            XPSeq_PT nseq = xpseq_deep_copyn(seq, 0, 4);

            xassert(nseq->size == 0);
            xassert(nseq->head == 0);

            xpseq_free(&seq);
            xpseq_deep_free(&nseq);
        }

        /* sequence is not empty, 0 < count <= size, head == 0 */
        {
            XPSeq_PT seq = xpseq_new(10);
            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "def");
            xpseq_push_back(seq, "xyz");

            XPSeq_PT nseq = xpseq_deep_copyn(seq, 2, 4);

            xassert(nseq->size == 2);
            xassert(nseq->head == 0);

            xassert(strcmp((char*)xpseq_get(nseq, 0), "abc") == 0);
            xassert(strcmp((char*)xpseq_get(nseq, 1), "def") == 0);

            xpseq_free(&seq);
            xpseq_deep_free(&nseq);
        }

        /* sequence is not empty, 0 < count <= size, 0 < head, count <= (size - head + 1) */
        {
            XPSeq_PT seq = xpseq_new(10);
            xpseq_push_back(seq, "hij");
            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "def");
            xpseq_push_back(seq, "xyz");
            xpseq_pop_front(seq);

            XPSeq_PT nseq = xpseq_deep_copyn(seq, 2, 4);

            xassert(nseq->size == 2);
            xassert(nseq->head == 0);

            xassert(strcmp((char*)xpseq_get(nseq, 0), "abc") == 0);
            xassert(strcmp((char*)xpseq_get(nseq, 1), "def") == 0);

            xpseq_free(&seq);
            xpseq_deep_free(&nseq);
        }

        /* sequence is not empty, 0 < count <= size, 0 < head, (size - head + 1) < count */
        {
            XPSeq_PT seq = xpseq_new(5);
            xpseq_push_back(seq, "hij");
            xpseq_push_back(seq, "hij");
            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "def");
            xpseq_push_back(seq, "xyz");
            xpseq_pop_front(seq);
            xpseq_pop_front(seq);
            xpseq_push_back(seq, "hij");

            XPSeq_PT nseq = xpseq_deep_copyn(seq, 4, 4);

            xassert(nseq->size == 4);
            xassert(nseq->head == 0);

            xassert(strcmp((char*)xpseq_get(nseq, 0), "abc") == 0);
            xassert(strcmp((char*)xpseq_get(nseq, 1), "def") == 0);
            xassert(strcmp((char*)xpseq_get(nseq, 2), "xyz") == 0);
            xassert(strcmp((char*)xpseq_get(nseq, 3), "hij") == 0);

            xpseq_free(&seq);
            xpseq_deep_free(&nseq);
        }
    }

    /* xpseq_vload */
    {
        /* seq is NULL */
        {
            bool except = false;
            XEXCEPT_TRY
                xpseq_vload(NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        /* 0 < size */
        {
            XPSeq_PT seq = xpseq_new(10);
            xpseq_push_back(seq, "abc");
            xpseq_vload(seq, "def", NULL);
            xpseq_free(&seq);
        }

        /* x == NULL */
        {
            XPSeq_PT seq = xpseq_new(10);
            xpseq_vload(seq, NULL);
            xassert(seq->size == 0);
            xpseq_free(&seq);
        }

        /* load multiple values */
        {
            XPSeq_PT seq = xpseq_new(10);
            xassert(xpseq_vload(seq, "abc", "def", "xyz", NULL) == 3);

            xassert(seq->size == 3);
            xassert(strcmp((char*)xpseq_get(seq, 0), "abc") == 0);
            xassert(strcmp((char*)xpseq_get(seq, 1), "def") == 0);
            xassert(strcmp((char*)xpseq_get(seq, 2), "xyz") == 0);

            xpseq_free(&seq);
        }
    }

    /* xpseq_aload */
    {
        /* seq is NULL */
        {
            XPArray_PT array = xparray_new(10);
            bool except = false;
            XEXCEPT_TRY
                xpseq_aload(NULL, array);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xparray_free(&array);
        }

        /* 0 < size */
        {
            XPSeq_PT seq = xpseq_new(10);
            xpseq_push_back(seq, "abc");
            XPArray_PT array = xparray_new(10);
            xpseq_aload(seq, array);
            xpseq_free(&seq);
            xparray_free(&array);
        }

        /* xs == NULL */
        {
            XPSeq_PT seq = xpseq_new(10);

            bool except = false;
            XEXCEPT_TRY
                xpseq_aload(seq, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xpseq_free(&seq);
        }

        /* xs is empty */
        {
            XPSeq_PT seq = xpseq_new(10);
            XPArray_PT array = xparray_new(10);

            xassert(xpseq_aload(seq, array) == 0);

            xassert(seq->size == 0);

            xpseq_free(&seq);
            xparray_free(&array);
        }

        /* capacity < xs size */
        {
            XPSeq_PT seq = xpseq_new(2);
            XPArray_PT array = xparray_new(3);
            xparray_vload(array, "abc", "def", "xyz", NULL);

            xassert(xpseq_aload(seq, array) == 2);

            xassert(seq->size == 2);
            xassert(strcmp((char*)xpseq_get(seq, 0), "abc") == 0);
            xassert(strcmp((char*)xpseq_get(seq, 1), "def") == 0);

            xpseq_free(&seq);
            xparray_free(&array);
        }

        /* xs size < capacity */
        {
            XPSeq_PT seq = xpseq_new(10);
            XPArray_PT array = xparray_new(3);
            xparray_vload(array, "abc", "def", "xyz", NULL);

            xassert(xpseq_aload(seq, array) == 3);

            xassert(seq->size == 3);
            xassert(strcmp((char*)xpseq_get(seq, 0), "abc") == 0);
            xassert(strcmp((char*)xpseq_get(seq, 1), "def") == 0);
            xassert(strcmp((char*)xpseq_get(seq, 2), "xyz") == 0);

            xpseq_free(&seq);
            xparray_free(&array);
        }
    }

    /* xpseq_push_front */
    {
        XPSeq_PT seq = xpseq_new(7);

        xpseq_push_front(seq, "a5");
        xassert(seq->size == 1);
        xassert(seq->head == 6);

        xpseq_push_front(seq, "a4");
        xassert(seq->size == 2);
        xassert(seq->head == 5);

        xpseq_push_front(seq, "a3");
        xassert(seq->size == 3);
        xassert(seq->head == 4);

        xpseq_push_back(seq, "a6");
        xassert(seq->size == 4);
        xassert(seq->head == 4);

        xpseq_push_back(seq, "a7");
        xassert(seq->size == 5);
        xassert(seq->head == 4);

        xpseq_push_front(seq, "a2");
        xassert(seq->size == 6);
        xassert(seq->head == 3);

        xpseq_push_front(seq, "a1");
        xassert(seq->size == 7);
        xassert(seq->head == 2);

        xassert_false(xpseq_push_front(seq, "a8"));
        xassert(seq->size == 7);
        xassert(seq->head == 2);

        xassert(strcmp((char*)xpseq_get_impl(seq, 0), "a1") == 0);
        xassert(strcmp((char*)xpseq_get_impl(seq, 1), "a2") == 0);
        xassert(strcmp((char*)xpseq_get_impl(seq, 2), "a3") == 0);
        xassert(strcmp((char*)xpseq_get_impl(seq, 3), "a4") == 0);
        xassert(strcmp((char*)xpseq_get_impl(seq, 4), "a5") == 0);
        xassert(strcmp((char*)xpseq_get_impl(seq, 5), "a6") == 0);
        xassert(strcmp((char*)xpseq_get_impl(seq, 6), "a7") == 0);

        xassert(strcmp((char*)xpseq_pop_front(seq), "a1") == 0);
        xassert(seq->size == 6);
        xassert(seq->head == 3);
        xassert(strcmp((char*)xpseq_pop_back(seq), "a7") == 0);
        xassert(seq->size == 5);
        xassert(seq->head == 3);
        xassert(strcmp((char*)xpseq_pop_front(seq), "a2") == 0);
        xassert(seq->size == 4);
        xassert(seq->head == 4);
        xassert(strcmp((char*)xpseq_pop_back(seq), "a6") == 0);
        xassert(seq->size == 3);
        xassert(seq->head == 4);
        xassert(strcmp((char*)xpseq_pop_front(seq), "a3") == 0);
        xassert(seq->size == 2);
        xassert(seq->head == 5);
        xassert(strcmp((char*)xpseq_pop_back(seq), "a5") == 0);
        xassert(seq->size == 1);
        xassert(seq->head == 5);
        xassert(strcmp((char*)xpseq_pop_front(seq), "a4") == 0);
        xassert(seq->size == 0);
        xassert(seq->head == 6);

        xpseq_free(&seq);
    }

    /* xpseq_pop_front*/
    {
        /* head == 0 */
        {
            XPSeq_PT seq = xpseq_new(5);

            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "def");
            xpseq_push_back(seq, "hij");
            xpseq_push_back(seq, "uvw");
            xpseq_push_back(seq, "xyz");

            xassert(strcmp((char*)xpseq_pop_front(seq), "abc") == 0);
            xassert(seq->size == 4);
            xassert(seq->head == 1);

            xassert(strcmp((char*)xpseq_pop_front(seq), "def") == 0);
            xassert(seq->size == 3);
            xassert(seq->head == 2);

            xassert(strcmp((char*)xpseq_pop_front(seq), "hij") == 0);
            xassert(seq->size == 2);
            xassert(seq->head == 3);

            xpseq_free(&seq);
        }

        /* 0 < head */
        {
            XPSeq_PT seq = xpseq_new(5);

            xpseq_push_front(seq, "abc");
            xpseq_push_front(seq, "def");
            xpseq_push_front(seq, "hij");

            xassert(strcmp((char*)xpseq_pop_front(seq), "hij") == 0);
            xassert(seq->size == 2);
            xassert(strcmp((char*)xpseq_pop_front(seq), "def") == 0);
            xassert(seq->size == 1);
            xassert(strcmp((char*)xpseq_pop_front(seq), "abc") == 0);
            xassert(seq->size == 0);

            xpseq_free(&seq);
        }
    }

    /* xpseq_push_back */
    {
        XPSeq_PT seq = xpseq_new(7);

        xpseq_push_back(seq, "a3");
        xassert(seq->size == 1);
        xassert(seq->head == 0);

        xpseq_push_back(seq, "a4");
        xassert(seq->size == 2);
        xassert(seq->head == 0);

        xpseq_push_back(seq, "a5");
        xassert(seq->size == 3);
        xassert(seq->head == 0);

        xpseq_push_front(seq, "a2");
        xassert(seq->size == 4);
        xassert(seq->head == 6);

        xpseq_push_front(seq, "a1");
        xassert(seq->size == 5);
        xassert(seq->head == 5);

        xpseq_push_back(seq, "a6");
        xassert(seq->size == 6);
        xassert(seq->head == 5);

        xpseq_push_back(seq, "a7");
        xassert(seq->size == 7);
        xassert(seq->head == 5);

        xassert_false(xpseq_push_front(seq, "a8"));
        xassert(seq->size == 7);
        xassert(seq->head == 5);

        xassert(strcmp((char*)xpseq_get_impl(seq, 0), "a1") == 0);
        xassert(strcmp((char*)xpseq_get_impl(seq, 1), "a2") == 0);
        xassert(strcmp((char*)xpseq_get_impl(seq, 2), "a3") == 0);
        xassert(strcmp((char*)xpseq_get_impl(seq, 3), "a4") == 0);
        xassert(strcmp((char*)xpseq_get_impl(seq, 4), "a5") == 0);
        xassert(strcmp((char*)xpseq_get_impl(seq, 5), "a6") == 0);
        xassert(strcmp((char*)xpseq_get_impl(seq, 6), "a7") == 0);

        xassert(strcmp((char*)xpseq_pop_front(seq), "a1") == 0);
        xassert(seq->size == 6);
        xassert(seq->head == 6);
        xassert(strcmp((char*)xpseq_pop_back(seq), "a7") == 0);
        xassert(seq->size == 5);
        xassert(seq->head == 6);
        xassert(strcmp((char*)xpseq_pop_front(seq), "a2") == 0);
        xassert(seq->size == 4);
        xassert(seq->head == 0);
        xassert(strcmp((char*)xpseq_pop_back(seq), "a6") == 0);
        xassert(seq->size == 3);
        xassert(seq->head == 0);
        xassert(strcmp((char*)xpseq_pop_front(seq), "a3") == 0);
        xassert(seq->size == 2);
        xassert(seq->head == 1);
        xassert(strcmp((char*)xpseq_pop_back(seq), "a5") == 0);
        xassert(seq->size == 1);
        xassert(seq->head == 1);
        xassert(strcmp((char*)xpseq_pop_front(seq), "a4") == 0);
        xassert(seq->size == 0);
        xassert(seq->head == 2);

        xpseq_free(&seq);
    }

    /* xpseq_pop_back */
    {
        /* head == 0 */
        {
            XPSeq_PT seq = xpseq_new(5);

            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "def");
            xpseq_push_back(seq, "hij");
            xpseq_push_back(seq, "uvw");
            xpseq_push_back(seq, "xyz");

            xassert(strcmp((char*)xpseq_pop_back(seq), "xyz") == 0);
            xassert(strcmp((char*)xpseq_pop_back(seq), "uvw") == 0);
            xassert(strcmp((char*)xpseq_pop_back(seq), "hij") == 0);

            xpseq_free(&seq);
        }

        /* 0 < head */
        {
            XPSeq_PT seq = xpseq_new(5);

            xpseq_push_front(seq, "abc");
            xpseq_push_front(seq, "def");
            xpseq_push_front(seq, "hij");

            xassert(strcmp((char*)xpseq_pop_back(seq), "abc") == 0);
            xassert(strcmp((char*)xpseq_pop_back(seq), "def") == 0);
            xassert(strcmp((char*)xpseq_pop_back(seq), "hij") == 0);

            xpseq_free(&seq);
        }
    }

    /* xpseq_front */
    {
        xassert_false(xpseq_front(NULL));

        {
            XPSeq_PT seq = xpseq_new(5);
            xassert_false(xpseq_front(NULL));
            xpseq_free(&seq);
        }

        {
            XPSeq_PT seq = xpseq_new(5);
            xpseq_push_back(seq, "hij");
            xpseq_push_back(seq, "hij");
            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "def");
            xpseq_push_back(seq, "xyz");
            xpseq_pop_front(seq);
            xpseq_pop_front(seq);
            xpseq_push_back(seq, "hij");

            xassert(strcmp((char*)xpseq_front(seq), "abc") == 0);

            xpseq_free(&seq);
        }
    }

    /* xpseq_back */
    {
        xassert_false(xpseq_back(NULL));

        {
            XPSeq_PT seq = xpseq_new(5);
            xassert_false(xpseq_back(NULL));
            xpseq_free(&seq);
        }

        {
            XPSeq_PT seq = xpseq_new(5);
            xpseq_push_back(seq, "hij");
            xpseq_push_back(seq, "hij");
            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "def");
            xpseq_push_back(seq, "xyz");
            xpseq_pop_front(seq);
            xpseq_pop_front(seq);

            xassert(strcmp((char*)xpseq_back(seq), "xyz") == 0);

            xpseq_push_back(seq, "hij");

            xassert(strcmp((char*)xpseq_back(seq), "hij") == 0);

            xpseq_free(&seq);
        }
    }

    /* xpseq_put */
    {
        /* seq is NULL */
        {
            bool except = false;
            XEXCEPT_TRY
                xpseq_put(NULL, 0, "abc", NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* i < 0 */
        {
            XPSeq_PT seq = xpseq_new(10);
            bool except = false;
            XEXCEPT_TRY
                xpseq_put(seq, -1, "abc", NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xpseq_free(&seq);
        }

        /* seq->array->size == i */
        {
            XPSeq_PT seq = xpseq_new(10);
            bool except = false;
            XEXCEPT_TRY
                xpseq_put(seq, 10, "abc", NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xpseq_free(&seq);
        }

        /* seq->array->size < i */
        {
            XPSeq_PT seq = xpseq_new(10);
            bool except = false;
            XEXCEPT_TRY
                xpseq_put(seq, 11, "abc", NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xpseq_free(&seq);
        }

        {
            XPSeq_PT seq = xpseq_new(10000);
            xpseq_put(seq, XUTILS_SEQ_DEFAULT_LENGTH + 30, "abc", NULL);
            xpseq_put(seq, XUTILS_SEQ_DEFAULT_LENGTH + 800, "def", NULL);
            xassert(strcmp((char*)xpseq_get(seq, XUTILS_SEQ_DEFAULT_LENGTH + 30), "abc") == 0);
            xassert(strcmp((char*)xpseq_get(seq, XUTILS_SEQ_DEFAULT_LENGTH + 800), "def") == 0);
            xpseq_free(&seq);
        }

        {
            XPSeq_PT seq = xpseq_new(10000);
            xpseq_push_back(seq, "hij");
            xpseq_push_back(seq, "hij");
            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "def");
            xpseq_push_back(seq, "xyz");
            xpseq_pop_front(seq);
            xpseq_pop_front(seq);

            xpseq_put(seq, XUTILS_SEQ_DEFAULT_LENGTH + 30, "abc", NULL);
            xpseq_put(seq, XUTILS_SEQ_DEFAULT_LENGTH + 800, "def", NULL);
            xassert(strcmp((char*)xpseq_get(seq, XUTILS_SEQ_DEFAULT_LENGTH + 30), "abc") == 0);
            xassert(strcmp((char*)xpseq_get(seq, XUTILS_SEQ_DEFAULT_LENGTH + 800), "def") == 0);
            xpseq_free(&seq);
        }

        {
            XPSeq_PT seq = xpseq_new(10000);
            xpseq_push_front(seq, "hij");
            xpseq_push_front(seq, "hij");
            xpseq_push_front(seq, "abc");

            xpseq_put(seq, XUTILS_SEQ_DEFAULT_LENGTH + 30, "abc", NULL);
            xpseq_put(seq, XUTILS_SEQ_DEFAULT_LENGTH + 800, "def", NULL);
            xassert(strcmp((char*)xpseq_get(seq, XUTILS_SEQ_DEFAULT_LENGTH + 30), "abc") == 0);
            xassert(strcmp((char*)xpseq_get(seq, XUTILS_SEQ_DEFAULT_LENGTH + 800), "def") == 0);
            xpseq_free(&seq);
        }

        /* 0 < capacity, i < seq->array->size */
        {
            XPSeq_PT seq = xpseq_new(11);
            xpseq_put(seq, 10, "abc", NULL);
            xassert(strcmp((char*)xpseq_get(seq, 10), "abc") == 0);
            xassert(xparray_size(seq->array) == 11);
            xpseq_free(&seq);
        }
    }

    /* xpseq_get */
    {
        /* seq is NULL */
        {
            bool except = false;
            XEXCEPT_TRY
                xpseq_get(NULL, 0);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* i < 0 */
        {
            XPSeq_PT seq = xpseq_new(10);
            bool except = false;
            XEXCEPT_TRY
                xpseq_get(seq, -1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xpseq_free(&seq);
        }

        /* seq->array->size < i */
        {
            XPSeq_PT seq = xpseq_new(10);
            bool except = false;
            XEXCEPT_TRY
                xpseq_get(seq, 11);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xpseq_free(&seq);
        }

        /* seq->array->size == i */
        {
            XPSeq_PT seq = xpseq_new(10);
            bool except = false;
            XEXCEPT_TRY
                xpseq_get(seq, 10);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xpseq_free(&seq);
        }
    }

    /* xpseq_map */
    {
        /* seq == NULL */
        {
            bool except = false;
            XEXCEPT_TRY
                xpseq_map(NULL, apply_true, "abc");
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* apply == NULL */
        {
            XPSeq_PT seq = xpseq_new(10);
            bool except = false;
            XEXCEPT_TRY
                xpseq_map(seq, NULL, "abc");
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xpseq_free(&seq);
        }

        /* normal */
        {
            XPSeq_PT seq = xpseq_new(5);
            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "abc");
            xpseq_push_back(seq, "abc");
            xassert(xpseq_map(seq, apply_true, "xyz") == 5);
            xassert(xpseq_map(seq, apply_false, "abc") == 0);

            xpseq_put(seq, 4, "def", NULL);
            xassert(xpseq_map(seq, apply_compare, "abc") == 4);
            xassert(xpseq_map(seq, apply_compare, "def") == 1);

            xpseq_free(&seq);
        }
    }

    /* xpseq_map_break_if_true */
    {
        XPSeq_PT seq = xpseq_new(5);
        xpseq_push_back(seq, "abc");
        xpseq_push_back(seq, "abc");
        xpseq_push_back(seq, "abc");
        xpseq_push_back(seq, "abc");
        xpseq_push_back(seq, "abc");
        xpseq_put(seq, 4, "def", NULL);

        xassert(xpseq_map_break_if_true(seq, apply_compare, "def"));
        xassert_false(xpseq_map_break_if_true(seq, apply_compare, "xyz"));

        xpseq_free(&seq);
    }

    /* xpseq_map_break_if_false */
    {
        XPSeq_PT seq = xpseq_new(5);
        xpseq_push_back(seq, "abc");
        xpseq_push_back(seq, "abc");
        xpseq_push_back(seq, "abc");
        xpseq_push_back(seq, "abc");
        xpseq_push_back(seq, "abc");

        xassert(xpseq_map_break_if_false(seq, apply_compare, "def"));
        xassert_false(xpseq_map_break_if_false(seq, apply_compare, "abc"));

        xpseq_put(seq, 4, "def", NULL);
        xassert(xpseq_map_break_if_false(seq, apply_compare, "abc"));

        xpseq_free(&seq);
    }

    /* xpseq_expand */
    {
        /* |5|6|7|0|1|2|3|4|   ->   |5|6|7|-|-|-|0|1|2|3|4| */
        XPSeq_PT seq = xpseq_new(8);
        xpseq_push_front(seq, "4");
        xpseq_push_front(seq, "3");
        xpseq_push_front(seq, "2");
        xpseq_push_front(seq, "1");
        xpseq_push_front(seq, "0");
        xpseq_push_back(seq, "5");
        xpseq_push_back(seq, "6");
        xpseq_push_back(seq, "7");

        xpseq_expand(seq, 3);
        xassert(seq->size == 8);
        xassert(seq->array->size == 11);
        xassert(seq->head == 6);

        xassert(strcmp((char*)xpseq_get(seq, 0), "0") == 0);
        xassert(strcmp((char*)xpseq_get(seq, 1), "1") == 0);
        xassert(strcmp((char*)xpseq_get(seq, 2), "2") == 0);
        xassert(strcmp((char*)xpseq_get(seq, 3), "3") == 0);
        xassert(strcmp((char*)xpseq_get(seq, 4), "4") == 0);
        xassert(strcmp((char*)xpseq_get(seq, 5), "5") == 0);
        xassert(strcmp((char*)xpseq_get(seq, 6), "6") == 0);
        xassert(strcmp((char*)xpseq_get(seq, 7), "7") == 0);
        xassert_false(xpseq_get(seq, 8));
        xassert_false(xpseq_get(seq, 9));
        xassert_false(xpseq_get(seq, 10));

        xpseq_clear(seq);
        xassert(seq->size == 0);
        xassert(seq->head == 0);

        xpseq_free(&seq);
    }

    /* xpseq_free */
    /* xpseq_deep_free */
    /* xpseq_clear */
    /* xpseq_deep_clear */
    {
        XPSeq_PT seq = xpseq_random_string(10);
        void *t = xpseq_pop_front(seq);
        XMEM_FREE(t);
        t = xpseq_pop_front(seq);
        XMEM_FREE(t);
        xpseq_deep_clear(seq);
        xassert(seq->size == 0);
        xassert(seq->head == 0);
        xpseq_deep_free(&seq);
    }

    /* xpseq_size */
    /* xpseq_is_empty */
    /* xpseq_swap */

    /* xpseq_heapify_impl */
    {
        {
            XPSeq_PT seq = xpseq_random_string(1000);
            xassert(xpseq_heapify_impl(seq, 0, 999, true, sort_compare, NULL));
            xpseq_deep_free(&seq);
        }

        {
            for (int i = 100; i < 200; ++i) {
                XPSeq_PT seq = xpseq_random_string(i);
                xassert(xpseq_heapify_impl(seq, 0, i / 3, true, sort_compare, NULL));
                xassert(xpseq_heapify_impl(seq, i / 3, i / 3 * 2, true, sort_compare, NULL));
                xassert(xpseq_heapify_impl(seq, i / 3 * 2, seq->size - 1, true, sort_compare, NULL));
                xpseq_deep_free(&seq);
            }
        }

        {
            XPSeq_PT seq = xpseq_random_string(1000);
            xassert(xpseq_heapify_impl(seq, 0, 999, false, sort_compare, NULL));
            xpseq_deep_free(&seq);
        }

        {
            for (int i = 100; i < 200; ++i) {
                XPSeq_PT seq = xpseq_random_string(i);
                xassert(xpseq_heapify_impl(seq, 0, i / 3, false, sort_compare, NULL));
                xassert(xpseq_heapify_impl(seq, i / 3, i / 3 * 2, false, sort_compare, NULL));
                xassert(xpseq_heapify_impl(seq, i / 3 * 2, seq->size - 1, false, sort_compare, NULL));
                xpseq_deep_free(&seq);
            }
        }
    }

    /* xpseq_heapify_min */
    {
        {
            XPSeq_PT seq = xpseq_random_string(1000);
            xassert(xpseq_heapify_min(seq, sort_compare, NULL));
            xpseq_deep_free(&seq);
        }

        {
            for (int i = 1; i < 200; ++i) {
                XPSeq_PT seq = xpseq_random_string(i);
                xassert(xpseq_heapify_min(seq, sort_compare, NULL));
                xpseq_deep_free(&seq);
            }
        }
    }

    /* xpseq_heapify_max */
    {
        {
            XPSeq_PT seq = xpseq_random_string(1000);
            xassert(xpseq_heapify_max(seq, sort_compare, NULL));
            xpseq_deep_free(&seq);
        }

        {
            for (int i = 1; i < 200; ++i) {
                XPSeq_PT seq = xpseq_random_string(i);
                xassert(xpseq_heapify_max(seq, sort_compare, NULL));
                xpseq_deep_free(&seq);
            }
        }
    }

    /* xpseq_heap_sort_impl */
    {
        for (int i = 100; i < 200; ++i) {
            XPSeq_PT seq = xpseq_random_string(i);
            xassert(xpseq_heap_sort_impl(seq, 0, i / 3, true, sort_compare, NULL));
            xassert(xpseq_heap_sort_impl(seq, i / 3, i / 3 * 2, true, sort_compare, NULL));
            xassert(xpseq_heap_sort_impl(seq, i / 3 * 2, seq->size - 1, true, sort_compare, NULL));
            xpseq_deep_free(&seq);
        }

        for (int i = 100; i < 200; ++i) {
            XPSeq_PT seq = xpseq_random_string(i);
            xassert(xpseq_heap_sort_impl(seq, 0, i / 3, false, sort_compare, NULL));
            xassert(xpseq_heap_sort_impl(seq, i / 3, i / 3 * 2, false, sort_compare, NULL));
            xassert(xpseq_heap_sort_impl(seq, i / 3 * 2, seq->size - 1, false, sort_compare, NULL));
            xpseq_deep_free(&seq);
        }
    }

    /* xpseq_heap_sort */
    {
        {
            XPSeq_PT seq = xpseq_random_string(1000);
            xassert(xpseq_heap_sort(seq, sort_compare, NULL));
            xpseq_deep_free(&seq);
        }

        {
            for (int i = 1; i < 200; ++i) {
                XPSeq_PT seq = xpseq_random_string(i);
                xassert(xpseq_heap_sort(seq, sort_compare, NULL));
                xpseq_deep_free(&seq);
            }
        }
    }

    /* xpseq_quick_sort */
    {
        {
            XPSeq_PT seq = xpseq_random_string(1000);
            {
                void *x = xpseq_pop_front(seq);
                XMEM_FREE(x);
            }
            {
                void *x = xpseq_pop_front(seq);
                XMEM_FREE(x);
            }
            {
                void *x = xpseq_pop_front(seq);
                XMEM_FREE(x);
            }
            xassert(xpseq_quick_sort(seq, sort_compare, NULL));
            xassert(xpseq_quick_sort(seq, sort_compare, NULL));
            xassert(xpseq_quick_sort(seq, sort_compare, NULL));
            xpseq_deep_free(&seq);
        }

        {
            for (int i = 1; i < 200; ++i) {
                XPSeq_PT seq = xpseq_random_string(i);
                xassert(xpseq_quick_sort(seq, sort_compare, NULL));
                xassert(xpseq_quick_sort(seq, sort_compare, NULL));
                xpseq_deep_free(&seq);
            }
        }
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}


