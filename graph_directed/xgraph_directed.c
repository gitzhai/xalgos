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
#include "../include/xqueue_deque.h"
#include "../include/xqueue_stack.h"
#include "../tree_set/xtree_set_x.h"
#include "../include/xgraph_undirected.h"
#include "xgraph_directed_x.h"

XDigraph_PT xdigraph_new(int(*cmp)(void *vertex1, void *vertex2, void *cl), void *cl) {
    return xgraph_new(cmp, cl);
}

void xdigraph_free(XDigraph_PT *graph) {
    xgraph_free(graph);
}

void xdigraph_deep_free(XDigraph_PT *graph) {
    xgraph_deep_free(graph);
}

XDigraph_PT xdigraph_copy(XDigraph_PT graph) {
    return xgraph_copy(graph);
}

static
bool xdigraph_reverse_adjset_apply(void *tvertex, void *cl) {
    XDigraph_3Paras_PT paras = (XDigraph_3Paras_PT)cl;
    return xdigraph_add_edge_repeat(paras->graph/*ngraph*/, tvertex, paras->para1/*svertex*/);
}

static
bool xdigraph_reverse_vertex_apply(void *svertex, void **adjset, void *cl) {
    if (xset_is_empty(*adjset)) {
        return xdigraph_add_vertex((XDigraph_PT)cl/*ngraph*/, svertex);
    }
    else {
        XDigraph_3Paras_T paras = { (XDigraph_PT)cl/*ngraph*/, svertex, NULL, NULL };
        return !xset_map_break_if_false((XSet_PT)(*adjset), xdigraph_reverse_adjset_apply, (void*)&paras);
    }
}

XDigraph_PT xdigraph_reverse(XDigraph_PT graph) {
    xassert(graph);

    if (!graph) {
        return NULL;
    }

    {
        XDigraph_PT ngraph = xdigraph_new(graph->cmp, graph->cl);
        if (!ngraph) {
            return NULL;
        }

        if (xmap_map_break_if_false(graph->adjsets, xdigraph_reverse_vertex_apply, ngraph)) {
            xdigraph_free(&ngraph);
            return NULL;
        }

        return ngraph;
    }
}

static
bool xdigraph_to_graph_set_apply(void *vertex, void *cl) {
    XDigraph_3Paras_PT paras = (XDigraph_3Paras_PT)cl;
    if (!xdigraph_is_edge(paras->graph, vertex, paras->para2)) {
        return xdigraph_add_edge_repeat(paras->para1, vertex, paras->para2);
    }

    return true;
}

static
bool xdigraph_to_graph_map_apply(void *vertex, void **adjset, void *cl) {
    XDigraph_3Paras_PT paras = (XDigraph_3Paras_PT)cl;
    paras->para2 = vertex; /*source vertex*/
    return !xset_map_break_if_false(*((XSet_PT*)adjset), xdigraph_to_graph_set_apply, cl);
}

XGraph_PT xdigraph_to_graph(XDigraph_PT graph) {
    XGraph_PT ngraph = xdigraph_copy(graph);
    if (!ngraph) {
        return NULL;
    }

    {
        XDigraph_3Paras_T paras = { graph, ngraph, NULL/*source vertex*/, NULL };

        /* add the other direction edge for all the edges which has only one direction in digraph */
        if (xmap_map_break_if_false(graph->adjsets, xdigraph_to_graph_map_apply, (void*)&paras)) {
            xgraph_free(&ngraph);
            return NULL;
        }
    }

    return ngraph;
}

bool xdigraph_add_vertex(XDigraph_PT graph, void *vertex) {
    return xgraph_add_vertex(graph, vertex);
}

static
bool xdigraph_remove_vertex_apply(void *nvertex, void **adjset, void *cl) {
    xset_remove_all((XSet_PT)(*adjset), cl/*vertex to delete*/);
    return true;
}

void xdigraph_remove_vertex(XDigraph_PT graph, void *vertex) {
    XSet_PT adjset = xdigraph_adjset(graph, vertex);
    if (!adjset) {
        return ;
    }

    xmap_map(graph->adjsets, xdigraph_remove_vertex_apply, vertex);

    xmap_remove(graph->adjsets, vertex);
    xset_free(&adjset);
}

