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
#include "../list_s_raw/xlist_s_raw_x.h"
#include "../list_s/xlist_s_x.h"
#include "xlist_sc_x.h"

static inline 
void xsclist_uncycle(XSCList_PT slist) {
    if (slist && slist->head) {
        slist->tail->next = NULL;
    }
}

static inline 
void xsclist_cycle(XSCList_PT slist) {
    if (slist && slist->head) {
        slist->tail->next = slist->head;
    }
}

XSCList_PT xsclist_new(void) {
    return xslist_new();
}

int xsclist_vload_repeat(XSCList_PT slist, void *value, ...) {
    int count = 0;

    xsclist_uncycle(slist);
    {
        va_list ap;
        va_start(ap, value);
        for (; value; value = va_arg(ap, void *)) {
            if (!xslist_push_back_repeat(slist, value)) {
                break;
            }

            ++count;
        }
        va_end(ap);
    }
    xsclist_cycle(slist);

    return count;
}

int xsclist_vload_unique(XSCList_PT slist, void *value, ...) {
    int count = 0;

    xsclist_uncycle(slist);
    {
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
    }
    xsclist_cycle(slist);

    return count;
}

int xsclist_vload_unique_if(XSCList_PT slist, int(*cmp)(void *value1, void *value2, void *cl), void *cl, void *value, ...) {
    int count = 0;

    xsclist_uncycle(slist);
    {
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
    }
    xsclist_cycle(slist);

    return count;
}

int xsclist_vload_replace_if(XSCList_PT slist, int(*cmp)(void *value1, void *value2, void *cl), void *cl, void *value, ...) {
    int count = 0;

    xsclist_uncycle(slist);
    {
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
    }
    xsclist_cycle(slist);

    return count;
}

int xsclist_aload_repeat(XSCList_PT slist, XPArray_PT xs) {
    int count = 0;

    xsclist_uncycle(slist);
    count = xslist_aload_repeat(slist, xs);
    xsclist_cycle(slist);

    return count;
}

int xsclist_aload_unique(XSCList_PT slist, XPArray_PT xs) {
    int count = 0;

    xsclist_uncycle(slist);
    count = xslist_aload_unique(slist, xs);
    xsclist_cycle(slist);

    return count;
}

int xsclist_aload_unique_if(XSCList_PT slist, XPArray_PT xs, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    int count = 0;

    xsclist_uncycle(slist);
    count = xslist_aload_unique_if(slist, xs, cmp, cl);
    xsclist_cycle(slist);

    return count;
}

int xsclist_aload_replace_if(XSCList_PT slist, XPArray_PT xs, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    int count = 0;

    xsclist_uncycle(slist);
    count = xslist_aload_replace_if(slist, xs, cmp, cl);
    xsclist_cycle(slist);

    return count;
}

XSCList_PT xsclist_copyn_if_impl(XSCList_PT slist, int value_size, int count, bool deep, int(*cmp)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2) {
    XSCList_PT nslist = NULL;

    xsclist_uncycle(slist);
    nslist = xslist_copyn_if_impl(slist, value_size, count, deep, cmp, cl1, apply, cl2);
    xsclist_cycle(slist);

    xsclist_cycle(nslist);

    return nslist;
}

XSCList_PT xsclist_copyn_equal_if(XSCList_PT slist, int count, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(value);

    if (!value) {
        return NULL;
    }

    return xsclist_copyn_if_impl(slist, 0, count, false, cmp, cl, NULL, value);
}

XSCList_PT xsclist_copyn_apply_if(XSCList_PT slist, int count, bool (*apply)(void *value, void *cl), void *cl) {
    return xsclist_copyn_if_impl(slist, 0, count, false, NULL, NULL, apply, cl);
}

XSCList_PT xsclist_copyn(XSCList_PT slist, int count) {
    return xsclist_copyn_apply_if(slist, count, NULL, NULL);
}

XSCList_PT xsclist_copy(XSCList_PT slist) {
    return xsclist_copyn(slist, (slist ? slist->size : 0));
}

