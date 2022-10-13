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
#include "../list_s/xlist_s_x.h"
#include "../include/xpair.h"
#include "../array_pointer/xarray_pointer_x.h"
#include "xlist_kvs_x.h"

XKVSList_PT xkvslist_new(void) {
    return xslist_new();
}

static
XRSList_PT xkvslist_new_node(void *key, void *value) {
    XPair_PT pair = xpair_new(key, value);
    if (!pair) {
        return NULL;
    }

    XRSList_PT node = xrslist_new(pair);
    if (!node) {
        xpair_free(&pair);
        return NULL;
    }

    return node;
}

int xkvslist_vload_repeat(XKVSList_PT slist, void *key, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, key);
    for (; key; key = va_arg(ap, void *)) {
        void* value = va_arg(ap, void *);
        if (!value) {
            break;
        }

        if (!xkvslist_push_back_repeat(slist, key, value)) {
            break;
        }

        ++count;
    }
    va_end(ap);

    return count;
}

int xkvslist_vload_unique(XKVSList_PT slist, void *key, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, key);
    for (; key; key = va_arg(ap, void *)) {
        void* value = va_arg(ap, void *);
        if (!value) {
            break;
        }

        int ret = xkvslist_push_back_unique(slist, key, value);
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

int xkvslist_vload_unique_if(XKVSList_PT slist, int(*cmp)(void *key1, void *key2, void *cl), void *cl, void *key, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, key);
    for (; key; key = va_arg(ap, void *)) {
        void* value = va_arg(ap, void *);
        if (!value) {
            break;
        }

        int ret = xkvslist_push_back_unique_if(slist, key, value, cmp, cl);
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

int xkvslist_vload_replace(XKVSList_PT slist, void *key, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, key);
    for (; key; key = va_arg(ap, void *)) {
        void* value = va_arg(ap, void *);
        if (!value) {
            break;
        }

        int ret = xkvslist_push_back_replace(slist, key, value);
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

int xkvslist_vload_replace_if(XKVSList_PT slist, int(*cmp)(void *key1, void *key2, void *cl), void *cl, void *key, ...) {
    int count = 0;

    va_list ap;
    va_start(ap, key);
    for (; key; key = va_arg(ap, void *)) {
        void* value = va_arg(ap, void *);
        if (!value) {
            break;
        }

        int ret = xkvslist_push_back_replace_if(slist, key, value, cmp, cl);
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
int xkvslist_aload_impl(XKVSList_PT slist, XPArray_PT xs, bool repeat,
                        bool  (*push_back_repeat) (XKVSList_PT slist, void *key, void *value),
                        int   (*push_back)        (XKVSList_PT slist, void *key, void *value),
                        int   (*push_back_if)     (XKVSList_PT slist, void *key, void *value, int(*cmp)(void *key1, void *key2, void *cl), void *cl),
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
                if (!push_back_repeat(slist, key, value)) {
                    break;
                }
                ++count;
            }
            else {
                int ret = cmp ? push_back_if(slist, key, value, cmp, cl): push_back(slist, key, value);
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

int xkvslist_aload_repeat(XKVSList_PT slist, XPArray_PT xs) {
    return xkvslist_aload_impl(slist, xs, true, xkvslist_push_back_repeat, NULL, NULL, NULL, NULL);
}

int xkvslist_aload_unique(XKVSList_PT slist, XPArray_PT xs) {
    return xkvslist_aload_impl(slist, xs, false, NULL, xkvslist_push_back_unique, NULL, NULL, NULL);
}

int xkvslist_aload_unique_if(XKVSList_PT slist, XPArray_PT xs, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(cmp);

    if (!cmp) {
        return -1;
    }

    return xkvslist_aload_impl(slist, xs, false, NULL, NULL, xkvslist_push_back_unique_if, cmp, cl);
}

int xkvslist_aload_replace(XKVSList_PT slist, XPArray_PT xs) {
    return xkvslist_aload_impl(slist, xs, false, NULL, xkvslist_push_back_replace, NULL, NULL, NULL);
}

int xkvslist_aload_replace_if(XKVSList_PT slist, XPArray_PT xs, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(cmp);

    if (!cmp) {
        return -1;
    }

    return xkvslist_aload_impl(slist, xs, false, NULL, NULL, xkvslist_push_back_replace_if, cmp, cl);
}

XKVSList_PT xkvslist_copyn_if_impl(XKVSList_PT slist, int key_size, int value_size, int count, bool deep, 
                                   int (*cmp)    (void *key1, void *key2, void *cl1), void *cl1,
                                   bool (*applyk) (void *key, void *cl2),
                                   bool (*applykv)(void *key, void *value, void *cl2),
                                   void *cl2) {
    xassert(slist);
    xassert(0 <= count);

    if (!slist || (count < 0)) {
        return NULL;
    }

    if (deep) {
        xassert(0 < key_size);
        if (key_size <= 0) {
            return NULL;
        }
    }

    if (slist->size < count) {
        count = slist->size;
    }

    {
        XKVSList_PT nslist = xkvslist_new();
        if (!nslist) {
            return NULL;
        }

        if (count == 0 || slist->size == 0) {
            return nslist;
        }

        {
            XRSList_PT *pp = &nslist->head, step = NULL;

            for (step = slist->head; step && (0 < count); step = step->next) {
                bool ret = cmp ? (cmp(xpair_first(step->value), cl2, cl1) == 0) : (applykv ? applykv(xpair_first(step->value), xpair_second(step->value), cl2) : (applyk ? applyk(xpair_first(step->value), cl2) : true));
                /* ignore the item if ret == false */
                if (!ret) {
                    continue;
                }

                XMEM_NEW0(*pp);

                if (!*pp) {
                    deep ? xkvslist_deep_free(&nslist) : xkvslist_free(&nslist);
                    return NULL;
                }

                if (deep) {
                    (*pp)->value = xpair_deep_copy(step->value, key_size, value_size);
                    if (!(*pp)->value) {
                        xkvslist_deep_free(&nslist);
                        return NULL;
                    }
                }
                else {
                    (*pp)->value = xpair_copy(step->value);
                    if (!(*pp)->value) {
                        xkvslist_free(&nslist);
                        return NULL;
                    }
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

XKVSList_PT xkvslist_copyn_equal_if(XKVSList_PT slist, int count, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(key);

    if (!key) {
        return NULL;
    }

    return xkvslist_copyn_if_impl(slist, 0, 0, count, false, cmp, cl, NULL, NULL, key);
}

XKVSList_PT xkvslist_copyn_apply_if(XKVSList_PT slist, int count, bool (*apply)(void *key, void *value, void *cl), void *cl) {
    return xkvslist_copyn_if_impl(slist, 0, 0, count, false, NULL, NULL, NULL, apply, cl);
}

XKVSList_PT xkvslist_copyn_apply_key_if(XKVSList_PT slist, int count, bool (*apply)(void *key, void *cl), void *cl) {
    return xkvslist_copyn_if_impl(slist, 0, 0, count, false, NULL, NULL, apply, NULL, cl);
}

XKVSList_PT xkvslist_copyn(XKVSList_PT slist, int count) {
    return xkvslist_copyn_apply_if(slist, count, NULL, NULL);
}

XKVSList_PT xkvslist_copy(XKVSList_PT slist) {
    return xkvslist_copyn(slist, (slist ? slist->size : 0));
}

XKVSList_PT xkvslist_copy_equal_if(XKVSList_PT slist, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvslist_copyn_equal_if(slist, (slist ? slist->size : 0), key, cmp, cl);
}

XKVSList_PT xkvslist_copy_apply_if(XKVSList_PT slist, bool (*apply)(void *key, void *value, void *cl), void *cl) {
    return xkvslist_copyn_apply_if(slist, (slist ? slist->size : 0), apply, cl);
}

XKVSList_PT xkvslist_copy_apply_key_if(XKVSList_PT slist, bool (*apply)(void *key, void *cl), void *cl) {
    return xkvslist_copyn_apply_key_if(slist, (slist ? slist->size : 0), apply, cl);
}

XKVSList_PT xkvslist_deep_copyn_equal_if(XKVSList_PT slist, int key_size, int value_size, int count, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(key);

    if (!key) {
        return NULL;
    }

    return xkvslist_copyn_if_impl(slist, key_size, value_size, count, true, cmp, cl, NULL, NULL, key);
}

XKVSList_PT xkvslist_deep_copyn_apply_if(XKVSList_PT slist, int key_size, int value_size, int count, bool (*apply)(void *key, void *value, void *cl), void *cl) {
    return xkvslist_copyn_if_impl(slist, key_size, value_size, count, true, NULL, NULL, NULL, apply, cl);
}

XKVSList_PT xkvslist_deep_copyn_apply_key_if(XKVSList_PT slist, int key_size, int value_size, int count, bool (*apply)(void *key, void *cl), void *cl) {
    return xkvslist_copyn_if_impl(slist, key_size, value_size, count, true, NULL, NULL, apply, NULL, cl);
}

XKVSList_PT xkvslist_deep_copyn(XKVSList_PT slist, int key_size, int value_size, int count) {
    return xkvslist_deep_copyn_apply_if(slist, key_size, value_size, count, NULL, NULL);
}

XKVSList_PT xkvslist_deep_copy(XKVSList_PT slist, int key_size, int value_size) {
    return xkvslist_deep_copyn(slist, key_size, value_size, (slist ? slist->size : 0));
}

XKVSList_PT xkvslist_deep_copy_equal_if(XKVSList_PT slist, int key_size, int value_size, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvslist_deep_copyn_equal_if(slist, key_size, value_size, (slist ? slist->size : 0), key, cmp, cl);
}

XKVSList_PT xkvslist_deep_copy_apply_if(XKVSList_PT slist, int key_size, int value_size, bool (*apply)(void *key, void *value, void *cl), void *cl) {
    return xkvslist_deep_copyn_apply_if(slist, key_size, value_size, (slist ? slist->size : 0), apply, cl);
}

XKVSList_PT xkvslist_deep_copy_apply_key_if(XKVSList_PT slist, int key_size, int value_size, bool (*apply)(void *key, void *cl), void *cl) {
    return xkvslist_deep_copyn_apply_key_if(slist, key_size, value_size, (slist ? slist->size : 0), apply, cl);
}

bool xkvslist_push_front_repeat(XKVSList_PT slist, void *key, void *value) {
    xassert(slist);
    xassert(key);

    if (!slist || !key) {
        return false;
    }

    {
        XRSList_PT p = xkvslist_new_node(key, value);
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

bool xkvslist_pop_front(XKVSList_PT slist, void **key, void **value) {
    xassert(slist);

    if (!slist || (slist->size == 0)) {
        return false;
    }

    {
        XRSList_PT p = slist->head;
        if (key) {
            *key = xpair_first(p->value);
        }
        if (value) {
            *value = xpair_second(p->value);
        }

        slist->head = p->next;
        if (!slist->head) {
            slist->tail = NULL;
        }

        xpair_free((XPair_PT*)&p->value);
        XMEM_FREE(p);

        --slist->size;
    }

    return true;
}

XPair_PT xkvslist_front(XKVSList_PT slist) {
    return slist ? (slist->head ? (XPair_PT)slist->head->value : NULL) : NULL;
}

XPair_PT xkvslist_back(XKVSList_PT slist) {
    return slist ? (slist->tail ? (XPair_PT)slist->tail->value : NULL) : NULL;
}

XRSList_PT xkvslist_front_node(XKVSList_PT slist) {
    return slist ? slist->head : NULL;
}

XRSList_PT xkvslist_back_node(XKVSList_PT slist) {
    return slist ? slist->tail : NULL;
}

bool xkvslist_push_back_repeat(XKVSList_PT slist, void *key, void *value) {
    xassert(slist);
    xassert(key);

    if (!slist || !key) {
        return false;
    }

    {
        XRSList_PT p = xkvslist_new_node(key, value);
        if (!p) {
            return false;
        }

        slist->tail ? (slist->tail->next = p) : (slist->head = p);
        slist->tail = p;

        ++slist->size;
    }

    return true;
}

int xkvslist_push_front_unique(XKVSList_PT slist, void *key, void *value) {
    if (!xkvslist_find(slist, key)) {
        return xkvslist_push_front_repeat(slist, key, value) ? 1 : -1;
    }

    return 0;
}

int xkvslist_push_front_unique_if(XKVSList_PT slist, void *key, void *value, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    if (!xkvslist_find_equal_if(slist, key, cmp, cl)) {
        return xkvslist_push_front_repeat(slist, key, value) ? 1 : -1;
    }

    return 0;
}

static
int xkvslist_replace_if_impl(XKVSList_PT slist, void *key, void *value, bool deep, bool push_replace(XKVSList_PT slist, void *key, void *value), int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(key);

    if (!key) {
        return -1;
    }

    {
        XRSList_PT node = xkvslist_find_equal_if(slist, key, cmp, cl);
        if (node) {
            if (deep) {
                if(key != xpair_first(node->value)) {
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

        return push_replace(slist, key, value) ? 1 : -1;
    }
}

int xkvslist_push_front_replace(XKVSList_PT slist, void *key, void *value) {
    return xkvslist_replace_if_impl(slist, key, value, false, xkvslist_push_front_repeat, NULL, NULL);
}

int xkvslist_push_front_deep_replace(XKVSList_PT slist, void *key, void *value) {
    return xkvslist_replace_if_impl(slist, key, value, true, xkvslist_push_front_repeat, NULL, NULL);
}

int xkvslist_push_front_replace_if(XKVSList_PT slist, void *key, void *value, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvslist_replace_if_impl(slist, key, value, false, xkvslist_push_front_repeat, cmp, cl);
}

int xkvslist_push_front_deep_replace_if(XKVSList_PT slist, void *key, void *value, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvslist_replace_if_impl(slist, key, value, true, xkvslist_push_front_repeat, cmp, cl);
}

int xkvslist_push_back_unique(XKVSList_PT slist, void *key, void *value) {
    if (!xkvslist_find(slist, key)) {
        return xkvslist_push_back_repeat(slist, key, value) ? 1 : -1;
    }

    return 0;
}

int xkvslist_push_back_unique_if(XKVSList_PT slist, void *key, void *value, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    if (!xkvslist_find_equal_if(slist, key, cmp, cl)) {
        return xkvslist_push_back_repeat(slist, key, value) ? 1 : -1;
    }

    return 0;
}

int xkvslist_push_back_replace(XKVSList_PT slist, void *key, void *value) {
    return xkvslist_replace_if_impl(slist, key, value, false, xkvslist_push_back_repeat, NULL, NULL);
}

int xkvslist_push_back_deep_replace(XKVSList_PT slist, void *key, void *value) {
    return xkvslist_replace_if_impl(slist, key, value, true, xkvslist_push_back_repeat, NULL, NULL);
}

int xkvslist_push_back_replace_if(XKVSList_PT slist, void *key, void *value, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvslist_replace_if_impl(slist, key, value, false, xkvslist_push_back_repeat, cmp, cl);
}

int xkvslist_push_back_deep_replace_if(XKVSList_PT slist, void *key, void *value, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvslist_replace_if_impl(slist, key, value, true, xkvslist_push_back_repeat, cmp, cl);
}

bool xkvslist_insert_after(XKVSList_PT slist, XRSList_PT node, void *key, void *value) {
    if (!node) {
        return xkvslist_push_back_repeat(slist, key, value);
    }

    xassert(slist);
    xassert(key);

    if (!slist || !key) {
        return false;
    }

    {
        XRSList_PT p = xkvslist_new_node(key, value);
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

bool xkvslist_remove_after(XKVSList_PT slist, XRSList_PT node, void **key, void **value) {
    xassert(slist);

    if (!slist || !node || (slist->size == 0)) {
        return false;
    }

    if (!(node->next)) {
        return false;
    }

    {
        XRSList_PT p = node->next;
        if (key) {
            *key = xpair_first(p->value);
        }
        if (value) {
            *value = xpair_second(p->value);
        }

        node->next = p->next;
        if (!node->next) {
            slist->tail = node;
        }

        xpair_free((XPair_PT*)&p->value);
        XMEM_FREE(p);

        --slist->size;
    }

    return true;
}

int xkvslist_remove_if_impl(XKVSList_PT slist, bool deep, bool break_first, 
                            int (*cmp)    (void *key1, void *key2, void *cl1), void *cl1,
                            bool (*applyk) (void *key, void *cl2), 
                            bool (*applykv)(void *key, void **value, void *cl2), 
                            void *cl2) {
    xassert(slist);

    if (!slist) {
        return -1;
    }

    {
        XRSList_PT step = NULL, np = NULL, prev = NULL;
        int count = 0;
        void *value = NULL;

        for (step = slist->head; step; step = np) {
            np = step->next;

            if (applykv) {
                value = xpair_second(step->value);
            }

            bool ret = cmp ? (cmp(xpair_first(step->value), cl2, cl1) == 0) : (applykv ? applykv(xpair_first(step->value), &value, cl2) : (applyk ? applyk(xpair_first(step->value), cl2) : (xpair_first(step->value) == cl2)));
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

            deep ? xpair_deep_free((XPair_PT*)&step->value) : xpair_free((XPair_PT*)&step->value);
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

bool xkvslist_remove(XKVSList_PT slist, void *key) {
    xassert(key);

    if (!key) {
        return false;
    }

    return (0 <= xkvslist_remove_if_impl(slist, false, true, NULL, NULL, NULL, NULL, key)) ? true : false;
}

int xkvslist_remove_all(XKVSList_PT slist, void *key) {
    xassert(key);

    if (!key) {
        return -1;
    }

    return xkvslist_remove_if_impl(slist, false, false, NULL, NULL, NULL, NULL, key);
}

bool xkvslist_deep_remove(XKVSList_PT slist, void **key) {
    xassert(key);
    xassert(*key);

    if (!key || !*key) {
        return false;
    }

    {
        int ret = xkvslist_remove_if_impl(slist, true, true, NULL, NULL, NULL, NULL, *key);
        if (0 < ret) {
            *key = NULL;
        }

        return 0 <= ret ? true : false;
    }
}

int xkvslist_remove_apply_if(XKVSList_PT slist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return xkvslist_remove_if_impl(slist, false, false, NULL, NULL, NULL, apply, cl);
}

int xkvslist_remove_apply_key_if(XKVSList_PT slist, bool (*apply)(void *key, void *cl), void *cl) {
    return xkvslist_remove_if_impl(slist, false, false, NULL, NULL, apply, NULL, cl);
}

int xkvslist_remove_equal_if(XKVSList_PT slist, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvslist_remove_if_impl(slist, false, false, cmp, cl, NULL, NULL, key);
}

bool xkvslist_remove_apply_break_if(XKVSList_PT slist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return (0 <= xkvslist_remove_if_impl(slist, false, true, NULL, NULL, NULL, apply, cl));
}

bool xkvslist_remove_apply_key_break_if(XKVSList_PT slist, bool (*apply)(void *key, void *cl), void *cl) {
    return (0 <= xkvslist_remove_if_impl(slist, false, true, NULL, NULL, apply, NULL, cl));
}

bool xkvslist_remove_equal_break_if(XKVSList_PT slist, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return (0 <= xkvslist_remove_if_impl(slist, false, true, cmp, cl, NULL, NULL, key));
}

int xkvslist_deep_remove_apply_if(XKVSList_PT slist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return xkvslist_remove_if_impl(slist, true, false, NULL, NULL, NULL, apply, cl);
}

int xkvslist_deep_remove_apply_key_if(XKVSList_PT slist, bool (*apply)(void *key, void *cl), void *cl) {
    return xkvslist_remove_if_impl(slist, true, false, NULL, NULL, apply, NULL, cl);
}

int xkvslist_deep_remove_equal_if(XKVSList_PT slist, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvslist_remove_if_impl(slist, true, false, cmp, cl, NULL, NULL, key);
}

bool xkvslist_deep_remove_apply_break_if(XKVSList_PT slist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return (0<= xkvslist_remove_if_impl(slist, true, true, NULL, NULL, NULL, apply, cl));
}

bool xkvslist_deep_remove_apply_key_break_if(XKVSList_PT slist, bool (*apply)(void *key, void *cl), void *cl) {
    return (0 <= xkvslist_remove_if_impl(slist, true, true, NULL, NULL, apply, NULL, cl));
}

bool xkvslist_deep_remove_equal_break_if(XKVSList_PT slist, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return (0 <= xkvslist_remove_if_impl(slist, true, true, cmp, cl, NULL, NULL, key));
}

static
void xkvslist_free_impl(XKVSList_PT slist, bool deep, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!slist) {
        return;
    }

    {
        XRSList_PT step = NULL, np = NULL;

        for (step = slist->head; step; step = np) {
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

    return;
}

void xkvslist_free(XKVSList_PT *pslist) {
    if (!pslist) {
        return;
    }

    xkvslist_free_impl(*pslist, false, NULL, NULL);
    XMEM_FREE(*pslist);
}

void xkvslist_free_apply(XKVSList_PT *pslist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!pslist) {
        return;
    }

    xkvslist_free_impl(*pslist, false, apply, cl);
    XMEM_FREE(*pslist);
}

void xkvslist_deep_free(XKVSList_PT *pslist) {
    if (!pslist) {
        return;
    }

    xkvslist_free_impl(*pslist, true, NULL, NULL);
    XMEM_FREE(*pslist);
}

static
void xkvslist_clear_impl(XKVSList_PT slist, bool deep, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    if (!slist) {
        return;
    }

    xkvslist_free_impl(slist, deep, apply, cl);
    slist->head = NULL;
    slist->tail = NULL;
    slist->size = 0;
}

void xkvslist_clear(XKVSList_PT slist) {
    xkvslist_clear_impl(slist, false, NULL, NULL);
}

void xkvslist_clear_apply(XKVSList_PT slist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    xkvslist_clear_impl(slist, false, apply, cl);
}

void xkvslist_deep_clear(XKVSList_PT slist) {
    xkvslist_clear_impl(slist, true, NULL, NULL);
}

bool xkvslist_swap(XKVSList_PT slist1, XKVSList_PT slist2) {
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
void xkvslist_merge_impl(XKVSList_PT slist1, XKVSList_PT slist2) {
    if (slist2->head) {
        slist1->head ? (slist1->tail->next = slist2->head) : (slist1->head = slist2->head);
        slist1->tail = slist2->tail;
        slist1->size += slist2->size;
    }
}

int xkvslist_merge(XKVSList_PT slist1, XKVSList_PT *pslist2) {
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

        xkvslist_merge_impl(slist1, (*pslist2));
        XMEM_FREE(*pslist2);

        return count;
    }
}

void xkvslist_reverse(XKVSList_PT slist) {
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
int xkvslist_map_impl(XKVSList_PT slist, bool break_first, bool break_true, XRSList_PT *node, 
                      int (*equalk)  (void *key1, void *key2, void *cl1), void *cl1,
                      bool (*applyk)  (void *key, void *cl2), 
                      bool (*applykv) (void *key, void **value, void *cl2), 
                      void *cl2) {
    xassert(slist);

    if (!slist) {
        return -1;
    }

    {
        int count = 0;

        for (XRSList_PT step = slist->head; step; step = step->next) {
            bool ret = false;
            if (equalk) {
                ret = equalk(xpair_first(step->value), cl2, cl1) == 0;
            }
            else {
                void *value = xpair_second(step->value);
                ret = applykv ? applykv(xpair_first(step->value), &value, cl2) : (applyk ? applyk(xpair_first(step->value), cl2) : (xpair_first(step->value) == cl2));
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

int xkvslist_map(XKVSList_PT slist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return xkvslist_map_impl(slist, false, false, NULL, NULL, NULL, NULL, apply, cl);
}

bool xkvslist_map_break_if_true(XKVSList_PT slist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return (0 < xkvslist_map_impl(slist, true, true, NULL, NULL, NULL, NULL, apply, cl));
}

bool xkvslist_map_break_if_false(XKVSList_PT slist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    return (0 < xkvslist_map_impl(slist, true, false, NULL, NULL, NULL, NULL, apply, cl));
}

int xkvslist_map_key(XKVSList_PT slist, bool (*apply)(void *key, void *cl), void *cl) {
    return xkvslist_map_impl(slist, false, false, NULL, NULL, NULL, apply, NULL, cl);
}

bool xkvslist_map_key_break_if_true(XKVSList_PT slist, bool (*apply)(void *key, void *cl), void *cl) {
    return (0 < xkvslist_map_impl(slist, true, true, NULL, NULL, NULL, apply, NULL, cl));
}

bool xkvslist_map_key_break_if_false(XKVSList_PT slist, bool (*apply)(void *key, void *cl), void *cl) {
    return (0 < xkvslist_map_impl(slist, true, false, NULL, NULL, NULL, apply, NULL, cl));
}

XRSList_PT xkvslist_find(XKVSList_PT slist, void *key) {
    XRSList_PT node = NULL;
    xkvslist_map_impl(slist, true, true, &node, NULL, NULL, NULL, NULL, key);
    return node;
}

XRSList_PT xkvslist_find_equal_if(XKVSList_PT slist, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    XRSList_PT node = NULL;
    xkvslist_map_impl(slist, true, true, &node, cmp, cl, NULL, NULL, key);
    return node;
}

XRSList_PT xkvslist_find_apply_if(XKVSList_PT slist, bool (*apply)(void *key, void **value, void *cl), void *cl) {
    XRSList_PT node = NULL;
    xkvslist_map_impl(slist, true, true, &node, NULL, NULL, NULL, apply, cl);
    return node;
}

XRSList_PT xkvslist_find_apply_key_if(XKVSList_PT slist, bool (*apply)(void *key, void *cl), void *cl) {
    XRSList_PT node = NULL;
    xkvslist_map_impl(slist, true, true, &node, NULL, NULL, apply, NULL, cl);
    return node;
}

XPArray_PT xkvslist_to_array(XKVSList_PT slist) {
    xassert(slist);

    if (!slist) {
        return NULL;
    }

    {
        XPArray_PT array = xparray_new(slist->size * 2);
        if (!array) {
            return NULL;
        }

        {
            XRSList_PT step = slist->head;
            for (int i = 0; step; i += 2, step = step->next) {
                xparray_put_impl(array, i, xpair_first(step->value));
                xparray_put_impl(array, i + 1, xpair_second(step->value));
            }
        }

        return array;
    }
}

XPArray_PT xkvslist_keys_to_array(XKVSList_PT slist) {
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
                xparray_put_impl(array, i, xpair_first(step->value));
            }
        }

        return array;
    }
}

XPArray_PT xkvslist_values_to_array(XKVSList_PT slist) {
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
                xparray_put_impl(array, i, xpair_second(step->value));
            }
        }

        return array;
    }
}

int xkvslist_size(XKVSList_PT slist) {
    return (slist ? slist->size : 0);
}

bool xkvslist_is_empty(XKVSList_PT slist) {
    return (slist ? (slist->size == 0) : true);
}

static 
bool xkvslist_unique_impl(XKVSList_PT slist, bool deep, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    if (xkvslist_sort(slist, cmp, cl)) {
        for (XRSList_PT step = slist->head; step && step->next; /*nothing here*/) {
            if (0 == cmp(xpair_first(step->value), xpair_first(step->next->value), cl)) {
                void *key = NULL;
                void *value = NULL;

                if (!xkvslist_remove_after(slist, step, &key, &value)) {
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

bool xkvslist_unique(XKVSList_PT slist, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvslist_unique_impl(slist, false, cmp, cl);
}

bool xkvslist_deep_unique(XKVSList_PT slist, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    return xkvslist_unique_impl(slist, true, cmp, cl);
}

static
XRSList_PT xkvslist_merge_sort_impl_merge(XRSList_PT slist1, XRSList_PT slist2, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    struct XRSList head = { NULL, NULL };
    XRSList_PT *nslist = &head.next;
    while (slist1 && slist2) {
        if (cmp(xpair_first(slist1->value), xpair_first(slist2->value), cl) <= 0) {
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
XRSList_PT xkvslist_merge_sort_impl(XRSList_PT slist, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
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
            left = xkvslist_merge_sort_impl(left, cmp, cl);
            right = xkvslist_merge_sort_impl(right, cmp, cl);
            return xkvslist_merge_sort_impl_merge(left, right, cmp, cl);
        }
    }
}

bool xkvslist_sort(XKVSList_PT slist, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(slist);
    xassert(cmp);

    if (!slist || !cmp) {
        return false;
    }

    if (slist->size <= 1) {
        return true;
    }

    slist->head = xkvslist_merge_sort_impl(slist->head, cmp, cl);

    /* get the right pointer of slist->tail */
    XRSList_PT step = slist->head;
    while (step && step->next) {
        step = step->next;
    }
    slist->tail = step;

    xassert(xkvslist_is_sorted(slist, cmp, cl));

    return true;
}

bool xkvslist_is_sorted(XKVSList_PT slist, int(*cmp)(void *key1, void *key2, void *cl), void *cl) {
    xassert(slist);
    xassert(cmp);

    if (!slist || !cmp) {
        return false;
    }

    {
        for (XRSList_PT step = slist->head; step && step->next; /*nothing here*/) {
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
