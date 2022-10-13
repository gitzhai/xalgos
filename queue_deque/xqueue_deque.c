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

#include <stddef.h>
#include <stdarg.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../utils/xutils.h"
#include "../include/xarith_int.h"
#include "../array_pointer/xarray_pointer_x.h"
#include "../queue_sequence/xqueue_sequence_x.h"
#include "xqueue_deque_x.h"

XDeque_PT xdeque_new(int capacity) {
    xassert(0 <= capacity);

    if (capacity < 0) {
        return NULL;
    }

    {
        XDeque_PT deque = XMEM_CALLOC(1, sizeof(*deque));
        if (!deque) {
            return NULL;
        }

        deque->layer1_seq = xpseq_new((capacity == 0) ? XUTILS_DEQUE_LAYER1_DEFAULT_LENGTH : capacity);
        if (!deque->layer1_seq) {
            XMEM_FREE(deque);
            return NULL;
        }
        
        deque->size = 0;
        deque->capacity = capacity;
        deque->discard_strategy = XUTILS_QUEUE_STRATEGY_DISCARD_NEW;

        return deque;
    }
}

static
XDeque_PT xdeque_copy_impl(XDeque_PT deque, int count, int elem_size, bool deep) {
    if (deque->size < count) {
        count = deque->size;
    }

    {
        XDeque_PT ndeque = xdeque_new(deque->capacity);
        if (!ndeque) {
            return NULL;
        }

        if (count == 0) {
            return ndeque;
        }

        if (deque->capacity != 0) {
            XPSeq_PT seq = xpseq_copyn_impl(deque->layer1_seq, count, elem_size, deep);
            if (!seq) {
                xdeque_free(&ndeque);
                return NULL;
            }

            xpseq_free(&ndeque->layer1_seq);
            ndeque->layer1_seq = seq;
            ndeque->size = count;
            ndeque->capacity = count;
            ndeque->discard_strategy = deque->discard_strategy;
            return ndeque;
        }

        // copy layer 2 XPSeq_PT
        for (int i = 0; 0 < count; i++) {
            XPSeq_PT oseq = xpseq_get_impl(deque->layer1_seq, i);

            int elem_num = oseq->size <= count ? oseq->size : count;
            count -= elem_num;

            {
                XPSeq_PT nseq = deep ? xpseq_deep_copyn(oseq, elem_num, elem_size) : xpseq_copyn(oseq, elem_num);
                if (!nseq) {
                    deep ? xdeque_deep_free(&ndeque) : xdeque_free(&ndeque);
                    return NULL;
                }

                if (!xpseq_push_back(ndeque->layer1_seq, (void*)nseq)) {
                    if (deep) {
                        xpseq_deep_free(&nseq);
                        xdeque_deep_free(&ndeque);
                    }
                    else {
                        xpseq_free(&nseq);
                        xdeque_free(&ndeque);
                    }
                    return NULL;
                }

                ndeque->size += nseq->size;
            }
        }

        ndeque->discard_strategy = deque->discard_strategy;
        return ndeque;
    }
}

XDeque_PT xdeque_copy(XDeque_PT deque) {
    return xdeque_copyn(deque, (deque ? deque->size : 0));
}

XDeque_PT xdeque_copyn(XDeque_PT deque, int count) {
    xassert(deque);
    xassert(0 <= count);

    if (!deque || (count < 0)) {
        return NULL;
    }

    return xdeque_copy_impl(deque, count, 0, false);
}

XDeque_PT xdeque_deep_copy(XDeque_PT deque, int elem_size) {
    return xdeque_deep_copyn(deque, (deque ? deque->size : 0), elem_size);
}

XDeque_PT xdeque_deep_copyn(XDeque_PT deque, int count, int elem_size) {
    xassert(deque);
    xassert(0 <= count);
    xassert(0 < elem_size);

    if (!deque || (count < 0) || (elem_size <= 0)) {
        return NULL;
    }

    return xdeque_copy_impl(deque, count, elem_size, true);
}

int xdeque_vload(XDeque_PT deque, void *x, ...) {
    xassert(deque);

    if (!deque) {
        return 0;
    }

    {
        int count = deque->size;

        va_list ap;
        va_start(ap, x);
        for (; x; x = va_arg(ap, void *)) {
            if (!xdeque_push_back(deque, x)) {
                break;
            }
        }

        va_end(ap);

        return deque->size - count;
    }
}

