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

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../list_s/xlist_s_x.h"
#include "../list_s_raw/xlist_s_raw_x.h"
#include "../include/xtree_redblack_list.h"
#include "../include/xtree_set.h"
#include "xtree_set_x.h"

XSet_PT xset_new(int (*cmp)(void *elem1, void *elem2, void *cl), void *cl) {
    return xlistrbtree_new(cmp, cl);
}

XSet_PT xset_copy(XSet_PT set) {
    return xlistrbtree_copy(set);
}

static
XSet_Node_PT xset_deep_copy_node(XSet_Node_PT node, XSet_Node_PT nparent, bool *false_found, void *cl) {
    XListRBTree_3Paras_PT paras = (XListRBTree_3Paras_PT)cl;

    XSet_Node_PT nnode = XMEM_CALLOC(1, sizeof(*nnode));
    if (!nnode) {
        *false_found = true;
        return NULL;
    }

    /* should not deep copy the key since it's saved in values too */
    nnode->key = node->key;

    /* only need to deep copy the values since all keys saved in it */
    nnode->values = xrslist_deep_copy(node->values, *((int*)paras->para1));
    if (node->values && !nnode->values) {
        *false_found = true;
        XMEM_FREE(nnode);
        return NULL;
    }

    nnode->node_size = node->node_size;
    nnode->size = node->size;
    nnode->color = node->color;

    nnode->parent = nparent;
    //nnode->left = NULL;
    //nnode->right = NULL;

    return nnode;
}

XSet_PT xset_deep_copy(XSet_PT set, int elem_size) {
    xassert(set);
    xassert(0 < elem_size);

    if (!set || (elem_size <= 0)) {
        return NULL;
    }

    {
        XSet_PT nset = xset_new(set->cmp, set->cl);
        if (!nset) {
            return NULL;
        }

        {
            XListRBTree_3Paras_T paras = { nset, (void*)&elem_size, NULL, NULL };

            bool false_found = false;
            xlistrbtree_copy_break_if_false_impl(set, set->root, nset, NULL, true, false, &false_found, xset_deep_copy_node, (void*)&paras);
            if (false_found) {
                xset_deep_free(&nset);
                return NULL;
            }
        }

        xassert(xset_size(set) == xset_size(nset));

        return nset;
    }
}

static
void xset_free_impl(XSet_PT tree, XSet_Node_PT node, bool deep, bool (*apply)(void *elem, void *cl), void *cl) {
    if (!node) {
        return;
    }

    xset_free_impl(tree, node->left, deep, apply, cl);
    xset_free_impl(tree, node->right, deep, apply, cl);

    /* should not deep free the keys, but only the values */
    deep ? xrslist_deep_free(&node->values) : (apply ? xrslist_free_apply(&node->values, apply, cl) : xrslist_free(&node->values));
    XMEM_FREE(node);
}

void xset_free(XSet_PT *pset) {
    if (!pset || !*pset) {
        return;
    }

    xset_free_impl(*pset, (*pset)->root, false, NULL, NULL);
    XMEM_FREE(*pset);
}

void xset_free_apply(XSet_PT *pset, bool (*apply)(void *elem, void *cl), void *cl) {
    if (!pset || !*pset) {
        return;
    }

    xset_free_impl(*pset, (*pset)->root, false, apply, cl);
    XMEM_FREE(*pset);
}

void xset_deep_free(XSet_PT *pset) {
    if (!pset || !*pset) {
        return;
    }

    xset_free_impl(*pset, (*pset)->root, true, NULL, NULL);
    XMEM_FREE(*pset);
}

void xset_clear(XSet_PT set) {
    xset_free_impl(set, (set ? set->root : NULL), false, NULL, NULL);
    if (set) {
        set->root = NULL;
    }
}

void xset_clear_apply(XSet_PT set, bool (*apply)(void *elem, void *cl), void *cl) {
    xset_free_impl(set, (set ? set->root : NULL), false, apply, cl);
    if (set) {
        set->root = NULL;
    }
}

void xset_deep_clear(XSet_PT set) {
    xset_free_impl(set, (set ? set->root : NULL), true, NULL, NULL);
    if (set) {
        set->root = NULL;
    }
}

bool xset_find(XSet_PT set, void *elem) {
    return xlistrbtree_find(set, elem);
}

void* xset_get(XSet_PT set, void *elem) {
    return xlistrbtree_get(set, elem);
}

