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

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../utils/xutils.h"
#include "../include/xarith_int.h"
#include "../array_int/xarray_int_x.h"
#include "xqueue_sequence_int_x.h"

XISeq_PT xiseq_new(int capacity) {
    xassert(0 < capacity);

    if (capacity <= 0) {
        return NULL;
    }

    {
        XISeq_PT seq = XMEM_CALLOC(1, sizeof(*seq));
        if (!seq) {
            return NULL;
        }

        seq->array = xiarray_new(capacity);
        if (!seq->array) {
            XMEM_FREE(seq);
            return NULL;
        }

        //seq->size = 0;
        //seq->head = 0;

        return seq;
    }
}


XISeq_PT xiseq_copyn_impl(XISeq_PT seq, int count, int elem_size) {
    if (seq->size < count) {
        count = seq->size;
    }

    {
        XISeq_PT nseq = XMEM_CALLOC(1, sizeof(*seq));
        if (!nseq) {
            return NULL;
        }

        if (seq->head == 0) {
            nseq->array = xiarray_copyn(seq->array, count);
        }
        else {
            if (count == 0) {
                nseq->array = xiarray_new(count);
            }
            else if (count <= (seq->array->size - seq->head)) {
                nseq->array = xiarray_scope_copy(seq->array, seq->head, seq->head + count - 1);
            }
            else {
                nseq->array = xiarray_scope_copy(seq->array, seq->head, seq->array->size - 1);
                if (!nseq->array) {
                    XMEM_FREE(nseq);
                    return NULL;
                }
                nseq->size = seq->array->size - seq->head;

                if (!xiarray_scope_index_copy_resize(seq->array, 0, (count - (seq->array->size - seq->head) - 1), nseq->array, nseq->size)) {
                    xiseq_free(&nseq);
                    return NULL;
                }
            }
        }

        if (!nseq->array) {
            XMEM_FREE(nseq);
            return NULL;
        }

        nseq->size = count;
        nseq->head = 0; /* pay attention that head is 0 now ! */

        return nseq;
    }
}

XISeq_PT xiseq_copy(XISeq_PT seq) {
    return xiseq_copyn(seq, seq ? seq->array->size : 0);
}

XISeq_PT xiseq_copyn(XISeq_PT seq, int count) {
    xassert(seq);
    xassert(0 <= count);

    if (!seq || (count < 0)) {
        return NULL;
    }

    return xiseq_copyn_impl(seq, count, 0);
}

bool xiseq_push_front(XISeq_PT seq, int x) {
    xassert(seq);

    if (!seq || (seq->array->size <= seq->size)) {
        return false;
    }

    if (--seq->head < 0) {
        seq->head = seq->array->size - 1;
    }

    seq->array->datas[seq->head] = x;
    ++seq->size;

    return true;
}

int xiseq_pop_front(XISeq_PT seq) {
    xassert(seq);
    xassert(0 < seq->size);

    if (!seq || (seq->size <= 0)) {
        return 0;
    }

    {
        int x = seq->array->datas[seq->head];
        seq->array->datas[seq->head] = 0;

        seq->head = (seq->head + 1) % seq->array->size;
        --seq->size;

        return x;
    }
}

void xiseq_push_back_impl(XISeq_PT seq, int x) {
    int i = seq->size++;
    seq->array->datas[(seq->head + i) % seq->array->size] = x;
}

bool xiseq_push_back(XISeq_PT seq, int x) {
    xassert(seq);

    if (!seq || (seq->array->size <= seq->size)) {
        return false;
    }

    xiseq_push_back_impl(seq, x);

    return true;
}

int xiseq_pop_back_impl(XISeq_PT seq) {
    int i = --seq->size;

    int x = seq->array->datas[(seq->head + i) % seq->array->size];
    seq->array->datas[(seq->head + i) % seq->array->size] = 0;

    return x;
}

int xiseq_pop_back(XISeq_PT seq) {
    xassert(seq);
    xassert(0 < seq->size);

    if (!seq || (seq->size <= 0)) {
        return 0;
    }

    return xiseq_pop_back_impl(seq);
}

