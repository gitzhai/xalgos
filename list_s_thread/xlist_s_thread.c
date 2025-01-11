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

#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../list_s_raw/xlist_s_raw_x.h"
#include "xlist_s_thread_x.h"

XTS_SList_PT xts_slist_new(int capacity) {
    XTS_SList_PT slist = XMEM_CALLOC(1, sizeof(*slist));
    if (!slist) {
        return NULL;
    }

    if (pthread_mutex_init(&slist->mutex, NULL) != 0) {
        XMEM_FREE(slist);
        return NULL;
    }

    if (pthread_cond_init(&slist->not_empty, NULL) != 0) {
        pthread_mutex_destroy(&(slist->mutex));
        XMEM_FREE(slist);
        return NULL;
    }

    //slist->head = NULL;
    //slist->tail = NULL;
    //slist->size = 0;
    slist->capacity = capacity;

    return slist;
}

bool xts_slist_push_front_repeat(XTS_SList_PT slist, void* value) {
    xassert(slist);
    xassert(value);

    if (!slist || !value) {
        return false;
    }

    if (pthread_mutex_lock(&slist->mutex) != 0) {
        return false;
    }

    // Check if the list has reached its capacity
    if (slist->capacity != -1 && slist->size >= slist->capacity) {
        pthread_mutex_unlock(&slist->mutex);
        return false;
    }

    {
        XRSList_PT p = xrslist_new(value);
        if (!p) {
            pthread_mutex_unlock(&slist->mutex);
            return false;
        }

        p->next = slist->head;

        slist->head = p;
        if (!slist->tail) {
            slist->tail = p;
        }

        ++slist->size;
    }

    pthread_cond_signal(&slist->not_empty);
    pthread_mutex_unlock(&slist->mutex);

    return true;
}

void* xts_slist_pop_front(XTS_SList_PT slist) {
    xassert(slist);

    if (!slist) {
        return NULL;
    }

    if (pthread_mutex_lock(&slist->mutex) != 0) {
        return NULL;
    }

    while (slist->size == 0) {
        if (pthread_cond_wait(&slist->not_empty, &slist->mutex) != 0) {
            pthread_mutex_unlock(&slist->mutex);
            return NULL;
        }
    }

    {
        XRSList_PT p = slist->head;

        slist->head = p->next;
        if (!slist->head) {
            slist->tail = NULL;
        }

        --slist->size;

        pthread_mutex_unlock(&slist->mutex);

        void* ret = p->value;
        XMEM_FREE(p);
        return ret;
    }
}

void* xts_slist_pop_front_timeout(XTS_SList_PT slist, long timeout_ms) {
    xassert(slist);

    if (!slist) {
        return NULL;
    }

    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        return NULL;  // Error getting time
    }

    // Convert timeout_ms to seconds and nanoseconds
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;

    // Handle nanosecond overflow
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000;
    }

    if (pthread_mutex_lock(&slist->mutex) != 0) {
        return NULL;
    }

    while (slist->size == 0) {
        int ret = pthread_cond_timedwait(&slist->not_empty, &slist->mutex, &ts);
        if (ret == ETIMEDOUT) {
            pthread_mutex_unlock(&slist->mutex);
            return NULL;  // Timeout occurred
        }
        else if (ret != 0) {
            pthread_mutex_unlock(&slist->mutex);
            return NULL;  // Other error occurred
        }
    }

    {
        XRSList_PT p = slist->head;

        slist->head = p->next;
        if (!slist->head) {
            slist->tail = NULL;
        }

        --slist->size;

        pthread_mutex_unlock(&slist->mutex);

        void* ret = p->value;
        XMEM_FREE(p);
        return ret;
    }
}

bool xts_slist_push_back_repeat(XTS_SList_PT slist, void* value) {
    xassert(slist);
    xassert(value);

    if (!slist || !value) {
        return false;
    }

    if (pthread_mutex_lock(&slist->mutex) != 0) {
        return false;
    }

    // Check if the list has reached its capacity
    if (slist->capacity != -1 && slist->size >= slist->capacity) {
        pthread_mutex_unlock(&slist->mutex);
        return false;
    }

    {
        XRSList_PT p = xrslist_new(value);
        if (!p) {
            pthread_mutex_unlock(&slist->mutex);
            return false;
        }

        slist->tail ? (slist->tail->next = p) : (slist->head = p);
        slist->tail = p;

        ++slist->size;
    }

    pthread_cond_signal(&slist->not_empty);
    pthread_mutex_unlock(&slist->mutex);

    return true;
}

static
void xts_slist_free_impl(XTS_SList_PT slist, bool deep) {
    if (!slist) {
        return;
    }

    {
        XRSList_PT step = NULL, np = NULL;

        for (step = slist->head; step; step = np) {
            np = step->next;

            if (deep) {
                XMEM_FREE(step->value);
            }
            XMEM_FREE(step);
        }
    }
}

void xts_slist_free(XTS_SList_PT *pslist) {
    if (!pslist) {
        return;
    }

    pthread_mutex_lock(&(*pslist)->mutex);
    xts_slist_free_impl(*pslist, false);
    pthread_mutex_unlock(&(*pslist)->mutex);

    pthread_mutex_destroy(&(*pslist)->mutex);
    pthread_cond_destroy(&(*pslist)->not_empty);

    XMEM_FREE(*pslist);
}

void xts_slist_deep_free(XTS_SList_PT *pslist) {
    if (!pslist) {
        return;
    }

    pthread_mutex_lock(&(*pslist)->mutex);
    xts_slist_free_impl(*pslist, true);
    pthread_mutex_unlock(&(*pslist)->mutex);

    pthread_mutex_destroy(&(*pslist)->mutex);
    pthread_cond_destroy(&(*pslist)->not_empty);

    XMEM_FREE(*pslist);
}

static
void xts_slist_clear_impl(XTS_SList_PT slist, bool deep) {
    if (!slist) {
        return;
    }

    xts_slist_free_impl(slist, deep);
    slist->head = NULL;
    slist->tail = NULL;
    slist->size = 0;
}

void xts_slist_clear(XTS_SList_PT slist) {
    pthread_mutex_lock(&slist->mutex);
    xts_slist_clear_impl(slist, false);
    pthread_mutex_unlock(&slist->mutex);
}

void xts_slist_deep_clear(XTS_SList_PT slist) {
    pthread_mutex_lock(&slist->mutex);
    xts_slist_clear_impl(slist, true);
    pthread_mutex_unlock(&slist->mutex);
}

int xts_slist_size(XTS_SList_PT slist) {
    pthread_mutex_lock(&slist->mutex);
    int size = slist ? slist->size : 0;
    pthread_mutex_unlock(&slist->mutex);

    return size;
}

bool xts_slist_is_empty(XTS_SList_PT slist) {
    pthread_mutex_lock(&slist->mutex);
    bool is_empty = slist ? (slist->size == 0) : true;
    pthread_mutex_unlock(&slist->mutex);

    return is_empty;
}

int xts_slist_capacity(XTS_SList_PT slist) {
    pthread_mutex_lock(&slist->mutex);
    int capacity = slist ? slist->capacity : -1;
    pthread_mutex_unlock(&slist->mutex);

    return capacity;
}

#endif
