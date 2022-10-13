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
#include "xlist_d_raw_x.h"

/* Note :
*    no input check for efficiency reason since XRDList_PT is a raw double list
*/

XRDList_PT xrdlist_new(void *value) {
    XRDList_PT dlist = XMEM_CALLOC(1, sizeof(*dlist));
    if (!dlist) {
        return NULL;
    }

    dlist->prev = NULL;
    dlist->next = NULL;
    dlist->value = value;

    return dlist;
}

XRDList_PT xrdlist_copy_impl(XRDList_PT dlist, int value_size, bool deep) {
    XRDList_PT ndlist = NULL;
    XRDList_PT *pp = &ndlist, prev = NULL, step = NULL;

    for (step = dlist; step; step = step->next) {
        XMEM_NEW0(*pp);

        if (!*pp) {
            if (ndlist) {
                deep ? xrdlist_deep_free(&ndlist) : xrdlist_free(&ndlist);
            }
            return NULL;
        }

        if (deep) {
            (*pp)->value = XMEM_CALLOC(1, value_size);
            if (!(*pp)->value) {
                xrdlist_deep_free(&ndlist);
                return NULL;
            }
            memcpy((*pp)->value, step->value, value_size);
        }
        else {
            (*pp)->value = step->value;
        }

        (*pp)->prev = prev;
        prev = *pp;

        pp = &(*pp)->next;
    }
    *pp = NULL;

    return ndlist;
}

XRDList_PT xrdlist_copy(XRDList_PT dlist) {
    return xrdlist_copy_impl(dlist, 0, false);
}

XRDList_PT xrdlist_deep_copy(XRDList_PT dlist, int value_size) {
    return xrdlist_copy_impl(dlist, value_size, true);
}

bool xrdlist_push_front_repeat(XRDList_PT *pdlist, void *value) {
    XRDList_PT p = xrdlist_new(value);
    if (!p) {
        return false;
    }

    p->next = *pdlist;
    (*pdlist)->prev = p;
    *pdlist = p;

    return true;
}

void* xrdlist_pop_front(XRDList_PT *pdlist) {
    XRDList_PT p = *pdlist;
    void* ret = p->value;

    *pdlist = p->next;
    if (*pdlist) {
        (*pdlist)->prev = NULL;
    }

    XMEM_FREE(p);

    return ret;
}

int xrdlist_push_front_unique(XRDList_PT *pdlist, void *value) {
    if (!xrdlist_find(*pdlist, value)) {
        return xrdlist_push_front_repeat(pdlist, value) ? 1 : -1;
    }

    return 0;
}

int xrdlist_push_front_unique_if(XRDList_PT *pdlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    if (!xrdlist_find_equal_if(*pdlist, value, cmp, cl)) {
        return xrdlist_push_front_repeat(pdlist, value) ? 1 : -1;
    }

    return 0;
}

static
int xrdlist_replace_if_impl(XRDList_PT *pdlist, void *value, bool deep, bool push_replace(XRDList_PT *pdlist, void *value), int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    XRDList_PT node = xrdlist_find_equal_if(*pdlist, value, cmp, cl);
    if (node) {
        if (deep && (value != node->value)) {
            XMEM_FREE(node->value);
        }

        node->value = value;
        return 0;
    }

    return push_replace(pdlist, value) ? 1 : -1;
}

int xrdlist_push_front_replace_if(XRDList_PT *pdlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xrdlist_replace_if_impl(pdlist, value, false, xrdlist_push_front_repeat, cmp, cl);
}

int xrdlist_push_front_deep_replace_if(XRDList_PT *pdlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xrdlist_replace_if_impl(pdlist, value, true, xrdlist_push_front_repeat, cmp, cl);
}

void* xrdlist_front(XRDList_PT dlist) {
    return dlist ? dlist->value : NULL;
}

bool xrdlist_insert_before(XRDList_PT *pdlist, XRDList_PT node, void *value) {
    XRDList_PT p = xrdlist_new(value);
    if (!p) {
        return false;
    }

    p->prev = node->prev;
    p->next = node;
    node->prev ? (node->prev->next = p) : (*pdlist = p);
    node->prev = p;

    return true;
}

bool xrdlist_insert_after(XRDList_PT dlist, XRDList_PT node, void *value) {
    XRDList_PT p = xrdlist_new(value);
    if (!p) {
        return false;
    }

    p->prev = node;
    p->next = node->next;

    if (node->next) {
        node->next->prev = p;
    }
    node->next = p;

    return true;
}

