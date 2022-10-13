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
#include "../include/xqueue_stack.h"
#include "../include/xtriple.h"
#include "../include/xtree_map.h"
#include "../tree_set/xtree_set_x.h"
#include "../include/xtree_multiple_branch.h"
#include "../include/xgraph_directed.h"
#include "../include/xgraph_weight_edge.h"
#include "../include/xgraph_weight_undirected.h"
#include "../include/xgraph_weight_directed.h"
#include "xgraph_weight_directed_x.h"

XWDigraph_PT xwdigraph_new(int (*cmp)(void *vertex1, void *vertex2, void *cl), void *cl) {
    return xwgraph_new(cmp, cl);
}

XDigraph_PT xwgraph_native_digraph(XWDigraph_PT graph) {
    return graph ? graph->native_graph : NULL;
}

void xwdigraph_free(XWDigraph_PT *graph) {
    xwgraph_free(graph);
}

bool xwdigraph_add_vertex(XWDigraph_PT graph, void *vertex) {
    return xwgraph_add_vertex(graph, vertex);
}

XSet_PT xwdigraph_adjset(XWDigraph_PT graph, void *vertex) {
    return xmap_get(graph ? graph->wadjsets : NULL, vertex);
}

bool xwdigraph_add_edge_repeat(XWDigraph_PT graph, void *svertex, void *tvertex, double weight) {
    if (!xdigraph_add_edge_repeat(graph ? graph->native_graph : NULL, svertex, tvertex)) {
        return false;
    }

    {
        XSet_PT adjset_vertex = xwdigraph_adjset(graph, svertex);
        if (!adjset_vertex) {
            adjset_vertex = xset_new(xwedge_cmp_directed, graph->native_graph->cl);
            if (!adjset_vertex) {
                return false;
            }

            if (!xmap_put_repeat(graph->wadjsets, svertex, adjset_vertex)) {
                xset_free(&adjset_vertex);
                return false;
            }
        }

        {
            XWEdge_PT edge = xwedge_new(svertex, tvertex, weight, graph->native_graph->cmp, graph->native_graph->cl);
            if (!edge) {
                xdigraph_remove_edge(graph->native_graph, svertex, tvertex);
                return false;
            }

            if (!xset_put_repeat(adjset_vertex, edge)) {
                xdigraph_remove_edge(graph->native_graph, svertex, tvertex);
                return false;
            }
        }
    }

    return true;
}

bool xwdigraph_add_edge_unique(XWDigraph_PT graph, void *svertex, void *tvertex, double weight) {
    if (xdigraph_is_edge(graph ? graph->native_graph : NULL, svertex, tvertex)) {
        return true;
    }

    return xwdigraph_add_edge_repeat(graph, svertex, tvertex, weight);
}

static
bool xwdigraph_map_union_dfs_impl(XWDigraph_PT graph, void *vertex, void *parent, double weight, XWDigraph_3Paras_PT paras);

static
bool xwdigraph_map_union_dfs_impl_apply(void *nedge, void *cl) {
    XWDigraph_3Paras_PT paras = (XWDigraph_3Paras_PT)cl;
    XWEdge_PT edge = (XWEdge_PT)nedge;

    if (!xset_find((XSet_PT)(paras->para1/*marked*/), edge->w)) {
        return xwdigraph_map_union_dfs_impl(paras->graph, edge->w, edge->v, edge->weight, paras);
    }

    return true;
}

static
bool xwdigraph_map_union_dfs_impl(XWDigraph_PT graph, void *vertex, void *parent, double weight, XWDigraph_3Paras_PT paras) {
    if (!xset_put_repeat((XSet_PT)paras->para1/*marked*/, vertex)) {
        return false;
    }

    bool (*apply)(void *vertex, void *parent_vertex, double weight, void *cl) = paras->para2 /*apply*/;
    if (apply && !apply(vertex, parent, weight, paras->para3/*cl*/)) {
        return false;
    }

    if (xset_map_break_if_false(xwdigraph_adjset(graph, vertex), xwdigraph_map_union_dfs_impl_apply, (void*)paras)) {
        return false;
    }

    return true;
}

