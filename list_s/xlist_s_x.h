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

#ifndef XSLISTX_INCLUDED
#define XSLISTX_INCLUDED

#include "../include/xlist_s.h"

struct XSList {
    XRSList_PT head, tail;

    int        size;
};

/* O(N) */
extern XSList_PT  xslist_copyn_if_impl    (XSList_PT dlist, int value_size, int count, bool deep, int (*cmp)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2);
extern int        xslist_remove_if_impl   (XSList_PT dlist, bool deep, bool break_first, int (*cmp)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2);

/* O(N) */
extern bool       xslist_push_back_rslist (XSList_PT slist, XRSList_PT rslist);

/* O(N) */
extern XSList_PT  xslist_new_with_rslist  (XRSList_PT list);

#endif
