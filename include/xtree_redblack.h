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

#ifndef XRBTREE_INCLUDED
#define XRBTREE_INCLUDED

#include "xlist_s.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XRBTree*      XRBTree_PT;

/* O(1) */
extern XRBTree_PT   xrbtree_new               (int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(NlgN) */
extern XRBTree_PT   xrbtree_copy              (XRBTree_PT tree);
extern XRBTree_PT   xrbtree_deep_copy         (XRBTree_PT tree, int key_size, int value_size);

/* O(lgN) : all put interfaces */
extern bool         xrbtree_put_repeat        (XRBTree_PT tree, void *key, void *value);
extern bool         xrbtree_put_unique        (XRBTree_PT tree, void *key, void *value);
extern bool         xrbtree_put_replace       (XRBTree_PT tree, void *key, void *value, void **old_value);
extern bool         xrbtree_put_deep_replace  (XRBTree_PT tree, void *key, void *value);

/* O(lgN) */
extern void*        xrbtree_min               (XRBTree_PT tree);
extern void*        xrbtree_max               (XRBTree_PT tree);

/* O(lgN) */
extern void*        xrbtree_floor             (XRBTree_PT tree, void *key);
extern void*        xrbtree_ceiling           (XRBTree_PT tree, void *key);

/* O(lgN) */
extern void*        xrbtree_select            (XRBTree_PT tree, int k);
extern int          xrbtree_rank              (XRBTree_PT tree, void *key);

/* O(lgN) */
extern void*        xrbtree_get               (XRBTree_PT tree, void *key);
/* O(NlgN) */
extern XSList_PT    xrbtree_get_all           (XRBTree_PT tree, void *key);

/* O(lgN) */
extern bool         xrbtree_find              (XRBTree_PT tree, void *key);
extern bool         xrbtree_find_replace      (XRBTree_PT tree, void *key, void *value, void **old_value);
extern bool         xrbtree_find_deep_replace (XRBTree_PT tree, void *key, void *value);

/* O(lgN) */
extern bool         xrbtree_index_replace     (XRBTree_PT tree, int k, void *value, void **old_value);
extern bool         xrbtree_index_deep_replace(XRBTree_PT tree, int k, void *value);

/* O(N) */
extern XSList_PT    xrbtree_keys              (XRBTree_PT tree, void *low, void *high);

/* O(NlgN) */
extern void         xrbtree_clear             (XRBTree_PT tree);
extern void         xrbtree_clear_apply       (XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern void         xrbtree_deep_clear        (XRBTree_PT tree);

/* O(NlgN) */
extern void         xrbtree_free              (XRBTree_PT *ptree);
extern void         xrbtree_free_apply        (XRBTree_PT *ptree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern void         xrbtree_deep_free         (XRBTree_PT *ptree);

/* O(lgN) */
extern void         xrbtree_remove_min        (XRBTree_PT tree);
extern void         xrbtree_remove_max        (XRBTree_PT tree);

/* O(lgN) */
extern void         xrbtree_remove_save_min   (XRBTree_PT tree, void **key, void **value);
extern void         xrbtree_remove_save_max   (XRBTree_PT tree, void **key, void **value);

/* O(lgN) */
extern void         xrbtree_deep_remove_min   (XRBTree_PT tree);
extern void         xrbtree_deep_remove_max   (XRBTree_PT tree);

/* O(lgN) */
extern int          xrbtree_remove            (XRBTree_PT tree, void *key);

/* O(NlgN) */
extern int          xrbtree_remove_all        (XRBTree_PT tree, void *key);

/* O(lgN) */
extern int          xrbtree_remove_save       (XRBTree_PT tree, void *key, void **value);
extern int          xrbtree_deep_remove       (XRBTree_PT tree, void *key);

/* O(NlgN) */
extern int          xrbtree_deep_remove_all   (XRBTree_PT tree, void *key);

/* O(N) */
extern int          xrbtree_map_preorder      (XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int          xrbtree_map_inorder       (XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int          xrbtree_map_postorder     (XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int          xrbtree_map_levelorder    (XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(N) */
extern int          xrbtree_map_min_to_max                      (XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool         xrbtree_map_min_to_max_break_if_true        (XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool         xrbtree_map_min_to_max_break_if_false       (XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(N) */
extern int          xrbtree_map_max_to_min                      (XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool         xrbtree_map_max_to_min_break_if_true        (XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool         xrbtree_map_max_to_min_break_if_false       (XRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(N) */
extern int          xrbtree_map_key_min_to_max                  (XRBTree_PT tree, bool (*apply)(void *key, void *cl), void *cl);
extern bool         xrbtree_map_key_min_to_max_break_if_true    (XRBTree_PT tree, bool (*apply)(void *key, void *cl), void *cl);
extern bool         xrbtree_map_key_min_to_max_break_if_false   (XRBTree_PT tree, bool (*apply)(void *key, void *cl), void *cl);

/* O(N) */
extern int          xrbtree_map_key_max_to_min                  (XRBTree_PT tree, bool (*apply)(void *key, void *cl), void *cl);
extern bool         xrbtree_map_key_max_to_min_break_if_true    (XRBTree_PT tree, bool (*apply)(void *key, void *cl), void *cl);
extern bool         xrbtree_map_key_max_to_min_break_if_false   (XRBTree_PT tree, bool (*apply)(void *key, void *cl), void *cl);

/* O(N) */
extern int          xrbtree_scope_map_min_to_max                (XRBTree_PT tree, void *low, void *high, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool         xrbtree_scope_map_min_to_max_break_if_true  (XRBTree_PT tree, void *low, void *high, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool         xrbtree_scope_map_min_to_max_break_if_false (XRBTree_PT tree, void *low, void *high, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(1) */
extern bool         xrbtree_swap                                (XRBTree_PT tree1, XRBTree_PT tree2);

/* O(1) */
extern int          xrbtree_size                                (XRBTree_PT tree);
extern bool         xrbtree_is_empty                            (XRBTree_PT tree);

/* O(lgN) */
extern int          xrbtree_keys_size                           (XRBTree_PT tree, void *low, void *high);

/* O(NlgN) */
extern int          xrbtree_height                              (XRBTree_PT tree);

/* NlgN */
extern bool         xrbtree_is_rbtree                           (XRBTree_PT tree);

#ifdef __cplusplus
}
#endif

#endif
