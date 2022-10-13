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

#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../list_s_raw/xlist_s_raw_x.h"
#include "../array_pointer/xarray_pointer_x.h"
#include "xlist_s_x.h"

XSList_PT xslist_new(void) {
    XSList_PT slist = XMEM_CALLOC(1, sizeof(*slist));
    if (!slist) {
        return NULL;
    }

    //slist->head = NULL;
    //slist->tail = NULL;
    //slist->size = 0;

    return slist;
}

XSList_PT xslist_new_with_rslist(XRSList_PT list) {
    XSList_PT slist = XMEM_CALLOC(1, sizeof(*slist));
    if (!slist) {
        return NULL;
    }

    slist->head = list;
    {
        slist->size = 0;
        while (list) {
            ++slist->size;
            slist->tail = list;
            list = list->next;
        }
    }

    return slist;
}

int xslist_vload_repeat(XSList_PT slist, void *value, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, value);
    for (; value; value = va_arg(ap, void *)) {
        if (!xslist_push_back_repeat(slist, value)) {
            break;
        }

        ++count;
    }
    va_end(ap);

    return count;
}

int xslist_vload_unique(XSList_PT slist, void *value, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, value);
    for (; value; value = va_arg(ap, void *)) {
        int ret = xslist_push_back_unique(slist, value);
        if (ret < 0) {
            break;
        }
        else if (0 < ret) {
            ++count;
        }
        /* ignore the ret == 0 here */
    }
    va_end(ap);

    return count;
}

int xslist_vload_unique_if(XSList_PT slist, int(*cmp)(void *value1, void *value2, void *cl), void *cl, void *value, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, value);
    for (; value; value = va_arg(ap, void *)) {
        int ret = xslist_push_back_unique_if(slist, value, cmp, cl);
        if (ret < 0) {
            break;
        }
        if (0 < ret) {
            ++count;
        }
        /* ignore the ret == 0 here */
    }
    va_end(ap);

    return count;
}

int xslist_vload_replace_if(XSList_PT slist, int(*cmp)(void *value1, void *value2, void *cl), void *cl, void *value, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, value);
    for (; value; value = va_arg(ap, void *)) {
        int ret = xslist_push_back_replace_if(slist, value, cmp, cl);
        if (ret < 0) {
            break;
        }
        if (0 < ret) {
            ++count;
        }
        /* ignore the ret == 0 here */
    }
    va_end(ap);

    return count;
}

static
int xslist_aload_impl(XSList_PT slist, XPArray_PT xs, bool repeat,
    bool (*push_back_repeat) (XSList_PT slist, void *value),
    int (*push_back)        (XSList_PT slist, void *value),
    int (*push_back_if)     (XSList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl),
    int (*cmp)              (void *value1, void *value2, void *cl), void *cl) {
    xassert(xs);

    if (!xs) {
        return 0;
    }

    {
        int count = 0;

        for (int i = 0; i < xs->size; ++i) {
            void *value = xparray_get_impl(xs, i);
            if (!value) {
                continue;
            }

            if (repeat) {
                if (!push_back_repeat(slist, value)) {
                    break;
                }
                ++count;
            }
            else {
                int ret = cmp ? push_back_if(slist, value, cmp, cl) : push_back(slist, value);
                if (ret < 0) {
                    break;
                }
                if (0 < ret) {
                    ++count;
                }
                /* ignore the ret == 0 here */
            }
        }

        return count;
    }
}

int xslist_aload_repeat(XSList_PT slist, XPArray_PT xs) {
    return xslist_aload_impl(slist, xs, true, xslist_push_back_repeat, NULL, NULL, NULL, NULL);
}

int xslist_aload_unique(XSList_PT slist, XPArray_PT xs) {
    return xslist_aload_impl(slist, xs, false, NULL, xslist_push_back_unique, NULL, NULL, NULL);
}

int xslist_aload_unique_if(XSList_PT slist, XPArray_PT xs, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(cmp);

    if (!cmp) {
        return -1;
    }

    return xslist_aload_impl(slist, xs, false, NULL, NULL, xslist_push_back_unique_if, cmp, cl);
}

