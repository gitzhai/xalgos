/*
*   Copyright (C) 2022, Xiaosan Zhai(tom.zhai@aliyun.com)
*   This file is part of the xalgos library.
*
*   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
*   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*   See the MIT License for more details.
*
*   You should have received a copy of the MIT License along with this program.
*   If not, see <https://mit-license.org/>.
*/

#if defined(__linux__)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "../include/xmem.h"
#include "../include/xlist_s_thread.h"
#include "../include/xlist_d.h"
#include "../include/xthread_pool_static.h"

#include "xtimer_async_x.h"

static void* timer_trigger_func(void* arg);

static
TimingWheel_PT timing_wheel_new(int tick_time, int wheel_size, long start_time) {
    TimingWheel_PT wheel = (TimingWheel_PT)XMEM_CALLOC(1, sizeof(*wheel));
    if (!wheel) {
        return NULL;
    }

    wheel->tick_time = tick_time;
    wheel->wheel_size = wheel_size;
    wheel->interval = tick_time * wheel_size;

    wheel->current_time = start_time - (start_time % tick_time);  // make sure current_time is multiple times of tick_time

    wheel->task_count = 0;
    wheel->overflow_wheel = NULL;

    wheel->buckets = (XDList_PT*)XMEM_CALLOC(wheel_size, sizeof(XDList_PT));
    if (!wheel->buckets) {
        XMEM_FREE(wheel);
        return NULL;
    }

    for (int i = 0; i < wheel_size; ++i) {
        wheel->buckets[i] = xdlist_new();
        if (!wheel->buckets[i]) {
            for (int j = i - 1; j >= 0; --j) {
                xdlist_free(&wheel->buckets[j]);
            }
            XMEM_FREE(wheel->buckets);
            XMEM_FREE(wheel);
            return NULL;
        }
    }

    return wheel;
}

static
void timing_wheel_free(TimingWheel_PT wheel) {
    if (!wheel) {
        return;
    }

    if (wheel->overflow_wheel) {
        timing_wheel_free(wheel->overflow_wheel);
    }

    for (int i = 0; i < wheel->wheel_size; ++i) {
        XDList_PT bucket = wheel->buckets[i];
        while (!xdlist_is_empty(bucket)) {
            TimerTask_PT task = (TimerTask_PT)xdlist_pop_front(bucket);
            XMEM_FREE(task);
        }
        xdlist_free(&bucket);
    }
    XMEM_FREE(wheel->buckets);
    XMEM_FREE(wheel);
}

// Initialize and start a new async timer
XAsyncTimer_PT xasync_timer_init(int tick_time, int wheel_size, long start_time, int capacity, int pool_size) {
    XAsyncTimer_PT timer = (XAsyncTimer_PT)XMEM_CALLOC(1, sizeof(*timer));
    if (!timer) {
        return NULL;
    }

    timer->wheel = timing_wheel_new(tick_time, wheel_size, start_time);
    if (!timer->wheel) {
        XMEM_FREE(timer);
        return NULL;
    }

    timer->task_count = 0;
    timer->capacity = capacity;

    timer->timeout_handlers = xsthreadpool_init(pool_size, capacity * 10, -1);
    if (!timer->timeout_handlers) {
        timing_wheel_free(timer->wheel);
        XMEM_FREE(timer);
        return NULL;
    }

    timer->running = true; // Set the running flag to true

    pthread_mutex_init(&timer->lock, NULL);

    pthread_create(&timer->timer_trigger, NULL, timer_trigger_func, timer);

    return timer;
}

// Add a timer task to the async timer
static
bool xasync_timer_add_task(XAsyncTimer_PT timer, TimingWheel_PT wheel, TimerTask_PT task, long delay) {
    bool result = true;
    bool overflow = false;

    pthread_mutex_lock(&timer->lock);

    while (true) {
        if (timer->capacity != -1 && timer->task_count >= timer->capacity) {
            XMEM_FREE(task);
            result = false; // Task limit reached
        } else {
            long expiration = wheel->current_time + delay;
            task->expiration = expiration;

            if (delay < wheel->tick_time) {
                // Directly add the task to the thread pool
                xsthreadpool_add_work(timer->timeout_handlers, task->callback, task->data);

                if (task->recurrence) {
                    // add the task to wheel at the same time
                    task->expiration += task->recurrence;
                    delay = task->recurrence;
                    overflow = false;
                    continue;
                } else {
                    XMEM_FREE(task);
                }
            } else if (expiration < wheel->current_time + wheel->interval) {
                // Add to the current timing wheel
                int bucketIndex = (expiration / wheel->tick_time) % wheel->wheel_size;
                xdlist_push_back_repeat(wheel->buckets[bucketIndex], task);
                wheel->task_count++;
                timer->task_count++;
                overflow = false;
            } else {
                overflow = true;
                // Add to the overflow timing wheel
                if (!wheel->overflow_wheel) {
                    wheel->overflow_wheel = timing_wheel_new(wheel->interval, wheel->wheel_size, wheel->current_time);
                    if (!wheel->overflow_wheel) {
                        result = false;
                        overflow = false;
                        XMEM_FREE(task);
                    }
                }
            }
        }

        if (!overflow) {
            break;
        }

        wheel = wheel->overflow_wheel;
    }

    pthread_mutex_unlock(&timer->lock);

    return result;
}