XSet_PT xwdigraph_map_union_dfs(XWDigraph_PT graph, void *vertex, bool (*apply)(void *vertex, void *parent_vertex, double weight, void *cl), void *cl) {
    if (!xdigraph_is_vertex(graph ? graph->native_graph : NULL, vertex)) {
        return NULL;
    }

    {
        XSet_PT marked = xset_new(graph->native_graph->cmp, graph->native_graph->cl);
        if (!marked) {
            return NULL;
        }

        {
            XWDigraph_3Paras_T paras = { graph, marked, apply, cl };

            if (!xwdigraph_map_union_dfs_impl(graph, vertex, NULL, 0, &paras)) {
                xset_free(&marked);
                return NULL;
            }
        }

        return marked;
    }
}

static
bool xwdigraph_map_union_bfs_apply(void *nedge, void *cl) {
    XWDigraph_6Paras_PT paras = (XWDigraph_6Paras_PT)cl;
    XWEdge_PT edge = (XWEdge_PT)nedge;

    if (!xset_find((XSet_PT)(paras->para1/*marked*/), edge->w)) {
        if (!xset_put_repeat((XSet_PT)paras->para1/*marked*/, edge->w)) {
            return false;
        }

        bool (*apply)(void *vertex, void *parent, double weight, void *cl) = paras->para3 /*apply*/;
        if (apply && !apply(edge->w, edge->v, edge->weight, paras->para4/*cl*/)) {
            return false;
        }

        return xfifo_push_no_limit((XFifo_PT)(paras->para2/*fifo*/), edge->w);
    }

    return true;
}

static
void* xwdigraph_map_union_bfs_free(XSet_PT* set, XFifo_PT* fifo) {
    xset_free(set);
    xfifo_free(fifo);
    return NULL;
}

XSet_PT xwdigraph_map_union_bfs(XWDigraph_PT graph, void *vertex, bool (*apply)(void *nvertex, void *parent_vertex, double weight, void *cl), void *cl) {
    if (!xdigraph_is_vertex(graph ? graph->native_graph : NULL, vertex)) {
        return NULL;
    }

    if (apply && !apply(vertex, NULL, 0, cl)) {
        return NULL;
    }

    {
        XSet_PT marked = xset_new(graph->native_graph->cmp, graph->native_graph->cl);
        XFifo_PT fifo = xfifo_new(xdigraph_vertex_size(graph->native_graph));

        if (!marked || !fifo) {
            return xwdigraph_map_union_bfs_free((marked ? &marked : NULL), (fifo ? &fifo : NULL));
        }

        if (!xset_put_repeat(marked, vertex)) {
            return xwdigraph_map_union_bfs_free((marked ? &marked : NULL), (fifo ? &fifo : NULL));
        }

        if (!xfifo_push_no_limit(fifo, vertex)) {
            return xwdigraph_map_union_bfs_free(&marked, &fifo);
        }

        while (!xfifo_is_empty(fifo)) {
            vertex = xfifo_pop(fifo);

            {
                XWDigraph_6Paras_T paras = { graph, marked, fifo, apply, cl, NULL, NULL };

                if (xset_map_break_if_false(xwdigraph_adjset(graph, vertex), xwdigraph_map_union_bfs_apply, (void*)&paras)) {
                    return xwdigraph_map_union_bfs_free(&marked, &fifo);
                }
            }
        }

        xfifo_free(&fifo);
        return marked;
    }
}

XDList_PT xwdigraph_path_by_found_paths(XWDigraph_PT graph, XMap_PT paths, void *svertex, void *tvertex) {
    return xdigraph_path_by_found_paths(graph ? graph->native_graph : NULL, paths, svertex, tvertex);
}

static
bool xwdigraph_spt_free_dist(void *vertex, void **dist, void *cl) {
    XMEM_FREE(*dist);
    return true;
}

