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

#include <stddef.h>
#include <stdlib.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../utils/xutils.h"
#include "../include/xqueue_fifo.h"
#include "../include/xlist_d.h"
#include "../include/xtriple.h"
#include "../include/xqueue_stack.h"
#include "../include/xtree_map.h"
#include "../tree_set/xtree_set_x.h"
#include "../include/xqueue_key_index_priority_min.h"
#include "../include/xtree_multiple_branch.h"
#include "../include/xgraph_undirected.h"
#include "../include/xgraph_weight_edge.h"
#include "xgraph_weight_undirected_x.h"

XWGraph_PT xwgraph_new(int (*cmp)(void *vertex1, void *vertex2, void *cl), void *cl) {
    XWGraph_PT graph = (XWGraph_PT)XMEM_CALLOC(1, sizeof(*graph));
    if (!graph) {
        return NULL;
    }

    graph->wadjsets = xmap_new(cmp, cl);
    if (!graph->wadjsets) {
        XMEM_FREE(graph);
        return NULL;
    }

    graph->native_graph = xgraph_new(cmp, cl);
    if (!graph->native_graph) {
        xmap_free(&graph->wadjsets);
        XMEM_FREE(graph);
        return NULL;
    }

    return graph;
}

XGraph_PT xwgraph_native_graph(XWGraph_PT graph) {
    return graph ? graph->native_graph : NULL;
}

static
bool xwgraph_free_edge_adjsets_apply(void *edge, void *cl) {
    xwedge_free((XWEdge_PT*)&edge);
    return true;
}

static
bool xwgraph_free_vertex_adjsets_apply(void *vertex, void **adjset, void *cl) {
    xset_map(*((XSet_PT*)adjset), xwgraph_free_edge_adjsets_apply, NULL);
    xset_free((XSet_PT*)adjset);
    return true;
}

void xwgraph_free(XWGraph_PT *graph) {
    if (graph && *graph) {
        xgraph_free(&(*graph)->native_graph);

        xmap_map((*graph)->wadjsets, xwgraph_free_vertex_adjsets_apply, NULL);
        xmap_free(&(*graph)->wadjsets);

        XMEM_FREE(*graph);
    }
}

bool xwgraph_add_vertex(XWGraph_PT graph, void *vertex) {
    if(!xgraph_is_vertex(graph ? graph->native_graph : NULL, vertex)) {
        if (!xgraph_add_vertex(graph->native_graph, vertex)) {            
            return false;
        }

        {
            XSet_PT adjset = xset_new(xwedge_cmp_undirected, graph->native_graph->cl);
            if (!adjset) {
                xgraph_remove_vertex(graph->native_graph, vertex);
                return false;
            }

            if (!xmap_put_unique(graph->wadjsets, vertex, adjset)) {
                xgraph_remove_vertex(graph->native_graph, vertex);
                xset_free(&adjset);
                return false;
            }
        }
    }

    return true;
}

XSet_PT xwgraph_adjset(XWGraph_PT graph, void *vertex) {
    return (XSet_PT)xmap_get(graph ? graph->wadjsets : NULL, vertex);
}

