
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/xutils.h"
#include "../queue_deque/xqueue_deque_x.h"
#include "../queue_sequence/xqueue_sequence_x.h"
#include "../include/xalgos.h"

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

void test_xlifo() {

    /* xlifo_push */
    /* xlifo_pop */
    /* xlifo_push_no_limit */
    /* xlifo_peek */
    {
        /* capacity != 0*/
        {
            XLifo_PT lifo = xlifo_new(10);
            xlifo_push(lifo, "a0");
            xassert(strcmp((char*)xlifo_peek(lifo), "a0") == 0);
            xlifo_push(lifo, "a1");
            xassert(strcmp((char*)xlifo_peek(lifo), "a1") == 0);
            xlifo_push(lifo, "a2");
            xassert(strcmp((char*)xlifo_peek(lifo), "a2") == 0);
            xlifo_push(lifo, "a3");
            xassert(strcmp((char*)xlifo_peek(lifo), "a3") == 0);
            xlifo_push(lifo, "a4");
            xassert(strcmp((char*)xlifo_peek(lifo), "a4") == 0);
            xlifo_push(lifo, "a5");
            xassert(strcmp((char*)xlifo_peek(lifo), "a5") == 0);
            xlifo_push(lifo, "a6");
            xassert(strcmp((char*)xlifo_peek(lifo), "a6") == 0);
            xlifo_push(lifo, "a7");
            xassert(strcmp((char*)xlifo_peek(lifo), "a7") == 0);
            xlifo_push(lifo, "a8");
            xassert(strcmp((char*)xlifo_peek(lifo), "a8") == 0);
            xlifo_push(lifo, "a9");
            xassert(strcmp((char*)xlifo_peek(lifo), "a9") == 0);

            xassert(lifo->size == 10);
            xassert(lifo->capacity == 10);

            xassert(strcmp((char*)xlifo_pop(lifo), "a9") == 0);
            xassert(strcmp((char*)xlifo_pop(lifo), "a8") == 0);
            xassert(strcmp((char*)xlifo_pop(lifo), "a7") == 0);
            xassert(strcmp((char*)xlifo_pop(lifo), "a6") == 0);
            xassert(strcmp((char*)xlifo_pop(lifo), "a5") == 0);
            xassert(strcmp((char*)xlifo_pop(lifo), "a4") == 0);
            xassert(strcmp((char*)xlifo_pop(lifo), "a3") == 0);
            xassert(strcmp((char*)xlifo_pop(lifo), "a2") == 0);
            xassert(strcmp((char*)xlifo_pop(lifo), "a1") == 0);
            xassert(strcmp((char*)xlifo_pop(lifo), "a0") == 0);

            xassert(lifo->size == 0);
            xassert(lifo->capacity == 10);

            xlifo_free(&lifo);
        }

        /* capacity == 0 */
        {
            XLifo_PT lifo = xlifo_new(2);
            xlifo_push(lifo, "a0");
            xassert(strcmp((char*)xlifo_peek(lifo), "a0") == 0);
            xlifo_push(lifo, "a1");
            xassert(strcmp((char*)xlifo_peek(lifo), "a1") == 0);
            xlifo_push_no_limit(lifo, "a2");
            xassert(strcmp((char*)xlifo_peek(lifo), "a2") == 0);
            xlifo_push(lifo, "a3");
            xassert(strcmp((char*)xlifo_peek(lifo), "a3") == 0);
            xlifo_push(lifo, "a4");
            xassert(strcmp((char*)xlifo_peek(lifo), "a4") == 0);
            xlifo_push(lifo, "a5");
            xassert(strcmp((char*)xlifo_peek(lifo), "a5") == 0);
            xlifo_push(lifo, "a6");
            xassert(strcmp((char*)xlifo_peek(lifo), "a6") == 0);
            xlifo_push(lifo, "a7");
            xassert(strcmp((char*)xlifo_peek(lifo), "a7") == 0);
            xlifo_push(lifo, "a8");
            xassert(strcmp((char*)xlifo_peek(lifo), "a8") == 0);
            xlifo_push(lifo, "a9");
            xassert(strcmp((char*)xlifo_peek(lifo), "a9") == 0);

            xassert(lifo->size == 10);
            xassert(lifo->capacity == 0);

            xassert(strcmp((char*)xlifo_pop(lifo), "a9") == 0);
            xassert(strcmp((char*)xlifo_pop(lifo), "a8") == 0);
            xassert(strcmp((char*)xlifo_pop(lifo), "a7") == 0);
            xassert(strcmp((char*)xlifo_pop(lifo), "a6") == 0);
            xassert(strcmp((char*)xlifo_pop(lifo), "a5") == 0);
            xassert(strcmp((char*)xlifo_pop(lifo), "a4") == 0);
            xassert(strcmp((char*)xlifo_pop(lifo), "a3") == 0);
            xassert(strcmp((char*)xlifo_pop(lifo), "a2") == 0);
            xassert(strcmp((char*)xlifo_pop(lifo), "a1") == 0);
            xassert(strcmp((char*)xlifo_pop(lifo), "a0") == 0);

            xassert(lifo->size == 0);
            xassert(lifo->capacity == 0);

            xlifo_free(&lifo);
        }
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}


