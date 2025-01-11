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

#ifndef XTHREAD_POOL_DYNAMIC_INCLUDED
#define XTHREAD_POOL_DYNAMIC_INCLUDED

#if defined(__linux__)

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct XDThreadPool* XDThreadPool_PT;

XDThreadPool_PT  xdthreadpool_init              (int min_pool_size, int max_pool_size, int job_queue_capacity, long long job_timeout_in_queue);

bool             xdthreadpool_add_work          (XDThreadPool_PT pool, void (*function_p)(void*), void* arg_p);

// wait all jobs to be done
void             xdthreadpool_wait              (XDThreadPool_PT pool);

bool             xdthreadpool_resize            (XDThreadPool_PT pool, int new_min_pool_size, int new_max_pool_size);

void             xdthreadpool_destroy           (XDThreadPool_PT pool);

int              xdthreadpool_num_threads_working              (XDThreadPool_PT pool);
int              xdthreadpool_num_threads_alive                (XDThreadPool_PT pool);

void             xdthreadpool_job_queue_monitor_period         (XDThreadPool_PT pool, long long period);
void             xdthreadpool_job_timeout_in_queue             (XDThreadPool_PT pool, long long timeout);
void             xdthreadpool_thread_adjustment_monitor_period (XDThreadPool_PT pool, long long period);

#ifdef __cplusplus
}
#endif

#endif
#endif
