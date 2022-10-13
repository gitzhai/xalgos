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

#include "../include/xassert.h"
#include "../list_d_raw/xlist_d_raw_x.h"
#include "../list_d/xlist_d_x.h"
#include "xlist_dc_x.h"

static inline
void xdclist_uncycle(XDCList_PT dclist) {
    if (dclist && dclist->head) {
        dclist->head->prev = NULL;
        dclist->tail->next = NULL;
    }
}

static inline
void xdclist_cycle(XDCList_PT dclist) {
    if (dclist && dclist->head) {
        dclist->head->prev = dclist->tail;
        dclist->tail->next = dclist->head;
    }
}

XDCList_PT xdclist_new(void) {
    return xdlist_new();
}

int xdclist_vload_repeat(XDCList_PT dlist, void *value, ...) {
    int count = 0;

    xdclist_uncycle(dlist);
    {
        va_list ap;
        va_start(ap, value);
        for (; value; value = va_arg(ap, void *)) {
            if (!xdlist_push_back_repeat(dlist, value)) {
                break;
            }

            ++count;
        }
        va_end(ap);
    }
    xdclist_cycle(dlist);

    return count;
}

int xdclist_vload_unique(XDCList_PT dlist, void *value, ...) {
    int count = 0;

    xdclist_uncycle(dlist);
    {
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
    }
    xdclist_cycle(dlist);

    return count;
}

int xdclist_vload_unique_if(XDCList_PT dlist, int(*cmp)(void *value1, void *value2, void *cl), void *cl, void *value, ...) {
    int count = 0;

    xdclist_uncycle(dlist);
    {
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
    }
    xdclist_cycle(dlist);

    return count;
}

int xdclist_vload_replace_if(XDCList_PT dlist, int(*cmp)(void *value1, void *value2, void *cl), void *cl, void *value, ...) {
    int count = 0;

    xdclist_uncycle(dlist);
    {
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
    }
    xdclist_cycle(dlist);

    return count;
}

int xdclist_aload_repeat(XDCList_PT dlist, XPArray_PT xs) {
    int count = 0;

    xdclist_uncycle(dlist);
    count = xdlist_aload_repeat(dlist, xs);
    xdclist_cycle(dlist);

    return count;
}

int xdclist_aload_unique(XDCList_PT dlist, XPArray_PT xs) {
    int count = 0;

    xdclist_uncycle(dlist);
    count = xdlist_aload_unique(dlist, xs);
    xdclist_cycle(dlist);

    return count;
}

int xdclist_aload_unique_if(XDCList_PT dlist, XPArray_PT xs, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    int count = 0;

    xdclist_uncycle(dlist);
    count = xdlist_aload_unique_if(dlist, xs, cmp, cl);
    xdclist_cycle(dlist);

    return count;
}

int xdclist_aload_replace_if(XDCList_PT dlist, XPArray_PT xs, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    int count = 0;

    xdclist_uncycle(dlist);
    count = xdlist_aload_replace_if(dlist, xs, cmp, cl);
    xdclist_cycle(dlist);

    return count;
}

XDCList_PT xdclist_copyn_if_impl(XDCList_PT dlist, int value_size, int count, bool deep, int(*cmp)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2) {
    XDCList_PT ndlist = NULL;

    xdclist_uncycle(dlist);
    ndlist = xdlist_copyn_if_impl(dlist, value_size, count, deep, cmp, cl1, apply, cl2);
    xdclist_cycle(dlist);

    xdclist_cycle(ndlist);

    return ndlist;
}

XDCList_PT xdclist_copyn_equal_if(XDCList_PT dlist, int count, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(value);

    if (!value) {
        return NULL;
    }

    return xdclist_copyn_if_impl(dlist, 0, count, false, cmp, cl, NULL, value);
}

XDCList_PT xdclist_copyn_apply_if(XDCList_PT dlist, int count, bool (*apply)(void *value, void *cl), void *cl) {
    return xdclist_copyn_if_impl(dlist, 0, count, false, NULL, NULL, apply, cl);
}

XDCList_PT xdclist_copyn(XDCList_PT dlist, int count) {
    return xdclist_copyn_apply_if(dlist, count, NULL, NULL);
}

XDCList_PT xdclist_copy(XDCList_PT dlist) {
    return xdclist_copyn(dlist, (dlist ? dlist->size : 0));
}

XDCList_PT xdclist_copy_equal_if(XDCList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xdclist_copyn_equal_if(dlist, (dlist ? dlist->size : 0), value, cmp, cl);
}

XDCList_PT xdclist_copy_apply_if(XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    return xdclist_copyn_apply_if(dlist, (dlist ? dlist->size : 0), apply, cl);
}

