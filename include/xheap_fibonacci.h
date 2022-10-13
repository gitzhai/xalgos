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
*       <<Introduction to Algorithms>> Third Edition. chapter 19
*/

#ifndef XFIBHEAP_INCLUDED
#define XFIBHEAP_INCLUDED

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XFibHeap* XFibHeap_PT;

/* O(1) */
extern XFibHeap_PT xfibheap_new                (int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(1) */
extern bool        xfibheap_push               (XFibHeap_PT heap, void *data);
extern void*       xfibheap_pop                (XFibHeap_PT heap);
extern void*       xfibheap_peek               (XFibHeap_PT heap);

/* O(1) */
extern bool        xfibheap_merge              (XFibHeap_PT heap1, XFibHeap_PT *heap2);

/* O(N) */
extern int         xfibheap_map                (XFibHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl);
extern bool        xfibheap_map_break_if_true  (XFibHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl);
extern bool        xfibheap_map_break_if_false (XFibHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl);

/* O(N) */
extern void        xfibheap_free               (XFibHeap_PT *pheap);
extern void        xfibheap_free_apply         (XFibHeap_PT *pheap, bool(*apply)(void *x, void *cl), void *cl);
extern void        xfibheap_deep_free          (XFibHeap_PT *pheap);

/* O(N) */
extern void        xfibheap_clear              (XFibHeap_PT heap);
extern void        xfibheap_clear_apply        (XFibHeap_PT pheap, bool(*apply)(void *x, void *cl), void *cl);
extern void        xfibheap_deep_clear         (XFibHeap_PT heap);

/* O(1) */
extern int         xfibheap_size               (XFibHeap_PT heap);
extern bool        xfibheap_is_empty           (XFibHeap_PT heap);

extern bool        xfibheap_is_fibheap         (XFibHeap_PT heap);

#ifdef __cplusplus
}
#endif

#endif
