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

#ifndef XTHREAD_POOL_STATICX_INCLUDED
#define XTHREAD_POOL_STATICX_INCLUDED

#if defined(__linux__)

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#include "../include/xlist_s_thread.h"
#include "../include/xthread_sem.h"
#include "../include/xthread_pool_static.h"

typedef struct XSThreadPool_Job XSThreadPool_Job_T;
typedef struct XSThreadPool_Job* XSThreadPool_Job_PT;
struct XSThreadPool_Job {
	void   (*function)(void* arg);       /* function pointer          */
	void    *arg;                        /* function's argument       */
	struct timeval create_time;
};

typedef struct XSThreadPool_Thread XSThreadPool_Thread_T;
typedef struct XSThreadPool_Thread* XSThreadPool_Thread_PT;
struct XSThreadPool_Thread {
	int              id;                  /* friendly id               */
	pthread_t        pthread;             /* pointer to actual thread  */
	XSThreadPool_PT  pool;                /* access to the pool        */
	struct timeval   start_time;          /* job start time            */
};

/* Threadpool */
typedef struct XSThreadPool XSThreadPool_T;
struct XSThreadPool {
	XSThreadPool_Thread_PT*  threads;                /* pointer to threads        */
	int                      pool_size;

	int                      num_threads_alive;      /* threads currently alive   */
	int                      num_threads_working;    /* threads currently working */
	
	pthread_mutex_t          count_lock;             /* used for thread count     */
	pthread_cond_t           threads_all_idle;       /* signal to the pool_wait   */

	bool                     keep_alive;

	XTS_SList_PT             job_queue;
	XSThreadPool_Thread_PT   job_queue_monitor;
	long long                job_queue_monitor_period;
	long long                job_timeout_in_queue;
};

#endif
#endif
