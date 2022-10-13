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

#ifndef XKVHASHTABX_INCLUDED
#define XKVHASHTABX_INCLUDED

#include "../include/xarray_pointer.h"
#include "../include/xhash_kvtable.h"

struct XKVHashtab {
    XPArray_PT buckets;                      /* buckets[i] is type of XKVDList_PT */
    int    slot;                             /* == buckets->size */

    int    size;                             /* member number */

    int  (*hash) (void *key);                /* hash function */

    int  (*cmp)  (void *key1, void *key2, void *cl);   /* compare the key in XRBTree_PT */
    void  *cl;
};


/* used to transfer one kind of interface to another */
typedef struct XKVHashtab_Cmp_Keys_Paras* XKVHashtab_Cmp_Keys_Paras_PT;
typedef struct XKVHashtab_Cmp_Keys_Paras  XKVHashtab_Cmp_Keys_Paras_T;

struct XKVHashtab_Cmp_Keys_Paras {
    int     (*cmp)(void *key1, void* key2, void *cl);
    void     *key1;
    void     *key2;
    void     *cl;
};

#endif
