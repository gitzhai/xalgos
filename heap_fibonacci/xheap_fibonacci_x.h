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

#ifndef XFIBHEAPX_INCLUDED
#define XFIBHEAPX_INCLUDED

#include "../include/xheap_fibonacci.h"

typedef struct XFibHeap_Node* XFibHeap_Node_PT;

struct XFibHeap_Node {
    void *value;             /* Note : hold "value" the first position always to support "value" update interface */

    XFibHeap_Node_PT parent; /* each node has its parent */
    XFibHeap_Node_PT child;  /* parent has only one child pointed directly */

    XFibHeap_Node_PT prev;   /* link to brother node */
    XFibHeap_Node_PT next;   /* link to brother node */

    int   mark;              /* lost own children or not since becoming the child */
    int   degree;            /* total number of children values of this node */
};

struct XFibHeap {
    XFibHeap_Node_PT root;
    int    size;    /* total number of values in the heap */

    int  (*cmp)(void *x, void *y, void *cl);
    void  *cl;
};

#endif
