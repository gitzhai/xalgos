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

#ifndef XINDEXMINHEAP_INCLUDED
#define XINDEXMINHEAP_INCLUDED

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XIndexMinHeap* XIndexMinHeap_PT;

/* O(1) */
extern XIndexMinHeap_PT  xindexminheap_new                (int capacity, int (*cmp)(void *x, void *y, void *cl), void *cl);  /* 0 < capacity */

/* O(N) */
extern XIndexMinHeap_PT  xindexminheap_copy               (XIndexMinHeap_PT heap);
extern XIndexMinHeap_PT  xindexminheap_deep_copy          (XIndexMinHeap_PT heap, int elem_size);

/* O(lgN) */
extern bool              xindexminheap_push               (XIndexMinHeap_PT heap, int i, void *data, void **old_data);
extern void*             xindexminheap_pop                (XIndexMinHeap_PT heap, int *i);

/* O(1) */
extern void*             xindexminheap_peek               (XIndexMinHeap_PT heap, int *i);

/* O(1) */
extern void*             xindexminheap_get                (XIndexMinHeap_PT heap, int i);

/* O(lgN) */
extern bool              xindexminheap_remove             (XIndexMinHeap_PT heap, int i, void **old_data);

/* O(NlgN) */
extern int               xindexminheap_map                (XIndexMinHeap_PT heap, bool (*apply)(int i, void *x, void *cl), void *cl);
extern bool              xindexminheap_map_break_if_true  (XIndexMinHeap_PT heap, bool (*apply)(int i, void *x, void *cl), void *cl);
extern bool              xindexminheap_map_break_if_false (XIndexMinHeap_PT heap, bool (*apply)(int i, void *x, void *cl), void *cl);

/* O(1) */
extern void              xindexminheap_free               (XIndexMinHeap_PT *pheap);
/* O(N) */
extern void              xindexminheap_deep_free          (XIndexMinHeap_PT *pheap);

/* O(1) */
extern void              xindexminheap_clear              (XIndexMinHeap_PT heap);
/* O(N) */
extern void              xindexminheap_deep_clear         (XIndexMinHeap_PT heap);

/* O(1) */
extern int               xindexminheap_size               (XIndexMinHeap_PT heap);
extern bool              xindexminheap_is_empty           (XIndexMinHeap_PT heap);

/* O(NlgN) */
extern bool              xindexminheap_is_minheap         (XIndexMinHeap_PT heap);

#ifdef __cplusplus
}
#endif

#endif