XDCList_PT xdclist_deep_copyn_equal_if(XDCList_PT dlist, int value_size, int count, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(value);

    if (!value) {
        return NULL;
    }

    return xdclist_copyn_if_impl(dlist, value_size, count, true, cmp, cl, NULL, value);
}

XDCList_PT xdclist_deep_copyn_apply_if(XDCList_PT dlist, int value_size, int count, bool (*apply)(void *value, void *cl), void *cl) {
    return xdclist_copyn_if_impl(dlist, value_size, count, true, NULL, NULL, apply, cl);
}

XDCList_PT xdclist_deep_copyn(XDCList_PT dlist, int value_size, int count) {
    return xdclist_deep_copyn_apply_if(dlist, value_size, count, NULL, NULL);
}

XDCList_PT xdclist_deep_copy(XDCList_PT dlist, int value_size) {
    return xdclist_deep_copyn(dlist, value_size, (dlist ? dlist->size : 0));
}

XDCList_PT xdclist_deep_copy_equal_if(XDCList_PT dlist, int value_size, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xdclist_deep_copyn_equal_if(dlist, value_size, (dlist ? dlist->size : 0), value, cmp, cl);
}

XDCList_PT xdclist_deep_copy_apply_if(XDCList_PT dlist, int value_size, bool (*apply)(void *value, void *cl), void *cl) {
    return xdclist_deep_copyn_apply_if(dlist, value_size, (dlist ? dlist->size : 0), apply, cl);
}

bool xdclist_push_front_repeat(XDCList_PT dlist, void *x) {
    bool ret = false;

    xdclist_uncycle(dlist);
    ret = xdlist_push_front_repeat(dlist, x);
    xdclist_cycle(dlist);

    return ret;
}

void* xdclist_pop_front(XDCList_PT dlist) {
    void* ret = NULL;

    xdclist_uncycle(dlist);
    ret = xdlist_pop_front(dlist);
    xdclist_cycle(dlist);

    return ret;
}

void* xdclist_front(XDCList_PT dlist) {
    return xdlist_front(dlist);
}

void* xdclist_back(XDCList_PT dlist) {
    return xdlist_back(dlist);
}

XRDList_PT xdclist_front_node(XDCList_PT dlist) {
    return xdlist_front_node(dlist);
}

XRDList_PT xdclist_back_node(XDCList_PT dlist) {
    return xdlist_back_node(dlist);
}

bool xdclist_push_back_repeat(XDCList_PT dlist, void *x) {
    bool ret = false;

    xdclist_uncycle(dlist);
    ret = xdlist_push_back_repeat(dlist, x);
    xdclist_cycle(dlist);

    return ret;
}

void* xdclist_pop_back(XDCList_PT dlist) {
    void* ret = NULL;

    xdclist_uncycle(dlist);
    ret = xdlist_pop_back(dlist);
    xdclist_cycle(dlist);

    return ret;
}

int xdclist_push_front_unique(XDCList_PT dlist, void *value) {
    int count = 0;

    xdclist_uncycle(dlist);
    count = xdlist_push_front_unique(dlist, value);
    xdclist_cycle(dlist);

    return count;
}

int xdclist_push_front_unique_if(XDCList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    int count = 0;

    xdclist_uncycle(dlist);
    count = xdlist_push_front_unique_if(dlist, value, cmp, cl);
    xdclist_cycle(dlist);

    return count;
}

int xdclist_push_front_replace_if(XDCList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    int count = 0;

    xdclist_uncycle(dlist);
    count = xdlist_push_front_replace_if(dlist, value, cmp, cl);
    xdclist_cycle(dlist);

    return count;
}

int xdclist_push_front_deep_replace_if(XDCList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    int count = 0;

    xdclist_uncycle(dlist);
    count = xdlist_push_front_deep_replace_if(dlist, value, cmp, cl);
    xdclist_cycle(dlist);

    return count;
}

int xdclist_push_back_unique(XDCList_PT dlist, void *value) {
    int count = 0;

    xdclist_uncycle(dlist);
    count = xdlist_push_back_unique(dlist, value);
    xdclist_cycle(dlist);

    return count;
}

int xdclist_push_back_unique_if(XDCList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    int count = 0;

    xdclist_uncycle(dlist);
    count = xdlist_push_back_unique_if(dlist, value, cmp, cl);
    xdclist_cycle(dlist);

    return count;
}

int xdclist_push_back_replace_if(XDCList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    int count = 0;

    xdclist_uncycle(dlist);
    count = xdlist_push_back_replace_if(dlist, value, cmp, cl);
    xdclist_cycle(dlist);

    return count;
}

