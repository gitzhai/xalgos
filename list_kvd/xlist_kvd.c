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
#include "../include/xpair.h"
#include "../list_d_raw/xlist_d_raw_x.h"
#include "../list_d/xlist_d_x.h"
#include "../array_pointer/xarray_pointer_x.h"
#include "xlist_kvd_x.h"

XKVDList_PT xkvdlist_new(void) {
    return xdlist_new();
}

static
XRDList_PT xkvdlist_new_node(void *key, void *value) {
    XPair_PT pair = xpair_new(key, value);
    if (!pair) {
        return NULL;
    }

    XRDList_PT node = xrdlist_new(pair);
    if (!node) {
        xpair_free(&pair);
        return NULL;
    }

    return node;
}

int xkvdlist_vload_repeat(XKVDList_PT dlist, void *key, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, key);
    for (; key; key = va_arg(ap, void *)) {
        void* value = va_arg(ap, void *);
        if (!value) {
            break;
        }

        if (!xkvdlist_push_back_repeat(dlist, key, value)) {
            break;
        }

        ++count;
    }
    va_end(ap);

    return count;
}

int xkvdlist_vload_unique(XKVDList_PT dlist, void *key, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, key);
    for (; key; key = va_arg(ap, void *)) {
        void* value = va_arg(ap, void *);
        if (!value) {
            break;
        }

        int ret = xkvdlist_push_back_unique(dlist, key, value);
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

int xkvdlist_vload_unique_if(XKVDList_PT dlist, int(*cmp)(void *key1, void *key2, void *cl), void *cl, void *key, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, key);
    for (; key; key = va_arg(ap, void *)) {
        void* value = va_arg(ap, void *);
        if (!value) {
            break;
        }

        int ret = xkvdlist_push_back_unique_if(dlist, key, value, cmp, cl);
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

int xkvdlist_vload_replace(XKVDList_PT dlist, void *key, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, key);
    for (; key; key = va_arg(ap, void *)) {
        void* value = va_arg(ap, void *);
        if (!value) {
            break;
        }

        int ret = xkvdlist_push_back_replace(dlist, key, value);
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

int xkvdlist_vload_replace_if(XKVDList_PT dlist, int(*cmp)(void *key1, void *key2, void *cl), void *cl, void *key, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, key);
    for (; key; key = va_arg(ap, void *)) {
        void* value = va_arg(ap, void *);
        if (!value) {
            break;
        }

        int ret = xkvdlist_push_back_replace_if(dlist, key, value, cmp, cl);
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
int xkvdlist_aload_impl(XKVDList_PT dlist, XPArray_PT xs, bool repeat,
                        bool  (*push_back_repeat) (XKVDList_PT dlist, void *key, void *value),
                        int   (*push_back)        (XKVDList_PT dlist, void *key, void *value),
                        int   (*push_back_if)     (XKVDList_PT dlist, void *key, void *value, int(*cmp)(void *key1, void *key2, void *cl), void *cl),
                        int   (*cmp)              (void *key1, void *key2, void *cl), void *cl) {
    xassert(xs);

    if (!xs) {
        return 0;
    }

    {
        int count = 0;

        for (int i = 0; i < xs->size; i += 2) {
            void *key = xparray_get_impl(xs, i);
            void *value = xparray_get_impl(xs, i + 1);
            if (!key || !value) {
                continue;
            }

            if (repeat) {
                if (!push_back_repeat(dlist, key, value)) {
                    break;
                }
                ++count;
            }
            else {
                int ret = cmp ? push_back_if(dlist, key, value, cmp, cl) : push_back(dlist, key, value);
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

int xkvdlist_aload_repeat(XKVDList_PT dlist, XPArray_PT xs) {
    return xkvdlist_aload_impl(dlist, xs, true, xkvdlist_push_back_repeat, NULL, NULL, NULL, NULL);
}

int xkvdlist_aload_unique(XKVDList_PT dlist, XPArray_PT xs) {
    return xkvdlist_aload_impl(dlist, xs, false, NULL, xkvdlist_push_back_unique, NULL, NULL, NULL);
}

int xkvdlist_aload_unique_if(XKVDList_PT dlist, XPArray_PT xs, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(cmp);

    if (!cmp) {
        return -1;
    }

    return xkvdlist_aload_impl(dlist, xs, false, NULL, NULL, xkvdlist_push_back_unique_if, cmp, cl);
}

int xkvdlist_aload_replace(XKVDList_PT dlist, XPArray_PT xs) {
    return xkvdlist_aload_impl(dlist, xs, false, NULL, xkvdlist_push_back_replace, NULL, NULL, NULL);
}

int xkvdlist_aload_replace_if(XKVDList_PT dlist, XPArray_PT xs, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(cmp);

    if (!cmp) {
        return -1;
    }

    return xkvdlist_aload_impl(dlist, xs, false, NULL, NULL, xkvdlist_push_back_replace_if, cmp, cl);
}

XKVDList_PT xkvdlist_copyn_if_impl(XKVDList_PT dlist, int key_size, int value_size, int count, bool deep, 
                                  int(*cmp)(void *key1, void *key2, void *cl1), void *cl1,
                                  bool (*applyk)(void *key, void *cl2), 
                                  bool (*applykv)(void *key, void *value, void *cl2), 
                                  void *cl2) {
    xassert(dlist);
    xassert(0 <= count);

    if (!dlist || (count < 0)) {
        return NULL;
    }

    if (deep) {
        xassert(0 < key_size);
        if (key_size <= 0) {
            return NULL;
        }
    }

    if (dlist->size < count) {
        count = dlist->size;
    }

    {
        XKVDList_PT ndlist = xkvdlist_new();
        if (!ndlist) {
            return NULL;
        }

        if (count == 0 || dlist->size == 0) {
            return ndlist;
        }

        {
            XRDList_PT *pp = &(ndlist->head), prev = NULL, step = NULL;
            int number = 0;

            for (step = dlist->head; step && (0 < count); step = step->next) {
                bool ret = cmp ? (cmp(xpair_first(step->value), cl2, cl1) == 0) : (applykv ? applykv(xpair_first(step->value), xpair_second(step->value), cl2) : (applyk ? applyk(xpair_first(step->value), cl2) : true));
                /* ignore the item if ret == false */
                if (!ret) {
                    continue;
                }

                XMEM_NEW0(*pp);
                if (!*pp) {
                    deep ? xkvdlist_deep_free(&ndlist) : xkvdlist_free(&ndlist);
                    return NULL;
                }

                if (deep) {
                    (*pp)->value = xpair_deep_copy(step->value, key_size, value_size);
                    if (!(*pp)->value) {
                        xkvdlist_deep_free(&ndlist);
                        return NULL;
                    }
                }
                else {
                    (*pp)->value = xpair_copy(step->value);
                    if (!(*pp)->value) {
                        xkvdlist_free(&ndlist);
                        return NULL;
                    }
                }

                (*pp)->prev = prev;
                prev = *pp;

                ndlist->tail = *pp;
                pp = &(*pp)->next;

                ++number;
                --count;
            }
            *pp = NULL;
            ndlist->size = number;
        }

        return ndlist;
    }
}

XKVDList_PT xkvdlist_copyn_equal_if(XKVDList_PT dlist, int count, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(key);

    if (!key) {
        return NULL;
    }

    return xkvdlist_copyn_if_impl(dlist, 0, 0, count, false, cmp, cl, NULL, NULL, key);
}

XKVDList_PT xkvdlist_copyn_apply_if(XKVDList_PT dlist, int count, bool (*apply)(void *key, void *value, void *cl), void *cl) {
    return xkvdlist_copyn_if_impl(dlist, 0, 0, count, false, NULL, NULL, NULL, apply, cl);
}

XKVDList_PT xkvdlist_copyn_apply_key_if(XKVDList_PT dlist, int count, bool (*apply)(void *key, void *cl), void *cl) {
    return xkvdlist_copyn_if_impl(dlist, 0, 0, count, false, NULL, NULL, apply, NULL, cl);
}

XKVDList_PT xkvdlist_copyn(XKVDList_PT dlist, int count) {
    return xkvdlist_copyn_apply_if(dlist, count, NULL, NULL);
}

XKVDList_PT xkvdlist_copy(XKVDList_PT dlist) {
    return xkvdlist_copyn(dlist, (dlist ? dlist->size : 0));
}

XKVDList_PT xkvdlist_copy_equal_if(XKVDList_PT dlist, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvdlist_copyn_equal_if(dlist, (dlist ? dlist->size : 0), key, cmp, cl);
}

XKVDList_PT xkvdlist_copy_apply_if(XKVDList_PT dlist, bool (*apply)(void *key, void *value, void *cl), void *cl) {
    return xkvdlist_copyn_apply_if(dlist, (dlist ? dlist->size : 0), apply, cl);
}

XKVDList_PT xkvdlist_copy_apply_key_if(XKVDList_PT dlist, bool (*apply)(void *key, void *cl), void *cl) {
    return xkvdlist_copyn_apply_key_if(dlist, (dlist ? dlist->size : 0), apply, cl);
}

XKVDList_PT xkvdlist_deep_copyn_equal_if(XKVDList_PT dlist, int key_size, int value_size, int count, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(key);

    if (!key) {
        return NULL;
    }

    return xkvdlist_copyn_if_impl(dlist, key_size, value_size, count, true, cmp, cl, NULL, NULL, key);
}

XKVDList_PT xkvdlist_deep_copyn_apply_if(XKVDList_PT dlist, int key_size, int value_size, int count, bool (*apply)(void *key, void *value, void *cl), void *cl) {
    return xkvdlist_copyn_if_impl(dlist, key_size, value_size, count, true, NULL, NULL, NULL, apply, cl);
}

XKVDList_PT xkvdlist_deep_copyn_apply_key_if(XKVDList_PT dlist, int key_size, int value_size, int count, bool (*apply)(void *key, void *cl), void *cl) {
    return xkvdlist_copyn_if_impl(dlist, key_size, value_size, count, true, NULL, NULL, apply, NULL, cl);
}

XKVDList_PT xkvdlist_deep_copyn(XKVDList_PT dlist, int key_size, int value_size, int count) {
    return xkvdlist_deep_copyn_apply_if(dlist, key_size, value_size, count, NULL, NULL);
}

XKVDList_PT xkvdlist_deep_copy(XKVDList_PT dlist, int key_size, int value_size) {
    return xkvdlist_deep_copyn(dlist, key_size, value_size, (dlist ? dlist->size : 0));
}

XKVDList_PT xkvdlist_deep_copy_equal_if(XKVDList_PT dlist, int key_size, int value_size, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvdlist_deep_copyn_equal_if(dlist, key_size, value_size, (dlist ? dlist->size : 0), key, cmp, cl);
}

XKVDList_PT xkvdlist_deep_copy_apply_if(XKVDList_PT dlist, int key_size, int value_size, bool (*apply)(void *key, void *value, void *cl), void *cl) {
    return xkvdlist_deep_copyn_apply_if(dlist, key_size, value_size, (dlist ? dlist->size : 0), apply, cl);
}

XKVDList_PT xkvdlist_deep_copy_apply_key_if(XKVDList_PT dlist, int key_size, int value_size, bool (*apply)(void *key, void *cl), void *cl) {
    return xkvdlist_deep_copyn_apply_key_if(dlist, key_size, value_size, (dlist ? dlist->size : 0), apply, cl);
}

bool xkvdlist_push_front_repeat(XKVDList_PT dlist, void *key, void *value) {
    xassert(dlist);
    xassert(key);

    if (!dlist || !key) {
        return false;
    }

    {
        XRDList_PT p = xkvdlist_new_node(key, value);
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

bool xkvdlist_pop_front(XKVDList_PT dlist, void **key, void **value) {
    xassert(dlist);

    if (!dlist || (dlist->size == 0)) {
        return false;
    }

    {
        XRDList_PT p = dlist->head;
        if (key) {
            *key = xpair_first(p->value);
        }
        if (value) {
            *value = xpair_second(p->value);
        }

        dlist->head = p->next;
        dlist->head ? (dlist->head->prev = NULL) : (dlist->tail = NULL);

        xpair_free((XPair_PT*)&p->value);
        XMEM_FREE(p);

        --dlist->size;
    }

    return true;
}

XPair_PT xkvdlist_front(XKVDList_PT dlist) {
    return dlist ? (dlist->head ? (XPair_PT)dlist->head->value : NULL) : NULL;
}

XPair_PT xkvdlist_back(XKVDList_PT dlist) {
    return dlist ? (dlist->tail ? (XPair_PT)dlist->tail->value : NULL) : NULL;
}

XRDList_PT xkvdlist_front_node(XKVDList_PT dlist) {
    return dlist ? dlist->head : NULL;
}

XRDList_PT xkvdlist_back_node(XKVDList_PT dlist) {
    return dlist ? dlist->tail : NULL;
}

bool xkvdlist_push_back_repeat(XKVDList_PT dlist, void *key, void *value) {
    xassert(dlist);
    xassert(key);

    if (!dlist || !key) {
        return false;
    }

    {
        XRDList_PT p = xkvdlist_new_node(key, value);
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

bool xkvdlist_pop_back(XKVDList_PT dlist, void **key, void **value) {
    xassert(dlist);

    if (!dlist || (dlist->size == 0)) {
        return false;
    }

    {
        XRDList_PT p = dlist->tail;
        if (key) {
            *key = xpair_first(p->value);
        }
        if (value) {
            *value = xpair_second(p->value);
        }

        dlist->tail = p->prev;
        dlist->tail ? (dlist->tail->next = NULL) : (dlist->head = NULL);

        xpair_free((XPair_PT*)&p->value);
        XMEM_FREE(p);

        --dlist->size;
    }

    return true;
}

int xkvdlist_push_front_unique(XKVDList_PT dlist, void *key, void *value) {
    if (!xkvdlist_find(dlist, key)) {
        return xkvdlist_push_front_repeat(dlist, key, value) ? 1 : -1;
    }

    return 0;
}

int xkvdlist_push_front_unique_if(XKVDList_PT dlist, void *key, void *value, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    if (!xkvdlist_find_equal_if(dlist, key, cmp, cl)) {
        return xkvdlist_push_front_repeat(dlist, key, value) ? 1 : -1;
    }

    return 0;
}

static
int xkvdlist_replace_if_impl(XKVDList_PT dlist, void *key, void *value, bool deep, bool push_replace(XKVDList_PT dlist, void *key, void *value), int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    XRDList_PT node = xkvdlist_find_equal_if(dlist, key, cmp, cl);
    if (node) {
        if (deep) {
            if (key != xpair_first(node->value)) {
                xpair_free_first(node->value);
            }
            if (value != xpair_second(node->value)) {
                xpair_free_second(node->value);
            }
        }

        xpair_set_first(node->value, key);
        xpair_set_second(node->value, value);
        return 0;
    }

    return push_replace(dlist, key, value) ? 1 : -1;
}

int xkvdlist_push_front_replace(XKVDList_PT dlist, void *key, void *value) {
    return xkvdlist_replace_if_impl(dlist, key, value, false, xkvdlist_push_front_repeat, NULL, NULL);
}

int xkvdlist_push_front_deep_replace(XKVDList_PT dlist, void *key, void *value) {
    return xkvdlist_replace_if_impl(dlist, key, value, true, xkvdlist_push_front_repeat, NULL, NULL);
}

int xkvdlist_push_front_replace_if(XKVDList_PT dlist, void *key, void *value, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvdlist_replace_if_impl(dlist, key, value, false, xkvdlist_push_front_repeat, cmp, cl);
}

int xkvdlist_push_front_deep_replace_if(XKVDList_PT dlist, void *key, void *value, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvdlist_replace_if_impl(dlist, key, value, true, xkvdlist_push_front_repeat, cmp, cl);
}

int xkvdlist_push_back_unique(XKVDList_PT dlist, void *key, void *value) {
    if (!xkvdlist_find(dlist, key)) {
        return xkvdlist_push_back_repeat(dlist, key, value) ? 1 : -1;
    }

    return 0;
}

int xkvdlist_push_back_unique_if(XKVDList_PT dlist, void *key, void *value, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    if (!xkvdlist_find_equal_if(dlist, key, cmp, cl)) {
        return xkvdlist_push_back_repeat(dlist, key, value) ? 1 : -1;
    }

    return 0;
}

int xkvdlist_push_back_replace(XKVDList_PT dlist, void *key, void *value) {
    return xkvdlist_replace_if_impl(dlist, key, value, false, xkvdlist_push_back_repeat, NULL, NULL);
}

int xkvdlist_push_back_deep_replace(XKVDList_PT dlist, void *key, void *value) {
    return xkvdlist_replace_if_impl(dlist, key, value, true, xkvdlist_push_back_repeat, NULL, NULL);
}

int xkvdlist_push_back_replace_if(XKVDList_PT dlist, void *key, void *value, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvdlist_replace_if_impl(dlist, key, value, false, xkvdlist_push_back_repeat, cmp, cl);
}

int xkvdlist_push_back_deep_replace_if(XKVDList_PT dlist, void *key, void *value, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvdlist_replace_if_impl(dlist, key, value, true, xkvdlist_push_back_repeat, cmp, cl);
}

bool xkvdlist_insert_before(XKVDList_PT dlist, XRDList_PT node, void *key, void *value) {
    if (!node) {
        return xkvdlist_push_front_repeat(dlist, key, value);
    }

    xassert(dlist);
    xassert(key);

    if (!dlist || !key) {
        return false;
    }

    {
        XRDList_PT p = xkvdlist_new_node(key, value);
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

bool xkvdlist_insert_after(XKVDList_PT dlist, XRDList_PT node, void *key, void *value) {
    if (!node) {
        return xkvdlist_push_back_repeat(dlist, key, value);
    }

    xassert(dlist);
    xassert(key);

    if (!dlist || !key) {
        return false;
    }

    {
        XRDList_PT p = xkvdlist_new_node(key, value);
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

bool xkvdlist_remove_before(XKVDList_PT dlist, XRDList_PT node, void **key, void **value) {
    xassert(dlist);

    if (!dlist || !node || (dlist->size == 0)) {
        return false;
    }

    if (!(node->prev)) {
        return false;
    }

    {
        XRDList_PT p = node->prev;
        if (key) {
            *key = xpair_first(p->value);
        }
        if (value) {
            *value = xpair_second(p->value);
        }
        
        node->prev = p->prev;
        p->prev ? (p->prev->next = node) : (dlist->head = node);

        xpair_free((XPair_PT*)&p->value);
        XMEM_FREE(p);

        --dlist->size;
    }

    return true;
}

bool xkvdlist_remove_after(XKVDList_PT dlist, XRDList_PT node, void **key, void **value) {
    xassert(dlist);

    if (!dlist || !node || (dlist->size == 0)) {
        return false;
    }

    if (!(node->next)) {
        return false;
    }

    {
        XRDList_PT p = node->next;
        if (key) {
            *key = xpair_first(p->value);
        }
        if (value) {
            *value = xpair_second(p->value);
        }

        node->next = p->next;
        node->next ? (node->next->prev = node) : (dlist->tail = node);

        xpair_free((XPair_PT*)&p->value);
        XMEM_FREE(p);

        --dlist->size;
    }

    return true;
}

int xkvdlist_remove_if_impl(XKVDList_PT dlist, bool deep, bool break_first, 
                            int(*cmp)(void *key1, void *key2, void *cl1), void *cl1,
                            bool (*applyk)(void *key, void *cl2), 
                            bool (*applykv)(void *key, void **value, void *cl2), 
                            void *cl2) {
    xassert(dlist);

    if (!dlist) {
        return -1;
    }

    {
        XRDList_PT step = NULL, np = NULL, prev = NULL;
        int count = 0;
        void *value = NULL;

        for (step = dlist->head; step; step = np) {
            np = step->next;

            if (applykv) {
                value = xpair_second(step->value);
            }

            bool ret = cmp ? (cmp(xpair_first(step->value), cl2, cl1) == 0) : (applykv ? applykv(xpair_first(step->value), &value, cl2) : (applyk ? applyk(xpair_first(step->value), cl2) : (xpair_first(step->value) == cl2)));
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

            deep ? xpair_deep_free((XPair_PT*)&step->value) : xpair_free((XPair_PT*)&step->value);
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

bool xkvdlist_remove(XKVDList_PT dlist, void *key) {
    xassert(key);

    if (!key) {
        return false;
    }

    return (0 <= xkvdlist_remove_if_impl(dlist, false, true, NULL, NULL, NULL, NULL, key)) ? true : false;
}

int xkvdlist_remove_all(XKVDList_PT dlist, void *key) {
    xassert(key);

    if (!key) {
        return -1;
    }

    return xkvdlist_remove_if_impl(dlist, false, false, NULL, NULL, NULL, NULL, key);
}

bool xkvdlist_deep_remove(XKVDList_PT dlist, void **key) {
    xassert(key);
    xassert(*key);

    if (!key || !*key) {
        return false;
    }

    int ret = xkvdlist_remove_if_impl(dlist, true, true, NULL, NULL, NULL, NULL, *key);
    if(0 < ret) {
        *key = NULL;
    }

    return 0 <= ret ? true : false;
}

int xkvdlist_remove_apply_if(XKVDList_PT dlist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return xkvdlist_remove_if_impl(dlist, false, false, NULL, NULL, NULL, apply, cl);
}

int xkvdlist_remove_apply_key_if(XKVDList_PT dlist, bool (*apply)(void *key, void *cl), void *cl) {
    return xkvdlist_remove_if_impl(dlist, false, false, NULL, NULL, apply, NULL, cl);
}

int xkvdlist_remove_equal_if(XKVDList_PT dlist, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvdlist_remove_if_impl(dlist, false, false, cmp, cl, NULL, NULL, key);
}

bool xkvdlist_remove_apply_break_if(XKVDList_PT dlist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return (0 <= xkvdlist_remove_if_impl(dlist, false, true, NULL, NULL, NULL, apply, cl));
}

bool xkvdlist_remove_apply_key_break_if(XKVDList_PT dlist, bool (*apply)(void *key, void *cl), void *cl) {
    return (0 <= xkvdlist_remove_if_impl(dlist, false, true, NULL, NULL, apply, NULL, cl));
}

bool xkvdlist_remove_equal_break_if(XKVDList_PT dlist, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return (0 <= xkvdlist_remove_if_impl(dlist, false, true, cmp, cl, NULL, NULL, key));
}

int xkvdlist_deep_remove_apply_if(XKVDList_PT dlist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return xkvdlist_remove_if_impl(dlist, true, false, NULL, NULL, NULL, apply, cl);
}

int xkvdlist_deep_remove_apply_key_if(XKVDList_PT dlist, bool (*apply)(void *key, void *cl), void *cl) {
    return xkvdlist_remove_if_impl(dlist, true, false, NULL, NULL, apply, NULL, cl);
}

int xkvdlist_deep_remove_equal_if(XKVDList_PT dlist, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvdlist_remove_if_impl(dlist, true, false, cmp, cl, NULL, NULL, key);
}

bool xkvdlist_deep_remove_apply_break_if(XKVDList_PT dlist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return (0 <= xkvdlist_remove_if_impl(dlist, true, true, NULL, NULL, NULL, apply, cl));
}

bool xkvdlist_deep_remove_apply_key_break_if(XKVDList_PT dlist, bool (*apply)(void *key, void *cl), void *cl) {
    return (0 <= xkvdlist_remove_if_impl(dlist, true, true, NULL, NULL, apply, NULL, cl));
}

bool xkvdlist_deep_remove_equal_break_if(XKVDList_PT dlist, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return (0 <= xkvdlist_remove_if_impl(dlist, true, true, cmp, cl, NULL, NULL, key));
}

static
void xkvdlist_free_impl(XKVDList_PT dlist, bool deep, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!dlist) {
        return;
    }

    {
        XRDList_PT step = NULL, np = NULL;

        for (step = dlist->head; step; step = np) {
            np = step->next;

            if (apply) {
                xpair_free_apply((XPair_PT*)&step->value, apply, cl);
            }
            else {
                deep ? xpair_deep_free((XPair_PT*)&step->value) : xpair_free((XPair_PT*)&step->value);
            }
            XMEM_FREE(step);
        }
    }
}

void xkvdlist_free(XKVDList_PT *pdlist) {
    if (!pdlist) {
        return;
    }

    xkvdlist_free_impl(*pdlist, false, NULL, NULL);
    XMEM_FREE(*pdlist);
}

void xkvdlist_free_apply(XKVDList_PT *pdlist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!pdlist) {
        return;
    }

    xkvdlist_free_impl(*pdlist, false, apply, cl);
    XMEM_FREE(*pdlist);
}

void xkvdlist_deep_free(XKVDList_PT *pdlist) {
    if (!pdlist) {
        return;
    }

    xkvdlist_free_impl(*pdlist, true, NULL, NULL);
    XMEM_FREE(*pdlist);
}

static
void xkvdlist_clear_impl(XKVDList_PT dlist, bool deep, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!dlist) {
        return;
    }

    xkvdlist_free_impl(dlist, deep, apply, cl);
    dlist->head = NULL;
    dlist->tail = NULL;
    dlist->size = 0;
}

void xkvdlist_clear(XKVDList_PT dlist) {
    xkvdlist_clear_impl(dlist, false, NULL, NULL);
}

void xkvdlist_clear_apply(XKVDList_PT dlist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xkvdlist_clear_impl(dlist, false, apply, cl);
}

void xkvdlist_deep_clear(XKVDList_PT dlist) {
    xkvdlist_clear_impl(dlist, true, NULL, NULL);
}

bool xkvdlist_swap(XKVDList_PT dlist1, XKVDList_PT dlist2) {
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
void xkvdlist_merge_impl(XKVDList_PT dlist1, XKVDList_PT dlist2) {
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

int xkvdlist_merge(XKVDList_PT dlist1, XKVDList_PT *pdlist2) {
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

        xkvdlist_merge_impl(dlist1, *pdlist2);
        XMEM_FREE(*pdlist2);

        return count;
    }
}

void xkvdlist_reverse(XKVDList_PT dlist) {
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
int xkvdlist_map_impl(XKVDList_PT dlist, bool break_first, bool break_true, XRDList_PT *node,
                      int(*equalk)  (void *key1, void *key2, void *cl1), void *cl1,
                      bool (*applyk)  (void *key, void *cl2),
                      bool (*applykv) (void *key, void **value, void *cl2),
                      void *cl2) {
    xassert(dlist);

    if (!dlist) {
        return -1;
    }

    {
        int count = 0;

        for (XRDList_PT step = dlist->head; step; step = step->next) {
            bool ret = false;
            if (equalk) {
                ret = equalk(xpair_first(step->value), cl2, cl1) == 0;
            }
            else {
                void *value = xpair_second(step->value);
                ret = applykv ? applykv(xpair_first(step->value), &value, cl2) : (applyk ? applyk(xpair_first(step->value), cl2) : xpair_first(step->value) == cl2);
            }

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

int xkvdlist_map(XKVDList_PT dlist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return xkvdlist_map_impl(dlist, false, false, NULL, NULL, NULL, NULL, apply, cl);
}

bool xkvdlist_map_break_if_true(XKVDList_PT dlist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return (0 < xkvdlist_map_impl(dlist, true, true, NULL, NULL, NULL, NULL, apply, cl));
}

bool xkvdlist_map_break_if_false(XKVDList_PT dlist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return (0 < xkvdlist_map_impl(dlist, true, false, NULL, NULL, NULL, NULL, apply, cl));
}

int xkvdlist_map_key(XKVDList_PT dlist, bool (*apply)(void *key, void *cl), void *cl) {
    return xkvdlist_map_impl(dlist, false, false, NULL, NULL, NULL, apply, NULL, cl);
}

bool xkvdlist_map_key_break_if_true(XKVDList_PT dlist, bool (*apply)(void *key, void *cl), void *cl) {
    return (0 < xkvdlist_map_impl(dlist, true, true, NULL, NULL, NULL, apply, NULL, cl));
}

bool xkvdlist_map_key_break_if_false(XKVDList_PT dlist, bool (*apply)(void *key, void *cl), void *cl) {
    return (0 < xkvdlist_map_impl(dlist, true, false, NULL, NULL, NULL, apply, NULL, cl));
}

XRDList_PT xkvdlist_find(XKVDList_PT dlist, void *key) {
    XRDList_PT node = NULL;
    xkvdlist_map_impl(dlist, true, true, &node, NULL, NULL, NULL, NULL, key);
    return node;
}

XRDList_PT xkvdlist_find_equal_if(XKVDList_PT dlist, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    XRDList_PT node = NULL;
    xkvdlist_map_impl(dlist, true, true, &node, cmp, cl, NULL, NULL, key);
    return node;
}

XRDList_PT xkvdlist_find_apply_if(XKVDList_PT dlist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    XRDList_PT node = NULL;
    xkvdlist_map_impl(dlist, true, true, &node, NULL, NULL, NULL, apply, cl);
    return node;
}

XRDList_PT xkvdlist_find_apply_key_if(XKVDList_PT dlist, bool (*apply)(void *key, void *cl), void *cl) {
    XRDList_PT node = NULL;
    xkvdlist_map_impl(dlist, true, true, &node, NULL, NULL, apply, NULL, cl);
    return node;
}

XPArray_PT xkvdlist_to_array(XKVDList_PT dlist) {
    xassert(dlist);

    if (!dlist) {
        return NULL;
    }

    {
        XPArray_PT array = xparray_new(dlist->size * 2);
        if (!array) {
            return NULL;
        }

        {
            XRDList_PT step = dlist->head;
            for (int i = 0; step; i += 2, step = step->next) {
                xparray_put_impl(array, i, xpair_first(step->value));
                xparray_put_impl(array, i + 1, xpair_second(step->value));
            }
        }

        return array;
    }
}

XPArray_PT xkvdlist_keys_to_array(XKVDList_PT dlist) {
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
                xparray_put_impl(array, i, xpair_first(step->value));
            }
        }

        return array;
    }
}

XPArray_PT xkvdlist_values_to_array(XKVDList_PT dlist) {
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
                xparray_put_impl(array, i, xpair_second(step->value));
            }
        }

        return array;
    }
}

int xkvdlist_size(XKVDList_PT dlist) {
    return (dlist ? dlist->size : 0);
}

bool xkvdlist_is_empty(XKVDList_PT dlist) {
    return (dlist ? (dlist->size == 0) : true);
}

static 
bool xkvdlist_unique_impl(XKVDList_PT dlist, bool deep, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    if (xkvdlist_sort(dlist, cmp, cl)) {
        for (XRDList_PT step = dlist->head; step && step->next; /*nothing*/) {
            if (0 == cmp(xpair_first(step->value), xpair_first(step->next->value), cl)) {
                void *key = NULL;
                void *value = NULL;

                if (!xkvdlist_remove_after(dlist, step, &key, &value)) {
                    return false;
                }

                if (deep) {
                    XMEM_FREE(key);
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

bool xkvdlist_unique(XKVDList_PT dlist, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvdlist_unique_impl(dlist, false, cmp, cl);
}

bool xkvdlist_deep_unique(XKVDList_PT dlist, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvdlist_unique_impl(dlist, true, cmp, cl);
}

static
XRDList_PT xkvdlist_merge_sort_impl_merge(XRDList_PT dlist1, XRDList_PT dlist2, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    struct XRDList head = { NULL, NULL, NULL };
    XRDList_PT *ndlist = &head.next, prev = NULL;
    while (dlist1 && dlist2) {
        if (cmp(xpair_first(dlist1->value), xpair_first(dlist2->value), cl) <= 0) {
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
XRDList_PT xkvdlist_merge_sort_impl(XRDList_PT dlist, int(*cmp)(void *value1, void *value2, void *cl), void *cl) {
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
            left = xkvdlist_merge_sort_impl(left, cmp, cl);
            right = xkvdlist_merge_sort_impl(right, cmp, cl);
            return xkvdlist_merge_sort_impl_merge(left, right, cmp, cl);
        }
    }
}

bool xkvdlist_sort(XKVDList_PT dlist, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(dlist);
    xassert(cmp);

    if (!dlist || !cmp) {
        return false;
    }

    if (dlist->size <= 1) {
        return true;
    }

    dlist->head = xkvdlist_merge_sort_impl(dlist->head, cmp, cl);

    /* get the right pointer of slist->tail */
    XRDList_PT step = dlist->head;
    while (step && step->next) {
        step = step->next;
    }
    dlist->tail = step;

    xassert(xkvdlist_is_sorted(dlist, cmp, cl));

    return true;
}

bool xkvdlist_is_sorted(XKVDList_PT dlist, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(dlist);
    xassert(cmp);

    if (!dlist || !cmp) {
        return false;
    }

    {
        for (XRDList_PT step = dlist->head; step && step->next; /*nothing*/) {
            if (0 < cmp(xpair_first(step->value), xpair_first(step->next->value), cl)) {
                return false;
            }
            else {
                step = step->next;
            }
        }

        return true;
    }
}