// in order to support xdeque_front, do not use xassert(seq)
int xiseq_front(XISeq_PT seq) {
    return seq ? (seq->size == 0 ? 0 : seq->array->datas[seq->head]) : 0;
}

// in order to support xdeque_back, do not use xassert(seq)
int xiseq_back(XISeq_PT seq) {
    return seq ? (seq->size == 0 ? 0 : seq->array->datas[(seq->head + seq->size - 1) % seq->array->size]) : 0;
}

bool xiseq_save_and_put_impl(XISeq_PT seq, int i, int x, int *old_x) {
    int k = (seq->head + i) % seq->array->size;

    if (old_x) {
        *old_x = (i < seq->size) ? (seq->array->datas[k]) : 0;
    }

    seq->array->datas[k] = x;

    return true;
}

bool xiseq_put(XISeq_PT seq, int i, int x, int *old_x) {
    xassert(seq);
    xassert(0 <= i);
    xassert(i < seq->array->size);

    if (!seq || (i < 0) || (seq->array->size <= i)) {
        return false;
    }

    return xiseq_save_and_put_impl(seq, i, x, old_x);
}

int xiseq_get(XISeq_PT seq, int i) {
    xassert(seq);
    xassert(0 <= i);
    xassert(i < seq->array->size );

    if (!seq || (i < 0) || (seq->array->size <= i)) {
        return 0;
    }

    return xiseq_get_impl(seq, i);
}

void xiseq_free(XISeq_PT *pseq) {
    if (!pseq || !*pseq) {
        return;
    }

    xiarray_free(&((*pseq)->array));
    XMEM_FREE(*pseq);
}

void xiseq_clear(XISeq_PT seq) {
    xassert(seq);

    if (!seq) {
        return;
    }

    xiarray_clear(seq->array);

    seq->size = 0;
    seq->head = 0;
}

int xiseq_size(XISeq_PT seq) {
    return (seq ? seq->size : 0);
}

bool xiseq_is_empty(XISeq_PT seq) {
    return (seq ? (seq->size == 0) : true);
}

bool xiseq_is_full(XISeq_PT seq) {
    return (seq && (seq->array->size == seq->size));
}

int xiseq_capacity(XISeq_PT seq) {
    return (seq ? seq->array->size : 0);
}

/*  xiseq_expand(XISeq_PT seq, 3) :
*     |5|6|7|0|1|2|3|4|   ->   |5|6|7|-|-|-|0|1|2|3|4|
*/
bool xiseq_expand(XISeq_PT seq, int expand_size) {
    int move_num = seq->array->size - seq->head;

    if (!xiarray_resize(seq->array, (seq->array->size + expand_size))) {
        return false;
    }

    if (0 < seq->head) {
        int new_head = seq->array->size - move_num;

        void *old = seq->array->datas + seq->head;
        void *target = seq->array->datas + new_head;

        memmove(target, old, move_num * sizeof(int));
        memset(old, 0, (new_head - seq->head) * sizeof(int));

        seq->head = new_head;
    }

    return true;
}

bool xiseq_is_sorted_impl(XISeq_PT seq, int lo, int hi, bool min_to_max) {
    if (min_to_max) {
        for (int i = lo + 1; i <= hi; ++i) {
            if ((seq->array->datas)[(seq->head + i) % seq->array->size] < (seq->array->datas)[(seq->head + i - 1) % seq->array->size]) {
                return false;
            }
        }
    }
    else {
        for (int i = lo + 1; i <= hi; ++i) {
            if ((seq->array->datas)[(seq->head + i - 1) % seq->array->size] < (seq->array->datas)[(seq->head + i) % seq->array->size]) {
                return false;
            }
        }
    }

    return true;
}

bool xiseq_is_sorted(XISeq_PT seq) {
    xassert(seq);

    if (!seq) {
        return false;
    }

    return xiseq_is_sorted_impl(seq, 0, seq->size - 1, true);
}

#define xiseq_heap_parent(npos) ((int)(((npos) - 1) >> 1))
#define xiseq_heap_left(npos)   (((npos) << 1) + 1)
#define xiseq_heap_right(npos)  (((npos) << 1) + 2)

