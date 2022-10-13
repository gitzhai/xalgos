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
*       <<Algorithms in C>> Third Edition : chapter 9.7
*/

#include <stddef.h>
#include <stdarg.h>

#include "../include/xassert.h"
#include "../include/xarray_pointer.h"
#include "../array_pointer/xarray_pointer_x.h"
#include "../queue_binomial_min/xqueue_binomial_min_x.h"
#include "../include/xqueue_binomial_max.h"

XMaxBinQue_PT xmaxbinque_new(int capacity, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    return xminbinque_new(capacity, cmp, cl);
}

XMaxBinQue_PT xmaxbinque_copy(XMaxBinQue_PT queue) {
    return xminbinque_copy(queue);
}

XMaxBinQue_PT xmaxbinque_deep_copy(XMaxBinQue_PT queue, int elem_size) {
    return xminbinque_deep_copy(queue, elem_size);
}

int xmaxbinque_vload(XMaxBinQue_PT queue, void *x, ...) {
    xassert(queue);

    if (!queue) {
        return 0;
    }

    {
        int count = queue->size;

        va_list ap;
        va_start(ap, x);
        for (; x; x = va_arg(ap, void *)) {
            if (!xmaxbinque_push(queue, x)) {
                break;
            }
        }
        va_end(ap);

        return queue->size - count;
    }
}

int xmaxbinque_aload(XMaxBinQue_PT queue, XPArray_PT xs) {
    xassert(queue);
    xassert(xs);

    if (!queue || !xs) {
        return 0;
    }

    {
        int count = queue->size;

        for (int i = 0; i < xs->size; i++) {
            /* ignore the NULL element */
            void *value = xparray_get_impl(xs, i);
            if (!value) {
                continue;
            }

            if (!xminbinque_push_impl(queue, value, true)) {
                break;
            }
        }

        return queue->size - count;
    }
}

bool xmaxbinque_push(XMaxBinQue_PT queue, void *x) {
    xassert(queue);
    xassert(x);

    if (!queue || !x) {
        return false;
    }

    return xminbinque_push_impl(queue, x, true);
}

void* xmaxbinque_pop(XMaxBinQue_PT queue) {
    xassert(queue);

    if (!queue || (queue->size == 0)) {
        return NULL;
    }

    return xminbinque_pop_impl(queue, true);
}

void* xmaxbinque_peek(XMaxBinQue_PT queue) {
    xassert(queue);

    if (!queue || (queue->size == 0)) {
        return NULL;
    }

    return xminbinque_peek_impl(queue, true);
}

bool xmaxbinque_merge(XMaxBinQue_PT queue1, XMaxBinQue_PT *pqueue2) {
    xassert(queue1);
    xassert(pqueue2);
    xassert(*pqueue2);

    if (!queue1 || !pqueue2 || !*pqueue2) {
        return false;
    }

    return xminbinque_merge_impl(queue1, pqueue2, true, true);
}

int xmaxbinque_map(XMaxBinQue_PT queue, bool (*apply)(void *x, void *cl), void *cl) {
    return xminbinque_map(queue, apply, cl);
}

bool xmaxbinque_map_break_if_true(XMaxBinQue_PT queue, bool (*apply)(void *x, void *cl), void *cl) {
    return xminbinque_map_break_if_true(queue, apply, cl);
}

bool xmaxbinque_map_break_if_false(XMaxBinQue_PT queue, bool (*apply)(void *x, void *cl), void *cl) {
    return xminbinque_map_break_if_false(queue, apply, cl);
}

void xmaxbinque_free(XMaxBinQue_PT *pqueue) {
    xminbinque_free(pqueue);
}

void xmaxbinque_deep_free(XMaxBinQue_PT *pqueue) {
    xminbinque_deep_free(pqueue);
}

void xmaxbinque_free_apply(XMaxBinQue_PT *pqueue, bool (*apply)(void **x, void *cl), void *cl) {
    xminbinque_free_apply(pqueue, apply, cl);
}

void xmaxbinque_clear(XMaxBinQue_PT queue) {
    xminbinque_clear(queue);
}

void xmaxbinque_clear_apply(XMaxBinQue_PT queue, bool (*apply)(void **x, void *cl), void *cl) {
    xminbinque_clear_apply(queue, apply, cl);
}

void xmaxbinque_deep_clear(XMaxBinQue_PT queue) {
    xminbinque_deep_clear(queue);
}

int xmaxbinque_size(XMaxBinQue_PT queue) {
    return xminbinque_size(queue);
}

bool xmaxbinque_is_empty(XMaxBinQue_PT queue) {
    return xminbinque_is_empty(queue);
}

bool xmaxbinque_swap(XMaxBinQue_PT queue1, XMaxBinQue_PT queue2) {
    return xminbinque_swap(queue1, queue2);
}

/* Note : make sure the capacity of queue is "M" which is the wanted limitation at first */
bool xmaxbinque_keep_min_values(XMaxBinQue_PT queue, void *data, void **odata) {
    xassert(queue);
    xassert(data);
    xassert(queue->capacity != 0);

    if (!queue || !data || (queue->capacity == 0)) {
        return false;
    }

    /* no capacity*/
    if (queue->capacity <= queue->size) {
        /* delete the top minimum elements to save the bigger one */
        if (queue->cmp(xmaxbinque_peek(queue), data, queue->cl) < 0) {
            void *tmp = xminbinque_pop_impl(queue, true);
            if (!tmp) {
                return false;
            }

            if (odata) {
                *odata = tmp;
            }
        }
        else {
            /* ignore the smaller input */
            return true;
        }
    }

    return xminbinque_push_impl(queue, data, true);
}

bool xmaxbinque_set_strategy_discard_new(XMaxBinQue_PT queue) {
    return xminbinque_set_strategy_discard_new(queue);
}

bool xmaxbinque_set_strategy_discard_top(XMaxBinQue_PT queue) {
    return xminbinque_set_strategy_discard_top(queue);
}