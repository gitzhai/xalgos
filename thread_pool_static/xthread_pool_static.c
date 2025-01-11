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
#include "xthread_pool_static_x.h"

#define XSTHREAD_POOL_START_UP_CHECK_PERIOD 100000 // 0.1 second in microseconds
#define XSTHREAD_POOL_JOB_QUEUE_WAIT_TIMEOUT 5000 // 5 seconds in microseconds

static bool  xsthreadpool_thread_init       (XSThreadPool_PT pool, XSThreadPool_Thread_PT* thread, int id);
static void* xsthreadpool_thread_do         (void* arg);
static void* xsthreadpool_job_queue_monitor (void* arg);

XSThreadPool_PT xsthreadpool_init(int pool_size, int job_queue_capacity, long long job_timeout_in_queue) {
    XSThreadPool_PT pool = (XSThreadPool_PT)XMEM_CALLOC(1, sizeof(XSThreadPool_T));
    if (!pool) {
        return NULL;
    }

    pool->pool_size = pool_size;

    pool->num_threads_alive = 0;
    pool->num_threads_working = 0;

    pthread_mutex_init(&pool->count_lock, NULL);
    pthread_cond_init(&pool->threads_all_idle, NULL);

    pool->keep_alive = true;

    pool->job_queue_monitor_period = 1000000; // 1 second
    pool->job_timeout_in_queue = job_timeout_in_queue;

    pool->job_queue = xts_slist_new(job_queue_capacity);
    if (!pool->job_queue) {
        XMEM_FREE(pool);
        return NULL;
    }

    pool->threads = (XSThreadPool_Thread_PT*)XMEM_CALLOC(pool_size, sizeof(XSThreadPool_Thread_PT));
    if (!pool->threads) {
        xts_slist_free(&pool->job_queue);
        XMEM_FREE(pool);
        return NULL;
    }

    for (int n = 0; n < pool_size; n++) {
        if (!xsthreadpool_thread_init(pool, &pool->threads[n], n)) {
            xsthreadpool_destroy(pool);
            return NULL;
        }
    }

    while (pool->num_threads_alive != pool_size) {
        //printf("waiting all threads alive in pool\n");
        usleep(XSTHREAD_POOL_START_UP_CHECK_PERIOD);
    }

    if (job_timeout_in_queue != -1) {
        pool->job_queue_monitor = (XSThreadPool_Thread_PT)XMEM_CALLOC(1, sizeof(XSThreadPool_Thread_T));
        if (pool->job_queue_monitor) {
            pool->job_queue_monitor->pool = pool;
            pthread_create(&pool->job_queue_monitor->pthread, NULL, xsthreadpool_job_queue_monitor, pool);
        }
        else {
            xsthreadpool_destroy(pool);
            return NULL;
        }
    }

    return pool;
}

