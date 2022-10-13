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

#ifndef XHASHMAP_INCLUDED
#define XHASHMAP_INCLUDED

#include <stdbool.h>
#include "xqueue_deque.h"
#include "xhash_rbtree.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XRBTreeHash_PT XHashMap_PT;

/* O(1) */
extern XHashMap_PT  xhashmap_new                 (int hint, int (*hash)(void *key), int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(NlgN) */
extern XHashMap_PT  xhashmap_copy                (XHashMap_PT map);
extern XHashMap_PT  xhashmap_deep_copy           (XHashMap_PT map, int key_size, int value_size);

/* O(lgN) */
extern bool         xhashmap_put_repeat          (XHashMap_PT map, void *key, void *value);
extern bool         xhashmap_put_unique          (XHashMap_PT map, void *key, void *value);
extern bool         xhashmap_put_replace         (XHashMap_PT map, void *key, void *value);
extern bool         xhashmap_put_deep_replace    (XHashMap_PT map, void *key, void *value);

/* O(lgN) */
extern bool         xhashmap_remove              (XHashMap_PT map, void *key);
extern bool         xhashmap_remove_all          (XHashMap_PT map, void *key);

/* O(NlgN) */
extern void         xhashmap_clear               (XHashMap_PT map);
extern void         xhashmap_clear_apply         (XHashMap_PT map, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern void         xhashmap_deep_clear          (XHashMap_PT map);

/* O(NlgN) */
extern void         xhashmap_free                (XHashMap_PT *map);
extern void         xhashmap_free_apply          (XHashMap_PT *map, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern void         xhashmap_deep_free           (XHashMap_PT *map);

/* O(1) */
extern bool         xhashmap_swap                (XHashMap_PT map1, XHashMap_PT map2);

/* O(NlgN) : all map interfaces */
extern int          xhashmap_map                 (XHashMap_PT map, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool         xhashmap_map_break_if_true   (XHashMap_PT map, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool         xhashmap_map_break_if_false  (XHashMap_PT map, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(lgN) */
extern void*        xhashmap_get                 (XHashMap_PT map, void *key);
extern XSList_PT    xhashmap_get_all             (XHashMap_PT map, void *key);

/* O(lgN) */
extern bool         xhashmap_find                (XHashMap_PT map, void *key);

/* O(1) */
extern int          xhashmap_size                (XHashMap_PT map);
extern bool         xhashmap_is_empty            (XHashMap_PT map);

/* O(1) */
extern double       xhashmap_loading_factor      (XHashMap_PT map);

/* O(NlgN) */
extern int          xhashmap_key_size            (XHashMap_PT map, void *key);

#ifdef __cplusplus
}
#endif

#endif
