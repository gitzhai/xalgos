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

#ifndef XLISTRBTREE_INCLUDED
#define XLISTRBTREE_INCLUDED

#include "xlist_s.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XListRBTree*      XListRBTree_PT;

/* O(1) */
extern XListRBTree_PT xlistrbtree_new               (int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(NlgN) */
extern XListRBTree_PT xlistrbtree_copy              (XListRBTree_PT tree);
extern XListRBTree_PT xlistrbtree_deep_copy         (XListRBTree_PT tree, int key_size, int value_size);

/* O(lgN) */
extern bool           xlistrbtree_put_repeat        (XListRBTree_PT tree, void *key, void *value);
extern bool           xlistrbtree_put_unique        (XListRBTree_PT tree, void *key, void *value);
extern bool           xlistrbtree_put_replace       (XListRBTree_PT tree, void *key, void *value, void **old_value);
extern bool           xlistrbtree_put_deep_replace  (XListRBTree_PT tree, void *key, void *value);

/* O(lgN) */
extern void           xlistrbtree_key_unique        (XListRBTree_PT tree, void *key);
/* O(NlgN) */
extern void           xlistrbtree_unique            (XListRBTree_PT tree);

/* O(lgN) */
extern void*          xlistrbtree_min               (XListRBTree_PT tree);
extern void*          xlistrbtree_max               (XListRBTree_PT tree);

/* O(lgN) */
extern void*          xlistrbtree_floor             (XListRBTree_PT tree, void *key);
extern void*          xlistrbtree_ceiling           (XListRBTree_PT tree, void *key);

/* O(lgN) */
extern void*          xlistrbtree_select            (XListRBTree_PT tree, int k);
extern int            xlistrbtree_rank              (XListRBTree_PT tree, void *key);

/* O(lgN) */
extern void*          xlistrbtree_get               (XListRBTree_PT tree, void *key);
/* O(NlgN) */
extern XSList_PT      xlistrbtree_get_all           (XListRBTree_PT tree, void *key);

/* O(lgN) */
extern bool           xlistrbtree_find              (XListRBTree_PT tree, void *key);
extern bool           xlistrbtree_find_replace      (XListRBTree_PT tree, void *key, void *value, void **old_value);
extern bool           xlistrbtree_find_deep_replace (XListRBTree_PT tree, void *key, void *value);

/* O(lgN) */
extern bool           xlistrbtree_index_replace     (XListRBTree_PT tree, int k, void *value, void **old_value);
extern bool           xlistrbtree_index_deep_replace(XListRBTree_PT tree, int k, void *value);

/* O(N) */
extern XSList_PT      xlistrbtree_keys              (XListRBTree_PT tree, void *low, void *high);

/* O(NlgN) */
extern void           xlistrbtree_clear             (XListRBTree_PT tree);
extern void           xlistrbtree_deep_clear        (XListRBTree_PT tree);

/* O(NlgN) */
extern void           xlistrbtree_free              (XListRBTree_PT *ptree);
extern void           xlistrbtree_deep_free         (XListRBTree_PT *ptree);

/* O(lgN) */
extern void           xlistrbtree_remove_min        (XListRBTree_PT tree);
extern void           xlistrbtree_remove_max        (XListRBTree_PT tree);

/* O(lgN) */
extern void           xlistrbtree_remove_save_min   (XListRBTree_PT tree, void **key, void **value);
extern void           xlistrbtree_remove_save_max   (XListRBTree_PT tree, void **key, void **value);

/* O(lgN) */
extern void           xlistrbtree_deep_remove_min   (XListRBTree_PT tree);
extern void           xlistrbtree_deep_remove_max   (XListRBTree_PT tree);

/* O(lgN) */
extern int            xlistrbtree_remove            (XListRBTree_PT tree, void *key);
/* O(NlgN) */
extern int            xlistrbtree_remove_all        (XListRBTree_PT tree, void *key);

/* O(lgN) */
extern int            xlistrbtree_remove_save       (XListRBTree_PT tree, void *key, void **value);
extern int            xlistrbtree_deep_remove       (XListRBTree_PT tree, void *key);
/* O(NlgN) */
extern int            xlistrbtree_deep_remove_all   (XListRBTree_PT tree, void *key);

/* O(N) */
extern int            xlistrbtree_map_preorder      (XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int            xlistrbtree_map_inorder       (XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int            xlistrbtree_map_postorder     (XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int            xlistrbtree_map_levelorder    (XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(N) */
extern int            xlistrbtree_map_min_to_max                      (XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool           xlistrbtree_map_min_to_max_break_if_true        (XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool           xlistrbtree_map_min_to_max_break_if_false       (XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(N) */
extern int            xlistrbtree_map_max_to_min                      (XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool           xlistrbtree_map_max_to_min_break_if_true        (XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool           xlistrbtree_map_max_to_min_break_if_false       (XListRBTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(N) */
extern int            xlistrbtree_scope_map_min_to_max                (XListRBTree_PT tree, void *low, void *high, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool           xlistrbtree_scope_map_min_to_max_break_if_true  (XListRBTree_PT tree, void *low, void *high, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool           xlistrbtree_scope_map_min_to_max_break_if_false (XListRBTree_PT tree, void *low, void *high, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(1) */
extern bool           xlistrbtree_swap              (XListRBTree_PT tree1, XListRBTree_PT tree2);

/* O(NlgN) */
extern bool           xlistrbtree_merge_repeat      (XListRBTree_PT tree1, XListRBTree_PT *ptree2);
extern bool           xlistrbtree_merge_unique      (XListRBTree_PT tree1, XListRBTree_PT *ptree2);

/* O(1) */
extern int            xlistrbtree_size              (XListRBTree_PT tree);
extern bool           xlistrbtree_is_empty          (XListRBTree_PT tree);

/* O(N) */
extern int            xlistrbtree_keys_size         (XListRBTree_PT tree, void *low, void *high);

/* O(NlgN) */
extern int            xlistrbtree_height            (XListRBTree_PT tree);

/* NlgN */
extern bool           xlistrbtree_is_rbtree         (XListRBTree_PT tree);

#ifdef __cplusplus
}
#endif

#endif
