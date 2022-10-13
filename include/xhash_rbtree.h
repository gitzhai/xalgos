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

#ifndef XRBTREEHASH_INCLUDED
#define XRBTREEHASH_INCLUDED

#include <stdbool.h>
#include "xlist_s.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XRBTreeHash* XRBTreeHash_PT;

/* O(1) */
extern XRBTreeHash_PT    xrbtreehash_new                (int hint, int (*hash)(void *key), int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(NlgN) */
extern XRBTreeHash_PT    xrbtreehash_copy               (XRBTreeHash_PT table);
extern XRBTreeHash_PT    xrbtreehash_deep_copy          (XRBTreeHash_PT table, int key_size, int value_size);

/* O(lgN) */
extern bool              xrbtreehash_put_repeat         (XRBTreeHash_PT table, void *key, void *value);
extern bool              xrbtreehash_put_unique         (XRBTreeHash_PT table, void *key, void *value);
extern bool              xrbtreehash_put_replace        (XRBTreeHash_PT table, void *key, void *value);
extern bool              xrbtreehash_put_deep_replace   (XRBTreeHash_PT table, void *key, void *value);

/* O(lgN) */
extern void*             xrbtreehash_get                (XRBTreeHash_PT table, void *key);
extern bool              xrbtreehash_find               (XRBTreeHash_PT table, void *key);

/* O(NlgN) */
extern XSList_PT         xrbtreehash_get_all            (XRBTreeHash_PT table, void *key);

/* O(lgN) */
extern bool              xrbtreehash_remove             (XRBTreeHash_PT table, void *key);
extern bool              xrbtreehash_deep_remove        (XRBTreeHash_PT table, void *key);

/* O(NlgN) */
extern int               xrbtreehash_remove_all         (XRBTreeHash_PT table, void *key);
extern int               xrbtreehash_deep_remove_all    (XRBTreeHash_PT table, void *key);

/* O(NlgN) */
extern void              xrbtreehash_clear              (XRBTreeHash_PT table);
extern void              xrbtreehash_clear_apply        (XRBTreeHash_PT table, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern void              xrbtreehash_deep_clear         (XRBTreeHash_PT table);

/* O(NlgN) */
extern void              xrbtreehash_free               (XRBTreeHash_PT *table);
extern void              xrbtreehash_free_apply         (XRBTreeHash_PT *table, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern void              xrbtreehash_deep_free          (XRBTreeHash_PT *table);

/* O(NlgN) : all map interfaces */
extern int               xrbtreehash_map                (XRBTreeHash_PT table, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool              xrbtreehash_map_break_if_true  (XRBTreeHash_PT table, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool              xrbtreehash_map_break_if_false (XRBTreeHash_PT table, bool (*apply)(void *key, void **value, void *cl), void *cl);

extern int               xrbtreehash_map_key                (XRBTreeHash_PT table, bool (*apply)(void *key, void *cl), void *cl);
extern bool              xrbtreehash_map_key_break_if_true  (XRBTreeHash_PT table, bool (*apply)(void *key, void *cl), void *cl);
extern bool              xrbtreehash_map_key_break_if_false (XRBTreeHash_PT table, bool (*apply)(void *key, void *cl), void *cl);

/* O(1) */
extern bool              xrbtreehash_swap               (XRBTreeHash_PT table1, XRBTreeHash_PT table2);

/* O(1) */
extern int               xrbtreehash_size               (XRBTreeHash_PT table);
extern bool              xrbtreehash_is_empty           (XRBTreeHash_PT table);

/* O(NlgN) */
extern int               xrbtreehash_key_size           (XRBTreeHash_PT table, void *key);

/* O(1) */
extern double            xrbtreehash_loading_factor     (XRBTreeHash_PT table);

/* O(1) */
extern int               xrbtreehash_bucket_size        (XRBTreeHash_PT table);

/* O(1) */
extern int               xrbtreehash_elems_in_bucket    (XRBTreeHash_PT table, int bucket);

#ifdef __cplusplus
}
#endif

#endif
