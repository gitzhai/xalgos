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

#ifndef XISEQX_INCLUDED
#define XISEQX_INCLUDED

#include "../array_int/xarray_int_x.h"
#include "../include/xqueue_sequence_int.h"

struct XISeq {
    int   head;     /* position of index 0 of the sequence                        */
    int   size;     /* number of valid elements in sequence (not the memory size) */

    XIArray_PT array;
};

extern void         xiseq_push_back_impl     (XISeq_PT seq, int x);
extern int          xiseq_pop_back_impl      (XISeq_PT seq);

extern bool         xiseq_heapify_impl       (XISeq_PT seq, int lo, int hi, bool minheap);
extern bool         xiseq_heap_sort_impl     (XISeq_PT seq, int lo, int hi, bool minheap);

extern bool         xiseq_save_and_put_impl  (XISeq_PT seq, int i, int x, int *old_x);

static inline
int xiseq_get_impl (XISeq_PT seq, int i) {
    return seq->array->datas[(seq->head + i) % seq->array->size];
}

static inline
void xiseq_put_impl(XISeq_PT seq, int i, int x) {
    seq->array->datas[(seq->head + i) % seq->array->size] = x;
}

static inline
void xiseq_exch_impl(XISeq_PT seq, int i, int j) {
    int x = seq->array->datas[(seq->head + i) % seq->array->size];
    seq->array->datas[(seq->head + i) % seq->array->size] = seq->array->datas[(seq->head + j) % seq->array->size];
    seq->array->datas[(seq->head + j) % seq->array->size] = x;
}

#endif
