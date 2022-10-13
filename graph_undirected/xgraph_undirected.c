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
#include <string.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../utils/xutils.h"
#include "../include/xqueue_fifo.h"
#include "../include/xlist_d.h"
#include "../include/xqueue_stack.h"
#include "../include/xtree_map.h"
#include "../tree_set/xtree_set_x.h"
#include "../include/xtree_multiple_branch.h"
#include "xgraph_undirected_x.h"

XGraph_PT xgraph_new(int (*cmp)(void *vertex1, void *vertex2, void *cl), void *cl) {
    xassert(cmp);

    if (!cmp) {
        return NULL;
    }

    {
        XGraph_PT graph = (XGraph_PT)XMEM_CALLOC(1, sizeof(*graph));
        if (!graph) {
            return NULL;
        }

        graph->adjsets = xmap_new(cmp, cl);
        if (!graph->adjsets) {
            XMEM_FREE(graph);
            return NULL;
        }

        graph->cmp = cmp;
        graph->cl = cl;

        return graph;
    }
}

static 
bool xgraph_free_adjsets_apply(void *vertex, void **adjset, void *cl) {
    xset_free((XSet_PT*)adjset);
    return true;
}

static 
void xgraph_free_impl(XGraph_PT graph, bool deep) {
    xmap_map(graph->adjsets, xgraph_free_adjsets_apply, NULL);
    deep ? xmap_deep_free(&graph->adjsets) : xmap_free(&graph->adjsets);
}

void xgraph_free(XGraph_PT *graph) {
    if (!graph || !*graph) {
        return;
    }

    xgraph_free_impl(*graph, false);
    XMEM_FREE(*graph);
}

void xgraph_deep_free(XGraph_PT *graph) {
    if (!graph || !*graph) {
        return;
    }

    xgraph_free_impl(*graph, true);
    XMEM_FREE(*graph);
}

static
bool xgraph_copy_adjsets_apply(void *vertex, void **adjset, void *cl) {
    XGraph_PT ngraph = (XGraph_PT)cl;

    XSet_PT set = xset_copy((XSet_PT)*adjset);
    if (!set) {
        return false;
    }

    if (!xmap_put_repeat(ngraph->adjsets, vertex, set)) {
        xset_free(&set);
        return false;
    }

    return true;
}

XGraph_PT xgraph_copy(XGraph_PT graph) {
    xassert(graph);

    if (!graph) {
        return NULL;
    }

    {
        XGraph_PT ngraph = xgraph_new(graph->cmp, graph->cl);
        if (!ngraph) {
            return NULL;
        }

        if (xmap_map_break_if_false(graph->adjsets, xgraph_copy_adjsets_apply, (void*)ngraph)) {
            xgraph_free(&ngraph);
            return NULL;
        }

        return ngraph;
    }
}

bool xgraph_add_vertex(XGraph_PT graph, void *vertex) {
    if (!xmap_find(graph ? graph->adjsets : NULL, vertex)) {
        XSet_PT adjset = xset_new(graph->cmp, graph->cl);
        if (!adjset) {
            return false;
        }

        if (!xmap_put_repeat(graph->adjsets, vertex, adjset)) {
            xset_free(&adjset);
            return false;
        }
    }

    return true;
}

static 
bool xgraph_remove_vertex_apply(void *nvertex, void *cl) {
    XGraph_3Paras_PT paras = (XGraph_3Paras_PT)cl;
    xset_remove_all(xgraph_adjset(paras->graph, nvertex), paras->para1/*vertex to delete*/);
    return true;
}

void xgraph_remove_vertex(XGraph_PT graph, void *vertex) {
    XSet_PT adjset = xgraph_adjset(graph, vertex);
    if (!adjset) {
        return ;
    }

    {
        XGraph_3Paras_T paras = { graph, vertex, NULL, NULL };
        xset_map(adjset, xgraph_remove_vertex_apply, &paras);
    }

    xmap_remove(graph->adjsets, vertex);
    xset_free(&adjset);
}

bool xgraph_is_vertex(XGraph_PT graph, void *vertex) {
    return xmap_find(graph ? graph->adjsets : NULL, vertex);
}

int xgraph_vertex_size(XGraph_PT graph) {
    return xmap_size(graph ? graph->adjsets : NULL);
}

XSet_PT xgraph_adjset(XGraph_PT graph, void *vertex) {
    return xmap_get(graph ? graph->adjsets : NULL, vertex);
}

bool xgraph_is_adjacent(XGraph_PT graph, void *vertex1, void *vertex2) {
    return xset_find(xgraph_adjset(graph, vertex1), vertex2);
}

static 
bool xgraph_add_edge_impl(XGraph_PT graph, void *vertex1, void *vertex2, bool (*xset_put_function)(XSet_PT set, void *key)) {
    if (!xgraph_add_vertex(graph, vertex1) || !xgraph_add_vertex(graph, vertex2)) {
        return false;
    }

    if (!xset_put_function(xgraph_adjset(graph, vertex1), vertex2)) {
        return false;
    }

    /* add vertex1 into the adjset of vertex2 if vertex1 is not equal to vertex2 */
    if (graph->cmp(vertex1, vertex2, graph->cl) != 0) {
        return xset_put_function(xgraph_adjset(graph, vertex2), vertex1);
    }

    return true;
}

bool xgraph_add_edge_unique(XGraph_PT graph, void *vertex1, void *vertex2) {
    return xgraph_add_edge_impl(graph, vertex1, vertex2, xset_put_unique);
}

bool xgraph_add_edge_repeat(XGraph_PT graph, void *vertex1, void *vertex2) {
    return xgraph_add_edge_impl(graph, vertex1, vertex2, xset_put_repeat);
}

void xgraph_remove_edge_impl(XGraph_PT graph, void *vertex1, void *vertex2, void(*xset_remove_function)(XSet_PT set, void *key)) {
    xset_remove_function(xgraph_adjset(graph, vertex1), vertex2);
    if (graph->cmp(vertex1, vertex2, graph->cl) != 0) {
        xset_remove_function(xgraph_adjset(graph, vertex2), vertex1);
    }
}

void xgraph_remove_edge(XGraph_PT graph, void *vertex1, void *vertex2) {
    xgraph_remove_edge_impl(graph, vertex1, vertex2, xset_remove);
}

void xgraph_remove_edge_all(XGraph_PT graph, void *vertex1, void *vertex2) {
    xgraph_remove_edge_impl(graph, vertex1, vertex2, xset_remove_all);
}

bool xgraph_is_edge(XGraph_PT graph, void *vertex1, void *vertex2) {
    return xset_find(xgraph_adjset(graph, vertex1), vertex2);
}

static 
bool xgraph_edge_size_apply(void *vertex, void **adjset, void *cl) {
    int *size = (int*)cl;
    *size += xset_size((XSet_PT)(*adjset));
    return true;
}

