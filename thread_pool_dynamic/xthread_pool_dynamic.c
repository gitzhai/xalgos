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
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "../include/xmem.h"
#include "xthread_pool_dynamic_x.h"

#define XDTHREAD_POOL_START_UP_CHECK_PERIOD 100000 // 0.1 second in microseconds
#define XDTHREAD_POOL_JOB_QUEUE_WAIT_TIMEOUT 5000 // 5 seconds in microseconds
#define XDTHREAD_POOL_ADJUSTMENT_THRESHOLD 5 // Threshold for adjusting threads

static bool xdthreadpool_thread_init(XDThreadPool_PT pool, XDThreadPool_Thread_PT* thread, int id);
static void* xdthreadpool_thread_do(void* arg);
static void* xdthreadpool_job_queue_monitor(void* arg);
static void* xdthreadpool_adjust_threads_monitor(void* arg);
static void xdthreadpool_adjust_threads(XDThreadPool_PT pool, int average_job_queue_size);

XDThreadPool_PT xdthreadpool_init(int min_pool_size, int max_pool_size, int job_queue_capacity, long long job_timeout_in_queue) {
    XDThreadPool_PT pool = (XDThreadPool_PT)XMEM_CALLOC(1, sizeof(XDThreadPool_T));
    if (!pool) {
        return NULL;
    }

    pool->min_pool_size = min_pool_size;
    pool->max_pool_size = max_pool_size;

    pool->num_threads_alive = 0;
    pool->num_threads_working = 0;

    pthread_mutex_init(&pool->count_lock, NULL);
    pthread_cond_init(&pool->threads_all_idle, NULL);

    pool->keep_alive = true;

    pool->job_timeout_in_queue = job_timeout_in_queue;
    pool->job_queue_monitor_period = 1000000; // 1 second
    pool->thread_adjustment_monitor_period = 1000000; // 1 second;

    pool->job_queue = xts_slist_new(job_queue_capacity);
    if (!pool->job_queue) {
        XMEM_FREE(pool);
        return NULL;
    }

    pool->threads = xdlist_new();
    if (!pool->threads) {
        xts_slist_free(&pool->job_queue);
        XMEM_FREE(pool);
        return NULL;
    }

    for (int n = 0; n < min_pool_size; n++) {
        XDThreadPool_Thread_PT thread;
        if (!xdthreadpool_thread_init(pool, &thread, n)) {
            xdthreadpool_destroy(pool);
            return NULL;
        }
        xdlist_push_back_repeat(pool->threads, thread);
    }

    while (pool->num_threads_alive != min_pool_size) {
        //printf("waiting all threads alive in pool\n");
        usleep(XDTHREAD_POOL_START_UP_CHECK_PERIOD);
    }

    if (job_timeout_in_queue != -1) {
        pool->job_queue_monitor = (XDThreadPool_Thread_PT)XMEM_CALLOC(1, sizeof(XDThreadPool_Thread_T));
        if (pool->job_queue_monitor) {
            pool->job_queue_monitor->pool = pool;
            pthread_create(&pool->job_queue_monitor->pthread, NULL, xdthreadpool_job_queue_monitor, pool);
        }
        else {
            xdthreadpool_destroy(pool);
            return NULL;
        }
    }

    pool->thread_adjustment_monitor = (XDThreadPool_Thread_PT)XMEM_CALLOC(1, sizeof(XDThreadPool_Thread_T));
    if (pool->thread_adjustment_monitor) {
        pool->thread_adjustment_monitor->pool = pool;
        pthread_create(&pool->thread_adjustment_monitor->pthread, NULL, xdthreadpool_adjust_threads_monitor, pool);
    }
    else {
        xdthreadpool_destroy(pool);
        return NULL;
    }

    return pool;
}

bool xdthreadpool_add_work(XDThreadPool_PT pool, void (*function_p)(void*), void* arg_p) {
    if (!pool || !pool->keep_alive) {
        return false;
    }

    XDThreadPool_Job_PT new_job = (XDThreadPool_Job_PT)XMEM_CALLOC(1, sizeof(XDThreadPool_Job_T));
    if (!new_job) {
        return false;
    }

    new_job->function = function_p;
    new_job->arg = arg_p;

    gettimeofday(&new_job->create_time, NULL);

    if (!xts_slist_push_back_repeat(pool->job_queue, new_job)) {
        XMEM_FREE(new_job);
        return false;
    }

    return true;
}

void xdthreadpool_wait(XDThreadPool_PT pool) {
    pthread_mutex_lock(&pool->count_lock);

    while (pool->keep_alive && (xts_slist_size(pool->job_queue) > 0) || (pool->num_threads_working > 0)) {
        pthread_cond_wait(&pool->threads_all_idle, &pool->count_lock);
    }

    pthread_mutex_unlock(&pool->count_lock);
}

int xdthreadpool_num_threads_working(XDThreadPool_PT pool) {
    pthread_mutex_lock(&pool->count_lock);
    int num = pool ? pool->num_threads_working : 0;
    pthread_mutex_unlock(&pool->count_lock);
    return num;
}

