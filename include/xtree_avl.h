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

#ifndef XAVLTREE_INCLUDED
#define XAVLTREE_INCLUDED

#include "xlist_s.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XAVLTree*  XAVLTree_PT;

/* O(1) */
extern XAVLTree_PT  xavltree_new                (int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(lgN) */
extern bool         xavltree_put_repeat         (XAVLTree_PT tree, void *key, void *value);
extern bool         xavltree_put_unique         (XAVLTree_PT tree, void *key, void *value);
extern bool         xavltree_put_replace        (XAVLTree_PT tree, void *key, void *value, void **old_value);
extern bool         xavltree_put_deep_replace   (XAVLTree_PT tree, void *key, void *value);

/* O(lgN) */
extern void*        xavltree_min                (XAVLTree_PT tree);
extern void*        xavltree_max                (XAVLTree_PT tree);

/* O(lgN) */
extern void*        xavltree_floor              (XAVLTree_PT tree, void *key);
extern void*        xavltree_ceiling            (XAVLTree_PT tree, void *key);

/* O(lgN) */
extern void*        xavltree_select             (XAVLTree_PT tree, int k);
extern int          xavltree_rank               (XAVLTree_PT tree, void *key);

/* O(lgN) */
extern void*        xavltree_get                (XAVLTree_PT tree, void *key);

/* O(lgN) */
extern bool         xavltree_find               (XAVLTree_PT tree, void *key);

/* O(NlgN) */
extern void         xavltree_clear              (XAVLTree_PT tree);
extern void         xavltree_clear_apply        (XAVLTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern void         xavltree_deep_clear         (XAVLTree_PT tree);

/* O(NlgN) */
extern void         xavltree_free               (XAVLTree_PT *ptree);
extern void         xavltree_free_apply         (XAVLTree_PT *ptree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern void         xavltree_deep_free          (XAVLTree_PT *ptree);

/* O(lgN) */
extern void         xavltree_remove_min         (XAVLTree_PT tree);
extern void         xavltree_remove_max         (XAVLTree_PT tree);

/* O(lgN) */
extern int          xavltree_remove             (XAVLTree_PT tree, void *key);

/* O(N) */
extern int          xavltree_map_preorder       (XAVLTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int          xavltree_map_inorder        (XAVLTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int          xavltree_map_postorder      (XAVLTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int          xavltree_map_levelorder     (XAVLTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(N) */
extern int          xavltree_map_min_to_max     (XAVLTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool         xavltree_map_min_to_max_break_if_true  (XAVLTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool         xavltree_map_min_to_max_break_if_false (XAVLTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(1) */
extern bool         xavltree_swap               (XAVLTree_PT tree1, XAVLTree_PT tree2);

/* O(1) */
extern int          xavltree_size               (XAVLTree_PT tree);
extern bool         xavltree_is_empty           (XAVLTree_PT tree);

/* O(NlgN) */
extern int          xavltree_height             (XAVLTree_PT tree);

/* NlgN */
extern bool         xavltree_is_avltree         (XAVLTree_PT tree);

#ifdef __cplusplus
}
#endif

#endif