bool xrdlist_remove_before(XRDList_PT *pdlist, XRDList_PT node, void **value) {
    XRDList_PT p = node->prev;
    if (value) {
        *value = p->value;
    }

    node->prev = p->prev;
    node->prev ? (node->prev->next = node) : (*pdlist = node);

    XMEM_FREE(p);

    return true;
}

bool xrdlist_remove_after(XRDList_PT dlist, XRDList_PT node, void **value) {
    XRDList_PT p = node->next;
    if (value) {
        *value = p->value;
    }

    node->next = p->next;
    if (p->next) {
        p->next->prev = node;
    }

    XMEM_FREE(p);

    return true;
}

int xrdlist_remove_if_impl(XRDList_PT *pdlist, bool deep, bool break_first, int(*cmp)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2) {
    XRDList_PT *step = pdlist, p = NULL, prev = NULL;
    int count = 0;

    for (p = *step; p; p = *step) {
        bool ret = apply ? apply(p->value, cl2) : (cmp ? (cmp(p->value, cl2, cl1) == 0) : (p->value == cl2));
        if (!ret) {
            prev = p;
            step = &p->next;
            continue;
        }

        *step = p->next;
        if (p->next) {
            p->next->prev = prev;
        }

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

bool xrdlist_remove(XRDList_PT *pdlist, void *value) {
    return (0 <= xrdlist_remove_if_impl(pdlist, false, true, NULL, NULL, NULL, value)) ? true : false;
}

int xrdlist_remove_all(XRDList_PT *pdlist, void *value) {
    return xrdlist_remove_if_impl(pdlist, false, false, NULL, NULL, NULL, value);
}

bool xrdlist_deep_remove(XRDList_PT *pdlist, void **value) {
    if (0 < xrdlist_remove_if_impl(pdlist, true, true, NULL, NULL, NULL, *value)) {
        *value = NULL;
        return true;
    }

    return false;
}

int xrdlist_remove_apply_if(XRDList_PT *pdlist, bool (*apply)(void *value, void *cl), void *cl) {
    return xrdlist_remove_if_impl(pdlist, false, false, NULL, NULL, apply, cl);
}

int xrdlist_remove_equal_if(XRDList_PT *pdlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xrdlist_remove_if_impl(pdlist, false, false, cmp, cl, NULL, value);
}

bool xrdlist_remove_apply_break_if(XRDList_PT *pdlist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 <= xrdlist_remove_if_impl(pdlist, false, true, NULL, NULL, apply, cl));
}

bool xrdlist_remove_equal_break_if(XRDList_PT *pdlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return (0 <= xrdlist_remove_if_impl(pdlist, false, true, cmp, cl, NULL, value));
}

int xrdlist_deep_remove_apply_if(XRDList_PT *pdlist, bool (*apply)(void *value, void *cl), void *cl) {
    return xrdlist_remove_if_impl(pdlist, true, false, NULL, NULL, apply, cl);
}

int xrdlist_deep_remove_equal_if(XRDList_PT *pdlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xrdlist_remove_if_impl(pdlist, true, false, cmp, cl, NULL, value);
}

bool xrdlist_deep_remove_apply_break_if(XRDList_PT *pdlist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 <= xrdlist_remove_if_impl(pdlist, true, true, NULL, NULL, apply, cl));
}

bool xrdlist_deep_remove_equal_break_if(XRDList_PT *pdlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    return (0 <= xrdlist_remove_if_impl(pdlist, true, true, cmp, cl, NULL, value));
}

static
void xrdlist_free_impl(XRDList_PT *dlist, bool deep, bool (*apply)(void *value, void *cl), void *cl) {
    XRDList_PT step = NULL, np = NULL;

    for (step = *dlist; step; step = np) {
        np = step->next;

        if (deep) {
            XMEM_FREE(step->value);
        }
        else if (apply) {
            apply(step->value, cl);
        }

        XMEM_FREE(step);
    }
    *dlist = NULL;
}

void xrdlist_free(XRDList_PT *pdlist) {
    xrdlist_free_impl(pdlist, false, NULL, NULL);
}

void xrdlist_free_apply(XRDList_PT *pdlist, bool (*apply)(void *value, void *cl), void *cl) {
    xrdlist_free_impl(pdlist, false, apply, cl);
}

void xrdlist_deep_free(XRDList_PT *pdlist) {
    xrdlist_free_impl(pdlist, true, NULL, NULL);
}

bool xrdlist_swap(XRDList_PT *pdlist1, XRDList_PT *pdlist2) {
    XRDList_PT dlist = *pdlist1;
    *pdlist1 = *pdlist2;
    *pdlist2 = dlist;
    return true;
}

