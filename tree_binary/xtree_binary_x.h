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

#ifndef XBINTREEX_INCLUDED
#define XBINTREEX_INCLUDED

#include "../include/xtree_binary.h"

struct XBinTree_Node {
    XBinTree_Node_PT parent;

    XBinTree_Node_PT left;
    XBinTree_Node_PT right;

    void *data;
};

struct XBinTree {
    XBinTree_Node_PT root;

    int size;
};

/* used for internal implementations */
typedef struct XBinTree_3Paras  XBinTree_3Paras_T;
typedef struct XBinTree_3Paras* XBinTree_3Paras_PT;

struct XBinTree_3Paras {
    XBinTree_PT tree;

    void      *para1;  /* para1 - para3 are used for internal function call */
    void      *para2;
    void      *para3;
};


/* for internal use */

/* O(1) */
extern XBinTree_Node_PT xbintree_new_node                         (void *data);
extern void*            xbintree_node_data                        (XBinTree_Node_PT node);
extern XBinTree_PT      xbintree_new_with_root                    (XBinTree_Node_PT root, int size);
extern XBinTree_Node_PT xbintree_reset                            (XBinTree_PT tree);

/* O(N) */
extern int              xbintree_map_inorder_impl                 (XBinTree_Node_PT node, bool (*apply)(void *data, void *cl), void *cl);
extern void             xbintree_map_inorder_break_if_true_impl   (XBinTree_Node_PT node, bool *true_found, bool (*apply)(void *data, void *cl), void *cl);
extern void             xbintree_map_inorder_break_if_false_impl  (XBinTree_Node_PT node, bool *false_found, bool (*apply)(void *data, void *cl), void *cl);

/* O(N) */
extern void             xbintree_free_node_impl                   (XBinTree_Node_PT node, bool deep);
extern void             xbintree_free_node_impl_apply             (XBinTree_Node_PT node, bool (*apply)(void **data, void *cl), void *cl);

#endif
