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
#include "../list_d_raw/xlist_d_raw_x.h"
#include "xlist_d_thread_x.h"

XTS_DList_PT xts_dlist_new(int capacity) {
    XTS_DList_PT dlist = XMEM_CALLOC(1, sizeof(*dlist));
    if (!dlist) {
        return NULL;
    }

    if (pthread_mutex_init(&dlist->mutex, NULL) != 0) {
        XMEM_FREE(dlist);
        return NULL;
    }

    if (pthread_cond_init(&dlist->not_empty, NULL) != 0) {
        pthread_mutex_destroy(&(dlist->mutex));
        XMEM_FREE(dlist);
        return NULL;
    }

    //dlist->size = 0;
    //dlist->head = NULL;
    //dlist->tail = NULL;
    dlist->capacity = capacity;

    return dlist;
}

bool xts_dlist_push_front_repeat(XTS_DList_PT dlist, void *value) {
    xassert(dlist);
    xassert(value);

    if (!dlist || !value) {
        return false;
    }

    if (pthread_mutex_lock(&dlist->mutex) != 0) {
        return false;
    }

    // Check if the list has reached its capacity
    if (dlist->capacity != -1 && dlist->size >= dlist->capacity) {
        pthread_mutex_unlock(&dlist->mutex);
        return false;
    }

    {
        XRDList_PT p = xrdlist_new(value);
        if (!p) {
            pthread_mutex_unlock(&dlist->mutex);
            return false;
        }

        p->next = dlist->head;
        dlist->tail ? (dlist->head->prev = p) : (dlist->tail = p);
        dlist->head = p;

        ++dlist->size;

    }

    pthread_cond_signal(&dlist->not_empty);
    pthread_mutex_unlock(&dlist->mutex);

    return true;
}

void* xts_dlist_pop_front(XTS_DList_PT dlist) {
    xassert(dlist);

    if (!dlist) {
        return NULL;
    }

    if (pthread_mutex_lock(&dlist->mutex) != 0) {
        return NULL;
    }

    while (dlist->size == 0) {
        pthread_cond_wait(&dlist->not_empty, &dlist->mutex);
    }

    {
        XRDList_PT p = dlist->head;

        dlist->head = p->next;
        dlist->head ? (dlist->head->prev = NULL) : (dlist->tail = NULL);

        --dlist->size;

        pthread_mutex_unlock(&dlist->mutex);

        void* ret = p->value;
        XMEM_FREE(p);
        return ret;
    }
}