XSCList_PT xsclist_copy_equal_if(XSCList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xsclist_copyn_equal_if(slist, (slist ? slist->size : 0), value, cmp, cl);
}

XSCList_PT xsclist_copy_apply_if(XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    return xsclist_copyn_apply_if(slist, (slist ? slist->size : 0), apply, cl);
}

XSCList_PT xsclist_deep_copyn_equal_if(XSCList_PT slist, int value_size, int count, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(value);

    if (!value) {
        return NULL;
    }

    return xsclist_copyn_if_impl(slist, value_size, count, true, cmp, cl, NULL, value);
}

XSCList_PT xsclist_deep_copyn_apply_if(XSCList_PT slist, int value_size, int count, bool (*apply)(void *value, void *cl), void *cl) {
    return xsclist_copyn_if_impl(slist, value_size, count, true, NULL, NULL, apply, cl);
}

XSCList_PT xsclist_deep_copyn(XSCList_PT slist, int value_size, int count) {
    return xsclist_deep_copyn_apply_if(slist, value_size, count, NULL, NULL);
}

XSCList_PT xsclist_deep_copy(XSCList_PT slist, int value_size) {
    return xsclist_deep_copyn(slist, value_size, (slist ? slist->size : 0));
}

XSCList_PT xsclist_deep_copy_equal_if(XSCList_PT slist, int value_size, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xsclist_deep_copyn_equal_if(slist, value_size, (slist ? slist->size : 0), value, cmp, cl);
}

XSCList_PT xsclist_deep_copy_apply_if(XSCList_PT slist, int value_size, bool (*apply)(void *value, void *cl), void *cl) {
    return xsclist_deep_copyn_apply_if(slist, value_size, (slist ? slist->size : 0), apply, cl);
}

bool xsclist_push_front_repeat(XSCList_PT slist, void *x) {
    bool ret = false;

    xsclist_uncycle(slist);
    ret = xslist_push_front_repeat(slist, x);
    xsclist_cycle(slist);

    return ret;
}

void* xsclist_pop_front(XSCList_PT slist) {
    void *ret = NULL;

    xsclist_uncycle(slist);
    ret = xslist_pop_front(slist);
    xsclist_cycle(slist);

    return ret;
}

void* xsclist_front(XSCList_PT slist) {
    return xslist_front(slist);
}

void* xsclist_back(XSCList_PT slist) {
    return xslist_back(slist);
}

XRSList_PT xsclist_front_node(XSCList_PT slist) {
    return xslist_front_node(slist);
}

XRSList_PT xsclist_back_node(XSCList_PT slist) {
    return xslist_back_node(slist);
}

bool xsclist_push_back_repeat(XSCList_PT slist, void *x) {
    bool ret = false;

    xsclist_uncycle(slist);
    ret = xslist_push_back_repeat(slist, x);
    xsclist_cycle(slist);

    return ret;
}

int xsclist_push_front_unique(XSCList_PT slist, void *value) {
    int count = 0;

    xsclist_uncycle(slist);
    count = xslist_push_front_unique(slist, value);
    xsclist_cycle(slist);

    return count;
}

int xsclist_push_front_unique_if(XSCList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    int count = 0;

    xsclist_uncycle(slist);
    count = xslist_push_front_unique_if(slist, value, cmp, cl);
    xsclist_cycle(slist);

    return count;
}

int xsclist_push_front_replace_if(XSCList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    int count = 0;

    xsclist_uncycle(slist);
    count = xslist_push_front_replace_if(slist, value, cmp, cl);
    xsclist_cycle(slist);

    return count;
}

int xsclist_push_front_deep_replace_if(XSCList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    int count = 0;

    xsclist_uncycle(slist);
    count = xslist_push_front_deep_replace_if(slist, value, cmp, cl);
    xsclist_cycle(slist);

    return count;
}

int xsclist_push_back_unique(XSCList_PT slist, void *value) {
    int count = 0;

    xsclist_uncycle(slist);
    count = xslist_push_back_unique(slist, value);
    xsclist_cycle(slist);

    return count;
}

