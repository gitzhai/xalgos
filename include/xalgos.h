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

#ifndef XALGOS_INCLUDED
#define XALGOS_INCLUDED

/*  General Utils             (directory name)                 (header file)     test 
 *
 *      except :              (except)                         xexcept.h         Tested
 *          XEXCEPT_TRY
 *          XEXCEPT_CATCH
 *          XEXCEPT_ELSE
 *          XEXCEPT_FINALLY
 *          XEXCEPT_END_TRY
 *          XEXCEPT_RETURN
 *
 *      assert :              (assert)                         xassert.h         Tested
 *          xassert
 *          xassert_false
 *
 *      memory :              (mem)                            xmem.h            Partially
 *          XMEM_MALLOC
 *          XMEM_CALLOC
 *          XMEM_FREE
 *          XMEM_NEW
 *          XMEM_NEWN
 *          XMEM_NEW0
 *          XMEM_NEW0N
 *          XMEM_RESIZE
 *          XMEM_RESIZE0
 *
 *  Data Structures           (directory name)                 (header file)     test
 *
 *      Memory Arena :
 *          XArena_PT         (mem_arena)                      xmem_arena.h     
 *
 *      Bit :
 *          XBit_PT           (bit)                            xbit.h            Tested
 *
 *      Pair :
 *          XPair_PT          (pair)
 *
 *      List :
 *          XRSList_PT        (list_s_raw)                     xlist_s_raw.h     Tested
 *          XSList_PT         (list_s)                         xlist_s.h         Tested
 *          XSCList_PT        (list_sc)                        xlist_sc.h        Tested
 *          XKVSList_PT       (list_kvs)                       xlist_kvs.h       Tested
 *
 *          XRDList_PT        (list_d_raw)                     xlist_d_raw.h     Tested
 *          XDList_PT         (list_d)                         xlist_d.h         Tested
 *          XDCList_PT        (list_dc)                        xlist_dc.h        Tested
 *          XKVDList_PT       (list_kvd)                       xlist_kvd.h       Tested
 *
 *      Array :
 *          XArray_PT         (array)                          xarray.h          Tested
 *          XPArray_PT        (array_pointer)                  xarray_pointer.h  Tested
 *          XIArray_PT        (array_int)                      xarray_i.h        Tested
 *
 *      Atom :
 *          XTreeAtom_PT      (tree_atom)                      xatom.h           Tested
 *
 *      Sequence :
 *          XPSeq_PT          (queue_sequence)                 xqueue_sequence.h                Tested
 *          XISeq_PT          (queue_sequence_int)             xqueue_sequence_int.h            Tested
 *
 *      Queue :
 *          XDeque_PT         (queue_deque)                    xqueue_deque.h                   Tested
 *          XQueue_PT         (queue)                          xqueue.h                         Tested
 *          XMaxPQ_PT         (queue_priority_max)             xqueue_priority_max.h            Tested
 *          XMinPQ_PT         (queue_priority_min)             xqueue_priority_min.h            Tested
 *          XFifo_PT          (queue_fifo)                     xqueue_fifo.h                    Tested
 *          XLifo_PT          (queue_lifo)                     xqueue_lifo.h                    Tested
 *          XIndexMaxPQ_PT    (queue_index_priority_max)       xqueue_index_priority_max.h      Tested
 *          XIndexMinPQ_PT    (queue_index_priority_min)       xqueue_index_priority_min.h      Tested
 *          XKeyIndexMaxPQ_PT (queue_key_index_priority_max)   xqueue_key_index_priority_max.h  Tested
 *          XKeyIndexMinPQ_PT (queue_key_index_priority_min)   xqueue_key_index_priority_min.h  Tested
 *          XMaxBinQue_PT     (queue_binomial_max)             xmaxbinque.h                     Tested
 *          XMinBinQue_PT     (queue_binomial_min)             xminbinque.h                     Tested
 *
 *      Stack :
 *          XStack_PT         (queue_stack)                    xqueue_stack.h    Tested
 *
 *      Heap :
 *          XMaxHeap_PT       (heap_max)                       xheap_max.h       Tested
 *          XMinHeap_PT       (heap_min)                       xheap_min.h       Tested
 *          XIndexMaxHeap_PT  (heap_index_max)                 xheap_index_max.h Tested
 *          XIndexMinHeap_PT  (heap_index_min)                 xheap_index_min.h Tested
 *          XFibHeap_PT       (heap_fibonacci)                 xheap_fibonacci.h Tested
 *
 *      Hash :
 *          XKVHashtab_PT     (hash_kvtable)                   xhash_kvtable.h   Tested
 *          XHashtab_PT       (hash_table)                     xhash_table.h     Tested by XRBTreeHash_PT
 *          XRBTreeHash_PT    (hash_rbtree)                    xhash_rbtree.h    Tested
 *
 *      Tree :
 *          XBinTree_PT       (tree_binary)                    xtree_binary.h          Tested
 *          XBSTree_PT        (tree_binary_search)             xtree_binary_search.h   Tested
 *          XRBTree_PT        (tree_redblack)                  xtree_redblack.h        Tested
 *          XListRBTree_PT    (tree_redblack_list)             xtree_redblack_list.h   Tested      (all values for the "same" key are saved in a XRSList_PT)
 *          XAVLTree_PT       (tree_avl)                       xtree_avl.h             Tested
 *          XMTree_PT         (tree_multiple_branch)           xmtree.h
 *
 *      Map :
 *          XHashMap_PT       (hash_map)                       xhash_map.h
 *          XMap_PT           (tree_map)                       xmap.h
 *
 *      Set :
 *          XHashSet_PT       (hash_set)                       xhash_set.h            Tested 
 *          XSet_PT           (tree_set)                       xtree_set.h            Tested
 *
 *      Graph :
 *          XGraph_PT         (graph_undirected)               xgraph_undirected.h
 *          XDigraph_PT       (graph_direct)                   xdigraph_directed.h
 *          XWEdge_PT         (graph_weight_edge)              xgraph_weight_edge.h
 *          XWGraph_PT        (graph_weight_undirected)        xgraph_weight_undirected.h
 *          XWDigraph_PT      (graph_weight_directed)          xgraph_weight_directed.h
 *
 *
 *  Algorithms :
 *
 *      Sort :
 *          1. bubble sort
 *             xparray_bubble_sort                             xarray_pointer.h
 *
 *          2. select sort
 *             xparray_select_sort                             xarray_pointer.h
 *
 *          3. insert sort                                     xarray_pointer.h
 *             xparray_insert_sort
 *
 *          4. shell sort
 *             xparray_shell_sort                              xarray_pointer.h
 *
 *          5. merge sort
 *              5.1 xparray_merge_sort                         xarray_pointer.h
 *                 5.1.1 xparray_merge_sort_impl_no_copy              (#define MERGE_SORT_NO_COPY_MERGE)
 *                 5.1.2 xparray_merge_sort_impl_bottom_up            (#define MERGE_SORT_BOTTOM_UP)
 *                 5.1.3 xparray_merge_sort_impl_up_bottom            (default)
 *
 *              5.2 xrslist_sort                               xlist_s_raw.h
 *
 *              5.3 multiway merge sort
 *                  5.3.1 xdeque_vmultiway_sort                xqueue_deque.h
 *                  5.3.2 xdeque_amultiway_sort                xqueue_deque.h
 *
 *          6. quick sort
 *             6.1 xparray_quick_sort                          xarray_pointer.h
 *              6.1.1 xparray_quick_sort_impl_basic_split             (#define QUICK_SORT_NORMAL_SPLIT)
 *              6.1.2 xparray_quick_sort_impl_random_split            (#define QUICK_SORT_RANDOM_SPLIT)
 *              6.1.3 xparray_quick_sort_impl_median_of_three_split   (#define QUICK_SORT_MEDIAN_OF_THREE)
 *              6.1.4 xparray_quick_sort_impl_3_way_split             (#define QUICK_SORT_3_WAY_SPLIT)
 *              6.1.5 xparray_quick_sort_impl_quick_3_way_split       (default)
 *
 *          7. heap sort
 *             xparray_heap_sort                               xarray_pointer.h
 *
 *          8. pointer sort
 *             8.1 xarray_pointer_sort                         xarray_pointer.h
 *             8.2 xarray_pointer_inplace_sort                 xarray_pointer.h
 *
 *          9. index sort
 *             9.1 xarray_index_sort                           xarray_pointer.h
 *             9.2 xarray_index_inplace_sort                   xarray_pointer.h
 *
 *          10. counting sort
 *             xiarray_counting_sort                           xarray_int.h
 *
 *          11. bucket sort
 *             xiarray_bucket_sort                             xarray_int.h
 *
 *      find minimum M values :
 *          xmaxpq_keep_min_values                             xqueue_priority_max.h
 *
 *      find maximum M values :
 *          xminpq_keep_max_values                             xqueue_priority_min.h
 *
 *      binary search :
 *          xparray_binary_search
 *
 *      get kth element :
 *          xparray_get_kth_element                            xarray_pointer.h
 *            1. xparray_get_kth_element_impl_heap_sort               (#define GET_KTH_ELEMENT_HEAP_SORT)
 *            2. xparray_get_kth_element_impl_quick_sort              (default)
 *
 *      make array a min heap :
 *          xparray_heapify_min                                xarray_pointer.h
 *
 *      make array a max heap :
 *          xparray_heapify_max                                xarray_pointer.h
 *
 */