int xslist_aload_replace_if(XSList_PT slist, XPArray_PT xs, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(cmp);

    if (!cmp) {
        return -1;
    }

    return xslist_aload_impl(slist, xs, false, NULL, NULL, xslist_push_back_replace_if, cmp, cl);
}

XSList_PT xslist_copyn_if_impl(XSList_PT slist, int value_size, int count, bool deep, int(*cmp)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2) {
    xassert(slist);
    xassert(0 <= count);

    if (!slist || (count < 0)) {
        return NULL;
    }

    if (deep) {
        xassert(0 < value_size);
        if (value_size <= 0) {
            return NULL;
        }
    }

    if (slist->size < count) {
        count = slist->size;
    }

    {
        XSList_PT nslist = xslist_new();
        if (!nslist) {
            return NULL;
        }

        if (count == 0 || slist->size == 0) {
            return nslist;
        }

        {
            XRSList_PT *pp = &nslist->head, step = NULL;

            for (step = slist->head; step && (0 < count); step = step->next) {
                bool ret = apply ? apply(step->value, cl2) : (cmp ? (cmp(step->value, cl2, cl1) == 0) : true);
                /* ignore the item if ret == false */
                if (!ret) {
                    continue;
                }

                XMEM_NEW0(*pp);

                if (!*pp) {
                    deep ? xslist_deep_free(&nslist) : xslist_free(&nslist);
                    return NULL;
                }

                if (deep) {
                    (*pp)->value = XMEM_CALLOC(1, value_size);
                    if (!(*pp)->value) {
                        xslist_deep_free(&nslist);
                        return NULL;
                    }
                    memcpy((*pp)->value, step->value, value_size);
                }
                else {
                    (*pp)->value = step->value;
                }

                nslist->tail = *pp;
                pp = &(*pp)->next;

                ++nslist->size;
                --count;
            }
            *pp = NULL;
        }

        return nslist;
    }
}

XSList_PT xslist_copyn_equal_if(XSList_PT slist, int count, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(value);

    if (!value) {
        return NULL;
    }

    return xslist_copyn_if_impl(slist, 0, count, false, cmp, cl, NULL, value);
}

XSList_PT xslist_copyn_apply_if(XSList_PT slist, int count, bool (*apply)(void *value, void *cl), void *cl) {
    return xslist_copyn_if_impl(slist, 0, count, false, NULL, NULL, apply, cl);
}

XSList_PT xslist_copyn(XSList_PT slist, int count) {
    return xslist_copyn_apply_if(slist, count, NULL, NULL);
}

XSList_PT xslist_copy(XSList_PT slist) {
    return xslist_copyn(slist, (slist ? slist->size : 0));
}

XSList_PT xslist_copy_equal_if(XSList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xslist_copyn_equal_if(slist, (slist ? slist->size : 0), value, cmp, cl);
}

XSList_PT xslist_copy_apply_if(XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    return xslist_copyn_apply_if(slist, (slist ? slist->size : 0), apply, cl);
}

XSList_PT xslist_deep_copyn_equal_if(XSList_PT slist, int value_size, int count, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(value);

    if (!value) {
        return NULL;
    }

    return xslist_copyn_if_impl(slist, value_size, count, true, cmp, cl, NULL, value);
}

XSList_PT xslist_deep_copyn_apply_if(XSList_PT slist, int value_size, int count, bool (*apply)(void *value, void *cl), void *cl) {
    return xslist_copyn_if_impl(slist, value_size, count, true, NULL, NULL, apply, cl);
}

XSList_PT xslist_deep_copyn(XSList_PT slist, int value_size, int count) {
    return xslist_deep_copyn_apply_if(slist, value_size, count, NULL, NULL);
}

XSList_PT xslist_deep_copy(XSList_PT slist, int value_size) {
    return xslist_deep_copyn(slist, value_size, (slist ? slist->size : 0));
}

