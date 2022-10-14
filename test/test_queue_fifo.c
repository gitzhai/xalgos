
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

void test_xfifo() {

    /* xfifo_push */
    /* xfifo_pop */
    /* xfifo_push_no_limit */
    /* xfifo_peek */
    {
        /* capacity != 0*/
        {
            XFifo_PT fifo = xfifo_new(10);
            xfifo_push(fifo, "a0");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);
            xfifo_push(fifo, "a1");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);
            xfifo_push(fifo, "a2");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);
            xfifo_push(fifo, "a3");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);
            xfifo_push(fifo, "a4");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);
            xfifo_push(fifo, "a5");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);
            xfifo_push(fifo, "a6");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);
            xfifo_push(fifo, "a7");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);
            xfifo_push(fifo, "a8");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);
            xfifo_push(fifo, "a9");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);

            xassert(fifo->size == 10);
            xassert(fifo->capacity == 10);

            xassert(strcmp((char*)xfifo_pop(fifo), "a0") == 0);
            xassert(strcmp((char*)xfifo_pop(fifo), "a1") == 0);
            xassert(strcmp((char*)xfifo_pop(fifo), "a2") == 0);
            xassert(strcmp((char*)xfifo_pop(fifo), "a3") == 0);
            xassert(strcmp((char*)xfifo_pop(fifo), "a4") == 0);
            xassert(strcmp((char*)xfifo_pop(fifo), "a5") == 0);
            xassert(strcmp((char*)xfifo_pop(fifo), "a6") == 0);
            xassert(strcmp((char*)xfifo_pop(fifo), "a7") == 0);
            xassert(strcmp((char*)xfifo_pop(fifo), "a8") == 0);
            xassert(strcmp((char*)xfifo_pop(fifo), "a9") == 0);

            xassert(fifo->size == 0);
            xassert(fifo->capacity == 10);

            xfifo_free(&fifo);
        }

        /* capacity == 0 */
        {
            XFifo_PT fifo = xfifo_new(2);
            xfifo_push(fifo, "a0");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);
            xfifo_push(fifo, "a1");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);
            xfifo_push_no_limit(fifo, "a2");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);
            xfifo_push(fifo, "a3");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);
            xfifo_push(fifo, "a4");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);
            xfifo_push(fifo, "a5");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);
            xfifo_push(fifo, "a6");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);
            xfifo_push(fifo, "a7");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);
            xfifo_push(fifo, "a8");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);
            xfifo_push(fifo, "a9");
            xassert(strcmp((char*)xfifo_peek(fifo), "a0") == 0);

            xassert(fifo->size == 10);
            xassert(fifo->capacity == 0);

            xassert(strcmp((char*)xfifo_pop(fifo), "a0") == 0);
            xassert(strcmp((char*)xfifo_pop(fifo), "a1") == 0);
            xassert(strcmp((char*)xfifo_pop(fifo), "a2") == 0);
            xassert(strcmp((char*)xfifo_pop(fifo), "a3") == 0);
            xassert(strcmp((char*)xfifo_pop(fifo), "a4") == 0);
            xassert(strcmp((char*)xfifo_pop(fifo), "a5") == 0);
            xassert(strcmp((char*)xfifo_pop(fifo), "a6") == 0);
            xassert(strcmp((char*)xfifo_pop(fifo), "a7") == 0);
            xassert(strcmp((char*)xfifo_pop(fifo), "a8") == 0);
            xassert(strcmp((char*)xfifo_pop(fifo), "a9") == 0);

            xassert(fifo->size == 0);
            xassert(fifo->capacity == 0);

            xfifo_free(&fifo);
        }
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}


