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

#ifndef XLISTRBTREEX_INCLUDED
#define XLISTRBTREEX_INCLUDED

#include <stdbool.h>
#include "../include/xlist_s_raw.h"
#include "../include/xtree_redblack_list.h"

typedef struct XListRBTree_Node* XListRBTree_Node_PT;

struct XListRBTree_Node {
    XListRBTree_Node_PT parent;
    XListRBTree_Node_PT left;
    XListRBTree_Node_PT right;

    void *key;
    XRSList_PT values;  /* just has one "key" saved for all the "same keys" if "cmp" return true */

    int   node_size;    /* the element size of the node itself */
    int   size;         /* the element size including its children */
    bool  color;        /* red : false,  black : true */
};

struct XListRBTree {
    XListRBTree_Node_PT root;

    int (*cmp)(void *key1, void *key2, void *cl);
    void *cl;
};

/* used for internal implementations */
typedef struct XListRBTree_3Paras  XListRBTree_3Paras_T;
typedef struct XListRBTree_3Paras* XListRBTree_3Paras_PT;

struct XListRBTree_3Paras {
    XListRBTree_PT tree;

    void      *para1;  /* para1 - para3 are used for internal function call */
    void      *para2;
    void      *para3;
};

/* O(NlgN) */
extern void                 xlistrbtree_copy_break_if_false_impl     (XListRBTree_PT tree, XListRBTree_Node_PT node, XListRBTree_PT ntree, XListRBTree_Node_PT nparent, bool root, bool left, bool *false_found, XListRBTree_Node_PT (*apply)(XListRBTree_Node_PT node, XListRBTree_Node_PT nparent, bool *false_found, void *cl), void *cl);
extern int                  xlistrbtree_deep_remove_impl             (XListRBTree_PT tree, void *key, bool remove_all, bool for_set);

/* O(N) */
extern int                  xlistrbtree_map_min_to_max_impl          (XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool                 xlistrbtree_map_min_to_max_break_if_impl (XListRBTree_PT tree, bool break_true, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(N) */
extern int                  xlistrbtree_map_max_to_min_impl          (XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool                 xlistrbtree_map_max_to_min_break_if_impl (XListRBTree_PT tree, bool break_true, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(lgN) */
extern XListRBTree_Node_PT  xlistrbtree_get_impl                     (XListRBTree_PT tree, XListRBTree_Node_PT node, void *key);

/* O(lgN) */
extern XListRBTree_Node_PT  xlistrbtree_min_impl                     (XListRBTree_PT tree, XListRBTree_Node_PT node);
extern XListRBTree_Node_PT  xlistrbtree_max_impl                     (XListRBTree_PT tree, XListRBTree_Node_PT node);

/* O(1) */
extern XListRBTree_Node_PT  xlistrbtree_prev_node                    (XListRBTree_PT tree, XListRBTree_Node_PT node);
extern XListRBTree_Node_PT  xlistrbtree_next_node                    (XListRBTree_PT tree, XListRBTree_Node_PT node);

/* O(NlgN) */
extern void                 xlistrbtree_unique_except                (XListRBTree_PT tree, void *key);
extern XListRBTree_PT       xlistrbtree_unique_except_return_uniqued (XListRBTree_PT tree, void *key);
extern XListRBTree_PT       xlistrbtree_unique_return_uniqued        (XListRBTree_PT tree);

/* O(N) */
extern bool                 xlistrbtree_has_repeat_keys_except       (XListRBTree_PT tree, void *key);
extern int                  xlistrbtree_repeat_keys_size_except      (XListRBTree_PT tree, void *key);

#endif