bool xwgraph_add_edge_repeat(XWGraph_PT graph, void *vertex1, void *vertex2, double weight) {
    if (!xgraph_add_edge_repeat(graph ? graph->native_graph : NULL, vertex1, vertex2)) {
        return false;
    }

    {
        XSet_PT adjset_vertex1 = xwgraph_adjset(graph, vertex1);
        XSet_PT adjset_vertex2 = xwgraph_adjset(graph, vertex2);

        if (!adjset_vertex1) {
            adjset_vertex1 = xset_new(xwedge_cmp_undirected, graph->native_graph->cl);
            if (!adjset_vertex1) {
                xgraph_remove_edge(graph->native_graph, vertex1, vertex2);
                return false;
            }
            if (!xmap_put_repeat(graph->wadjsets, vertex1, adjset_vertex1)) {
                xset_free(&adjset_vertex1);
                xgraph_remove_edge(graph->native_graph, vertex1, vertex2);
                return false;
            }
        }
        if (!adjset_vertex2) {
            adjset_vertex2 = xset_new(xwedge_cmp_undirected, graph->native_graph->cl);
            if (!adjset_vertex2) {
                xmap_remove(graph->wadjsets, vertex1);
                xset_free(&adjset_vertex1);
                xgraph_remove_edge(graph->native_graph, vertex1, vertex2);
                return false;
            }
            if (!xmap_put_repeat(graph->wadjsets, vertex2, adjset_vertex2)) {
                xmap_remove(graph->wadjsets, vertex1);
                xset_free(&adjset_vertex1);
                xset_free(&adjset_vertex2);
                xgraph_remove_edge(graph->native_graph, vertex1, vertex2);
                return false;
            }
        }

        {
            XWEdge_PT edge = xwedge_new(vertex1, vertex2, weight, graph->native_graph->cmp, graph->native_graph->cl);
            if (!edge) {
                xgraph_remove_edge(graph->native_graph, vertex1, vertex2);
                return false;
            }

            if (!xset_put_repeat(adjset_vertex1, edge)) {
                xgraph_remove_edge(graph->native_graph, vertex1, vertex2);
                return false;
            }

            /* add vertex1 into the adjset of vertex2 if vertex1 is not equal to vertex2 */
            if (graph->native_graph->cmp(vertex1, vertex2, graph->native_graph->cl) != 0) {
                XWEdge_PT edge2 = xwedge_new(vertex1, vertex2, weight, graph->native_graph->cmp, graph->native_graph->cl);
                if (!edge2) {
                    xset_remove(adjset_vertex1, edge);
                    xwedge_free(&edge);
                    xgraph_remove_edge(graph->native_graph, vertex1, vertex2);
                    return false;
                }

                if (!xset_put_repeat(adjset_vertex2, edge2)) {
                    xset_remove(adjset_vertex1, edge);
                    xwedge_free(&edge);
                    xgraph_remove_edge(graph->native_graph, vertex1, vertex2);
                    return false;
                }
            }
        }
    }

    return true;
}

bool xwgraph_add_edge_unique(XWGraph_PT graph, void *vertex1, void *vertex2, double weight) {
    if (xgraph_is_edge(graph ? graph->native_graph : NULL, vertex1, vertex2)) {
        return true;
    }

    return xwgraph_add_edge_repeat(graph, vertex1, vertex2, weight);
}

static
bool xwgraph_map_union_dfs_impl(XWGraph_PT graph, void *vertex, void *parent, double weight, XWGraph_6Paras_PT paras);

static
bool xwgraph_map_union_dfs_impl_apply(void *nedge, void *cl) {
    XWGraph_6Paras_PT paras = (XWGraph_6Paras_PT)cl;

    void *parent = xstack_peek((XStack_PT)paras->para2/*vertex stack*/);
    XWEdge_PT edge = (XWEdge_PT)nedge;
    void *nvertex = xwedge_other(edge, parent);

    if (!xset_find((XSet_PT)(paras->para1/*marked*/), nvertex)) {
        return xwgraph_map_union_dfs_impl(paras->graph, nvertex, parent, edge->weight, paras);
    }

    return true;
}

static
bool xwgraph_map_union_dfs_impl(XWGraph_PT graph, void *vertex, void *parent, double weight, XWGraph_6Paras_PT paras) {
    if (!xset_put_repeat((XSet_PT)paras->para1/*marked*/, vertex)) {
        return false;
    }

    bool (*apply)(void *vertex, void *parent_vertex, double weight, void *cl) = paras->para3 /*apply*/;
    if (apply && !apply(vertex, parent, weight, paras->para4/*cl*/)) {
        return false;
    }

    /* record the previous (parent) vertex */
    if (!xstack_push_no_limit((XStack_PT)paras->para2/*vertex stack*/, vertex)) {
        return false;
    }

    if (xset_map_break_if_false(xwgraph_adjset(graph, vertex), xwgraph_map_union_dfs_impl_apply, (void*)paras)) {
        return false;
    }

    xstack_pop((XStack_PT)paras->para2/*vertex stack*/);

    return true;
}