void xrdlist_reverse(XRDList_PT *pdlist) {
    XRDList_PT p = NULL, np = NULL, prev = NULL;

    for (p = *pdlist; p; p = np) {
        np = p->next;
        p->next = prev;
        p->prev = np;
        prev = p;
    }
    *pdlist = prev;
}

static
int xrdlist_map_impl(XRDList_PT dlist, bool break_first, bool break_true, XRDList_PT *node, int(*equal)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2) {
    int count = 0;

    for (XRDList_PT step = dlist; step; step = step->next) {
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

int xrdlist_map(XRDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    return xrdlist_map_impl(dlist, false, false, NULL, NULL, NULL, apply, cl);
}

bool xrdlist_map_break_if_true(XRDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 < xrdlist_map_impl(dlist, true, true, NULL, NULL, NULL, apply, cl));
}

bool xrdlist_map_break_if_false(XRDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    return (0 < xrdlist_map_impl(dlist, true, false, NULL, NULL, NULL, apply, cl));
}

XRDList_PT xrdlist_find(XRDList_PT dlist, void *value) {
    XRDList_PT node = NULL;
    xrdlist_map_impl(dlist, true, true, &node, NULL, NULL, NULL, value);
    return node;
}

XRDList_PT xrdlist_find_equal_if(XRDList_PT dlist, void *value, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    XRDList_PT node = NULL;
    xrdlist_map_impl(dlist, true, true, &node, cmp, cl, NULL, value);
    return node;
}

XRDList_PT xrdlist_find_apply_if(XRDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl) {
    XRDList_PT node = NULL;
    xrdlist_map_impl(dlist, true, true, &node, NULL, NULL, apply, cl);
    return node;
}

bool xrdlist_merge(XRDList_PT dlist1, XRDList_PT *pdlist2) {
    XRDList_PT *pp = &dlist1, prev = NULL;
    for (; *pp;) {
        prev = *pp;
        pp = &(*pp)->next;
    }

    *pp = *pdlist2;
    (*pdlist2)->prev = prev;
    *pdlist2 = NULL;

    return true;
}

int xrdlist_size(XRDList_PT dlist) {
    int count = 0;

    while (dlist) {
        ++count;
        dlist = dlist->next;
    }

    return count;
}

static
XRDList_PT xrdlist_merge_sort_impl_merge(XRDList_PT dlist1, XRDList_PT dlist2, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    struct XRDList head = { NULL, NULL, NULL };
    XRDList_PT *ndlist = &head.next, prev = NULL;
    while (dlist1 && dlist2) {
        if (cmp(dlist1->value, dlist2->value, cl) <= 0) {
            *ndlist = dlist1;
            dlist1 = dlist1->next;
        }
        else {
            *ndlist = dlist2;
            dlist2 = dlist2->next;
        }

        (*ndlist)->prev = prev;
        prev = *ndlist;

        ndlist = &(*ndlist)->next;
    }

    *ndlist = dlist1 ? dlist1 : dlist2;

    if (*ndlist) {
        (*ndlist)->prev = prev;
    }

    return head.next;
}

/* <<Algorithms in C>> Third Edition : chapter 8.7 */
XRDList_PT xrdlist_merge_sort_impl(XRDList_PT dlist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    if (!dlist->next) {
        return dlist;
    }

    {
        /* make "left" to be the first node of slist
        *  move "right" to be the middle node of slist
        */
        XRDList_PT left = dlist;
        XRDList_PT right = dlist->next;
        while (right && right->next) {
            dlist = dlist->next;
            right = right->next->next;
        }
        right = dlist->next;
        dlist->next = NULL;
        right->prev = NULL;

        {
            left = xrdlist_merge_sort_impl(left, cmp, cl);
            right = xrdlist_merge_sort_impl(right, cmp, cl);
            return xrdlist_merge_sort_impl_merge(left, right, cmp, cl);
        }
    }
}

bool xrdlist_sort(XRDList_PT *pdlist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    *pdlist = xrdlist_merge_sort_impl(*pdlist, cmp, cl);

    xassert(xrdlist_is_sorted(*pdlist, cmp, cl));

    return true;
}

bool xrdlist_is_sorted(XRDList_PT dlist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
    for (XRDList_PT step = dlist; step && step->next; /*nothing*/) {
        if (0 < cmp(step->value, step->next->value, cl)) {
            return false;
        }
        else {
            step = step->next;
        }
    }

    return true;
}
