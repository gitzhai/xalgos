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

#ifndef XRSLIST_INCLUDED
#define XRSLIST_INCLUDED

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XRSList*  XRSList_PT;

/* O(1) */
extern XRSList_PT  xrslist_new                        (void *value);

/* O(N) */
extern XRSList_PT  xrslist_copy                       (XRSList_PT slist);
extern XRSList_PT  xrslist_deep_copy                  (XRSList_PT slist, int value_size);

/* O(1) */
extern bool        xrslist_push_front_repeat          (XRSList_PT *pslist, void *value);
extern void*       xrslist_pop_front                  (XRSList_PT *pslist);

/* O(N) */
extern int         xrslist_push_front_unique          (XRSList_PT *pslist, void *value);
extern int         xrslist_push_front_unique_if       (XRSList_PT *pslist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern int         xrslist_push_front_replace_if      (XRSList_PT *pslist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern int         xrslist_push_front_deep_replace_if (XRSList_PT *pslist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(1) */
extern void*       xrslist_front                      (XRSList_PT slist);

/* O(1) */
extern bool        xrslist_insert_after               (XRSList_PT slist, XRSList_PT node, void *value);
extern bool        xrslist_remove_after               (XRSList_PT slist, XRSList_PT node, void **value);

/* O(N) */
extern bool        xrslist_remove                     (XRSList_PT *pslist, void *value);
extern int         xrslist_remove_all                 (XRSList_PT *pslist, void *value);
extern int         xrslist_remove_apply_if            (XRSList_PT *pslist, bool (*apply)(void *value, void *cl), void *cl);
extern bool        xrslist_remove_apply_break_if      (XRSList_PT *pslist, bool (*apply)(void *value, void *cl), void *cl);
extern int         xrslist_remove_equal_if            (XRSList_PT *pslist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern bool        xrslist_remove_equal_break_if      (XRSList_PT *pslist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern bool        xrslist_deep_remove                (XRSList_PT *pslist, void **value);
extern int         xrslist_deep_remove_apply_if       (XRSList_PT *pslist, bool (*apply)(void *value, void *cl), void *cl);
extern bool        xrslist_deep_remove_apply_break_if (XRSList_PT *pslist, bool (*apply)(void *value, void *cl), void *cl);
extern int         xrslist_deep_remove_equal_if       (XRSList_PT *pslist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern bool        xrslist_deep_remove_equal_break_if (XRSList_PT *pslist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern void        xrslist_free                       (XRSList_PT *pslist);
extern void        xrslist_free_apply                 (XRSList_PT *pslist, bool (*apply)(void *value, void *cl), void *cl);
extern void        xrslist_deep_free                  (XRSList_PT *pslist);

/* O(1) */
extern bool        xrslist_swap                       (XRSList_PT *pslist1, XRSList_PT *pslist2);

/* O(N) */
extern void        xrslist_reverse                    (XRSList_PT *pslist);

/* O(N) */
extern int         xrslist_map                        (XRSList_PT slist, bool (*apply)(void *value, void *cl), void *cl);
extern bool        xrslist_map_break_if_true          (XRSList_PT slist, bool (*apply)(void *value, void *cl), void *cl);
extern bool        xrslist_map_break_if_false         (XRSList_PT slist, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XRSList_PT  xrslist_find                       (XRSList_PT slist, void *value);
extern XRSList_PT  xrslist_find_equal_if              (XRSList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XRSList_PT  xrslist_find_apply_if              (XRSList_PT slist, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern bool        xrslist_merge                      (XRSList_PT slist1, XRSList_PT *pslist2);

/* O(N) */
extern int         xrslist_size                       (XRSList_PT slist);

/* O(NlgN) */
extern bool        xrslist_sort                       (XRSList_PT *pslist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern bool        xrslist_is_sorted                  (XRSList_PT slist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

#ifdef __cplusplus
}
#endif

#endif