// Add a new timer task with proper parameters
bool xasync_timer_add(XAsyncTimer_PT timer, XAsyncTimer_Task_Callback callback, void* data, long delay, long recurrence, char* id) {
    if(!timer || !callback) {
        return false;
    }

    TimerTask_PT task = (TimerTask_PT)XMEM_CALLOC(1, sizeof(*task));
    if (!task) {
        return false;
    }

    task->callback = callback;
    task->data = data;
    task->recurrence = recurrence;

    if (id) {
        strncpy(task->id, id, sizeof(task->id) - 1);
    }

    return xasync_timer_add_task(timer, timer->wheel, task, delay);
}

// Advance the async timer
// it's bad idea for large mount of tasks to do demote, so not use it here
static
void xasync_timer_advance(XAsyncTimer_PT timer, long time_ms) {
    TimingWheel_PT current_wheel = timer->wheel;

    pthread_mutex_lock(&timer->lock);

    while (current_wheel) {
        if (time_ms >= current_wheel->current_time + current_wheel->tick_time) {
            current_wheel->current_time = time_ms - (time_ms % current_wheel->tick_time);
            current_wheel = current_wheel->overflow_wheel;
        }
        else {
            current_wheel = NULL;
        }
    }

    pthread_mutex_unlock(&timer->lock);
}

// Execute and remove expired tasks from the given wheel
static
void xasync_timer_tick_wheel(XAsyncTimer_PT timer, TimingWheel_PT wheel, XDList_PT recurring_tasks) {
    int bucketIndex = (wheel->current_time / wheel->tick_time) % wheel->wheel_size;
    XDList_PT bucket = wheel->buckets[bucketIndex];

    while (!xdlist_is_empty(bucket)) {
        TimerTask_PT task = (TimerTask_PT)xdlist_front(bucket);

        if (task->expiration <= wheel->current_time) {
            xdlist_pop_front(bucket);

            // Directly add the task to the thread pool
            xsthreadpool_add_work(timer->timeout_handlers, task->callback, task->data);
            timer->task_count--;
            wheel->task_count--;

            if (task->recurrence) {
                task->expiration += task->recurrence;
                xdlist_push_back_repeat(recurring_tasks, task);
            }
            else {
                XMEM_FREE(task);
            }
        }
        else {
            break;
        }
    }
}

// Execute and remove expired tasks from all wheels
static
void xasync_timer_tick(XAsyncTimer_PT timer) {
    XDList_PT recurring_tasks = xdlist_new();
    if (!recurring_tasks) {
        return;
    }

    pthread_mutex_lock(&timer->lock);

    TimingWheel_PT current_wheel = timer->wheel;
    TimingWheel_PT prev_wheel = NULL;

    while (current_wheel) {
        xasync_timer_tick_wheel(timer, current_wheel, recurring_tasks);

        // give chance to free the last wheel if no tasks in it
        // !current_wheel->overflow_wheel means the last wheel which will be check to free or not
        if (!current_wheel->overflow_wheel && (current_wheel->task_count == 0) && prev_wheel) {
            prev_wheel->overflow_wheel = NULL;
            timing_wheel_free(current_wheel);
            break;
        }
        else {
            prev_wheel = current_wheel;
            current_wheel = current_wheel->overflow_wheel;
        }
    }

    pthread_mutex_unlock(&timer->lock);

    // Re-add recurring tasks
    while (!xdlist_is_empty(recurring_tasks)) {
        TimerTask_PT task = (TimerTask_PT)xdlist_pop_front(recurring_tasks);
        xasync_timer_add_task(timer, timer->wheel, task, task->recurrence);
    }

    xdlist_free(&recurring_tasks);
}

// Timer trigger function
static
void* timer_trigger_func(void* arg) {
    XAsyncTimer_PT timer = (XAsyncTimer_PT)arg;
    while (timer->running) {
        xasync_timer_advance(timer, time(NULL) * 1000);
        xasync_timer_tick(timer);
        usleep(timer->wheel->tick_time * 1000); // Sleep for tick_time milliseconds
    }
    return NULL;
}

// Destroy the async timer
void xasync_timer_destroy(XAsyncTimer_PT timer) {
    if (!timer) {
        return;
    }

    timer->running = false;
    pthread_join(timer->timer_trigger, NULL);

    timing_wheel_free(timer->wheel);

    xsthreadpool_destroy(timer->timeout_handlers);
    pthread_mutex_destroy(&timer->lock);
    XMEM_FREE(timer);
}

// Cancel a timer task by id
bool xasync_timer_cancel(XAsyncTimer_PT timer, char* id) {
    if(!timer || !id) {
        return false;
    }

    bool canceled = false;
    TimingWheel_PT current_wheel = timer->wheel;

    pthread_mutex_lock(&timer->lock);

    while (current_wheel) {
        for (int i = 0; i < current_wheel->wheel_size; ++i) {
            if (xdlist_remove(current_wheel->buckets[i], id)) {
                timer->task_count--;
                current_wheel->task_count--;
                canceled = true;
                break;
            }
        }

        if (canceled) {
            break;
        }

        current_wheel = current_wheel->overflow_wheel;
    }

    pthread_mutex_unlock(&timer->lock);
    return canceled;
}

#endif