int xgraph_edge_size(XGraph_PT graph) {
    int edge_size = 0;
    xmap_map(graph ? graph->adjsets : NULL, xgraph_edge_size_apply, (void*)&edge_size);

    int self_cycles_size = xgraph_self_cycles_size(graph);
    return self_cycles_size + (edge_size - self_cycles_size)/2;
}

bool xgraph_has_parallel_edge(XGraph_PT graph, void *vertex) {
    return xset_has_repeat_elems_except(xgraph_adjset(graph, vertex), vertex);
}

int xgraph_parallel_edge_size(XGraph_PT graph, void *vertex) {
    return xset_repeat_elems_size_except(xgraph_adjset(graph, vertex), vertex);
}

static 
bool xgraph_remove_parallel_edge_apply(void *nvertex, void *cl) {
    XGraph_3Paras_PT paras = (XGraph_3Paras_PT)cl;
    xset_elem_unique(xgraph_adjset(paras->graph, nvertex), paras->para1/*vertex*/);
    return true;
}

bool xgraph_remove_parallel_edge(XGraph_PT graph, void *vertex) {
    XSet_PT uniqued = xset_unique_except_return_uniqued(xgraph_adjset(graph, vertex), vertex);
    if (!uniqued) {
        return false;
    }

    {
        XGraph_3Paras_T paras = { graph, (void*)vertex, NULL, NULL };
        xset_map(uniqued, xgraph_remove_parallel_edge_apply, (void*)&paras);
    }

    xset_free(&uniqued);
    return true;
}

static
bool xgraph_has_parallel_edges_apply(void *vertex, void **adjset, void *cl) {
    return xset_has_repeat_elems_except((XSet_PT)(*adjset), vertex);
}

bool xgraph_has_parallel_edges(XGraph_PT graph) {
    return xmap_map_break_if_true(graph ? graph->adjsets : NULL, xgraph_has_parallel_edges_apply, NULL);
}

static
bool xgraph_parallel_edges_size_apply(void *vertex, void **adjset, void *cl) {
    int *count = (int*)cl;
    *count += xset_repeat_elems_size_except((XSet_PT)(*adjset), vertex);
    return true;
}

int xgraph_parallel_edges_size(XGraph_PT graph) {
    int count = 0;
    xmap_map(graph ? graph->adjsets : NULL, xgraph_parallel_edges_size_apply, (void*)&count);
    return count/2;
}

static
bool xgraph_remove_parallel_edges_apply(void *vertex, void **adjset, void *cl) {
    xset_unique_except((XSet_PT)(*adjset), vertex);
    return true;
}

void xgraph_remove_parallel_edges(XGraph_PT graph) {
    xmap_map(graph ? graph->adjsets : NULL, xgraph_remove_parallel_edges_apply, NULL);
}

bool xgraph_has_self_cycle(XGraph_PT graph, void *vertex) {
    return xset_find(xgraph_adjset(graph, vertex), vertex);
}

int xgraph_self_cycle_size(XGraph_PT graph, void *vertex) {
    return xset_elem_size(xgraph_adjset(graph, vertex), vertex);
}

void xgraph_remove_self_cycle(XGraph_PT graph, void *vertex) {
    xset_remove_all(xgraph_adjset(graph, vertex), vertex);
}

static
bool xgraph_has_self_cycles_apply(void *vertex, void **adjset, void *cl) {
    return xset_find((XSet_PT)(*adjset), vertex);
}

bool xgraph_has_self_cycles(XGraph_PT graph) {
    return xmap_map_break_if_true(graph ? graph->adjsets : NULL, xgraph_has_self_cycles_apply, NULL);
}

static
bool xgraph_self_cycles_size_apply(void *vertex, void **adjset, void *cl) {
    int *size = (int*)cl;
    *size += xset_elem_size((XSet_PT)(*adjset), vertex);
    return true;
}

int xgraph_self_cycles_size(XGraph_PT graph) {
    int size = 0;
    xmap_map(graph ? graph->adjsets : NULL, xgraph_self_cycles_size_apply, (void*)&size);
    return size;
}

static
bool xgraph_remove_self_cycles_apply(void *vertex, void **adjset, void *cl) {
    xset_remove_all((XSet_PT)(*adjset), vertex);
    return true;
}

void xgraph_remove_self_cycles(XGraph_PT graph) {
    xmap_map(graph ? graph->adjsets : NULL, xgraph_remove_self_cycles_apply, NULL);
}

bool xgraph_is_simple(XGraph_PT graph) {
    return !xgraph_has_self_cycles(graph) && !xgraph_has_parallel_edges(graph);
}

static
bool xgraph_simple_graph_apply(void *vertex, void **adjset, void *cl) {
    xset_unique((XSet_PT)(*adjset));
    xset_remove((XSet_PT)(*adjset), vertex);
    return true;
}

void xgraph_simple_graph(XGraph_PT graph) {
    xmap_map(graph ? graph->adjsets : NULL, xgraph_simple_graph_apply, NULL);
}

static
bool xgraph_map_union_dfs_impl(void *vertex, void *cl) {
    XGraph_6Paras_PT paras = cl;

    /* ignore the vertex which is in marked already */
    if (xset_find((XSet_PT)(paras->para1/*marked*/), vertex)) {
        return true;
    }

    bool (*apply)(void *vertex, void *parent_vertex, void *cl) = paras->para3 /*apply*/;
    if (apply && !apply(vertex, xstack_peek((XStack_PT)paras->para2/*vertex stack*/), paras->para4/*cl*/)) {
        return false;
    }

    /* save the scaned vertex into marked */
    if (!xset_put_repeat((XSet_PT)paras->para1/*marked*/, vertex)) {
        return false;
    }

    /* record the parent vertex */
    if (!xstack_push_no_limit((XStack_PT)paras->para2/*vertex stack*/, vertex)) {
        return false;
    }

    if (xset_map_break_if_false(xgraph_adjset(paras->graph, vertex), xgraph_map_union_dfs_impl, (void*)paras)) {
        return false;
    }

    xstack_pop((XStack_PT)paras->para2/*vertex stack*/);

    return true;
}

XSet_PT xgraph_map_union_dfs(XGraph_PT graph, void *vertex, bool (*apply)(void *vertex, void *parent_vertex, void *cl), void *cl) {
    if (!xgraph_is_vertex(graph, vertex)) {
        return NULL;
    }

    {
        XSet_PT marked = xset_new(graph->cmp, graph->cl);
        if (!marked) {
            return NULL;
        }
        XStack_PT stack = xstack_new(xgraph_vertex_size(graph));
        if (!stack) {
            xset_free(&marked);
            return NULL;
        }

        {
            XGraph_6Paras_T paras = { graph, marked, stack, apply, cl, NULL, NULL };

            if (!xgraph_map_union_dfs_impl(vertex, (void*)&paras)) {
                xset_free(&marked);
                xstack_free(&stack);
                return NULL;
            }
        }

        xstack_free(&stack);
        return marked;
    }
}

