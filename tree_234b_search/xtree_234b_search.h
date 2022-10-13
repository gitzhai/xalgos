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

#ifndef T234BSTREE_INCLUDED
#define T234BSTREE_INCLUDED

#include "../include/xqueue_fifo.h"

typedef struct T234BSTree*      T234BSTree_PT;
typedef struct T234BSTree_Node* T234BSTree_Node_PT;

extern T234BSTree_PT t234bstree_new               (int (*cmp)(void *key1, void *key2, void *cl), void *cl);

extern T234BSTree_PT t234bstree_copy              (T234BSTree_PT tree);

extern bool          t234bstree_put_repeat        (T234BSTree_PT tree, void *key, void *value);
extern bool          t234bstree_put_unique        (T234BSTree_PT tree, void *key, void *value);
extern bool          t234bstree_put_replace       (T234BSTree_PT tree, void *key, void *value);
extern bool          t234bstree_put_deep_replace  (T234BSTree_PT tree, void *key, void *value);

extern void*         t234bstree_min               (T234BSTree_PT tree);
extern void*         t234bstree_max               (T234BSTree_PT tree);

extern void*         t234bstree_floor             (T234BSTree_PT tree, void *key);
extern void*         t234bstree_ceiling           (T234BSTree_PT tree, void *key);

extern void*         t234bstree_select            (T234BSTree_PT tree, int k);
extern int           t234bstree_rank              (T234BSTree_PT tree, void *key);

extern void*         t234bstree_get               (T234BSTree_PT tree, void *key);
extern bool          t234bstree_find              (T234BSTree_PT tree, void *key);

extern XFifo_PT      t234bstree_keys              (T234BSTree_PT tree, void *low, void *high);

extern void          t234bstree_free              (T234BSTree_PT *ptree);
extern void          t234bstree_deep_free         (T234BSTree_PT *ptree);

extern void          t234bstree_clear             (T234BSTree_PT tree);
extern void          t234bstree_deep_clear        (T234BSTree_PT tree);

extern void          t234bstree_remove_min        (T234BSTree_PT tree);
extern void          t234bstree_remove_max        (T234BSTree_PT tree);
extern void          t234bstree_remove            (T234BSTree_PT tree, void *key);

extern int           t234bstree_map_preorder      (T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool          t234bstree_map_preorder_break_if_true  (T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool          t234bstree_map_preorder_break_if_false (T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);

extern int           t234bstree_map_inorder       (T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool          t234bstree_map_inorder_break_if_true  (T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool          t234bstree_map_inorder_break_if_false (T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);

extern int           t234bstree_map_postorder     (T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int           t234bstree_map_levelorder    (T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);

extern int           t234bstree_map_min_to_max    (T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int           t234bstree_map_max_to_min    (T234BSTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);

extern bool          t234bstree_swap              (T234BSTree_PT tree1, T234BSTree_PT tree2);

extern int           t234bstree_height            (T234BSTree_PT tree);

extern int           t234bstree_size              (T234BSTree_PT tree);
extern int           t234bstree_keys_size         (T234BSTree_PT tree, void *low, void *high);

extern bool          t234bstree_is_empty          (T234BSTree_PT tree);

#endif