bool xsthreadpool_add_work(XSThreadPool_PT pool, void (*function_p)(void*), void* arg_p) {
    if (!pool || !pool->keep_alive) {
        return false;
    }

    XSThreadPool_Job_PT new_job = (XSThreadPool_Job_PT)XMEM_CALLOC(1, sizeof(XSThreadPool_Job_T));
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

void xsthreadpool_wait(XSThreadPool_PT pool) {
    pthread_mutex_lock(&pool->count_lock);

    while (pool->keep_alive && (xts_slist_size(pool->job_queue) > 0) || (pool->num_threads_working > 0)) {
        pthread_cond_wait(&pool->threads_all_idle, &pool->count_lock);
    }

    pthread_mutex_unlock(&pool->count_lock);
}

int xsthreadpool_num_threads_working(XSThreadPool_PT pool) {
    pthread_mutex_lock(&pool->count_lock);
    int num = pool ? pool->num_threads_working : 0;
    pthread_mutex_unlock(&pool->count_lock);
    return num;
}

int xsthreadpool_num_threads_alive(XSThreadPool_PT pool) {
    pthread_mutex_lock(&pool->count_lock);
    int num = pool ? pool->num_threads_alive : 0;
    pthread_mutex_unlock(&pool->count_lock);
    return num;
}

bool xsthreadpool_resize(XSThreadPool_PT pool, int new_pool_size) {
    if (!pool || new_pool_size <= 0) {
        return false;
    }

    pthread_mutex_lock(&pool->count_lock);

    if (new_pool_size > pool->pool_size) {
        XSThreadPool_Thread_PT* new_threads = (XSThreadPool_Thread_PT*)XMEM_RESIZE(pool->threads, new_pool_size * sizeof(XSThreadPool_Thread_PT));
        if (!new_threads) {
            pthread_mutex_unlock(&pool->count_lock);
            return false;
        }
        pool->threads = new_threads;

        for (int n = pool->pool_size; n < new_pool_size; n++) {
            if (!xsthreadpool_thread_init(pool, &pool->threads[n], n)) {
                pthread_mutex_unlock(&pool->count_lock);
                return false;
            }
        }
    }
    else if (new_pool_size < pool->pool_size) {
        for (int n = new_pool_size; n < pool->pool_size; n++) {
            pthread_cancel(pool->threads[n]->pthread);
            XMEM_FREE(pool->threads[n]);
        }
        XSThreadPool_Thread_PT* new_threads = (XSThreadPool_Thread_PT*)XMEM_RESIZE(pool->threads, new_pool_size * sizeof(XSThreadPool_Thread_PT));
        if (!new_threads) {
            pthread_mutex_unlock(&pool->count_lock);
            return false;
        }
        pool->threads = new_threads;
    }

    pool->pool_size = new_pool_size;
    pthread_mutex_unlock(&pool->count_lock);
    return true;
}

void xsthreadpool_destroy(XSThreadPool_PT pool) {
    if (!pool) {
        return;
    }

    pool->keep_alive = false;

    // Wake up the pool wait thread
    pthread_cond_signal(&pool->threads_all_idle);

    for (unsigned int n = 0; n < pool->num_threads_alive; n++) {
        pthread_join(pool->threads[n]->pthread, NULL);
        XMEM_FREE(pool->threads[n]);
    }

    if (pool->job_queue_monitor) {
        pthread_join(pool->job_queue_monitor->pthread, NULL);
        XMEM_FREE(pool->job_queue_monitor);
    }

    pthread_mutex_destroy(&pool->count_lock);
    pthread_cond_destroy(&pool->threads_all_idle);

    xts_slist_deep_free(&pool->job_queue);
    XMEM_FREE(pool->threads);
    XMEM_FREE(pool);
}

static
bool xsthreadpool_thread_init(XSThreadPool_PT pool, XSThreadPool_Thread_PT* thread, int id) {
    *thread = (XSThreadPool_Thread_PT)XMEM_CALLOC(1, sizeof(XSThreadPool_Thread_T));
    if (!*thread) {
        return false;
    }

    (*thread)->pool = pool;
    (*thread)->id = id;

    pthread_create(&(*thread)->pthread, NULL, (void* (*)(void*)) xsthreadpool_thread_do, *thread);

    return true;
}

static 
void xsthreadpool_thread_cleanup(void* arg) {
    XSThreadPool_Thread_PT thread = (XSThreadPool_Thread_PT)arg;
    XSThreadPool_PT pool = thread->pool;

    if(pool->keep_alive) {
        printf("thread in static pool exits abnormally\n");
    }
}

static
void* xsthreadpool_thread_do(void* arg) {
    XSThreadPool_Thread_PT thread = (XSThreadPool_Thread_PT)arg;
    XSThreadPool_PT pool = thread->pool;

    pthread_mutex_lock(&pool->count_lock);
    pool->num_threads_alive++;
    pthread_mutex_unlock(&pool->count_lock);

    pthread_cleanup_push(xsthreadpool_thread_cleanup, thread);

    while (pool->keep_alive) {
        // timeout pop to make pool to have chance to shutdown
        XSThreadPool_Job_PT job = (XSThreadPool_Job_PT)xts_slist_pop_front_timeout(pool->job_queue, XSTHREAD_POOL_JOB_QUEUE_WAIT_TIMEOUT);
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

    pthread_cleanup_pop(1);

    return NULL;
}

static
void* xsthreadpool_job_queue_monitor(void* arg) {
    XSThreadPool_PT pool = (XSThreadPool_PT)arg;

    while (pool->keep_alive) {
        XSThreadPool_Job_PT job = (XSThreadPool_Job_PT)xts_slist_pop_front_timeout(pool->job_queue, XSTHREAD_POOL_JOB_QUEUE_WAIT_TIMEOUT);
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

void xsthreadpool_job_queue_monitor_period(XSThreadPool_PT pool, long long period) {
    if (pool) {
        pool->job_queue_monitor_period = period;
    }
}

void xsthreadpool_job_timeout_in_queue(XSThreadPool_PT pool, long long timeout) {
    if (pool) {
        pool->job_timeout_in_queue = timeout;
        if (timeout != -1 && !pool->job_queue_monitor) {
            pool->job_queue_monitor = (XSThreadPool_Thread_PT)XMEM_CALLOC(1, sizeof(XSThreadPool_Thread_T));
            if (pool->job_queue_monitor) {
                pool->job_queue_monitor->pool = pool;
                pthread_create(&pool->job_queue_monitor->pthread, NULL, xsthreadpool_job_queue_monitor, pool);
            }
        }
    }
}

#endif