bool xiseq_section_is_heap_sorted_impl(XISeq_PT seq, int k, int lo, int hi, bool minheap) {
    if (hi <= lo + k) {
        return true;
    }

    {
        int lpos = xiseq_heap_left(k);
        int rpos = xiseq_heap_right(k);

        /* check left branch */
        if (hi < lo + lpos) {
            return true;
        }

        if (minheap) {
            if (seq->array->datas[(seq->head + lo + lpos) % seq->array->size] < seq->array->datas[(seq->head + lo + k) % seq->array->size]) {
                return false;
            }
        }
        else {
            if (seq->array->datas[(seq->head + lo + k) % seq->array->size] < seq->array->datas[(seq->head + lo + lpos) % seq->array->size]) {
                return false;
            }
        }

        /* check right branch */
        if (hi < lo + rpos) {
            return true;
        }

        if (minheap) {
            if (seq->array->datas[(seq->head + lo + rpos) % seq->array->size] < seq->array->datas[(seq->head + lo + k) % seq->array->size]) {
                return false;
            }
        }
        else {
            if (seq->array->datas[(seq->head + lo + k) % seq->array->size] < seq->array->datas[(seq->head + lo + rpos) % seq->array->size]) {
                return false;
            }
        }

        /* continue to check the children of left and right branches */
        return xiseq_section_is_heap_sorted_impl(seq, lpos, lo, hi, minheap) && xiseq_section_is_heap_sorted_impl(seq, rpos, lo, hi, minheap);
    }
}

/* << Algorithms >> Fourth Edition.chapter 2.4.4.1 */
void xiseq_heapify_swim_elem(XISeq_PT seq, int k, int lo, int hi, bool minheap) {
    int parent = lo + xiseq_heap_parent(k);

    while (lo <= parent) {
        if (minheap) {
            /* parent is smaller than k, do nothing */
            if (seq->array->datas[(seq->head + parent) % seq->array->size] < seq->array->datas[(seq->head + lo + k) % seq->array->size]) {
                break;
            }
        }
        else {
            /* parent is bigger than k, do nothing */
            if (seq->array->datas[(seq->head + lo + k) % seq->array->size] < seq->array->datas[(seq->head + parent) % seq->array->size]) {
                break;
            }
        }

        /* exchange the parent with k */
        xiseq_exch_impl(seq, (lo + k), parent);

        /* prepare the next round compare */
        k = parent - lo;
        parent = lo + xiseq_heap_parent(k);
    }
}

/* << Algorithms >> Fourth Edition.chapter 2.4.4.2 */
void xiseq_heapify_sink_elem(XISeq_PT seq, int k, int lo, int hi, bool minheap) {
    int left_child = 0;
    int right_child = 0;
    int child = 0;

    while (true) {
        left_child = lo + xiseq_heap_left(k);
        if (hi < left_child) {
            break;
        }

        right_child = left_child + 1;
        child = left_child;

        if (minheap) {
            /* right child is smaller than left child */
            if ((right_child <= hi) && (seq->array->datas[(seq->head + right_child) % seq->array->size] < seq->array->datas[(seq->head + left_child) % seq->array->size])) {
                child = right_child;
            }

            /* k is equal or smaller than child, do noting */
            if (seq->array->datas[(seq->head + lo + k) % seq->array->size] <= seq->array->datas[(seq->head + child) % seq->array->size]) {
                break;
            }
        }
        else {
            /* right child is bigger than left child */
            if ((right_child <= hi) && (seq->array->datas[(seq->head + left_child) % seq->array->size] < seq->array->datas[(seq->head + right_child) % seq->array->size])) {
                child = right_child;
            }

            /* bigger child is equal or smaller than k, do noting */
            if (seq->array->datas[(seq->head + child) % seq->array->size] <= seq->array->datas[(seq->head + lo + k) % seq->array->size]) {
                break;
            }
        }

        /* change the big child with k */
        xiseq_exch_impl(seq, (lo + k), child);

        /* prepare the next round compare */
        k = child - lo;
    }
}

