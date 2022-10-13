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
*       <<Introduction to Algorithms>> Third Edition. chapter 19
*/

#include <stddef.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../utils/xutils.h"
#include "../array_pointer/xarray_pointer_x.h"
#include "xheap_fibonacci_x.h"

XFibHeap_PT xfibheap_new(int (*cmp)(void *x, void *y, void *cl), void *cl) {
    xassert(cmp);

    if (!cmp) {
        return NULL;
    }

    {
        XFibHeap_PT heap = XMEM_CALLOC(1, sizeof(*heap));
        if (!heap) {
            return NULL;
        }

        //heap->root = NULL;
        //heap->size = 0;

        heap->cmp = cmp;
        heap->cl = cl;

        return heap;
    }
}

static 
XFibHeap_Node_PT xfibheap_new_node(void *value) {
    XFibHeap_Node_PT node = XMEM_CALLOC(1, sizeof(*node));
    if (!node) {
        return NULL;
    }

    //node->parent = NULL;
    //node->child = NULL;
    node->prev = node;
    node->next = node;

    node->value = value;
    //node->mark = false;
    //node->degree = 0;

    return node;
}

static inline
void xfibheap_insert_node(XFibHeap_Node_PT node, XFibHeap_Node_PT new_node) {
    new_node->prev = node;
    new_node->next = node->next;

    node->next->prev = new_node;
    node->next = new_node;
}

static inline
void xfibheap_remove_node(XFibHeap_Node_PT node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;

    node->prev = NULL;
    node->next = NULL;
}

bool xfibheap_push(XFibHeap_PT heap, void *data) {
    xassert(heap);
    xassert(data);

    if (!heap || !data) {
        return false;
    }

    {
        XFibHeap_Node_PT node = xfibheap_new_node(data);
        if (!node) {
            return false;
        }

        if (heap->root) {
            xfibheap_insert_node(heap->root, node);
            if (heap->cmp(data, heap->root->value, heap->cl) < 0) {
                heap->root = node;
            }
        }
        else {
            heap->root = node;
        }

        ++heap->size;
        return true;
    }
}

static 
void xfibheap_swap_node_address(XFibHeap_Node_PT *node1, XFibHeap_Node_PT *node2) {
    XFibHeap_Node_PT tmp = *node1;
    *node1 = *node2;
    *node2 = tmp;
}

static 
void xfibheap_link(XFibHeap_Node_PT parent, XFibHeap_Node_PT child) {
    if (parent->child) {
        xfibheap_insert_node(parent->child, child);
    }
    else {
        parent->child = child;
        child->prev = child;
        child->next = child;
    }
    parent->degree += 1;

    child->parent = parent;
    child->mark = false;
}

static 
void xfibheap_consolidate(XFibHeap_PT heap, XPArray_PT array) {
    XFibHeap_Node_PT x = NULL;
    XFibHeap_Node_PT y = NULL;

    while(heap->root) {
        x = heap->root;
        heap->root = (x->next == x) ? NULL : x->next;
        xfibheap_remove_node(x);

        int d = x->degree;
        while (xparray_get_impl(array, d)) {
            y = xparray_get_impl(array, d);
            if (heap->cmp(y->value, x->value, heap->cl) < 0) {
                xfibheap_swap_node_address(&x, &y);
            }
            
            /* make y the child of x */
            xfibheap_link(x, y);

            xparray_put_impl(array, d, NULL);
            d += 1;
        }

        xparray_put_impl(array, d, x);
    };

    for (int i = 0; i < XUTILS_UNLIMITED_BASED_ON_POWER_2; ++i) {
        y = xparray_get_impl(array, i);
        if (y) {
            if (!heap->root) {
                heap->root = y;
                y->next = y;
                y->prev = y;
            }
            else {
                xfibheap_insert_node(heap->root, y);
                if (heap->cmp(y->value, heap->root->value, heap->cl) < 0) {
                    heap->root = y;
                }
            }
        }
    }
}