bool xdigraph_is_vertex(XDigraph_PT graph, void *vertex) {
    return xmap_find(graph ? graph->adjsets : NULL, vertex);
}

int xdigraph_vertex_size(XDigraph_PT graph) {
    return xmap_size(graph ? graph->adjsets : NULL);
}

XSet_PT xdigraph_adjset(XDigraph_PT graph, void *vertex) {
    return (XSet_PT)xmap_get(graph ? graph->adjsets : NULL, vertex);
}

bool xdigraph_is_adjacent(XDigraph_PT graph, void *svertex, void *tvertex) {
    return xset_find(xdigraph_adjset(graph, svertex), tvertex);
}

static
bool xdigraph_add_edge_impl(XDigraph_PT graph, void *svertex, void *tvertex, bool (*xset_put_function)(XSet_PT set, void *key)) {
    if (!xdigraph_is_vertex(graph, svertex)) {
        if (!xdigraph_add_vertex(graph, svertex)) {
            return false;
        }
    }

    if (!xdigraph_is_vertex(graph, tvertex)) {
        if (!xdigraph_add_vertex(graph, tvertex)) {
            return false;
        }
    }

    return xset_put_function(xdigraph_adjset(graph, svertex), tvertex);
}

bool xdigraph_add_edge_unique(XDigraph_PT graph, void *svertex, void *tvertex) {
    return xdigraph_add_edge_impl(graph, svertex, tvertex, xset_put_unique);
}

bool xdigraph_add_edge_repeat(XDigraph_PT graph, void *svertex, void *tvertex) {
    return xdigraph_add_edge_impl(graph, svertex, tvertex, xset_put_repeat);
}

void xdigraph_remove_edge(XDigraph_PT graph, void *svertex, void *tvertex) {
    xset_remove(xdigraph_adjset(graph, svertex), tvertex);
}

void xdigraph_remove_edge_all(XDigraph_PT graph, void *svertex, void *tvertex) {
    xset_remove_all(xdigraph_adjset(graph, svertex), tvertex);
}

bool xdigraph_is_edge(XDigraph_PT graph, void *svertex, void *tvertex) {
    return xset_find(xdigraph_adjset(graph, svertex), tvertex);
}

static
bool xdigraph_edge_size_apply(void *vertex, void **adjset, void *cl) {
    int *size = (int*)cl;
    *size += xset_size((XSet_PT)(*adjset));
    return true;
}

int xdigraph_edge_size(XDigraph_PT graph) {
    int size = 0;
    xmap_map(graph ? graph->adjsets : NULL, xdigraph_edge_size_apply, (void*)&size);
    return size;
}

bool xdigraph_has_parallel_edge(XDigraph_PT graph, void *vertex) {
    return xset_has_repeat_elems_except(xdigraph_adjset(graph, vertex), vertex);
}

int xdigraph_parallel_edge_size(XDigraph_PT graph, void *vertex) {
    return xset_repeat_elems_size_except(xdigraph_adjset(graph, vertex), vertex);
}

void xdigraph_remove_parallel_edge(XDigraph_PT graph, void *vertex) {
    xset_unique_except(xdigraph_adjset(graph, vertex), vertex);
}

bool xdigraph_has_parallel_edges(XDigraph_PT graph) {
    return xgraph_has_parallel_edges(graph);
}

void xdigraph_remove_parallel_edges(XDigraph_PT graph) {
    xgraph_remove_parallel_edges(graph);
}

int xdigraph_parallel_edges_size(XDigraph_PT graph) {
    return 2 * xgraph_parallel_edges_size(graph);
}

bool xdigraph_has_self_cycle(XDigraph_PT graph, void *vertex) {
    return xset_find(xdigraph_adjset(graph, vertex), vertex);
}

int xdigraph_self_cycle_size(XDigraph_PT graph, void *vertex) {
    return xset_elem_size(xdigraph_adjset(graph, vertex), vertex);
}

void xdigraph_remove_self_cycle(XDigraph_PT graph, void *vertex) {
    xset_remove_all(xdigraph_adjset(graph, vertex), vertex);
}

bool xdigraph_has_self_cycles(XDigraph_PT graph) {
    return xgraph_has_self_cycles(graph);
}

void xdigraph_remove_self_cycles(XDigraph_PT graph) {
    xgraph_remove_self_cycles(graph);
}

