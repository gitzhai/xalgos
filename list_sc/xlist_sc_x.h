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

#ifndef XSCLISTX_INCLUDED
#define XSCLISTX_INCLUDED

#include "../include/xlist_sc.h"

/* O(N) */
extern XSCList_PT xsclist_copyn_if_impl(XSCList_PT slist, int value_size, int count, bool deep, int (*cmp)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2);
extern int        xsclist_remove_if_impl(XSCList_PT slist, bool deep, bool break_first, int (*cmp)(void *value1, void *value2, void *cl1), void *cl1, bool (*apply)(void *value, void *cl2), void *cl2);

#endif
