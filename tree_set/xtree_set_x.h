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

#ifndef XSETX_INCLUDED
#define XSETX_INCLUDED

#include "../include/xqueue_priority_min.h"
#include "../tree_redblack_list/xtree_redblack_list_x.h"
#include "../include/xtree_set.h"

/* Note :
*      key will be saved as "value" in rbtree too, then all the keys will be saved in the "values",
*      by this way, we can solve all the tricky issues caused by "repeat" keys
*/
typedef XListRBTree_Node_PT XSet_Node_PT;

/* O(NlgN) */
extern void      xset_unique_except(XSet_PT set, void *elem);
extern XSet_PT   xset_unique_except_return_uniqued (XSet_PT set, void *elem);
extern XSet_PT   xset_unique_return_uniqued        (XSet_PT set);

/* O(N) */
extern bool      xset_has_repeat_elems_except      (XSet_PT set, void *elem);
extern int       xset_repeat_elems_size_except     (XSet_PT set, void *elem);

/* O(lgN) */
extern void*     xset_get                          (XSet_PT set, void *elem);

#endif