XSet_PT xwgraph_map_union_dfs(XWGraph_PT graph, void *vertex, bool (*apply)(void *vertex, void *parent_vertex, double weight, void *cl), void *cl) {
    if (!xgraph_is_vertex(graph ? graph->native_graph : NULL, vertex)) {
        return NULL;
    }

    {
        XSet_PT marked = xset_new(graph->native_graph->cmp, graph->native_graph->cl);
        XStack_PT stack = xstack_new(xgraph_vertex_size(graph->native_graph));
        if (!marked || !stack) {
            xset_free(marked ? &marked : NULL);
            xstack_free(stack ? &stack : NULL);
            return NULL;
        }

        {
            XWGraph_6Paras_T paras = { graph, marked, stack, apply, cl, NULL, NULL };

            if (!xwgraph_map_union_dfs_impl(graph, vertex, NULL, 0, &paras)) {
                xset_free(&marked);
                xstack_free(&stack);
                return NULL;
            }
        }

        xstack_free(&stack);
        return marked;
    }
}

static
bool xwgraph_map_union_bfs_apply(void *nedge, void *cl) {
    XWGraph_6Paras_PT paras = (XWGraph_6Paras_PT)cl;

    void *parent = paras->para3/*parent vertext*/;
    XWEdge_PT edge = (XWEdge_PT)nedge;
    void *nvertex = xwedge_other(edge, parent);

    if (!xset_find((XSet_PT)(paras->para1/*marked*/), nvertex)) {
        if (!xset_put_repeat((XSet_PT)paras->para1/*marked*/, nvertex)) {
            return false;
        }

        bool (*apply)(void *vertex, void *parent, double weight, void *cl) = paras->para4 /*apply*/;
        if (apply && !apply(nvertex, parent, edge->weight, paras->para5/*cl*/)) {
            return false;
        }

        return xfifo_push_no_limit((XFifo_PT)(paras->para2/*fifo*/), nvertex);
    }

    return true;
}

static
void* xwgraph_map_union_bfs_free(XSet_PT* set, XFifo_PT* fifo) {
    xset_free(set);
    xfifo_free(fifo);
    return NULL;
}

XSet_PT xwgraph_map_union_bfs(XWGraph_PT graph, void *vertex, bool (*apply)(void *nvertex, void *parent_vertex, double weight, void *cl), void *cl) {
    if (!xgraph_is_vertex(graph ? graph->native_graph : NULL, vertex)) {
        return NULL;
    }

    if (apply && !apply(vertex, NULL, 0, cl)) {
        return NULL;
    }

    {
        XSet_PT marked = xset_new(graph->native_graph->cmp, graph->native_graph->cl);
        XFifo_PT fifo = xfifo_new(xgraph_vertex_size(graph->native_graph));

        if (!marked || !fifo) {
            return xwgraph_map_union_bfs_free((marked ? &marked : NULL), (fifo ? &fifo : NULL));
        }

        if (!xset_put_repeat(marked, vertex)) {
            return xwgraph_map_union_bfs_free((marked ? &marked : NULL), (fifo ? &fifo : NULL));
        }

        if (!xfifo_push_no_limit(fifo, vertex)) {
            return xwgraph_map_union_bfs_free(&marked, &fifo);
        }

        while (!xfifo_is_empty(fifo)) {
            vertex = xfifo_pop(fifo);

            {
                XWGraph_6Paras_T paras = { graph, marked, fifo, vertex/*parent vertex*/, apply, cl, NULL };

                if (xset_map_break_if_false(xwgraph_adjset(graph, vertex), xwgraph_map_union_bfs_apply, (void*)&paras)) {
                    return xwgraph_map_union_bfs_free(&marked, &fifo);
                }
            }
        }

        xfifo_free(&fifo);
        return marked;
    }
}

static
bool xwgraph_mst_lazy_prim_impl_apply(void *edge, void *cl) {
    XWGraph_3Paras_PT paras = (XWGraph_3Paras_PT)cl;

    void *nvertex = xwedge_other((XWEdge_PT)edge, paras->para3/*parent*/);

    /* only save the edge which has the vertex not in MST */
    if (xset_find((XSet_PT)paras->para1/*marked*/, nvertex)) {
        return xminpq_push((XMinPQ_PT)paras->para2/*queue*/, edge);
    }

    return true;
}

