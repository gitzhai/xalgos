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
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "../thread_semaphore/xthread_sem_x.h"
#include "../include/xalgos.h"


static
void* thread_func(void* arg) {
    XThread_Sem_PT sem = (XThread_Sem_PT)arg;
    xassert(xthread_sem_wait(sem));
    return NULL;
}

static
void check_mem_leak(const void* ptr, long size, const char* file, int line, void* cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

void test_xthread_sem() {

    // xthread_sem_init
    {
        XThread_Sem_PT sem = xthread_sem_init(0);
        xassert(sem);
        xthread_sem_free(&sem);
    }

    // xthread_sem_post
    // xthread_sem_wait
    {
        XThread_Sem_PT sem = xthread_sem_init(0);
        xassert(sem);

        pthread_t thread;
        pthread_create(&thread, NULL, thread_func, (void*)sem);
        sleep(1); // Ensure the thread is waiting on the semaphore

        xassert(xthread_sem_post(sem));

        pthread_join(thread, NULL);
        xthread_sem_free(&sem);
    }

    // xthread_sem_post_all
    {
        XThread_Sem_PT sem = xthread_sem_init(0);
        xassert(sem);

        pthread_t thread;
        pthread_create(&thread, NULL, thread_func, (void*)sem);
        sleep(1); // Ensure the thread is waiting on the semaphore

        xassert(xthread_sem_post_all(sem));

        pthread_join(thread, NULL);
        xthread_sem_free(&sem);
    }

    // xthread_sem_reset
    {
        XThread_Sem_PT sem = xthread_sem_init(0);
        xassert(sem);

        xassert(xthread_sem_reset(sem));

        pthread_t thread;
        pthread_create(&thread, NULL, thread_func, (void*)sem);
        sleep(1); // Ensure the thread is waiting on the semaphore

        xassert(xthread_sem_post(sem));

        pthread_join(thread, NULL);
        xthread_sem_free(&sem);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}

#endif
