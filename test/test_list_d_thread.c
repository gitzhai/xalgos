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

#include "../list_d_thread/xlist_d_thread_x.h"
#include "../include/xalgos.h"

#define NUM_THREADS 10
#define NUM_OPERATIONS 1000
#define TIMEOUT_MS 1000

static
void* push_front_thread(void* arg) {
    XTS_DList_PT dlist = (XTS_DList_PT)arg;
    for (int i = 0; i < NUM_OPERATIONS; ++i) {
        int* value = (int*)XMEM_CALLOC(1, sizeof(int));
        *value = i;
        xassert(xts_dlist_push_front_repeat(dlist, value));
    }
    return NULL;
}

static
void* push_back_thread(void* arg) {
    XTS_DList_PT dlist = (XTS_DList_PT)arg;
    for (int i = 0; i < NUM_OPERATIONS; ++i) {
        int* value = (int*)XMEM_CALLOC(1, sizeof(int));
        *value = i;
        xassert(xts_dlist_push_back_repeat(dlist, value));
    }
    return NULL;
}

static
void* pop_front_thread(void* arg) {
    XTS_DList_PT dlist = (XTS_DList_PT)arg;
    for (int i = 0; i < NUM_OPERATIONS; ++i) {
        int* value = (int*)xts_dlist_pop_front(dlist);
        if (value) {
            XMEM_FREE(value);
        }
    }
    return NULL;
}

static
void* pop_back_thread(void* arg) {
    XTS_DList_PT dlist = (XTS_DList_PT)arg;
    for (int i = 0; i < NUM_OPERATIONS; ++i) {
        int* value = (int*)xts_dlist_pop_back(dlist);
        if (value) {
            XMEM_FREE(value);
        }
    }
    return NULL;
}

static
void* pop_front_timeout_thread(void* arg) {
    XTS_DList_PT dlist = (XTS_DList_PT)arg;
    for (int i = 0; i < NUM_OPERATIONS; ++i) {
        int* value = (int*)xts_dlist_pop_front_timeout(dlist, TIMEOUT_MS);
        if (value) {
            XMEM_FREE(value);
        }
        else {
            printf("Timeout occurred in pop_front_timeout_thread\n");
        }
    }
    return NULL;
}

static
void* pop_back_timeout_thread(void* arg) {
    XTS_DList_PT dlist = (XTS_DList_PT)arg;
    for (int i = 0; i < NUM_OPERATIONS; ++i) {
        int* value = (int*)xts_dlist_pop_back_timeout(dlist, TIMEOUT_MS);
        if (value) {
            XMEM_FREE(value);
        }
        else {
            printf("Timeout occurred in pop_back_timeout_thread\n");
        }
    }
    return NULL;
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

void test_xlist_d_thread() {
    {
        XTS_DList_PT dlist = xts_dlist_new(-1);
        xassert(dlist);

        pthread_t threads[NUM_THREADS];

        // Create threads to push elements to the front
        for (int i = 0; i < NUM_THREADS / 2; ++i) {
            pthread_create(&threads[i], NULL, push_front_thread, (void*)dlist);
        }

        // Create threads to push elements to the back
        for (int i = NUM_THREADS / 2; i < NUM_THREADS; ++i) {
            pthread_create(&threads[i], NULL, push_back_thread, (void*)dlist);
        }

        // Wait for all push threads to finish
        for (int i = 0; i < NUM_THREADS; ++i) {
            pthread_join(threads[i], NULL);
        }

        // Check the size of the list
        int size = xts_dlist_size(dlist);
        xassert(size == NUM_THREADS * NUM_OPERATIONS);

        // Create threads to pop elements from the front
        for (int i = 0; i < NUM_THREADS / 2; ++i) {
            pthread_create(&threads[i], NULL, pop_front_thread, (void*)dlist);
        }

        // Create threads to pop elements from the back
        for (int i = NUM_THREADS / 2; i < NUM_THREADS; ++i) {
            pthread_create(&threads[i], NULL, pop_back_thread, (void*)dlist);
        }

        // Wait for all pop threads to finish
        for (int i = 0; i < NUM_THREADS; ++i) {
            pthread_join(threads[i], NULL);
        }

        // Check the size of the list
        size = xts_dlist_size(dlist);
        xassert(size == 0);

        // Test timeout pop operations on an empty list
        void* value = xts_dlist_pop_front_timeout(dlist, TIMEOUT_MS);
        xassert(value == NULL);

        value = xts_dlist_pop_back_timeout(dlist, TIMEOUT_MS);
        xassert(value == NULL);

        // Create threads to push elements to the front again
        for (int i = 0; i < NUM_THREADS / 2; ++i) {
            pthread_create(&threads[i], NULL, push_front_thread, (void*)dlist);
        }

        // Create threads to push elements to the back again
        for (int i = NUM_THREADS / 2; i < NUM_THREADS; ++i) {
            pthread_create(&threads[i], NULL, push_back_thread, (void*)dlist);
        }

        // Wait for all push threads to finish
        for (int i = 0; i < NUM_THREADS; ++i) {
            pthread_join(threads[i], NULL);
        }

        // Create threads to pop elements from the front with timeout
        for (int i = 0; i < NUM_THREADS / 2; ++i) {
            pthread_create(&threads[i], NULL, pop_front_timeout_thread, (void*)dlist);
        }

        // Create threads to pop elements from the back with timeout
        for (int i = NUM_THREADS / 2; i < NUM_THREADS; ++i) {
            pthread_create(&threads[i], NULL, pop_back_timeout_thread, (void*)dlist);
        }

        // Wait for all pop threads to finish
        for (int i = 0; i < NUM_THREADS; ++i) {
            pthread_join(threads[i], NULL);
        }

        // Check the size of the list
        size = xts_dlist_size(dlist);
        xassert(size == 0);

        // Clean up
        xts_dlist_free(&dlist);
        xassert(dlist == NULL);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}

#endif
