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

#ifndef XINDEXMINPQ_INCLUDED
#define XINDEXMINPQ_INCLUDED

#include "xheap_index_min.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XIndexMinHeap_PT XIndexMinPQ_PT;

/* O(1) */
extern XIndexMinPQ_PT  xindexminpq_new                (int capacity, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(N) */
extern XIndexMinPQ_PT  xindexminpq_copy               (XIndexMinPQ_PT queue);
extern XIndexMinPQ_PT  xindexminpq_deep_copy          (XIndexMinPQ_PT queue, int elem_size);

/* O(lgN) */
extern bool            xindexminpq_push               (XIndexMinPQ_PT queue, int i, void *data, void **old_data);
extern void*           xindexminpq_pop                (XIndexMinPQ_PT queue, int *i);

/* O(1) */
extern void*           xindexminpq_peek               (XIndexMinPQ_PT queue, int *i);

/* O(1) */
extern void*           xindexminpq_get                (XIndexMinPQ_PT queue, int i);

/* O(lgN) */
extern bool            xindexminpq_remove             (XIndexMinPQ_PT queue, int i, void **old_data);

/* O(NlgN) */
extern int             xindexminpq_map                (XIndexMinPQ_PT queue, bool (*apply)(int i, void *x, void *cl), void *cl);
extern bool            xindexminpq_map_break_if_true  (XIndexMinPQ_PT queue, bool (*apply)(int i, void *x, void *cl), void *cl);
extern bool            xindexminpq_map_break_if_false (XIndexMinPQ_PT queue, bool (*apply)(int i, void *x, void *cl), void *cl);

/* O(1) */
extern void            xindexminpq_free               (XIndexMinPQ_PT *pqueue);
/* O(N) */
extern void            xindexminpq_deep_free          (XIndexMinPQ_PT *pqueue);

/* O(1) */
extern void            xindexminpq_clear              (XIndexMinPQ_PT queue);
/* O(N) */
extern void            xindexminpq_deep_clear         (XIndexMinPQ_PT queue);

/* O(1) */
extern int             xindexminpq_size               (XIndexMinPQ_PT queue);
extern bool            xindexminpq_is_empty           (XIndexMinPQ_PT queue);

/* O(NlgN) */
extern bool            xindexminpq_is_minpq           (XIndexMinPQ_PT queue);

#ifdef __cplusplus
}
#endif

#endif
