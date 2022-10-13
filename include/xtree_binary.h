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

#ifndef XBINTREE_INCLUDED
#define XBINTREE_INCLUDED

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XBinTree*      XBinTree_PT;
typedef struct XBinTree_Node* XBinTree_Node_PT;

/* O(1) */
extern XBinTree_PT xbintree_new                        (void);

/* O(NlgN) */
extern XBinTree_PT xbintree_copy                       (XBinTree_PT tree);
extern XBinTree_PT xbintree_deep_copy                  (XBinTree_PT tree, int elem_size);

/* O(1) */
extern bool        xbintree_insert_left                (XBinTree_PT tree, XBinTree_Node_PT node, void *data);
extern bool        xbintree_insert_right               (XBinTree_PT tree, XBinTree_Node_PT node, void *data);

/* O(1) */
extern void*       xbintree_root                       (XBinTree_PT tree);

/* O(NlgN) */
extern bool        xbintree_remove_left                (XBinTree_PT tree, XBinTree_Node_PT node);
extern bool        xbintree_deep_remove_left           (XBinTree_PT tree, XBinTree_Node_PT node);
extern bool        xbintree_remove_right               (XBinTree_PT tree, XBinTree_Node_PT node);
extern bool        xbintree_deep_remove_right          (XBinTree_PT tree, XBinTree_Node_PT node);

/* O(N) */
extern void       xbintree_remove_left_most            (XBinTree_PT tree);
extern void       xbintree_remove_save_left_most       (XBinTree_PT tree, void **data);
extern void       xbintree_deep_remove_left_most       (XBinTree_PT tree);

/* O(N) */
extern void       xbintree_remove_right_most           (XBinTree_PT tree);
extern void       xbintree_remove_save_right_most      (XBinTree_PT tree, void **data);
extern void       xbintree_deep_remove_right_most      (XBinTree_PT tree);

/* O(NlgN) */
extern void       xbintree_clear                       (XBinTree_PT tree);
extern void       xbintree_clear_apply                 (XBinTree_PT tree, bool (*apply)(void *data, void *cl), void *cl);
extern void       xbintree_deep_clear                  (XBinTree_PT tree);

/* O(NlgN) */
extern void       xbintree_free                        (XBinTree_PT *ptree);
extern void       xbintree_free_apply                  (XBinTree_PT *ptree, bool (*apply)(void *data, void *cl), void *cl);
extern void       xbintree_deep_free                   (XBinTree_PT *ptree);

/* O(N) */
extern int        xbintree_map_preorder                (XBinTree_PT tree, bool (*apply)(void *data, void *cl), void *cl);
extern int        xbintree_map_postorder               (XBinTree_PT tree, bool (*apply)(void *data, void *cl), void *cl);
extern int        xbintree_map_levelorder              (XBinTree_PT tree, bool (*apply)(void *data, void *cl), void *cl);

/* O(N) */
extern int        xbintree_map_inorder                 (XBinTree_PT tree, bool (*apply)(void *data, void *cl), void *cl);
extern bool       xbintree_map_inorder_break_if_true   (XBinTree_PT tree, bool (*apply)(void *data, void *cl), void *cl);
extern bool       xbintree_map_inorder_break_if_false  (XBinTree_PT tree, bool (*apply)(void *data, void *cl), void *cl);

/* O(1) */
extern bool       xbintree_swap                        (XBinTree_PT tree1, XBinTree_PT tree2);

/* O(1) */
extern int        xbintree_size                        (XBinTree_PT tree);
extern bool       xbintree_is_empty                    (XBinTree_PT tree);

/* O(NlgN) */
extern int        xbintree_height                      (XBinTree_PT tree);

#ifdef __cplusplus
}
#endif

#endif
