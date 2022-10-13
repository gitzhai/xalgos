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

#ifndef XGRAPH_INCLUDED
#define XGRAPH_INCLUDED

#include <stdbool.h>
#include "xtree_map.h"
#include "xtree_set.h"
#include "xlist_d.h"
#include "xtree_multiple_branch.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XGraph *XGraph_PT;

/* ToDo :
*    make the vertex atom, this can use "vertex1 == vertex2" in internal functions
*/


/* Note : 
 *   1. dfs(depth first search) method may cause heap memory exhusted,
 *      so, use bfs method please for large scale graph !
 *
 *   2. think about self cycle and parallel edge for all methods
 */

/* O(1) */
extern XGraph_PT  xgraph_new                    (int (*cmp)(void *vertex1, void *vertex2, void *cl), void *cl);

/* O(ElgE+VlgV) */
extern void       xgraph_free                   (XGraph_PT *graph);
extern void       xgraph_deep_free              (XGraph_PT *graph);

/* O(ElgE+lgV) */
extern XGraph_PT  xgraph_copy                   (XGraph_PT graph);

/* O(lgV) */
extern bool       xgraph_add_vertex             (XGraph_PT graph, void *vertex);
/* O(ElgE+lgV) */
extern void       xgraph_remove_vertex          (XGraph_PT graph, void *vertex);

/* O(lgV) */
extern bool       xgraph_is_vertex              (XGraph_PT graph, void *vertex);
/* O(1) */
extern int        xgraph_vertex_size            (XGraph_PT graph);

/* O(lgV) */
extern XSet_PT    xgraph_adjset                 (XGraph_PT graph, void *vertex);
/* O(lgV+lgE) */
extern bool       xgraph_is_adjacent            (XGraph_PT graph, void *vertex1, void *vertex2);

/* O(lgV+lgE) */
extern bool       xgraph_add_edge_unique        (XGraph_PT graph, void *vertex1, void *vertex2);
extern bool       xgraph_add_edge_repeat        (XGraph_PT graph, void *vertex1, void *vertex2);

/* O(lgV+lgE) */
extern void       xgraph_remove_edge            (XGraph_PT graph, void *vertex1, void *vertex2);
extern void       xgraph_remove_edge_all        (XGraph_PT graph, void *vertex1, void *vertex2);

/* O(lgV+lgE) */
extern bool       xgraph_is_edge                (XGraph_PT graph, void *vertex1, void *vertex2);
/* O(VlgE) */
extern int        xgraph_edge_size              (XGraph_PT graph);

/* O(lgV+E) */
extern bool       xgraph_has_parallel_edge      (XGraph_PT graph, void *vertex);
extern int        xgraph_parallel_edge_size     (XGraph_PT graph, void *vertex);
/* O(E(V+lgE)) */
extern bool       xgraph_remove_parallel_edge   (XGraph_PT graph, void *vertex);

/* O(E) */
extern bool       xgraph_has_parallel_edges     (XGraph_PT graph);
extern int        xgraph_parallel_edges_size    (XGraph_PT graph);
/* O(ElgE) */
extern void       xgraph_remove_parallel_edges  (XGraph_PT graph);

/* O(lgV+lgE) */
extern bool       xgraph_has_self_cycle         (XGraph_PT graph, void *vertex);
extern int        xgraph_self_cycle_size        (XGraph_PT graph, void *vertex);
extern void       xgraph_remove_self_cycle      (XGraph_PT graph, void *vertex);

/* O(VlgE) */
extern bool       xgraph_has_self_cycles        (XGraph_PT graph);
extern int        xgraph_self_cycles_size       (XGraph_PT graph);
extern void       xgraph_remove_self_cycles     (XGraph_PT graph);

/* O(VlgE+E) */
extern bool       xgraph_is_simple              (XGraph_PT graph);
/* O(ElgE) */
extern void       xgraph_simple_graph           (XGraph_PT graph);

/* O(ElgV) */
XSet_PT           xgraph_map_union_dfs          (XGraph_PT graph, void *vertex, bool (*apply)(void *vertex, void *parent_vertex, void *cl), void *cl);
extern XSet_PT    xgraph_union_dfs              (XGraph_PT graph, void *vertex);
extern XSet_PT    xgraph_multi_unions_dfs       (XGraph_PT graph, XSet_PT vset);
extern XDList_PT  xgraph_unions_dfs             (XGraph_PT graph);   /* multiple set in list */

