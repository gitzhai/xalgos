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

#ifndef XSCLIST_INCLUDED
#define XSCLIST_INCLUDED

#include <stdbool.h>
#include "xarray_pointer.h"
#include "xlist_s_raw.h"
#include "xlist_s.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XSList_PT       XSCList_PT;

/* O(1) */
extern XSCList_PT      xsclist_new(void);

/* O(1) */
extern int             xsclist_vload_repeat               (XSCList_PT slist, void *value, ...);

/* O(N) */
extern int             xsclist_vload_unique               (XSCList_PT slist, void *value, ...);
extern int             xsclist_vload_unique_if            (XSCList_PT slist, int (*cmp)(void *value1, void *value2, void *cl), void *cl, void *value, ...);

/* O(N) */
extern int             xsclist_vload_replace_if           (XSCList_PT slist, int (*cmp)(void *value1, void *value2, void *cl), void *cl, void *value, ...);

/* O(1) */
extern int             xsclist_aload_repeat               (XSCList_PT slist, XPArray_PT xs);

/* O(N) */
extern int             xsclist_aload_unique               (XSCList_PT slist, XPArray_PT xs);
extern int             xsclist_aload_unique_if            (XSCList_PT slist, XPArray_PT xs, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern int             xsclist_aload_replace_if           (XSCList_PT slist, XPArray_PT xs, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern XSCList_PT      xsclist_copy                       (XSCList_PT slist);
extern XSCList_PT      xsclist_copy_equal_if              (XSCList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XSCList_PT      xsclist_copy_apply_if              (XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XSCList_PT      xsclist_copyn                      (XSCList_PT slist, int count);
extern XSCList_PT      xsclist_copyn_equal_if             (XSCList_PT slist, int count, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XSCList_PT      xsclist_copyn_apply_if             (XSCList_PT slist, int count, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XSCList_PT      xsclist_deep_copy                  (XSCList_PT slist, int value_size);
extern XSCList_PT      xsclist_deep_copy_equal_if         (XSCList_PT slist, int value_size, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XSCList_PT      xsclist_deep_copy_apply_if         (XSCList_PT slist, int value_size, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XSCList_PT      xsclist_deep_copyn                 (XSCList_PT slist, int value_size, int count);
extern XSCList_PT      xsclist_deep_copyn_equal_if        (XSCList_PT slist, int value_size, int count, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XSCList_PT      xsclist_deep_copyn_apply_if        (XSCList_PT slist, int value_size, int count, bool (*apply)(void *value, void *cl), void *cl);

/* O(1) */
extern bool            xsclist_push_front_repeat          (XSCList_PT slist, void *value);
extern void*           xsclist_pop_front                  (XSCList_PT slist);

/* O(1) */
extern bool            xsclist_push_back_repeat           (XSCList_PT slist, void *value);

/* O(1) */
extern void*           xsclist_front                      (XSCList_PT slist);
extern void*           xsclist_back                       (XSCList_PT slist);

/* O(1) */
extern XRSList_PT      xsclist_front_node                 (XSCList_PT slist);
extern XRSList_PT      xsclist_back_node                  (XSCList_PT slist);

/* O(N) */
extern int             xsclist_push_front_unique          (XSCList_PT slist, void *value);
extern int             xsclist_push_front_unique_if       (XSCList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern int             xsclist_push_front_replace_if      (XSCList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern int             xsclist_push_front_deep_replace_if (XSCList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern int             xsclist_push_back_unique           (XSCList_PT slist, void *value);
extern int             xsclist_push_back_unique_if        (XSCList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern int             xsclist_push_back_replace_if       (XSCList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern int             xsclist_push_back_deep_replace_if  (XSCList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(1) */
extern bool            xsclist_insert_after               (XSCList_PT slist, XRSList_PT node, void *value);

/* O(1) */
extern void*           xsclist_remove_after               (XSCList_PT slist, XRSList_PT node);

/* O(N) */
extern bool            xsclist_remove                     (XSCList_PT slist, void *value);
extern int             xsclist_remove_all                 (XSCList_PT slist, void *value);
extern int             xsclist_remove_apply_if            (XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl);
extern bool            xsclist_remove_apply_break_if      (XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl);
extern int             xsclist_remove_equal_if            (XSCList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern bool            xsclist_remove_equal_break_if      (XSCList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern bool            xsclist_deep_remove                (XSCList_PT slist, void **value);
extern int             xsclist_deep_remove_apply_if       (XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl);
extern bool            xsclist_deep_remove_apply_break_if (XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl);
extern int             xsclist_deep_remove_equal_if       (XSCList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern bool            xsclist_deep_remove_equal_break_if (XSCList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern void            xsclist_free                       (XSCList_PT *pslist);
extern void            xsclist_free_apply                 (XSCList_PT *pslist, bool (*apply)(void *value, void *cl), void *cl);
extern void            xsclist_deep_free                  (XSCList_PT *pslist);

/* O(N) */
extern void            xsclist_clear                      (XSCList_PT slist);
extern void            xsclist_clear_apply                (XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl);
extern void            xsclist_deep_clear                 (XSCList_PT slist);

/* O(1) */
extern bool            xsclist_swap                       (XSCList_PT slist1, XSCList_PT slist2);

/* O(1) */
extern int             xsclist_merge                      (XSCList_PT slist1, XSCList_PT *pslist2);

/* O(N) */
extern void            xsclist_reverse                    (XSCList_PT slist);

/* O(N) */
extern int             xsclist_map                        (XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl);
extern bool            xsclist_map_break_if_true          (XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl);
extern bool            xsclist_map_break_if_false         (XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XRSList_PT      xsclist_find                       (XSCList_PT slist, void *value);
extern XRSList_PT      xsclist_find_equal_if              (XSCList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XRSList_PT      xsclist_find_apply_if              (XSCList_PT slist, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XPArray_PT      xsclist_to_array                   (XSCList_PT slist);

/* O(1) */
extern int             xsclist_size                       (XSCList_PT slist);
extern bool            xsclist_is_empty                   (XSCList_PT slist);

/* O(NlogN) */
extern bool            xsclist_unique                     (XSCList_PT slist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern bool            xsclist_deep_unique                (XSCList_PT slist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(NlogN) */
extern bool            xsclist_sort                       (XSCList_PT slist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern bool            xsclist_is_sorted                  (XSCList_PT slist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

#ifdef __cplusplus
}
#endif

#endif
