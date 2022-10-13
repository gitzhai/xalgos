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

#ifndef XMEM_INCLUDED
#define XMEM_INCLUDED

/* macro definition : 
*    XDEBUG :
*      with this macro defined, file xmemchk.c will implement all interfaces,
*      if not defined, xmem.c will implement all interfaces.
*
*    XMEM_RAISE_EXCEPT :
*      without this macro defined (default), pointer will return directly without checking,
*      if defined,  except "xg_memory_failed" will be raised for failed memory allocation
*/

#ifdef __cplusplus
extern "C" {
#endif

extern void* xmem_malloc (long nbytes, const char *file, int line);
extern void* xmem_calloc (long count, long nbytes, const char *file, int line);
extern void* xmem_resize (void *ptr,  long nbytes, const char *file, int line);
extern void* xmem_resize0(void *ptr, long obytes, long nbytes, const char *file, int line);
extern void  xmem_free   (void *ptr, const char *file, int line);

#ifdef XDEBUG
#ifdef XWRAP_MALLOC
#include <stddef.h>
extern void* __wrap_malloc  (size_t size);
extern void  __wrap_free    (void *ptr);

extern void* __real_malloc  (size_t size);
extern void  __real_free    (void *ptr);
#endif
#endif

/* defined macros for easy of use */
#define XMEM_MALLOC(nbytes)                 xmem_malloc((nbytes), __FILE__, __LINE__)
#define XMEM_CALLOC(count, nbytes)          xmem_calloc((count), (nbytes), __FILE__, __LINE__)
#define XMEM_FREE(ptr)                      ((void)(xmem_free((ptr), __FILE__, __LINE__), (ptr) ? (ptr) = 0 : 0))

#define XMEM_NEW(ptr)                       ((ptr) = XMEM_MALLOC((long)sizeof *(ptr)))
#define XMEM_NEWN(ptr, N)                   ((ptr) = XMEM_MALLOC((N) * (long)sizeof *(ptr)))

#define XMEM_NEW0(ptr)                      ((ptr) = XMEM_CALLOC(1, (long)sizeof *(ptr)))
#define XMEM_NEW0N(ptr, N)                  ((ptr) = XMEM_CALLOC((N), (long)sizeof *(ptr)))

#define XMEM_RESIZE(ptr, nbytes)            ((ptr) = xmem_resize((ptr), (nbytes), __FILE__, __LINE__))
#define XMEM_RESIZE0(ptr, obytes, nbytes)   ((ptr) = xmem_resize0((ptr), (obytes), (nbytes), __FILE__, __LINE__))


/* xmem_leak is used for memory leak checking */
extern void  xmem_leak(void(*apply)  (const void *ptr, long size, const char *file, int line, void *cl), void *cl);

/* TODO :
*    1. Some memory may use for special aim but it's not memory leak, then we can ignore the memory leak checking for it :
*       extern void  xmem_leak_igore(void *ptr);
*
*    2. add one debug interface to check pointer which is removed by many times
*       extern void xmem_multi_free(void (*apply)(const void *ptr, long size, const char *file, int line, void *cl), void *cl);
*/

#ifdef __cplusplus
}
#endif

#endif
