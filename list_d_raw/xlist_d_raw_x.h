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

#ifndef XRDLISTX_INCLUDED
#define XRDLISTX_INCLUDED

#include "../include/xlist_d_raw.h"

struct XRDList {
    XRDList_PT   prev;
    XRDList_PT   next;

    void        *value;
};

/* O(N) */
extern int xrdlist_remove_if_impl(XRDList_PT *pdlist, bool deep, bool break_first, int (*cmp)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2);

/* O(NlgN) */
extern XRDList_PT xrdlist_merge_sort_impl(XRDList_PT slist, int (*cmp)(void *value1, void *value2, void *cl), void *cl);

#endif