/* design rules :
 *
 *   1. in order to avoid namespace conflict, use prefix "x"/"X" for all global symbols
 *
 *   2. an ADT(abstract data type) is implemented with three files : 
 *      a : xadt_name.h   : define all the interfaces which can be used externally
 *      b : xadt_name_x.h : define the detailed data structures used to implement the ADT
 *      c : xadt_name.c   : define all the interface implementation of the ADT
 *
 *   3. struct type defined with Capital Letter, like :
 *      struct XMem_Descriptor
 *
 *   4. typedef define to the struct type with postfix T, like : 
 *      typedef struct XMem_Descriptor XMem_Descriptor_T
 *
 *   5. typedef define to the struct type pointer with postfix PT, like :
 *      typedef struct XMem_Descriptor* XMem_Descriptor_PT
 *
 *   6. global variable defined with letter g, like :
 *      const Except_T  xg_assert_failed;
 *
 *   7. global pointer variable defined with letter gp, like :
 *      const Except_T* xgp_assert_failed;
 *
 *   8. only "<" or "<=", but not ">" or ">=" used for logic judgement
 *
 */

/* except */
#include "xexcept.h"

/* assert */
#include "xassert.h"

/* memory */
#include "xmem.h"
#include "xmem_arena.h"

/* pair */
#include "xpair.h"

