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
*       <<Algorithms>> Fourth Edition. chapter 2.4
*/

#ifndef TMINHEAP_INCLUDED
#define TMINHEAP_INCLUDED

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TMinHeap* TMinHeap_PT;

/* O(1) */
extern TMinHeap_PT tminheap_new                (int capacity, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(lgN) */
extern bool        tminheap_push               (TMinHeap_PT heap, void *data);
extern void*       tminheap_pop                (TMinHeap_PT heap);

/* O(1) */
extern void*       tminheap_peek               (TMinHeap_PT heap);

/* O(N) */
extern int         tminheap_map                (TMinHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl);
extern bool        tminheap_map_break_if_true  (TMinHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl);
extern bool        tminheap_map_break_if_false (TMinHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl);

/* O(1) */
extern void        tminheap_free               (TMinHeap_PT *pheap);
extern void        tminheap_clear              (TMinHeap_PT heap);

/* O(N) */
extern void        tminheap_deep_free          (TMinHeap_PT *pheap);
extern void        tminheap_deep_clear         (TMinHeap_PT heap);

/* O(1) */
extern int         tminheap_size               (TMinHeap_PT heap);
extern bool        tminheap_is_empty           (TMinHeap_PT heap);

/* O(NlgN) */
extern bool        tminheap_is_minheap         (TMinHeap_PT heap);

/* O(NlgN) : sort from A to Z */
extern bool        tminheap_sort               (TMinHeap_PT heap);

#ifdef __cplusplus
}
#endif

#endif
