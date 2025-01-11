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
*
*   Refer to :
*       https://github.com/Pithikos/C-Thread-Pool
*/

#if defined(__linux__)

#include <stdio.h>
#include <string.h>

#include "../include/xmem.h"
#include "xthread_sem_x.h"

XThread_Sem_PT xthread_sem_init(int value) {
    if (value < 0 || value > 1) {
        return NULL;
    }

    {
        XThread_Sem_PT sem = (XThread_Sem_PT)XMEM_CALLOC(1, sizeof(*sem));
        if (!sem) {
            return NULL;
        }

        int ret;
        if ((ret = pthread_mutex_init(&(sem->mutex), NULL)) != 0) {
            fprintf(stderr, "pthread_mutex_init failed: %s\n", strerror(ret));
            XMEM_FREE(sem);
            return NULL;
        }

        if ((ret = pthread_cond_init(&(sem->cond), NULL)) != 0) {
            fprintf(stderr, "pthread_cond_init failed: %s\n", strerror(ret));
            pthread_mutex_destroy(&(sem->mutex));
            XMEM_FREE(sem);
            return NULL;
        }

        sem->v = value;
        return sem;
    }
}

bool xthread_sem_post(XThread_Sem_PT sem) {
    if(!sem) {
        return false;
    }
    
    int ret;
    if ((ret = pthread_mutex_lock(&sem->mutex)) != 0) {
        fprintf(stderr, "pthread_mutex_lock failed: %s\n", strerror(ret));
        return false;
    }

    sem->v = 1;
    if ((ret = pthread_cond_signal(&sem->cond)) != 0) {
        fprintf(stderr, "pthread_cond_signal failed: %s\n", strerror(ret));
    }

    if ((ret = pthread_mutex_unlock(&sem->mutex)) != 0) {
        fprintf(stderr, "pthread_mutex_unlock failed: %s\n", strerror(ret));
        return false;
    }

    return true;
}

bool xthread_sem_post_all(XThread_Sem_PT sem) {
    if(!sem) {
        return false;
    }

    int ret;
    if ((ret = pthread_mutex_lock(&sem->mutex)) != 0) {
        fprintf(stderr, "pthread_mutex_lock failed: %s\n", strerror(ret));
        return false;
    }

    sem->v = 1;
    if ((ret = pthread_cond_broadcast(&sem->cond)) != 0) {
        fprintf(stderr, "pthread_cond_broadcast failed: %s\n", strerror(ret));
    }

    if ((ret = pthread_mutex_unlock(&sem->mutex)) != 0) {
        fprintf(stderr, "pthread_mutex_unlock failed: %s\n", strerror(ret));
        return false;
    }

    return true;
}

bool xthread_sem_wait(XThread_Sem_PT sem) {
    if(!sem) {
        return false;
    }

    int ret;
    if ((ret = pthread_mutex_lock(&sem->mutex)) != 0) {
        fprintf(stderr, "pthread_mutex_lock failed: %s\n", strerror(ret));
        return false;
    }

    while (sem->v != 1) {
        if ((ret = pthread_cond_wait(&sem->cond, &sem->mutex)) != 0) {
            fprintf(stderr, "pthread_cond_wait failed: %s\n", strerror(ret));
            break;
        }
    }

    sem->v = 0;
    if ((ret = pthread_mutex_unlock(&sem->mutex)) != 0) {
        fprintf(stderr, "pthread_mutex_unlock failed: %s\n", strerror(ret));
        return false;
    }

    return true;
}

bool xthread_sem_reset(XThread_Sem_PT sem) {
    if(!sem) {
        return false;
    }

    pthread_mutex_destroy(&(sem->mutex));
    pthread_cond_destroy(&(sem->cond));

    int ret;
    if ((ret = pthread_mutex_init(&(sem->mutex), NULL)) != 0) {
        fprintf(stderr, "pthread_mutex_init failed: %s\n", strerror(ret));
        return false;
    }

    if ((ret = pthread_cond_init(&(sem->cond), NULL)) != 0) {
        fprintf(stderr, "pthread_cond_init failed: %s\n", strerror(ret));
        pthread_mutex_destroy(&(sem->mutex));
        return false;
    }

    sem->v = 0;
    return true;
}

void xthread_sem_free(XThread_Sem_PT *sem) {
    if (!sem || !*sem) {
        return;
    }

    pthread_mutex_destroy(&((*sem)->mutex));
    pthread_cond_destroy(&((*sem)->cond));
    XMEM_FREE(*sem);
}

#endif
