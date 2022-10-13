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

#include "../include/xqueue_deque.h"
#include "../include/xqueue_stack.h"

XStack_PT xstack_new(int capacity) {
    return xdeque_new(capacity);
}

XStack_PT xstack_copy(XStack_PT stack) {
    return xdeque_copy(stack);
}

XStack_PT xstack_copyn(XStack_PT stack, int count) {
    return xdeque_copyn(stack, count);
}

XStack_PT xstack_deep_copy(XStack_PT stack, int elem_size) {
    return xdeque_deep_copy(stack, elem_size);
}

XStack_PT xstack_deep_copyn(XStack_PT stack, int count, int elem_size) {
    return xdeque_deep_copyn(stack, count, elem_size);
}

bool xstack_push(XStack_PT stack, void *x) {
    return xdeque_push_back(stack, x);
}

bool xstack_push_no_limit(XStack_PT stack, void *x) {
    return xdeque_push_back_no_limit(stack, x);
}

void* xstack_pop(XStack_PT stack) {
    return xdeque_pop_back(stack);
}

void* xstack_peek(XStack_PT stack) {
    return xdeque_back(stack);
}

void xstack_free(XStack_PT *pstack) {
    xdeque_free(pstack);
}

void xstack_free_apply(XStack_PT *pstack, bool (*apply)(void *x, void *cl), void *cl) {
    xdeque_free_apply(pstack, apply, cl);
}

void xstack_deep_free(XStack_PT *pstack) {
    xdeque_deep_free(pstack);
}

void xstack_clear(XStack_PT stack) {
    xdeque_clear(stack);
}

void xstack_clear_apply(XStack_PT stack, bool (*apply)(void *x, void *cl), void *cl) {
    xdeque_clear_apply(stack, apply, cl);
}

void xstack_deep_clear(XStack_PT stack) {
    xdeque_deep_clear(stack);
}

int xstack_size(XStack_PT stack) {
    return xdeque_size(stack);
}

bool xstack_is_empty(XStack_PT stack) {
    return xdeque_is_empty(stack);
}

int xstack_capacity(XStack_PT stack) {
    return xdeque_capacity(stack);
}

bool xstack_set_capacity_no_limit(XStack_PT stack) {
    return xdeque_set_capacity_no_limit(stack);
}

bool xstack_swap(XStack_PT stack1, XStack_PT stack2) {
    return xdeque_swap(stack1, stack2);
}

bool xstack_set_strategy_discard_new(XStack_PT stack) {
    return xdeque_set_strategy_discard_new(stack);
}

bool xstack_set_strategy_discard_top(XStack_PT stack) {
    return xdeque_set_strategy_discard_back(stack);
}

bool xstack_set_strategy_discard_bottom(XStack_PT stack) {
    return xdeque_set_strategy_discard_front(stack);
}
