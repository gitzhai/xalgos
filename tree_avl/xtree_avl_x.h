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

#ifndef XAVLTREEX_INCLUDED
#define XAVLTREEX_INCLUDED

#include "../include/xtree_avl.h"

typedef struct XAVLTree_Node* XAVLTree_Node_PT;

struct XAVLTree_Node {
    XAVLTree_Node_PT parent;
    XAVLTree_Node_PT left;
    XAVLTree_Node_PT right;

    void *key;
    void *value;

    int   height;  /* height of the node */
    int   size;    /* total keys included in node and its children */
};

struct XAVLTree {
    XAVLTree_Node_PT root;

    int (*cmp)(void *key1, void *key2, void *cl);
    void *cl;
};

#endif
