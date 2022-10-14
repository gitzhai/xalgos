# xalgos
data structures and algorithms implemented with C

header file foler :

    include

test file folder :

    test

test main function entry :

    test/test.c

make file usage :

    1. test the library
       a. change folder name xalgos to xalgos_bin
       b. make
       c. ./xalgos

    2. dynamic library
       a. delete test folder (or move it to other place)
       b. change folder name xalgos to xalgos_libso
       c. make
       d. libxalgos.so is created

    3. static library
       a. delete test folder (or move it to other place)
       b. change folder name xalgos to xalgos_liba
       c. make
       d. libxalgos.a is created


description to the whole library :

    include/xalgos.h


Data Structures-----------(directory name)-----------------(header file)

    Memory Arena :
        XArena_PT         (mem_arena)                      xmem_arena.h

    Bit :
        XBit_PT           (bit)                            xbit.h

    Pair :
        XPair_PT          (pair)

    List :
        XRSList_PT        (list_s_raw)                     xlist_s_raw.h
        XSList_PT         (list_s)                         xlist_s.h
        XSCList_PT        (list_sc)                        xlist_sc.h
        XKVSList_PT       (list_kvs)                       xlist_kvs.h

        XRDList_PT        (list_d_raw)                     xlist_d_raw.h
        XDList_PT         (list_d)                         xlist_d.h
        XDCList_PT        (list_dc)                        xlist_dc.h
        XKVDList_PT       (list_kvd)                       xlist_kvd.h

    Array :
        XArray_PT         (array)                          xarray.h
        XPArray_PT        (array_pointer)                  xarray_pointer.h
        XIArray_PT        (array_int)                      xarray_i.h

    Atom :
        XTreeAtom_PT      (tree_atom)                      xatom.h

    Sequence :
        XPSeq_PT          (queue_sequence)                 xqueue_sequence.h
        XISeq_PT          (queue_sequence_int)             xqueue_sequence_int.h

    Queue :
        XDeque_PT         (queue_deque)                    xqueue_deque.h
        XQueue_PT         (queue)                          xqueue.h
        XMaxPQ_PT         (queue_priority_max)             xqueue_priority_max.h
        XMinPQ_PT         (queue_priority_min)             xqueue_priority_min.h
        XFifo_PT          (queue_fifo)                     xqueue_fifo.h
        XLifo_PT          (queue_lifo)                     xqueue_lifo.h
        XIndexMaxPQ_PT    (queue_index_priority_max)       xqueue_index_priority_max.h
        XIndexMinPQ_PT    (queue_index_priority_min)       xqueue_index_priority_min.h
        XKeyIndexMaxPQ_PT (queue_key_index_priority_max)   xqueue_key_index_priority_max.h
        XKeyIndexMinPQ_PT (queue_key_index_priority_min)   xqueue_key_index_priority_min.h
        XMaxBinQue_PT     (queue_binomial_max)             xmaxbinque.h
        XMinBinQue_PT     (queue_binomial_min)             xminbinque.h

    Stack :
        XStack_PT         (queue_stack)                    xqueue_stack.h

    Heap :
        XMaxHeap_PT       (heap_max)                       xheap_max.h
        XMinHeap_PT       (heap_min)                       xheap_min.h
        XIndexMaxHeap_PT  (heap_index_max)                 xheap_index_max.h
        XIndexMinHeap_PT  (heap_index_min)                 xheap_index_min.h
        XFibHeap_PT       (heap_fibonacci)                 xheap_fibonacci.h

    Hash :
        XKVHashtab_PT     (hash_kvtable)                   xhash_kvtable.h
        XHashtab_PT       (hash_table)                     xhash_table.h
        XRBTreeHash_PT    (hash_rbtree)                    xhash_rbtree.h

    Tree :
        XBinTree_PT       (tree_binary)                    xtree_binary.h
        XBSTree_PT        (tree_binary_search)             xtree_binary_search.h
        XRBTree_PT        (tree_redblack)                  xtree_redblack.h
        XListRBTree_PT    (tree_redblack_list)             xtree_redblack_list.h
        XAVLTree_PT       (tree_avl)                       xtree_avl.h
        XMTree_PT         (tree_multiple_branch)           xmtree.h

    Map :
        XHashMap_PT       (hash_map)                       xhash_map.h
        XMap_PT           (tree_map)                       xmap.h

    Set :
        XHashSet_PT       (hash_set)                       xhash_set.h
        XSet_PT           (tree_set)                       xtree_set.h

    Graph :
        XGraph_PT         (graph_undirected)               xgraph_undirected.h
        XDigraph_PT       (graph_direct)                   xdigraph_directed.h
        XWEdge_PT         (graph_weight_edge)              xgraph_weight_edge.h
        XWGraph_PT        (graph_weight_undirected)        xgraph_weight_undirected.h
        XWDigraph_PT      (graph_weight_directed)          xgraph_weight_directed.h