void* xts_dlist_pop_front_timeout(XTS_DList_PT dlist, long timeout_ms) {
    xassert(dlist);

    if (!dlist) {
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

    if (pthread_mutex_lock(&dlist->mutex) != 0) {
        return NULL;
    }

    while (dlist->size == 0) {
        int ret = pthread_cond_timedwait(&dlist->not_empty, &dlist->mutex, &ts);
        if (ret == ETIMEDOUT) {
            pthread_mutex_unlock(&dlist->mutex);
            return NULL;  // Timeout occurred
        }
        else if (ret != 0) {
            pthread_mutex_unlock(&dlist->mutex);
            return NULL;  // Other error occurred
        }
    }

    {
        XRDList_PT p = dlist->head;

        dlist->head = p->next;
        dlist->head ? (dlist->head->prev = NULL) : (dlist->tail = NULL);

        --dlist->size;

        pthread_mutex_unlock(&dlist->mutex);

        void* ret = p->value;
        XMEM_FREE(p);
        return ret;
    }
}

bool xts_dlist_push_back_repeat(XTS_DList_PT dlist, void *value) {
    xassert(dlist);
    xassert(value);

    if (!dlist || !value) {
        return false;
    }

    if (pthread_mutex_lock(&dlist->mutex) != 0) {
        return false;
    }

    // Check if the list has reached its capacity
    if (dlist->capacity != -1 && dlist->size >= dlist->capacity) {
        pthread_mutex_unlock(&dlist->mutex);
        return false;
    }

    {
        XRDList_PT p = xrdlist_new(value);
        if (!p) {
            pthread_mutex_unlock(&dlist->mutex);
            return false;
        }

        p->prev = dlist->tail;
        dlist->head ? (dlist->tail->next = p) : (dlist->head = p);
        dlist->tail = p;

        ++dlist->size;

    }

    pthread_cond_signal(&dlist->not_empty);
    pthread_mutex_unlock(&dlist->mutex);

    return true;
}

void* xts_dlist_pop_back(XTS_DList_PT dlist) {
    xassert(dlist);

    if (!dlist) {
        return NULL;
    }

    if (pthread_mutex_lock(&dlist->mutex) != 0) {
        return NULL;
    }

    while (dlist->size == 0) {
        pthread_cond_wait(&dlist->not_empty, &dlist->mutex);
    }

    {
        XRDList_PT p = dlist->tail;

        dlist->tail = p->prev;
        dlist->tail ? (dlist->tail->next = NULL) : (dlist->head = NULL);

        --dlist->size;

        pthread_mutex_unlock(&dlist->mutex);

        void* ret = p->value;
        XMEM_FREE(p);
        return ret;
    }
}

void* xts_dlist_pop_back_timeout(XTS_DList_PT dlist, long timeout_ms) {
    xassert(dlist);

    if (!dlist) {
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

    if (pthread_mutex_lock(&dlist->mutex) != 0) {
        return NULL;
    }

    while (dlist->size == 0) {
        int ret = pthread_cond_timedwait(&dlist->not_empty, &dlist->mutex, &ts);
        if (ret == ETIMEDOUT) {
            pthread_mutex_unlock(&dlist->mutex);
            return NULL;  // Timeout occurred
        }
        else if (ret != 0) {
            pthread_mutex_unlock(&dlist->mutex);
            return NULL;  // Other error occurred
        }
    }

    {
        XRDList_PT p = dlist->tail;

        dlist->tail = p->prev;
        dlist->tail ? (dlist->tail->next = NULL) : (dlist->head = NULL);

        --dlist->size;

        pthread_mutex_unlock(&dlist->mutex);

        void* ret = p->value;
        XMEM_FREE(p);
        return ret;
    }
}

static
void xts_dlist_free_impl(XTS_DList_PT dlist, bool deep) {
    if (!dlist) {
        return;
    }

    {
        XRDList_PT step = NULL, np = NULL;

        for (step = dlist->head; step; step = np) {
            np = step->next;

            if (deep) {
                XMEM_FREE(step->value);
            }

            XMEM_FREE(step);
        }
    }

    return;
}

void xts_dlist_free(XTS_DList_PT *pdlist) {
    if (!pdlist) {
        return;
    }

    if (pthread_mutex_lock(&(*pdlist)->mutex) != 0) {
        return ;
    }
    xts_dlist_free_impl(*pdlist, false);
    pthread_mutex_unlock(&(*pdlist)->mutex);

    pthread_mutex_destroy(&(*pdlist)->mutex);
    pthread_cond_destroy(&(*pdlist)->not_empty);

    XMEM_FREE(*pdlist);
}

void xts_dlist_deep_free(XTS_DList_PT *pdlist) {
    if (!pdlist) {
        return;
    }

    if (pthread_mutex_lock(&(*pdlist)->mutex) != 0) {
        return;
    }
    xts_dlist_free_impl(*pdlist, true);
    pthread_mutex_unlock(&(*pdlist)->mutex);

    pthread_mutex_destroy(&(*pdlist)->mutex);
    pthread_cond_destroy(&(*pdlist)->not_empty);

    XMEM_FREE(*pdlist);
}

static
void xts_dlist_clear_impl(XTS_DList_PT dlist, bool deep) {
    if (!dlist) {
        return;
    }

    xts_dlist_free_impl(dlist, deep);
    dlist->head = NULL;
    dlist->tail = NULL;
    dlist->size = 0;
}

void xts_dlist_clear(XTS_DList_PT dlist) {
    if (pthread_mutex_lock(&dlist->mutex) != 0) {
        return;
    }
    xts_dlist_clear_impl(dlist, false);
    pthread_mutex_unlock(&dlist->mutex);
}

void xts_dlist_deep_clear(XTS_DList_PT dlist) {
    if (pthread_mutex_lock(&dlist->mutex) != 0) {
        return;
    }
    xts_dlist_clear_impl(dlist, true);
    pthread_mutex_unlock(&dlist->mutex);
}

int xts_dlist_size(XTS_DList_PT dlist) {
    if (pthread_mutex_lock(&dlist->mutex) != 0) {
        return 0;
    }
    int size = dlist ? dlist->size : 0;
    pthread_mutex_unlock(&dlist->mutex);
    return size;
}

bool xts_dlist_is_empty(XTS_DList_PT dlist) {
    if (pthread_mutex_lock(&dlist->mutex) != 0) {
        return true;
    }
    bool is_empty = dlist ? (dlist->size == 0) : true;
    pthread_mutex_unlock(&dlist->mutex);
    return is_empty;
}

int xts_dlist_capacity(XTS_DList_PT dlist) {
    if (pthread_mutex_lock(&dlist->mutex) != 0) {
        return -1;
    }
    int capacity = dlist ? dlist->capacity : -1;
    pthread_mutex_unlock(&dlist->mutex);
    return capacity;
}

#endif