int xdclist_push_back_deep_replace_if(XDCList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    int count = 0;

    xdclist_uncycle(dlist);
    count = xdlist_push_back_deep_replace_if(dlist, value, cmp, cl);
    xdclist_cycle(dlist);

    return count;
}

bool xdclist_insert_before(XDCList_PT dlist, XRDList_PT node, void *value) {
    bool ret = false;

    xdclist_uncycle(dlist);
    ret = xdlist_insert_before(dlist, node, value);
    xdclist_cycle(dlist);

    return ret;
}

bool xdclist_insert_after(XDCList_PT dlist, XRDList_PT node, void *value) {
    bool ret = false;

    xdclist_uncycle(dlist);
    ret = xdlist_insert_after(dlist, node, value);
    xdclist_cycle(dlist);

    return ret;
}

bool xdclist_remove_before(XDCList_PT dlist, XRDList_PT node, void **value) {
    bool ret = false;

    xdclist_uncycle(dlist);
    ret = xdlist_remove_before(dlist, node, value);
    xdclist_cycle(dlist);

    return ret;
}

bool xdclist_remove_after(XDCList_PT dlist, XRDList_PT node, void **value) {
    bool ret = false;

    xdclist_uncycle(dlist);
    ret = xdlist_remove_after(dlist, node, value);
    xdclist_cycle(dlist);

    return ret;
}

int xdclist_remove_if_impl(XDCList_PT dlist, bool deep, bool break_first, int(*cmp)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2) {
    int count = 0;

    xdclist_uncycle(dlist);
    count = xdlist_remove_if_impl(dlist, deep, break_first, cmp, cl1, apply, cl2);
    xdclist_cycle(dlist);

    return count;
}

bool xdclist_remove(XDCList_PT dlist, void *value) {
    xassert(value);

    if (!value) {
        return false;
    }

    return (0 <= xdclist_remove_if_impl(dlist, false, true, NULL, NULL, NULL, value)) ? true : false;
}

int xdclist_remove_all(XDCList_PT dlist, void *value) {
    xassert(value);

    if (!value) {
        return -1;
    }

    return xdclist_remove_if_impl(dlist, false, false, NULL, NULL, NULL, value);
}

bool xdclist_deep_remove(XDCList_PT dlist, void **value) {
    bool ret = false;

    xdclist_uncycle(dlist);
    ret = xdlist_deep_remove(dlist, value);
    xdclist_cycle(dlist);

    return ret;
}

int xdclist_remove_apply_if(XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    return xdclist_remove_if_impl(dlist, false, false, NULL, NULL, apply, cl);
}

int xdclist_remove_equal_if(XDCList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xdclist_remove_if_impl(dlist, false, false, cmp, cl, NULL, value);
}

bool xdclist_remove_apply_break_if(XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 <= xdclist_remove_if_impl(dlist, false, true, NULL, NULL, apply, cl));
}

bool xdclist_remove_equal_break_if(XDCList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return (0 <= xdclist_remove_if_impl(dlist, false, true, cmp, cl, NULL, value));
}

int xdclist_deep_remove_apply_if(XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    return xdclist_remove_if_impl(dlist, true, false, NULL, NULL, apply, cl);
}

int xdclist_deep_remove_equal_if(XDCList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xdclist_remove_if_impl(dlist, true, false, cmp, cl, NULL, value);
}

bool xdclist_deep_remove_apply_break_if(XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 <= xdclist_remove_if_impl(dlist, true, true, NULL, NULL, apply, cl));
}

bool xdclist_deep_remove_equal_break_if(XDCList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return (0 <= xdclist_remove_if_impl(dlist, true, true, cmp, cl, NULL, value));
}

void xdclist_free(XDCList_PT *pdlist) {
    if (!pdlist) {
        return;
    }

    xdclist_uncycle(*pdlist);
    xdlist_free(pdlist);
}

void xdclist_free_apply(XDCList_PT *pdlist, bool (*apply)(void *value, void *cl), void *cl) {
    if (!pdlist) {
        return;
    }

    xdclist_uncycle(*pdlist);
    xdlist_free_apply(pdlist, apply, cl);
}

void xdclist_deep_free(XDCList_PT *pdlist) {
    if (!pdlist) {
        return;
    }

    xdclist_uncycle(*pdlist);
    xdlist_deep_free(pdlist);
}

void xdclist_clear(XDCList_PT dlist) {
    xdclist_uncycle(dlist);
    xdlist_clear(dlist);
}

void xdclist_clear_apply(XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    xdclist_uncycle(dlist);
    xdlist_clear_apply(dlist, apply, cl);
}