void* xfibheap_pop(XFibHeap_PT heap) {
    xassert(heap);

    if (!heap || (heap->size <= 0)) {
        return NULL;
    }

    {
        XFibHeap_Node_PT pop = heap->root;
        void *value = NULL;

        /* used for consolidate */
        XPArray_PT array = xparray_new(XUTILS_UNLIMITED_BASED_ON_POWER_2);
        if (!array) {
            return NULL;
        }

        /* insert all children into root list */
        if (pop->child) {
            XFibHeap_Node_PT step = pop->child;
            XFibHeap_Node_PT next = NULL;

            step->prev->next = NULL; /* make the "while" loop has end node */
            while (step) {
                next = step->next;

                /* insert step into root list */
                xfibheap_insert_node(pop, step);
                step->parent = NULL;
                step->mark = false;

                step = next;
            }

            pop->child = NULL;
            pop->degree = 0;
        }

        /* remove pop from root list */
        if (pop == pop->next) {
            heap->root = NULL;
        }
        else {
            heap->root = pop->next;
            xfibheap_remove_node(pop);
            xfibheap_consolidate(heap, array);
        }
        --heap->size;

        value = pop->value;
        XMEM_FREE(pop);
        xparray_free(&array);

        return value;
    }
}

void* xfibheap_peek(XFibHeap_PT heap) {
    return heap ? (heap->root ? heap->root->value : NULL) : NULL;
}

static inline
void xfibheap_merge_list(XFibHeap_Node_PT node1, XFibHeap_Node_PT node2) {
    XFibHeap_Node_PT tail1 = node1->prev;
    XFibHeap_Node_PT tail2 = node2->prev;

    node1->prev = tail2;
    tail2->next = node1;

    tail1->next = node2;
    node2->prev = tail1;
}

bool xfibheap_merge(XFibHeap_PT heap1, XFibHeap_PT *heap2) {
    xassert(heap1);

    if (!heap1) {
        return false;
    }

    if (!heap2 || !*heap2) {
        return true;
    }

    if (heap1->root) {
        if ((*heap2)->root) {
            xfibheap_merge_list(heap1->root, (*heap2)->root);

            /* update the root if needed */
            if (heap1->cmp((*heap2)->root->value, heap1->root->value, heap1->cl) < 0) {
                heap1->root = (*heap2)->root;
            }
        }
    }
    else {
        heap1->root = (*heap2)->root;
    }

    heap1->size += (*heap2)->size;
    XMEM_FREE(*heap2);

    return true;
}

static
int xfibheap_map_impl(XFibHeap_Node_PT node, bool(*apply)(void *x, void *cl), void *cl) {
    XFibHeap_Node_PT start = node;
    XFibHeap_Node_PT next = NULL;
    int count = 0;

    while (node) {
        if (node->child) {
            count += xfibheap_map_impl(node->child, apply, cl);
        }

        next = node->next;

        if (apply(node->value, cl)) {
            ++count;
        }

        if (next == start) {
            return count;
        }

        node = next;
    }

    return count;
}

int xfibheap_map(XFibHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl) {
    return xfibheap_map_impl(heap ? heap->root : NULL, apply, cl);
}

static
int xfibheap_map_break_impl(XFibHeap_Node_PT node, bool break_true, bool(*apply)(void *x, void *cl), void *cl) {
    XFibHeap_Node_PT start = node;
    XFibHeap_Node_PT next = NULL;
    int count = 0;

    while (node) {
        if (node->child) {
            count += xfibheap_map_break_impl(node->child, break_true, apply, cl);
            if (count == 1) {
                return count;
            }
        }

        next = node->next;

        bool ret = apply(node->value, cl);
        if (ret && break_true) {
            return ++count;
        }
        if (!ret && !break_true) {
            return ++count;
        }

        if (next == start) {
            return count;
        }

        node = next;
    }

    return count;
}