bool xset_put_repeat(XSet_PT set, void *elem) {
    return xlistrbtree_put_repeat(set, elem, elem); /* key must be saved as value too */
}

bool xset_put_unique(XSet_PT set, void *elem) {
    return xlistrbtree_put_unique(set, elem, elem); /* key must be saved as value too */
}

void xset_elem_unique(XSet_PT set, void *elem) {
    xlistrbtree_key_unique(set, elem);
}

void xset_unique(XSet_PT set) {
    xlistrbtree_unique(set);
}

void xset_unique_except(XSet_PT set, void *elem) {
    xlistrbtree_unique_except(set, elem);
}

XSet_PT xset_unique_except_return_uniqued(XSet_PT set, void *elem) {
    return xlistrbtree_unique_except_return_uniqued(set, elem);
}

XSet_PT xset_unique_return_uniqued(XSet_PT set) {
    return xlistrbtree_unique_return_uniqued(set);
}

bool xset_has_repeat_elems_except(XSet_PT set, void *elem) {
    return xlistrbtree_has_repeat_keys_except(set, elem);
}

int xset_repeat_elems_size_except(XSet_PT set, void *elem) {
    return xlistrbtree_repeat_keys_size_except(set, elem);
}

void* xset_select(XSet_PT set, int k) {
    return xlistrbtree_select(set, k);
}

XSList_PT xset_elems_impl(XSet_PT tree, void *low, void *high) {
    XSList_PT list = xslist_new();
    if (!list) {
        return NULL;
    }

    if (0 < tree->cmp(low, high, tree->cl)) {
        void *tmp = low;
        low = high;
        high = tmp;
    }

    {
        XSet_Node_PT node = xlistrbtree_get_impl(tree, tree->root, low);
        while (node && (tree->cmp(node->key, high, tree->cl) <= 0)) {
            /* keys are all saved in the values */
            XRSList_PT rslist = xrslist_copy(node->values);
            if (!rslist) {
                xslist_free(&list);
                return NULL;
            }
            xslist_push_back_rslist(list, rslist);

            node = xlistrbtree_next_node(tree, node);
        }
    }

    return list;
}

XSList_PT xset_elems(XSet_PT tree, void *low, void *high) {
    xassert(tree);
    xassert(low);
    xassert(high);

    if (!tree || !low || !high) {
        return NULL;
    }

    return xset_elems_impl(tree, low, high);
}

void xset_remove(XSet_PT set, void *elem) {
    xlistrbtree_remove(set, elem);
}

void xset_remove_all(XSet_PT set, void *elem) {
    xlistrbtree_remove_all(set, elem);
}

void xset_deep_remove(XSet_PT set, void *elem) {
    xlistrbtree_deep_remove_impl(set, elem, false, true);
}

void xset_deep_remove_all(XSet_PT set, void *elem) {
    xlistrbtree_deep_remove_impl(set, elem, true, true);
}

int xset_map_min_to_max_impl(XSet_PT set, bool (*apply)(void *elem, void *cl), void *cl) {
    int count = 0;

    XSet_Node_PT node = xlistrbtree_min_impl(set, set ? set->root : NULL);
    while (node) {
        for (XRSList_PT step = node->values; step; step = step->next) {
            if (apply(step->value, cl)) {
                ++count;
            }
        }

        node = xlistrbtree_next_node(set, node);
    }

    return count;
}

