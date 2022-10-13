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
*       <<C Interfaces and Implementations>> David R. Hanson, chapter 5
*/

#ifndef XDEBUG

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "../include/xassert.h"
#include "../include/xexcept.h"
#include "../include/xmem.h"

/* global variable xg_memory_failed will be declared in xexcept.h */
const XExcept_T xg_memory_failed = { "Memory Allocation Failed" };

void* xmem_malloc(long nbytes, const char *file, int line) {
    xassert(0 < nbytes);

    if (nbytes <= 0) {
        return NULL;
    }

    {
        void *ptr = malloc(nbytes);

#ifdef XMEM_RAISE_EXCEPT
        if (!ptr)
        {
            if (file && (line == 0)) {
                XEXCEPT_RAISE(xg_memory_failed);
            }
            else {
                xexcept_raise(&xg_memory_failed, file, line);
            }
        }
#endif

        return ptr;
    }
}

void* xmem_calloc(long count, long nbytes, const char *file, int line) {
    xassert(0 < count);
    xassert(0 < nbytes);

    if ((count <= 0) || (nbytes <= 0)) {
        return NULL;
    }

    {
        void *ptr = calloc(count, nbytes);

#ifdef XMEM_RAISE_EXCEPT
        if (!ptr)
        {
            if (file && (line == 0)) {
                XEXCEPT_RAISE(xg_memory_failed);
            }
            else {
                xexcept_raise(&xg_memory_failed, file, line);
            }
        }
#endif

        return ptr;
    }
}

void* xmem_resize(void *ptr, long nbytes, const char *file, int line) {
    xassert(ptr);
    xassert(0 < nbytes);

    if (!ptr || (nbytes <= 0)) {
        return NULL;
    }

    ptr = realloc(ptr, nbytes);

#ifdef XMEM_RAISE_EXCEPT
    if (!ptr)
    {
        if (file && (line == 0)) {
            XEXCEPT_RAISE(xg_memory_failed);
        }
        else {
            xexcept_raise(&xg_memory_failed, file, line);
        }
    }
#endif

    return ptr;
}

void* xmem_resize0(void *ptr, long obytes, long nbytes, const char *file, int line) {
    xassert(ptr);
    xassert(0 < obytes);
    xassert(0 < nbytes);

    if (!ptr || (obytes <= 0) || (nbytes <= 0)) {
        return NULL;
    }

    ptr = realloc(ptr, nbytes);
    if (!ptr) {
#ifdef XMEM_RAISE_EXCEPT
        if (file && (line == 0)) {
            XEXCEPT_RAISE(xg_memory_failed);
        }
        else {
            xexcept_raise(&xg_memory_failed, file, line);
        }
#endif
    }
    else {
        if (obytes < nbytes) {
            memset((char*)ptr + obytes, 0, (nbytes - obytes));
        }
    }

    return ptr;
}

void xmem_free(void *ptr, const char *file, int line) {
    if (ptr) {
        free(ptr);
    }
}

void xmem_leak(void (*apply)(const void *ptr, long slot, const char *file, int line, void *cl), void *cl) {
    /* just implemented in "xmemchk.c" for debug */
    xassert(false);
    return ;
}

#endif
