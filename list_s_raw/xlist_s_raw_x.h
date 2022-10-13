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

#ifndef XRSLISTX_INCLUDED
#define XRSLISTX_INCLUDED

#include "../include/xlist_s_raw.h"

struct XRSList {
    XRSList_PT   next;

    void        *value;
};

/* O(N) */
extern int         xrslist_remove_if_impl     (XRSList_PT *pslist, bool deep, bool break_first, int (*cmp)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2);
extern int         xrslist_free_except_front  (XRSList_PT slist);
extern XRSList_PT  xrslist_get_kth            (XRSList_PT slist, int k);
extern void*       xrslist_pop_kth            (XRSList_PT *pslist, int k);

/* O(NlgN) */
extern XRSList_PT  xrslist_merge_sort_impl    (XRSList_PT slist, int(*cmp)(void *value1, void *value2, void *cl), void *cl);

#endif
