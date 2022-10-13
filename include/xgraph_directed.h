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

#ifndef XDIGRAPH_INCLUDED
#define XDIGRAPH_INCLUDED

#include <stdbool.h>
#include "xtree_set.h"
#include "xqueue_deque.h"
#include "xgraph_undirected.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XGraph_PT XDigraph_PT;

extern XDigraph_PT  xdigraph_new                   (int (*cmp)(void *vertex1, void *vertex2, void *cl), void *cl);

extern void         xdigraph_free                  (XDigraph_PT *graph);
extern void         xdigraph_deep_free             (XDigraph_PT *graph);

extern XDigraph_PT  xdigraph_copy                  (XDigraph_PT graph);

extern XDigraph_PT  xdigraph_reverse               (XDigraph_PT graph);

extern XGraph_PT    xdigraph_to_graph              (XDigraph_PT graph);

extern bool         xdigraph_add_vertex            (XDigraph_PT graph, void *vertex);
extern void         xdigraph_remove_vertex         (XDigraph_PT graph, void *vertex);

extern bool         xdigraph_is_vertex             (XDigraph_PT graph, void *vertex);
extern int          xdigraph_vertex_size           (XDigraph_PT graph);

extern XSet_PT      xdigraph_adjset                (XDigraph_PT graph, void *vertex);
extern bool         xdigraph_is_adjacent           (XDigraph_PT graph, void *svertex, void *tvertex);

extern bool         xdigraph_add_edge_unique       (XDigraph_PT graph, void *svertex, void *tvertex);
extern bool         xdigraph_add_edge_repeat       (XDigraph_PT graph, void *svertex, void *tvertex);

extern void         xdigraph_remove_edge           (XDigraph_PT graph, void *svertex, void *tvertex);
extern void         xdigraph_remove_edge_all       (XDigraph_PT graph, void *svertex, void *tvertex);

extern bool         xdigraph_is_edge               (XDigraph_PT graph, void *svertex, void *tvertex);
extern int          xdigraph_edge_size             (XDigraph_PT graph);

extern bool         xdigraph_has_parallel_edge     (XDigraph_PT graph, void *vertex);
extern int          xdigraph_parallel_edge_size    (XDigraph_PT graph, void *vertex);
extern void         xdigraph_remove_parallel_edge  (XDigraph_PT graph, void *vertex);

extern bool         xdigraph_has_parallel_edges    (XDigraph_PT graph);
extern int          xdigraph_parallel_edges_size   (XDigraph_PT graph);
extern void         xdigraph_remove_parallel_edges (XDigraph_PT graph);

extern bool         xdigraph_has_self_cycle        (XDigraph_PT graph, void *vertex);
extern int          xdigraph_self_cycle_size       (XDigraph_PT graph, void *vertex);
extern void         xdigraph_remove_self_cycle     (XDigraph_PT graph, void *vertex);

extern bool         xdigraph_has_self_cycles       (XDigraph_PT graph);
extern int          xdigraph_self_cycles_size      (XDigraph_PT graph);
extern void         xdigraph_remove_self_cycles    (XDigraph_PT graph);

extern bool         xdigraph_is_simple             (XDigraph_PT graph);
extern void         xdigraph_simple_graph          (XDigraph_PT graph);

extern XDList_PT    xdigraph_path_dfs              (XDigraph_PT graph, void *svertex, void *tvertex);
extern XMap_PT      xdigraph_paths_dfs             (XDigraph_PT graph, void *vertex);

extern XDList_PT    xdigraph_path_bfs              (XDigraph_PT graph, void *svertex, void *tvertex);
extern XMap_PT      xdigraph_paths_bfs             (XDigraph_PT graph, void *vertex);

extern XDList_PT    xdigraph_shortest_path         (XDigraph_PT graph, void *svertex, void *tvertex);
extern XMap_PT      xdigraph_shortest_paths        (XDigraph_PT graph, void *vertex);

extern XDList_PT    xdigraph_path_by_found_paths   (XDigraph_PT graph, XMap_PT paths, void *svertex, void *tvertex);

extern bool         xdigraph_is_reachable              (XDigraph_PT graph, void *svertex, void *tvertex);
extern XSet_PT      xdigraph_reachable_from_dfs        (XDigraph_PT graph, void *vertex);
extern XSet_PT      xdigraph_all_reachable_from_dfs    (XDigraph_PT graph, XSet_PT vset);

extern XDList_PT    xdigraph_cycle_include              (XDigraph_PT graph, void *vertex);
extern XDList_PT    xdigraph_any_cycle_check_from       (XDigraph_PT graph, void *vertex);
extern XDList_PT    xdigraph_any_cycle                  (XDigraph_PT graph);

extern bool         xdigraph_has_cycle_include          (XDigraph_PT graph, void *vertex);
extern bool         xdigraph_has_any_cycle_check_from   (XDigraph_PT graph, void *vertex);
extern bool         xdigraph_has_any_cycle              (XDigraph_PT graph);

extern XDList_PT    xdigraph_two_color_dfs              (XDigraph_PT graph);
extern XDList_PT    xdigraph_two_color_bfs              (XDigraph_PT graph);

extern bool         xdigraph_is_two_color               (XDigraph_PT graph);

extern bool         xdigraph_map_pre_order              (XDigraph_PT graph, bool (*apply)(void *vertex, void *cl), void *cl);
extern bool         xdigraph_map_post_order             (XDigraph_PT graph, bool (*apply)(void *vertex, void *cl), void *cl);
extern bool         xdigraph_map_reverse_post_order     (XDigraph_PT graph, bool (*apply)(void *vertex, void *cl), void *cl);

extern XDList_PT    xdigraph_topologial_order_dfs       (XDigraph_PT graph);

extern XSet_PT      xdigraph_strongly_connected_dfs     (XDigraph_PT graph, void *vertex);
extern XDList_PT    xdigraph_all_strongly_connected_dfs (XDigraph_PT graph);
extern bool         xdigraph_is_strongly_connected_dfs  (XDigraph_PT graph);
extern bool         xdigraph_is_connected_dfs           (XDigraph_PT graph);

extern XDigraph_PT  xdigraph_transitive_closure    (XDigraph_PT graph);

extern int          xdigraph_in_degree             (XDigraph_PT graph, void *vertex);
extern int          xdigraph_out_degree            (XDigraph_PT graph, void *vertex);

extern int          xdigraph_max_in_degree         (XDigraph_PT graph);
extern int          xdigraph_max_out_degree        (XDigraph_PT graph);

extern double       xdigraph_average_degree        (XDigraph_PT graph);

extern double       xdigraph_density               (XDigraph_PT graph);
extern bool         xdigraph_is_dense              (XDigraph_PT graph);
extern bool         xdigraph_is_sparse             (XDigraph_PT graph);

#ifdef __cplusplus
}
#endif

#endif