XSet_PT xgraph_union_dfs(XGraph_PT graph, void *vertex) {
    return xgraph_map_union_dfs(graph, vertex, NULL, NULL);
}

static
bool xgraph_multi_unions_impl_apply(void *vertex, void *cl) {
    XGraph_3Paras_PT paras = (XGraph_3Paras_PT)cl;

    /* ignore the vertex which is in unions already */
    if (xset_find((XSet_PT)paras->para1/*unions*/, vertex)) {
        return true;
    }

    {
        XSet_PT (*union_function)(XGraph_PT graph, void *vertex) = paras->para2;

        XSet_PT marked = union_function(paras->graph, vertex);
        if (!marked) {
            return false;
        }

        if (!xset_merge_repeat((XSet_PT)paras->para1/*unions*/, &marked)) {
            xset_free(&marked);
            return false;
        }

        return true;
    }
}

static
XSet_PT xgraph_multi_unions_impl(XGraph_PT graph, XSet_PT vset, XSet_PT (*union_function)(XGraph_PT graph, void *vertex)) {
    if (xgraph_vertex_size(graph) == 0) {
        return NULL;
    }

    if (xset_size(vset) == 0) {
        return NULL;
    }

    {
        XSet_PT unions = xset_new(graph->cmp, graph->cl);
        if (!unions) {
            return NULL;
        }

        XGraph_3Paras_T paras = { graph, unions, union_function, NULL };

        if (xset_map_break_if_false(vset, xgraph_multi_unions_impl_apply, (void*)&paras)) {
            xset_free(&unions);
            return NULL;
        }

        return unions;
    }
}

XSet_PT xgraph_multi_unions_dfs(XGraph_PT graph, XSet_PT vset) {
    return xgraph_multi_unions_impl(graph, vset, xgraph_union_dfs);
}

static
bool xgraph_unions_impl_apply(void *vertex, void **adjset, void *cl) {
    XGraph_3Paras_PT paras = (XGraph_3Paras_PT)cl;
    XSet_PT marked = (XSet_PT)(paras->para2/*marked*/);

    /* vertex has been found in previous ccsets */
    if (xset_find(marked, vertex)) {
        return true;
    }

    /* find one ccset */
    {
        XSet_PT(*union_function)(XGraph_PT graph, void *vertex) = paras->para3;

        XSet_PT ccset = union_function(paras->graph, vertex);
        if (!ccset) {
            return false;
        }

        if (!xset_copy_merge_repeat(marked, ccset)) {
            xset_free(&ccset);
            return false;
        }

        if (!xdlist_push_back_repeat((XDList_PT)(paras->para1/*unions*/), (void*)ccset)) {
            xset_free(&ccset);
            return false;
        }
    }

    return true;
}

static
bool xgraph_unions_impl_unions_free_apply(void *ccset, void *cl) {
    xset_free((XSet_PT*)&ccset);
    return true;
}

static
XDList_PT xgraph_unions_impl(XGraph_PT graph, XSet_PT (*union_function)(XGraph_PT graph, void *vertex)) {
    if (xgraph_vertex_size(graph) == 0) {
        return NULL;
    }

    {
        XDList_PT unions = xdlist_new();
        if (!unions) {
            return NULL;
        }
        XSet_PT marked = xset_new(graph->cmp, graph->cl);
        if (!marked) {
            xdlist_free(&unions);
            return NULL;
        }

        {
            XGraph_3Paras_T paras = { graph, (void*)unions, (void*)marked, (void*)union_function };

            if (xmap_map_break_if_false(graph->adjsets, xgraph_unions_impl_apply, &paras)) {
                xdlist_map(unions, xgraph_unions_impl_unions_free_apply, NULL);
                xdlist_free(&unions);
                xset_free(&marked);
                return NULL;
            }
        }

        xset_free(&marked);
        return unions;
    }
}

XDList_PT xgraph_unions_dfs(XGraph_PT graph) {
    return xgraph_unions_impl(graph, xgraph_union_dfs);
}

static 
bool xgraph_map_union_bfs_apply(void *vertex, void *cl) {
    XGraph_6Paras_PT paras = (XGraph_6Paras_PT)cl;

    if (!xset_find((XSet_PT)(paras->para1/*marked*/), vertex)) {
        bool (*apply)(void *nvertex, void *parent_vertex, void *cl) = paras->para4 /*apply*/;
        if (apply && !apply(vertex, paras->para3/*parent vertext*/, paras->para5/*cl*/)) {
            return false;
        }

        if (!xset_put_repeat((XSet_PT)paras->para1/*marked*/, vertex)) {
            return false;
        }

        return xfifo_push((XFifo_PT)(paras->para2/*fifo*/), vertex);
    }

    return true;
}

static
void* xgraph_map_union_bfs_free(XSet_PT* set, XFifo_PT* fifo) {
    xset_free(set);
    xfifo_free(fifo);
    return NULL;
}

XSet_PT xgraph_map_union_bfs(XGraph_PT graph, void *vertex, bool (*apply)(void *nvertex, void *parent_vertex, void *cl), void *cl) {
    if (!xgraph_is_vertex(graph, vertex)) {
        return NULL;
    }

    if (apply && !apply(vertex, NULL, cl)) {
        return NULL;
    }

    {
        XSet_PT marked = xset_new(graph->cmp, graph->cl);
        if (!marked) {
            return NULL;
        }
        XFifo_PT fifo = xfifo_new(xgraph_vertex_size(graph));
        if (!fifo) {
            return xgraph_map_union_bfs_free(&marked, NULL);
        }

        if (!xset_put_repeat(marked, vertex)) {
            return xgraph_map_union_bfs_free(&marked, &fifo);
        }

        if (!xfifo_push(fifo, vertex)) {
            return xgraph_map_union_bfs_free(&marked, &fifo);
        }

        while (!xfifo_is_empty(fifo)) {
            vertex = xfifo_pop(fifo);

            {
                XGraph_6Paras_T paras = { graph, marked, fifo, vertex/*parent vertex*/, apply, cl, NULL };

                if (xset_map_break_if_false(xgraph_adjset(graph, vertex), xgraph_map_union_bfs_apply, (void*)&paras)) {
                    return xgraph_map_union_bfs_free(&marked, &fifo);
                }
            }
        }

        xfifo_free(&fifo);
        return marked;
    }
}

XSet_PT xgraph_union_bfs(XGraph_PT graph, void *vertex) {
    return xgraph_map_union_bfs(graph, vertex, NULL, NULL);
}

