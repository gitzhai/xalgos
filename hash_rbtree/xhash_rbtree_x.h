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

#ifndef XRBTREEHASHX_INCLUDED
#define XRBTREEHASHX_INCLUDED

#include "../include/xarray_pointer.h"
#include "../include/xhash_rbtree.h"

struct XRBTreeHash {
    XPArray_PT buckets;                      /* buckets[i] is type of XRBTree_PT */
    int slot;                                /* == buckets->size */

    int size;                                /* member number */

    int  (*hash)(void *key);                 /* hash function */

    int  (*cmp)(void *key1, void *key2, void *cl);   /* compare the key in XRBTree_PT */
    void  *cl;
};

#endif
