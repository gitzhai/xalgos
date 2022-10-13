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

#ifndef XISEQ_INCLUDED
#define XISEQ_INCLUDED

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XISeq* XISeq_PT;

/* O(1) */
extern XISeq_PT xiseq_new              (int capacity);  /* 0 < capacity */

/* O(N) */
extern XISeq_PT xiseq_copy             (XISeq_PT seq);
extern XISeq_PT xiseq_copyn            (XISeq_PT seq, int count);

/* O(1) */
extern bool    xiseq_push_front        (XISeq_PT seq, int x);
extern int     xiseq_pop_front         (XISeq_PT seq);

/* O(1) */
extern bool    xiseq_push_back         (XISeq_PT seq, int x);
extern int     xiseq_pop_back          (XISeq_PT seq);

/* O(1) */
extern int     xiseq_front             (XISeq_PT seq);
extern int     xiseq_back              (XISeq_PT seq);

/* O(1) */
extern void    xiseq_free              (XISeq_PT *seq);
extern void    xiseq_clear             (XISeq_PT seq);

/* O(1) */
extern int     xiseq_size              (XISeq_PT seq);
extern int     xiseq_capacity          (XISeq_PT seq);
extern bool    xiseq_is_empty          (XISeq_PT seq);
extern bool    xiseq_is_full           (XISeq_PT seq);

/* O(N) */
extern bool    xiseq_expand            (XISeq_PT seq, int expand_size);

/* O(N) */
extern bool    xiseq_heapify_min       (XISeq_PT seq);
extern bool    xiseq_heapify_max       (XISeq_PT seq);

/* O(NlgN) */
extern bool    xiseq_heap_sort         (XISeq_PT seq);

/* O(NlgN) */
extern bool    xiseq_quick_sort        (XISeq_PT seq);

/* O(N) */
extern bool    xiseq_is_sorted         (XISeq_PT seq);

#ifdef __cplusplus
}
#endif

#endif
