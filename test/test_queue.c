
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

void test_xqueue() {

    /* xqueue_push */
    /* xqueue_pop */
    /* xqueue_push_no_limit */
    /* xqueue_peek */
    {
        /* capacity != 0*/
        {
            XQueue_PT queue = xqueue_new(10);
            xqueue_push(queue, "a0");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);
            xqueue_push(queue, "a1");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);
            xqueue_push(queue, "a2");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);
            xqueue_push(queue, "a3");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);
            xqueue_push(queue, "a4");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);
            xqueue_push(queue, "a5");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);
            xqueue_push(queue, "a6");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);
            xqueue_push(queue, "a7");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);
            xqueue_push(queue, "a8");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);
            xqueue_push(queue, "a9");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);

            xassert(queue->size == 10);
            xassert(queue->capacity == 10);

            xassert(strcmp((char*)xqueue_pop(queue), "a0") == 0);
            xassert(strcmp((char*)xqueue_pop(queue), "a1") == 0);
            xassert(strcmp((char*)xqueue_pop(queue), "a2") == 0);
            xassert(strcmp((char*)xqueue_pop(queue), "a3") == 0);
            xassert(strcmp((char*)xqueue_pop(queue), "a4") == 0);
            xassert(strcmp((char*)xqueue_pop(queue), "a5") == 0);
            xassert(strcmp((char*)xqueue_pop(queue), "a6") == 0);
            xassert(strcmp((char*)xqueue_pop(queue), "a7") == 0);
            xassert(strcmp((char*)xqueue_pop(queue), "a8") == 0);
            xassert(strcmp((char*)xqueue_pop(queue), "a9") == 0);

            xassert(queue->size == 0);
            xassert(queue->capacity == 10);

            xqueue_free(&queue);
        }

        /* capacity == 0 */
        {
            XQueue_PT queue = xqueue_new(2);
            xqueue_push(queue, "a0");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);
            xqueue_push(queue, "a1");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);
            xqueue_push_no_limit(queue, "a2");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);
            xqueue_push(queue, "a3");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);
            xqueue_push(queue, "a4");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);
            xqueue_push(queue, "a5");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);
            xqueue_push(queue, "a6");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);
            xqueue_push(queue, "a7");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);
            xqueue_push(queue, "a8");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);
            xqueue_push(queue, "a9");
            xassert(strcmp((char*)xqueue_peek(queue), "a0") == 0);

            xassert(queue->size == 10);
            xassert(queue->capacity == 0);

            xassert(strcmp((char*)xqueue_pop(queue), "a0") == 0);
            xassert(strcmp((char*)xqueue_pop(queue), "a1") == 0);
            xassert(strcmp((char*)xqueue_pop(queue), "a2") == 0);
            xassert(strcmp((char*)xqueue_pop(queue), "a3") == 0);
            xassert(strcmp((char*)xqueue_pop(queue), "a4") == 0);
            xassert(strcmp((char*)xqueue_pop(queue), "a5") == 0);
            xassert(strcmp((char*)xqueue_pop(queue), "a6") == 0);
            xassert(strcmp((char*)xqueue_pop(queue), "a7") == 0);
            xassert(strcmp((char*)xqueue_pop(queue), "a8") == 0);
            xassert(strcmp((char*)xqueue_pop(queue), "a9") == 0);

            xassert(queue->size == 0);
            xassert(queue->capacity == 0);

            xqueue_free(&queue);
        }
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}


