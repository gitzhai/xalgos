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

#ifndef XRDLIST_INCLUDED
#define XRDLIST_INCLUDED

#include <stdbool.h>
#include "xarray_pointer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XRDList*  XRDList_PT;

/* O(1) */
extern XRDList_PT   xrdlist_new                        (void *value);

/* O(N) */
extern XRDList_PT   xrdlist_copy                       (XRDList_PT dlist);
extern XRDList_PT   xrdlist_deep_copy                  (XRDList_PT dlist, int value_size);

/* O(1) */
extern bool         xrdlist_push_front_repeat          (XRDList_PT *pdlist, void *value);
extern void*        xrdlist_pop_front                  (XRDList_PT *pdlist);

/* O(N) */
extern int          xrdlist_push_front_unique          (XRDList_PT *pdlist, void *value);
extern int          xrdlist_push_front_unique_if       (XRDList_PT *pdlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern int          xrdlist_push_front_replace_if      (XRDList_PT *pdlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern int          xrdlist_push_front_deep_replace_if (XRDList_PT *pdlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(1) */
extern void*        xrdlist_front                      (XRDList_PT dlist);

/* O(1) */
extern bool         xrdlist_insert_before              (XRDList_PT *pdlist, XRDList_PT node, void *value);
extern bool         xrdlist_insert_after               (XRDList_PT dlist, XRDList_PT node, void *value);

/* O(1) */
extern bool         xrdlist_remove_before              (XRDList_PT *pdlist, XRDList_PT node, void **value);
extern bool         xrdlist_remove_after               (XRDList_PT dlist, XRDList_PT node, void **value);

/* O(N) */
extern bool         xrdlist_remove                     (XRDList_PT *pdlist, void *value);
extern int          xrdlist_remove_all                 (XRDList_PT *pdlist, void *value);
extern int          xrdlist_remove_apply_if            (XRDList_PT *pdlist, bool (*apply)(void *value, void *cl), void *cl);
extern bool         xrdlist_remove_apply_break_if      (XRDList_PT *pdlist, bool (*apply)(void *value, void *cl), void *cl);
extern int          xrdlist_remove_equal_if            (XRDList_PT *pdlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern bool         xrdlist_remove_equal_break_if      (XRDList_PT *pdlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern bool         xrdlist_deep_remove                (XRDList_PT *pdlist, void **value);
extern int          xrdlist_deep_remove_apply_if       (XRDList_PT *pdlist, bool (*apply)(void *value, void *cl), void *cl);
extern bool         xrdlist_deep_remove_apply_break_if (XRDList_PT *pdlist, bool (*apply)(void *value, void *cl), void *cl);
extern int          xrdlist_deep_remove_equal_if       (XRDList_PT *pdlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern bool         xrdlist_deep_remove_equal_break_if (XRDList_PT *pdlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern void         xrdlist_free                       (XRDList_PT *pdlist);
extern void         xrdlist_free_apply                 (XRDList_PT *pdlist, bool (*apply)(void *value, void *cl), void *cl);
extern void         xrdlist_deep_free                  (XRDList_PT *pdlist);

/* O(1) */
extern bool         xrdlist_swap                       (XRDList_PT *pdlist1, XRDList_PT *dlist2);

/* O(N) */
extern void         xrdlist_reverse                    (XRDList_PT *pdlist);

/* O(N) */
extern int          xrdlist_map                        (XRDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);
extern bool         xrdlist_map_break_if_true          (XRDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);
extern bool         xrdlist_map_break_if_false         (XRDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XRDList_PT   xrdlist_find                       (XRDList_PT dlist, void *value);
extern XRDList_PT   xrdlist_find_equal_if              (XRDList_PT dlist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XRDList_PT   xrdlist_find_apply_if              (XRDList_PT dlist, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern bool         xrdlist_merge                      (XRDList_PT dlist1, XRDList_PT *pdlist2);

/* O(1) */
extern int          xrdlist_size                       (XRDList_PT dlist);

/* O(NlgN) */
extern bool         xrdlist_sort                       (XRDList_PT *pdlist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern bool         xrdlist_is_sorted                  (XRDList_PT dlist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

#ifdef __cplusplus
}
#endif

#endif