XSList_PT xslist_deep_copy_equal_if(XSList_PT slist, int value_size, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xslist_deep_copyn_equal_if(slist, value_size, (slist ? slist->size : 0), value, cmp, cl);
}

XSList_PT xslist_deep_copy_apply_if(XSList_PT slist, int value_size, bool (*apply)(void *value, void *cl), void *cl) {
    return xslist_deep_copyn_apply_if(slist, value_size, (slist ? slist->size : 0), apply, cl);
}

bool xslist_push_front_repeat(XSList_PT slist, void *value) {
    xassert(slist);
    xassert(value);

    if (!slist || !value) {
        return false;
    }

    {
        XRSList_PT p = xrslist_new(value);
        if (!p) {
            return false;
        }

        p->next = slist->head;

        slist->head = p;
        if (!slist->tail) {
            slist->tail = p;
        }

        ++slist->size;
    }

    return true;
}

void* xslist_pop_front(XSList_PT slist) {
    xassert(slist);

    if (!slist || (!slist->head)) {
        return NULL;
    }

    {
        XRSList_PT p = slist->head;
        void *ret = p->value;

        slist->head = p->next;
        if (!slist->head) {
            slist->tail = NULL;
        }

        --slist->size;
        XMEM_FREE(p);

        return ret;
    }
}

void* xslist_front(XSList_PT slist) {
    return slist ? (slist->head ? slist->head->value : NULL) : NULL;
}

void* xslist_back(XSList_PT slist) {
    return slist ? (slist->tail ? slist->tail->value : NULL) : NULL;
}

XRSList_PT xslist_front_node(XSList_PT slist) {
    return slist ? slist->head : NULL;
}

XRSList_PT xslist_back_node(XSList_PT slist) {
    return slist ? slist->tail : NULL;
}

bool xslist_push_back_repeat(XSList_PT slist, void *value) {
    xassert(slist);
    xassert(value);

    if (!slist || !value) {
        return false;
    }

    {
        XRSList_PT p = xrslist_new(value);
        if (!p) {
            return false;
        }

        slist->tail ? (slist->tail->next = p) : (slist->head = p);
        slist->tail = p;

        ++slist->size;
    }

    return true;
}

bool xslist_push_back_rslist(XSList_PT slist, XRSList_PT rslist) {
    if (!slist) {
        return false;
    }

    slist->tail ? (slist->tail->next = rslist) : (slist->head = rslist);
    while (rslist) {
        slist->tail = rslist;
        ++slist->size;

        rslist = rslist->next;
    }

    return true;
}

int xslist_push_front_unique(XSList_PT slist, void *value) {
    if (!xslist_find(slist, value)) {
        return xslist_push_front_repeat(slist, value) ? 1 : -1;
    }

    return 0;
}

int xslist_push_front_unique_if(XSList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    if (!xslist_find_equal_if(slist, value, cmp, cl)) {
        return xslist_push_front_repeat(slist, value) ? 1 : -1;
    }

    return 0;
}

static
int xslist_replace_if_impl(XSList_PT slist, void *value, bool deep, bool push_replace(XSList_PT slist, void *value), int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(value);

    if (!value) {
        return -1;
    }

    {
        XRSList_PT node = xslist_find_equal_if(slist, value, cmp, cl);
        if (node) {
            if (deep && (value != node->value)) {
                XMEM_FREE(node->value);
            }
            node->value = value;
            return 0;
        }

        return push_replace(slist, value) ? 1 : -1;
    }
}

int xslist_push_front_replace_if(XSList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xslist_replace_if_impl(slist, value, false, xslist_push_front_repeat, cmp, cl);
}

int xslist_push_front_deep_replace_if(XSList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xslist_replace_if_impl(slist, value, true, xslist_push_front_repeat, cmp, cl);
}

int xslist_push_back_unique(XSList_PT slist, void *value) {
    if (!xslist_find(slist, value)) {
        return xslist_push_back_repeat(slist, value) ? 1 : -1;
    }

    return 0;
}