int xdeque_aload(XDeque_PT deque, XPArray_PT xs) {
    xassert(deque);
    xassert(xs);

    if (!deque || !xs) {
        return 0;
    }

    int count = deque->size;

    for (int i = 0; i < xs->size; i++) {
        /* ignore the NULL element */
        void *value = xparray_get_impl(xs, i);
        if (!value) {
            continue;
        }

        if (!xdeque_push_back(deque, value)) {
            break;
        }
    }

    return deque->size - count;
}

bool xdeque_push_front(XDeque_PT deque, void *x) {
    xassert(deque);
    xassert(x);

    if (!deque || !x) {
        return false;
    }

    if (deque->capacity != 0) {
        if (xpseq_is_full(deque->layer1_seq)) {
            if(deque->discard_strategy == XUTILS_QUEUE_STRATEGY_DISCARD_NEW) {
                return false;
            }
            else if (deque->discard_strategy == XUTILS_QUEUE_STRATEGY_DISCARD_FRONT) {
                xpseq_pop_front(deque->layer1_seq);
                --deque->size;
            }
            else if (deque->discard_strategy == XUTILS_QUEUE_STRATEGY_DISCARD_BACK) {
                xpseq_pop_back(deque->layer1_seq);
                --deque->size;
            }
        }

        if (xpseq_push_front(deque->layer1_seq, x)) {
            ++deque->size;
            return true;
        }

        return false;
    }

    {
        // get the layer 2 front XPSeq_PT directly and try to push front to it
        XPSeq_PT seq = xpseq_front(deque->layer1_seq);
        if (seq && xpseq_push_front(seq, x)) {
            ++deque->size;
            return true;
        }

        if (xpseq_is_full(deque->layer1_seq)) {
            if (!xpseq_expand(deque->layer1_seq, XUTILS_DEQUE_LAYER1_DEFAULT_LENGTH)) {
                return false;
            }
        }

        {
            // add one new XPSeq_PT to layer 1 XPSeq_PT front
            XPSeq_PT nseq = xpseq_new(seq ? seq->array->size : XUTILS_DEQUE_LAYER2_DEFAULT_LENGTH);
            if (!nseq) {
                return false;
            }

            if (!xpseq_push_front(deque->layer1_seq, (void*)nseq)) {
                return false;
            }

            if (xpseq_push_front(nseq, x)) {
                ++deque->size;
                return true;
            }
        }

        return false;
    }
}

bool xdeque_push_front_no_limit(XDeque_PT deque, void *x) {
    xassert(deque);

    if (!deque) {
        return false;
    }

    /* when we first create a deque with a limit capacity, but we want to save elements without capacity limit actually, 
    *  in order to improve efficiency :
    *    when first layer is not full, we can push "x" to the first layer directly,
    *    there is no need to create the second layer now, we can do that later when first layer is full already.
    */
    if (deque->capacity != 0) {
        if (xpseq_is_full(deque->layer1_seq)) {
            if (!xdeque_set_capacity_no_limit(deque)) {
                return false;
            }
        }
    }

    return xdeque_push_front(deque, x);
}

void* xdeque_pop_front(XDeque_PT deque) {
    xassert(deque);

    if (!deque || (deque->size == 0)) {
        return NULL;
    }

    if (deque->capacity != 0) {
        void *ret = xpseq_pop_front(deque->layer1_seq);
        if(ret) {
            --deque->size;
        }

        return ret;
    }

    {
        // just access the layer 2 front XPSeq_PT at first
        XPSeq_PT seq = xpseq_front(deque->layer1_seq);
        if (!seq || seq->size == 0) {
            // this should not happen
            xassert(0);
            return NULL;
        }

        {
            void *ret = xpseq_pop_front(seq);
            if (!ret) {
                // this should not happen
                xassert(0);
                return NULL;
            }

            if (seq->size == 0) {  // no elements left in sequence
                seq = xpseq_pop_front(deque->layer1_seq);
                xassert(seq);
                xpseq_free(&seq);
            }

            --deque->size;

            return ret;
        }
    }
}

