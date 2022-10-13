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

#ifndef TMAXHEAP_INCLUDED
#define TMAXHEAP_INCLUDED

#include <stdbool.h>
#include "../heap_min_t/theap_min.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef TMinHeap_PT TMaxHeap_PT;

/* O(1) */
extern TMaxHeap_PT tmaxheap_new                (int capacity, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(lgN) */
extern bool        tmaxheap_push               (TMaxHeap_PT heap, void *data);
extern void*       tmaxheap_pop                (TMaxHeap_PT heap);

/* O(1) */
extern void*       tmaxheap_peek               (TMaxHeap_PT heap);

/* O(N) */
extern int         tmaxheap_map                (TMaxHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl);
extern bool        tmaxheap_map_break_if_true  (TMaxHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl);
extern bool        tmaxheap_map_break_if_false (TMaxHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl);

/* O(1) */
extern void        tmaxheap_free               (TMaxHeap_PT *pheap);
/* O(N) */
extern void        tmaxheap_free_apply         (TMaxHeap_PT *pheap, bool (*apply)(void *x, void *cl), void *cl);
extern void        tmaxheap_deep_free          (TMaxHeap_PT *pheap);

/* O(1) */
extern void        tmaxheap_clear              (TMaxHeap_PT heap);
/* O(N) */
extern void        tmaxheap_clear_apply        (TMaxHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl);
extern void        tmaxheap_deep_clear         (TMaxHeap_PT heap);

/* O(1) */
extern int         tmaxheap_size               (TMaxHeap_PT heap);
extern bool        tmaxheap_is_empty           (TMaxHeap_PT heap);

/* O(NlgN) */
extern bool        tmaxheap_is_maxheap         (TMaxHeap_PT heap);

/* O(NlgN) : sort from Z to A */
extern bool        tmaxheap_sort               (TMaxHeap_PT heap);

#ifdef __cplusplus
}
#endif

#endif