int xslist_push_back_unique_if(XSList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    if (!xslist_find_equal_if(slist, value, cmp, cl)) {
        return xslist_push_back_repeat(slist, value) ? 1 : -1;
    }

    return 0;
}

int xslist_push_back_replace_if(XSList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xslist_replace_if_impl(slist, value, false, xslist_push_back_repeat, cmp, cl);
}

int xslist_push_back_deep_replace_if(XSList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xslist_replace_if_impl(slist, value, true, xslist_push_back_repeat, cmp, cl);
}

bool xslist_insert_after(XSList_PT slist, XRSList_PT node, void *value) {
    if (!node) {
        return xslist_push_back_repeat(slist, value);
    }

    xassert(slist);
    xassert(value);

    if (!slist || !value) {
        return false;
    }

    {
        XRSList_PT p = xrslist_new(value);
        if (!p) {
            return false;
        }

        p->next = node->next;
        node->next = p;

        if (!p->next) {
            slist->tail = p;
        }

        ++slist->size;
    }

    return true;
}

void* xslist_remove_after(XSList_PT slist, XRSList_PT node) {
    xassert(slist);

    if (!slist || !node || (slist->size == 0)) {
        return NULL;
    }

    if (!node->next) {
        return NULL;
    }

    {
        XRSList_PT p = node->next;
        void* ret = p->value;

        node->next = p->next;
        if (!node->next) {
            slist->tail = node;
        }

        --slist->size;
        XMEM_FREE(p);

        return ret;
    }
}

int xslist_remove_if_impl(XSList_PT slist, bool deep, bool break_first, int(*cmp)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2) {
    xassert(slist);

    if (!slist) {
        return -1;
    }

    {
        XRSList_PT step = NULL, np = NULL, prev = NULL;
        int count = 0;

        for (step = slist->head; step; step = np) {
            np = step->next;

            bool ret = apply ? apply(step->value, cl2) : (cmp ? (cmp(step->value, cl2, cl1) == 0) : (step->value == cl2));
            if (!ret) {
                prev = step;
                continue;
            }

            if (step == slist->head) {
                slist->head = np;
                if (step == slist->tail) {
                    slist->tail = np;
                }
            }
            else {
                prev->next = np;

                if (step == slist->tail) {
                    slist->tail = prev;
                }
            }

            if (deep) {
                XMEM_FREE(step->value);
            }
            XMEM_FREE(step);

            --slist->size;
            ++count;

            if (break_first) {
                break;
            }
        }

        return count;
    }
}

bool xslist_remove(XSList_PT slist, void *value) {
    xassert(value);

    if (!value) {
        return false;
    }

    return (0 <= xslist_remove_if_impl(slist, false, true, NULL, NULL, NULL, value)) ? true : false;
}

int xslist_remove_all(XSList_PT slist, void *value) {
    xassert(value);

    if (!value) {
        return -1;
    }

    return xslist_remove_if_impl(slist, false, false, NULL, NULL, NULL, value);
}

bool xslist_deep_remove(XSList_PT slist, void **value) {
    xassert(value);
    xassert(*value);

    if (!value || !*value) {
        return false;
    }

    {
        int ret = xslist_remove_if_impl(slist, true, true, NULL, NULL, NULL, *value);
        if (0 < ret) {
            *value = NULL;
        }

        return 0 <= ret ? true : false;
    }
}

int xslist_remove_apply_if(XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    return xslist_remove_if_impl(slist, false, false, NULL, NULL, apply, cl);
}

int xslist_remove_equal_if(XSList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xslist_remove_if_impl(slist, false, false, cmp, cl, NULL, value);
}

bool xslist_remove_apply_break_if(XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 <= xslist_remove_if_impl(slist, false, true, NULL, NULL, apply, cl));
}

bool xslist_remove_equal_break_if(XSList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return (0 <= xslist_remove_if_impl(slist, false, true, cmp, cl, NULL, value));
}

int xslist_deep_remove_apply_if(XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    return xslist_remove_if_impl(slist, true, false, NULL, NULL, apply, cl);
}