bool xdeque_push_back(XDeque_PT deque, void *x) {
    xassert(deque);
    xassert(x);

    if (!deque || !x) {
        return false;
    }

    if (deque->capacity != 0) {
        if (xpseq_is_full(deque->layer1_seq)) {
            if (deque->discard_strategy == XUTILS_QUEUE_STRATEGY_DISCARD_NEW) {
                return false;
            }
            else if (deque->discard_strategy == XUTILS_QUEUE_STRATEGY_DISCARD_FRONT) {
                xpseq_pop_front(deque->layer1_seq);
                --deque->size;
            }
            else if (deque->discard_strategy == XUTILS_QUEUE_STRATEGY_DISCARD_BACK) {
                xpseq_pop_back(deque->layer1_seq);
                --deque->size;
            }
        }

        if (xpseq_push_back(deque->layer1_seq, x)) {
            ++deque->size;
            return true;
        }

        return false;
    }

    {
        // get the layer 2 back XPSeq_PT directly and try to push back to it
        XPSeq_PT seq = xpseq_back(deque->layer1_seq);
        if (seq && xpseq_push_back(seq, x)) {
            deque->size++;
            return true;
        }

        if (xpseq_is_full(deque->layer1_seq)) {
            if (!xpseq_expand(deque->layer1_seq, XUTILS_DEQUE_LAYER1_DEFAULT_LENGTH)) {
                return false;
            }
        }

        // add one new XPSeq_PT to layer 1 XPSeq_PT back
        XPSeq_PT nseq = xpseq_new(seq ? seq->array->size : XUTILS_DEQUE_LAYER2_DEFAULT_LENGTH);
        if (!nseq) {
            return false;
        }

        if (!xpseq_push_back(deque->layer1_seq, (void*)nseq)) {
            return false;
        }

        // redo the push back again
        return xdeque_push_back(deque, x);
    }
}

bool xdeque_push_back_no_limit(XDeque_PT deque, void *x) {
    xassert(deque);

    if (!deque) {
        return false;
    }

    /* when we first create a deque with a limit capacity, but we want to save elements without capacity limit actually,
    *  in order to improve efficiency :
    *    when first layer is not full, we can push "x" to the first layer directly,
    *    there is no need to create the second layer now, we can do that later when first layer is full already.
    */
    if (deque->capacity != 0) {
        if (xpseq_is_full(deque->layer1_seq)) {
            if (!xdeque_set_capacity_no_limit(deque)) {
                return false;
            }
        }
    }

    return xdeque_push_back(deque, x);
}

void* xdeque_pop_back(XDeque_PT deque) {
    xassert(deque);

    if (!deque || (deque->size == 0)) {
        return NULL;
    }

    if (deque->capacity != 0) {
        void *ret = xpseq_pop_back(deque->layer1_seq);
        if (ret) {
            --deque->size;
        }

        return ret;
    }

    {
        // just access the layer 2 back XPSeq_PT at first
        XPSeq_PT seq = xpseq_back(deque->layer1_seq);
        if (!seq) {
            // this should not happen
            xassert(0);
            return NULL;
        }

        {
            void *ret = xpseq_pop_back(seq);
            if (!ret) {
                // this should not happen
                xassert(0);
                return NULL;
            }

            if (seq->size == 0) {  // no other elements left
                seq = xpseq_pop_back(deque->layer1_seq);
                xassert(seq);
                xpseq_free(&seq);
            }

            --deque->size;

            return ret;
        }
    }
}

void* xdeque_front(XDeque_PT deque) {
    if (!deque || (deque->size == 0)) {
        return NULL;
    }

    if (deque->capacity != 0) {
        return xpseq_front(deque->layer1_seq);
    }

    return xpseq_front(xpseq_front(deque->layer1_seq));
}

void* xdeque_back(XDeque_PT deque) {
    if (!deque || (deque->size == 0)) {
        return NULL;
    }

    if (deque->capacity != 0) {
        return xpseq_back(deque->layer1_seq);
    }

    return xpseq_back(xpseq_back(deque->layer1_seq));
}

bool xdeque_put_impl(XDeque_PT deque, int i, void *x, void **old_x) {
    if (deque->capacity != 0) {
        return xpseq_save_and_put_impl(deque->layer1_seq, i, x, old_x);
    }

    {
        // find the layer 2 XPSeq_PT at first
        int seq_num = 0;
        XPSeq_PT seq = xpseq_front(deque->layer1_seq);
        if (seq->size <= i) {
            seq_num = 1 + (i - seq->size) / seq->array->size;
            i -= seq->size + (seq_num - 1) * seq->array->size;
        }

        {
            XPSeq_PT seq = xpseq_get_impl(deque->layer1_seq, seq_num);
            if (!seq) {
                // should not happen
                xassert(0);
                return false;
            }

            if (seq->size <= i) {
                // should not happen
                xassert(0);
                return false;
            }

            return xpseq_save_and_put_impl(seq, i, x, old_x);
        }
    }
}

