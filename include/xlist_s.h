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

#ifndef XSLIST_INCLUDED
#define XSLIST_INCLUDED

#include <stdbool.h>
#include "xlist_s_raw.h"
#include "xarray_pointer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XSList*       XSList_PT;

/* O(1) */
extern XSList_PT      xslist_new                        (void);

/* O(1) */
extern int            xslist_vload_repeat               (XSList_PT slist, void *value, ...);

/* O(N) */
extern int            xslist_vload_unique               (XSList_PT slist, void *value, ...);
extern int            xslist_vload_unique_if            (XSList_PT slist, int (*cmp)(void *value1, void *value2, void *cl), void *cl, void *value, ...);
extern int            xslist_vload_replace_if           (XSList_PT slist, int (*cmp)(void *value1, void *value2, void *cl), void *cl, void *value, ...);

/* O(1) */
extern int            xslist_aload_repeat               (XSList_PT slist, XPArray_PT xs);

/* O(N) */
extern int            xslist_aload_unique               (XSList_PT slist, XPArray_PT xs);
extern int            xslist_aload_unique_if            (XSList_PT slist, XPArray_PT xs, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern int            xslist_aload_replace_if           (XSList_PT slist, XPArray_PT xs, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern XSList_PT      xslist_copy                       (XSList_PT slist);
extern XSList_PT      xslist_copy_equal_if              (XSList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XSList_PT      xslist_copy_apply_if              (XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XSList_PT      xslist_copyn                      (XSList_PT slist, int count);
extern XSList_PT      xslist_copyn_equal_if             (XSList_PT slist, int count, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XSList_PT      xslist_copyn_apply_if             (XSList_PT slist, int count, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XSList_PT      xslist_deep_copy                  (XSList_PT slist, int value_size);
extern XSList_PT      xslist_deep_copy_equal_if         (XSList_PT slist, int value_size, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XSList_PT      xslist_deep_copy_apply_if         (XSList_PT slist, int value_size, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XSList_PT      xslist_deep_copyn                 (XSList_PT slist, int value_size, int count);
extern XSList_PT      xslist_deep_copyn_equal_if        (XSList_PT slist, int value_size, int count, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XSList_PT      xslist_deep_copyn_apply_if        (XSList_PT slist, int value_size, int count, bool (*apply)(void *value, void *cl), void *cl);

/* O(1) */
extern bool           xslist_push_front_repeat          (XSList_PT slist, void *value);
extern void*          xslist_pop_front                  (XSList_PT slist);

/* O(1) */
extern bool           xslist_push_back_repeat           (XSList_PT slist, void *value);

/* O(1) */
extern void*          xslist_front                      (XSList_PT slist);
extern void*          xslist_back                       (XSList_PT slist);

/* O(1) */
extern XRSList_PT     xslist_front_node                 (XSList_PT slist);
extern XRSList_PT     xslist_back_node                  (XSList_PT slist);

/* O(N) */
extern int            xslist_push_front_unique          (XSList_PT slist, void *value);
extern int            xslist_push_front_unique_if       (XSList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern int            xslist_push_front_replace_if      (XSList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern int            xslist_push_front_deep_replace_if (XSList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern int            xslist_push_back_unique           (XSList_PT slist, void *value);
extern int            xslist_push_back_unique_if        (XSList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern int            xslist_push_back_replace_if       (XSList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern int            xslist_push_back_deep_replace_if  (XSList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(1) */
extern bool           xslist_insert_after               (XSList_PT slist, XRSList_PT node, void *value);

/* O(1) */
extern void*          xslist_remove_after               (XSList_PT slist, XRSList_PT node);

/* O(N) */
extern bool           xslist_remove                     (XSList_PT slist, void *value);
extern int            xslist_remove_all                 (XSList_PT slist, void *value);
extern int            xslist_remove_apply_if            (XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl);
extern bool           xslist_remove_apply_break_if      (XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl);
extern int            xslist_remove_equal_if            (XSList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern bool           xslist_remove_equal_break_if      (XSList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern bool           xslist_deep_remove                (XSList_PT slist, void **value);
extern int            xslist_deep_remove_apply_if       (XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl);
extern bool           xslist_deep_remove_apply_break_if (XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl);
extern int            xslist_deep_remove_equal_if       (XSList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern bool           xslist_deep_remove_equal_break_if (XSList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern void           xslist_free                       (XSList_PT *pslist);
extern void           xslist_free_apply                 (XSList_PT *pslist, bool (*apply)(void *value, void *cl), void *cl);
extern void           xslist_deep_free                  (XSList_PT *pslist);

/* O(N) */
extern void           xslist_clear                      (XSList_PT slist);
extern void           xslist_clear_apply                (XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl);
extern void           xslist_deep_clear                 (XSList_PT slist);

/* O(1) */
extern bool           xslist_swap                       (XSList_PT slist1, XSList_PT slist2);

/* O(1) */
extern int            xslist_merge                      (XSList_PT slist1, XSList_PT *pslist2);

/* O(N) */
extern void           xslist_reverse                    (XSList_PT slist);

/* O(N) */
extern int            xslist_map                        (XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl);
extern bool           xslist_map_break_if_true          (XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl);
extern bool           xslist_map_break_if_false         (XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XRSList_PT     xslist_find                       (XSList_PT slist, void *value);
extern XRSList_PT     xslist_find_equal_if              (XSList_PT slist, void *value, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern XRSList_PT     xslist_find_apply_if              (XSList_PT slist, bool (*apply)(void *value, void *cl), void *cl);

/* O(N) */
extern XPArray_PT     xslist_to_array                   (XSList_PT slist);

/* O(1) */
extern int            xslist_size                       (XSList_PT slist);
extern bool           xslist_is_empty                   (XSList_PT slist);

/* O(NlogN) */
extern bool           xslist_unique                     (XSList_PT slist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);
extern bool           xslist_deep_unique                (XSList_PT slist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(NlogN) */
extern bool           xslist_sort                       (XSList_PT slist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(N) */
extern bool           xslist_is_sorted                  (XSList_PT slist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

#ifdef __cplusplus
}
#endif

#endif
