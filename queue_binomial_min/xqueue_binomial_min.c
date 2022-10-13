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
*       << Algorithms in C >> Third Edition : chapter 9.7
*/


#include <stddef.h>
#include <stdarg.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../utils/xutils.h"
#include "../include/xarith_int.h"
#include "../array_pointer/xarray_pointer_x.h"
#include "../queue_sequence/xqueue_sequence_x.h"
#include "../tree_binary/xtree_binary_x.h"
#include "xqueue_binomial_min_x.h"

XMinBinQue_PT xminbinque_new(int capacity, int(*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(0 <= capacity);
    xassert(cmp);

    if ((capacity < 0) || !cmp) {
        return NULL;
    }

    {
        XMinBinQue_PT queue = XMEM_CALLOC(1, sizeof(*queue));
        if (!queue) {
            return NULL;
        }

        queue->buckets = xpseq_new(XUTILS_UNLIMITED_BASED_ON_POWER_2);
        if (!queue->buckets) {
            XMEM_FREE(queue);
            return NULL;
        }
        
        queue->size = 0;
        queue->capacity = capacity;
        queue->discard_strategy = XUTILS_QUEUE_STRATEGY_DISCARD_NEW;
        queue->cmp = cmp;
        queue->cl = cl;

        return queue;
    }
}

static
XMinBinQue_PT xminbinque_copy_impl(XMinBinQue_PT queue, int elem_size, bool deep) {
    XMinBinQue_PT nqueue = XMEM_CALLOC(1, sizeof(*nqueue));
    if (!nqueue) {
        return NULL;
    }

    nqueue->buckets = xpseq_new(XUTILS_UNLIMITED_BASED_ON_POWER_2);
    if (!nqueue->buckets) {
        XMEM_FREE(nqueue);
        return NULL;
    }

    for (int i = 0; i < queue->buckets->size; ++i) {
        XBinTree_Node_PT node = xpseq_get_impl(queue->buckets, i);
        if (node) {
            XBinTree_PT tree = xbintree_new_with_root(node, xiarith_pow2(i));
            XBinTree_PT ntree = deep ? xbintree_deep_copy(tree, elem_size) : xbintree_copy(tree);
            if (!ntree) {
                if (tree) {
                    xbintree_reset(tree);
                    xbintree_free(&tree);
                }
                deep ? xminbinque_deep_free(&nqueue) : xminbinque_free(&nqueue);
                return NULL;
            }

            xpseq_put_impl(nqueue->buckets, i, ntree->root);

            xbintree_reset(tree);
            xbintree_free(&tree);
            xbintree_reset(ntree);
            xbintree_free(&ntree);
        }
    }

    nqueue->buckets->size = queue->buckets->size;

    nqueue->size = queue->size;
    nqueue->capacity = queue->capacity;
    nqueue->discard_strategy = queue->discard_strategy;
    nqueue->cmp = queue->cmp;
    nqueue->cl = queue->cl;

    return nqueue;
}

XMinBinQue_PT xminbinque_copy(XMinBinQue_PT queue) {
    return xminbinque_copy_impl(queue, 0, false);
}

XMinBinQue_PT xminbinque_deep_copy(XMinBinQue_PT queue, int elem_size) {
    return xminbinque_copy_impl(queue, elem_size, true);
}

int xminbinque_vload(XMinBinQue_PT queue, void *x, ...) {
    xassert(queue);

    if (!queue) {
        return 0;
    }

    {
        int count = queue->size;

        va_list ap;
        va_start(ap, x);
        for (; x; x = va_arg(ap, void *)) {
            if (!xminbinque_push_impl(queue, x, false)) {
                break;
            }
        }
        va_end(ap);

        return queue->size - count;
    }
}

int xminbinque_aload(XMinBinQue_PT queue, XPArray_PT xs) {
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

            if (!xminbinque_push_impl(queue, value, false)) {
                break;
            }
        }

        return queue->size - count;
    }
}

/* Merget two XBinTree_PT (X is bigger, T is smaller):
*  1. make X's left branch to be T's right branch
*  2. make T to be X's left branch
*
*        X         T                  X
*       / \       / \                / \
*      W         S        -->       T
*     / \       / \               /   \
*    P   M     R   N             S     W
*                               / \   / \
*                              R   N P   M
*/
static
XBinTree_Node_PT xminbinque_merge_two_binque_node(XMinBinQue_PT queue, XBinTree_Node_PT tree1, XBinTree_Node_PT tree2, bool maxp) {
    if ((maxp && (queue->cmp(tree1->data, tree2->data, queue->cl) < 0)) || (!maxp && (queue->cmp(tree2->data, tree1->data, queue->cl) < 0))) {
        tree1->right = tree2->left;
        tree2->left = tree1;

        return tree2;
    }
    else {
        tree2->right = tree1->left;
        tree1->left = tree2;

        return tree1;
    }
}