XSet_PT xgraph_multi_unions_bfs(XGraph_PT graph, XSet_PT vset) {
    return xgraph_multi_unions_impl(graph, vset, xgraph_union_bfs);
}

XDList_PT xgraph_unions_bfs(XGraph_PT graph) {
    return xgraph_unions_impl(graph, xgraph_union_bfs);
}

static
bool xgraph_path_impl_apply(void *vertex, void *parent_vertex, void *cl) {
    XGraph_3Paras_PT paras3 = (XGraph_3Paras_PT)cl;

    if (!xmap_put_repeat((XMap_PT)(paras3->para1/*paths*/), vertex, parent_vertex)) {
        return false;
    }

    /* found vertex2 */
    if (paras3->graph->cmp(vertex, paras3->para2/*vertex2*/, paras3->graph->cl) == 0) {
        return false;
    }

    return true;
}

static 
XDList_PT xgraph_path_impl(XGraph_PT graph, void *vertex1, void *vertex2,
                           XSet_PT (*xgraph_map_union_function)(XGraph_PT graph, void *vertex, bool (*apply)(void *vertex, void *parent_vertex, void *cl), void *cl)) {
    if (!graph) {
        return NULL;
    }

    {
        XMap_PT paths = xmap_new(graph->cmp, graph->cl);
        if (!paths) {
            return NULL;
        }

        XGraph_3Paras_T paras = { graph, paths, vertex2, NULL };
        XSet_PT ccset = xgraph_map_union_function(graph, vertex1, xgraph_path_impl_apply, (void*)&paras);
        if (!ccset) {
            /* found vertex2 : xgraph_path_impl_apply will return false to interrupt xgraph_map_union_dfs, which make ccset NULL returned */
            if (paths) {
                /* output the path */
                XDList_PT list = xgraph_path_by_found_paths_impl(graph, paths, vertex1, vertex2);
                xmap_free(&paths);
                return list;
            }

            return NULL;
        }

        xset_free(&ccset);
        xmap_free(&paths);
        return NULL;
    }
}

XDList_PT xgraph_path_dfs(XGraph_PT graph, void *vertex1, void *vertex2) {
    return xgraph_path_impl(graph, vertex1, vertex2, xgraph_map_union_dfs);
}

XDList_PT xgraph_path_bfs(XGraph_PT graph, void *vertex1, void *vertex2) {
    return xgraph_path_impl(graph, vertex1, vertex2, xgraph_map_union_bfs);
}

XDList_PT xgraph_shortest_path(XGraph_PT graph, void *vertex1, void *vertex2) {
    return xgraph_path_impl(graph, vertex1, vertex2, xgraph_map_union_bfs);
}

static 
bool xgraph_paths_impl_apply(void *vertex, void *parent_vertex, void *cl) {
    return xmap_put_repeat((XMap_PT)cl/*paths*/, vertex, parent_vertex);
}

static 
XMap_PT xgraph_paths_impl(XGraph_PT graph, void *vertex, 
                          XSet_PT (*xgraph_map_union_function)(XGraph_PT graph, void *vertex, bool (*apply)(void *vertex, void *parent_vertex, void *cl), void *cl)) {
    if (!graph) {
        return NULL;
    }

    {
        XMap_PT paths = xmap_new(graph->cmp, graph->cl);
        if (!paths) {
            return NULL;
        }

        XSet_PT ccset = xgraph_map_union_function(graph, vertex, xgraph_paths_impl_apply, (void*)paths);
        if (!ccset) {
            xmap_free(&paths);
            return NULL;
        }

        xset_free(&ccset);
        return paths;
    }
}

XMap_PT xgraph_paths_dfs(XGraph_PT graph, void *vertex) {
    return xgraph_paths_impl(graph, vertex, xgraph_map_union_dfs);
}

XMap_PT xgraph_paths_bfs(XGraph_PT graph, void *vertex) {
    return xgraph_paths_impl(graph, vertex, xgraph_map_union_bfs);
}

XMap_PT xgraph_shortest_paths(XGraph_PT graph, void *vertex) {
    return xgraph_paths_impl(graph, vertex, xgraph_map_union_bfs);
}

XDList_PT xgraph_path_by_found_paths_impl(XGraph_PT graph, XMap_PT paths, void *svertex, void *tvertex) {
    XDList_PT list = xdlist_new();
    if (!list) {
        return NULL;
    }

    {
        int size = xmap_size(paths);
        void *vertex = tvertex;

        do {
            if (!xdlist_push_front_repeat(list, vertex)) {
                xdlist_free(&list);
                return NULL;
            }

            vertex = xmap_get(paths, vertex);
            if (!vertex) {
                xdlist_free(&list);
                return NULL;
            }

            --size;
        } while ((graph->cmp(vertex, svertex, graph->cl) != 0) && (0 < size));

        /* error found : size<=0 now, but vertex is not the svertex */
        if (graph->cmp(vertex, svertex, graph->cl) != 0) {
            xdlist_free(&list);
            return NULL;
        }

        /* add the svertex */
        if (!xdlist_push_front_repeat(list, vertex)) {
            xdlist_free(&list);
            return NULL;
        }
    }

    return list;
}

XDList_PT xgraph_path_by_found_paths(XGraph_PT graph, XMap_PT paths, void *svertex, void *tvertex) {
    xassert(paths);

    if (!paths) {
        return NULL;
    }

    if (!xgraph_is_vertex(graph, svertex) || !xgraph_is_vertex(graph, tvertex)) {
        return NULL;
    }

    return xgraph_path_by_found_paths_impl(graph, paths, svertex, tvertex);
}

bool xgraph_is_connected(XGraph_PT graph, void *vertex1, void *vertex2) {
    XDList_PT path = xgraph_shortest_path(graph, vertex1, vertex2);
    if (path) {
        xdlist_free(&path);
        return true;
    }

    return false;
}

static
bool xgraph_has_euler_path_apply(void *vertex, void **ajdset, void *cl) {
    XGraph_3Paras_PT paras = (XGraph_3Paras_PT)cl;

    /* ingore the svertex and tvertex */
    if (!paras->graph->cmp(vertex, paras->para1/*svertex*/, paras->graph->cl) || !paras->graph->cmp(vertex, paras->para2/*tvertex*/, paras->graph->cl)) {
        return true;
    }

    if (xgraph_degree(paras->graph, vertex) %2 != 0) {
        return false;
    }

    return true;
}

