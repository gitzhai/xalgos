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
*       << Algorithms in C >> Third Edition : chapter 9.7
*/

#ifndef XMINBINQUEX_INCLUDED
#define XMINBINQUEX_INCLUDED

#include "../include/xqueue_sequence.h"
#include "../include/xqueue_binomial_min.h"

/* XMinBinQue_PT has two layers :
 *   1. the size of the first layer XPSeq_PT is fixed to XUTILS_UNLIMITED_BASED_ON_POWER_2 (64) which is enough to hold everything (2^64)
 *   2. the elements of the first layer XPSeq_PT are XBinTree_Node_PT
 *      --------------------------------------------------------------------------------------------------------------
 *      | XBinTree_Node_PT0 | XBinTree_Node_PT1 | XBinTree_Node_PT2 | XBinTree_Node_PT3 | ...... | XBinTree_Node_PTn |
 *      ---------|--------------------|-------------------|-------------------|---------------------------------------
 *               *                    *                   *                   * 
 *                                   /                   /                   /
 *                                  *                   *                   * 
 *                                                     / \                /   \
 *                                                    *   *              *     *
 *                                                                      / \   / \
 *                                                                     *   * *   * 
 */

struct XMinBinQue {
    int      size;             /* number of valid elements */
    int      capacity;         /* number of elements can be filled into the Deque, 0 means no limitation */

    int      discard_strategy; /* how to discard element when no capacity left :
                                *   0 : discard new (default)
                                *   1 : discard top priority element
                                */

    XPSeq_PT buckets;          /* hold XUTILS_UNLIMITED_BASED_ON_POWER_2 (64) XBinTree_Node_PT */

    int    (*cmp)(void *x, void *y, void *cl);
    void    *cl;
};

/* O(lgN) */
extern bool  xminbinque_push_impl   (XMinBinQue_PT queue, void *x, bool maxp);
extern void* xminbinque_pop_impl    (XMinBinQue_PT queue, bool maxp);
extern void* xminbinque_peek_impl   (XMinBinQue_PT queue, bool maxp);
extern bool  xminbinque_merge_impl  (XMinBinQue_PT queue1, XMinBinQue_PT *pqueue2, bool maxp, bool increase_size);

#endif