static 
void xminbinque_push_binque_node(XMinBinQue_PT queue, XBinTree_Node_PT node, int slot, bool maxp, bool increase_size) {
    if (!node) {
        return;
    }

    {
        /* find suitable position from index "slot" start */
        int i = slot;
        while (true) {
            /* current index has no value, save the node here */
            if (!xpseq_get_impl(queue->buckets, i)) {
                xpseq_put_impl(queue->buckets, i, (void*)node);

                /* set the top index of xpseq to be valid if needed */
                if (queue->buckets->size <= i) {
                    queue->buckets->size = i + 1;
                }

                break;
            }
            /* current index has value already, merge it with input node, and prepare for next slot saving or merging */
            else {
                node = xminbinque_merge_two_binque_node(queue, xpseq_get_impl(queue->buckets, i), node, maxp);
                /* clear current index */
                xpseq_put_impl(queue->buckets, i, NULL);
                ++i;
            }
        }
    }

    if (increase_size) {
        queue->size += xiarith_pow2(slot);
    }
}

bool xminbinque_push_impl(XMinBinQue_PT queue, void *x, bool maxp) {
    /* TODO : some logs here to notify the user the queue is full */
    if ((0 < queue->capacity) && (queue->capacity <= queue->size)) {
        if (queue->discard_strategy == XUTILS_QUEUE_STRATEGY_DISCARD_TOP) {
            if (!xminbinque_pop_impl(queue, maxp)) {
                return false;
            }
        }
        else {
            return false;
        }
    }

    {
        /* 1. create one new XBinTree_Node_PT */
        XBinTree_Node_PT nnode = xbintree_new_node(x);
        if (!nnode) {
            return false;
        }

        /* 2. find suitable position to insert/merge above nnode */
        xminbinque_push_binque_node(queue, nnode, 0, maxp, true);

        return true;
    }
}

bool xminbinque_push(XMinBinQue_PT queue, void *x) {
    xassert(queue);
    xassert(x);

    if (!queue || !x) {
        return false;
    }

    return xminbinque_push_impl(queue, x, false);
}

/* find the index of max/min value element in the first layer */
static
int xminbinque_peek_index(XMinBinQue_PT queue, bool maxp) {
    int top = -1;

    for (int i = 0; i < queue->buckets->size; ++i) {
        if (xpseq_get_impl(queue->buckets, i)) {
            if (top == -1) {
                top = i;
            }
            else {
                if (maxp) {
                    if (queue->cmp(xbintree_node_data((XBinTree_Node_PT)xpseq_get_impl(queue->buckets, top)), xbintree_node_data((XBinTree_Node_PT)xpseq_get_impl(queue->buckets, i)), queue->cl) < 0) {
                        top = i;
                    }
                }
                else {
                    if (queue->cmp(xbintree_node_data((XBinTree_Node_PT)xpseq_get_impl(queue->buckets, i)), xbintree_node_data((XBinTree_Node_PT)xpseq_get_impl(queue->buckets, top)), queue->cl) < 0) {
                        top = i;
                    }
                }
            }
        }
    }

    return top;
}

bool xminbinque_merge_impl(XMinBinQue_PT queue1, XMinBinQue_PT *pqueue2, bool maxp, bool increase_size) {
    for (int i = 0; i < (*pqueue2)->buckets->size; ++i) {
        xminbinque_push_binque_node(queue1, xpseq_get_impl((*pqueue2)->buckets, i), i, maxp, increase_size);
    }

    xpseq_clear((*pqueue2)->buckets);
    xminbinque_free(pqueue2);

    return true;
}