bool xfibheap_map_break_if_true(XFibHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl) {
    return xfibheap_map_break_impl(heap ? heap->root : NULL, true, apply, cl);
}

bool xfibheap_map_break_if_false(XFibHeap_PT heap, bool (*apply)(void *x, void *cl), void *cl) {
    return xfibheap_map_break_impl(heap ? heap->root : NULL, false, apply, cl);
}

static
void xfibheap_free_impl(XFibHeap_Node_PT node, bool deep, bool(*apply)(void *x, void *cl), void *cl) {
    if (node) {
        node->prev->next = NULL;
        node->prev = NULL;
    }

    while (node) {
        if (node->child) {
            xfibheap_free_impl(node->child, deep, apply, cl);
        }

        {
            XFibHeap_Node_PT next = node->next;

            if (deep) {
                XMEM_FREE(node->value);
            }
            else if (apply) {
                apply(node->value, cl);
            }
            XMEM_FREE(node);

            node = next;
        }
    }
}

void xfibheap_free(XFibHeap_PT *pheap) {
    xfibheap_free_impl((pheap && *pheap) ? (*pheap)->root : NULL, false, NULL, NULL);
    XMEM_FREE(*pheap);
}

void xfibheap_free_apply(XFibHeap_PT *pheap, bool(*apply)(void *x, void *cl), void *cl) {
    xfibheap_free_impl((pheap && *pheap) ? (*pheap)->root : NULL, false, apply, cl);
    XMEM_FREE(*pheap);
}

void xfibheap_deep_free(XFibHeap_PT *pheap) {
    xfibheap_free_impl((pheap && *pheap) ? (*pheap)->root : NULL, true, NULL, NULL);
    XMEM_FREE(*pheap);
}

void xfibheap_clear(XFibHeap_PT heap) {
    if (heap) {
        xfibheap_free_impl(heap->root, false, NULL, NULL);
        heap->root = NULL;
        heap->size = 0;
    }
}

void xfibheap_clear_apply(XFibHeap_PT heap, bool(*apply)(void *x, void *cl), void *cl) {
    if (heap) {
        xfibheap_free_impl(heap->root, false, apply, cl);
        heap->root = NULL;
        heap->size = 0;
    }
}

void xfibheap_deep_clear(XFibHeap_PT heap) {
    if (heap) {
        xfibheap_free_impl(heap->root, true, NULL, NULL);
        heap->root = NULL;
        heap->size = 0;
    }
}

int xfibheap_size(XFibHeap_PT heap) {
    return (heap ? heap->size : 0);
}

bool xfibheap_is_empty(XFibHeap_PT heap) {
    return (heap ? (heap->size == 0) : true);
}

static
bool xfibheap_is_fibheap_impl(XFibHeap_PT heap, XFibHeap_Node_PT parent, XFibHeap_Node_PT node, int* count) {
    XFibHeap_Node_PT start = node;
    XFibHeap_Node_PT next = NULL;
    int degree = 0;

    while (node) {
        if (node->child && !xfibheap_is_fibheap_impl(heap, node, node->child, count)) {
            return false;
        }

        /* next and prev are correct */
        next = node->next;
        if (next->prev != node) {
            return false;
        }

        /* parent is correct */
        if (node->parent != parent) {
            return false;
        }

        /* heap->root has the minimum value */
        if (heap->cmp(node->value, heap->root->value, heap->cl) < 0) {
            return false;
        }

        ++*count;
        ++degree;

        if (next == start) {
            /* parent degree is correct */
            if (parent && parent->degree != degree) {
                return false;
            }

            return true;
        }

        node = next;
    }

    return true;
}

bool xfibheap_is_fibheap(XFibHeap_PT heap) {
    xassert(heap);

    if (!heap) {
        return false;
    }

    {
        int count = 0;

        if (!xfibheap_is_fibheap_impl(heap, NULL, heap->root, &count)) {
            return false;
        }

        /* total size is correct */
        if (heap->size != count) {
            return false;
        }
    }

    return true;
}

