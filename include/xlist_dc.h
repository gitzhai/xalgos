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

#ifndef XDCLIST_INCLUDED
#define XDCLIST_INCLUDED

#include <stdbool.h>
#include "xarray_pointer.h"
#include "xlist_d.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XDList_PT       XDCList_PT;

/* O(1) */
extern XDCList_PT    xdclist_new                        (void);

/* O(1) */
extern int           xdclist_vload_repeat               (XDCList_PT dlist, void *value, ...);

/* O(N) */
extern int           xdclist_vload_unique               (XDCList_PT dlist, void *value, ...);
extern int           xdclist_vload_unique_if            (XDCList_PT dlist, int (*cmp)(void *value1, void *value2, void *cl), void *cl, void *value, ...);

/* O(N) */
extern int           xdclist_vload_replace_if           (XDCList_PT dlist, int (*cmp)(void *value1, void *value2, void *cl), void *cl, void *value, ...);

/* O(1) */
extern int           xdclist_aload_repeat               (XDCList_PT dlist, XPArray_PT xs);

/* O(N) */
extern int           xdclist_aload_unique               (XDCList_PT dlist, XPArray_PT xs);
extern int           xdclist_aload_unique_if            (XDCList_PT dlist, XPArray_PT xs, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern int           xdclist_aload_replace_if           (XDCList_PT dlist, XPArray_PT xs, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern XDCList_PT    xdclist_copy                       (XDCList_PT dlist);
extern XDCList_PT    xdclist_copy_equal_if              (XDCList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XDCList_PT    xdclist_copy_apply_if              (XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XDCList_PT    xdclist_copyn                      (XDCList_PT dlist, int count);
extern XDCList_PT    xdclist_copyn_equal_if             (XDCList_PT dlist, int count, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XDCList_PT    xdclist_copyn_apply_if             (XDCList_PT dlist, int count, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XDCList_PT    xdclist_deep_copy                  (XDCList_PT dlist, int value_size);
extern XDCList_PT    xdclist_deep_copy_equal_if         (XDCList_PT dlist, int value_size, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XDCList_PT    xdclist_deep_copy_apply_if         (XDCList_PT dlist, int value_size, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XDCList_PT    xdclist_deep_copyn                 (XDCList_PT dlist, int value_size, int count);
extern XDCList_PT    xdclist_deep_copyn_equal_if        (XDCList_PT dlist, int value_size, int count, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XDCList_PT    xdclist_deep_copyn_apply_if        (XDCList_PT dlist, int value_size, int count, bool (*apply)(void *value, void *cl), void *cl);

/* O(1) */
extern bool          xdclist_push_front_repeat          (XDCList_PT dlist, void *value);
extern void*         xdclist_pop_front                  (XDCList_PT dlist);

/* O(1) */
extern bool          xdclist_push_back_repeat           (XDCList_PT dlist, void *value);
extern void*         xdclist_pop_back                   (XDCList_PT dlist);

/* O(1) */
extern void*         xdclist_front                      (XDCList_PT dlist);
extern void*         xdclist_back                       (XDCList_PT dlist);

/* O(1) */
extern XRDList_PT    xdclist_front_node                 (XDCList_PT dlist);
extern XRDList_PT    xdclist_back_node                  (XDCList_PT dlist);

/* O(N) */
extern int           xdclist_push_front_unique          (XDCList_PT dlist, void *value);
extern int           xdclist_push_front_unique_if       (XDCList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern int           xdclist_push_front_replace_if      (XDCList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern int           xdclist_push_front_deep_replace_if (XDCList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern int           xdclist_push_back_unique           (XDCList_PT dlist, void *value);
extern int           xdclist_push_back_unique_if        (XDCList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern int           xdclist_push_back_replace_if       (XDCList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern int           xdclist_push_back_deep_replace_if  (XDCList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(1) */
extern bool          xdclist_insert_before              (XDCList_PT dlist, XRDList_PT node, void *value);
extern bool          xdclist_insert_after               (XDCList_PT dlist, XRDList_PT node, void *value);

/* O(1) */
extern bool          xdclist_remove_before              (XDCList_PT dlist, XRDList_PT node, void **value);
extern bool          xdclist_remove_after               (XDCList_PT dlist, XRDList_PT node, void **value);

/* O(N) */
extern bool          xdclist_remove                     (XDCList_PT dlist, void *value);
extern int           xdclist_remove_all                 (XDCList_PT dlist, void *value);
extern int           xdclist_remove_apply_if            (XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);
extern bool          xdclist_remove_apply_break_if      (XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);
extern int           xdclist_remove_equal_if            (XDCList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern bool          xdclist_remove_equal_break_if      (XDCList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern bool          xdclist_deep_remove                (XDCList_PT dlist, void **value);
extern int           xdclist_deep_remove_apply_if       (XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);
extern bool          xdclist_deep_remove_apply_break_if (XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);
extern int           xdclist_deep_remove_equal_if       (XDCList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern bool          xdclist_deep_remove_equal_break_if (XDCList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern void          xdclist_free                       (XDCList_PT *pdlist);
extern void          xdclist_free_apply                 (XDCList_PT *pdlist, bool (*apply)(void *value, void *cl), void *cl);
extern void          xdclist_deep_free                  (XDCList_PT *pdlist);

/* O(N) */
extern void          xdclist_clear                      (XDCList_PT dlist);
extern void          xdclist_clear_apply                (XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);
extern void          xdclist_deep_clear                 (XDCList_PT dlist);

/* O(1) */
extern bool          xdclist_swap                       (XDCList_PT dlist1, XDCList_PT dlist2);

/* O(1) */
extern int           xdclist_merge                      (XDCList_PT dlist1, XDCList_PT *pdlist2);

/* O(N) */
extern void          xdclist_reverse                    (XDCList_PT dlist);

/* O(N) */
extern void          xdclist_rotate                     (XDCList_PT dlist, int n);

/* O(N) */
extern int           xdclist_map                        (XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);
extern bool          xdclist_map_break_if_true          (XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);
extern bool          xdclist_map_break_if_false         (XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XRDList_PT    xdclist_find                       (XDCList_PT dlist, void *value);
extern XRDList_PT    xdclist_find_equal_if              (XDCList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XRDList_PT    xdclist_find_apply_if              (XDCList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XPArray_PT    xdclist_to_array                   (XDCList_PT dlist);

/* O(1) */
extern int           xdclist_size                       (XDCList_PT dlist);
extern bool          xdclist_is_empty                   (XDCList_PT dlist);

/* O(NlogN) */
extern bool          xdclist_unique                     (XDCList_PT dlist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern bool          xdclist_deep_unique                (XDCList_PT dlist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(NlogN) */
extern bool          xdclist_sort                       (XDCList_PT dlist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern bool          xdclist_is_sorted                  (XDCList_PT dlist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

#ifdef __cplusplus
}
#endif

#endif