int xsclist_push_back_unique_if(XSCList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    int count = 0;

    xsclist_uncycle(slist);
    count = xslist_push_back_unique_if(slist, value, cmp, cl);
    xsclist_cycle(slist);

    return count;
}

int xsclist_push_back_replace_if(XSCList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    int count = 0;

    xsclist_uncycle(slist);
    count = xslist_push_back_replace_if(slist, value, cmp, cl);
    xsclist_cycle(slist);

    return count;
}

int xsclist_push_back_deep_replace_if(XSCList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    int count = 0;

    xsclist_uncycle(slist);
    count = xslist_push_back_deep_replace_if(slist, value, cmp, cl);
    xsclist_cycle(slist);

    return count;
}

bool xsclist_insert_after(XSCList_PT slist, XRSList_PT node, void *value) {
    bool ret = false;

    xsclist_uncycle(slist);
    ret = xslist_insert_after(slist, node, value);
    xsclist_cycle(slist);

    return ret;
}

void* xsclist_remove_after(XSCList_PT slist, XRSList_PT node) {
    void* ret = NULL;

    xsclist_uncycle(slist);
    ret = xslist_remove_after(slist, node);
    xsclist_cycle(slist);

    return ret;
}

int xsclist_remove_if_impl(XSCList_PT slist, bool deep, bool break_first, int(*cmp)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2) {
    int count = 0;

    xsclist_uncycle(slist);
    count = xslist_remove_if_impl(slist, deep, break_first, cmp, cl1, apply, cl2);
    xsclist_cycle(slist);

    return count;
}

bool xsclist_remove(XSCList_PT slist, void *value) {
    xassert(value);

    if (!value) {
        return false;
    }

    return (0 <= xsclist_remove_if_impl(slist, false, true, NULL, NULL, NULL, value)) ? true : false;
}

int xsclist_remove_all(XSCList_PT slist, void *value) {
    xassert(value);

    if (!value) {
        return -1;
    }

    return xsclist_remove_if_impl(slist, false, false, NULL, NULL, NULL, value);
}

bool xsclist_deep_remove(XSCList_PT slist, void **value) {
    bool ret = false;

    xsclist_uncycle(slist);
    ret = xslist_deep_remove(slist, value);
    xsclist_cycle(slist);

    return ret;
}

int xsclist_remove_apply_if(XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    return xsclist_remove_if_impl(slist, false, false, NULL, NULL, apply, cl);
}

int xsclist_remove_equal_if(XSCList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xsclist_remove_if_impl(slist, false, false, cmp, cl, NULL, value);
}

bool xsclist_remove_apply_break_if(XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 <= xsclist_remove_if_impl(slist, false, true, NULL, NULL, apply, cl));
}

bool xsclist_remove_equal_break_if(XSCList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return (0 <= xsclist_remove_if_impl(slist, false, true, cmp, cl, NULL, value));
}

int xsclist_deep_remove_apply_if(XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    return xsclist_remove_if_impl(slist, true, false, NULL, NULL, apply, cl);
}

int xsclist_deep_remove_equal_if(XSCList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xsclist_remove_if_impl(slist, true, false, cmp, cl, NULL, value);
}

bool xsclist_deep_remove_apply_break_if(XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 <= xsclist_remove_if_impl(slist, true, true, NULL, NULL, apply, cl));
}

bool xsclist_deep_remove_equal_break_if(XSCList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return (0 <= xsclist_remove_if_impl(slist, true, true, cmp, cl, NULL, value));
}

void xsclist_free(XSCList_PT *pslist) {
    if (!pslist) {
        return;
    }

    xsclist_uncycle(*pslist);
    xslist_free(pslist);
}

void xsclist_free_apply(XSCList_PT *pslist, bool (*apply)(void *value, void *cl), void *cl) {
    if (!pslist) {
        return;
    }

    xsclist_uncycle(*pslist);
    xslist_free_apply(pslist, apply, cl);
}

void xsclist_deep_free(XSCList_PT *pslist) {
    if (!pslist) {
        return;
    }

    xsclist_uncycle(*pslist);
    xslist_deep_free(pslist);
}

