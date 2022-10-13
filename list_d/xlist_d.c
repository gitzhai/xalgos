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
#include "../list_d_raw/xlist_d_raw_x.h"
#include "../array_pointer/xarray_pointer_x.h"
#include "xlist_d_x.h"

XDList_PT xdlist_new(void) {
    XDList_PT dlist = XMEM_CALLOC(1, sizeof(*dlist));
    if (!dlist) {
        return NULL;
    }

    //dlist->size = 0;
    //dlist->head = NULL;
    //dlist->tail = NULL;

    return dlist;
}

int xdlist_vload_repeat(XDList_PT dlist, void *value, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, value);
    for (; value; value = va_arg(ap, void *)) {
        if (!xdlist_push_back_repeat(dlist, value)) {
            break;
        }

        ++count;
    }
    va_end(ap);

    return count;
}

int xdlist_vload_unique(XDList_PT dlist, void *value, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, value);
    for (; value; value = va_arg(ap, void *)) {
        int ret = xdlist_push_back_unique(dlist, value);
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

int xdlist_vload_unique_if(XDList_PT dlist, int(*cmp)(void *value1, void *value2, void *cl), void *cl, void *value, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, value);
    for (; value; value = va_arg(ap, void *)) {
        int ret = xdlist_push_back_unique_if(dlist, value, cmp, cl);
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

int xdlist_vload_replace_if(XDList_PT dlist, int(*cmp)(void *value1, void *value2, void *cl), void *cl, void *value, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, value);
    for (; value; value = va_arg(ap, void *)) {
        int ret = xdlist_push_back_replace_if(dlist, value, cmp, cl);
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
int xdlist_aload_impl(XDList_PT dlist, XPArray_PT xs, bool repeat,
                        bool  (*push_back_repeat) (XDList_PT dlist, void *value),
                        int   (*push_back)        (XDList_PT dlist, void *value),
                        int   (*push_back_if)     (XDList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl),
                        int   (*cmp)              (void *value1, void *value2, void *cl), void *cl) {
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
                if (!push_back_repeat(dlist, value)) {
                    break;
                }
                ++count;
            }
            else {
                int ret = cmp ? push_back_if(dlist, value, cmp, cl) : push_back(dlist, value);
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

int xdlist_aload_repeat(XDList_PT dlist, XPArray_PT xs) {
    return xdlist_aload_impl(dlist, xs, true, xdlist_push_back_repeat, NULL, NULL, NULL, NULL);
}

int xdlist_aload_unique(XDList_PT dlist, XPArray_PT xs) {
    return xdlist_aload_impl(dlist, xs, false, NULL, xdlist_push_back_unique, NULL, NULL, NULL);
}

int xdlist_aload_unique_if(XDList_PT dlist, XPArray_PT xs, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(cmp);

    if (!cmp) {
        return -1;
    }

    return xdlist_aload_impl(dlist, xs, false, NULL, NULL, xdlist_push_back_unique_if, cmp, cl);
}

int xdlist_aload_replace_if(XDList_PT dlist, XPArray_PT xs, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(cmp);

    if (!cmp) {
        return -1;
    }

    return xdlist_aload_impl(dlist, xs, false, NULL, NULL, xdlist_push_back_replace_if, cmp, cl);
}

XDList_PT xdlist_copyn_if_impl(XDList_PT dlist, int value_size, int count, bool deep, int(*cmp)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2) {
    xassert(dlist);
    xassert(0 <= count);

    if (!dlist || (count < 0)) {
        return NULL;
    }

    if (deep) {
        xassert(0 < value_size);
        if (value_size <= 0) {
            return NULL;
        }
    }

    if (dlist->size < count) {
        count = dlist->size;
    }

    {
        XDList_PT ndlist = xdlist_new();
        if (!ndlist) {
            return NULL;
        }

        if (count == 0 || dlist->size == 0) {
            return ndlist;
        }

        {
            XRDList_PT *pp = &ndlist->head, prev = NULL, step = NULL;

            for (step = dlist->head; step && (0 < count); step = step->next) {
                bool ret = apply ? apply(step->value, cl2) : (cmp ? (cmp(step->value, cl2, cl1) == 0) : true);
                /* ignore the item if ret == false */
                if (!ret) {
                    continue;
                }

                XMEM_NEW0(*pp);

                if (!*pp) {
                    deep ? xdlist_deep_free(&ndlist) : xdlist_free(&ndlist);
                    return NULL;
                }

                if (deep) {
                    (*pp)->value = XMEM_CALLOC(1, value_size);
                    if (!(*pp)->value) {
                        xdlist_deep_free(&ndlist);
                        return NULL;
                    }
                    memcpy((*pp)->value, step->value, value_size);
                }
                else {
                    (*pp)->value = step->value;
                }

                (*pp)->prev = prev;
                prev = *pp;

                ndlist->tail = *pp;
                pp = &(*pp)->next;

                ++ndlist->size;
                --count;
            }
            *pp = NULL;
        }

        return ndlist;
    }
}

XDList_PT xdlist_copyn_equal_if(XDList_PT dlist, int count, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(value);

    if (!value) {
        return NULL;
    }

    return xdlist_copyn_if_impl(dlist, 0, count, false, cmp, cl, NULL, value);
}

XDList_PT xdlist_copyn_apply_if(XDList_PT dlist, int count, bool (*apply)(void *value, void *cl), void *cl) {
    return xdlist_copyn_if_impl(dlist, 0, count, false, NULL, NULL, apply, cl);
}

XDList_PT xdlist_copyn(XDList_PT dlist, int count) {
    return xdlist_copyn_apply_if(dlist, count, NULL, NULL);
}

XDList_PT xdlist_copy(XDList_PT dlist) {
    return xdlist_copyn(dlist, (dlist ? dlist->size : 0));
}

XDList_PT xdlist_copy_equal_if(XDList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xdlist_copyn_equal_if(dlist, (dlist ? dlist->size : 0), value, cmp, cl);
}

XDList_PT xdlist_copy_apply_if(XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    return xdlist_copyn_apply_if(dlist, (dlist ? dlist->size : 0), apply, cl);
}

XDList_PT xdlist_deep_copyn_equal_if(XDList_PT dlist, int value_size, int count, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(value);

    if (!value) {
        return NULL;
    }

    return xdlist_copyn_if_impl(dlist, value_size, count, true, cmp, cl, NULL, value);
}

XDList_PT xdlist_deep_copyn_apply_if(XDList_PT dlist, int value_size, int count, bool (*apply)(void *value, void *cl), void *cl) {
    return xdlist_copyn_if_impl(dlist, value_size, count, true, NULL, NULL, apply, cl);
}

XDList_PT xdlist_deep_copyn(XDList_PT dlist, int value_size, int count) {
    return xdlist_deep_copyn_apply_if(dlist, value_size, count, NULL, NULL);
}

XDList_PT xdlist_deep_copy(XDList_PT dlist, int value_size) {
    return xdlist_deep_copyn(dlist, value_size, (dlist ? dlist->size : 0));
}

XDList_PT xdlist_deep_copy_equal_if(XDList_PT dlist, int value_size, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xdlist_deep_copyn_equal_if(dlist, value_size, (dlist ? dlist->size : 0), value, cmp, cl);
}

XDList_PT xdlist_deep_copy_apply_if(XDList_PT dlist, int value_size, bool (*apply)(void *value, void *cl), void *cl) {
    return xdlist_deep_copyn_apply_if(dlist, value_size, (dlist ? dlist->size : 0), apply, cl);
}

bool xdlist_push_front_repeat(XDList_PT dlist, void *value) {
    xassert(dlist);
    xassert(value);

    if (!dlist || !value) {
        return false;
    }

    {
        XRDList_PT p = xrdlist_new(value);
        if (!p) {
            return false;
        }

        p->next = dlist->head;
        dlist->tail ? (dlist->head->prev = p) : (dlist->tail = p);
        dlist->head = p;

        ++dlist->size;
    }

    return true;
}

void* xdlist_pop_front(XDList_PT dlist) {
    xassert(dlist);

    if (!dlist || (dlist->size == 0)) {
        return NULL;
    }

    {
        XRDList_PT p = dlist->head;
        void* ret = p->value;

        dlist->head = p->next;
        dlist->head ? (dlist->head->prev = NULL) : (dlist->tail = NULL);

        XMEM_FREE(p);
        --dlist->size;

        return ret;
    }
}

void* xdlist_front(XDList_PT dlist) {
    return dlist ? (dlist->head ? dlist->head->value : NULL) : NULL;
}

void* xdlist_back(XDList_PT dlist) {
    return dlist ? (dlist->tail ? dlist->tail->value : NULL) : NULL;
}

XRDList_PT xdlist_front_node(XDList_PT dlist) {
    return dlist ? dlist->head : NULL;
}

XRDList_PT xdlist_back_node(XDList_PT dlist) {
    return dlist ? dlist->tail : NULL;
}

bool xdlist_push_back_repeat(XDList_PT dlist, void *value) {
    xassert(dlist);
    xassert(value);

    if (!dlist || !value) {
        return false;
    }

    {
        XRDList_PT p = xrdlist_new(value);
        if (!p) {
            return false;
        }

        p->prev = dlist->tail;
        dlist->head ? (dlist->tail->next = p) : (dlist->head = p);
        dlist->tail = p;

        ++dlist->size;
    }

    return true;
}

void* xdlist_pop_back(XDList_PT dlist) {
    xassert(dlist);

    if (!dlist || (dlist->size == 0)) {
        return NULL;
    }

    {
        XRDList_PT p = dlist->tail;
        void* ret = p->value;

        dlist->tail = p->prev;
        dlist->tail ? (dlist->tail->next = NULL) : (dlist->head = NULL);

        XMEM_FREE(p);
        --dlist->size;

        return ret;
    }
}

int xdlist_push_front_unique(XDList_PT dlist, void *value) {
    if (!xdlist_find(dlist, value)) {
        return xdlist_push_front_repeat(dlist, value) ? 1 : -1;
    }

    return 0;
}

int xdlist_push_front_unique_if(XDList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    if (!xdlist_find_equal_if(dlist, value, cmp, cl)) {
        return xdlist_push_front_repeat(dlist, value) ? 1 : -1;
    }

    return 0;
}

static
int xdlist_replace_if_impl(XDList_PT dlist, void *value, bool deep, bool push_replace(XDList_PT dlist, void *value), int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(value);

    if (!value) {
        return -1;
    }

    {
        XRDList_PT node = xdlist_find_equal_if(dlist, value, cmp, cl);
        if (node) {
            if (deep && (value != node->value)) {
                XMEM_FREE(node->value);
            }

            node->value = value;
            return 0;
        }

        return push_replace(dlist, value) ? 1 : -1;
    }
}

int xdlist_push_front_replace_if(XDList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xdlist_replace_if_impl(dlist, value, false, xdlist_push_front_repeat, cmp, cl);
}

int xdlist_push_front_deep_replace_if(XDList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xdlist_replace_if_impl(dlist, value, true, xdlist_push_front_repeat, cmp, cl);
}

int xdlist_push_back_unique(XDList_PT dlist, void *value) {
    if (!xdlist_find(dlist, value)) {
        return xdlist_push_back_repeat(dlist, value) ? 1 : -1;
    }

    return 0;
}

int xdlist_push_back_unique_if(XDList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    if (!xdlist_find_equal_if(dlist, value, cmp, cl)) {
        return xdlist_push_back_repeat(dlist, value) ? 1 : -1;
    }

    return 0;
}

int xdlist_push_back_replace_if(XDList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xdlist_replace_if_impl(dlist, value, false, xdlist_push_back_repeat, cmp, cl);
}

int xdlist_push_back_deep_replace_if(XDList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xdlist_replace_if_impl(dlist, value, true, xdlist_push_back_repeat, cmp, cl);
}

bool xdlist_insert_before(XDList_PT dlist, XRDList_PT node, void *value) {
    if (!node) {
        return xdlist_push_front_repeat(dlist, value);
    }

    xassert(dlist);
    xassert(value);

    if (!dlist || !value) {
        return false;
    }

    {
        XRDList_PT p = xrdlist_new(value);
        if (!p) {
            return false;
        }

        p->prev = node->prev;
        p->next = node;

        node->prev ? (node->prev->next = p) : (dlist->head = p);
        node->prev = p;

        ++dlist->size;
    }

    return true;
}

bool xdlist_insert_after(XDList_PT dlist, XRDList_PT node, void *value) {
    if (!node) {
        return xdlist_push_back_repeat(dlist, value);
    }

    xassert(dlist);
    xassert(value);

    if (!dlist || !value) {
        return false;
    }

    {
        XRDList_PT p = xrdlist_new(value);
        if (!p) {
            return false;
        }

        p->prev = node;
        p->next = node->next;

        node->next ? (node->next->prev = p) : (dlist->tail = p);
        node->next = p;

        ++dlist->size;
    }

    return true;
}

bool xdlist_remove_before(XDList_PT dlist, XRDList_PT node, void **value) {
    xassert(dlist);

    if (!dlist || !node || (dlist->size == 0)) {
        return false;
    }

    if (!node->prev) {
        return false;
    }

    {
        XRDList_PT p = node->prev;
        if (value) {
            *value = p->value;
        }
        
        node->prev = p->prev;
        p->prev ? (p->prev->next = node) : (dlist->head = node);

        XMEM_FREE(p);
        --dlist->size;
    }

    return true;
}

bool xdlist_remove_after(XDList_PT dlist, XRDList_PT node, void **value) {
    xassert(dlist);

    if (!dlist || !node || (dlist->size == 0)) {
        return false;
    }

    if (!node->next) {
        return false;
    }

    {
        XRDList_PT p = node->next;
        if (value) {
            *value = p->value;
        }

        node->next = p->next;
        node->next ? (node->next->prev = node) : (dlist->tail = node);

        XMEM_FREE(p);
        --dlist->size;
    }

    return true;
}

int xdlist_remove_if_impl(XDList_PT dlist, bool deep, bool break_first, int(*cmp)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2) {
    xassert(dlist);

    if (!dlist) {
        return -1;
    }

    {
        XRDList_PT step = NULL, np = NULL, prev = NULL;
        int count = 0;

        for (step = dlist->head; step; step = np) {
            np = step->next;

            bool ret = apply ? apply(step->value, cl2) : (cmp ? (cmp(step->value, cl2, cl1) == 0) : (step->value == cl2));
            if (!ret) {
                prev = step;
                continue;
            }

            if (step == dlist->head) {
                dlist->head = np;
                if (step == dlist->tail) {
                    dlist->tail = np;
                }
            }
            else {
                prev->next = np;
                if (np) {
                    np->prev = prev;
                }

                if (step == dlist->tail) {
                    dlist->tail = prev;
                }
            }

            if (deep) {
                XMEM_FREE(step->value);
            }
            XMEM_FREE(step);

            --dlist->size;
            ++count;

            if (break_first) {
                break;
            }
        }

        return count;
    }
}

bool xdlist_remove(XDList_PT dlist, void *value) {
    xassert(value);

    if (!value) {
        return false;
    }

    return (0 <= xdlist_remove_if_impl(dlist, false, true, NULL, NULL, NULL, value)) ? true : false;
}

int xdlist_remove_all(XDList_PT dlist, void *value) {
    xassert(value);

    if (!value) {
        return -1;
    }

    return xdlist_remove_if_impl(dlist, false, false, NULL, NULL, NULL, value);
}

bool xdlist_deep_remove(XDList_PT dlist, void **value) {
    xassert(value);
    xassert(*value);

    if (!value || !*value) {
        return false;
    }

    if (0 < xdlist_remove_if_impl(dlist, true, true, NULL, NULL, NULL, *value)) {
        *value = NULL;
        return true;
    }

    return false;
}

int xdlist_remove_apply_if(XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    return xdlist_remove_if_impl(dlist, false, false, NULL, NULL, apply, cl);
}

int xdlist_remove_equal_if(XDList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xdlist_remove_if_impl(dlist, false, false, cmp, cl, NULL, value);
}

bool xdlist_remove_apply_break_if(XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 <= xdlist_remove_if_impl(dlist, false, true, NULL, NULL, apply, cl));
}

bool xdlist_remove_equal_break_if(XDList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return (0 <= xdlist_remove_if_impl(dlist, false, true, cmp, cl, NULL, value));
}

int xdlist_deep_remove_apply_if(XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    return xdlist_remove_if_impl(dlist, true, false, NULL, NULL, apply, cl);
}

int xdlist_deep_remove_equal_if(XDList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xdlist_remove_if_impl(dlist, true, false, cmp, cl, NULL, value);
}

bool xdlist_deep_remove_apply_break_if(XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 <= xdlist_remove_if_impl(dlist, true, true, NULL, NULL, apply, cl));
}

bool xdlist_deep_remove_equal_break_if(XDList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return (0 <= xdlist_remove_if_impl(dlist, true, true, cmp, cl, NULL, value));
}

static
void xdlist_free_impl(XDList_PT dlist, bool deep, bool (*apply)(void *value, void *cl), void *cl) {
    if (!dlist) {
        return;
    }

    {
        XRDList_PT step = NULL, np = NULL;

        for (step = dlist->head; step; step = np) {
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

    return;
}

void xdlist_free(XDList_PT *pdlist) {
    if (!pdlist) {
        return;
    }

    xdlist_free_impl(*pdlist, false, NULL, NULL);
    XMEM_FREE(*pdlist);
}

void xdlist_free_apply(XDList_PT *pdlist, bool (*apply)(void *value, void *cl), void *cl) {
    if (!pdlist) {
        return;
    }

    xdlist_free_impl(*pdlist, false, apply, cl);
    XMEM_FREE(*pdlist);
}

void xdlist_deep_free(XDList_PT *pdlist) {
    if (!pdlist) {
        return;
    }

    xdlist_free_impl(*pdlist, true, NULL, NULL);
    XMEM_FREE(*pdlist);
}

static
void xdlist_clear_impl(XDList_PT dlist, bool deep, bool (*apply)(void *value, void *cl), void *cl) {
    if (!dlist) {
        return;
    }

    xdlist_free_impl(dlist, deep, apply, cl);
    dlist->head = NULL;
    dlist->tail = NULL;
    dlist->size = 0;
}

void xdlist_clear(XDList_PT dlist) {
    xdlist_clear_impl(dlist, false, NULL, NULL);
}

void xdlist_clear_apply(XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    xdlist_clear_impl(dlist, false, apply, cl);
}

void xdlist_deep_clear(XDList_PT dlist) {
    xdlist_clear_impl(dlist, true, NULL, NULL);
}

bool xdlist_swap(XDList_PT dlist1, XDList_PT dlist2) {
    xassert(dlist1);
    xassert(dlist2);

    if (!dlist1 || !dlist2) {
        return false;
    }

    {
        XRDList_PT head = dlist1->head;
        XRDList_PT tail = dlist1->tail;
        int size = dlist1->size;

        dlist1->head = dlist2->head;
        dlist1->tail = dlist2->tail;
        dlist1->size = dlist2->size;

        dlist2->head = head;
        dlist2->tail = tail;
        dlist2->size = size;
    }

    return true;
}

static 
void xdlist_merge_impl(XDList_PT dlist1, XDList_PT dlist2) {
    if (dlist2->head) {
        if (!dlist1->head) {
            dlist1->head = dlist2->head;
        }
        else {
            dlist1->tail->next = dlist2->head;
            dlist2->head->prev = dlist1->tail;
        }
        dlist1->tail = dlist2->tail;

        dlist1->size += dlist2->size;
    }
}

int xdlist_merge(XDList_PT dlist1, XDList_PT *pdlist2) {
    xassert(dlist1);
    xassert(pdlist2);

    if (!dlist1 || !pdlist2) {
        return -1;
    }

    if (!*pdlist2) {
        return 0;
    }

    {
        int count = (*pdlist2)->size;

        xdlist_merge_impl(dlist1, (*pdlist2));
        XMEM_FREE(*pdlist2);

        return count;
    }
}

void xdlist_reverse(XDList_PT dlist) {
    xassert(dlist);

    if (!dlist || (dlist->size <= 1)) {
        return;
    }

    {
        XRDList_PT p = NULL, np = NULL, prev = NULL;

        dlist->tail = dlist->head;
        for (p = dlist->head; p; p = np) {
            np = p->next;
            p->next = prev;
            p->prev = np;
            prev = p;
        }
        dlist->head = prev;
    }

    return;
}

static
int xdlist_map_impl(XDList_PT dlist, bool break_first, bool break_true, XRDList_PT *node, int(*equal)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2) {
    xassert(dlist);
    if (!dlist) {
        return 0;
    }

    {
        int count = 0;

        for (XRDList_PT step = dlist->head; step; step = step->next) {
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

int xdlist_map(XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    return xdlist_map_impl(dlist, false, false, NULL, NULL, NULL, apply, cl);
}

bool xdlist_map_break_if_true(XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 < xdlist_map_impl(dlist, true, true, NULL, NULL, NULL, apply, cl));
}

bool xdlist_map_break_if_false(XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 < xdlist_map_impl(dlist, true, false, NULL, NULL, NULL, apply, cl));
}

XRDList_PT xdlist_find(XDList_PT dlist, void *value) {
    XRDList_PT node = NULL;
    xdlist_map_impl(dlist, true, true, &node, NULL, NULL, NULL, value);
    return node;
}

XRDList_PT xdlist_find_equal_if(XDList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    XRDList_PT node = NULL;
    xdlist_map_impl(dlist, true, true, &node, cmp, cl, NULL, value);
    return node;
}

XRDList_PT xdlist_find_apply_if(XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    XRDList_PT node = NULL;
    xdlist_map_impl(dlist, true, true, &node, NULL, NULL, apply, cl);
    return node;
}

XPArray_PT xdlist_to_array(XDList_PT dlist) {
    xassert(dlist);

    if (!dlist) {
        return NULL;
    }

    {
        XPArray_PT array = xparray_new(dlist->size);
        if (!array) {
            return NULL;
        }

        {
            XRDList_PT step = dlist->head;
            for (int i = 0; step; ++i, step = step->next) {
                xparray_put_impl(array, i, step->value);
            }
        }

        return array;
    }
}

int xdlist_size(XDList_PT dlist) {
    return (dlist ? dlist->size : 0);
}

bool xdlist_is_empty(XDList_PT dlist) {
    return (dlist ? (dlist->size == 0) : true);
}

static 
bool xdlist_unique_impl(XDList_PT dlist, bool deep, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    if (xdlist_sort(dlist, cmp, cl)) {
        for (XRDList_PT step = dlist->head; step && step->next; /*nothing*/) {
            if (0 == cmp(step->value, step->next->value, cl)) {
                void *value = NULL;

                if (!xdlist_remove_after(dlist, step, &value)) {
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

bool xdlist_unique(XDList_PT dlist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xdlist_unique_impl(dlist, false, cmp, cl);
}

bool xdlist_deep_unique(XDList_PT dlist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xdlist_unique_impl(dlist, true, cmp, cl);
}

bool xdlist_sort(XDList_PT dlist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(dlist);
    xassert(cmp);

    if (!dlist || !cmp) {
        return false;
    }

    if (dlist->size <= 1) {
        return true;
    }

    dlist->head = xrdlist_merge_sort_impl(dlist->head, cmp, cl);

    /* get the right pointer of slist->tail */
    XRDList_PT step = dlist->head;
    while (step && step->next) {
        step = step->next;
    }
    dlist->tail = step;

    xassert(xdlist_is_sorted(dlist, cmp, cl));

    return true;
}

bool xdlist_is_sorted(XDList_PT dlist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(dlist);
    xassert(cmp);

    if (!dlist || !cmp) {
        return false;
    }

    {
        for (XRDList_PT step = dlist->head; step && step->next; /*nothing*/) {
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
