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

#ifndef XKVSLIST_INCLUDED
#define XKVSLIST_INCLUDED

#include <stdbool.h>
#include "xpair.h"
#include "xarray_pointer.h"
#include "xlist_s_raw.h"
#include "xlist_s.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XSList_PT   XKVSList_PT;

/* O(1) */
extern XKVSList_PT  xkvslist_new                            (void);

/* O(1) */
extern int          xkvslist_vload_repeat                   (XKVSList_PT slist, void *key, ...);

/* O(N) */
extern int          xkvslist_vload_unique                   (XKVSList_PT slist, void *key, ...);
extern int          xkvslist_vload_unique_if                (XKVSList_PT slist, int (*cmp)(void *key1, void *key2, void *cl), void *cl, void *key, ...);

/* O(N) */
extern int          xkvslist_vload_replace                  (XKVSList_PT slist, void *key, ...);
extern int          xkvslist_vload_replace_if               (XKVSList_PT slist, int (*cmp)(void *key1, void *key2, void *cl), void *cl, void *key, ...);

/* O(1) */
extern int          xkvslist_aload_repeat                   (XKVSList_PT slist, XPArray_PT xs);

/* O(N) */
extern int          xkvslist_aload_unique                   (XKVSList_PT slist, XPArray_PT xs);
extern int          xkvslist_aload_unique_if                (XKVSList_PT slist, XPArray_PT xs, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern int          xkvslist_aload_replace                  (XKVSList_PT slist, XPArray_PT xs);
extern int          xkvslist_aload_replace_if               (XKVSList_PT slist, XPArray_PT xs, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern XKVSList_PT  xkvslist_copy                           (XKVSList_PT slist);
extern XKVSList_PT  xkvslist_copy_equal_if                  (XKVSList_PT slist, void *key, int (*cmp)(void *key1, void *key2, void *cl), void *cl);
extern XKVSList_PT  xkvslist_copy_apply_if                  (XKVSList_PT slist, bool (*apply)(void *key, void *value, void *cl), void *cl);
extern XKVSList_PT  xkvslist_copy_apply_key_if              (XKVSList_PT slist, bool (*apply)(void *key, void *cl), void *cl);

/* O(N) */
extern XKVSList_PT  xkvslist_copyn                          (XKVSList_PT slist, int count);
extern XKVSList_PT  xkvslist_copyn_equal_if                 (XKVSList_PT slist, int count, void *key, int (*cmp)(void *key1, void *key2, void *cl), void *cl);
extern XKVSList_PT  xkvslist_copyn_apply_if                 (XKVSList_PT slist, int count, bool (*apply)(void *key, void *value, void *cl), void *cl);
extern XKVSList_PT  xkvslist_copyn_apply_key_if             (XKVSList_PT slist, int count, bool (*apply)(void *key, void *cl), void *cl);

/* O(N) */
extern XKVSList_PT  xkvslist_deep_copy                      (XKVSList_PT slist, int key_size, int value_size);
extern XKVSList_PT  xkvslist_deep_copy_equal_if             (XKVSList_PT slist, int key_size, int value_size, void *key, int (*cmp)(void *key1, void *key2, void *cl), void *cl);
extern XKVSList_PT  xkvslist_deep_copy_apply_if             (XKVSList_PT slist, int key_size, int value_size, bool (*apply)(void *key, void *value, void *cl), void *cl);
extern XKVSList_PT  xkvslist_deep_copy_apply_key_if         (XKVSList_PT slist, int key_size, int value_size, bool (*apply)(void *key, void *cl), void *cl);

/* O(N) */
extern XKVSList_PT  xkvslist_deep_copyn                     (XKVSList_PT slist, int key_size, int value_size, int count);
extern XKVSList_PT  xkvslist_deep_copyn_equal_if            (XKVSList_PT slist, int key_size, int value_size, int count, void *key, int (*cmp)(void *key1, void *key2, void *cl), void *cl);
extern XKVSList_PT  xkvslist_deep_copyn_apply_if            (XKVSList_PT slist, int key_size, int value_size, int count, bool (*apply)(void *key, void *value, void *cl), void *cl);
extern XKVSList_PT  xkvslist_deep_copyn_apply_key_if        (XKVSList_PT slist, int key_size, int value_size, int count, bool (*apply)(void *key, void *cl), void *cl);

/* O(1) */
extern bool         xkvslist_push_front_repeat              (XKVSList_PT slist, void *key,  void *value);
extern bool         xkvslist_pop_front                      (XKVSList_PT slist, void **key, void **value);

/* O(1) */
extern bool         xkvslist_push_back_repeat               (XKVSList_PT slist, void *key,  void *value);

/* O(1) */
extern XPair_PT     xkvslist_front                          (XKVSList_PT slist);
extern XPair_PT     xkvslist_back                           (XKVSList_PT slist);

/* O(1) */
extern XRSList_PT   xkvslist_front_node                     (XKVSList_PT slist);
extern XRSList_PT   xkvslist_back_node                      (XKVSList_PT slist);

/* O(N) */
extern int          xkvslist_push_front_unique              (XKVSList_PT slist, void *key, void *value);
extern int          xkvslist_push_front_unique_if           (XKVSList_PT slist, void *key, void *value, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern int          xkvslist_push_front_replace             (XKVSList_PT slist, void *key, void *value);
extern int          xkvslist_push_front_replace_if          (XKVSList_PT slist, void *key, void *value, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern int          xkvslist_push_front_deep_replace        (XKVSList_PT slist, void *key, void *value);
extern int          xkvslist_push_front_deep_replace_if     (XKVSList_PT slist, void *key, void *value, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern int          xkvslist_push_back_unique               (XKVSList_PT slist, void *key, void *value);
extern int          xkvslist_push_back_unique_if            (XKVSList_PT slist, void *key, void *value, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern int          xkvslist_push_back_replace              (XKVSList_PT slist, void *key, void *value);
extern int          xkvslist_push_back_replace_if           (XKVSList_PT slist, void *key, void *value, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern int          xkvslist_push_back_deep_replace         (XKVSList_PT slist, void *key, void *value);
extern int          xkvslist_push_back_deep_replace_if      (XKVSList_PT slist, void *key, void *value, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(1) */
extern bool         xkvslist_insert_after                   (XKVSList_PT slist, XRSList_PT node, void *key, void *value);

/* O(1) */
extern bool         xkvslist_remove_after                   (XKVSList_PT slist, XRSList_PT node, void **key, void **value);

/* O(N) */
extern bool         xkvslist_remove                         (XKVSList_PT slist, void *key);
extern int          xkvslist_remove_all                     (XKVSList_PT slist, void *key);
extern int          xkvslist_remove_apply_if                (XKVSList_PT slist, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool         xkvslist_remove_apply_break_if          (XKVSList_PT slist, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int          xkvslist_remove_apply_key_if            (XKVSList_PT slist, bool (*apply)(void *key, void *cl), void *cl);
extern bool         xkvslist_remove_apply_key_break_if      (XKVSList_PT slist, bool (*apply)(void *key, void *cl), void *cl);
extern int          xkvslist_remove_equal_if                (XKVSList_PT slist, void *key, int (*cmp)(void *key1, void *key2, void *cl), void *cl);
extern bool         xkvslist_remove_equal_break_if          (XKVSList_PT slist, void *key, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern bool         xkvslist_deep_remove                    (XKVSList_PT slist, void **key);
extern int          xkvslist_deep_remove_apply_if           (XKVSList_PT slist, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool         xkvslist_deep_remove_apply_break_if     (XKVSList_PT slist, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int          xkvslist_deep_remove_apply_key_if       (XKVSList_PT slist, bool (*apply)(void *key, void *cl), void *cl);
extern bool         xkvslist_deep_remove_apply_key_break_if (XKVSList_PT slist, bool (*apply)(void *key, void *cl), void *cl);
extern int          xkvslist_deep_remove_equal_if           (XKVSList_PT slist, void *key, int (*cmp)(void *key1, void *key2, void *cl), void *cl);
extern bool         xkvslist_deep_remove_equal_break_if     (XKVSList_PT slist, void *key, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern void         xkvslist_free                           (XKVSList_PT *pslist);
extern void         xkvslist_free_apply                     (XKVSList_PT *pslist, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern void         xkvslist_deep_free                      (XKVSList_PT *pslist);

/* O(N) */
extern void         xkvslist_clear                          (XKVSList_PT slist);
extern void         xkvslist_clear_apply                    (XKVSList_PT slist, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern void         xkvslist_deep_clear                     (XKVSList_PT slist);

/* O(1) */
extern bool         xkvslist_swap                           (XKVSList_PT slist1, XKVSList_PT slist2);

/* O(1) */
extern int          xkvslist_merge                          (XKVSList_PT slist1, XKVSList_PT *pslist2);

/* O(N) */
extern void         xkvslist_reverse                        (XKVSList_PT slist);

/* O(N) */
extern int          xkvslist_map                            (XKVSList_PT slist, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool         xkvslist_map_break_if_true              (XKVSList_PT slist, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool         xkvslist_map_break_if_false             (XKVSList_PT slist, bool (*apply)(void *key, void **value, void *cl), void *cl);

/* O(N) */
extern int          xkvslist_map_key                        (XKVSList_PT slist, bool (*apply)(void *key, void *cl), void *cl);
extern bool         xkvslist_map_key_break_if_true          (XKVSList_PT slist, bool (*apply)(void *key, void *cl), void *cl);
extern bool         xkvslist_map_key_break_if_false         (XKVSList_PT slist, bool (*apply)(void *key, void *cl), void *cl);

/* O(N) */
extern XRSList_PT   xkvslist_find                           (XKVSList_PT slist, void *key);
extern XRSList_PT   xkvslist_find_equal_if                  (XKVSList_PT slist, void *key, int (*cmp)(void *key1, void *key2, void *cl), void *cl);
extern XRSList_PT   xkvslist_find_apply_if                  (XKVSList_PT slist, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern XRSList_PT   xkvslist_find_apply_key_if              (XKVSList_PT slist, bool (*apply)(void *key, void *cl), void *cl);

/* O(N) */
extern XPArray_PT   xkvslist_to_array                       (XKVSList_PT slist);
extern XPArray_PT   xkvslist_keys_to_array                  (XKVSList_PT slist);
extern XPArray_PT   xkvslist_values_to_array                (XKVSList_PT slist);

/* O(1) */
extern int          xkvslist_size                           (XKVSList_PT slist);
extern bool         xkvslist_is_empty                       (XKVSList_PT slist);

/* O(NlogN) */
extern bool         xkvslist_unique                         (XKVSList_PT slist, int (*cmp)(void *key1, void *key2, void *cl), void *cl);
extern bool         xkvslist_deep_unique                    (XKVSList_PT slist, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(NlogN) */
extern bool         xkvslist_sort                           (XKVSList_PT slist, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(N) */
extern bool         xkvslist_is_sorted                      (XKVSList_PT slist, int (*cmp)(void *key1, void *key2, void *cl), void *cl);

#ifdef __cplusplus
}
#endif

#endif
