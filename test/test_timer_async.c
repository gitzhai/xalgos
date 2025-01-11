/*
*   Copyright (C) 2022, Xiaosan Zhai(tom.zhai@aliyun.com)
*   This file is part of the xalgos library.
*
*   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
*    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*    See the MIT License for more details.
*
*   You should have received a copy of the MIT License along with this program.
*   If not, see <https://mit-license.org/>.
*/

#if defined(__linux__)

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "../include/xalgos.h"

static
void on_timer_expired(void* data) {
    //printf("Timer expired! Executing callback with data: %s\n", (char*)data);
}

static
void test_callback(void* data) {
    //printf("Test callback executed with data: %s\n", (char*)data);
}


static
void* add_tasks(void* arg) {
    XAsyncTimer_PT timer = (XAsyncTimer_PT)arg;
    for (int i = 0; i < 5; ++i) {
        char id[16];
        snprintf(id, sizeof(id), "concurrent_task_%d", i);
        xasync_timer_add(timer, test_callback, id, 2000, 0, id);
        usleep(100000); // Sleep for 100ms
    }
    return NULL;
}

static
void* cancel_tasks(void* arg) {
    XAsyncTimer_PT timer = (XAsyncTimer_PT)arg;
    for (int i = 0; i < 5; ++i) {
        char id[16];
        snprintf(id, sizeof(id), "concurrent_task_%d", i);
        xasync_timer_cancel(timer, id);
        usleep(150000); // Sleep for 150ms
    }
    return NULL;
}

static
void check_mem_leak(const void* ptr, long size, const char* file, int line, void* cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

void test_xtimer_async() {
    // Initialize and start a new async timer with a tick duration of 1000ms, 60 buckets, a maximum of 10 tasks, and a thread pool size of 4
    XAsyncTimer_PT timer = xasync_timer_init(1000, 60, time(NULL) * 1000, 10, 4);

    // Test case for adding a single task
    {
        xassert(xasync_timer_add(timer, test_callback, "Single Task", 2000, 0, "task_single"));
        sleep(3); // Wait for the task to expire
    }

    // Test case for canceling a task
    {
        xassert(xasync_timer_add(timer, test_callback, "Task to Cancel", 5000, 0, "task_cancel"));
        sleep(2); // Wait for a bit before canceling

        xassert(xasync_timer_cancel(timer, "task_cancel"));
        sleep(4); // Wait to ensure the task does not execute
    }

    // Test case for handling overflow
    {
        xassert(xasync_timer_add(timer, test_callback, "Overflow Task", 70000, 0, "task_overflow"));
        sleep(75); // Wait for the overflow task to expire
    }

    // Zero delay task
    {
        xassert(xasync_timer_add(timer, test_callback, "Zero Delay Task", 0, 0, "task_zero_delay"));
        sleep(1); // Wait to ensure the task executes immediately
    }

    // Maximum capacity
    {
        for (int i = 0; i < 10; ++i) {
            char id[16];
            snprintf(id, sizeof(id), "task_%d", i);
            xassert(xasync_timer_add(timer, test_callback, id, 2000, 0, id));
        }
        // Adding one more task beyond capacity
        xassert(!xasync_timer_add(timer, test_callback, "Overflow Task", 2000, 0, "task_overflow"));
        sleep(3); // Wait for tasks to expire
    }

    // Test case for adding a recurring task
    {
        xassert(xasync_timer_add(timer, test_callback, "Recurring Task", 1000, 1000, "task_recurring"));
        sleep(5); // Wait to see multiple executions of the recurring task
    }

    // Recurring task with zero interval
    {
        xassert(xasync_timer_add(timer, test_callback, "Zero Interval Recurring Task", 1000, 0, "task_zero_interval"));
        sleep(3); // Wait to see if the task executes repeatedly
    }

    // Canceling non - existent task
    {
        xassert(xasync_timer_cancel(timer, "non_existent_task"));
    }

    // Simultaneous tasks
    {
        for (int i = 0; i < 5; ++i) {
            char id[16];
            snprintf(id, sizeof(id), "sim_task_%d", i);
            xassert(xasync_timer_add(timer, test_callback, id, 2000, 0, id));
        }
        sleep(3); // Wait for tasks to expire
    }

    // Task execution order
    {
        xassert(xasync_timer_add(timer, test_callback, "Task 1", 3000, 0, "task_1"));
        xassert(xasync_timer_add(timer, test_callback, "Task 2", 2000, 0, "task_2"));
        xassert(xasync_timer_add(timer, test_callback, "Task 3", 1000, 0, "task_3"));
        sleep(4); // Wait for tasks to expire in order
    }

    // Concurrent add and cancel
    {
        pthread_t add_thread, cancel_thread;

        pthread_create(&add_thread, NULL, add_tasks, timer);
        pthread_create(&cancel_thread, NULL, cancel_tasks, timer);

        pthread_join(add_thread, NULL);
        pthread_join(cancel_thread, NULL);

        sleep(3); // Wait to ensure tasks are handled correctly
    }

    // Destroy the async timer
    xasync_timer_destroy(timer);

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}

#endif
