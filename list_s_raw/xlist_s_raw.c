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

/* Note :
*    no input check for efficiency reason since XRSList_PT is a raw single list
*/

#include <stddef.h>
#include <string.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "xlist_s_raw_x.h"

XRSList_PT xrslist_new(void *value) {
    XRSList_PT slist = XMEM_CALLOC(1, sizeof(*slist));
    if (!slist) {
        return NULL;
    }

    //slist->next = NULL;
    slist->value = value;

    return slist;
}

static
XRSList_PT xrslist_copy_impl(XRSList_PT slist, int value_size, bool deep) {
    XRSList_PT nslist = NULL, step = NULL;
    XRSList_PT *pp = &nslist;

    for (step = slist; step; step = step->next) {
        XMEM_NEW0(*pp);

        if (!*pp) {
            if (nslist) {
                deep ? xrslist_deep_free(&nslist) : xrslist_free(&nslist);
            }
            return NULL;
        }

        if (deep) {
            (*pp)->value = XMEM_CALLOC(1, value_size);
            if (!(*pp)->value) {
                xrslist_deep_free(&nslist);
                return NULL;
            }
            memcpy((*pp)->value, step->value, value_size);
        }
        else {
            (*pp)->value = step->value;
        }

        pp = &(*pp)->next;
    }
    *pp = NULL;

    return nslist;
}

XRSList_PT xrslist_copy(XRSList_PT slist) {
    return xrslist_copy_impl(slist, 0, false);
}

XRSList_PT xrslist_deep_copy(XRSList_PT slist, int value_size) {
    return xrslist_copy_impl(slist, value_size, true);
}

bool xrslist_push_front_repeat(XRSList_PT *pslist, void *value) {
    XRSList_PT p = xrslist_new(value);
    if (!p) {
        return false;
    }

    p->next = *pslist;
    *pslist = p;

    return true;
}

void* xrslist_pop_front(XRSList_PT *pslist) {
    XRSList_PT p = *pslist;
    void *ret = p->value;

    *pslist = p->next;
    XMEM_FREE(p);

    return ret;
}

void* xrslist_pop_kth(XRSList_PT *pslist, int k) {
    if (0 == k) {
        return xrslist_pop_front(pslist);
    }

    {
        void *ret = NULL;
        XRSList_PT step = *pslist;

        for (; step && (1 < k); step = step->next) {
            --k;
        }

        /* no enough element */
        if ((1 != k) || !step || !step->next) {
            return NULL;
        }

        ret = step->next->value;

        {
            XRSList_PT tmp = step->next;
            step->next = step->next->next;
            XMEM_FREE(tmp);
        }
        return ret;
    }
}

int xrslist_push_front_unique(XRSList_PT *pslist, void *value) {
    if (!xrslist_find(*pslist, value)) {
        return xrslist_push_front_repeat(pslist, value) ? 1 : -1;
    }

    return 0;
}

int xrslist_push_front_unique_if(XRSList_PT *pslist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    if (!xrslist_find_equal_if(*pslist, value, cmp, cl)) {
        return xrslist_push_front_repeat(pslist, value) ? 1 : -1;
    }

    return 0;
}

static
int xrslist_replace_if_impl(XRSList_PT *pslist, void *value, bool deep, bool push_replace(XRSList_PT *slist, void *value), int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    XRSList_PT node = xrslist_find_equal_if(*pslist, value, cmp, cl);
    if (node) {
        if (deep && (value != node->value)) {
            XMEM_FREE(node->value);
        }
        node->value = value;
        return 0;
    }

    return push_replace(pslist, value) ? 1 : -1;
}

int xrslist_push_front_replace_if(XRSList_PT *pslist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xrslist_replace_if_impl(pslist, value, false, xrslist_push_front_repeat, cmp, cl);
}

int xrslist_push_front_deep_replace_if(XRSList_PT *pslist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xrslist_replace_if_impl(pslist, value, true, xrslist_push_front_repeat, cmp, cl);
}

void* xrslist_front(XRSList_PT slist) {
    return slist ? slist->value : NULL;
}

bool xrslist_insert_after(XRSList_PT slist, XRSList_PT node, void *value) {
    XRSList_PT p = xrslist_new(value);
    if (!p) {
        return false;
    }

    p->next = node->next;
    node->next = p;

    return true;
}

bool xrslist_remove_after(XRSList_PT slist, XRSList_PT node, void **value) {
    XRSList_PT p = node->next;
    if (value) {
        *value = p->value;
    }

    node->next = p->next;
    XMEM_FREE(p);

    return true;
}