Algorithms :

    Sort :
        1. bubble sort
           xparray_bubble_sort                             xarray_pointer.h

        2. select sort
           xparray_select_sort                             xarray_pointer.h

        3. insert sort                                     xarray_pointer.h
           xparray_insert_sort

        4. shell sort
           xparray_shell_sort                              xarray_pointer.h

        5. merge sort
            5.1 xparray_merge_sort                         xarray_pointer.h
               5.1.1 xparray_merge_sort_impl_no_copy              (#define MERGE_SORT_NO_COPY_MERGE)
               5.1.2 xparray_merge_sort_impl_bottom_up            (#define MERGE_SORT_BOTTOM_UP)
               5.1.3 xparray_merge_sort_impl_up_bottom            (default)

            5.2 xrslist_sort                               xlist_s_raw.h

            5.3 multiway merge sort
                5.3.1 xdeque_vmultiway_sort                xqueue_deque.h
                5.3.2 xdeque_amultiway_sort                xqueue_deque.h

        6. quick sort
           6.1 xparray_quick_sort                          xarray_pointer.h
            6.1.1 xparray_quick_sort_impl_basic_split             (#define QUICK_SORT_NORMAL_SPLIT)
            6.1.2 xparray_quick_sort_impl_random_split            (#define QUICK_SORT_RANDOM_SPLIT)
            6.1.3 xparray_quick_sort_impl_median_of_three_split   (#define QUICK_SORT_MEDIAN_OF_THREE)
            6.1.4 xparray_quick_sort_impl_3_way_split             (#define QUICK_SORT_3_WAY_SPLIT)
            6.1.5 xparray_quick_sort_impl_quick_3_way_split       (default)

        7. heap sort
           xparray_heap_sort                               xarray_pointer.h

        8. pointer sort
           8.1 xarray_pointer_sort                         xarray_pointer.h
           8.2 xarray_pointer_inplace_sort                 xarray_pointer.h

        9. index sort
           9.1 xarray_index_sort                           xarray_pointer.h
           9.2 xarray_index_inplace_sort                   xarray_pointer.h

        10. counting sort
           xiarray_counting_sort                           xarray_int.h

        11. bucket sort
           xiarray_bucket_sort                             xarray_int.h

    find minimum M values :
        xmaxpq_keep_min_values                             xqueue_priority_max.h

    find maximum M values :
        xminpq_keep_max_values                             xqueue_priority_min.h

    binary search :
        xparray_binary_search

    get kth element :
        xparray_get_kth_element                            xarray_pointer.h
          1. xparray_get_kth_element_impl_heap_sort               (#define GET_KTH_ELEMENT_HEAP_SORT)
          2. xparray_get_kth_element_impl_quick_sort              (default)

    make array a min heap :
        xparray_heapify_min                                xarray_pointer.h

    make array a max heap :
        xparray_heapify_max                                xarray_pointer.h

General Utils-------------(directory name)-----------------(header file)

    except :              (except)                         xexcept.h
        XEXCEPT_TRY
        XEXCEPT_CATCH
        XEXCEPT_ELSE
        XEXCEPT_FINALLY
        XEXCEPT_END_TRY
        XEXCEPT_RETURN

    assert :              (assert)                         xassert.h
        xassert
        xassert_false

    memory :              (mem)                            xmem.h
        XMEM_MALLOC
        XMEM_CALLOC
        XMEM_FREE
        XMEM_NEW
        XMEM_NEWN
        XMEM_NEW0
        XMEM_NEW0N
        XMEM_RESIZE
        XMEM_RESIZE0