/* <<Algorithms in C>> Third Edition. Chapter 17.7 */
bool xgraph_has_euler_path(XGraph_PT graph, void *svertex, void *tvertex) {
    if (!xgraph_is_vertex(graph, svertex) || !xgraph_is_vertex(graph, tvertex)) {
        return false;
    }

    /* if svertex and tvertex are the same vertex, its degree must be even number */
    if (!graph->cmp(svertex, tvertex, graph->cl)) {
        if (xgraph_degree(graph, svertex) % 2 != 0) {
            return false;
        }
    }
    else {
        /* the degree summary must be even number, that means both of their degree can be odd number */
        if ((xgraph_degree(graph, svertex) + xgraph_degree(graph, tvertex)) % 2 != 0) {
            return false;
        }
    }

    /* graph must be connected */
    {
        XSet_PT ccset = xgraph_union_dfs(graph, svertex);
        if (xset_size(ccset) != xgraph_vertex_size(graph)) {
            xset_free(&ccset);
            return false;
        }
        xset_free(&ccset);
    }

    /* all other vertexes' degree must be even number */
    {
        XGraph_3Paras_T paras = { graph, svertex, tvertex, NULL };
        if (xmap_map_break_if_false(graph->adjsets, xgraph_has_euler_path_apply, (void*)&paras)) {
            return false;
        }

        return true;
    }
}

static
void* xgraph_euler_path_free(XStack_PT *stack, XDList_PT *list) {
    xstack_free(stack);
    xdlist_free(list);
    return NULL;
}

static 
bool xgraph_euler_path_apply(XGraph_PT graph, XStack_PT stack, void *vertex) {
    while (true) {
        XSet_PT adjset = xgraph_adjset(graph, vertex);
        if (xset_size(adjset) == 0) {
            break;
        }

        if (!xstack_push_no_limit(stack, vertex)) {
            return false;
        }

        {
            void *vertex2 = xset_select(adjset, 0);
            xgraph_remove_edge(graph, vertex, vertex2);
            vertex = vertex2;
        }
    }

    return true;
}

/* <<Algorithms in C>> Third Edition. Chapter 17.7 */
extern XDList_PT xgraph_euler_path(XGraph_PT graph, void *svertex, void *tvertex) {
    XStack_PT stack = xstack_new(XUTILS_DEQUE_LAYER2_DEFAULT_LENGTH);
    XDList_PT list = xdlist_new();
    if (!stack || !list) {
        return xgraph_euler_path_free(stack ? &stack : NULL, list ? &list : NULL);
    }

    if (!xdlist_push_back_repeat(list, tvertex)) {
        return xgraph_euler_path_free(stack ? &stack : NULL, list ? &list : NULL);
    }

    while (xgraph_edge_size(graph) != 0) {
        if (!xgraph_euler_path_apply(graph, stack, svertex)) {
            return xgraph_euler_path_free(stack ? &stack : NULL, list ? &list : NULL);
        }

        if (!xstack_is_empty(stack)) {
            svertex = xstack_pop(stack);
            if (!xdlist_push_front_repeat(list, svertex)) {
                return xgraph_euler_path_free(stack ? &stack : NULL, list ? &list : NULL);
            }
        }
    }

    while (!xstack_is_empty(stack)) {
        svertex = xstack_pop(stack);
        if (!xdlist_push_front_repeat(list, svertex)) {
            return xgraph_euler_path_free(stack ? &stack : NULL, list ? &list : NULL);
        }
    }

    xstack_free(&stack);
    return list;
}

static
bool xgraph_cycle_dfs_impl_apply(void *vertex, void *cl) {
    XGraph_6Paras_PT paras = cl;

    /* cycle found */
    if (xset_find((XSet_PT)(paras->para1/*marked*/), vertex)) {

        /* ignore this scenario: A->B->A : vertex is A (in B's adjset) now*/
        void* pre2_vertex = xmap_get((XMap_PT)(paras->para2/*paths*/), xstack_peek(paras->para3));/*get B*/
        if (!pre2_vertex) {
            /* just self cycle A->A will reach here, so return true to continue the search */
            return true;
        }
        /* A->B->A */
        if (paras->graph->cmp(vertex, pre2_vertex, paras->graph->cl) == 0) {
            /* return true to continue the search */
            return true;
        }

        if (paras->para4/*vertex_in_cycle*/) {
            /* cycle found with vertex_in_cycle inside */
            if (paras->graph->cmp(paras->para4/*start vertex*/, vertex, paras->graph->cl) == 0) {
                xmap_put_replace((XMap_PT)(paras->para2/*paths*/), vertex, xstack_peek(paras->para3)/*parent vertex*/, NULL);

                /* return false to break the search */
                return false;
            }

            /* ignore it if it's not vertex_in_cycle */
            return true;
        }
        /* no special vertex assigned, any cycle found */
        else {
            xmap_put_replace((XMap_PT)(paras->para2/*paths*/), vertex, xstack_peek(paras->para3)/*parent vertex*/, NULL);

            /* record this vertex */
            paras->para4 = vertex;

            /* return false to break the search */
            return false;
        }
    }

    if (!xset_put_unique((XSet_PT)paras->para1/*marked*/, vertex)) {
        return false;
    }

    if (xstack_peek(paras->para3)/*parent vertex*/) {
        if (!xmap_put_repeat((XMap_PT)(paras->para2/*paths*/), vertex, xstack_peek(paras->para3)/*parent vertex*/)) {
            return false;
        }
    }

    if (!xstack_push_no_limit(paras->para3/*previous vertex stack*/, vertex)) {
        return false;
    }

    if (xset_map_break_if_false(xgraph_adjset(paras->graph, vertex), xgraph_cycle_dfs_impl_apply, (void*)paras)) {
        return false;
    }

    xstack_pop(paras->para3/*previous vertex stack*/);

    return true;
}

static 
XMap_PT xgraph_cycle_dfs_impl(XGraph_PT graph, void *vertex, void **vertex_in_cycle, XSet_PT marked) {
    XMap_PT paths = xmap_new(graph->cmp, graph->cl);
    if (!paths) {
        return NULL;
    }
    XStack_PT stack = xstack_new(xgraph_vertex_size(graph));
    if (!stack) {
        xmap_free(&paths);
        return NULL;
    }

    {
        XGraph_6Paras_T paras = { graph, marked, paths, stack, *vertex_in_cycle, NULL };
        xgraph_cycle_dfs_impl_apply(vertex, (void*)&paras);
        xstack_free(&stack);

        if (*vertex_in_cycle) {
            if (xmap_find(paths, *vertex_in_cycle)) {
                return paths;
            }
        }
        else {
            if (paras.para4) {
                *vertex_in_cycle = paras.para4;
                return paths;
            }
        }

        xmap_free(&paths);
        return NULL;
    }
}

