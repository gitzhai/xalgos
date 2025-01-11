
#if defined(__linux__)

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#include "../thread_pool_dynamic/xthread_pool_dynamic_x.h"
#include "../include/xalgos.h"


static
void simple_task(void* arg) {
    int a = 1 + 2;
    a = a * 3;
    //printf("Simple task executed\n");
}

static
void multiple_tasks(void* arg) {
    int task_num = *(int*)arg;
    task_num /= 2;
    //printf("Task %d executed\n", task_num);
}

static
void long_task(void* arg) {
    sleep(1);
    //printf("Long task executed\n");
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

void test_xthread_pool_dynamic() {
    //Basic Functionality
    {
        XDThreadPool_PT pool = xdthreadpool_init(2, 4, 10, 5000);
        xassert(pool);

        xdthreadpool_add_work(pool, simple_task, NULL);
        xdthreadpool_wait(pool);
        xdthreadpool_destroy(pool);
    }

    //Multiple Tasks
    {
        XDThreadPool_PT pool = xdthreadpool_init(50, 100, -1, 5000);
        xassert(pool);

        int task_nums[1000];
        for (int i = 0; i < 1000; i++) {
            task_nums[i] = i;
            xdthreadpool_add_work(pool, multiple_tasks, &task_nums[i]);
        }

        xdthreadpool_wait(pool);
        xdthreadpool_destroy(pool);
    }

    //Task Timeout in Queue
    {
        XDThreadPool_PT pool = xdthreadpool_init(2, 4, 10, 500);
        xassert(pool);

        xdthreadpool_add_work(pool, long_task, NULL);
        xdthreadpool_add_work(pool, long_task, NULL);
        xdthreadpool_add_work(pool, long_task, NULL);
        xdthreadpool_add_work(pool, simple_task, NULL);
        xdthreadpool_add_work(pool, long_task, NULL);
        xdthreadpool_add_work(pool, long_task, NULL);
        xdthreadpool_wait(pool);
        xdthreadpool_destroy(pool);
    }

    //Dynamic Resizing
    {
        XDThreadPool_PT pool = xdthreadpool_init(2, 4, 10, 5000);
        xassert(pool);

        int task_nums[5] = { 1, 2, 3, 4, 5 };
        for (int i = 0; i < 5; i++) {
            xdthreadpool_add_work(pool, multiple_tasks, &task_nums[i]);
        }

        xdthreadpool_resize(pool, 2, 6);

        int more_task_nums[5] = { 6, 7, 8, 9, 10 };
        for (int i = 0; i < 5; i++) {
            xdthreadpool_add_work(pool, multiple_tasks, &more_task_nums[i]);
        }

        xdthreadpool_wait(pool);
        xdthreadpool_destroy(pool);
    }

    //Graceful Shutdown
    {
        XDThreadPool_PT pool = xdthreadpool_init(2, 4, 10, 5000);
        xassert(pool);

        int task_nums[5] = { 1, 2, 3, 4, 5 };
        for (int i = 0; i < 5; i++) {
            xdthreadpool_add_work(pool, multiple_tasks, &task_nums[i]);
        }

        xdthreadpool_destroy(pool);
    }

    //Thread Adjustment Based on Average Job Queue Size
    {
        XDThreadPool_PT pool = xdthreadpool_init(10, 30, -1, 5000);
        xassert(pool);

        xassert(xdthreadpool_num_threads_alive(pool) == 10);

        int task_nums[50];
        for (int i = 0; i < 50; i++) {
            task_nums[i] = i;
            xdthreadpool_add_work(pool, long_task, &task_nums[i]);
        }

        sleep(3);
        xassert(xdthreadpool_num_threads_alive(pool) > 20);

        sleep(10);
        xassert(xdthreadpool_num_threads_alive(pool) < 20);

        int more_task_nums[50];
        for (int i = 0; i < 50; i++) {
            task_nums[i] = i+51;
            xdthreadpool_add_work(pool, long_task, &more_task_nums[i]);
        }

        sleep(3);
        xassert(xdthreadpool_num_threads_alive(pool) > 20);

        // Wait for the thread adjustment monitor to adjust the number of threads again
        sleep(10);
        xassert(xdthreadpool_num_threads_alive(pool) < 20);

        xdthreadpool_wait(pool);
        xdthreadpool_destroy(pool);
    }


    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }    
}

#endif