static
bool xwgraph_mst_lazy_prim_impl(XWGraph_PT graph, XSet_PT marked, XSet_PT mst, XMinPQ_PT queue, XWGraph_3Paras_PT paras) {
    XWEdge_PT edge = NULL;
    void *other = NULL;

    while (!xminpq_is_empty(queue)) {
        edge = xminpq_pop(queue);

        /* save this edge into MST if one of the vertex is not in mst */
        other = NULL;
        if (xset_find(marked, edge->v)) {
            other = edge->v;
        }
        else if (xset_find(marked, edge->w)) {
            other = edge->w;
        }

        if (other) {
            /* save this edge into MST */
            if (!xset_put_repeat(mst, edge)) {
                return false;
            }

            /* put the vertex into MST */
            xset_remove(marked, other);

            /* finish building the mst if all vertexes are in MST already */
            if (xset_is_empty(marked)) {
                return true;
            }

            /* copy all the edges in adjset into queue */
            paras->para3/*parent*/ = other;
            if (xset_map_break_if_false(xwgraph_adjset(graph, other), xwgraph_mst_lazy_prim_impl_apply, (void*)paras)) {
                return false;
            }
        }

        /*ignore the edges if both vertexes are in mst already*/
    }

    return true;
}

static 
void* xwgraph_mst_lazy_prim_free(XSet_PT marked, XSet_PT mst, XMinPQ_PT queue) {
    xset_free(marked ? &marked : NULL);
    xset_free(mst ? &mst : NULL);
    xminpq_free(queue ? &queue : NULL);
    return NULL;
}

XSet_PT xwgraph_mst_lazy_prim(XWGraph_PT graph, void *vertex) {
    /* find the union at first to define all the mst vertexes */
    XSet_PT marked = xgraph_union_dfs(graph ? graph->native_graph : NULL, vertex);
    if (!marked) {
        return NULL;
    }

    {
        XSet_PT mst = xset_new(xwedge_cmp_weight_only, graph->native_graph->cl);
        XMinPQ_PT queue = xminpq_new(xgraph_edge_size(graph->native_graph), xwedge_cmp_weight_only, graph->native_graph->cl);
        if (!mst || !queue) {
            return xwgraph_mst_lazy_prim_free(marked, mst, queue);
        }

        {
            int total = xset_size(marked);
            XWGraph_3Paras_T paras = { graph, marked, queue, vertex/*parent*/};

            /* step 1 : put the source vertex into MST */
            xset_remove(marked, vertex);

            /* step 2.1 : copy all the valid edges in adjset into queue */
            if (xset_map_break_if_false(xwgraph_adjset(graph, vertex), xwgraph_mst_lazy_prim_impl_apply, (void*)&paras)) {
                return xwgraph_mst_lazy_prim_free(marked, mst, queue);
            }

            /* step 2.2 : find the valid edge which has minimum weight */
            if(!xwgraph_mst_lazy_prim_impl(graph, marked, mst, queue, &paras)) {
                return xwgraph_mst_lazy_prim_free(marked, mst, queue);
            }

            // verify the edge number in MST
            xassert(xset_size(mst) == (total - 1));
        }

        xminpq_free(&queue);
        xset_free(&marked);

        return mst;
    }
}

static
bool xwgraph_mst_prim_impl_apply(void *edge, void *cl) {
    XWGraph_3Paras_PT paras = (XWGraph_3Paras_PT)cl;

    XSet_PT marked = (XSet_PT)paras->para1/*marked*/;
    XKeyIndexMinPQ_PT queue = (XKeyIndexMinPQ_PT)paras->para2/*queue*/;

    void *parent = paras->para3/*parent*/;
    void *nvertex = xwedge_other((XWEdge_PT)edge, parent);

    /* check if the edge is not in mst and has minimum weight to mst */
    if (xset_find(marked, nvertex)) {
        XWEdge_PT edge1 = (XWEdge_PT)edge;

        XWEdge_PT nedge = (XWEdge_PT)xkeyindexminpq_get(queue, nvertex);
        if (!nedge) {
            /* save the first edge from nvertex to MST */
            return xkeyindexminpq_push(queue, nvertex, edge, NULL);
        }

        /* there is edge from nverex to MST already, keep the edge which has smaller weight */
        if (xwedge_cmp_weight_only(edge, nedge, paras->graph->native_graph->cl) < 0) {
            return xkeyindexminpq_push(queue, nvertex, edge, NULL);
        }

        /* ignore the edges which has higher weight to mst */
    }

    return true;
}

