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

#ifndef XMTREEX_INCLUDED
#define XMTREEX_INCLUDED

#include "../include/xtree_set.h"
#include "../include/xtree_multiple_branch.h"

struct XMTree_Node {
    XMTree_Node_PT parent;
    XSet_PT        children;  /* including all children XMTree_Node_PT */

    void  *key;
    void  *value;

    //int    size;
};

struct XMTree {
    XMTree_Node_PT root;

    int (*cmp) (void *key1, void *key2, void *cl);
    void *cl;
};

/* used for internal implementations */
typedef struct XMTree_3Paras  XMTree_3Paras_T;
typedef struct XMTree_3Paras* XMTree_3Paras_PT;

struct XMTree_3Paras {
    XMTree_PT tree;

    void      *para1;  /* para1 - para3 are used for internal function call */
    void      *para2;
    void      *para3;
};

#endif
