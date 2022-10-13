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
*
*   Refer to :
*       <<Algorithms in C>> Third Edition. chapter 9.6
*/

#ifndef XINDEXMINHEAPX_INCLUDED
#define XINDEXMINHEAPX_INCLUDED

#include "../include/xarray_int.h"
#include "../include/xarray_pointer.h"
#include "../include/xqueue_sequence_int.h"
#include "../include/xheap_index_min.h"

/* 
*              --0---1---2----------------i-----------------------
*  buckets :   | - | - | - | ------ | element-i | ------ | - | - |   all elements saved in "buckets" directly : <i, element-i>
*              ---------------------------------------------------
*                                         /
*                                  -------
*                                 /
*              --0---1-----------m-------------------
*  heap    :   | - | - | ------| i | ------ | - | - |    "i" of "element-i" will be used to build a max heap, its final index in heap is "m"
*              --------------------------------------
*                                \
*                                 -----
*                                      \
*              --0---1---2--------------i--------------------
*  index   :   | - | - | - | ------ |   m  | ------ | - | - |      "i" of "element-i" will be used to save "m" which is the position of "i" in heap : m = index[i]
*              ----------------------------------------------
*/

struct XIndexMinHeap {
    XPArray_PT  buckets;
    XISeq_PT    heap;
    XIArray_PT  index;

    int       (*cmp)(void *x, void *y, void *cl);
    void       *cl;
};

extern bool        xindexminheap_push_impl    (XIndexMinHeap_PT heap, int i, void *data, void **old_data, bool minheap);
extern void*       xindexminheap_pop_impl     (XIndexMinHeap_PT heap, int *i, bool minheap);

extern bool        xindexminheap_remove_impl  (XIndexMinHeap_PT heap, int i, void **old_data, bool minheap);

extern bool        xindexminheap_is_heap_sorted_impl (XIndexMinHeap_PT heap, int k, int lo, int hi, bool minheap);

#endif