int xdthreadpool_num_threads_alive(XDThreadPool_PT pool) {
    pthread_mutex_lock(&pool->count_lock);
    int num = pool ? pool->num_threads_alive : 0;
    pthread_mutex_unlock(&pool->count_lock);
    return num;
}

bool xdthreadpool_resize(XDThreadPool_PT pool, int new_min_pool_size, int new_max_pool_size) {
    if (!pool || new_min_pool_size <= 0 || new_max_pool_size <= 0 || new_min_pool_size > new_max_pool_size) {
        return false;
    }

    pthread_mutex_lock(&pool->count_lock);

    pool->min_pool_size = new_min_pool_size;
    pool->max_pool_size = new_max_pool_size;

    pthread_mutex_unlock(&pool->count_lock);
    return true;
}

void xdthreadpool_destroy(XDThreadPool_PT pool) {
    if (!pool) {
        return;
    }

    pool->keep_alive = false;

    // Wake up the pool wait thread
    pthread_cond_signal(&pool->threads_all_idle);

    while (!xdlist_is_empty(pool->threads)) {
        XDThreadPool_Thread_PT thread = (XDThreadPool_Thread_PT)xdlist_pop_front(pool->threads);
        if (thread) {
            pthread_join(thread->pthread, NULL);
            XMEM_FREE(thread);
        }
    }

    if (pool->job_queue_monitor) {
        pthread_join(pool->job_queue_monitor->pthread, NULL);
        XMEM_FREE(pool->job_queue_monitor);
    }

    if (pool->thread_adjustment_monitor) {
        pthread_join(pool->thread_adjustment_monitor->pthread, NULL);
        XMEM_FREE(pool->thread_adjustment_monitor);
    }

    pthread_mutex_destroy(&pool->count_lock);
    pthread_cond_destroy(&pool->threads_all_idle);

    xts_slist_deep_free(&pool->job_queue);
    xdlist_free(&pool->threads);
    XMEM_FREE(pool);
}

static
bool xdthreadpool_thread_init(XDThreadPool_PT pool, XDThreadPool_Thread_PT* thread, int id) {
    *thread = (XDThreadPool_Thread_PT)XMEM_CALLOC(1, sizeof(XDThreadPool_Thread_T));
    if (!*thread) {
        return false;
    }

    (*thread)->pool = pool;
    (*thread)->id = id;
    (*thread)->keep_alive = true;

    pthread_create(&(*thread)->pthread, NULL, (void* (*)(void*)) xdthreadpool_thread_do, *thread);

    return true;
}

static 
void xdthreadpool_thread_cleanup(void* arg) {
    XDThreadPool_Thread_PT thread = (XDThreadPool_Thread_PT)arg;
    XDThreadPool_PT pool = thread->pool;

    if(!thread->keep_alive) {
        XMEM_FREE(thread);
    }
}

static
void* xdthreadpool_thread_do(void* arg) {
    XDThreadPool_Thread_PT thread = (XDThreadPool_Thread_PT)arg;
    XDThreadPool_PT pool = thread->pool;

    pthread_mutex_lock(&pool->count_lock);
    pool->num_threads_alive++;
    pthread_mutex_unlock(&pool->count_lock);

    pthread_cleanup_push(xdthreadpool_thread_cleanup, thread);

    while (pool->keep_alive && thread->keep_alive) {
        // timeout pop to make pool to have chance to shutdown
        XDThreadPool_Job_PT job = (XDThreadPool_Job_PT)xts_slist_pop_front_timeout(pool->job_queue, XDTHREAD_POOL_JOB_QUEUE_WAIT_TIMEOUT);
        if (job) {
            if (pool->job_timeout_in_queue != -1) {
                // drop the timeout job
                struct timeval now;
                gettimeofday(&now, NULL);

                long long diff_ms = (now.tv_sec - job->create_time.tv_sec) * 1000LL + (now.tv_usec - job->create_time.tv_usec) / 1000LL;

                if (diff_ms > pool->job_timeout_in_queue) {
                    XMEM_FREE(job);
                    continue;
                }
            }

            pthread_mutex_lock(&pool->count_lock);
            pool->num_threads_working++;
            gettimeofday(&thread->start_time, NULL); // Record the start time of the job
            pthread_mutex_unlock(&pool->count_lock);

            job->function(job->arg);
            XMEM_FREE(job);

            pthread_mutex_lock(&pool->count_lock);
            pool->num_threads_working--;
            thread->start_time.tv_sec = 0; // Reset the start time
            thread->start_time.tv_usec = 0;
            // it may happen that all threads finished the jobs at the same time
            // but job queue still has jobs not handled
            // so the "all idle" may be a fake status at one moment
            if (pool->num_threads_working == 0) {
                pthread_cond_signal(&pool->threads_all_idle);
            }
            pthread_mutex_unlock(&pool->count_lock);
        }
    }

    pthread_mutex_lock(&pool->count_lock);
    pool->num_threads_alive--;
    pthread_mutex_unlock(&pool->count_lock);

    pthread_cleanup_pop(1);

    return NULL;
}