int xset_map(XSet_PT set, bool (*apply)(void *elem, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xset_map_min_to_max_impl(set, apply, cl);
}

int xset_map_min_to_max(XSet_PT set, bool (*apply)(void *elem, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xset_map_min_to_max_impl(set, apply, cl);
}

bool xset_map_min_to_max_break_if_impl(XSet_PT set, bool break_true, bool (*apply)(void *elem, void *cl), void *cl) {
    XSet_Node_PT node = xlistrbtree_min_impl(set, set ? set->root : NULL);
    while (node) {
        for (XRSList_PT step = node->values; step; step = step->next) {
            bool ret = apply(step->value, cl);
            if (ret && break_true) {
                return true;
            }
            if (!ret && !break_true) {
                return true;
            }
        }

        node = xlistrbtree_next_node(set, node);
    }

    return false;
}

bool xset_map_break_if_true(XSet_PT set, bool (*apply)(void *elem, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return false;
    }

    return xset_map_min_to_max_break_if_impl(set, true, apply, cl);
}

bool xset_map_break_if_false(XSet_PT set, bool (*apply)(void *elem, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return true;
    }

    return xset_map_min_to_max_break_if_impl(set, false, apply, cl);
}

int xset_map_max_to_min_impl(XSet_PT set, bool (*apply)(void *elem, void *cl), void *cl) {
    int count = 0;

    XSet_Node_PT node = xlistrbtree_max_impl(set, set ? set->root : NULL);
    while (node) {
        for (XRSList_PT step = node->values; step; step = step->next) {
            if (apply(step->value, cl)) {
                ++count;
            }
        }

        node = xlistrbtree_prev_node(set, node);
    }

    return count;
}

int xset_map_max_to_min(XSet_PT set, bool (*apply)(void *elem, void *cl), void *cl) {
    xassert(apply);

    if (!apply) {
        return 0;
    }

    return xset_map_max_to_min_impl(set, apply, cl);
}

bool xset_swap(XSet_PT set1, XSet_PT set2) {
    return xlistrbtree_swap(set1, set2);
}

bool xset_merge_repeat(XSet_PT set1, XSet_PT *pset2) {
    return xlistrbtree_merge_repeat(set1, pset2);
}

bool xset_merge_unique(XSet_PT set1, XSet_PT *pset2) {
    return xlistrbtree_merge_unique(set1, pset2);
}

bool xset_copy_merge_repeat(XSet_PT set1, XSet_PT pset2) {
    XSet_PT nset2 = xset_copy(pset2);
    if (!nset2) {
        return false;
    }

    if (!xlistrbtree_merge_repeat(set1, &nset2)) {
        xset_free(&nset2);
        return false;
    }

    return true;
}

bool xset_copy_merge_unique(XSet_PT set1, XSet_PT pset2) {
    XSet_PT nset2 = xset_copy(pset2);
    if (!nset2) {
        return false;
    }

    if (!xlistrbtree_merge_unique(set1, &nset2)) {
        xset_free(&nset2);
        return false;
    }

    return true;
}

int xset_elem_size(XSet_PT set, void *elem) {
    xassert(set);
    xassert(elem);

    if (!set || !elem) {
        return 0;
    }

    {
        XSet_Node_PT node = xlistrbtree_get_impl(set, set->root, elem);
        if (!node) {
            return 0;
        }

        return node->node_size;
    }
}

int xset_elems_size_impl(XSet_PT tree, void *low, void *high) {
    int count = 0;

    if (0 < tree->cmp(low, high, tree->cl)) {
        void *tmp = low;
        low = high;
        high = tmp;
    }

    {
        XSet_Node_PT node = xlistrbtree_get_impl(tree, tree->root, low);
        while (node && (tree->cmp(node->key, high, tree->cl) <= 0)) {
            count += node->node_size;
            node = xlistrbtree_next_node(tree, node);
        }
    }

    return count;
}

int xset_elems_size(XSet_PT set, void *low, void *high) {
    xassert(set);
    xassert(low);
    xassert(high);

    if (!set || !low || !high) {
        return 0;
    }

    return xset_elems_size_impl(set, low, high);
}

int xset_size(XSet_PT set) {
    return xlistrbtree_size(set);
}

bool xset_is_empty(XSet_PT set) {
    return xlistrbtree_is_empty(set);
}

#if 0
XHashSet_PT xhashset_union(XHashSet_PT s, XHashSet_PT t) {
    xassert(s && t);

    if (!s) {
        if (!t)
            return NULL;
        return xhashset_copy(t);
    }
    else if (!t)
        return xhashset_copy(s);
    else {
        if (!(s->cmp == t->cmp && s->hash == t->hash))
            return NULL;

        {
            XHashSet_PT set1 = xhashset_copy(s);
            if (!set1) {
                return NULL;
            }

            XHashSet_PT set2 = xhashset_copy(t);
            if (!set2) {
                xhashset_free(&set1);
            }

            if (0 <= xhashtab_merge_unique(set1, &set2)) {
                return set1;
            }

            return NULL;
        }
    }
}

XHashSet_PT xhashset_inter(XHashSet_PT s, XHashSet_PT t) {
    xassert(s && t);

    if (!s) {
        if (!t)
            return NULL;
        return xhashset_new(t->size, t->cmp, t->hash);
    }
    else if (!t)
        return xhashset_new(s->size, s->cmp, s->hash);
    else if (s->size < t->size)
        return xhashset_inter(t, s);
    else {
        if (!(s->cmp == t->cmp && s->hash == t->hash))
            return NULL;

        {
            XHashSet_PT set = xhashset_new(arith_int_min(s->size, t->size), s->cmp, s->hash);

            {
                int i;
                XSList_Node_PT q = NULL;

                for (i = 0; i < t->size; i++)
                    for (q = t->buckets[i]->head; q; q = q->next)
                        if (xhashset_find(s, q->data)) {
                            XSList_Node_PT p = NULL;
                            void *data = q->data;
                            int i = (*set->hash)(data) % set->size;

                            XMEM_NEW(p);
                            p->data = data;
                            p->next = set->buckets[i];
                            set->buckets[i] = p;

                            set->size++;
                        }
            }

            return set;
        }
    }
}

XHashSet_PT xhashset_minus(XHashSet_PT s, XHashSet_PT t) {
    xassert(s && t);

    if (!s) {
        if (!t)
            return NULL;
        return xhashset_new(t->size, t->cmp, t->hash);
    }
    else if (!t)
        return xhashset_copy(s);
    else {
        if (!(t->cmp == s->cmp && t->hash == s->hash))
            return NULL;

        {
            XHashSet_PT set = xhashset_new(arith_int_min(t->size, s->size), t->cmp, t->hash);

            {
                int i;
                XSList_PT q = NULL;

                for (i = 0; i < s->size; i++)
                    for (q = s->buckets[i]; q; q = q->next)
                        if (!xhashset_find(t, q->data)) {
                            XSList_PT p = NULL;
                            void *data = q->data;
                            int i = (*set->hash)(data) % set->size;

                            XMEM_NEW(p);
                            p->data = data;
                            p->next = set->buckets[i];
                            set->buckets[i] = p;

                            set->size++;
                        }
            }

            return set;
        }
    }
}

bool xhashset_subset(XHashSet_PT s, XHashSet_PT t) {
    xassert(s && t);

    if (!s) {
        if (!t)
            return true;
        return false;
    }
    else if (!t)
        return true;
    else {
        if (!(t->cmp == s->cmp && t->hash == s->hash))
            return false;

        if (s->size < t->size)
            return false;

        {
            int i;
            XSList_PT q = NULL;

            for (i = 0; i < t->size; i++)
                for (q = t->buckets[i]; q; q = q->next)
                    if (!xhashset_find(s, q->data)) {
                        return false;
                    }
        }

        return true;
    }
}

bool xhashset_equal(XHashSet_PT s, XHashSet_PT t) {
    xassert(s && t);

    if (xhashset_subset(s, t) && xhashset_subset(t, s))
        return true;

    return false;
}

XHashSet_PT xhashset_diff(XHashSet_PT s, XHashSet_PT t) {
    xassert(s && t);

    if (!s) {
        if (!t)
            return NULL;
        return xhashset_copy(t);
    }
    else if (!t)
        return xhashset_copy(s);
    else {
        if (!(s->cmp == t->cmp && s->hash == t->hash))
            return NULL;

        {
            XHashSet_PT set = xhashset_new(arith_int_min(s->size, t->size), s->cmp, s->hash);

            {
                int i;
                XSList_PT q = NULL;

                for (i = 0; i < t->size; i++)
                    for (q = t->buckets[i]; q; q = q->next)
                        if (!xhashset_find(s, q->data)) {
                            XSList_PT p = NULL;
                            void *data = q->data;
                            int i = (*set->hash)(data) % set->size;

                            XMEM_NEW(p);
                            p->data = data;
                            p->next = set->buckets[i];
                            set->buckets[i] = p;

                            set->size++;
                        }
            }

            { XHashSet_PT u = t; t = s; s = u; }

            {
                int i;
                XSList_PT q = NULL;

                for (i = 0; i < t->size; i++)
                    for (q = t->buckets[i]; q; q = q->next)
                        if (!xhashset_find(s, q->data)) {
                            XSList_PT p = NULL;
                            void *member = q->data;
                            int i = (*set->hash)(member) % set->size;

                            XMEM_NEW(p);
                            p->data = member;
                            p->next = set->buckets[i];
                            set->buckets[i] = p;

                            set->size++;
                        }
            }

            return set;
        }
    }
}
#endif