static
bool xwgraph_mst_prim_impl(XWGraph_PT graph, XSet_PT marked, XSet_PT mst, XKeyIndexMinPQ_PT queue, XWGraph_3Paras_PT paras) {
    XWEdge_PT edge = NULL;
    void *other = NULL;

    while (!xkeyindexminpq_is_empty(queue)) {
        xkeyindexminpq_pop(queue, &other, (void**)&edge);

        if (!xset_put_repeat(mst, edge)) {
            return false;
        }

        /* put the vertex into MST */
        xset_remove(marked, other);

        /* finish building the mst if all vertexes are in MST already */
        if (xset_is_empty(marked)) {
            return true;
        }

        /* copy the edges which has minimum weight to MST in adjset into queue */
        paras->para3/*parent*/ = other;
        if (xset_map_break_if_false(xwgraph_adjset(graph, other), xwgraph_mst_prim_impl_apply, (void*)paras)) {
            return false;
        }
    }

    return true;
}

static
void* xwgraph_mst_prim_free(XSet_PT marked, XSet_PT mst, XKeyIndexMinPQ_PT queue) {
    xset_free(marked ? &marked : NULL);
    xset_free(mst ? &mst : NULL);
    xkeyindexminpq_free(queue ? &queue : NULL);
    return NULL;
}

XSet_PT xwgraph_mst_prim(XWGraph_PT graph, void *vertex) {
    /* find the union at first to define all the mst vertexes */
    XSet_PT marked = xgraph_union_dfs(graph ? graph->native_graph : NULL, vertex);
    if (!marked) {
        return NULL;
    }

    {
        XSet_PT mst = xset_new(xwedge_cmp_weight_only, graph->native_graph->cl);
        XKeyIndexMinPQ_PT queue = xkeyindexminpq_new(graph->native_graph->cmp, xwedge_cmp_undirected, graph->native_graph->cl);
        if (!mst || !queue) {
            return xwgraph_mst_prim_free(marked, mst, queue);
        }

        {
            int total = xset_size(marked);
            XWGraph_3Paras_T paras = { graph, marked, queue, vertex/*parent*/ };

            /* step 1 : put the vertex into MST */
            xset_remove(marked, vertex);

            /* step 2.1 : copy the edges which has minimum weight to MST in adjset into queue */
            if (xset_map_break_if_false(xwgraph_adjset(graph, vertex), xwgraph_mst_prim_impl_apply, (void*)&paras)) {
                return xwgraph_mst_prim_free(marked, mst, queue);
            }

            /* step 2.2 : find the valid edge which has minimum weight */
            if (!xwgraph_mst_prim_impl(graph, marked, mst, queue, &paras)) {
                return xwgraph_mst_prim_free(marked, mst, queue);
            }

            // verify the edge number in MST
            xassert(xset_size(mst) == (total - 1));
        }

        xkeyindexminpq_free(&queue);
        xset_free(&marked);

        return mst;
    }
}

static 
bool xwgraph_mst_forest_prim_apply(void *vertex, void **adjset, void *cl) {
    XWGraph_3Paras_PT paras = (XWGraph_3Paras_PT)cl;
    XDList_PT msts = paras->para1/*msts*/;
    XSet_PT marked = paras->para2/*marked*/;

    if (xset_find(marked, vertex)) {
        return true;
    }

    {
        XSet_PT nmarked = xgraph_union_dfs(paras->graph->native_graph, vertex);
        if (!nmarked) {
            return false;
        }
        if (!xset_merge_repeat(marked, &nmarked)) {
            xset_free(&nmarked);
            return false;
        }

        {
            XSet_PT mst = xwgraph_mst_prim(paras->graph, vertex);
            if (!mst) {
                return false;
            }

            if (!xdlist_push_back_repeat(msts, mst)) {
                xset_free(&mst);
                return false;
            }
        }
    }

    return true;
}

static 
bool xwgraph_mst_forest_prim_free_apply(void *set, void *cl) {
    xset_free((XSet_PT*)&set);
    return true;
}