void* xminbinque_pop_impl(XMinBinQue_PT queue, bool maxp) {
    /* create one new binque to hold the XBinTree_Node_PT */
    XMinBinQue_PT nqueue = xminbinque_new(queue->capacity, queue->cmp, queue->cl);
    if (!nqueue) {
        return NULL;
    }

    {
        /* 1. find the index in buckets which contains the top data */
        int top = xminbinque_peek_index(queue, maxp);

        /*2. get the data */
        XBinTree_Node_PT node = (XBinTree_Node_PT)xpseq_get_impl(queue->buckets, top);
        void *data = node->data;

        /* clear the index top */
        xpseq_put_impl(queue->buckets, top, NULL);
        if (top == queue->buckets->size - 1) {
            queue->buckets->size = top;
        }

        /*3. merge the other XBinTree_Node_PT back */
        {
            /* free the top node at first */
            XBinTree_Node_PT tnode = node->left;
            XMEM_FREE(node);

            if (0 < top) {
                node = tnode;

                /* extract the XBinTree_Node_PT */
                for (int i = top - 1; 0 <= i; --i) {
                    tnode = node->right;
                    node->right = NULL;
                    xpseq_put_impl(nqueue->buckets, i, node);
                    node = tnode;
                }
                /* assign size to nqueue */
                nqueue->buckets->size = top;

                /* merge the nqueue back to original queue */
                xminbinque_merge_impl(queue, &nqueue, maxp, false);
            }
            else {
                xminbinque_free(&nqueue);
            }
        }

        --queue->size;

        return data;
    }
}

void* xminbinque_pop(XMinBinQue_PT queue) {
    xassert(queue);

    if (!queue || (queue->size == 0)) {
        return NULL;
    }

    return xminbinque_pop_impl(queue, false);
}

void* xminbinque_peek_impl(XMinBinQue_PT queue, bool maxp) {
    XBinTree_Node_PT node = xpseq_get_impl(queue->buckets, xminbinque_peek_index(queue, maxp));
    return node ? node->data : NULL;
}

void* xminbinque_peek(XMinBinQue_PT queue) {
    xassert(queue);

    if (!queue || (queue->size == 0)) {
        return NULL;
    }

    return xminbinque_peek_impl(queue, false);
}

bool xminbinque_merge(XMinBinQue_PT queue1, XMinBinQue_PT *pqueue2) {
    xassert(queue1);
    xassert(pqueue2);
    xassert(*pqueue2);

    if (!queue1 || !pqueue2 || !*pqueue2) {
        return false;
    }

    return xminbinque_merge_impl(queue1, pqueue2, false, true);
}

int xminbinque_map(XMinBinQue_PT queue, bool (*apply)(void *x, void *cl), void *cl) {
    xassert(queue);
    xassert(apply);

    if (!queue || !apply) {
        return 0;
    }

    {
        int count = 0;

        for (int i = 0; i < queue->buckets->size; ++i) {
            XBinTree_Node_PT node = xpseq_get_impl(queue->buckets, i);
            count += xbintree_map_inorder_impl(node, apply, cl);
        }

        return count;
    }
}

bool xminbinque_map_break_if_true(XMinBinQue_PT queue, bool (*apply)(void *x, void *cl), void *cl) {
    xassert(queue);
    xassert(apply);

    if (!queue || !apply) {
        return false;
    }

    {
        bool true_found = false;

        for (int i = 0; i < queue->buckets->size; ++i) {
            XBinTree_Node_PT node = xpseq_get_impl(queue->buckets, i);
            xbintree_map_inorder_break_if_true_impl(node, &true_found, apply, cl);
            if (true_found) {
                return true;
            }
        }

        return false;
    }
}

bool xminbinque_map_break_if_false(XMinBinQue_PT queue, bool (*apply)(void *x, void *cl), void *cl) {
    xassert(queue);
    xassert(apply);

    if (!queue || !apply) {
        return true;
    }

    {
        bool false_found = false;

        for (int i = 0; i < queue->buckets->size; ++i) {
            XBinTree_Node_PT node = xpseq_get_impl(queue->buckets, i);
            xbintree_map_inorder_break_if_false_impl(node, &false_found, apply, cl);
            if (false_found) {
                return true;
            }
        }

        return false;
    }
}

static
void xminbinque_free_datas_impl(XMinBinQue_PT queue, bool deep) {
    while (!xpseq_is_empty(queue->buckets)) {
        XBinTree_Node_PT node = xpseq_pop_back(queue->buckets);
        xbintree_free_node_impl(node, deep);
    }
}

static
void xminbinque_free_datas_impl_apply(XMinBinQue_PT queue, bool (*apply)(void **data, void *cl), void *cl) {
    while (!xpseq_is_empty(queue->buckets)) {
        XBinTree_Node_PT node = xpseq_pop_back(queue->buckets);
        xbintree_free_node_impl_apply(node, apply, cl);
    }
}

void xminbinque_free(XMinBinQue_PT *pqueue) {
    if (!pqueue || !*pqueue) {
        return;
    }

    xminbinque_free_datas_impl(*pqueue, false);

    // free layer 1 XPSeq_PT
    xpseq_free(&((*pqueue)->buckets));
    XMEM_FREE(*pqueue);
}

