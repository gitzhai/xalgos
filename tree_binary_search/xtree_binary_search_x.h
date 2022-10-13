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

#ifndef XBSTREEX_INCLUDED
#define XBSTREEX_INCLUDED

#include "../include/xtree_binary_search.h"

typedef struct XBSTree_Node* XBSTree_Node_PT;

struct XBSTree_Node {
    XBSTree_Node_PT parent;

    XBSTree_Node_PT left;
    XBSTree_Node_PT right;

    void *key;
    void *value;

    int   size;   /* = size(left) + size(right) + 1 */
};

struct XBSTree {
    XBSTree_Node_PT root;

    int (*cmp)(void *key1, void *key2, void *cl);
    void *cl;
};

/* used for internal implementations */
typedef struct XBSTree_3Paras  XBSTree_3Paras_T;
typedef struct XBSTree_3Paras* XBSTree_3Paras_PT;

struct XBSTree_3Paras {
    XBSTree_PT tree;

    void      *para1;  /* para1 - para3 are used for internal function call */
    void      *para2;
    void      *para3;
};

#endif