static
void* xwdigraph_spt_free(XSet_PT marked, XMap_PT edge_to, XMap_PT dist_to) {
    xset_free(marked ? &marked : NULL);
    xmap_free(edge_to ? &edge_to : NULL);
    xmap_map(dist_to, xwdigraph_spt_free_dist, NULL);
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
        XWDigraph_6Paras_PT paras = (XWDigraph_6Paras_PT)cl;
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
int xwdigraph_spt_cmp_weight(void *triple1, void *triple2, void *cl) {
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
bool xwdigraph_spt_queue_free(void **triple, void *cl) {
    xtriple_free((XTriple_PT*)triple);
    return true;
}

static
bool xwdigraph_spt_impl_dijkstra_impl(XWGraph_PT graph, XSet_PT marked, XMap_PT edge_to, XMap_PT dist_to, void *svertex, void *tvertex) {
    XMinPQ_PT queue = xminpq_new(xdigraph_edge_size(graph->native_graph), xwdigraph_spt_cmp_weight, graph->native_graph->cl);
    if (!queue) {
        return false;
    }

    {
        XWGraph_6Paras_T paras = { graph, marked, edge_to, dist_to, queue, NULL, NULL };

        /* step 1 : put the vertex into SPT */
        xset_remove(marked, svertex);

        /* step 2.1 : scan the edges which has the shortest dist to vertex in adjset */
        if (xset_map_break_if_false(xwdigraph_adjset(graph, svertex), xwdigraph_spt_impl_dijkstra_impl_apply, (void*)&paras)) {
            xminpq_free_apply(&queue, xwdigraph_spt_queue_free, NULL);
            return false;
        }

        /* step 2.2 : find the path which has the shortest dist to source vertex */
        XWEdge_PT edge = NULL;
        XTriple_PT triple = NULL;
        while (!xminpq_is_empty(queue)) {
            triple = xminpq_pop(queue);
            if(!triple) {
                xminpq_free_apply(&queue, xwdigraph_spt_queue_free, NULL);
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
            if (xset_map_break_if_false(xwdigraph_adjset(graph, svertex), xwdigraph_spt_impl_dijkstra_impl_apply, (void*)&paras)) {
                xminpq_free_apply(&queue, xwdigraph_spt_queue_free, NULL);
                return false;
            }
        }

        xminpq_free_apply(&queue, xwdigraph_spt_queue_free, NULL);
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
            return xwdigraph_spt_free(marked, edge_to, dist_to);
        }

        {
            int total = xset_size(marked);

            if (!xwdigraph_spt_impl_dijkstra_impl(graph, marked, edge_to, dist_to, svertex, tvertex)) {
                return xwdigraph_spt_free(marked, edge_to, dist_to);
            }

            xassert(xmap_size(edge_to) == (total - 1));
        }

        xmap_map(dist_to, xwdigraph_spt_free_dist, NULL);
        xmap_free(&dist_to);
        xset_free(&marked);

        return edge_to;
    }
}

XMap_PT xwdigraph_spt_dijkstra(XWGraph_PT graph, void *vertex) {
    return xwdigraph_spt_impl_dijkstra(graph, vertex, NULL);
}

static
XDList_PT xwdigraph_path_by_found_paths_impl(XWDigraph_PT graph, XMap_PT paths, void *svertex, void *tvertex) {
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

XDList_PT xwdigraph_shortest_path_dijkstra(XWGraph_PT graph, void *svertex, void *tvertex) {
    XMap_PT paths = xwdigraph_spt_impl_dijkstra(graph, svertex, tvertex);
    if (paths) {
        /* output the path */
        XDList_PT list = xwdigraph_path_by_found_paths_impl(graph, paths, svertex, tvertex);
        xmap_free(&paths);
        return list;
    }

    return NULL;
}

static
bool xwdigraph_spt_impl_no_cycle_apply(void *elem, void *cl) {
    XWEdge_PT edge = elem;

    XWDigraph_6Paras_PT paras = (XWDigraph_6Paras_PT)cl;
    XMap_PT edge_to = (XMap_PT)paras->para1/*paths*/;
    XMap_PT dist_to = (XMap_PT)paras->para2/*dist_to*/;

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

        return xmap_put_repeat(edge_to, edge->w, edge);
    }
    /* relex the edge */
    else if(v_dist) {
        bool shortest = *((bool*)paras->para4/*shortest*/);
        bool relax = false;

        if (shortest) {
            /* shortest paths */
            if (*v_dist + edge->weight < *w_dist) {
                relax = true;
            }
        }
        else {
            /* longest paths */
            if (*w_dist < *v_dist + edge->weight) {
                relax = true;
            }
        }

        if (relax) {
            *w_dist = *v_dist + edge->weight;

            if (!xmap_put_replace(dist_to, edge->w, (void*)w_dist, NULL)) {
                return false;
            }

            return xmap_put_replace(edge_to, edge->w, edge, NULL);
        }
    }

    /* ignore the edges which has higher weight */
    return true;
}

static 
XMap_PT xwdigraph_spt_impl_no_cycle(XWDigraph_PT graph, void *svertex, void *tvertex, bool shortest) {
    XDList_PT list = xdigraph_topologial_order_dfs(graph->native_graph);
    if (!list) {
        return NULL;
    }

    {
        XMap_PT edge_to = xmap_new(graph->native_graph->cmp, graph->native_graph->cl);
        XMap_PT dist_to = xmap_new(graph->native_graph->cmp, graph->native_graph->cl);
        if (!edge_to || !dist_to) {
            xmap_free(edge_to ? &edge_to : NULL);
            xmap_free(dist_to ? &dist_to : NULL);
            xdlist_free(&list);
            return NULL;
        }

        XWDigraph_6Paras_T paras = { graph, edge_to, dist_to, NULL/*parent vertex*/, (void*)&shortest, NULL, NULL };

        if (svertex) {
            while (0 != graph->native_graph->cmp(xdlist_front(list), svertex, graph->native_graph->cl)) {
                xdlist_pop_front(list);
            }
        }

        while(!xdlist_is_empty(list)) {
            paras.para3 = xdlist_pop_front(list);

            if (xset_map_break_if_false(xwdigraph_adjset(graph, paras.para3), xwdigraph_spt_impl_no_cycle_apply, (void*)&paras)) {
                xmap_free(edge_to ? &edge_to : NULL);
                xmap_map(dist_to, xwdigraph_spt_free_dist, NULL);
                xmap_free(dist_to ? &dist_to : NULL);
                xdlist_free(&list);
                return NULL;
            }

            if (tvertex) {
                if (0 == graph->native_graph->cmp(paras.para3, tvertex, graph->native_graph->cl)) {
                    break;
                }
            }
        }

        xmap_map(dist_to, xwdigraph_spt_free_dist, NULL);
        xmap_free(&dist_to);
        xdlist_free(&list);

        if (xmap_is_empty(edge_to)) {
            xmap_free(&edge_to);
        }

        return edge_to;
    }
}

XMap_PT xwdigraph_spt_no_cycle(XWDigraph_PT graph, void *svertex) {
    return xwdigraph_spt_impl_no_cycle(graph, svertex, NULL, true);
}

XDList_PT xwdigraph_shortest_path_no_cycle(XWDigraph_PT graph, void *svertex, void *tvertex) {
    XMap_PT paths = xwdigraph_spt_impl_no_cycle(graph, svertex, tvertex, true);
    if (paths) {
        /* output the path */
        XDList_PT list = xwdigraph_path_by_found_paths_impl(graph, paths, svertex, tvertex);
        xmap_free(&paths);
        return list;
    }

    return NULL;
}

XMap_PT xwdigraph_lpt_no_cycle(XWDigraph_PT graph, void *svertex) {
    return xwdigraph_spt_impl_no_cycle(graph, svertex, NULL, false);
}

XDList_PT xwdigraph_longest_path_no_cycle(XWDigraph_PT graph, void *svertex, void *tvertex) {
    XMap_PT paths = xwdigraph_spt_impl_no_cycle(graph, svertex, tvertex, false);
    if (paths) {
        /* output the path */
        XDList_PT list = xgraph_path_by_found_paths_impl(graph->native_graph, paths, svertex, tvertex);
        xmap_free(&paths);
        return list;
    }

    return NULL;
}

static 
bool xwdigraph_spt_impl_bellman_ford_apply(void *elem, void *cl) {
    XWDigraph_6Paras_PT paras = (XWDigraph_6Paras_PT)cl;
    XWEdge_PT edge = elem;

    XMap_PT edge_to = (XMap_PT)paras->para1/*paths*/;
    XMap_PT dist_to = (XMap_PT)paras->para2/*dist_to*/;
    XFifo_PT fifo = (XFifo_PT)paras->para3/*fifo*/;
    XSet_PT in_fifo = (XSet_PT)paras->para4/*in_fifo*/;

    int *total = (int*)paras->para5/*total*/;
    int *limit = (int*)paras->para6/*limit*/;

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

        if (!xmap_put_replace(edge_to, edge->w, edge, NULL)) {
            return false;
        }

        if (!xset_find(in_fifo, edge->w)) {
            if (!xfifo_push(fifo, edge->w)) {
                return false;
            }
            if (!xset_put_repeat(in_fifo, edge->w)) {
                return false;
            }
        }

        *total += 1;
        if (*total == *limit) {
            return false;
        }

        return true;
    }
    /* relex the edge */
    else if (v_dist && (*v_dist + edge->weight < *w_dist)) {
        *w_dist = *v_dist + edge->weight;

        if (!xmap_put_replace(edge_to, edge->w, edge, NULL)) {
            return false;
        }

        if (!xset_find(in_fifo, edge->w)) {
            if (!xfifo_push(fifo, edge->w)) {
                return false;
            }
            if (!xset_put_repeat(in_fifo, edge->w)) {
                return false;
            }
        }

        *total += 1;
        if (*total == *limit) {
            return false;
        }

        return true;
    }

    /* ignore the edges which has higher weight */

    return true;
}

static 
void* xwdigraph_spt_impl_bellman_ford_free(XMap_PT *edge_to, XMap_PT *dist_to, XFifo_PT *fifo, XSet_PT *in_fifo) {
    xmap_free(edge_to);
    xmap_free(dist_to);
    xfifo_free(fifo);
    xset_free(in_fifo);
    return NULL;
}

static 
XMap_PT xwdigraph_spt_impl_bellman_ford(XWDigraph_PT graph, void *svertex, bool find_negative_cycle) {
    XMap_PT edge_to = xmap_new(graph->native_graph->cmp, graph->native_graph->cl);
    XMap_PT dist_to = xmap_new(graph->native_graph->cmp, graph->native_graph->cl);
    XFifo_PT fifo = xfifo_new(xdigraph_vertex_size(graph->native_graph));
    XSet_PT in_fifo = xset_new(graph->native_graph->cmp, graph->native_graph->cl);
    if (!edge_to || !dist_to || !fifo || !in_fifo) {
        return xwdigraph_spt_impl_bellman_ford_free((edge_to ? &edge_to : NULL), (dist_to ? &dist_to : NULL), (fifo ? &fifo : NULL), (in_fifo ? &in_fifo : NULL));
    }

    if (!xfifo_push(fifo, svertex)) {
        return xwdigraph_spt_impl_bellman_ford_free(&edge_to, &dist_to, &fifo, &in_fifo);
    }
    if (!xset_put_repeat(in_fifo, svertex)) {
        return xwdigraph_spt_impl_bellman_ford_free(&edge_to, &dist_to, &fifo, &in_fifo);
    }

    {
        int total = 0;
        int limit = xdigraph_vertex_size(graph->native_graph) * xdigraph_edge_size(graph->native_graph);
        XWDigraph_6Paras_T paras = { graph, edge_to, dist_to, fifo, in_fifo, (void*)&total, (void*)&limit };

        while (!xfifo_is_empty(fifo)) {
            svertex = xfifo_pop(fifo);
            xset_remove(in_fifo, svertex);

            if (xset_map_break_if_false(xwdigraph_adjset(graph, svertex), xwdigraph_spt_impl_bellman_ford_apply, (void*)&paras)) {
                xmap_map(dist_to, xwdigraph_spt_free_dist, NULL);
                if (find_negative_cycle && (total == limit)) {
                    xwdigraph_spt_impl_bellman_ford_free(NULL, &dist_to, &fifo, &in_fifo);
                    return edge_to;
                }
                else {
                    return xwdigraph_spt_impl_bellman_ford_free(&edge_to, &dist_to, &fifo, &in_fifo);
                }
            }
        }
    }

    xmap_map(dist_to, xwdigraph_spt_free_dist, NULL);
    xmap_free(&dist_to);
    xfifo_free(&fifo);
    xset_free(&in_fifo);

    if (xmap_is_empty(edge_to)) {
        xmap_free(&edge_to);
    }

    return edge_to;
}

XMap_PT xwdigraph_spt_bellman_ford(XWDigraph_PT graph, void *vertex) {
    return xwdigraph_spt_impl_bellman_ford(graph, vertex, false);
}

XMap_PT xwdigraph_spt_bellman_ford_negative_cycle(XWDigraph_PT graph, void *vertex) {
    return xwdigraph_spt_impl_bellman_ford(graph, vertex, true);
}

static
bool xwdigraph_find_negative_cycle_apply(void *vertex, void **elem, void *cl) {
    XDigraph_PT digraph = cl;
    XWEdge_PT edge = *elem;

    void *nvertex = xwedge_other(edge, vertex);
    if (!xdigraph_add_edge_repeat(digraph, vertex, nvertex)) {
        return false;
    }

    return true;
}

XDList_PT xwdigraph_negative_cycle(XWDigraph_PT graph, XMap_PT edge_to) {
    XDigraph_PT digraph = xdigraph_new(graph->native_graph->cmp, graph->native_graph->cl);
    if (xmap_map_break_if_false(edge_to, xwdigraph_find_negative_cycle_apply, (void*)digraph)) {
        xdigraph_free(&digraph);
        return false;
    }

    {
        XDList_PT cycle = xdigraph_any_cycle(digraph);
        xdigraph_free(&digraph);
        return cycle;
    }
}
