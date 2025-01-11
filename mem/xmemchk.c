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

/*  Note :
*     This implementation has to define all hash and list details, because :
*     if we use the xhashmap interface for example, that will lead to dead loop for memory allocation :
*     xmem_malloc will call xhashmap_new->xmem_malloc->xhashmap_new->xmem_malloc ......
*/

#ifdef XDEBUG

#include <stdlib.h>
#include <string.h>
#if defined(__linux__)
#include <pthread.h>
#endif

#include "../include/xassert.h"
#include "../include/xexcept.h"
#include "../include/xmem.h"

const XExcept_T xg_memory_failed = { "Memory Allocation Failed" };

#define XMEM_HASH(p, t) (unsigned int)(((unsigned long long)(p)>>3) & (sizeof(t)/sizeof((t)[0]) - 1))

#define XMEM_HASHTAB_SIZE 2048

typedef struct XMem_Descriptor* XMem_Descriptor_PT;
struct XMem_Descriptor {
    XMem_Descriptor_PT next;

    const void *ptr;
    long        size;

    const char *file;
    int         line;
};

static XMem_Descriptor_PT xmem_hashtab[XMEM_HASHTAB_SIZE] = { NULL };
#if defined(__linux__)
static pthread_mutex_t xmem_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

static
XMem_Descriptor_PT xmem_find(const void *ptr) {
#if defined(__linux__)
    pthread_mutex_lock(&xmem_mutex);
#endif

    XMem_Descriptor_PT bp = xmem_hashtab[XMEM_HASH(ptr, xmem_hashtab)];

    while (bp && bp->ptr != ptr)
        bp = bp->next;

#if defined(__linux__)
    pthread_mutex_unlock(&xmem_mutex);
#endif

    return bp;
}

static
XMem_Descriptor_PT xmem_dalloc(void *ptr, long size, const char *file, int line) {
#ifdef XWRAP_MALLOC
    XMem_Descriptor_PT descriptor = __real_malloc(sizeof(*descriptor));
#else
    XMem_Descriptor_PT descriptor = malloc(sizeof(*descriptor));
#endif
    if (!descriptor) {
#ifdef XMEM_RAISE_EXCEPT
        if (file && line == 0)
            XEXCEPT_RAISE(xg_memory_failed);
        else
            xexcept_raise(&xg_memory_failed, file, line);
#endif
    }

    xassert(descriptor);

    descriptor->ptr = ptr;
    descriptor->size = size;
    descriptor->file = file;
    descriptor->line = line;
    descriptor->next = NULL;

    return descriptor;
}

/* malloc nbytes memory, then save the memory info into the hash table : xmem_hashtab */
void* xmem_malloc(long nbytes, const char *file, int line) {
    xassert(0 < nbytes);


    {
#ifdef XWRAP_MALLOC
        void *ptr = __real_malloc(nbytes);
#else
        void *ptr = malloc(nbytes);
#endif
        if (ptr) {
#if defined(__linux__)
            pthread_mutex_lock(&xmem_mutex);
#endif

            XMem_Descriptor_PT bp = xmem_dalloc(ptr, nbytes, file, line);
            unsigned int h = XMEM_HASH(ptr, xmem_hashtab);
            bp->next = xmem_hashtab[h];
            xmem_hashtab[h] = bp;

#if defined(__linux__)
            pthread_mutex_unlock(&xmem_mutex);
#endif
            return ptr;
        }
        else {
#ifdef XMEM_RAISE_EXCEPT
            if (file && (line == 0))
                XEXCEPT_RAISE(xg_memory_failed);
            else
                xexcept_raise(&xg_memory_failed, file, line);
#endif
        }
    }

    xassert(0);

    return NULL;
}

void xmem_free(void *ptr, const char *file, int line) {
    if (!ptr) {
        return;
    }

#if defined(__linux__)
    pthread_mutex_lock(&xmem_mutex);
#endif

    {
        unsigned int h = XMEM_HASH(ptr, xmem_hashtab);
        XMem_Descriptor_PT bp = xmem_hashtab[h];
        xassert(bp);

        if (bp->ptr == ptr) {
            xmem_hashtab[h] = bp->next;
#ifdef XWRAP_MALLOC
            __real_free(bp);
#else
            free(bp);
#endif
            bp = NULL;
        }
        else {
            XMem_Descriptor_PT np = bp->next;
            while (np) {
                if (np->ptr == ptr) {
                    bp->next = np->next;
                    break;
                }
                else {
                    bp = np;
                    np = np->next;
                }
            }

            xassert(np);
#ifdef XWRAP_MALLOC
            __real_free(np);
#else
            free(np);
#endif
            np = NULL;
        }
    }

#ifdef XWRAP_MALLOC
    __real_free(ptr);
#else
    free(ptr);
#endif

#if defined(__linux__)
    pthread_mutex_unlock(&xmem_mutex);
#endif
    return;
}

void* xmem_calloc(long count, long nbytes, const char *file, int line) {
    xassert(0 < count);
    xassert(0 < nbytes);

    {
        void *ptr = xmem_malloc(count*nbytes, file, line);
        xassert(ptr);

        memset(ptr, '\0', count*nbytes);

        return ptr;
    }
}

void* xmem_resize(void *ptr, long nbytes, const char *file, int line) {
    xassert(ptr);
    xassert(0 < nbytes);

    {
        /* check the ptr is allocated by xmem.h */
        XMem_Descriptor_PT bp = xmem_find(ptr);
        xassert(bp);

        {
            void *newptr = xmem_calloc(1, nbytes, file, line);
            memcpy(newptr, ptr, nbytes < bp->size ? nbytes : bp->size);

            xmem_free(ptr, file, line);

            return newptr;
        }
    }
}

void* xmem_resize0(void *ptr, long obytes, long nbytes, const char *file, int line) {
    xassert(ptr);
    xassert(0 < obytes);
    xassert(0 < nbytes);

    {
        /* check the ptr is allocated by xmem.h */
        XMem_Descriptor_PT bp = xmem_find(ptr);
        xassert(bp);

        {
            void *newptr = xmem_calloc(1, nbytes, file, line);
            memcpy(newptr, ptr, nbytes < bp->size ? nbytes : bp->size);

            if (bp->size < nbytes) {
                memset((char*)newptr + bp->size, 0, nbytes - bp->size);
            }

            xmem_free(ptr, file, line);

            return newptr;
        }
    }
}

#ifdef XWRAP_MALLOC
void* __wrap_malloc(size_t size) {
    return xmem_malloc((long)size, __FILE__, __LINE__);
}

void __wrap_free(void *ptr) {
    xmem_free(ptr, __FILE__, __LINE__);
}
#endif

void xmem_leak(void(*apply)(const void *ptr, long size, const char *file, int line, void *cl), void *cl) {
    XMem_Descriptor_PT bp = NULL;
    xassert(apply);

#if defined(__linux__)
    pthread_mutex_lock(&xmem_mutex);
#endif
    for (int i = 0; i < XMEM_HASHTAB_SIZE; i++) {
        for (bp = xmem_hashtab[i]; bp; bp = bp->next) {
            apply(bp->ptr, bp->size, bp->file, bp->line, cl);
        }
    }
#if defined(__linux__)
    pthread_mutex_unlock(&xmem_mutex);
#endif
}

#endif