int xslist_deep_remove_equal_if(XSList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xslist_remove_if_impl(slist, true, false, cmp, cl, NULL, value);
}

bool xslist_deep_remove_apply_break_if(XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 <= xslist_remove_if_impl(slist, true, true, NULL, NULL, apply, cl));
}

bool xslist_deep_remove_equal_break_if(XSList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return (0 <= xslist_remove_if_impl(slist, true, true, cmp, cl, NULL, value));
}

static
void xslist_free_impl(XSList_PT slist, bool deep, bool (*apply)(void *value, void *cl), void *cl) {
    if (!slist) {
        return;
    }

    {
        XRSList_PT step = NULL, np = NULL;

        for (step = slist->head; step; step = np) {
            np = step->next;

            if (deep) {
                XMEM_FREE(step->value);
            }
            else if (apply) {
                apply(step->value, cl);
            }
            XMEM_FREE(step);
        }
    }
}

void xslist_free(XSList_PT *pslist) {
    if (!pslist) {
        return;
    }

    xslist_free_impl(*pslist, false, NULL, NULL);
    XMEM_FREE(*pslist);
}

void xslist_free_apply(XSList_PT *pslist, bool (*apply)(void *value, void *cl), void *cl) {
    if (!pslist) {
        return;
    }

    xslist_free_impl(*pslist, false, apply, cl);
    XMEM_FREE(*pslist);
}

void xslist_deep_free(XSList_PT *pslist) {
    if (!pslist) {
        return;
    }

    xslist_free_impl(*pslist, true, NULL, NULL);
    XMEM_FREE(*pslist);
}

static
void xslist_clear_impl(XSList_PT slist, bool deep, bool (*apply)(void *value, void *cl), void *cl) {
    if (!slist) {
        return;
    }

    xslist_free_impl(slist, deep, apply, cl);
    slist->head = NULL;
    slist->tail = NULL;
    slist->size = 0;
}

void xslist_clear(XSList_PT slist) {
    xslist_clear_impl(slist, false, NULL, NULL);
}

void xslist_clear_apply(XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    xslist_clear_impl(slist, false, apply, cl);
}

void xslist_deep_clear(XSList_PT slist) {
    xslist_clear_impl(slist, true, NULL, NULL);
}

bool xslist_swap(XSList_PT slist1, XSList_PT slist2) {
    xassert(slist1);
    xassert(slist2);

    if (!slist1 || !slist2) {
        return false;
    }

    {
        XRSList_PT head = slist1->head;
        XRSList_PT tail = slist1->tail;
        int size = slist1->size;

        slist1->head = slist2->head;
        slist1->tail = slist2->tail;
        slist1->size = slist2->size;

        slist2->head = head;
        slist2->tail = tail;
        slist2->size = size;
    }

    return true;
}

static
void xslist_merge_impl(XSList_PT slist1, XSList_PT slist2) {
    if (slist2->head) {
        slist1->head ? (slist1->tail->next = slist2->head) : (slist1->head = slist2->head);
        slist1->tail = slist2->tail;
        slist1->size += slist2->size;
    }
}

int xslist_merge(XSList_PT slist1, XSList_PT *pslist2) {
    xassert(slist1);
    xassert(pslist2);

    if (!slist1 || !pslist2) {
        return -1;
    }

    if (!*pslist2) {
        return 0;
    }

    {
        int count = (*pslist2)->size;

        xslist_merge_impl(slist1, *pslist2);
        XMEM_FREE(*pslist2);

        return count;
    }
}

void xslist_reverse(XSList_PT slist) {
    xassert(slist);

    if (!slist || (slist->size <= 1)) {
        return;
    }

    {
        XRSList_PT p = NULL, np = NULL, prev = NULL;

        slist->tail = slist->head;
        for (p = slist->head; p; p = np) {
            np = p->next;
            p->next = prev;
            prev = p;
        }
        slist->head = prev;
    }

    return;
}

