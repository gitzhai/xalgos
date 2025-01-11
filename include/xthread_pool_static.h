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

#ifndef XTHREAD_POOL_STATIC_INCLUDED
#define XTHREAD_POOL_STATIC_INCLUDED

#if defined(__linux__)

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XSThreadPool* XSThreadPool_PT;

XSThreadPool_PT  xsthreadpool_init                 (int pool_size, int job_queue_capacity, long long job_timeout_in_queue);

bool             xsthreadpool_add_work             (XSThreadPool_PT pool, void (*function_p)(void*), void* arg_p);

// wait all jobs to be done
void             xsthreadpool_wait                 (XSThreadPool_PT pool);

bool             xsthreadpool_resize               (XSThreadPool_PT pool, int new_pool_size);

void             xsthreadpool_destroy              (XSThreadPool_PT pool);

int              xsthreadpool_num_threads_working  (XSThreadPool_PT pool);
int              xsthreadpool_num_threads_alive    (XSThreadPool_PT pool);

void             xsthreadpool_job_queue_monitor_period      (XSThreadPool_PT pool, long long period);
void             xsthreadpool_job_timeout_in_queue          (XSThreadPool_PT pool, long long timeout);

#ifdef __cplusplus
}
#endif

#endif
#endif
