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

#ifndef XKVHASHTAB_INCLUDED
#define XKVHASHTAB_INCLUDED

#include <stdbool.h>
#include "xlist_d_raw.h"
#include "xarray_pointer.h"
#include "xqueue_deque.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XKVHashtab* XKVHashtab_PT;

/* O(hint) */
extern XKVHashtab_PT     xkvhashtab_new                (int hint, int (*hash)(void *key), int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern XKVHashtab_PT     xkvhashtab_copy               (XKVHashtab_PT table);
extern XKVHashtab_PT     xkvhashtab_deep_copy          (XKVHashtab_PT table, int key_size, int value_size);

/* O(1) */
extern int               xkvhashtab_put_repeat         (XKVHashtab_PT table, void *key, void *value);

/* O(N/Slot) */
extern int               xkvhashtab_put_unique         (XKVHashtab_PT table, void *key, void *value);

/* O(N/Slot) */
extern int               xkvhashtab_put_replace        (XKVHashtab_PT table, void *key, void *value);
extern int               xkvhashtab_put_deep_replace   (XKVHashtab_PT table, void *key, void *value);

/* O(N/Slot) */
extern XRDList_PT        xkvhashtab_get                (XKVHashtab_PT table, void *key);
extern bool              xkvhashtab_find               (XKVHashtab_PT table, void *key);

/* O(N/Slot) */
extern XDeque_PT         xkvhashtab_get_all            (XKVHashtab_PT table, void *key);

/* O(N/Slot) */
extern bool              xkvhashtab_remove             (XKVHashtab_PT table, void *key);
extern int               xkvhashtab_remove_all         (XKVHashtab_PT table, void *key);

/* O(N) */
extern void              xkvhashtab_clear              (XKVHashtab_PT table);
extern void              xkvhashtab_clear_apply        (XKVHashtab_PT table, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern void              xkvhashtab_deep_clear         (XKVHashtab_PT table);

/* O(N) */
extern void              xkvhashtab_free               (XKVHashtab_PT *table);
extern void              xkvhashtab_free_apply         (XKVHashtab_PT *table, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern void              xkvhashtab_deep_free          (XKVHashtab_PT *table);

/* O(N) */
extern int               xkvhashtab_map                    (XKVHashtab_PT table, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool              xkvhashtab_map_break_if_true      (XKVHashtab_PT table, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool              xkvhashtab_map_break_if_false     (XKVHashtab_PT table, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(N) */
extern int               xkvhashtab_map_key                (XKVHashtab_PT table, bool (*apply)(void *key, void *cl), void *cl);
extern bool              xkvhashtab_map_key_break_if_true  (XKVHashtab_PT table, bool (*apply)(void *key, void *cl), void *cl);
extern bool              xkvhashtab_map_key_break_if_false (XKVHashtab_PT table, bool (*apply)(void *key, void *cl), void *cl);

/* O(1) */
extern bool              xkvhashtab_swap               (XKVHashtab_PT table1, XKVHashtab_PT table2);

/* O(1) */
extern int               xkvhashtab_size               (XKVHashtab_PT table);
extern bool              xkvhashtab_is_empty           (XKVHashtab_PT table);

/* O(1) */
extern double            xkvhashtab_loading_factor     (XKVHashtab_PT table);

/* O(1) */
extern int               xkvhashtab_bucket_size        (XKVHashtab_PT table);
extern int               xkvhashtab_max_bucket_size    (XKVHashtab_PT table);

/* O(1) */
extern int               xkvhashtab_elems_in_bucket    (XKVHashtab_PT table, int bucket);

/* O(N/Slot) */
extern int               xkvhashtab_key_size           (XKVHashtab_PT table, void *key);

/* O(N) */
extern bool              xkvhashtab_resize             (XKVHashtab_PT table, int new_hint);

#ifdef __cplusplus
}
#endif

#endif
