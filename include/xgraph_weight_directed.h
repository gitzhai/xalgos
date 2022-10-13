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

#ifndef XWDIGRAPH_INCLUDED
#define XWDIGRAPH_INCLUDED

#include <stdbool.h>
#include "xtree_map.h"
#include "xtree_set.h"
#include "xlist_d.h"
#include "xtree_multiple_branch.h"
#include "../include/xgraph_directed.h"
#include "xgraph_weight_undirected.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XWGraph_PT XWDigraph_PT;

/* O(1) */
extern XWDigraph_PT  xwdigraph_new                    (int (*cmp)(void *vertex1, void *vertex2, void *cl), void *cl);

/* O(1) */
extern XDigraph_PT   xwgraph_native_digraph           (XWDigraph_PT graph);   /* We can get all same functions as XDigraph_PT by this interface */

extern void          xwdigraph_free                   (XWDigraph_PT *graph);
extern void          xwdigraph_deep_free              (XWDigraph_PT *graph);

extern XWDigraph_PT  xwdigraph_copy                   (XWDigraph_PT graph);

/* O(lgV) */
extern bool          xwdigraph_add_vertex             (XWDigraph_PT graph, void *vertex);
/* O(VlgV) */
extern bool          xwdigraph_remove_vertex          (XWDigraph_PT graph, void *vertex);

/* O(lgV) */
extern XSet_PT       xwdigraph_adjset                 (XWDigraph_PT graph, void *vertex);

/* O(lgV + lgE) */
extern bool          xwdigraph_add_edge_repeat        (XWDigraph_PT graph, void *svertex, void *tvertex, double weight);
extern bool          xwdigraph_add_edge_unique        (XWDigraph_PT graph, void *svertex, void *tvertex, double weight);

/* O((E+V)lgV) */
extern XSet_PT       xwdigraph_map_union_dfs          (XWDigraph_PT graph, void *vertex, bool (*apply)(void *vertex, void *parent_vertex, double weight, void *cl), void *cl);
extern XSet_PT       xwdigraph_map_union_bfs          (XWDigraph_PT graph, void *vertex, bool (*apply)(void *nvertex, void *parent_vertex, double weight, void *cl), void *cl);

/* O(VlgV) */
extern XDList_PT     xwdigraph_shortest_path_dijkstra (XWGraph_PT graph, void *svertex, void *tvertex);
extern XMap_PT       xwdigraph_spt_dijkstra           (XWGraph_PT graph, void *vertex);   /* spt : shortest path tree */

extern XDList_PT     xwdigraph_shortest_path_no_cycle (XWDigraph_PT graph, void *svertex, void *tvertex);
extern XMap_PT       xwdigraph_spt_no_cycle           (XWDigraph_PT graph, void *vertex);

extern XDList_PT     xwdigraph_longest_path_no_cycle  (XWDigraph_PT graph, void *svertex, void *tvertex);
extern XMap_PT       xwdigraph_lpt_no_cycle           (XWDigraph_PT graph, void *vertex); /* lpt : longest path tree */

extern XMap_PT       xwdigraph_spt_bellman_ford       (XWDigraph_PT graph, void *vertex);

extern XMap_PT       xwdigraph_spt_bellman_ford_negative_cycle(XWDigraph_PT graph, void *vertex);
extern XDList_PT     xwdigraph_negative_cycle         (XWDigraph_PT graph, XMap_PT edge_to);

/* O(VlgV) */
extern XDList_PT     xwdigraph_path_by_found_paths    (XWDigraph_PT graph, XMap_PT paths, void *svertex, void *tvertex);

#ifdef __cplusplus
}
#endif

#endif