void xdclist_deep_clear(XDCList_PT dlist) {
    xdclist_uncycle(dlist);
    xdlist_deep_clear(dlist);
}

bool xdclist_swap(XDCList_PT dlist1, XDCList_PT dlist2) {
    bool ret = false;

    xdclist_uncycle(dlist1);
    xdclist_uncycle(dlist2);

    ret = xdlist_swap(dlist1, dlist2);

    xdclist_cycle(dlist1);
    xdclist_cycle(dlist2);

    return ret;
}

int xdclist_merge(XDCList_PT dlist1, XDCList_PT* pdlist2) {
    xassert(pdlist2);

    if (!pdlist2) {
        return -1;
    }

    xdclist_uncycle(dlist1);
    xdclist_uncycle(*pdlist2);

    {
        int count = xdlist_merge(dlist1, pdlist2);

        xdclist_cycle(dlist1);

        return count;
    }
}

void xdclist_reverse(XDCList_PT dlist) {
    xdclist_uncycle(dlist);
    xdlist_reverse(dlist);
    xdclist_cycle(dlist);
}

void xdclist_rotate(XDCList_PT dlist, int n) {
    xassert(dlist);

    if (!dlist) {
        return;
    }

    {
        if (0 <= n)
            n = n % dlist->size;
        else
            n = dlist->size - (-n) % dlist->size;

        {
            XRDList_PT head = dlist->head;
            if (n <= dlist->size / 2)
                for (int i = n; 0 < i--; )
                    head = head->next;
            else
                for (int i = dlist->size - n; 0 < i--; )
                    head = head->prev;

            dlist->head = head;
            dlist->tail = dlist->head->prev;
        }
    }
}

int xdclist_map(XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    int count = 0;

    xdclist_uncycle(dlist);
    count = xdlist_map(dlist, apply, cl);
    xdclist_cycle(dlist);

    return count;
}

bool xdclist_map_break_if_true(XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    bool ret = false;

    xdclist_uncycle(dlist);
    ret = xdlist_map_break_if_true(dlist, apply, cl);
    xdclist_cycle(dlist);

    return ret;
}

bool xdclist_map_break_if_false(XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    bool ret = false;

    xdclist_uncycle(dlist);
    ret = xdlist_map_break_if_false(dlist, apply, cl);
    xdclist_cycle(dlist);

    return ret;
}

XRDList_PT xdclist_find(XDCList_PT dlist, void *value) {
    XRDList_PT node = NULL;

    xdclist_uncycle(dlist);
    node = xdlist_find(dlist, value);
    xdclist_cycle(dlist);

    return node;
}

XRDList_PT xdclist_find_equal_if(XDCList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    XRDList_PT node = NULL;

    xdclist_uncycle(dlist);
    node = xdlist_find_equal_if(dlist, value, cmp, cl);
    xdclist_cycle(dlist);

    return node;
}

XRDList_PT xdclist_find_apply_if(XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    XRDList_PT node = NULL;

    xdclist_uncycle(dlist);
    node = xdlist_find_apply_if(dlist, apply, cl);
    xdclist_cycle(dlist);

    return node;
}

XPArray_PT xdclist_to_array(XDCList_PT dlist) {
    XPArray_PT array = NULL;

    xdclist_uncycle(dlist);
    array = xdlist_to_array(dlist);
    xdclist_cycle(dlist);

    return array;
}

int xdclist_size(XDCList_PT dlist) {
    return (dlist ? dlist->size : 0);
}

bool xdclist_is_empty(XDCList_PT dlist) {
    return (dlist ? (dlist->size == 0) : true);
}

bool xdclist_unique(XDCList_PT dlist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    bool ret = false;

    xdclist_uncycle(dlist);
    ret = xdlist_unique(dlist, cmp, cl);
    xdclist_cycle(dlist);

    return ret;
}

bool xdclist_deep_unique(XDCList_PT dlist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    bool ret = false;

    xdclist_uncycle(dlist);
    ret = xdlist_deep_unique(dlist, cmp, cl);
    xdclist_cycle(dlist);

    return ret;
}

bool xdclist_sort(XDCList_PT dlist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    bool ret = false;

    xdclist_uncycle(dlist);
    ret = xdlist_sort(dlist, cmp, cl);
    xdclist_cycle(dlist);

    return ret;
}

bool xdclist_is_sorted(XDCList_PT dlist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    bool ret = false;

    xdclist_uncycle(dlist);
    ret = xdlist_is_sorted(dlist, cmp, cl);
    xdclist_cycle(dlist);

    return ret;
}