XDList_PT xwgraph_mst_forest_prim(XWGraph_PT graph) {
    if (!graph) {
        return NULL;
    }

    {
        XDList_PT msts = xdlist_new();
        XSet_PT marked = xset_new(graph->native_graph->cmp, graph->native_graph->cl);
        if (!msts || !marked) {
            xdlist_free(msts ? &msts : NULL);
            xset_free(marked ? &marked : NULL);
            return NULL;
        }

        {
            XWGraph_3Paras_T paras = { graph, msts, marked, NULL };

            if (xmap_map_break_if_false(graph->wadjsets, xwgraph_mst_forest_prim_apply, (void*)&paras)) {
                xdlist_map(msts, xwgraph_mst_forest_prim_free_apply, NULL);
                xdlist_free(&msts);
                xset_free(&marked);
                return NULL;
            }
        }

        xset_free(&marked);
        return msts;
    }
}

static 
bool xwgraph_mst_kruskal_adjset_apply(void *elem, void *cl) {
    return xminpq_push((XMinPQ_PT)cl, elem);
}

static 
bool xwgraph_mst_kruskal_marked_set_apply(void *vertex, void *cl) {
    XWGraph_3Paras_PT paras = (XWGraph_3Paras_PT)cl;
    return !xset_map_break_if_false(xwgraph_adjset(paras->graph, vertex), xwgraph_mst_kruskal_adjset_apply, paras->para1/*queue*/);
}

static
void* xwgraph_mst_kruskal_free(XSet_PT marked, XSet_PT mst, XMinPQ_PT queue, XGraph_PT graph) {
    xset_free(marked ? &marked : NULL);
    xset_free(mst ? &mst : NULL);
    xminpq_free(queue ? &queue : NULL);
    xgraph_free(graph ? &graph : NULL);
    return NULL;
}

XSet_PT xwgraph_mst_kruskal(XWGraph_PT graph, void *vertex) {
    /* find the union at first to define all the mst vertexes */
    XSet_PT marked = xgraph_union_dfs(graph ? graph->native_graph : NULL, vertex);
    if (!marked) {
        return NULL;
    }

    {
        XMinPQ_PT queue = xminpq_new(2*xgraph_edge_size(graph->native_graph), xwedge_cmp_weight_only, graph->native_graph->cl);
        if (!queue) {
            xset_free(&marked);
            return NULL;
        }

        {
            /* step 1 : put all edges into queue */
            XWGraph_3Paras_T paras = { graph, queue, NULL, NULL };
            if (xset_map_break_if_false(marked, xwgraph_mst_kruskal_marked_set_apply, (void*)&paras)) {
                return xwgraph_mst_kruskal_free(marked, NULL, queue, NULL);
            }
        }

        {
            int total = xset_size(marked) - 1;

            XSet_PT mst = xset_new(xwedge_cmp_undirected, graph->native_graph->cl);
            XGraph_PT tgraph = xgraph_new(graph->native_graph->cmp, graph->native_graph->cl);
            if (!mst || !tgraph) {
                return xwgraph_mst_kruskal_free(marked, mst, queue, tgraph);
            }

            /* find the edge which is not in mst and has the minimum weight*/
            XWEdge_PT edge = NULL;
            while (0 < total) {
                /* step 2 : get the edge which has the minimum weight */
                edge = xminpq_pop(queue);

                /* ignore the self-cycle */
                if (graph->native_graph->cmp(edge->v, edge->w, graph->native_graph->cl) == 0) {
                    continue;
                }

                /* step 3 : only the vertexes which is not connected will be put in mst */
                if (!xgraph_is_connected(tgraph, edge->v, edge->w)) {
                    /* step 4 : add edge to graph for next time connection check */
                    /* step 5 : save the edge into mst */
                    if (!xgraph_add_edge_repeat(tgraph, edge->v, edge->w) || !xset_put_repeat(mst, edge)) {
                        return xwgraph_mst_kruskal_free(marked, mst, queue, tgraph);
                    }

                    /* one edge added into mst already */
                    total--;
                }

                /* ignore the edges which will create a cycle to mst if added into mst */
            }

            xgraph_free(&tgraph);
            xminpq_free(&queue);
            xset_free(&marked);

            return mst;
        }
    }
}

static
bool xwgraph_spt_free_dist(void *vertex, void **dist, void *cl) {
    XMEM_FREE(*dist);
    return true;
}