int xdigraph_self_cycles_size(XDigraph_PT graph) {
    return xgraph_self_cycles_size(graph);
}

bool xdigraph_is_simple(XDigraph_PT graph) {
    return !xdigraph_has_self_cycles(graph) && !xdigraph_has_parallel_edges(graph);
}

void xdigraph_simple_graph(XDigraph_PT graph) {
    xgraph_simple_graph(graph);
}

XDList_PT xdigraph_shortest_path(XDigraph_PT graph, void *svertex, void *tvertex) {
    return xgraph_shortest_path(graph, svertex, tvertex);
}

XMap_PT xdigraph_shortest_paths(XDigraph_PT graph, void *vertex) {
    return xgraph_shortest_paths(graph, vertex);
}

XDList_PT xdigraph_path_bfs(XDigraph_PT graph, void *svertex, void *tvertex) {
    return xgraph_path_bfs(graph, svertex, tvertex);
}

XMap_PT xdigraph_paths_bfs(XDigraph_PT graph, void *vertex) {
    return xgraph_paths_bfs(graph, vertex);
}

XDList_PT xdigraph_path_dfs(XDigraph_PT graph, void *svertex, void *tvertex) {
    return xgraph_path_dfs(graph, svertex, tvertex);
}

XMap_PT xdigraph_paths_dfs(XDigraph_PT graph, void *vertex) {
    return xgraph_paths_dfs(graph, vertex);
}

bool xdigraph_is_reachable(XDigraph_PT graph, void *svertex, void *tvertex) {
    return xgraph_is_connected(graph, svertex, tvertex);
}

static
bool xdigraph_reachable_from_dfs_impl_apply(void *vertex, void *cl) {
    XDigraph_3Paras_PT paras = cl;

    /* ignore the vertex which is in marked already */
    if (xset_find((XSet_PT)(paras->para1/*marked*/), vertex)) {
        return true;
    }

    /* save the scaned vertex into marked */
    if (!xset_put_repeat((XSet_PT)paras->para1/*marked*/, vertex)) {
        return false;
    }

    /* save the scaned vertex into ccset */
    if (!xset_put_repeat((XSet_PT)paras->para2/*ccset*/, vertex)) {
        return false;
    }

    if (xset_map_break_if_false(xgraph_adjset(paras->graph, vertex), xdigraph_reachable_from_dfs_impl_apply, (void*)paras)) {
        return false;
    }

    return true;
}

static
XSet_PT xdigraph_reachable_from_dfs_impl(XDigraph_PT graph, void *vertex, XSet_PT marked) {
    XSet_PT ccset = xset_new(graph->cmp, graph->cl);
    if (!ccset) {
        return NULL;
    }

    {
        XDigraph_3Paras_T paras = { graph, marked, ccset, NULL };

        if (!xdigraph_reachable_from_dfs_impl_apply(vertex, (void*)&paras)) {
            xset_free(&ccset);
            return NULL;
        }
    }

    return ccset;
}

XSet_PT xdigraph_reachable_from_dfs(XDigraph_PT graph, void *vertex) {
    XSet_PT marked = xset_new(graph->cmp, graph->cl);
    if (!marked) {
        return NULL;
    }

    {
        XSet_PT ccset = xdigraph_reachable_from_dfs_impl(graph, vertex, marked);
        xset_free(&marked);
        return ccset;
    }
}

static
bool xdigraph_all_reachable_from_dfs_apply(void *vertex, void *cl) {
    XDigraph_3Paras_PT paras = cl;

    /* ignore the vertex which is in marked already */
    if (xset_find((XSet_PT)(paras->para1/*marked*/), vertex)) {
        return true;
    }

    /* save the scaned vertex into marked */
    if (!xset_put_repeat((XSet_PT)paras->para1/*marked*/, vertex)) {
        return false;
    }

    if (xset_map_break_if_false(xgraph_adjset(paras->graph, vertex), xdigraph_all_reachable_from_dfs_apply, (void*)paras)) {
        return false;
    }

    return true;
}

XSet_PT xdigraph_all_reachable_from_dfs(XDigraph_PT graph, XSet_PT vset) {
    XSet_PT marked = xset_new(graph->cmp, graph->cl);
    if (!marked) {
        return NULL;
    }

    {
        XDigraph_3Paras_T paras = { graph, marked, NULL, NULL };

        if (xset_map_break_if_false(vset, xdigraph_all_reachable_from_dfs_apply, (void*)&paras)) {
            xset_free(&marked);
            return NULL;
        }
    }

    return marked;
}