int xrslist_remove_if_impl(XRSList_PT *pslist, bool deep, bool break_first, int(*cmp)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2) {
    XRSList_PT *step = pslist, p = NULL;
    int count = 0;

    for (p = *step; p; p = *step) {
        bool ret = apply ? apply(p->value, cl2) : (cmp ? (cmp(p->value, cl2, cl1) == 0) : (p->value == cl2));

        if (!ret) {
            step = &p->next;
            continue;
        }

        *step = p->next;

        if (deep) {
            XMEM_FREE(p->value);
        }
        XMEM_FREE(p);

        ++count;

        if (break_first) {
            break;
        }
    }

    return count;
}

bool xrslist_remove(XRSList_PT *pslist, void *value) {
    return (0 <= xrslist_remove_if_impl(pslist, false, true, NULL, NULL, NULL, value)) ? true : false;
}

bool xrslist_deep_remove(XRSList_PT *pslist, void **value) {
    if (0 < xrslist_remove_if_impl(pslist, true, true, NULL, NULL, NULL, *value)) {
        *value = NULL;
        return true;
    }

    return false;
}

int xrslist_remove_all(XRSList_PT *pslist, void *value) {
    return xrslist_remove_if_impl(pslist, false, false, NULL, NULL, NULL, value);
}

int xrslist_remove_apply_if(XRSList_PT *pslist, bool (*apply)(void *value, void *cl), void *cl) {
    return xrslist_remove_if_impl(pslist, false, false, NULL, NULL, apply, cl);
}

int xrslist_deep_remove_apply_if(XRSList_PT *pslist, bool (*apply)(void *value, void *cl), void *cl) {
    return xrslist_remove_if_impl(pslist, true, false, NULL, NULL, apply, cl);
}

int xrslist_remove_equal_if(XRSList_PT *pslist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xrslist_remove_if_impl(pslist, false, false, cmp, cl, NULL, value);
}

int xrslist_deep_remove_equal_if(XRSList_PT *pslist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xrslist_remove_if_impl(pslist, true, false, cmp, cl, NULL, value);
}

bool xrslist_remove_apply_break_if(XRSList_PT *pslist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 <= xrslist_remove_if_impl(pslist, false, true, NULL, NULL, apply, cl));
}

bool xrslist_deep_remove_apply_break_if(XRSList_PT *pslist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 <= xrslist_remove_if_impl(pslist, true, true, NULL, NULL, apply, cl));
}

bool xrslist_remove_equal_break_if(XRSList_PT *pslist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return (0 <= xrslist_remove_if_impl(pslist, false, true, cmp, cl, NULL, value));
}

bool xrslist_deep_remove_equal_break_if(XRSList_PT *pslist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return (0 <= xrslist_remove_if_impl(pslist, true, true, cmp, cl, NULL, value));
}

static
void xrslist_free_impl(XRSList_PT *slist, bool deep, bool (*apply)(void *value, void *cl), void *cl) {
    XRSList_PT step = *slist, np = NULL;

    for (/*nothing*/; step; step = np) {
        np = step->next;

        if (deep) {
            XMEM_FREE(step->value);
        }
        else if (apply) {
            apply(step->value, cl);
        }
        XMEM_FREE(step);
    }

    *slist = NULL;
}

void xrslist_free(XRSList_PT *pslist) {
    xrslist_free_impl(pslist, false, NULL, NULL);
}

void xrslist_free_apply(XRSList_PT *pslist, bool (*apply)(void *value, void *cl), void *cl) {
    xrslist_free_impl(pslist, false, apply, cl);
}

void xrslist_deep_free(XRSList_PT *pslist) {
    xrslist_free_impl(pslist, true, NULL, NULL);
}

int xrslist_free_except_front(XRSList_PT slist) {
    int count = 0;

    if (slist && slist->next) {
        XRSList_PT step = slist->next, np = NULL;
        for (; step; step = np) {
            np = step->next;
            XMEM_FREE(step);
            ++count;
        }
        slist->next = NULL;
    }

    return count;
}

bool xrslist_swap(XRSList_PT *pslist1, XRSList_PT *pslist2) {
    XRSList_PT slist = *pslist1;
    *pslist1 = *pslist2;
    *pslist2 = slist;
    return true;
}