static
void* xdthreadpool_job_queue_monitor(void* arg) {
    XDThreadPool_PT pool = (XDThreadPool_PT)arg;

    while (pool->keep_alive) {
        XDThreadPool_Job_PT job = (XDThreadPool_Job_PT)xts_slist_pop_front_timeout(pool->job_queue, XDTHREAD_POOL_JOB_QUEUE_WAIT_TIMEOUT);
        if (job) {
            struct timeval now;
            gettimeofday(&now, NULL);

            long long diff_ms = (now.tv_sec - job->create_time.tv_sec) * 1000LL + (now.tv_usec - job->create_time.tv_usec) / 1000LL;

            // drop the timeout job
            if (diff_ms > pool->job_timeout_in_queue) {
                XMEM_FREE(job);
                continue;
            }
            else {
                xts_slist_push_back_repeat(pool->job_queue, job);
                // first job not timeout, all others not timeout, sleep a while to check it again
                usleep(pool->job_queue_monitor_period);
            }
        }
    }

    return NULL;
}

static
void* xdthreadpool_adjust_threads_monitor(void* arg) {
    XDThreadPool_PT pool = (XDThreadPool_PT)arg;

    while (pool->keep_alive) {
        if(xts_slist_is_empty(pool->job_queue)) {
            usleep(pool->thread_adjustment_monitor_period);
            continue;
        }

        int total_jobs = 0;
        int sample_count = 10; // Number of samples to take
        for (int i = 0; i < sample_count; i++) {
            total_jobs += xts_slist_size(pool->job_queue);
            usleep(pool->thread_adjustment_monitor_period / sample_count);
        }

        int average_job_queue_size = total_jobs / sample_count;
        xdthreadpool_adjust_threads(pool, average_job_queue_size);
    }

    return NULL;
}

static
void xdthreadpool_adjust_threads(XDThreadPool_PT pool, int average_job_queue_size) {
    pthread_mutex_lock(&pool->count_lock);
    int current_threads = pool->num_threads_alive;
    pthread_mutex_unlock(&pool->count_lock);

    // ignore small scope job queue change
    int diff = abs(average_job_queue_size - current_threads);

    if (diff < XDTHREAD_POOL_ADJUSTMENT_THRESHOLD) {
        return;
    }

    if (average_job_queue_size > current_threads && current_threads < pool->max_pool_size) {
        int threads_to_add = average_job_queue_size - current_threads;
        if (threads_to_add + current_threads > pool->max_pool_size) {
            threads_to_add = pool->max_pool_size - current_threads;
        }
        pthread_mutex_lock(&pool->count_lock);
        for (int i = 0; i < threads_to_add; i++) {
            XDThreadPool_Thread_PT thread;
            if (xdthreadpool_thread_init(pool, &thread, current_threads + i)) {
                xdlist_push_back_repeat(pool->threads, thread);
            }
        }
        pthread_mutex_unlock(&pool->count_lock);
    } else if (average_job_queue_size < current_threads && current_threads > pool->min_pool_size) {
        int threads_to_remove = current_threads - average_job_queue_size;
        if (threads_to_remove + pool->min_pool_size > current_threads) {
            threads_to_remove = current_threads - pool->min_pool_size;
        }
        for (int i = 0; i < threads_to_remove; i++) {
            XDThreadPool_Thread_PT thread = (XDThreadPool_Thread_PT)xdlist_pop_front(pool->threads);
            if (thread) {
                thread->keep_alive = false;
                // dangerous to use pthread_cancel here because mutex lock/unlock things
                // call pthread_join will be very slow here, so, call pthread_detach directly to let OS to release the resources later.
                // pthread_join(thread->pthread, NULL);
                pthread_detach(thread->pthread);
            }
        }
    }
}

void xdthreadpool_job_queue_monitor_period(XDThreadPool_PT pool, long long period) {
    if (pool) {
        pool->job_queue_monitor_period = period;
    }
}

void xdthreadpool_job_timeout_in_queue(XDThreadPool_PT pool, long long timeout) {
    if (pool) {
        pool->job_timeout_in_queue = timeout;
        if (timeout != -1 && !pool->job_queue_monitor) {
            pool->job_queue_monitor = (XDThreadPool_Thread_PT)XMEM_CALLOC(1, sizeof(XDThreadPool_Thread_T));
            if (pool->job_queue_monitor) {
                pool->job_queue_monitor->pool = pool;
                pthread_create(&pool->job_queue_monitor->pthread, NULL, xdthreadpool_job_queue_monitor, pool);
            }
        }
    }
}

void xdthreadpool_thread_adjustment_monitor_period(XDThreadPool_PT pool, long long period) {
    if (pool) {
        pool->thread_adjustment_monitor_period = period;
    }
}

#endif