bool xdeque_put(XDeque_PT deque, int i, void *x, void **old_x) {
    xassert(deque);
    xassert(0 <= i);
    xassert(i < deque->size);
    xassert(x);

    if (!deque || (i < 0) || (deque->size <= i) || !x) {
        return false;
    }

    return xdeque_put_impl(deque, i, x, old_x);
}

void* xdeque_get_impl(XDeque_PT deque, int i) {
    if (deque->capacity != 0) {
        return xpseq_get_impl(deque->layer1_seq, i);
    }

    {
        // find the layer 2 XPSeq_PT at first
        int seq_num = 0;
        XPSeq_PT seq = xpseq_front(deque->layer1_seq);
        if (seq->size <= i) {
            seq_num = 1 + (i - seq->size) / seq->array->size;
            i -= seq->size + (seq_num - 1) * seq->array->size;
        }

        {
            XPSeq_PT seq = xpseq_get_impl(deque->layer1_seq, seq_num);
            if (!seq) {
                // should not happen
                xassert(0);
                return false;
            }

            if (seq->size <= i) {
                // should not happen
                xassert(0);
                return false;
            }

            return xpseq_get_impl(seq, i);
        }
    }
}

void* xdeque_get(XDeque_PT deque, int i) {
    xassert(deque);
    xassert(0 <= i);
    xassert(i < deque->size);

    if (!deque || (i < 0) || (deque->size <= i)) {
        return NULL;
    }

    return xdeque_get_impl(deque, i);
}