XDList_PT xdigraph_path_by_found_paths(XDigraph_PT graph, XMap_PT paths, void *svertex, void *tvertex) {
    return xgraph_path_by_found_paths(graph, paths, svertex, tvertex);
}

static
bool xdigraph_cycle_dfs_impl_apply(void *vertex, void *cl) {
    XDigraph_6Paras_PT paras = cl;

    /* cycle found */
    if (xset_find((XSet_PT)(paras->para4/*on_stack*/), vertex)) {
        /* igore self cycle A -> A */
        if (paras->graph->cmp(vertex, xstack_peek(paras->para3)/*parent vertex*/, paras->graph->cl) == 0) {
            /* return true to continue the search */
            return true;
        }

        if (paras->para5/*vertex_in_cycle*/) {
            /* cycle found with vertex_in_cycle inside */
            if (paras->graph->cmp(paras->para5/*vertex_in_cycle*/, vertex, paras->graph->cl) == 0) {
                xmap_put_replace((XMap_PT)(paras->para2/*paths*/), vertex, xstack_peek(paras->para3)/*parent vertex*/, NULL);

                /* return false to break the search */
                return false;
            }

            /* ignore it if it's not vertex_in_cycle */
            return true;
        }
        /* no vertex_in_cycle assigned, any cycle found */
        else {
            xmap_put_replace((XMap_PT)(paras->para2/*paths*/), vertex, xstack_peek(paras->para3)/*parent vertex*/, NULL);

            /* record this vertex */
            paras->para5/*vertex_in_cycle*/ = vertex;

            /* return false to break the search */
            return false;
        }
    }

    if (!xmap_put_repeat((XMap_PT)(paras->para2/*paths*/), vertex, xstack_peek(paras->para3)/*parent vertex*/)) {
        return false;
    }

    if (!xset_put_unique((XSet_PT)paras->para1/*marked*/, vertex)) {
        return false;
    }

    if (!xstack_push_no_limit((XStack_PT)paras->para3/*stack*/, vertex)) {
        return false;
    }

    if (!xset_put_repeat((XSet_PT)paras->para4/*on_stack*/, vertex)) {
        return false;
    }

    if (xset_map_break_if_false(xdigraph_adjset(paras->graph, vertex), xdigraph_cycle_dfs_impl_apply, (void*)paras)) {
        return false;
    }

    xset_remove((XSet_PT)paras->para4/*on_stack*/, vertex);
    xstack_pop((XStack_PT)paras->para3/*pre_vstack*/);

    return true;
}

static
XMap_PT xdigraph_cycle_dfs_impl(XDigraph_PT graph, void *vertex, void **vertex_in_cycle, XSet_PT marked) {
    XMap_PT paths = xmap_new(graph->cmp, graph->cl);
    if (!paths) {
        return NULL;
    }
    XStack_PT stack = xstack_new(xgraph_vertex_size(graph));
    if (!stack) {
        xmap_free(&paths);
        return NULL;
    }
    XSet_PT on_stack = xset_new(graph->cmp, graph->cl);
    if (!on_stack) {
        xstack_free(&stack);
        xmap_free(&paths);
        return NULL;
    }

    {
        XDigraph_6Paras_T paras = { graph, marked, paths, stack, on_stack, *vertex_in_cycle, NULL };
        xdigraph_cycle_dfs_impl_apply(vertex, (void*)&paras);

        xstack_free(&stack);
        xset_free(&on_stack);

        if (*vertex_in_cycle) {
            if (xmap_find(paths, *vertex_in_cycle)) {
                return paths;
            }
        }
        else {
            if (paras.para5) {
                *vertex_in_cycle = paras.para5;
                return paths;
            }
        }

        xmap_free(&paths);
        return NULL;
    }
}

