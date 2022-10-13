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

#ifndef XRBTREEX_INCLUDED
#define XRBTREEX_INCLUDED

#include <stdbool.h>
#include "../include/xtree_redblack.h"

typedef struct XRBTree_Node* XRBTree_Node_PT;

struct XRBTree_Node {
    XRBTree_Node_PT parent;
    XRBTree_Node_PT left;
    XRBTree_Node_PT right;

    void *key;
    void *value;

    int   size;
    bool  color;      /* red : false,  black : true */
};

struct XRBTree {
    XRBTree_Node_PT root;

    int (*cmp)(void *key1, void *key2, void *cl);
    void *cl;
};

/* used for internal implementations */
typedef struct XRBTree_3Paras  XRBTree_3Paras_T;
typedef struct XRBTree_3Paras* XRBTree_3Paras_PT;

struct XRBTree_3Paras {
    XRBTree_PT tree;

    void      *para1;  /* para1 - para3 are used for internal function call */
    void      *para2;
    void      *para3;
};

/* O(N) */
extern int   xrbtree_map_min_to_max_impl               (XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool  xrbtree_map_min_to_max_break_if_impl      (XRBTree_PT tree, bool break_true, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(N) */
extern int   xrbtree_map_max_to_min_impl               (XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool  xrbtree_map_max_to_min_break_if_impl      (XRBTree_PT tree, bool break_true, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(N) */
extern int   xrbtree_map_key_min_to_max_impl           (XRBTree_PT tree, bool (*apply)(void *key, void *cl), void *cl);
extern bool  xrbtree_map_key_min_to_max_break_if_impl  (XRBTree_PT tree, bool break_true, bool (*apply)(void *key, void *cl), void *cl);

/* O(N) */
extern int   xrbtree_map_key_max_to_min_impl           (XRBTree_PT tree, bool (*apply)(void *key, void *cl), void *cl);
extern bool  xrbtree_map_key_max_to_min_break_if_impl  (XRBTree_PT tree, bool break_true, bool (*apply)(void *key, void *cl), void *cl);

/* O(lgN) */
extern void  xrbtree_replace_key                       (XRBTree_PT tree, void *old_key, void *new_key);

#endif