static
void* xwgraph_spt_free(XSet_PT marked, XMap_PT edge_to, XMap_PT dist_to) {
    xset_free(marked ? &marked : NULL);
    xmap_free(edge_to ? &edge_to : NULL);
    xmap_map(dist_to, xwgraph_spt_free_dist, NULL);
    xmap_free(dist_to ? &dist_to : NULL);
    return NULL;
}

static
bool xwdigraph_spt_impl_dijkstra_impl_apply(void *elem, void *cl) {
    XWEdge_PT edge = elem;

    // check 0 < edge->weight 
    xassert(1e-15 < edge->weight);
    if (edge->weight <= 1e-15) {
        return false;
    }

    {
        XWGraph_6Paras_PT paras = (XWGraph_6Paras_PT)cl;
        XSet_PT marked = (XSet_PT)paras->para1/*marked*/;

        /* ignore the edge if both vertexes are in SPT already */
        if (!xset_find(marked, edge->w)) {
            return true;
        }

        {
            XMap_PT edge_to = (XMap_PT)paras->para2/*edge_to*/;
            XMap_PT dist_to = (XMap_PT)paras->para3/*dist_to*/;
            XMinPQ_PT queue = (XMinPQ_PT)paras->para4/*queue*/;

            /* check if the edge has shortest path */
            double *v_dist = xmap_get(dist_to, edge->v);
            double *w_dist = xmap_get(dist_to, edge->w);

            if (!w_dist) {
                w_dist = XMEM_CALLOC(1, sizeof(*w_dist));
                if (!w_dist) {
                    return false;
                }
                *w_dist = edge->weight + (v_dist ? *v_dist : 0);

                if (!xmap_put_repeat(dist_to, edge->w, (void*)w_dist)) {
                    XMEM_FREE(w_dist);
                    return false;
                }

                XTriple_PT triple = xtriple_new(edge->w, edge, w_dist);
                if (!triple) {
                    return false;
                }
                if (!xminpq_push(queue, triple)) {
                    xtriple_free(&triple);
                    return false;
                }

                return true;
            }
            /* relex the edge */
            else if (v_dist && (*v_dist + edge->weight < *w_dist)) {
                *w_dist = *v_dist + edge->weight;

                if (!xmap_put_replace(dist_to, edge->w, (void*)w_dist, NULL)) {
                    return false;
                }

                // update the edge->weight for queue sort
                XTriple_PT triple = xtriple_new(edge->w, edge, w_dist);
                if (!triple) {
                    return false;
                }
                //TODO : create a new key index priority queue to solve this update but not push again
                if (!xminpq_push(queue, triple)) {
                    return false;
                }
                return true;
            }

            /* ignore the edges which has higher weight */
        }
    }

    return true;
}

static
int xwgraph_spt_cmp_weight(void *triple1, void *triple2, void *cl) {
    XTriple_PT t1 = (XTriple_PT)triple1;
    XTriple_PT t2 = (XTriple_PT)triple2;

    double *v1 = t1->third;
    double *v2 = t2->third;

    if (*v1 < *v2) {
        return -1;
    }
    else if (*v1 < *v2) {
        return 1;
    }
    else {
        return 0;
    }
}

static
bool xwgraph_spt_queue_free(void **triple, void *cl) {
    xtriple_free((XTriple_PT*)triple);
    return true;
}

