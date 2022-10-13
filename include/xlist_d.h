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

#ifndef XDLIST_INCLUDED
#define XDLIST_INCLUDED

#include <stdbool.h>
#include "xlist_d_raw.h"
#include "xarray_pointer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XDList*       XDList_PT;

/* O(1) */
extern XDList_PT    xdlist_new                        (void);

/* O(1) */
extern int          xdlist_vload_repeat               (XDList_PT dlist, void *value, ...);

/* O(N) */
extern int          xdlist_vload_unique               (XDList_PT dlist, void *value, ...);
extern int          xdlist_vload_unique_if            (XDList_PT dlist, int (*cmp)(void *value1, void *value2, void *cl), void *cl, void *value, ...);

/* O(N) */
extern int          xdlist_vload_replace_if           (XDList_PT dlist, int (*cmp)(void *value1, void *value2, void *cl), void *cl, void *value, ...);

/* O(1) */
extern int          xdlist_aload_repeat               (XDList_PT dlist, XPArray_PT xs);

/* O(N) */
extern int          xdlist_aload_unique               (XDList_PT dlist, XPArray_PT xs);
extern int          xdlist_aload_unique_if            (XDList_PT dlist, XPArray_PT xs, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern int          xdlist_aload_replace_if           (XDList_PT dlist, XPArray_PT xs, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern XDList_PT    xdlist_copy                       (XDList_PT dlist);
extern XDList_PT    xdlist_copy_equal_if              (XDList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XDList_PT    xdlist_copy_apply_if              (XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XDList_PT    xdlist_copyn                      (XDList_PT dlist, int count);
extern XDList_PT    xdlist_copyn_equal_if             (XDList_PT dlist, int count, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XDList_PT    xdlist_copyn_apply_if             (XDList_PT dlist, int count, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XDList_PT    xdlist_deep_copy                  (XDList_PT dlist, int value_size);
extern XDList_PT    xdlist_deep_copy_equal_if         (XDList_PT dlist, int value_size, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XDList_PT    xdlist_deep_copy_apply_if         (XDList_PT dlist, int value_size, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XDList_PT    xdlist_deep_copyn                 (XDList_PT dlist, int value_size, int count);
extern XDList_PT    xdlist_deep_copyn_equal_if        (XDList_PT dlist, int value_size, int count, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XDList_PT    xdlist_deep_copyn_apply_if        (XDList_PT dlist, int value_size, int count, bool (*apply)(void *value, void *cl), void *cl);

/* O(1) */
extern bool         xdlist_push_front_repeat          (XDList_PT dlist, void *value);
extern void*        xdlist_pop_front                  (XDList_PT dlist);

/* O(1) */
extern bool         xdlist_push_back_repeat           (XDList_PT dlist, void *value);
extern void*        xdlist_pop_back                   (XDList_PT dlist);

/* O(1) */
extern void*        xdlist_front                      (XDList_PT dlist);
extern void*        xdlist_back                       (XDList_PT dlist);

/* O(1) */
extern XRDList_PT   xdlist_front_node                 (XDList_PT dlist);
extern XRDList_PT   xdlist_back_node                  (XDList_PT dlist);

/* O(N) */
extern int          xdlist_push_front_unique          (XDList_PT dlist, void *value);
extern int          xdlist_push_front_unique_if       (XDList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern int          xdlist_push_front_replace_if      (XDList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern int          xdlist_push_front_deep_replace_if (XDList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern int          xdlist_push_back_unique           (XDList_PT dlist, void *value);
extern int          xdlist_push_back_unique_if        (XDList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern int          xdlist_push_back_replace_if       (XDList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern int          xdlist_push_back_deep_replace_if  (XDList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(1) */
extern bool         xdlist_insert_before              (XDList_PT dlist, XRDList_PT node, void *value);
extern bool         xdlist_insert_after               (XDList_PT dlist, XRDList_PT node, void *value);

/* O(1) */
extern bool         xdlist_remove_before              (XDList_PT dlist, XRDList_PT node, void **value);
extern bool         xdlist_remove_after               (XDList_PT dlist, XRDList_PT node, void **value);

/* O(N) */
extern bool         xdlist_remove                     (XDList_PT dlist, void *value);
extern int          xdlist_remove_all                 (XDList_PT dlist, void *value);
extern int          xdlist_remove_apply_if            (XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);
extern bool         xdlist_remove_apply_break_if      (XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);
extern int          xdlist_remove_equal_if            (XDList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern bool         xdlist_remove_equal_break_if      (XDList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern bool         xdlist_deep_remove                (XDList_PT dlist, void **value);
extern int          xdlist_deep_remove_apply_if       (XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);
extern bool         xdlist_deep_remove_apply_break_if (XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);
extern int          xdlist_deep_remove_equal_if       (XDList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern bool         xdlist_deep_remove_equal_break_if (XDList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern void         xdlist_free                       (XDList_PT *pdlist);
extern void         xdlist_free_apply                 (XDList_PT *pdlist, bool (*apply)(void *value, void *cl), void *cl);
extern void         xdlist_deep_free                  (XDList_PT *pdlist);

/* O(N) */
extern void         xdlist_clear                      (XDList_PT dlist);
extern void         xdlist_clear_apply                (XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);
extern void         xdlist_deep_clear                 (XDList_PT dlist);

/* O(1) */
extern bool         xdlist_swap                       (XDList_PT dlist1, XDList_PT dlist2);

/* O(1) */
extern int          xdlist_merge                      (XDList_PT dlist1, XDList_PT *pdlist2);

/* O(N) */
extern void         xdlist_reverse                    (XDList_PT dlist);

/* O(N) */
extern int          xdlist_map                        (XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);
extern bool         xdlist_map_break_if_true          (XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);
extern bool         xdlist_map_break_if_false         (XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XRDList_PT   xdlist_find                       (XDList_PT dlist, void *value);
extern XRDList_PT   xdlist_find_equal_if              (XDList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XRDList_PT   xdlist_find_apply_if              (XDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XPArray_PT   xdlist_to_array                   (XDList_PT dlist);

/* O(1) */
extern int          xdlist_size                       (XDList_PT dlist);
extern bool         xdlist_is_empty                   (XDList_PT dlist);

/* O(NlogN) */
extern bool         xdlist_unique                     (XDList_PT dlist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern bool         xdlist_deep_unique                (XDList_PT dlist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(NlogN) */
extern bool         xdlist_sort                       (XDList_PT dlist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern bool         xdlist_is_sorted                  (XDList_PT dlist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

#ifdef __cplusplus
}
#endif

#endif