/* atom */
#include "xtree_atom.h"

/* arith */
#include "xarith_int.h"

/* bit */
#include "xbit.h"

/* list */
#include "xlist_s_raw.h"
#include "xlist_s.h"
#include "xlist_kvs.h"
#include "xlist_sc.h"

#include "xlist_d_raw.h"
#include "xlist_d.h"
#include "xlist_kvd.h"
#include "xlist_dc.h"

/* array */
#include "xarray.h"
#include "xarray_pointer.h"
#include "xarray_int.h"

/* queue */
#include "xqueue_sequence.h"
#include "xqueue_sequence_int.h"
#include "xqueue_deque.h"
#include "xqueue.h"
#include "xqueue_fifo.h"
#include "xqueue_lifo.h"
#include "xqueue_stack.h"

/* heap */
#include "xheap_min.h"
#include "xheap_max.h"

#include "xheap_index_min.h"
#include "xheap_index_max.h"

#include "xheap_fibonacci.h"

/* priority queue */
#include "xqueue_priority_min.h"
#include "xqueue_priority_max.h"

#include "xqueue_index_priority_min.h"
#include "xqueue_index_priority_max.h"

#include "xqueue_key_index_priority_min.h"
#include "xqueue_key_index_priority_max.h"

#include "xqueue_binomial_min.h"
#include "xqueue_binomial_max.h"

/* hash table */
#include "xhash_kvtable.h"
#include "xhash_table.h"
#include "xhash_rbtree.h"

/* tree */
#include "xtree_binary.h"
#include "xtree_binary_search.h"
#include "xtree_redblack.h"
#include "xtree_redblack_list.h"
#include "xtree_avl.h"
#include "xtree_multiple_branch.h"

/* map */
#include "xhash_map.h"
#include "xtree_map.h"

/* set */
#include "xhash_set.h"
#include "xtree_set.h"

/* graph */
#include "xgraph_weight_edge.h"

#include "xgraph_undirected.h"
#include "xgraph_weight_undirected.h"

#include "xgraph_directed.h"
#include "xgraph_weight_directed.h"

#endif


