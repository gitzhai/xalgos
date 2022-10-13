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
*       <<C Interfaces and Implementations>> David R. Hanson, chapter 11
*/

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../utils/xutils.h"
#include "../include/xarith_int.h"
#include "../array_pointer/xarray_pointer_x.h"
#include "xqueue_sequence_x.h"

XPSeq_PT xpseq_new(int capacity) {
    xassert(0 < capacity);

    if (capacity <= 0) {
        return NULL;
    }

    {
        XPSeq_PT seq = XMEM_CALLOC(1, sizeof(*seq));
        if (!seq) {
            return NULL;
        }

        seq->array = xparray_new(capacity);
        if (!seq->array) {
            XMEM_FREE(seq);
            return NULL;
        }

        //seq->size = 0;
        //seq->head = 0;

        return seq;
    }
}

XPSeq_PT xpseq_copyn_impl(XPSeq_PT seq, int count, int elem_size, bool deep) {
    if (seq->size < count) {
        count = seq->size;
    }

    {
        XPSeq_PT nseq = XMEM_CALLOC(1, sizeof(*seq));
        if (!nseq) {
            return NULL;
        }

        if (seq->head == 0) {
            nseq->array = deep ? xparray_deep_copyn(seq->array, count, elem_size) : xparray_copyn(seq->array, count);
        }
        else {
            if (count == 0) {
                nseq->array = xparray_new(count);
            }
            else if (count <= (seq->array->size - seq->head)) {
                nseq->array = deep ? xparray_scope_deep_copy(seq->array, seq->head, seq->head + count - 1, elem_size) : xparray_scope_copy(seq->array, seq->head, seq->head + count - 1);
            }
            else {
                nseq->array = deep ? xparray_scope_deep_copy(seq->array, seq->head, seq->array->size - 1, elem_size) : xparray_scope_copy(seq->array, seq->head, seq->array->size - 1);
                if (!nseq->array) {
                    XMEM_FREE(nseq);
                    return NULL;
                }
                nseq->size = seq->array->size - seq->head;

                if (deep) {
                    if (!xparray_scope_deep_index_copy_resize(seq->array, 0, (count - (seq->array->size - seq->head) - 1), nseq->array, nseq->size, elem_size)) {
                        xpseq_deep_free(&nseq);
                        return NULL;
                    }
                }
                else {
                    if (!xparray_scope_index_copy_resize(seq->array, 0, (count - (seq->array->size - seq->head) - 1), nseq->array, nseq->size)) {
                        xpseq_free(&nseq);
                        return NULL;
                    }
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

XPSeq_PT xpseq_copy(XPSeq_PT seq) {
    return xpseq_copyn(seq, seq ? seq->array->size : 0);
}

XPSeq_PT xpseq_copyn(XPSeq_PT seq, int count) {
    xassert(seq);
    xassert(0 <= count);

    if (!seq || (count < 0)) {
        return NULL;
    }

    return xpseq_copyn_impl(seq, count, 0, false);
}

XPSeq_PT xpseq_deep_copy(XPSeq_PT seq, int elem_size) {
    return xpseq_deep_copyn(seq, seq ? seq->array->size : 0, elem_size);
}

XPSeq_PT xpseq_deep_copyn(XPSeq_PT seq, int count, int elem_size) {
    xassert(seq);
    xassert(0 <= count);
    xassert(0 < elem_size);

    if (!seq || (count < 0) || (elem_size <= 0)) {
        return NULL;
    }

    return xpseq_copyn_impl(seq, count, elem_size, true);
}

int xpseq_vload(XPSeq_PT seq, void *x, ...) {
    xassert(seq);

    if (!seq) {
        return 0;
    }

    {
        int count = seq->size;

        va_list ap;
        va_start(ap, x);
        for (; x; x = va_arg(ap, void *)) {
            if (!xpseq_push_back(seq, x)) {
                break;
            }
        }
        va_end(ap);

        return seq->size - count;
    }
}

int xpseq_aload(XPSeq_PT seq, XPArray_PT xs) {
    xassert(seq);
    xassert(xs);

    if (!seq || !xs) {
        return 0;
    }

    {
        int count = seq->size;

        for (int i = 0; i < xs->size; ++i) {
            /* ignore the NULL element */
            void *value = xparray_get_impl(xs, i);
            if (!value) {
                continue;
            }

            if (!xpseq_push_back(seq, value)) {
                break;
            }
        }

        return seq->size - count;
    }
}

bool xpseq_push_front(XPSeq_PT seq, void *x) {
    xassert(seq);
    xassert(x);

    if (!seq || !x || (seq->array->size <= seq->size)) {
        return false;
    }

    if (--seq->head < 0) {
        seq->head = seq->array->size - 1;
    }

    seq->array->datas[seq->head] = x;
    ++seq->size;

    return true;
}

void* xpseq_pop_front(XPSeq_PT seq) {
    xassert(seq);
    xassert(0 < seq->size);

    if (!seq || (seq->size <= 0)) {
        return NULL;
    }

    {
        void *x = seq->array->datas[seq->head];
        seq->array->datas[seq->head] = NULL;

        seq->head = (seq->head + 1) % seq->array->size;
        --seq->size;

        return x;
    }
}

bool xpseq_push_back(XPSeq_PT seq, void *x) {
    xassert(seq);
    xassert(x);

    if (!seq || !x || (seq->array->size <= seq->size)) {
        return false;
    }
	
    {
        int i = seq->size++;
        seq->array->datas[(seq->head + i) % seq->array->size] = x;
    }

    return true;
}

void* xpseq_pop_back(XPSeq_PT seq) {
    xassert(seq);
    xassert(0 < seq->size);

    if (!seq || (seq->size <= 0)) {
        return NULL;
    }

    {
        int i = --seq->size;

        void *x = seq->array->datas[(seq->head + i) % seq->array->size];
        seq->array->datas[(seq->head + i) % seq->array->size] = NULL;

        return x;
    }
}

void* xpseq_front(XPSeq_PT seq) {
    return seq ? ((seq->size == 0) ? NULL : seq->array->datas[seq->head]) : NULL;
}

void* xpseq_back(XPSeq_PT seq) {
    return seq ? ((seq->size == 0) ? NULL : seq->array->datas[(seq->head + seq->size - 1) % seq->array->size]) : NULL;
}

bool xpseq_save_and_put_impl(XPSeq_PT seq, int i, void *x, void **old_x) {
    int k = (seq->head + i) % seq->array->size;

    if (old_x) {
        *old_x = seq->array->datas[k];
    }

    seq->array->datas[k] = x;

    return true;
}

bool xpseq_put(XPSeq_PT seq, int i, void *x, void **old_x) {
    xassert(seq);
    xassert(0 <= i);
    xassert(i < seq->array->size);

    if (!seq || (i < 0) || (seq->array->size <= i)) {
        return false;
    }

    return xpseq_save_and_put_impl(seq, i, x, old_x);
}

void* xpseq_get(XPSeq_PT seq, int i) {
    xassert(seq);
    xassert(0 <= i);
    xassert(i < seq->array->size );

    if (!seq || (i < 0) || (seq->array->size <= i)) {
        return NULL;
    }

    return xpseq_get_impl(seq, i);
}

int xpseq_map_impl(XPSeq_PT seq, bool break_first, bool break_true, bool (*apply)(void *x, void *cl), void *cl) {
    xassert(seq);
    xassert(apply);

    if (!seq || !apply) {
        return 0;
    }

    {
        int count = 0;

        for (int i = 0; i < seq->size; i++) {
            bool ret = apply(seq->array->datas[(seq->head + i) % seq->array->size], cl);

            if (break_first) {
                if (ret && break_true) {
                    count++;
                    break;
                }

                if (!ret && !break_true) {
                    count++;
                    break;
                }
            }
            else {
                ret ? count++ : 0;
            }
        }

        return count;
    }
}

int xpseq_map(XPSeq_PT seq, bool (*apply)(void *x, void *cl), void *cl) {
    return xpseq_map_impl(seq, false, false, apply, cl);
}

bool xpseq_map_break_if_true(XPSeq_PT seq, bool (*apply)(void *x, void *cl), void *cl) {
    return (0 < xpseq_map_impl(seq, true, true, apply, cl));
}

bool xpseq_map_break_if_false(XPSeq_PT seq, bool (*apply)(void *x, void *cl), void *cl) {
    return (0 < xpseq_map_impl(seq, true, false, apply, cl));
}

static
void xpseq_free_impl(XPSeq_PT *pseq, bool deep, bool (*apply)(void *x, void *cl), void *cl) {
    if (!pseq || !*pseq) {
        return;
    }

    if (deep || apply) {
        for (int i = 0; i < (*pseq)->size; i++) {
            void *ptr = (*pseq)->array->datas[((*pseq)->head + i) % (*pseq)->array->size];
            deep ? XMEM_FREE(ptr) : apply(ptr, cl);
        }
    }

    xparray_free(&((*pseq)->array));
    XMEM_FREE(*pseq);
}

void xpseq_free(XPSeq_PT *pseq) {
    xpseq_free_impl(pseq, false, NULL, NULL);
}

void xpseq_free_apply(XPSeq_PT *pseq, bool (*apply)(void *x, void *cl), void *cl) {
    xpseq_free_impl(pseq, false, apply, cl);
}

void xpseq_deep_free(XPSeq_PT *pseq) {
    xpseq_free_impl(pseq, true, NULL, NULL);
}

static
void xpseq_clear_impl(XPSeq_PT seq, bool deep, bool (*apply)(void *x, void *cl), void *cl) {
    xassert(seq);

    if (!seq) {
        return;
    }

    if (deep || apply) {
        for (int i = 0; i < seq->size; i++) {
            void *ptr = seq->array->datas[(seq->head + i) % seq->array->size];
            deep ? XMEM_FREE(ptr) : apply(ptr, cl);
        }
    }
    else {
        xparray_clear(seq->array);
    }

    seq->size = 0;
    seq->head = 0;
}

void xpseq_clear(XPSeq_PT seq) {
    xpseq_clear_impl(seq, false, NULL, NULL);
}

void xpseq_clear_apply(XPSeq_PT seq, bool (*apply)(void *x, void *cl), void *cl) {
    xpseq_clear_impl(seq, false, apply, cl);
}

void xpseq_deep_clear(XPSeq_PT seq) {
    xpseq_clear_impl(seq, true, NULL, NULL);
}

int xpseq_size(XPSeq_PT seq) {
    return (seq ? seq->size : 0);
}

int xpseq_capacity(XPSeq_PT seq) {
    return (seq ? seq->array->size : 0);
}

/*  xpseq_expand(XPSeq_PT seq, 3) :
*     |5|6|7|0|1|2|3|4|   ->   |5|6|7|-|-|-|0|1|2|3|4|
*/
bool xpseq_expand(XPSeq_PT seq, int expand_size) {
    int move_num = seq->array->size - seq->head;

    if (!xparray_resize(seq->array, (seq->array->size + expand_size))) {
        return false;
    }

    if (0 < seq->head) {
        int new_head = seq->array->size - move_num;

        void **old = seq->array->datas + seq->head;
        void **target = seq->array->datas + new_head;

        memmove(target, old, move_num * sizeof(void *));
        memset(old, 0, (new_head - seq->head) * sizeof(void *));

        seq->head = new_head;
    }

    return true;
}

bool xpseq_is_empty(XPSeq_PT seq) {
    return (seq ? (seq->size == 0) : true);
}

bool xpseq_is_full(XPSeq_PT seq) {
    return (seq && (seq->array->size == seq->size));
}

bool xpseq_swap(XPSeq_PT seq1, XPSeq_PT seq2) {
    xassert(seq1);
    xassert(seq2);

    if (!seq1 || !seq2) {
        return false;
    }

    {
        int size = seq1->size;
        int head = seq1->head;
        XPArray_PT array = seq1->array;

        seq1->size = seq2->size;
        seq1->head = seq2->head;
        seq1->array = seq2->array;

        seq2->size = size;
        seq2->head = head;
        seq2->array = array;
    }

    return true;
}

static inline
void xpseq_exch_impl(XPSeq_PT seq, int i, int j) {
    void *x = seq->array->datas[(seq->head + i) % seq->array->size];
    seq->array->datas[(seq->head + i) % seq->array->size] = seq->array->datas[(seq->head + j) % seq->array->size];
    seq->array->datas[(seq->head + j) % seq->array->size] = x;
}

bool xpseq_is_sorted_impl(XPSeq_PT seq, int lo, int hi, bool min_to_max, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    if (min_to_max) {
        for (int i = lo + 1; i <= hi; ++i) {
            if (cmp((seq->array->datas)[(seq->head + i) % seq->array->size], (seq->array->datas)[(seq->head + i - 1) % seq->array->size], cl) < 0) {
                return false;
            }
        }
    }
    else {
        for (int i = lo + 1; i <= hi; ++i) {
            if (cmp((seq->array->datas)[(seq->head + i - 1) % seq->array->size], (seq->array->datas)[(seq->head + i) % seq->array->size], cl) < 0) {
                return false;
            }
        }
    }

    return true;
}

bool xpseq_is_sorted(XPSeq_PT seq, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(seq);
    xassert(cmp);

    if (!seq || !cmp) {
        return false;
    }

    return xpseq_is_sorted_impl(seq, 0, seq->size - 1, true, cmp, cl);
}

#define xpseq_heap_parent(npos) ((int)(((npos) - 1) >> 1))
#define xpseq_heap_left(npos)   (((npos) << 1) + 1)
#define xpseq_heap_right(npos)  (((npos) << 1) + 2)

bool xpseq_section_is_heap_sorted_impl(XPSeq_PT seq, int k, int lo, int hi, bool minheap, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    if (hi <= lo + k) {
        return true;
    }

    {
        int lpos = xpseq_heap_left(k);
        int rpos = xpseq_heap_right(k);

        /* check left branch */
        if (hi < lo + lpos) {
            return true;
        }

        if (minheap) {
            if (cmp(seq->array->datas[(seq->head + lo + lpos) % seq->array->size], seq->array->datas[(seq->head + lo + k) % seq->array->size], cl) < 0) {
                return false;
            }
        }
        else {
            if (cmp(seq->array->datas[(seq->head + lo + k) % seq->array->size], seq->array->datas[(seq->head + lo + lpos) % seq->array->size], cl) < 0) {
                return false;
            }
        }

        /* check right branch */
        if (hi < lo + rpos) {
            return true;
        }

        if (minheap) {
            if (cmp(seq->array->datas[(seq->head + lo + rpos) % seq->array->size], seq->array->datas[(seq->head + lo + k) % seq->array->size], cl) < 0) {
                return false;
            }
        }
        else {
            if (cmp(seq->array->datas[(seq->head + lo + k) % seq->array->size], seq->array->datas[(seq->head + lo + rpos) % seq->array->size], cl) < 0) {
                return false;
            }
        }

        /* continue to check the children of left and right branches */
        return xpseq_section_is_heap_sorted_impl(seq, lpos, lo, hi, minheap, cmp, cl) && xpseq_section_is_heap_sorted_impl(seq, rpos, lo, hi, minheap, cmp, cl);
    }
}

/* << Algorithms >> Fourth Edition.chapter 2.4.4.1 */
void xpseq_heapify_swim_elem(XPSeq_PT seq, int k, int lo, int hi, bool minheap, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    int parent = lo + xpseq_heap_parent(k);

    while (lo <= parent) {
        if (minheap) {
            /* parent is smaller than k, do nothing */
            if (cmp(seq->array->datas[(seq->head + parent) % seq->array->size], seq->array->datas[(seq->head + lo + k) % seq->array->size], cl) <= 0) {
                break;
            }
        }
        else {
            /* parent is bigger than k, do nothing */
            if (cmp(seq->array->datas[(seq->head + lo + k) % seq->array->size], seq->array->datas[(seq->head + parent) % seq->array->size], cl) <= 0) {
                break;
            }
        }

        /* exchange the parent with k */
        xpseq_exch_impl(seq, (lo + k), parent);

        /* prepare the next round compare */
        k = parent - lo;
        parent = lo + xpseq_heap_parent(k);
    }
}

/* << Algorithms >> Fourth Edition.chapter 2.4.4.2 */
void xpseq_heapify_sink_elem(XPSeq_PT seq, int k, int lo, int hi, bool minheap, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    int left_child = 0;
    int right_child = 0;
    int child = 0;

    while (true) {
        left_child = lo + xpseq_heap_left(k);
        if (hi < left_child) {
            break;
        }

        right_child = left_child + 1;
        child = left_child;

        if (minheap) {
            /* right child is smaller than left child */
            if ((right_child <= hi) && (cmp(seq->array->datas[(seq->head + right_child) % seq->array->size], seq->array->datas[(seq->head + left_child) % seq->array->size], cl) < 0)) {
                child = right_child;
            }

            /* k is smaller than child, do noting */
            if (cmp(seq->array->datas[(seq->head + lo + k) % seq->array->size], seq->array->datas[(seq->head + child) % seq->array->size], cl) <= 0) {
                break;
            }
        }
        else {
            /* right child is bigger than left child */
            if ((right_child <= hi) && (cmp(seq->array->datas[(seq->head + left_child) % seq->array->size], seq->array->datas[(seq->head + right_child) % seq->array->size], cl) < 0)) {
                child = right_child;
            }

            /* bigger child is smaller than k, do noting */
            if (cmp(seq->array->datas[(seq->head + child) % seq->array->size], seq->array->datas[(seq->head + lo + k) % seq->array->size], cl) <= 0) {
                break;
            }
        }

        /* change the big child with k */
        xpseq_exch_impl(seq, (lo + k), child);

        /* prepare the next round compare */
        k = child - lo;
    }
}

/* << Algorithms >> Fourth Edition.chapter 2.4.5.1 */
bool xpseq_heapify_impl(XPSeq_PT seq, int lo, int hi, bool minheap, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    /* just need to scan half of the array */
    for (int k = (hi - lo) / 2; 0 <= k; --k) {
        xpseq_heapify_sink_elem(seq, k, lo, hi, minheap, cmp, cl);
    }

    xassert(xpseq_section_is_heap_sorted_impl(seq, 0, lo, hi, minheap, cmp, cl));

    return true;
}

/* min heap */
bool xpseq_heapify_min(XPSeq_PT seq, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(seq);
    xassert(cmp);

    if (!seq || !cmp) {
        return false;
    }

    if (seq->size <= 1) {
        return true;
    }

    return xpseq_heapify_impl(seq, 0, seq->size - 1, true, cmp, cl);
}

/* max heap */
bool xpseq_heapify_max(XPSeq_PT seq, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(seq);
    xassert(cmp);

    if (!seq || !cmp) {
        return false;
    }

    if (seq->size <= 1) {
        return true;
    }

    return xpseq_heapify_impl(seq, 0, seq->size - 1, false, cmp, cl);
}

/* << Algorithms >> Fourth Edition.chapter 2.4.5 */
bool xpseq_heap_sort_impl(XPSeq_PT seq, int lo, int hi, bool minheap, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    int ohi = hi;

    /* make the scope a heap */
    xpseq_heapify_impl(seq, lo, hi, !minheap, cmp, cl);

    /* make the scope sorted : save the min/max element one bye one at the end of the scope */
    {
        while (lo < hi) {
            xpseq_exch_impl(seq, lo, hi);
            --hi;
            xpseq_heapify_sink_elem(seq, 0, lo, hi, !minheap, cmp, cl);
        }
    }

    xassert(xpseq_is_sorted_impl(seq, lo, ohi, minheap, cmp, cl));

    return true;
}

bool xpseq_heap_sort(XPSeq_PT seq, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(seq);
    xassert(cmp);

    if (!seq || !cmp) {
        return false;
    }

    if (seq->size <= 1) {
        return true;
    }

    return xpseq_heap_sort_impl(seq, 0, seq->size - 1, true, cmp, cl);
}

static
void xpseq_insert_sort_impl(XPSeq_PT seq, int lo, int step, int hi, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    for (int i = lo + step; i <= hi; ++i) {
        void *x = seq->array->datas[(seq->head + i) % seq->array->size];

        int j = i - step;
        for (; lo <= j; j -= step) {
            if (cmp((seq->array->datas)[(seq->head + j) % seq->array->size], x, cl) <= 0) {
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
void xpseq_quick_sort_impl(XPSeq_PT seq, int lo, int hi, int depth_limit, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    if (hi <= lo + 10) {
        xpseq_insert_sort_impl(seq, lo, 1, hi, cmp, cl);
        return;
    }

    if (depth_limit == 0) {
        xpseq_heap_sort_impl(seq, lo, hi, true, cmp, cl);
        return;
    }

    --depth_limit;

    {
        int lt = lo, i = lo + 1, gt = hi;

        void *val = seq->array->datas[(seq->head + lt) % seq->array->size];
        while (i <= gt) {
            int ret = cmp(seq->array->datas[(seq->head + i) % seq->array->size], val, cl);
            if (ret < 0) {
                xpseq_exch_impl(seq, lt, i);
                ++lt;
                ++i;
            }
            else if (0 < ret) {
                xpseq_exch_impl(seq, i, gt);
                --gt;
            }
            else {
                ++i;
            }
        }

        xpseq_quick_sort_impl(seq, lo, lt - 1, depth_limit, cmp, cl);
        xpseq_quick_sort_impl(seq, gt + 1, hi, depth_limit, cmp, cl);
    }
}

bool xpseq_quick_sort(XPSeq_PT seq, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(seq);
    xassert(cmp);

    if (!seq || !cmp) {
        return false;
    }

    if (seq->size <= 1) {
        return true;
    }

    xpseq_quick_sort_impl(seq, 0, seq->size - 1, xiarith_lg(seq->size - 1) * 2, cmp, cl);

    xassert(xpseq_is_sorted(seq, cmp, cl));

    return true;
}