void xrslist_reverse(XRSList_PT *pslist) {
    XRSList_PT p = NULL, np = NULL, prev = NULL;

    for (p = *pslist; p; p = np) {
        np = p->next;
        p->next = prev;
        prev = p;
    }
    *pslist = prev;
}

static
int xrslist_map_impl(XRSList_PT slist, bool break_first, bool break_true, XRSList_PT *node, int(*equal)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2) {
    int count = 0;

    for (XRSList_PT step = slist; step; step = step->next) {
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

                count++;
                break;
            }
        }
    }

    return count;
}

int xrslist_map(XRSList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    return xrslist_map_impl(slist, false, false, NULL, NULL, NULL, apply, cl);
}

bool xrslist_map_break_if_true(XRSList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 < xrslist_map_impl(slist, true, true, NULL, NULL, NULL, apply, cl));
}

bool xrslist_map_break_if_false(XRSList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 < xrslist_map_impl(slist, true, false, NULL, NULL, NULL, apply, cl));
}

XRSList_PT xrslist_find(XRSList_PT slist, void *value) {
    XRSList_PT node = NULL;
    xrslist_map_impl(slist, true, true, &node, NULL, NULL, NULL, value);
    return node;
}

XRSList_PT xrslist_find_equal_if(XRSList_PT slist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    XRSList_PT node = NULL;
    xrslist_map_impl(slist, true, true, &node, cmp, cl, NULL, value);
    return node;
}

XRSList_PT xrslist_find_apply_if(XRSList_PT slist, bool (*apply)(void *value, void *cl), void *cl) {
    XRSList_PT node = NULL;
    xrslist_map_impl(slist, true, true, &node, NULL, NULL, apply, cl);
    return node;
}

XRSList_PT xrslist_get_kth(XRSList_PT slist, int k) {
    XRSList_PT step = slist;
    for (; step && (0 < k); step = step->next) {
        --k;
    }
    return step;
}

bool xrslist_merge(XRSList_PT slist1, XRSList_PT *pslist2) {
    XRSList_PT *pp = &slist1;
    for (; *pp;) {
        pp = &(*pp)->next;
    }

    *pp = *pslist2;
    *pslist2 = NULL;

    return true;
}

int xrslist_size(XRSList_PT slist) {
    int count = 0;

    while (slist) {
        ++count;
        slist = slist->next;
    }

    return count;
}

static 
XRSList_PT xrslist_merge_sort_impl_merge(XRSList_PT slist1, XRSList_PT slist2, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    struct XRSList head = {NULL, NULL};
    XRSList_PT *nslist = &head.next;
    while (slist1 && slist2) {
        if (cmp(slist1->value, slist2->value, cl) <= 0) {
            *nslist = slist1;
            slist1 = slist1->next;
        }
        else {
            *nslist = slist2;
            slist2 = slist2->next;
        }
        nslist = &(*nslist)->next;
    }

    *nslist = slist1 ? slist1 : slist2;
    return head.next;
}

/* <<Algorithms in C>> Third Edition : chapter 8.7 */
XRSList_PT xrslist_merge_sort_impl(XRSList_PT slist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    if (!slist->next) {
        return slist;
    }

    {
        /* make "left" to be the first node of slist 
        *  move "right" to be the middle node of slist
        */
        XRSList_PT left = slist;
        XRSList_PT right = slist->next;
        while (right && right->next) {
            slist = slist->next;
            right = right->next->next;
        }
        right = slist->next;
        slist->next = NULL;

        {
            left = xrslist_merge_sort_impl(left, cmp, cl);
            right = xrslist_merge_sort_impl(right, cmp, cl);
            return xrslist_merge_sort_impl_merge(left, right, cmp, cl);
        }
    }
}

/* other sort methods :
 *    put the pointer of value into a array, then use xparray_*_sort to sort it,
 *    then, save the value pointer back to list one by one.
 *    benefit : 1. use xparray_*_sort directly
 *              2. keep the list link order (->next) not changed, just move the value pointer
 */
bool xrslist_sort(XRSList_PT *pslist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    *pslist = xrslist_merge_sort_impl(*pslist, cmp, cl);

    xassert(xrslist_is_sorted(*pslist, cmp, cl));

    return true;
}

bool xrslist_is_sorted(XRSList_PT slist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    for (XRSList_PT step = slist; step && step->next; /*nothing*/) {
        if (0 < cmp(step->value, step->next->value, cl)) {
            return false;
        }
        else {
            step = step->next;
        }
    }

    return true;
}
