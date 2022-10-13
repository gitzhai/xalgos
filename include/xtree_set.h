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

#ifndef XSET_INCLUDED
#define XSET_INCLUDED

#include "xtree_redblack_list.h"
#include "xlist_s.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XListRBTree_PT      XSet_PT;

/* O(1) */
extern XSet_PT   xset_new               (int (*cmp)(void *elem1, void *elem2, void *cl), void *cl);

/* O(NlgN) */
extern XSet_PT   xset_copy              (XSet_PT set);
extern XSet_PT   xset_deep_copy         (XSet_PT set, int elem_size);

/* O(lgN) */
extern bool      xset_put_repeat        (XSet_PT set, void *elem);
extern bool      xset_put_unique        (XSet_PT set, void *elem);

/* O(lgN) */
extern void      xset_elem_unique       (XSet_PT set, void *elem);
/* O(NlgN) */
extern void      xset_unique            (XSet_PT set);

/* O(lgN) */
extern void*     xset_select            (XSet_PT set, int k);

/* O(lgN) */
extern bool      xset_find              (XSet_PT set, void *elem);

/* O(N) */
extern XSList_PT xset_elems             (XSet_PT set, void *low, void *high);

/* O(NlgN) */
extern void      xset_free              (XSet_PT *pset);
extern void      xset_free_apply        (XSet_PT *pset, bool (*apply)(void *elem, void *cl), void *cl);
extern void      xset_deep_free         (XSet_PT *pset);

/* O(NlgN) */
extern void      xset_clear             (XSet_PT set);
extern void      xset_clear_apply       (XSet_PT set, bool (*apply)(void *elem, void *cl), void *cl);
extern void      xset_deep_clear        (XSet_PT set);

/* O(lgN) */
extern void      xset_remove            (XSet_PT set, void *elem);
extern void      xset_remove_all        (XSet_PT set, void *elem);

/* O(lgN) */
extern void      xset_deep_remove       (XSet_PT set, void *elem);
extern void      xset_deep_remove_all   (XSet_PT set, void *elem);

/* O(N) */
extern int       xset_map               (XSet_PT set, bool (*apply)(void *elem, void *cl), void *cl);
extern bool      xset_map_break_if_true (XSet_PT set, bool (*apply)(void *elem, void *cl), void *cl);
extern bool      xset_map_break_if_false(XSet_PT set, bool (*apply)(void *elem, void *cl), void *cl);

/* O(N) */
extern int       xset_map_min_to_max    (XSet_PT set, bool (*apply)(void *elem, void *cl), void *cl);
extern int       xset_map_max_to_min    (XSet_PT set, bool (*apply)(void *elem, void *cl), void *cl);

/* O(1) */
extern bool      xset_swap              (XSet_PT set1, XSet_PT set2);

/* O(NlgN) */
extern bool      xset_merge_repeat      (XSet_PT set1, XSet_PT *pset2);
extern bool      xset_merge_unique      (XSet_PT set1, XSet_PT *pset2);

/* O(NlgN) */
extern bool      xset_copy_merge_repeat (XSet_PT set1, XSet_PT pset2);
extern bool      xset_copy_merge_unique (XSet_PT set1, XSet_PT pset2);

/* O(1) */
extern int       xset_size              (XSet_PT set);
extern bool      xset_is_empty          (XSet_PT set);

/* O(lgN) */
extern int       xset_elem_size         (XSet_PT set, void *elem);
/* O(N) */
extern int       xset_elems_size        (XSet_PT set, void *low, void *high);

//extern bool      xset_subset            (XSet_PT set1, XSet_PT set2);     /* t is subset of s */
//extern bool      xset_equal             (XSet_PT set1, XSet_PT set2);

//extern XSet_PT   xset_union             (XSet_PT set1, XSet_PT set2);
//extern XSet_PT   xset_inter             (XSet_PT set1, XSet_PT set2);
//extern XSet_PT   xset_minus             (XSet_PT set1, XSet_PT set2);     /* s - t */
//extern XSet_PT   xset_diff              (XSet_PT set1, XSet_PT set2);

#ifdef __cplusplus
}
#endif

#endif