XDList_PT xdigraph_cycle_include(XDigraph_PT graph, void *vertex) {
    if (!xdigraph_is_vertex(graph, vertex)) {
        return NULL;
    }

    {
        XSet_PT marked = xset_new(graph->cmp, graph->cl);
        if (!marked) {
            return NULL;
        }

        {
            XMap_PT paths = xdigraph_cycle_dfs_impl(graph, vertex, &vertex, marked);
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
bool xdigraph_any_cycle_dfs_apply(void *vertex, void **adjset, void *cl) {
    XDigraph_3Paras_PT paras = cl;

    /* ingore the vertex which is in marked already */
    if (xset_find((XSet_PT)paras->para1/*marked*/, vertex)) {
        return true;
    }

    {
        void *vertex_in_cycle = NULL;
        XMap_PT paths = xdigraph_cycle_dfs_impl(paras->graph, vertex, &vertex_in_cycle, paras->para1/*marked*/);

        if (vertex_in_cycle) {
            paras->para3/*vertex_in_cycle*/ = vertex_in_cycle;
            paras->para2/*paths*/ = paths;
            return false;
        }
    }

    return true;
}

XDList_PT xdigraph_any_cycle(XDigraph_PT graph) {
    XSet_PT marked = xset_new(graph->cmp, graph->cl);
    if (!marked) {
        return NULL;
    }

    {
        XDigraph_3Paras_T paras = { graph, marked, NULL/*paths*/, NULL/*vertex_in_cycle*/ };
        xmap_map_break_if_false(graph->adjsets, xdigraph_any_cycle_dfs_apply, (void*)&paras);
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

XDList_PT xdigraph_any_cycle_check_from(XDigraph_PT graph, void *vertex) {
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
            XMap_PT paths = xdigraph_cycle_dfs_impl(graph, vertex, &vertex_in_cycle, marked);
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

bool xdigraph_has_cycle_include(XDigraph_PT graph, void *vertex) {
    XDList_PT list = xdigraph_cycle_include(graph, vertex);
    if (list) {
        xdlist_free(&list);
        return true;
    }

    return false;
}

bool xdigraph_has_any_cycle_check_from(XDigraph_PT graph, void *vertex) {
    XDList_PT list = xdigraph_any_cycle_check_from(graph, vertex);
    if (list) {
        xdlist_free(&list);
        return true;
    }

    return false;
}

bool xdigraph_has_any_cycle(XDigraph_PT graph) {
    XDList_PT list = xdigraph_any_cycle(graph);
    if (list) {
        xdlist_free(&list);
        return true;
    }

    return false;
}

XDList_PT xdigraph_two_color_dfs(XDigraph_PT graph) {
    return xgraph_two_color_dfs(graph);
}

XDList_PT xdigraph_two_color_bfs(XDigraph_PT graph) {
    return xgraph_two_color_bfs(graph);
}

bool xdigraph_is_two_color(XDigraph_PT graph) {
    return xgraph_is_two_color(graph);
}

static
bool xdigraph_map_dfs_map_apply(void *vertex, void **adjset, void *cl);

static
bool xdigraph_map_dfs_set_apply(void *nvertex, void *ncl) {
    XDigraph_6Paras_PT paras = (XDigraph_6Paras_PT)ncl;

    if (!xset_find((XSet_PT)paras->para1/*marked*/, nvertex)) {
        void *adjset = xdigraph_adjset(paras->graph, nvertex);
        return xdigraph_map_dfs_map_apply(nvertex, &adjset, ncl);
    }

    return true;
}

static
bool xdigraph_map_dfs_map_apply(void *vertex, void **adjset, void *cl) {
    XDigraph_6Paras_PT paras = (XDigraph_6Paras_PT)cl;

    XSet_PT marked = (XSet_PT)(paras->para1/*marked*/);
    if (xset_find(marked, vertex)) {
        return true;
    }

    {
        if (!xset_put_repeat(marked, vertex)) {
            return false;
        }

        /* pre order */
        if (!xdlist_push_back_repeat((XDList_PT)paras->para2/*pre*/, vertex)) {
            return false;
        }

        if (xset_map_break_if_false((XSet_PT)(*adjset), xdigraph_map_dfs_set_apply, paras)) {
            return false;
        }

        /* post order */
        if (!xdlist_push_back_repeat((XDList_PT)paras->para3/*pre*/, vertex)) {
            return false;
        }

        /* reverse post order */
        if (!xdlist_push_front_repeat((XDList_PT)paras->para4/*pre*/, vertex)) {
            return false;
        }
    }

    return true;
}

static
bool xdigraph_map_dfs(XDigraph_PT graph, XDList_PT *pre_order, XDList_PT *post_order, XDList_PT *reverse_post_order) {
    if (xdigraph_vertex_size(graph) == 0) {
        return false;
    }

    {
        XDList_PT pre = xdlist_new();
        XDList_PT post = xdlist_new();
        XDList_PT reverse_post = xdlist_new();

        if (!pre || !post || !reverse_post) {
            xdlist_free(pre ? &pre : NULL);
            xdlist_free(post ? &post : NULL);
            xdlist_free(reverse_post ? &reverse_post : NULL);
            return false;
        }

        {
            XSet_PT marked = xset_new(graph->cmp, graph->cl);
            if (!marked) {
                return false;
            }

            {
                XDigraph_6Paras_T paras = { graph, marked, (void*)pre, (void*)post, (void*)reverse_post, NULL, NULL };

                if (xmap_map_break_if_false(graph->adjsets, xdigraph_map_dfs_map_apply, (void*)&paras)) {
                    xdlist_free(&pre);
                    xdlist_free(&post);
                    xdlist_free(&reverse_post);
                    xset_free(&marked);
                    return false;
                }
            }

            xset_free(&marked);
        }

        if (pre_order) {
            *pre_order = pre;
        }
        else {
            xdlist_free(&pre);
        }

        if (post_order) {
            *post_order = post;
        }
        else {
            xdlist_free(&post);
        }

        if (reverse_post_order) {
            *reverse_post_order = reverse_post;
        }
        else {
            xdlist_free(&reverse_post);
        }
    }

    return true;
}

XDList_PT xdigraph_pre_order_dfs(XDigraph_PT graph) {
    XDList_PT pre_order = NULL;
    return xdigraph_map_dfs(graph, &pre_order, NULL, NULL) ? pre_order : NULL;
}

XDList_PT xdigraph_post_order_dfs(XDigraph_PT graph) {
    XDList_PT post_order = NULL;
    return xdigraph_map_dfs(graph, NULL, &post_order, NULL) ? post_order : NULL;
}

XDList_PT xdigraph_reverse_post_order_dfs(XDigraph_PT graph) {
    XDList_PT reverse_post_order = NULL;
    return xdigraph_map_dfs(graph, NULL, NULL, &reverse_post_order) ? reverse_post_order : NULL;
}

bool xdigraph_map_pre_order(XDigraph_PT graph, bool (*apply)(void *vertex, void *cl), void *cl) {
    XDList_PT pre_order = xdigraph_pre_order_dfs(graph);
    if (pre_order) {
        if (xdlist_map_break_if_false(pre_order, apply, cl)) {
            xdlist_free(&pre_order);
            return false;
        }
    }

    return false;
}

bool xdigraph_map_post_order(XDigraph_PT graph, bool (*apply)(void *vertex, void *cl), void *cl) {
    XDList_PT post_order = xdigraph_post_order_dfs(graph);
    if (post_order) {
        if (xdlist_map_break_if_false(post_order, apply, cl)) {
            xdlist_free(&post_order);
            return false;
        }
    }

    return false;
}

bool xdigraph_map_reverse_post_order(XDigraph_PT graph, bool (*apply)(void *vertex, void *cl), void *cl) {
    XDList_PT reverse_post_order = xdigraph_reverse_post_order_dfs(graph);
    if (reverse_post_order) {
        if (xdlist_map_break_if_false(reverse_post_order, apply, cl)) {
            xdlist_free(&reverse_post_order);
            return false;
        }
    }

    return false;
}

XDList_PT xdigraph_topologial_order_dfs(XDigraph_PT graph) {
    if (xdigraph_has_any_cycle(graph)) {
        return NULL;
    }

    return xdigraph_reverse_post_order_dfs(graph);
}

static
XSet_PT xdigraph_strongly_connected_dfs_impl(XDigraph_PT graph, void *vertex, XDigraph_PT rgraph, XSet_PT marked) {
    XDList_PT list = xdigraph_reverse_post_order_dfs(rgraph);
    if (!list) {
        return NULL;
    }

    while (!xdlist_is_empty(list)) {
        void *rvertex = xdlist_pop_front(list);

        if (xset_find(marked, rvertex)) {
            continue;
        }

        /* get the strong ccset */
        XSet_PT ccset = xdigraph_reachable_from_dfs_impl(graph, rvertex, marked);
        if (!ccset) {
            xdlist_free(&list);
            return NULL;
        }

        /* check if vertex is in that ccset */
        if (xset_find(ccset, vertex)) {
            xdlist_free(&list);
            return ccset;
        }

        /* vertex is not in the found strong ccset, prepar to search the next one */
        if (!xset_merge_repeat(marked, &ccset)) {
            xdlist_free(&list);
            xset_free(&ccset);
            return NULL;
        }
    }

    xdlist_free(&list);
    return NULL;
}

XSet_PT xdigraph_strongly_connected_dfs(XDigraph_PT graph, void *vertex) {
    if (!xdigraph_is_vertex(graph, vertex)) {
        return NULL;
    }

    {
        XSet_PT marked = xset_new(graph->cmp, graph->cl);
        if (!marked) {
            return NULL;
        }
        XDigraph_PT rgraph = xdigraph_reverse(graph);
        if (!rgraph) {
            xset_free(&marked);
            return NULL;
        }

        {
            XSet_PT strong_union = xdigraph_strongly_connected_dfs_impl(graph, vertex, rgraph, marked);

            xset_free(&marked);
            xdigraph_free(&rgraph);

            return strong_union;
        }
    }
}

static
bool xdigraph_all_strongly_connected_dfs_free_apply(void *ccset, void *cl) {
    xset_free((XSet_PT*)&ccset);
    return true;
}

static 
void* xdigraph_all_strongly_connected_dfs_free(XSet_PT *marked, XDList_PT *ccsets) {
    xset_free(marked);
    xdlist_map(*ccsets, xdigraph_all_strongly_connected_dfs_free_apply, NULL);
    xdlist_free(ccsets);
    return NULL;
}

static 
bool xdigraph_all_strongly_connected_dfs_impl(XDigraph_PT graph, XDigraph_PT rgraph, XSet_PT marked, XDList_PT ccsets) {
    XDList_PT list = xdigraph_reverse_post_order_dfs(rgraph);
    if (!list) {
        return false;
    }

    while (!xdlist_is_empty(list)) {
        void *vertex = xdlist_pop_front(list);
        if (xset_find(marked, vertex)) {
            continue;
        }

        XSet_PT ccset = xdigraph_reachable_from_dfs_impl(graph, vertex, marked);
        if (!ccset) {
            xdlist_free(&list);
            return false;
        }

        if (!xdlist_push_back_repeat(ccsets, (void*)ccset)) {
            xdlist_free(&list);
            xset_free(&ccset);
            return false;
        }
    }

    xdlist_free(&list);
    return true;
}

XDList_PT xdigraph_all_strongly_connected_dfs(XDigraph_PT graph) {
    if (xdigraph_vertex_size(graph) == 0) {
        return NULL;
    }

    {
        XSet_PT marked = xset_new(graph->cmp, graph->cl);
        XDList_PT ccsets = xdlist_new();

        if (!marked || !ccsets) {
            return xdigraph_all_strongly_connected_dfs_free(&marked, &ccsets);
        }

        {
            XDigraph_PT rgraph = xdigraph_reverse(graph);
            if (!rgraph) {
                return xdigraph_all_strongly_connected_dfs_free(&marked, &ccsets);
            }

            if (!xdigraph_all_strongly_connected_dfs_impl(graph, rgraph, marked, ccsets)) {
                xdigraph_free(&rgraph);
                return xdigraph_all_strongly_connected_dfs_free(&marked, &ccsets);
            }

            xset_free(&marked);
            xdigraph_free(&rgraph);

            return ccsets;
        }
    }
}

bool xdigraph_is_strongly_connected_dfs(XDigraph_PT graph) {
    XSet_PT ccset = xdigraph_strongly_connected_dfs(graph, xmap_select(graph ? graph->adjsets : NULL, 0));
    int size = xset_size(ccset);
    xset_free(&ccset);
    return size == xdigraph_vertex_size(graph);
}

bool xdigraph_is_connected_dfs(XDigraph_PT graph) {
    XGraph_PT ngraph = xdigraph_to_graph(graph);
    if (!ngraph) {
        return false;
    }

    {
        XSet_PT ccset = xgraph_union_dfs(ngraph, xmap_select(ngraph->adjsets, 0));
        int size = xset_size(ccset);

        if (ccset) {
            xset_free(&ccset);
        }
        xgraph_free(&ngraph);

        return size == xdigraph_vertex_size(graph);
    }
}

static
bool xdigraph_transitive_closure_map_union_apply(void *vertex, void *parent_vertex, void *cl) {
    // ignore self cycle
    if (!parent_vertex) {
        return true;
    }

    {
        XDigraph_3Paras_PT paras = (XDigraph_3Paras_PT)cl;
        return xdigraph_add_edge_repeat((XDigraph_PT)paras->para1/*ngraph*/, paras->para2/*source_vertex*/, vertex);
    }
}

static
bool xdigraph_transitive_closure_map_apply(void *vertex, void **adjset, void *cl) {
    XDigraph_3Paras_PT paras = (XDigraph_3Paras_PT)cl;
    paras->para2 = vertex /*source_vertex*/;

    XSet_PT marked = xgraph_map_union_dfs(paras->graph, vertex, xdigraph_transitive_closure_map_union_apply, (void*)paras);
    if (!marked) {
        return false;
    }
    else {
        xset_free(&marked);
        return true;
    }
}

/* TODO : more effective way ? */
XDigraph_PT xdigraph_transitive_closure(XDigraph_PT graph) {
    xassert(graph);

    if (!graph) {
        return NULL;
    }

    {
        XDigraph_PT ngraph = xdigraph_new(graph->cmp, graph->cl);
        if (!ngraph) {
            return NULL;
        }

        XDigraph_3Paras_T paras = { graph, (void*)ngraph, NULL/*source_vertex*/, NULL };
        if (xmap_map_break_if_false(graph->adjsets, xdigraph_transitive_closure_map_apply, (void*)&paras)) {
            xdigraph_free(&ngraph);
            return NULL;
        }

        return ngraph;
    }
}

static
bool xdigraph_in_degree_apply(void *svertex, void **adjset, void *cl) {
    XDigraph_3Paras_PT paras = (XDigraph_3Paras_PT)cl;
    int *degree = (int*)paras->para2;
    *degree += xset_elem_size((XSet_PT)(*adjset), paras->para1/*source vertex*/);
    return true;
}

int xdigraph_in_degree(XDigraph_PT graph, void *vertex) {
    if (!xdigraph_is_vertex(graph, vertex)) {
        return 0;
    }

    {
        int degree = 0;

        XDigraph_3Paras_T paras = { graph, vertex, (void*)&degree, NULL };
        xmap_map(graph->adjsets, xdigraph_in_degree_apply, &paras);

        return degree;
    }
}

int xdigraph_out_degree(XDigraph_PT graph, void *vertex) {
    return xset_size(xdigraph_adjset(graph, vertex));
}

static
bool xdigraph_max_in_degree_apply(void *svertex, void **adjset, void *cl) {
    XDigraph_3Paras_PT paras = (XDigraph_3Paras_PT)cl;
    int *max = (int*)paras->para1;

    int size = xdigraph_in_degree(paras->graph, svertex);

    if (*max < size) {
        *max = size;
    }

    return true;
}

int xdigraph_max_in_degree(XDigraph_PT graph) {
    int max = 0;
    XDigraph_3Paras_T paras = { graph, (void*)&max, NULL, NULL };
    xmap_map(graph ? graph->adjsets : NULL, xdigraph_max_in_degree_apply, &paras);
    return max;
}

int xdigraph_max_out_degree(XDigraph_PT graph) {
    return xgraph_max_degree(graph);
}

double xdigraph_average_degree(XDigraph_PT graph) {
    return (double)xdigraph_edge_size(graph) / xdigraph_vertex_size(graph);
}

double xdigraph_density(XDigraph_PT graph) {
    return (2 * xdigraph_edge_size(graph)) / xdigraph_vertex_size(graph);
}

bool xdigraph_is_dense(XDigraph_PT graph) {
    return 1 <= (xdigraph_density(graph) / xdigraph_vertex_size(graph));
}

bool xdigraph_is_sparse(XDigraph_PT graph) {
    return (xdigraph_density(graph) / xdigraph_vertex_size(graph)) < 1;
}