/* << Algorithms >> Fourth Edition.chapter 2.4.5.1 */
bool xiseq_heapify_impl(XISeq_PT seq, int lo, int hi, bool minheap) {
    /* just need to scan half of the array */
    for (int k = (hi - lo) / 2; 0 <= k; --k) {
        xiseq_heapify_sink_elem(seq, k, lo, hi, minheap);
    }

    xassert(xiseq_section_is_heap_sorted_impl(seq, 0, lo, hi, minheap));

    return true;
}

/* min heap */
bool xiseq_heapify_min(XISeq_PT seq) {
    xassert(seq);

    if (!seq) {
        return false;
    }

    if (seq->size <= 1) {
        return true;
    }

    return xiseq_heapify_impl(seq, 0, seq->size - 1, true);
}

/* max heap */
bool xiseq_heapify_max(XISeq_PT seq) {
    xassert(seq);

    if (!seq) {
        return false;
    }

    if (seq->size <= 1) {
        return true;
    }

    return xiseq_heapify_impl(seq, 0, seq->size - 1, false);
}

/* << Algorithms >> Fourth Edition.chapter 2.4.5 */
bool xiseq_heap_sort_impl(XISeq_PT seq, int lo, int hi, bool minheap) {
    int ohi = hi;

    /* make the scope a heap */
    xiseq_heapify_impl(seq, lo, hi, !minheap);

    /* make the scope sorted : save the min/max element one bye one at the end of the scope */
    {
        while (lo < hi) {
            xiseq_exch_impl(seq, lo, hi);
            --hi;
            xiseq_heapify_sink_elem(seq, 0, lo, hi, !minheap);
        }
    }

    xassert(xiseq_is_sorted_impl(seq, lo, ohi, minheap));

    return true;
}

bool xiseq_heap_sort(XISeq_PT seq) {
    xassert(seq);

    if (!seq) {
        return false;
    }

    if (seq->size <= 1) {
        return true;
    }

    return xiseq_heap_sort_impl(seq, 0, seq->size - 1, true);
}

static
void xiseq_insert_sort_impl(XISeq_PT seq, int lo, int step, int hi) {
    for (int i = lo + step; i <= hi; ++i) {
        int x = seq->array->datas[(seq->head + i) % seq->array->size];

        int j = i - step;
        for (; lo <= j; j -= step) {
            if ((seq->array->datas)[(seq->head + j) % seq->array->size] <= x) {
                break;
            }
            seq->array->datas[(seq->head + j + step) % seq->array->size] = seq->array->datas[(seq->head + j) % seq->array->size];
        }

        j += step;
        if (j < i) {
            seq->array->datas[(seq->head + j) % seq->array->size] = x;
        }
    }
}

static
void xiseq_quick_sort_impl(XISeq_PT seq, int lo, int hi, int depth_limit) {
    if (hi <= lo + 10) {
        xiseq_insert_sort_impl(seq, lo, 1, hi);
        return;
    }

    if (depth_limit == 0) {
        xiseq_heap_sort_impl(seq, lo, hi, true);
        return;
    }

    --depth_limit;

    {
        int lt = lo, i = lo + 1, gt = hi;

        int val = seq->array->datas[(seq->head + lt) % seq->array->size];
        while (i <= gt) {
            if (seq->array->datas[(seq->head + i) % seq->array->size] < val) {
                xiseq_exch_impl(seq, lt, i);
                ++lt;
                ++i;
            }
            else if (val < seq->array->datas[(seq->head + i) % seq->array->size]) {
                xiseq_exch_impl(seq, i, gt);
                --gt;
            }
            else {
                ++i;
            }
        }

        xiseq_quick_sort_impl(seq, lo, lt - 1, depth_limit);
        xiseq_quick_sort_impl(seq, gt + 1, hi, depth_limit);
    }
}

bool xiseq_quick_sort(XISeq_PT seq) {
    xassert(seq);

    if (!seq) {
        return false;
    }

    if (seq->size <= 1) {
        return true;
    }

    xiseq_quick_sort_impl(seq, 0, seq->size - 1, xiarith_lg(seq->size - 1) * 2);

    xassert(xiseq_is_sorted(seq));

    return true;
}
