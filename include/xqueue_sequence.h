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

#ifndef XPSEQ_INCLUDED
#define XPSEQ_INCLUDED

#include <stdbool.h>
#include "xarray_pointer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XPSeq* XPSeq_PT;

/* O(1) */
extern XPSeq_PT xpseq_new              (int capacity);  /* 0 < capacity */

/* O(N) */
extern XPSeq_PT xpseq_copy             (XPSeq_PT seq);
extern XPSeq_PT xpseq_copyn            (XPSeq_PT seq, int count);

/* O(N) */
extern XPSeq_PT xpseq_deep_copy        (XPSeq_PT seq, int elem_size);
extern XPSeq_PT xpseq_deep_copyn       (XPSeq_PT seq, int count, int elem_size);

/* O(N) */
extern int     xpseq_vload             (XPSeq_PT seq, void *x, ...);
extern int     xpseq_aload             (XPSeq_PT seq, XPArray_PT xs);

/* O(1) */
extern bool    xpseq_push_front        (XPSeq_PT seq, void *x);
extern void*   xpseq_pop_front         (XPSeq_PT seq);

/* O(1) */
extern bool    xpseq_push_back         (XPSeq_PT seq, void *x);
extern void*   xpseq_pop_back          (XPSeq_PT seq);

/* O(1) */
extern void*   xpseq_front             (XPSeq_PT seq);
extern void*   xpseq_back              (XPSeq_PT seq);

/* O(N) */
extern int     xpseq_map               (XPSeq_PT seq, bool (*apply)(void *x, void *cl), void *cl);
extern bool    xpseq_map_break_if_true (XPSeq_PT seq, bool (*apply)(void *x, void *cl), void *cl);
extern bool    xpseq_map_break_if_false(XPSeq_PT seq, bool (*apply)(void *x, void *cl), void *cl);

/* O(1) */
extern void    xpseq_free              (XPSeq_PT *seq);
/* O(N) */
extern void    xpseq_free_apply        (XPSeq_PT *seq, bool (*apply)(void *x, void *cl), void *cl);
extern void    xpseq_deep_free         (XPSeq_PT *seq);

/* O(1) */
extern void    xpseq_clear             (XPSeq_PT seq);
/* O(N) */
extern void    xpseq_clear_apply       (XPSeq_PT seq, bool (*apply)(void *x, void *cl), void *cl);
extern void    xpseq_deep_clear        (XPSeq_PT seq);

/* O(1) */
extern int     xpseq_size              (XPSeq_PT seq);
extern bool    xpseq_is_empty          (XPSeq_PT seq);
extern bool    xpseq_is_full           (XPSeq_PT seq);
extern int     xpseq_capacity          (XPSeq_PT seq);

/* O(N) */
extern bool    xpseq_expand            (XPSeq_PT seq, int expand_size);

/* O(1) */
extern bool    xpseq_swap              (XPSeq_PT seq1, XPSeq_PT seq2);

/* O(N) */
extern bool    xpseq_heapify_min       (XPSeq_PT seq, int (*cmp)(void *x, void *y, void *cl), void *cl);
extern bool    xpseq_heapify_max       (XPSeq_PT seq, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(NlgN) */
extern bool    xpseq_heap_sort         (XPSeq_PT seq, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(NlgN) */
extern bool    xpseq_quick_sort        (XPSeq_PT seq, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(N) */
extern bool    xpseq_is_sorted         (XPSeq_PT seq, int (*cmp)(void *x, void *y, void *cl), void *cl);

#ifdef __cplusplus
}
#endif

#endif
