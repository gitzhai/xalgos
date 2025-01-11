
#if defined(__linux__)

#include <stdio.h>

#include "../thread_pool_static/xthread_pool_static_x.h"
#include "../include/xalgos.h"

static
void xthread_pool_static_test_check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static
void simple_task(void* arg) {
    int a = 1 + 2;
    a = a * 3;
}

static
void multiple_tasks(void* arg) {
    int task_num = *(int*)arg;
    task_num /= 2;
}

static
void long_task(void* arg) {
    sleep(2);
}

void test_xthread_pool_static() {

    // Basic Functionality
    {
        XSThreadPool_PT pool = xsthreadpool_init(2, 10, 5000);
        xassert(pool);

        xsthreadpool_add_work(pool, simple_task, NULL);
        xsthreadpool_wait(pool);
        xsthreadpool_destroy(pool);
    }

    // Multiple Tasks
    {
        XSThreadPool_PT pool = xsthreadpool_init(100, -1, 5000);
        xassert(pool);

        int task_nums[1000];
        for (int i = 0; i < 1000; i++) {
            task_nums[i] = i;
            xsthreadpool_add_work(pool, multiple_tasks, &task_nums[i]);
        }
        xsthreadpool_wait(pool);
        xsthreadpool_destroy(pool);
    }

    // Task Timeout in Queue
    {
        XSThreadPool_PT pool = xsthreadpool_init(2, 10, 1000);
        xassert(pool);

        xsthreadpool_add_work(pool, long_task, NULL);
        xsthreadpool_add_work(pool, long_task, NULL);
        xsthreadpool_add_work(pool, long_task, NULL);
        xsthreadpool_add_work(pool, simple_task, NULL);
        xsthreadpool_add_work(pool, long_task, NULL);
        xsthreadpool_add_work(pool, long_task, NULL);
        xsthreadpool_wait(pool);
        xsthreadpool_destroy(pool);
    }

    // Dynamic Resizing
    {
        XSThreadPool_PT pool = xsthreadpool_init(2, 10, 5000);
        xassert(pool);

        int task_nums[5] = { 1, 2, 3, 4, 5 };
        for (int i = 0; i < 5; i++) {
            xsthreadpool_add_work(pool, multiple_tasks, &task_nums[i]);
        }

        xsthreadpool_resize(pool, 4);

        int more_task_nums[5] = { 6, 7, 8, 9, 10 };
        for (int i = 0; i < 5; i++) {
            xsthreadpool_add_work(pool, multiple_tasks, &more_task_nums[i]);
        }

        xsthreadpool_wait(pool);
        xsthreadpool_destroy(pool);
    }

    // Graceful Shutdown
    {
        XSThreadPool_PT pool = xsthreadpool_init(2, 10, 5000);
        xassert(pool);

        int task_nums[5] = { 1, 2, 3, 4, 5 };
        for (int i = 0; i < 5; i++) {
            xsthreadpool_add_work(pool, multiple_tasks, &task_nums[i]);
        }

        xsthreadpool_destroy(pool);
    }

    {
        int count = 0;
        xmem_leak(xthread_pool_static_test_check_mem_leak, &count);
        xassert(count == 0);
    }
}

#endif