XDList_PT xgraph_cycle_include(XGraph_PT graph, void *vertex) {
    if (!xgraph_is_vertex(graph, vertex)) {
        return NULL;
    }

    {
        XSet_PT marked = xset_new(graph->cmp, graph->cl);
        if (!marked) {
            return NULL;
        }

        {
            XMap_PT paths = xgraph_cycle_dfs_impl(graph, vertex, &vertex, marked);
            xset_free(&marked);

            if (paths) {
                /* output the cycle path */
                XDList_PT list = xgraph_path_by_found_paths_impl(graph, paths, vertex, vertex);
                xmap_free(&paths);
                return list;
            }

            return NULL;
        }
    }
}

static 
bool xgraph_any_cycle_dfs_apply(void *vertex, void **adjset, void *cl) {
    XGraph_3Paras_PT paras = cl;

    /* ingore the vertex which is in marked already */
    if (xset_find((XSet_PT)paras->para1/*marked*/, vertex)) {
        return true;
    }

    {
        void *vertex_in_cycle = NULL;
        XMap_PT paths = xgraph_cycle_dfs_impl(paras->graph, vertex, &vertex_in_cycle, paras->para1/*marked*/);

        if (vertex_in_cycle) {
            paras->para3/*vertex_in_cycle*/ = vertex_in_cycle;
            paras->para2/*paths*/ = paths;
            return false;
        }
    }

    return true;
}

XDList_PT xgraph_any_cycle(XGraph_PT graph) {
    XSet_PT marked = xset_new(graph->cmp, graph->cl);
    if (!marked) {
        return NULL;
    }

    {
        XGraph_3Paras_T paras = { graph, marked, NULL/*paths*/, NULL/*vertex_in_cycle*/ };
        xmap_map_break_if_false(graph->adjsets, xgraph_any_cycle_dfs_apply, (void*)&paras);
        xset_free(&marked);

        if (paras.para3/*vertex_in_cycle*/) {
            /* output the cycle path */
            XDList_PT list = xgraph_path_by_found_paths_impl(graph, paras.para2/*paths*/, paras.para3/*vertex_in_cycle*/, paras.para3/*vertex_in_cycle*/);
            xmap_free((XMap_PT*)(&paras.para2/*paths*/));
            return list;
        }

        xmap_free((XMap_PT*)(&paras.para2/*paths*/));
        return NULL;
    }
}

XDList_PT xgraph_any_cycle_in_union(XGraph_PT graph, void *vertex) {
    if (!xgraph_is_vertex(graph, vertex)) {
        return NULL;
    }

    {
        XSet_PT marked = xset_new(graph->cmp, graph->cl);
        if (!marked) {
            return NULL;
        }

        {
            void *vertex_in_cycle = NULL;
            XMap_PT paths = xgraph_cycle_dfs_impl(graph, vertex, &vertex_in_cycle, marked);
            xset_free(&marked);

            if (vertex_in_cycle && paths) {
                /* output the cycle path */
                XDList_PT list = xgraph_path_by_found_paths_impl(graph, paths, vertex_in_cycle, vertex_in_cycle);
                xmap_free(&paths);
                return list;
            }

            xmap_free(&paths);
        }
    }

    return NULL;
}

/* bfs is not suitable to find cycle : 
 *   for each marked vertex, we need to check the paths to know if cycle exists or not
 */
bool xgraph_has_cycle_include(XGraph_PT graph, void *vertex) {
    XDList_PT list = xgraph_cycle_include(graph, vertex);
    if (list) {
        xdlist_free(&list);
        return true;
    }

    return false;
}

bool xgraph_has_any_cycle_in_union(XGraph_PT graph, void *vertex) {
    XDList_PT list = xgraph_any_cycle_in_union(graph, vertex);
    if (list) {
        xdlist_free(&list);
        return true;
    }

    return false;
}

bool xgraph_has_any_cycle(XGraph_PT graph) {
    /* if V <= E, cycyle must exist : only for the graph without parallel edge and self-cycle */
    //if ((0 < xgraph_vertex_size(graph)) && (xgraph_vertex_size(graph) <= xgraph_edge_size(graph))) {
    //    return true;
    //}

    XDList_PT list = xgraph_any_cycle(graph);
    if (list) {
        xdlist_free(&list);
        return true;
    }

    return false;
}

static 
bool xgraph_two_color_split_apply(void *vertex, void **color_value, void *cl) {
    XGraph_3Paras_PT paras = (XGraph_3Paras_PT)cl;

    if (strcmp("a", *color_value) == 0) {
        return xset_put_repeat((XSet_PT)paras->para1/*color_set1*/, vertex);
    }
    else {
        return xset_put_repeat((XSet_PT)paras->para2/*color_set2*/, vertex);
    }
}

static
void* xgraph_two_color_split_free(XSet_PT *color_set1, XSet_PT *color_set2, XDList_PT* color_set) {
    xset_free(color_set1);
    xset_free(color_set2);
    xdlist_free(color_set);
    return NULL;
}

static 
XDList_PT xgraph_two_color_split(XGraph_PT graph, XMap_PT color) {
    XSet_PT color_set1 = xset_new(graph->cmp, graph->cl);
    XSet_PT color_set2 = xset_new(graph->cmp, graph->cl);
    XDList_PT color_set = xdlist_new();

    if (!color_set1 || !color_set2 || !color_set) {
        return xgraph_two_color_split_free((color_set1 ? &color_set1 : NULL), (color_set2 ? &color_set2 : NULL), (color_set ? &color_set : NULL));
    }

    {
        XGraph_3Paras_T paras = { graph, color_set1, color_set2, NULL };

        if (xmap_map_break_if_false(color, xgraph_two_color_split_apply, &paras)) {
            return xgraph_two_color_split_free(&color_set1, &color_set2, &color_set);
        }
    }

    if (!xdlist_push_back_repeat(color_set, (void*)color_set1)) {
        return xgraph_two_color_split_free(&color_set1, &color_set2, &color_set);
    }

    if (!xdlist_push_back_repeat(color_set, (void*)color_set2)) {
        return xgraph_two_color_split_free(&color_set1, &color_set2, &color_set);
    }

    return color_set;
}

static
bool xgraph_two_color_dfs_identify_map_apply_impl(void *vertex, void *cl) {
    XGraph_3Paras_PT paras = cl;

    XMap_PT color = (XMap_PT)paras->para2/*color*/;
    XSet_PT marked = (XSet_PT)paras->para1/*marked*/;
    XStack_PT stack = (XStack_PT)paras->para3/*stack*/;

    char *parent_color = xmap_get(color, xstack_peek(stack));

    if (xset_find(marked, vertex)) {
        /* check if adjacent vertex's colors are the same */
        if (parent_color && (0 == strcmp(xmap_get(color, vertex), parent_color))) {
            return false;
        }

        return true;
    }

    {
        char *vertex_color = "a";
        if (parent_color && (0 == strcmp(parent_color, "a"))) {
            vertex_color = "b";
        }

        if (!xmap_put_repeat(color, vertex, vertex_color)) {
            return false;
        }

        if (!xset_put_repeat(marked, vertex)) {
            return false;
        }

        if (!xstack_push_no_limit(stack, vertex)) {
            return false;
        }

        if (xset_map_break_if_false(xgraph_adjset(paras->graph, vertex), xgraph_two_color_dfs_identify_map_apply_impl, cl)) {
            return false;
        }

        xstack_pop(stack);
    }

    return true;
}

