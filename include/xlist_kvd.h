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

#ifndef XKVDLIST_INCLUDED
#define XKVDLIST_INCLUDED

#include <stdbool.h>
#include "xarray_pointer.h"
#include "xpair.h"
#include "xlist_d_raw.h"
#include "xlist_d.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XDList_PT   XKVDList_PT;

/* O(1) */
extern XKVDList_PT  xkvdlist_new                              (void);

/* O(1) */
extern int          xkvdlist_vload_repeat                     (XKVDList_PT dlist, void *key, ...);

/* O(N) */
extern int          xkvdlist_vload_unique                     (XKVDList_PT dlist, void *key, ...);
extern int          xkvdlist_vload_unique_if                  (XKVDList_PT dlist, int (*cmp)(void *key1, void *key2, void *cl), void *cl, void *key, ...);

/* O(N) */
extern int          xkvdlist_vload_replace                    (XKVDList_PT dlist, void *key, ...);
extern int          xkvdlist_vload_replace_if                 (XKVDList_PT dlist, int (*cmp)(void *key1, void *key2, void *cl), void *cl, void *key, ...);

/* O(1) */
extern int          xkvdlist_aload_repeat                     (XKVDList_PT dlist, XPArray_PT xs);

/* O(N) */
extern int          xkvdlist_aload_unique                     (XKVDList_PT dlist, XPArray_PT xs);
extern int          xkvdlist_aload_unique_if                  (XKVDList_PT dlist, XPArray_PT xs, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern int          xkvdlist_aload_replace                    (XKVDList_PT dlist, XPArray_PT xs);
extern int          xkvdlist_aload_replace_if                 (XKVDList_PT dlist, XPArray_PT xs, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern XKVDList_PT  xkvdlist_copy                             (XKVDList_PT dlist);
extern XKVDList_PT  xkvdlist_copy_equal_if                    (XKVDList_PT dlist, void *key, int (*cmp)(void *key1, void *key2, void *cl), void *cl);
extern XKVDList_PT  xkvdlist_copy_apply_if                    (XKVDList_PT dlist, bool (*apply)(void *key, void *value, void *cl), void *cl);
extern XKVDList_PT  xkvdlist_copy_apply_key_if                (XKVDList_PT dlist, bool (*apply)(void *key, void *cl), void *cl);

/* O(N) */
extern XKVDList_PT  xkvdlist_copyn                            (XKVDList_PT dlist, int count);
extern XKVDList_PT  xkvdlist_copyn_equal_if                   (XKVDList_PT dlist, int count, void *key, int (*cmp)(void *key1, void *key2, void *cl), void *cl);
extern XKVDList_PT  xkvdlist_copyn_apply_if                   (XKVDList_PT dlist, int count, bool (*apply)(void *key, void *value, void *cl), void *cl);
extern XKVDList_PT  xkvdlist_copyn_apply_key_if               (XKVDList_PT dlist, int count, bool (*apply)(void *key, void *cl), void *cl);

/* O(N) */
extern XKVDList_PT  xkvdlist_deep_copy                        (XKVDList_PT dlist, int key_size, int value_size);
extern XKVDList_PT  xkvdlist_deep_copy_equal_if               (XKVDList_PT dlist, int key_size, int value_size, void *key, int(*cmp)(void *key1, void *key2, void *cl), void *cl);
extern XKVDList_PT  xkvdlist_deep_copy_apply_if               (XKVDList_PT dlist, int key_size, int value_size, bool (*apply)(void *key, void *value, void *cl), void *cl);
extern XKVDList_PT  xkvdlist_deep_copy_apply_key_if           (XKVDList_PT dlist, int key_size, int value_size, bool (*apply)(void *key, void *cl), void *cl);

/* O(N) */
extern XKVDList_PT  xkvdlist_deep_copyn                       (XKVDList_PT dlist, int key_size, int value_size, int count);
extern XKVDList_PT  xkvdlist_deep_copyn_equal_if              (XKVDList_PT dlist, int key_size, int value_size, int count, void *key, int (*cmp)(void *key1, void *key2, void *cl), void *cl);
extern XKVDList_PT  xkvdlist_deep_copyn_apply_if              (XKVDList_PT dlist, int key_size, int value_size, int count, bool (*apply)(void *key, void *value, void *cl), void *cl);
extern XKVDList_PT  xkvdlist_deep_copyn_apply_key_if          (XKVDList_PT dlist, int key_size, int value_size, int count, bool (*apply)(void *key, void *cl), void *cl);

/* O(1) */
extern bool         xkvdlist_push_front_repeat                (XKVDList_PT dlist, void *key,  void *value);
extern bool         xkvdlist_pop_front                        (XKVDList_PT dlist, void **key, void **value);

/* O(1) */
extern bool         xkvdlist_push_back_repeat                 (XKVDList_PT dlist, void *key,  void *value);
extern bool         xkvdlist_pop_back                         (XKVDList_PT dlist, void **key, void **value);

/* O(1) */
extern XPair_PT     xkvdlist_front                            (XKVDList_PT dlist);
extern XPair_PT     xkvdlist_back                             (XKVDList_PT dlist);

/* O(1) */
extern XRDList_PT   xkvdlist_front_node                       (XKVDList_PT dlist);
extern XRDList_PT   xkvdlist_back_node                        (XKVDList_PT dlist);

/* O(N) */
extern int          xkvdlist_push_front_unique                (XKVDList_PT dlist, void *key, void *value);
extern int          xkvdlist_push_front_unique_if             (XKVDList_PT dlist, void *key, void *value, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern int          xkvdlist_push_front_replace               (XKVDList_PT dlist, void *key, void *value);
extern int          xkvdlist_push_front_replace_if            (XKVDList_PT dlist, void *key, void *value, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern int          xkvdlist_push_front_deep_replace          (XKVDList_PT dlist, void *key, void *value);
extern int          xkvdlist_push_front_deep_replace_if       (XKVDList_PT dlist, void *key, void *value, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern int          xkvdlist_push_back_unique                 (XKVDList_PT dlist, void *key, void *value);
extern int          xkvdlist_push_back_unique_if              (XKVDList_PT dlist, void *key, void *value, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern int          xkvdlist_push_back_replace                (XKVDList_PT dlist, void *key, void *value);
extern int          xkvdlist_push_back_replace_if             (XKVDList_PT dlist, void *key, void *value, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern int          xkvdlist_push_back_deep_replace           (XKVDList_PT dlist, void *key, void *value);
extern int          xkvdlist_push_back_deep_replace_if        (XKVDList_PT dlist, void *key, void *value, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(1) */
extern bool         xkvdlist_insert_before                    (XKVDList_PT dlist, XRDList_PT node, void *key, void *value);
extern bool         xkvdlist_insert_after                     (XKVDList_PT dlist, XRDList_PT node, void *key, void *value);

/* O(1) */
extern bool         xkvdlist_remove_before                    (XKVDList_PT dlist, XRDList_PT node, void **key, void **value);
extern bool         xkvdlist_remove_after                     (XKVDList_PT dlist, XRDList_PT node, void **key, void **value);

/* O(N) */
extern bool         xkvdlist_remove                           (XKVDList_PT dlist, void *key);
extern int          xkvdlist_remove_all                       (XKVDList_PT dlist, void *key);
extern int          xkvdlist_remove_apply_if                  (XKVDList_PT dlist, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool         xkvdlist_remove_apply_break_if            (XKVDList_PT dlist, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int          xkvdlist_remove_apply_key_if              (XKVDList_PT dlist, bool (*apply)(void *key, void *cl), void *cl);
extern bool         xkvdlist_remove_apply_key_break_if        (XKVDList_PT dlist, bool (*apply)(void *key, void *cl), void *cl);
extern int          xkvdlist_remove_equal_if                  (XKVDList_PT dlist, void *key, int (*cmp)(void *key1, void *key2, void *cl), void *cl);
extern bool         xkvdlist_remove_equal_break_if            (XKVDList_PT dlist, void *key, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern bool         xkvdlist_deep_remove                      (XKVDList_PT dlist, void **key);
extern int          xkvdlist_deep_remove_apply_if             (XKVDList_PT dlist, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool         xkvdlist_deep_remove_apply_break_if       (XKVDList_PT dlist, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int          xkvdlist_deep_remove_apply_key_if         (XKVDList_PT dlist, bool (*apply)(void *key, void *cl), void *cl);
extern bool         xkvdlist_deep_remove_apply_key_break_if   (XKVDList_PT dlist, bool (*apply)(void *key, void *cl), void *cl);
extern int          xkvdlist_deep_remove_equal_if             (XKVDList_PT dlist, void *key, int (*cmp)(void *key1, void *key2, void *cl), void *cl);
extern bool         xkvdlist_deep_remove_equal_break_if       (XKVDList_PT dlist, void *key, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern void         xkvdlist_free                             (XKVDList_PT *pdlist);
extern void         xkvdlist_free_apply                       (XKVDList_PT *pdlist, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern void         xkvdlist_deep_free                        (XKVDList_PT *pdlist);

/* O(N) */
extern void         xkvdlist_clear                            (XKVDList_PT dlist);
extern void         xkvdlist_clear_apply                      (XKVDList_PT dlist, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern void         xkvdlist_deep_clear                       (XKVDList_PT dlist);

/* O(1) */
extern bool         xkvdlist_swap                             (XKVDList_PT dlist1, XKVDList_PT dlist2);

/* O(1) */
extern int          xkvdlist_merge                            (XKVDList_PT dlist1, XKVDList_PT *pdlist2);

/* O(N) */
extern void         xkvdlist_reverse                          (XKVDList_PT dlist);

/* O(N) */
extern int          xkvdlist_map                              (XKVDList_PT dlist, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool         xkvdlist_map_break_if_true                (XKVDList_PT dlist, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool         xkvdlist_map_break_if_false               (XKVDList_PT dlist, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(N) */
extern int          xkvdlist_map_key                          (XKVDList_PT dlist, bool (*apply)(void *key, void *cl), void *cl);
extern bool         xkvdlist_map_key_break_if_true            (XKVDList_PT dlist, bool (*apply)(void *key, void *cl), void *cl);
extern bool         xkvdlist_map_key_break_if_false           (XKVDList_PT dlist, bool (*apply)(void *key, void *cl), void *cl);

/* O(N) */
extern XRDList_PT   xkvdlist_find                             (XKVDList_PT dlist, void *key);
extern XRDList_PT   xkvdlist_find_equal_if                    (XKVDList_PT dlist, void *key, int (*cmp)(void *key1, void *key2, void *cl), void *cl);
extern XRDList_PT   xkvdlist_find_apply_if                    (XKVDList_PT dlist, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern XRDList_PT   xkvdlist_find_apply_key_if                (XKVDList_PT dlist, bool (*apply)(void *key, void *cl), void *cl);

/* O(N) */
extern XPArray_PT   xkvdlist_to_array                         (XKVDList_PT dlist);
extern XPArray_PT   xkvdlist_keys_to_array                    (XKVDList_PT dlist);
extern XPArray_PT   xkvdlist_values_to_array                  (XKVDList_PT dlist);

/* O(1) */
extern int          xkvdlist_size                             (XKVDList_PT dlist);
extern bool         xkvdlist_is_empty                         (XKVDList_PT dlist);

/* O(NlogN) */
extern bool         xkvdlist_unique                           (XKVDList_PT dlist, int (*cmp)(void *key1, void *key2, void *cl), void *cl);
extern bool         xkvdlist_deep_unique                      (XKVDList_PT dlist, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(NlogN) */
extern bool         xkvdlist_sort                             (XKVDList_PT dlist, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern bool         xkvdlist_is_sorted                        (XKVDList_PT dlist, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

#ifdef __cplusplus
}
#endif

#endif
