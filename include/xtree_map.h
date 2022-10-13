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

#ifndef XMAP_INCLUDED
#define XMAP_INCLUDED

#include "xlist_s.h"
#include "xtree_redblack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XRBTree_PT XMap_PT;

/* O(1) */
extern XMap_PT    xmap_new               (int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(NlgN) */
extern XMap_PT    xmap_copy              (XMap_PT map);
extern XMap_PT    xmap_deep_copy         (XMap_PT map, int key_size, int value_size);

/* O(lgN) */
extern bool       xmap_put_repeat        (XMap_PT map, void *key, void *value);
extern bool       xmap_put_unique        (XMap_PT map, void *key, void *value);
extern bool       xmap_put_replace       (XMap_PT map, void *key, void *value, void **old_value);
extern bool       xmap_put_deep_replace  (XMap_PT map, void *key, void *value);

/* O(lgN) */
extern void*      xmap_select            (XMap_PT map, int k);

/* O(lgN) */
extern void*      xmap_get               (XMap_PT map, void *key);

/* O(lgN) */
extern bool       xmap_find              (XMap_PT map, void *key);
extern bool       xmap_find_replace      (XMap_PT map, void *key, void *value, void **old_value);
extern bool       xmap_find_deep_replace (XMap_PT map, void *key, void *value);

/* O(NlgN) */
extern void       xmap_clear             (XMap_PT map);
extern void       xmap_clear_apply       (XMap_PT map, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern void       xmap_deep_clear        (XMap_PT map);

/* O(NlgN) */
extern void       xmap_free              (XMap_PT *pmap);
extern void       xmap_free_apply        (XMap_PT *pmap, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern void       xmap_deep_free         (XMap_PT *pmap);

/* O(lgN) */
extern void       xmap_remove            (XMap_PT map, void *key);

/* O(lgN) */
extern void       xmap_remove_save       (XMap_PT map, void *key, void **value);
extern void       xmap_deep_remove       (XMap_PT map, void *key);

/* O(N) */
extern int        xmap_map               (XMap_PT map, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool       xmap_map_break_if_true (XMap_PT map, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool       xmap_map_break_if_false(XMap_PT map, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(1) */
extern bool       xmap_swap              (XMap_PT map1, XMap_PT map2);

/* O(1) */
extern int        xmap_size              (XMap_PT map);
extern bool       xmap_is_empty          (XMap_PT map);

#ifdef __cplusplus
}
#endif

#endif
