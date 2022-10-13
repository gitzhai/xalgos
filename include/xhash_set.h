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

#ifndef XHASHSET_INCLUDED
#define XHASHSET_INCLUDED

#include <stdbool.h>
#include "xqueue_deque.h"
#include "xhash_rbtree.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XRBTreeHash_PT XHashSet_PT;

/* O(1) */
extern XHashSet_PT  xhashset_new                (int hint, int (*hash)(void *elem), int (*cmp)(void *elem1, void *elem2, void *cl), void *cl);

/* O(NlgN) */
extern XHashSet_PT  xhashset_copy               (XHashSet_PT set);
extern XHashSet_PT  xhashset_deep_copy          (XHashSet_PT set, int elem_size);

/* O(lgN) */
extern bool         xhashset_put_repeat         (XHashSet_PT set, void *elem);
extern int          xhashset_put_unique         (XHashSet_PT set, void *elem);

/* O(lgN) */
extern bool         xhashset_find               (XHashSet_PT set, void *elem);

/* O(lgN) */
extern bool         xhashset_remove             (XHashSet_PT set, void *elem);
extern bool         xhashset_deep_remove        (XHashSet_PT set, void *elem);

/* O(NlgN) */
extern int          xhashset_remove_all         (XHashSet_PT set, void *elem);
extern int          xhashset_deep_remove_all    (XHashSet_PT set, void *elem);

/* O(NlgN) */
extern void         xhashset_clear              (XHashSet_PT set);
extern void         xhashset_deep_clear         (XHashSet_PT set);

/* O(NlgN) */
extern void         xhashset_free               (XHashSet_PT *set);
extern void         xhashset_deep_free          (XHashSet_PT *set);

/* O(NlgN) */
extern int          xhashset_map                (XHashSet_PT set, bool (*apply)(void *elem, void *cl), void *cl);
extern bool         xhashset_map_break_if_true  (XHashSet_PT set, bool (*apply)(void *elem, void *cl), void *cl);
extern bool         xhashset_map_break_if_false (XHashSet_PT set, bool (*apply)(void *elem, void *cl), void *cl);

/* O(1) */
extern bool         xhashset_swap               (XHashSet_PT set1, XHashSet_PT set2);

/* O(1) */
extern int          xhashset_size               (XHashSet_PT set);
extern bool         xhashset_is_empty           (XHashSet_PT set);

/* O(lgN) */
extern int          xhashset_count              (XHashSet_PT set, void *elem);

/* O(1) */
extern double       xhashset_loading_factor     (XHashSet_PT set);

/* O(NlgN) */
extern int          xhashset_elem_size          (XHashSet_PT set, void *elem);

#ifdef __cplusplus
}
#endif

#endif