/* O(ElgV) */
XSet_PT           xgraph_map_union_bfs          (XGraph_PT graph, void *vertex, bool (*apply)(void *nvertex, void *parent_vertex, void *cl), void *cl);
extern XSet_PT    xgraph_union_bfs              (XGraph_PT graph, void *vertex);
extern XSet_PT    xgraph_multi_unions_bfs       (XGraph_PT graph, XSet_PT vset);
extern XDList_PT  xgraph_unions_bfs             (XGraph_PT graph);   /* multiple set in list */

/* O(ElgV) */
extern XDList_PT  xgraph_path_dfs               (XGraph_PT graph, void *vertex1, void *vertex2);
extern XMap_PT    xgraph_paths_dfs              (XGraph_PT graph, void *vertex);

/* O(ElgV) */
extern XDList_PT  xgraph_path_bfs               (XGraph_PT graph, void *vertex1, void *vertex2);
extern XMap_PT    xgraph_paths_bfs              (XGraph_PT graph, void *vertex);

/* O(ElgV) */
extern XDList_PT  xgraph_shortest_path          (XGraph_PT graph, void *vertex1, void *vertex2);
extern XMap_PT    xgraph_shortest_paths         (XGraph_PT graph, void *vertex);

/* O(ElgV) */
extern XDList_PT  xgraph_path_by_found_paths    (XGraph_PT graph, XMap_PT paths, void *svertex, void *tvertex);

/* O(ElgV) */
extern bool       xgraph_is_connected           (XGraph_PT graph, void *vertex1, void *vertex2);

/* O(V) */
extern bool       xgraph_has_euler_path         (XGraph_PT graph, void *svertex, void *tvertex);
/* O(E) : precondition : xgraph_has_euler_path has been checked */
extern XDList_PT  xgraph_euler_path             (XGraph_PT graph, void *svertex, void *tvertex);

/* O(ElgV) */
extern XDList_PT  xgraph_cycle_include          (XGraph_PT graph, void *vertex);
extern XDList_PT  xgraph_any_cycle_in_union     (XGraph_PT graph, void *vertex);
/* O((V+E)lgV) */
extern XDList_PT  xgraph_any_cycle              (XGraph_PT graph);

/* O(ElgV) */
extern bool       xgraph_has_cycle_include      (XGraph_PT graph, void *vertex);
extern bool       xgraph_has_any_cycle_in_union (XGraph_PT graph, void *vertex);
/* O((V+E)lgV) */
extern bool       xgraph_has_any_cycle          (XGraph_PT graph);

/* O((V+E)lgV) */
extern XDList_PT  xgraph_two_color_dfs          (XGraph_PT graph);
extern XDList_PT  xgraph_two_color_bfs          (XGraph_PT graph);

/* O((V+E)lgV) */
extern bool       xgraph_is_two_color           (XGraph_PT graph);

/* O(lgV) */
extern int        xgraph_degree                 (XGraph_PT graph, void *vertex);
/* O(V) */
extern int        xgraph_max_degree             (XGraph_PT graph);
/* O(VlgE) */
extern double     xgraph_average_degree         (XGraph_PT graph);

/* O(VlgE) */
extern double     xgraph_density                (XGraph_PT graph);

/* O(VlgE) */
extern bool       xgraph_is_dense               (XGraph_PT graph);
extern bool       xgraph_is_sparse              (XGraph_PT graph);

/* O(VlgE+E) */
extern bool       xgraph_is_complete            (XGraph_PT graph);

/* O(VlgE + VlgV + ElgV) */
extern bool       xgraph_is_tree                (XGraph_PT graph);

/* O(ElgV) */
extern XMTree_PT  xgraph_tree_dfs               (XGraph_PT graph, void *vertex);
extern XMTree_PT  xgraph_tree_bfs               (XGraph_PT graph, void *vertex);

/* O((V+E)lgV) */
extern XDList_PT  xgraph_forest_dfs             (XGraph_PT graph);   /* XMTree_PT --> XMTree_PT --> ... */
extern XDList_PT  xgraph_forest_bfs             (XGraph_PT graph);

#ifdef __cplusplus
}
#endif

#endif