static
int xslist_map_impl(XSList_PT slist, bool break_first, bool break_true, XRSList_PT *node, int(*equal)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2) {
    xassert(slist);

    if (!slist) {
        return -1;
    }

    {
        int count = 0;

        for (XRSList_PT step = slist->head; step; step = step->next) {
            bool ret = apply ? apply(step->value, cl2) : (equal ? (0 == equal(step->value, cl2, cl1)) : (step->value == cl2));

            if (!break_first) {
                if (ret) {
                    ++count;
                }
            }
            else {
                if ((ret && break_true) || (!ret && !break_true)) {
                    if (node) {
                        *node = step;
                    }

                    ++count;
                    break;
                }
            }
        }

        return count;
    }
}

int xslist_map(XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    return xslist_map_impl(slist, false, false, NULL, NULL, NULL, apply, cl);
}

bool xslist_map_break_if_true(XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 < xslist_map_impl(slist, true, true, NULL, NULL, NULL, apply, cl));
}

bool xslist_map_break_if_false(XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 < xslist_map_impl(slist, true, false, NULL, NULL, NULL, apply, cl));
}

XRSList_PT xslist_find(XSList_PT slist, void *value) {
    XRSList_PT node = NULL;
    xslist_map_impl(slist, true, true, &node, NULL, NULL, NULL, value);
    return node;
}

XRSList_PT xslist_find_equal_if(XSList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    XRSList_PT node = NULL;
    xslist_map_impl(slist, true, true, &node, cmp, cl, NULL, value);
    return node;
}

XRSList_PT xslist_find_apply_if(XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    XRSList_PT node = NULL;
    xslist_map_impl(slist, true, true, &node, NULL, NULL, apply, cl);
    return node;
}

XPArray_PT xslist_to_array(XSList_PT slist) {
    xassert(slist);

    if (!slist) {
        return NULL;
    }

    {
        XPArray_PT array = xparray_new(slist->size);
        if (!array) {
            return NULL;
        }

        {
            XRSList_PT step = slist->head;
            for (int i = 0; step; ++i, step = step->next) {
                xparray_put_impl(array, i, step->value);
            }
        }

        return array;
    }
}

int xslist_size(XSList_PT slist) {
    return (slist ? slist->size : 0);
}

bool xslist_is_empty(XSList_PT slist) {
    return (slist ? (slist->size == 0) : true);
}

static
bool xslist_unique_impl(XSList_PT slist, bool deep, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    if (xslist_sort(slist, cmp, cl)) {
        for (XRSList_PT step = slist->head; step && step->next; /*nothing*/) {
            if (0 == cmp(step->value, step->next->value, cl)) {
                void *value = xslist_remove_after(slist, step);
                if (!value) {
                    return false;
                }

                if (deep) {
                    XMEM_FREE(value);
                }
            }
            else {
                step = step->next;
            }
        }

        return true;
    }

    return false;
}

bool xslist_unique(XSList_PT slist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xslist_unique_impl(slist, false, cmp, cl);
}

bool xslist_deep_unique(XSList_PT slist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xslist_unique_impl(slist, true, cmp, cl);
}

bool xslist_sort(XSList_PT slist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(slist);
    xassert(cmp);

    if (!slist || !cmp) {
        return false;
    }

    if (slist->size <= 1) {
        return true;
    }

    slist->head = xrslist_merge_sort_impl(slist->head, cmp, cl);

    /* get the right pointer of slist->tail */
    XRSList_PT step = slist->head;
    while(step && step->next) {
        step = step->next;
    }
    slist->tail = step;

    xassert(xslist_is_sorted(slist, cmp, cl));

    return true;
}

bool xslist_is_sorted(XSList_PT slist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(slist);
    xassert(cmp);

    if (!slist || !cmp) {
        return false;
    }

    {
        for (XRSList_PT step = slist->head; step && step->next; /*nothing*/) {
            if (0 < cmp(step->value, step->next->value, cl)) {
                return false;
            }
            else {
                step = step->next;
            }
        }

        return true;
    }
}