static
bool xwdigraph_spt_impl_dijkstra_impl(XWGraph_PT graph, XSet_PT marked, XMap_PT edge_to, XMap_PT dist_to, void *svertex, void *tvertex) {
    XMinPQ_PT queue = xminpq_new(xgraph_edge_size(graph->native_graph), xwgraph_spt_cmp_weight, graph->native_graph->cl);
    if (!queue) {
        return false;
    }

    {
        XWGraph_6Paras_T paras = { graph, marked, edge_to, dist_to, queue, NULL, NULL };

        /* step 1 : put the vertex into SPT */
        xset_remove(marked, svertex);

        /* step 2.1 : scan the edges which has the shortest dist to vertex in adjset */
        if (xset_map_break_if_false(xwgraph_adjset(graph, svertex), xwdigraph_spt_impl_dijkstra_impl_apply, (void*)&paras)) {
            xminpq_free_apply(&queue, xwgraph_spt_queue_free, NULL);
            return false;
        }

        /* step 2.2 : find the path which has the shortest dist to source vertex */
        XWEdge_PT edge = NULL;
        XTriple_PT triple = NULL;
        while (!xminpq_is_empty(queue)) {
            triple = xminpq_pop(queue);
            if (!triple) {
                xminpq_free_apply(&queue, xwgraph_spt_queue_free, NULL);
                return false;
            }

            svertex = triple->first;
            edge = triple->second;

            xtriple_free(&triple);
            if (!xset_find(marked, svertex)) {
                continue;
            }

            /* put the vertex into SPT */
            xset_remove(marked, svertex);

            /* reach the target vertex already */
            if (tvertex && (graph->native_graph->cmp(svertex, tvertex, graph->native_graph->cl) == 0)) {
                break;
            }

            if (!xmap_put_replace(edge_to, svertex, edge, NULL)) {
                return false;
            }

            /* finish building the SPT if all vertexes are in SPT already */
            if (xset_is_empty(marked)) {
                break;
            }

            /* step 2.1 : scan the edges which has the shortest dist to vertex in adjset */
            if (xset_map_break_if_false(xwgraph_adjset(graph, svertex), xwdigraph_spt_impl_dijkstra_impl_apply, (void*)&paras)) {
                xminpq_free_apply(&queue, xwgraph_spt_queue_free, NULL);
                return false;
            }
        }

        xminpq_free_apply(&queue, xwgraph_spt_queue_free, NULL);
    }

    return true;
}

static
XMap_PT xwdigraph_spt_impl_dijkstra(XWGraph_PT graph, void *svertex, void *tvertex) {
    /* find the union at first to define all the spt vertexes */
    XSet_PT marked = xgraph_union_dfs(graph ? graph->native_graph : NULL, svertex);
    if (!marked) {
        return NULL;
    }

    // check if tvertex is reachable or not
    if (tvertex && !xset_find(marked, tvertex)) {
        return NULL;
    }

    {
        XMap_PT edge_to = xmap_new(graph->native_graph->cmp, graph->native_graph->cl);
        XMap_PT dist_to = xmap_new(graph->native_graph->cmp, graph->native_graph->cl);
        if (!edge_to || !dist_to) {
            return xwgraph_spt_free(marked, edge_to, dist_to);
        }

        {
            int total = xset_size(marked);

            if (!xwdigraph_spt_impl_dijkstra_impl(graph, marked, edge_to, dist_to, svertex, tvertex)) {
                return xwgraph_spt_free(marked, edge_to, dist_to);
            }

            xassert(xmap_size(edge_to) == (total - 1));
        }

        xmap_map(dist_to, xwgraph_spt_free_dist, NULL);
        xmap_free(&dist_to);
        xset_free(&marked);

        return edge_to;
    }
}

XMap_PT xwgraph_spt_dijkstra(XWGraph_PT graph, void *vertex) {
    return xwdigraph_spt_impl_dijkstra(graph, vertex, NULL);
}

static
XDList_PT xwgraph_path_by_found_paths_impl(XWGraph_PT graph, XMap_PT paths, void *svertex, void *tvertex) {
    XDList_PT list = xdlist_new();
    if (!list) {
        return NULL;
    }

    {
        int size = xmap_size(paths);
        void *vertex = tvertex;

        do {
            XWEdge_PT edge = xmap_get(paths, vertex);
            if (!edge || !xdlist_push_front_repeat(list, (void*)edge)) {
                xdlist_free(&list);
                return NULL;
            }

            vertex = edge->v;
            --size;
        } while ((graph->native_graph->cmp(vertex, svertex, graph->native_graph->cl) != 0) && (0 < size));

        /* error found : size<=0 now, but vertex is not the svertex */
        if (graph->native_graph->cmp(vertex, svertex, graph->native_graph->cl) != 0) {
            xdlist_free(&list);
            return NULL;
        }
    }

    return list;
}

XDList_PT xwgraph_shortest_path_dijkstra(XWGraph_PT graph, void *svertex, void *tvertex) {
    XMap_PT paths = xwdigraph_spt_impl_dijkstra(graph, svertex, tvertex);
    if (paths) {
        /* output the path */
        XDList_PT list = xwgraph_path_by_found_paths_impl(graph, paths, svertex, tvertex);
        xmap_free(&paths);
        return list;
    }

    return NULL;
}
