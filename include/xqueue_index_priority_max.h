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

#ifndef XINDEXMAXPQ_INCLUDED
#define XINDEXMAXPQ_INCLUDED

#include "xheap_index_max.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XIndexMaxHeap_PT XIndexMaxPQ_PT;

/* O(1) */
extern XIndexMaxPQ_PT  xindexmaxpq_new                (int capacity, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(N) */
extern XIndexMaxPQ_PT  xindexmaxpq_copy               (XIndexMaxPQ_PT queue);
extern XIndexMaxPQ_PT  xindexmaxpq_deep_copy          (XIndexMaxPQ_PT queue, int elem_size);

/* O(lgN) */
extern bool            xindexmaxpq_push               (XIndexMaxPQ_PT queue, int i, void *data, void **old_data);
extern void*           xindexmaxpq_pop                (XIndexMaxPQ_PT queue, int *i);

/* O(1) */
extern void*           xindexmaxpq_peek               (XIndexMaxPQ_PT queue, int *i);

/* O(1) */
extern void*           xindexmaxpq_get                (XIndexMaxPQ_PT queue, int i);

/* O(lgN) */
extern bool            xindexmaxpq_remove             (XIndexMaxPQ_PT queue, int i, void **old_data);

/* O(NlgN) */
extern int             xindexmaxpq_map                (XIndexMaxPQ_PT queue, bool (*apply)(int i, void *x, void *cl), void *cl);
extern bool            xindexmaxpq_map_break_if_true  (XIndexMaxPQ_PT queue, bool (*apply)(int i, void *x, void *cl), void *cl);
extern bool            xindexmaxpq_map_break_if_false (XIndexMaxPQ_PT queue, bool (*apply)(int i, void *x, void *cl), void *cl);

/* O(1) */
extern void            xindexmaxpq_free               (XIndexMaxPQ_PT *pqueue);
/* O(N) */
extern void            xindexmaxpq_deep_free          (XIndexMaxPQ_PT *pqueue);

/* O(1) */
extern void            xindexmaxpq_clear              (XIndexMaxPQ_PT queue);
/* O(N) */
extern void            xindexmaxpq_deep_clear         (XIndexMaxPQ_PT queue);

/* O(1) */
extern int             xindexmaxpq_size               (XIndexMaxPQ_PT queue);
extern bool            xindexmaxpq_is_empty           (XIndexMaxPQ_PT queue);

/* O(NlgN) */
extern bool            xindexmaxpq_is_maxpq           (XIndexMaxPQ_PT queue);

#ifdef __cplusplus
}
#endif

#endif
