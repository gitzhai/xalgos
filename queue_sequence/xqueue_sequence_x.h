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

#ifndef XPSEQX_INCLUDED
#define XPSEQX_INCLUDED

#include "../array_pointer/xarray_pointer_x.h"
#include "../include/xqueue_sequence.h"

struct XPSeq {
    int   head;     /* position of index 0 of the sequence                        */
    int   size;     /* number of valid elements in sequence (not the memory size) */

    XPArray_PT array;
};

/* O(N) */
extern XPSeq_PT      xpseq_copyn_impl (XPSeq_PT seq, int count, int elem_size, bool deep);

/* O(1) */
extern void*         xpseq_get                (XPSeq_PT seq, int i);
extern bool          xpseq_put                (XPSeq_PT seq, int i, void *x, void **old_x);
extern bool          xpseq_save_and_put_impl  (XPSeq_PT seq, int i, void *x, void **old_x);

/* O(lgN) */
extern void          xpseq_heapify_sink_elem  (XPSeq_PT seq, int k, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl), void *cl);
extern void          xpseq_heapify_swim_elem  (XPSeq_PT seq, int k, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(NlgN) */
extern bool          xpseq_heapify_impl       (XPSeq_PT seq, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl), void *cl);
extern bool          xpseq_heap_sort_impl     (XPSeq_PT seq, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(N) */
extern bool          xpseq_section_is_heap_sorted_impl (XPSeq_PT seq, int k, int lo, int hi, bool minheap, int (*cmp)(void *x, void *y, void *cl), void *cl);

/* O(1) */
static inline
void* xpseq_get_impl(XPSeq_PT seq, int i) {
    return seq->array->datas[(seq->head + i) % seq->array->size];
}

/* O(1) */
static inline
void xpseq_put_impl(XPSeq_PT seq, int i, void *x) {
    seq->array->datas[(seq->head + i) % seq->array->size] = x;
}

#endif