static
bool xgraph_two_color_dfs_identify_map_apply(void *vertex, void **adjset, void *cl) {
    XGraph_3Paras_PT paras = (XGraph_3Paras_PT)cl;

    /* ignore this vertex if marked already : in one ccset */
    if (xset_find((XSet_PT)paras->para1/*marked*/, vertex)) {
        return true;
    }

    /* for the new start ccset, clear the old data at first */
    xstack_clear((XStack_PT)paras->para3/*stack*/);

    return xgraph_two_color_dfs_identify_map_apply_impl(vertex, (void*)paras);
}

static
void* xgraph_two_color_dfs_identify_free(XSet_PT *mark, XMap_PT *path, XStack_PT *stack) {
    xset_free(mark);
    xmap_free(path);
    xstack_free(stack);
    return NULL;
}

static
XMap_PT xgraph_two_color_dfs_identify(XGraph_PT graph) {
    XSet_PT marked = xset_new(graph->cmp, graph->cl);
    XMap_PT color = xmap_new(graph->cmp, graph->cl);
    XStack_PT stack = xstack_new(xgraph_vertex_size(graph));

    if (!color || !marked || !stack) {
        return xgraph_two_color_dfs_identify_free((marked ? &marked : NULL), (color ? &color : NULL), (stack ? &stack : NULL));
    }

    {
        XGraph_3Paras_T paras = { graph, marked, color, stack };

        if (xmap_map_break_if_false(graph->adjsets, xgraph_two_color_dfs_identify_map_apply, &paras)) {
            return xgraph_two_color_dfs_identify_free(&marked, &color, &stack);
        }

        xset_free(&marked);
        xstack_free(&stack);
    }

    return color;
}

static
bool xgraph_two_color_bfs_identify_set_apply(void *nvertex, void *cl) {
    XGraph_6Paras_PT paras = (XGraph_6Paras_PT)cl;

    XSet_PT marked = (XSet_PT)(paras->para1/*marked*/);
    XMap_PT color = (XMap_PT)(paras->para2/*color*/);
    XFifo_PT fifo = (XFifo_PT)(paras->para3/*fifo*/);

    char *parent_color = xmap_get(color, paras->para4/*previous vertex*/);

    if (xset_find(marked, nvertex)) {
        /* check if adjacent vertex's colors are the same */
        if (parent_color && (0 == strcmp(xmap_get(color, nvertex), parent_color))) {
            return false;
        }

        return true;
    }

    {
        char *vertex_color = "a";
        if (parent_color && (0 == strcmp(parent_color, "a"))) {
            vertex_color = "b";
        }

        if (!xmap_put_repeat(color, nvertex, vertex_color)) {
            return false;
        }

        if (!xset_put_repeat(marked, nvertex)) {
            return false;
        }

        if (!xfifo_push_no_limit(fifo, nvertex)) {
            return false;
        }
    }

    return true;
}

static
bool xgraph_two_color_bfs_identify_map_apply(void *vertex, void **adjset, void *cl) {
    XGraph_6Paras_PT paras = (XGraph_6Paras_PT)cl;

    XSet_PT marked = (XSet_PT)paras->para1/*marked*/;
    XMap_PT color = (XMap_PT)(paras->para2/*color*/);
    XFifo_PT fifo = (XFifo_PT)paras->para3/*fifo*/;

    /* ignore this vertex if marked already : in one ccset */
    if (xset_find(marked, vertex)) {
        return true;
    }

    /* for the new start ccset, clear the old data at first */
    xfifo_clear(fifo);

    if (!xset_put_repeat(marked, vertex)) {
        return false;
    }

    /* set the inital color */
    if (!xmap_put_repeat(color, vertex, "a")) {
        return false;
    }

    if (!xfifo_push_no_limit(fifo, vertex)) {
        return false;
    }

    while (!xfifo_is_empty(fifo)) {
        vertex = xfifo_pop(fifo);
        /* previous vertex */
        paras->para4 = vertex;

        if (xset_map_break_if_false(xgraph_adjset(paras->graph, vertex), xgraph_two_color_bfs_identify_set_apply, paras)) {
            return false;
        }
    }

    return true;
}

static
void* xgraph_two_color_bfs_free(XSet_PT *mark, XMap_PT *path, XFifo_PT *fifo) {
    xset_free(mark);
    xmap_free(path);
    xfifo_free(fifo);
    return NULL;
}

static
XMap_PT xgraph_two_color_bfs_identify(XGraph_PT graph) {
    XSet_PT marked = xset_new(graph->cmp, graph->cl);
    XMap_PT color = xmap_new(graph->cmp, graph->cl);
    XFifo_PT fifo = xfifo_new(xgraph_vertex_size(graph));

    if (!marked || !color || !fifo) {
        return xgraph_two_color_bfs_free((marked ? &marked : NULL), (color ? &color : NULL), (fifo ? &fifo : NULL));
    }

    {
        XGraph_6Paras_T paras = { graph, marked, color, fifo, NULL/*previous vertex*/, NULL, NULL };

        if (xmap_map_break_if_false(graph->adjsets, xgraph_two_color_bfs_identify_map_apply, &paras)) {
            return xgraph_two_color_bfs_free(&marked, &color, &fifo);
        }

        xset_free(&marked);
        xfifo_free(&fifo);
    }

    return color;
}

static 
XDList_PT xgraph_two_color_impl(XGraph_PT graph, XMap_PT (*xgraph_two_color_identify)(XGraph_PT graph)) {
    if (xgraph_vertex_size(graph) < 2) {
        return NULL;
    }

    {
        XMap_PT color = xgraph_two_color_identify(graph);
        if (!color) {
            return NULL;
        }

        {
            /* get the two color sets */
            XDList_PT color_set = xgraph_two_color_split(graph, color);
            xmap_free(&color);
            return color_set;
        }
    }
}

XDList_PT xgraph_two_color_dfs(XGraph_PT graph) {
    return xgraph_two_color_impl(graph, xgraph_two_color_dfs_identify);
}

XDList_PT xgraph_two_color_bfs(XGraph_PT graph) {
    return xgraph_two_color_impl(graph, xgraph_two_color_bfs_identify);
}

bool xgraph_is_two_color(XGraph_PT graph) {
    if (xgraph_vertex_size(graph) < 2) {
        return false;
    }

    XMap_PT color = xgraph_two_color_bfs_identify(graph);
    if (!color) {
        return false;
    }

    xmap_free(&color);
    return true;
}

