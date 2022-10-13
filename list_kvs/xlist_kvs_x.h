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

#ifndef XKVSLISTX_INCLUDED
#define XKVSLISTX_INCLUDED

#include "../include/xlist_kvs.h"

/* O(N) */
extern XKVSList_PT xkvslist_copyn_if_impl   (XKVSList_PT dlist, int key_size, int value_size, int count, bool deep, int (*cmp)(void *key1, void *key2, void *cl1), void *cl1, bool (*applyk)(void *key, void *cl2), bool (*applykv)(void *key, void *value, void *cl2), void *cl2);
extern int         xkvslist_remove_if_impl (XKVSList_PT slist, bool deep, bool break_first, int (*cmp)(void *key1, void *key2, void *cl1), void *cl1, bool (*applyk) (void *key, void *cl2), bool (*applykv)(void *key, void **value, void *cl2), void *cl2);

#endif
