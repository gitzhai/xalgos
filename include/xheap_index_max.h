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
*       <<Algorithms in C>> Third Edition. chapter 9.6
*/

#ifndef XINDEXMAXHEAP_INCLUDED
#define XINDEXMAXHEAP_INCLUDED

#include "xheap_index_min.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XIndexMinHeap_PT XIndexMaxHeap_PT;

/* O(1) */
extern XIndexMaxHeap_PT  xindexmaxheap_new                (int capacity, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(N) */
extern XIndexMaxHeap_PT  xindexmaxheap_copy               (XIndexMaxHeap_PT heap);
extern XIndexMaxHeap_PT  xindexmaxheap_deep_copy          (XIndexMaxHeap_PT heap, int elem_size);

/* O(lgN) */
extern bool              xindexmaxheap_push               (XIndexMaxHeap_PT heap, int i, void *data, void **old_data);
extern void*             xindexmaxheap_pop                (XIndexMaxHeap_PT heap, int *i);

/* O(1) */
extern void*             xindexmaxheap_peek               (XIndexMaxHeap_PT heap, int *i);

/* O(1) */
extern void*             xindexmaxheap_get                (XIndexMaxHeap_PT heap, int i);

/* O(lgN) */
extern bool              xindexmaxheap_remove             (XIndexMaxHeap_PT heap, int i, void **old_data);

/* O(NlgN) */
extern int               xindexmaxheap_map                (XIndexMaxHeap_PT heap, bool (*apply)(int i, void *x, void *cl), void *cl);
extern bool              xindexmaxheap_map_break_if_true  (XIndexMaxHeap_PT heap, bool (*apply)(int i, void *x, void *cl), void *cl);
extern bool              xindexmaxheap_map_break_if_false (XIndexMaxHeap_PT heap, bool (*apply)(int i, void *x, void *cl), void *cl);

/* O(1) */
extern void              xindexmaxheap_free               (XIndexMaxHeap_PT *pheap);
/* O(N) */
extern void              xindexmaxheap_deep_free          (XIndexMaxHeap_PT *pheap);

/* O(1) */
extern void              xindexmaxheap_clear              (XIndexMaxHeap_PT heap);
/* O(N) */
extern void              xindexmaxheap_deep_clear         (XIndexMaxHeap_PT heap);

/* O(1) */
extern int               xindexmaxheap_size               (XIndexMaxHeap_PT heap);
extern bool              xindexmaxheap_is_empty           (XIndexMaxHeap_PT heap);

/* O(NlgN) */
extern bool              xindexmaxheap_is_maxheap         (XIndexMaxHeap_PT heap);

#ifdef __cplusplus
}
#endif

#endif