bool xgraph_is_complete(XGraph_PT graph) {
    return (xgraph_edge_size(graph) - xgraph_parallel_edges_size(graph)) == (xgraph_vertex_size(graph) * (xgraph_vertex_size(graph) - 1) / 2);
}

/* a connected graph without cycle is a tree */
bool xgraph_is_tree(XGraph_PT graph) {
    /* no cycle */
    if (xgraph_has_parallel_edges(graph) || xgraph_has_self_cycles(graph) || xgraph_has_any_cycle(graph)) {
        return false;
    }

    /* check if all vertexes are connected */
    {
        XSet_PT ccset = xgraph_union_dfs(graph, xmap_select(graph->adjsets, 0));
        if (ccset) {
            int size = xset_size(ccset);
            xset_free(&ccset);

            return size == xgraph_vertex_size(graph);
        }

        return false;
    }
}

static
bool xgraph_tree_apply(void *vertex, void *parent_vertex, void *cl) {
    XGraph_3Paras_PT paras = cl;
    XMTree_Node_PT parent_node = xmap_get(paras->para2/*map*/, parent_vertex);

    XMTree_Node_PT vertex_node = xmtree_put_repeat(paras->para1/*tree*/, parent_node, vertex, NULL);
    if (!vertex_node) {
        return false;
    }

    return xmap_put_repeat(paras->para2/*map*/, vertex, vertex_node);
}

static 
XMTree_PT xgraph_tree_impl(XGraph_PT graph, void *vertex,
                           XSet_PT (*xgraph_map_union_function)(XGraph_PT graph, void *vertex, bool (*apply)(void *vertex, void *parent_vertex, void *cl), void *cl)) {
    XMTree_PT tree = xmtree_new(graph->cmp, graph->cl);
    if (!tree) {
        return NULL;
    }
    XMap_PT map = xmap_new(graph->cmp, graph->cl);
    if (!map) {
        xmtree_free(&tree);
        return NULL;
    }

    {
        XGraph_3Paras_T paras = { graph, tree, map, NULL };
        XSet_PT marked = xgraph_map_union_function(graph, vertex, xgraph_tree_apply, (void*)&paras);
        if (!marked) {
            xmtree_free(&tree);
            xmap_free(&map);
            return NULL;
        }

        xset_free(&marked);
        xmap_free(&map);
    }

    return tree;
}

XMTree_PT xgraph_tree_dfs(XGraph_PT graph, void *vertex) {
    return xgraph_tree_impl(graph, vertex, xgraph_map_union_dfs);
}

XMTree_PT xgraph_tree_bfs(XGraph_PT graph, void *vertex) {
    return xgraph_tree_impl(graph, vertex, xgraph_map_union_bfs);
}

static
bool xgraph_forest_impl_apply(void *vertex, void **adjset, void *cl) {
    XGraph_6Paras_PT paras = (XGraph_6Paras_PT)cl;

    XSet_PT *marked = (XSet_PT*)(paras->para2/*&marked*/);
    /* vertex has been found in previous forest */
    if (xset_find(*marked, vertex)) {
        return true;
    }

    /* find one tree */
    {
        XMTree_PT(*xgraph_tree_function)(XGraph_PT graph, void *vertex) = paras->para3;
        XSet_PT(*xgraph_union_function)(XGraph_PT graph, void *vertex) = paras->para4;

        XMTree_PT tree = xgraph_tree_function(paras->graph, vertex);
        XSet_PT nmarked = xgraph_union_function(paras->graph, vertex);

        if (*marked) {
            if (!xset_merge_repeat(*marked, &nmarked)) {
                xmtree_free(&tree);
                return false;
            }
        }
        else {
            *marked = nmarked;
        }

        if (!xdlist_push_back_repeat((XDList_PT)(paras->para1/*forest*/), (void*)tree)) {
            xmtree_free(&tree);
            return false;
        }
    }

    return true;
}

static
bool xgraph_forest_impl_free_apply(void *tree, void *cl) {
    xmtree_free((XMTree_PT*)&tree);
    return true;
}

static 
XDList_PT xgraph_forest_impl(XGraph_PT graph, 
                             XMTree_PT (*xgraph_tree_function)(XGraph_PT graph, void *vertex),
                             XSet_PT   (*xgraph_union_function)(XGraph_PT graph, void *vertex)) {
    if (xgraph_vertex_size(graph) == 0) {
        return NULL;
    }

    {
        XSet_PT marked = NULL;

        XDList_PT forest = xdlist_new();
        if (!forest) {
            return NULL;
        }

        XGraph_6Paras_T paras = { graph, (void*)forest, (void*)&marked, (void*)xgraph_tree_function, (void*)xgraph_union_function, NULL, NULL };

        if (xmap_map_break_if_false(graph->adjsets, xgraph_forest_impl_apply, &paras)) {
            xdlist_map(forest, xgraph_forest_impl_free_apply, NULL);
            xdlist_free(&forest);
            xset_free(marked ? &marked : NULL);
            return NULL;
        }

        xset_free(&marked);
        return forest;
    }
}

XDList_PT xgraph_forest_dfs(XGraph_PT graph) {
    return xgraph_forest_impl(graph, xgraph_tree_dfs, xgraph_union_dfs);
}

XDList_PT xgraph_forest_bfs(XGraph_PT graph) {
    return xgraph_forest_impl(graph, xgraph_tree_bfs, xgraph_union_bfs);
}

int xgraph_degree(XGraph_PT graph, void *vertex) {
    return xset_size(xgraph_adjset(graph, vertex));
}

static
bool xgraph_max_degree_apply(void *vertex, void **adjset, void *cl) {
    int *max = (int*)cl;

    int size = xset_size((XSet_PT)(*adjset));
    if (*max < size) {
        *max = size;
    }

    return true;
}

int xgraph_max_degree(XGraph_PT graph) {
    int max = 0;
    xmap_map(graph ? graph->adjsets : NULL, xgraph_max_degree_apply, (void*)&max);
    return max;
}

double xgraph_average_degree(XGraph_PT graph) {
    return 2 * (double)xgraph_edge_size(graph) / xgraph_vertex_size(graph);
}

double xgraph_density(XGraph_PT graph) {
    return xgraph_average_degree(graph);            //  ?
}

bool xgraph_is_dense(XGraph_PT graph) {
    if(!graph) {
        return false;
    }
    return 1 <= xgraph_edge_size(graph) / (xgraph_vertex_size(graph) * xgraph_vertex_size(graph));
}

bool xgraph_is_sparse(XGraph_PT graph) {
    if (!graph) {
        return false;
    }
    return xgraph_edge_size(graph) / (xgraph_vertex_size(graph) * xgraph_vertex_size(graph)) < 1;
}
