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

#ifndef XBSTREE_INCLUDED
#define XBSTREE_INCLUDED

#include <stdbool.h>
#include "xlist_s.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XBSTree*      XBSTree_PT;

/* O(1) */
extern XBSTree_PT xbstree_new               (int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(NlgN) */
extern XBSTree_PT xbstree_copy              (XBSTree_PT tree);
extern XBSTree_PT xbstree_deep_copy         (XBSTree_PT tree, int key_size, int value_size);

/* O(lgN) */
extern bool       xbstree_put_repeat        (XBSTree_PT tree, void *key, void *value);
extern bool       xbstree_put_unique        (XBSTree_PT tree, void *key, void *value);
extern bool       xbstree_put_replace       (XBSTree_PT tree, void *key, void *value, void **old_value);
extern bool       xbstree_put_deep_replace  (XBSTree_PT tree, void *key, void *value);

/* O(lgN) */
extern void*      xbstree_min               (XBSTree_PT tree);
extern void*      xbstree_max               (XBSTree_PT tree);

/* O(lgN) */
extern void*      xbstree_floor             (XBSTree_PT tree, void *key);
extern void*      xbstree_ceiling           (XBSTree_PT tree, void *key);

/* O(lgN) */
extern void*      xbstree_select            (XBSTree_PT tree, int k);
extern int        xbstree_rank              (XBSTree_PT tree, void *key);

/* O(lgN) */
extern void*      xbstree_get               (XBSTree_PT tree, void *key);
/* O(NlgN) */
extern XSList_PT  xbstree_get_all           (XBSTree_PT tree, void *key);

/* O(lgN) */
extern bool       xbstree_find              (XBSTree_PT tree, void *key);
extern bool       xbstree_find_put_if_not   (XBSTree_PT tree, void *key, void *value);

/* O(NlgN) */
extern void       xbstree_clear             (XBSTree_PT tree);
extern void       xbstree_clear_apply       (XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern void       xbstree_deep_clear        (XBSTree_PT tree);

/* O(NlgN) */
extern void       xbstree_free              (XBSTree_PT *ptree);
extern void       xbstree_free_apply        (XBSTree_PT *ptree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern void       xbstree_deep_free         (XBSTree_PT *ptree);

/* O(lgN) */
extern void       xbstree_remove_min        (XBSTree_PT tree);
extern void       xbstree_remove_max        (XBSTree_PT tree);

/* O(lgN) */
extern void       xbstree_remove            (XBSTree_PT tree, void *key);
extern void       xbstree_deep_remove       (XBSTree_PT tree, void *key);

/* O(N) */
extern int        xbstree_map_preorder                  (XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int        xbstree_map_inorder                   (XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int        xbstree_map_postorder                 (XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int        xbstree_map_levelorder                (XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(N) */
extern int        xbstree_map_min_to_max                (XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool       xbstree_map_min_to_max_break_if_true  (XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool       xbstree_map_min_to_max_break_if_false (XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(N) */
extern int        xbstree_map_max_to_min                (XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool       xbstree_map_max_to_min_break_if_true  (XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool       xbstree_map_max_to_min_break_if_false (XBSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(N) */
extern int        xbstree_scope_map_min_to_max          (XBSTree_PT tree, void *low, void *high, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(1) */
extern bool       xbstree_swap              (XBSTree_PT tree1, XBSTree_PT tree2);

/* O(1) */
extern int        xbstree_size              (XBSTree_PT tree);
extern bool       xbstree_is_empty          (XBSTree_PT tree);

/* O(NlgN) */
extern int        xbstree_height            (XBSTree_PT tree);

/* O(N) */
extern bool       xbstree_is_bstree         (XBSTree_PT tree);

#ifdef __cplusplus
}
#endif

#endif