int xdeque_map_impl(XDeque_PT deque, bool break_first, bool break_true, bool (*apply)(void *x, void *cl), void *cl) {
    xassert(deque);
    xassert(apply);

    if (!deque || !apply) {
        return 0;
    }

    {
        int count = 0;

        for (int i = 0; i < deque->size; i++) {
            bool ret = apply(xdeque_get_impl(deque, i), cl);

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

int xdeque_map(XDeque_PT deque, bool (*apply)(void *x, void *cl), void *cl) {
    return xdeque_map_impl(deque, false, false, apply, cl);
}

bool xdeque_map_break_if_true(XDeque_PT deque, bool (*apply)(void *x, void *cl), void *cl) {
    return (0 < xdeque_map_impl(deque, true, true, apply, cl));
}

bool xdeque_map_break_if_false(XDeque_PT deque, bool (*apply)(void *x, void *cl), void *cl) {
    return (0 < xdeque_map_impl(deque, true, false, apply, cl));
}

static
void xdeque_free_datas_impl(XDeque_PT *pdeque, bool deep, bool (*apply)(void *x, void *cl), void *cl) {
    if (!pdeque || !*pdeque) {
        return;
    }

    // free every layer 2 XPSeq_PT
    while (xpseq_front((*pdeque)->layer1_seq)) {
        XPSeq_PT seq = xpseq_pop_front((*pdeque)->layer1_seq);
        xassert(seq);
        deep ? xpseq_deep_free(&seq) : (apply ? apply(&seq, cl) : xpseq_free(&seq));
    }
}

void xdeque_free(XDeque_PT *pdeque) {
    if (!pdeque || !*pdeque) {
        return;
    }

    if ((*pdeque)->capacity == 0) {
        xdeque_free_datas_impl(pdeque, false, NULL, NULL);
    }

    // free layer 1 XPSeq_PT
    xpseq_free(&((*pdeque)->layer1_seq));
    XMEM_FREE(*pdeque);
}

void xdeque_free_apply(XDeque_PT *pdeque, bool (*apply)(void *x, void *cl), void *cl) {
    if (!pdeque || !*pdeque) {
        return;
    }

    if ((*pdeque)->capacity == 0) {
        xdeque_free_datas_impl(pdeque, false, apply, cl);
    }

    // free layer 1 XPSeq_PT
    xpseq_free(&((*pdeque)->layer1_seq));
    XMEM_FREE(*pdeque);
}

void xdeque_deep_free(XDeque_PT *pdeque) {
    if (!pdeque || !*pdeque) {
        return;
    }

    if ((*pdeque)->capacity == 0) {
        xdeque_free_datas_impl(pdeque, true, NULL, NULL);
        xpseq_free(&((*pdeque)->layer1_seq));
    }
    else {
        xpseq_deep_free(&((*pdeque)->layer1_seq));
    }
    XMEM_FREE(*pdeque);
}

static 
void xdeque_clear_impl(XDeque_PT deque, bool deep, bool (*apply)(void *x, void *cl), void *cl) {
    xassert(deque);

    if (!deque || (deque->size == 0)) {
        return;
    }

    if (deque->capacity == 0) {
        // free every layer 2 XPSeq_PT
        xdeque_free_datas_impl(&deque, deep, apply, cl);
        xpseq_clear(deque->layer1_seq);
    }
    else {
        deep ? xpseq_deep_clear(deque->layer1_seq) : (apply ? xpseq_clear_apply(deque->layer1_seq, apply, cl) : xpseq_clear(deque->layer1_seq));
    }
    deque->size = 0;
}

void xdeque_clear(XDeque_PT deque) {
    xdeque_clear_impl(deque, false, NULL, NULL);
}

void xdeque_clear_apply(XDeque_PT deque, bool (*apply)(void *x, void *cl), void *cl) {
    xdeque_clear_impl(deque, false, apply, cl);
}

void xdeque_deep_clear(XDeque_PT deque) {
    xdeque_clear_impl(deque, true, NULL, NULL);
}

int xdeque_size(XDeque_PT deque) {
    return (deque ? deque->size : 0);
}

int xdeque_capacity(XDeque_PT deque) {
    return (deque ? deque->capacity : 0);
}

/* save the first layer XPSeq_PT into the second layer, make the capacity no limit */
bool xdeque_set_capacity_no_limit(XDeque_PT deque) {
    if (deque->capacity != 0) {
        XPSeq_PT nseq = xpseq_new(XUTILS_DEQUE_LAYER1_DEFAULT_LENGTH);
        if (!nseq) {
            return false;
        }

        if (!xpseq_push_back(nseq, (void*)deque->layer1_seq)) {
            xpseq_free(&nseq);
            return false;
        }

        deque->layer1_seq = nseq;
        deque->capacity = 0;
    }

    return true;
}

bool xdeque_is_empty(XDeque_PT deque) {
    return (deque ? (deque->size == 0) : true);
}

bool xdeque_swap(XDeque_PT deque1, XDeque_PT deque2) {
    xassert(deque1);
    xassert(deque2);

    if (!deque1 || !deque2) {
        return false;
    }

    {
        int size = deque1->size;
        int capacity = deque1->capacity;
        int strategy = deque1->discard_strategy;
        XPSeq_PT layer1_seq = deque1->layer1_seq;

        deque1->size = deque2->size;
        deque1->capacity = deque2->capacity;
        deque1->discard_strategy = deque2->discard_strategy;
        deque1->layer1_seq = deque2->layer1_seq;

        deque2->size = size;
        deque2->capacity = capacity;
        deque2->discard_strategy = strategy;
        deque2->layer1_seq = layer1_seq;
    }

    return true;
}

void xdeque_exch_impl(XDeque_PT deque, int i, int j, void *cl) {
    void *x = NULL;
    xdeque_put_impl(deque, i, xdeque_get_impl(deque, j), &x);
    xdeque_put_impl(deque, j, x, NULL);
}

static 
bool xdeque_is_sorted_impl(XDeque_PT deque, int lo, int hi, bool min_to_max, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    if (min_to_max) {
        for (int i = lo + 1; i <= hi; ++i) {
            if (cmp(xdeque_get_impl(deque, i), xdeque_get_impl(deque, i - 1), cl) < 0) {
                return false;
            }
        }
    }
    else {
        for (int i = lo + 1; i <= hi; ++i) {
            if (cmp(xdeque_get_impl(deque, i - 1), xdeque_get_impl(deque, i), cl) < 0) {
                return false;
            }
        }
    }

    return true;
}

bool xdeque_is_sorted(XDeque_PT deque, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(deque);
    xassert(cmp);

    if (!deque || !cmp) {
        return false;
    }

    return xdeque_is_sorted_impl(deque, 0, deque->size - 1, true, cmp, cl);
}

#define xdeque_heap_parent(npos) ((int)(((npos) - 1) >> 1))
#define xdeque_heap_left(npos)   (((npos) << 1) + 1)
#define xdeque_heap_right(npos)  (((npos) << 1) + 2)

bool xdeque_is_heap_sorted_impl(XDeque_PT deque, int k, int lo, int hi, bool minheap, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    if (hi <= lo + k) {
        return true;
    }

    {
        int lpos = xdeque_heap_left(k);
        int rpos = xdeque_heap_right(k);

        /* check left branch */
        if (hi < lo + lpos) {
            return true;
        }

        if (minheap) {
            if (cmp(xdeque_get_impl(deque, lo + lpos), xdeque_get_impl(deque, lo + k), cl) < 0) {
                return false;
            }
        }
        else {
            if (cmp(xdeque_get_impl(deque, lo + k), xdeque_get_impl(deque, lo + lpos), cl) < 0) {
                return false;
            }
        }

        /* check right branch */
        if (hi < lo + rpos) {
            return true;
        }

        if (minheap) {
            if (cmp(xdeque_get_impl(deque, lo + rpos), xdeque_get_impl(deque, lo + k), cl) < 0) {
                return false;
            }
        }
        else {
            if (cmp(xdeque_get_impl(deque, lo + k), xdeque_get_impl(deque, lo + rpos), cl) < 0) {
                return false;
            }
        }

        /* continue to check the children of left and right branches */
        return xdeque_is_heap_sorted_impl(deque, lpos, lo, hi, minheap, cmp, cl) && xdeque_is_heap_sorted_impl(deque, rpos, lo, hi, minheap, cmp, cl);
    }
}

/* << Algorithms >> Fourth Edition.chapter 2.4.4.1 */
void xdeque_heapify_swim_elem(XDeque_PT deque, int k, int lo, int hi, bool minheap, int(*cmp)(void *x, void *y, void *cl2), void *cl2) {
    int parent = lo + xdeque_heap_parent(k);

    while (lo <= parent) {
        if (minheap) {
            /* parent is smaller than k, do nothing */
            if (cmp(xdeque_get_impl(deque, parent), xdeque_get_impl(deque, lo + k), cl2) <= 0) {
                break;
            }
        }
        else {
            /* parent is bigger than k, do nothing */
            if (cmp(xdeque_get_impl(deque, lo + k), xdeque_get_impl(deque, parent), cl2) <= 0) {
                break;
            }
        }

        /* exchange the parent with k */
        xdeque_exch_impl(deque, (lo + k), parent, NULL);

        /* prepare the next round compare */
        k = parent - lo;
        parent = lo + xdeque_heap_parent(k);
    }
}

/* << Algorithms >> Fourth Edition.chapter 2.4.4.2 */
void xdeque_heapify_sink_elem(XDeque_PT deque, int k, int lo, int hi, bool minheap, int(*cmp)(void *x, void *y, void *cl2), void *cl2) {
    int left_child = 0;
    int right_child = 0;
    int child = 0;

    while (true) {
        left_child = lo + xdeque_heap_left(k);
        if (hi < left_child) {
            break;
        }

        right_child = left_child + 1;
        child = left_child;

        if (minheap) {
            /* right child is bigger than left child */
            if ((right_child <= hi) && (cmp(xdeque_get_impl(deque, right_child), xdeque_get_impl(deque, left_child), cl2) < 0)) {
                child = right_child;
            }

            /* k is equal or smaller than child, do noting */
            if (cmp(xdeque_get_impl(deque, lo + k), xdeque_get_impl(deque, child), cl2) <= 0) {
                break;
            }
        }
        else {
            /* right child is bigger than left child */
            if ((right_child <= hi) && (cmp(xdeque_get_impl(deque, left_child), xdeque_get_impl(deque, right_child), cl2) < 0)) {
                child = right_child;
            }

            /* big child is equal or smaller than k, do noting */
            if (cmp(xdeque_get_impl(deque, child), xdeque_get_impl(deque, lo + k), cl2) <= 0) {
                break;
            }
        }

        /* change the big child with k */
        xdeque_exch_impl(deque, (lo + k), child, NULL);

        /* prepare the next round compare */
        k = child - lo;
    }
}

/* << Algorithms >> Fourth Edition.chapter 2.4.5.1 */
bool xdeque_heapify_impl(XDeque_PT deque, int lo, int hi, bool minheap, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    /* just need to scan half of the array */
    for (int k = (hi - lo) / 2; 0 <= k; --k) {
        xdeque_heapify_sink_elem(deque, k, lo, hi, minheap, cmp, cl);
    }

    xassert(xdeque_is_heap_sorted_impl(deque, 0, lo, hi, minheap, cmp, cl));

    return true;
}

/* min heap */
bool xdeque_heapify_min(XDeque_PT deque, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(deque);
    xassert(cmp);

    if (!deque || !cmp) {
        return false;
    }

    if (deque->size <= 1) {
        return true;
    }

    return xdeque_heapify_impl(deque, 0, deque->size - 1, true, cmp, cl);
}

/* max heap */
bool xdeque_heapify_max(XDeque_PT deque, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(deque);
    xassert(cmp);

    if (!deque || !cmp) {
        return false;
    }

    if (deque->size <= 1) {
        return true;
    }

    return xdeque_heapify_impl(deque, 0, deque->size - 1, false, cmp, cl);
}

bool xdeque_sort_after_heapify_impl(XDeque_PT deque, int lo, int hi, bool minheap, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    /* save the front element one bye one at the end of the deque */
    while (lo < hi) {
        xdeque_exch_impl(deque, lo, hi, NULL);
        --hi;
        xdeque_heapify_sink_elem(deque, 0, lo, hi, !minheap, cmp, cl);
    }

    xassert(xdeque_is_sorted_impl(deque, lo, hi, minheap, cmp, cl));

    return true;
}

/* << Algorithms >> Fourth Edition.chapter 2.4.5 */
bool xdeque_heap_sort_impl(XDeque_PT deque, int lo, int hi, bool minheap, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    /* make the deque a heap */
    xdeque_heapify_impl(deque, lo, hi, !minheap, cmp, cl);

    /* save the front element one bye one at the end of the deque */
    return xdeque_sort_after_heapify_impl(deque, lo, hi, minheap, cmp, cl);
}

bool xdeque_heap_sort(XDeque_PT deque, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(deque);
    xassert(cmp);

    if (!deque || !cmp) {
        return false;
    }

    if (deque->size <= 1) {
        return true;
    }

    return xdeque_heap_sort_impl(deque, 0, deque->size - 1, true, cmp, cl);
}

static
void xdeque_insert_sort_impl(XDeque_PT deque, int lo, int step, int hi, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    for (int i = lo + step; i <= hi; ++i) {
        void *x = xdeque_get_impl(deque, i);

        int j = i - step;
        for (; lo <= j; j -= step) {
            if (cmp(xdeque_get_impl(deque, j), x, cl) <= 0) {
                break;
            }
            xdeque_put_impl(deque, j + step, xdeque_get_impl(deque, j), NULL);
        }

        j += step;
        if (j < i) {
            xdeque_put_impl(deque, j, x, NULL);
        }
    }
}

static
void xdeque_quick_sort_impl(XDeque_PT deque, int lo, int hi, int depth_limit, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    if (hi <= lo + 10) {
        xdeque_insert_sort_impl(deque, lo, 1, hi, cmp, cl);
        return;
    }

    if (depth_limit == 0) {
        xdeque_heap_sort_impl(deque, lo, hi, true, cmp, cl);
        return;
    }

    --depth_limit;

    {
        int lt = lo, i = lo + 1, gt = hi;

        void *val = xdeque_get_impl(deque, lt);
        while (i <= gt) {
            int ret = cmp(xdeque_get_impl(deque, i), val, cl);
            if (ret < 0) {
                xdeque_exch_impl(deque, lt, i, NULL);
                ++lt;
                ++i;
            }
            else if (0 < ret) {
                xdeque_exch_impl(deque, i, gt, NULL);
                --gt;
            }
            else {
                ++i;
            }
        }

        xdeque_quick_sort_impl(deque, lo, lt - 1, depth_limit, cmp, cl);
        xdeque_quick_sort_impl(deque, gt + 1, hi, depth_limit, cmp, cl);
    }
}

bool xdeque_quick_sort(XDeque_PT deque, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(deque);
    xassert(cmp);

    if (!deque || !cmp) {
        return false;
    }

    if (deque->size <= 1) {
        return true;
    }

    xdeque_quick_sort_impl(deque, 0, deque->size - 1, xiarith_lg(deque->size - 1) * 2, cmp, cl);

    xassert(xdeque_is_sorted(deque, cmp, cl));

    return true;
}

static
int xdeque_multiway_sort_apply(void *x, void *y, void *cl) {
    XDeque_Apply_Paras_PT paras = (XDeque_Apply_Paras_PT)cl;
    return (paras->apply)(xdeque_front((XDeque_PT)x), xdeque_front((XDeque_PT)y), paras->cl);
}

static
bool xdeque_multiway_sort_impl(XDeque_PT deque, XPSeq_PT seq, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    /* 3. make the sequence a heap by comparing the first element of each element deque in that sequence */
    xpseq_heapify_min(seq, cmp, cl);

    /* 4. output all the elements by order */
    while (0 < seq->size) {
        /* get the top deque of the heap, and save its first element to output */
        XDeque_PT tdeque = xpseq_front(seq);
        void *elem = xdeque_front(tdeque);
        if (!xdeque_push_back(deque, elem)) {
            return false;
        }
        xdeque_pop_front(tdeque);

        /* delete the top deque of the heap if its size becomes 0 */
        if (tdeque->size == 0) {
            xpseq_pop_front(seq);
        }

        /* heapify the deque again */
        if (1 < seq->size) {
            xpseq_heapify_sink_elem(seq, 0, 0, seq->size - 1, true, cmp, cl);
        }
    }

    return true;
}

bool xdeque_vmultiway_sort(XDeque_PT deque, int(*cmp)(void *x, void *y, void *cl), void *cl, XDeque_PT deque1, ...) {
    xassert(deque);
    xassert(cmp);

    if (!deque || !cmp) {
        return false;
    }

    if (!deque1) {
        return true;
    }

    {
        /* 1. create a new sequece to save all the input deques */
        XPSeq_PT seq = xpseq_new(XUTILS_DEQUE_LAYER1_DEFAULT_LENGTH);
        if (!seq) {
            return false;
        }

        /* 2. save all the input deques into the sequence */
        {
            va_list ap;
            va_start(ap, deque1);
            for (; deque1; deque1 = va_arg(ap, XDeque_PT)) {
                /* ignore the empty sequence */
                if (deque1->size == 0) {
                    continue;
                }

                /* sort this deque at first */
                if (!xdeque_quick_sort(deque1, cmp, cl)) {
                    xpseq_free(&seq);
                    return false;
                }

                if (!xpseq_push_back(seq, (void*)deque1)) {
                    xpseq_free(&seq);
                    return false;
                }
            }
            va_end(ap);

            if (seq->size == 0) {
                xpseq_free(&seq);
                return true;
            }
        }

        {
            XDeque_Apply_Paras_T paras = { cmp, cl };
            bool ret = xdeque_multiway_sort_impl(deque, seq, xdeque_multiway_sort_apply, (void*)&paras);
            if (ret) {
                xassert(xdeque_is_sorted(deque, cmp, cl));
            }

            xpseq_free(&seq);

            return ret;
        }
    }
}

bool xdeque_amultiway_sort(XDeque_PT deque, int(*cmp)(void *x, void *y, void *cl), void *cl, XPArray_PT deques) {
    xassert(deque);
    xassert(cmp);
    xassert(deques);

    if (!deque || !cmp || !deques) {
        return false;
    }

    {
        /* 1. create a new sequece to save all the input deques */
        XPSeq_PT seq = xpseq_new(xparray_size(deques));
        if (!seq) {
            return false;
        }

        /* 2. save all the input sequences into the new sequence */
        {
            int total = xparray_size(deques);
            for (int i = 0; i < total; ++i) {
                XDeque_PT tdeque = (XDeque_PT)xparray_get_impl(deques, i);
                /* ignore the NULL or empty sequence */
                if (!tdeque || (tdeque->size == 0)) {
                    continue;
                }

                if (!xdeque_quick_sort(tdeque, cmp, cl)) {
                    xpseq_free(&seq);
                    return false;
                }

                if (!xpseq_push_back(seq, (void*)tdeque)) {
                    xpseq_free(&seq);
                    return false;
                }
            }

            if (seq->size == 0) {
                xpseq_free(&seq);
                return true;
            }
        }

        {
            XDeque_Apply_Paras_T paras = { cmp, cl };
            bool ret = xdeque_multiway_sort_impl(deque, seq, xdeque_multiway_sort_apply, (void*)&paras);
            if (ret) {
                xassert(xdeque_is_sorted(deque, cmp, cl));
            }

            xpseq_free(&seq);

            return ret;
        }
    }
}

static
bool xdeque_set_strategy_drop_impl(XDeque_PT deque, int strategy) {
    xassert(deque);
    xassert(0 <= strategy);
    xassert(strategy < 3);

    if (!deque || (strategy < 0) || (2 < strategy)) {
        return false;
    }

    deque->discard_strategy = strategy;

    return true;
}

bool xdeque_set_strategy_discard_new(XDeque_PT deque) {
    return xdeque_set_strategy_drop_impl(deque, XUTILS_QUEUE_STRATEGY_DISCARD_NEW);
}

bool xdeque_set_strategy_discard_front(XDeque_PT deque) {
    return xdeque_set_strategy_drop_impl(deque, XUTILS_QUEUE_STRATEGY_DISCARD_FRONT);
}

bool xdeque_set_strategy_discard_back(XDeque_PT deque) {
    return xdeque_set_strategy_drop_impl(deque, XUTILS_QUEUE_STRATEGY_DISCARD_BACK);
}