void xminbinque_deep_free(XMinBinQue_PT *pqueue) {
    if (!pqueue || !*pqueue) {
        return;
    }

    xminbinque_free_datas_impl(*pqueue, true);

    // free layer 1 XPSeq_PT
    xpseq_free(&((*pqueue)->buckets));
    XMEM_FREE(*pqueue);
}

void xminbinque_free_apply(XMinBinQue_PT *pqueue, bool (*apply)(void **data, void *cl), void *cl) {
    if (!pqueue || !*pqueue) {
        return;
    }

    xminbinque_free_datas_impl_apply(*pqueue, apply, cl);

    // free layer 1 XPSeq_PT
    xpseq_free(&((*pqueue)->buckets));
    XMEM_FREE(*pqueue);
}

void xminbinque_clear_impl(XMinBinQue_PT queue, bool deep) {
    xassert(queue);

    if (!queue || (queue->size == 0)) {
        return;
    }

    // free every layer 2 XBinTree_Node_PT
    xminbinque_free_datas_impl(queue, deep);
    queue->size = 0;
}

void xminbinque_clear(XMinBinQue_PT queue) {
    xminbinque_clear_impl(queue, false);
}

void xminbinque_deep_clear(XMinBinQue_PT queue) {
    xminbinque_clear_impl(queue, true);
}

void xminbinque_clear_apply(XMinBinQue_PT queue, bool (*apply)(void **data, void *cl), void *cl) {
    xassert(queue);

    if (!queue || (queue->size == 0)) {
        return;
    }

    // free every layer 2 XBinTree_Node_PT
    xminbinque_free_datas_impl_apply(queue, apply, cl);
    queue->size = 0;
}

int xminbinque_size(XMinBinQue_PT queue) {
    return (queue ? queue->size : 0);
}

bool xminbinque_is_empty(XMinBinQue_PT queue) {
    return (queue ? (queue->size == 0) : true);
}

bool xminbinque_swap(XMinBinQue_PT queue1, XMinBinQue_PT queue2) {
    xassert(queue1);
    xassert(queue2);

    if (!queue1 || !queue2) {
        return false;
    }

    {
        int size = queue1->size;
        int capacity = queue1->capacity;
        int strategy = queue1->discard_strategy;
        XPSeq_PT buckets = queue1->buckets;
        int(*cmp)(void *x, void *y, void *cl) = queue1->cmp;
        void *cl = queue1->cl;

        queue1->size = queue2->size;
        queue1->capacity = queue2->capacity;
        queue1->discard_strategy = queue2->discard_strategy;
        queue1->buckets = queue2->buckets;
        queue1->cmp = queue2->cmp;
        queue1->cl = queue2->cl;

        queue2->size = size;
        queue2->capacity = capacity;
        queue2->discard_strategy = strategy;
        queue2->buckets = buckets;
        queue2->cmp = cmp;
        queue2->cl = cl;
    }

    return true;
}

/* Note : make sure the capacity of queue is "M" which is the wanted limitation at first */
bool xminbinque_keep_max_values(XMinBinQue_PT queue, void *data, void **odata) {
    xassert(queue);
    xassert(data);
    xassert(queue->capacity != 0);

    if (!queue || !data || (queue->capacity == 0)) {
        return false;
    }

    /* no capacity*/
    if (queue->capacity <= queue->size) {
        /* delete the top maximum elements to save the small one */
        if (queue->cmp(data, xminbinque_peek(queue), queue->cl) < 0) {
            void *tmp = xminbinque_pop_impl(queue, false);
            if (!tmp) {
                return false;
            }

            if (odata) {
                *odata = tmp;
            }
        }
        else {
            /* ignore the bigger input */
            return true;
        }
    }

    return xminbinque_push_impl(queue, data, false);
}

static
bool xminbinque_set_strategy_drop_impl(XMinBinQue_PT queue, int strategy) {
    xassert(queue);
    xassert((0 == strategy) || ((3 == strategy)));

    if (!queue || ((strategy != 0) && (strategy != 3))) {
        return false;
    }

    queue->discard_strategy = strategy;

    return true;
}

bool xminbinque_set_strategy_discard_new(XMinBinQue_PT queue) {
    return xminbinque_set_strategy_drop_impl(queue, XUTILS_QUEUE_STRATEGY_DISCARD_NEW);
}

bool xminbinque_set_strategy_discard_top(XMinBinQue_PT queue) {
    return xminbinque_set_strategy_drop_impl(queue, XUTILS_QUEUE_STRATEGY_DISCARD_TOP);
}