void xsclist_clear(XSCList_PT slist) {
    xsclist_uncycle(slist);
    xslist_clear(slist);
}

void xsclist_clear_apply(XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    xsclist_uncycle(slist);
    xslist_clear_apply(slist, apply, cl);
}

void xsclist_deep_clear(XSCList_PT slist) {
    xsclist_uncycle(slist);
    xslist_deep_clear(slist);
}

bool xsclist_swap(XSCList_PT slist1, XSCList_PT slist2) {
    bool ret = false;

    xsclist_uncycle(slist1);
    xsclist_uncycle(slist2);

    ret = xslist_swap(slist1, slist2);

    xsclist_cycle(slist1);
    xsclist_cycle(slist2);

    return ret;
}

int xsclist_merge(XSCList_PT slist1, XSCList_PT* pslist2) {
    xassert(pslist2);

    if (!pslist2) {
        return -1;
    }

    xsclist_uncycle(slist1);
    xsclist_uncycle(*pslist2);

    {
        int count = xslist_merge(slist1, pslist2);

        xsclist_cycle(slist1);

        return count;
    }
}

void xsclist_reverse(XSCList_PT slist) {
    xsclist_uncycle(slist);
    xslist_reverse(slist);
    xsclist_cycle(slist);
}

int xsclist_map(XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    int count = 0;

    xsclist_uncycle(slist);
    count = xslist_map(slist, apply, cl);
    xsclist_cycle(slist);

    return count;
}

bool xsclist_map_break_if_true(XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    bool ret = false;

    xsclist_uncycle(slist);
    ret = xslist_map_break_if_true(slist, apply, cl);
    xsclist_cycle(slist);

    return ret;
}

bool xsclist_map_break_if_false(XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    bool ret = false;

    xsclist_uncycle(slist);
    ret = xslist_map_break_if_false(slist, apply, cl);
    xsclist_cycle(slist);

    return ret;
}

XRSList_PT xsclist_find(XSCList_PT slist, void *value) {
    XRSList_PT node = NULL;

    xsclist_uncycle(slist);
    node = xslist_find(slist, value);
    xsclist_cycle(slist);

    return node;
}

XRSList_PT xsclist_find_equal_if(XSCList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    XRSList_PT node = NULL;

    xsclist_uncycle(slist);
    node = xslist_find_equal_if(slist, value, cmp, cl);
    xsclist_cycle(slist);

    return node;
}

XRSList_PT xsclist_find_apply_if(XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    XRSList_PT node = NULL;

    xsclist_uncycle(slist);
    node = xslist_find_apply_if(slist, apply, cl);
    xsclist_cycle(slist);

    return node;
}

XPArray_PT xsclist_to_array(XSCList_PT slist) {    
    XPArray_PT array = NULL;

    xsclist_uncycle(slist);
    array = xslist_to_array(slist);
    xsclist_cycle(slist);

    return array;
}

int xsclist_size(XSCList_PT slist) {
    return (slist ? slist->size : 0);
}

bool xsclist_is_empty(XSCList_PT slist) {
    return (slist ? (slist->size == 0) : true);
}

bool xsclist_unique(XSCList_PT slist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    bool ret = false;

    xsclist_uncycle(slist);
    ret = xslist_unique(slist, cmp, cl);
    xsclist_cycle(slist);

    return ret;
}

bool xsclist_deep_unique(XSCList_PT slist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    bool ret = false;

    xsclist_uncycle(slist);
    ret = xslist_deep_unique(slist, cmp, cl);
    xsclist_cycle(slist);

    return ret;
}

bool xsclist_sort(XSCList_PT slist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    bool ret = false;

    xsclist_uncycle(slist);
    ret = xslist_sort(slist, cmp, cl);
    xsclist_cycle(slist);

    return ret;
}

bool xsclist_is_sorted(XSCList_PT slist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    bool ret = false;

    xsclist_uncycle(slist);
    ret = xslist_is_sorted(slist, cmp, cl);
    xsclist_cycle(slist);

    return ret;
}

