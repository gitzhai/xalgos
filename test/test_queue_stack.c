
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

void test_xstack() {

    /* xstack_push */
    /* xstack_pop */
    /* xstack_push_no_limit */
    /* xstack_peek */
    {
        /* capacity != 0*/
        {
            XStack_PT stack = xstack_new(10);
            xstack_push(stack, "a0");
            xassert(strcmp((char*)xstack_peek(stack), "a0") == 0);
            xstack_push(stack, "a1");
            xassert(strcmp((char*)xstack_peek(stack), "a1") == 0);
            xstack_push(stack, "a2");
            xassert(strcmp((char*)xstack_peek(stack), "a2") == 0);
            xstack_push(stack, "a3");
            xassert(strcmp((char*)xstack_peek(stack), "a3") == 0);
            xstack_push(stack, "a4");
            xassert(strcmp((char*)xstack_peek(stack), "a4") == 0);
            xstack_push(stack, "a5");
            xassert(strcmp((char*)xstack_peek(stack), "a5") == 0);
            xstack_push(stack, "a6");
            xassert(strcmp((char*)xstack_peek(stack), "a6") == 0);
            xstack_push(stack, "a7");
            xassert(strcmp((char*)xstack_peek(stack), "a7") == 0);
            xstack_push(stack, "a8");
            xassert(strcmp((char*)xstack_peek(stack), "a8") == 0);
            xstack_push(stack, "a9");
            xassert(strcmp((char*)xstack_peek(stack), "a9") == 0);

            xassert(stack->size == 10);
            xassert(stack->capacity == 10);

            xassert(strcmp((char*)xstack_pop(stack), "a9") == 0);
            xassert(strcmp((char*)xstack_pop(stack), "a8") == 0);
            xassert(strcmp((char*)xstack_pop(stack), "a7") == 0);
            xassert(strcmp((char*)xstack_pop(stack), "a6") == 0);
            xassert(strcmp((char*)xstack_pop(stack), "a5") == 0);
            xassert(strcmp((char*)xstack_pop(stack), "a4") == 0);
            xassert(strcmp((char*)xstack_pop(stack), "a3") == 0);
            xassert(strcmp((char*)xstack_pop(stack), "a2") == 0);
            xassert(strcmp((char*)xstack_pop(stack), "a1") == 0);
            xassert(strcmp((char*)xstack_pop(stack), "a0") == 0);

            xassert(stack->size == 0);
            xassert(stack->capacity == 10);

            xstack_free(&stack);
        }

        /* capacity == 0 */
        {
            XStack_PT stack = xstack_new(2);
            xstack_push(stack, "a0");
            xassert(strcmp((char*)xstack_peek(stack), "a0") == 0);
            xstack_push(stack, "a1");
            xassert(strcmp((char*)xstack_peek(stack), "a1") == 0);
            xstack_push_no_limit(stack, "a2");
            xassert(strcmp((char*)xstack_peek(stack), "a2") == 0);
            xstack_push(stack, "a3");
            xassert(strcmp((char*)xstack_peek(stack), "a3") == 0);
            xstack_push(stack, "a4");
            xassert(strcmp((char*)xstack_peek(stack), "a4") == 0);
            xstack_push(stack, "a5");
            xassert(strcmp((char*)xstack_peek(stack), "a5") == 0);
            xstack_push(stack, "a6");
            xassert(strcmp((char*)xstack_peek(stack), "a6") == 0);
            xstack_push(stack, "a7");
            xassert(strcmp((char*)xstack_peek(stack), "a7") == 0);
            xstack_push(stack, "a8");
            xassert(strcmp((char*)xstack_peek(stack), "a8") == 0);
            xstack_push(stack, "a9");
            xassert(strcmp((char*)xstack_peek(stack), "a9") == 0);

            xassert(stack->size == 10);
            xassert(stack->capacity == 0);

            xassert(strcmp((char*)xstack_pop(stack), "a9") == 0);
            xassert(strcmp((char*)xstack_pop(stack), "a8") == 0);
            xassert(strcmp((char*)xstack_pop(stack), "a7") == 0);
            xassert(strcmp((char*)xstack_pop(stack), "a6") == 0);
            xassert(strcmp((char*)xstack_pop(stack), "a5") == 0);
            xassert(strcmp((char*)xstack_pop(stack), "a4") == 0);
            xassert(strcmp((char*)xstack_pop(stack), "a3") == 0);
            xassert(strcmp((char*)xstack_pop(stack), "a2") == 0);
            xassert(strcmp((char*)xstack_pop(stack), "a1") == 0);
            xassert(strcmp((char*)xstack_pop(stack), "a0") == 0);

            xassert(stack->size == 0);
            xassert(stack->capacity == 0);

            xstack_free(&stack);
        }
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}


