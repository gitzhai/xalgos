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

#ifndef XWGRAPH_INCLUDED
#define XWGRAPH_INCLUDED

#include <stdbool.h>
#include "xtree_map.h"
#include "xtree_set.h"
#include "xlist_d.h"
#include "xtree_multiple_branch.h"
#include "xgraph_undirected.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XWGraph *XWGraph_PT;

/* O(1) */
extern XWGraph_PT  xwgraph_new                    (int (*cmp)(void *vertex1, void *vertex2, void *cl), void *cl);

/* O(1) */
extern XGraph_PT   xwgraph_native_graph           (XWGraph_PT graph);  /* We can get all same functions as XGraph_PT by this interface */

extern void        xwgraph_free                   (XWGraph_PT *graph);

/* O(lgV) */
extern bool        xwgraph_add_vertex             (XWGraph_PT graph, void *vertex);
/* O(VlgV) */
extern bool        xwgraph_remove_vertex          (XWGraph_PT graph, void *vertex);

/* O(lgV) */
extern XSet_PT     xwgraph_adjset                 (XWGraph_PT graph, void *vertex);

/* O(lgV + lgE) */
extern bool        xwgraph_add_edge_repeat        (XWGraph_PT graph, void *vertex1, void *vertex2, double weight);
extern bool        xwgraph_add_edge_unique        (XWGraph_PT graph, void *vertex1, void *vertex2, double weight);

/* O(lgV) */
extern void        xwgraph_remove_edge            (XWGraph_PT graph, void *vertex1, void *vertex2);
extern void        xwgraph_remove_edge_all        (XWGraph_PT graph, void *vertex1, void *vertex2);

/* O((E+V)lgV) */
extern XSet_PT     xwgraph_map_union_dfs          (XWGraph_PT graph, void *vertex, bool (*apply)(void *vertex, void *parent_vertex, double weight, void *cl), void *cl);
extern XSet_PT     xwgraph_map_union_bfs          (XWGraph_PT graph, void *vertex, bool (*apply)(void *nvertex, void *parent_vertex, double weight, void *cl), void *cl);

/* O(max(ElgE, VlgV)) */
extern XSet_PT     xwgraph_mst_lazy_prim          (XWGraph_PT graph, void *vertex);
extern XSet_PT     xwgraph_mst_prim               (XWGraph_PT graph, void *vertex);
extern XSet_PT     xwgraph_mst_kruskal            (XWGraph_PT graph, void *vertex);

extern XDList_PT   xwgraph_mst_forest_prim        (XWGraph_PT graph);

extern XMap_PT     xwgraph_spt_dijkstra           (XWGraph_PT graph, void *vertex);
extern XDList_PT   xwgraph_shortest_path_dijkstra (XWGraph_PT graph, void *svertex, void *tvertex);

#ifdef __cplusplus
}
#endif

#endif
