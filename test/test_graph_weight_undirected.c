
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/xutils.h"
#include "../graph_weight_undirected/xgraph_weight_undirected_x.h"
#include "../include/xalgos.h"

static
bool xwgraph_edge_set_print(void *elem, void *cl) {
    XWEdge_PT edge = elem;
    printf("{%s, %s, %f}\n", (char*)edge->v, (char*)edge->w, edge->weight);
    return true;    
}

static
int xwgraph_test_cmp(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static
bool xwgraph_to_string_adjset_apply(void *edge, void *cl) {
    XWEdge_PT edge1 = edge;
    printf("(%s, %s : %.2f) ", (char*)edge1->v, (char*)edge1->w, edge1->weight);
    return true;
}

static
bool xwgraph_to_string_vertex_apply(void *vertex, void **adjset, void *cl) {
    printf("%s : ", (char*)vertex);
    xset_map((XSet_PT)(*adjset), xwgraph_to_string_adjset_apply, NULL);
    printf("\n");
    return true;
}

static
bool xwgraph_mst_forest_prim_free_apply(void *set, void *cl) {
    xset_free((XSet_PT*)&set);
    return true;
}

static
void xwgraph_to_string(XWGraph_PT graph) {
    xmap_map(graph->wadjsets, xwgraph_to_string_vertex_apply, NULL);
}

XWGraph_PT xwgraph_test_graph1(void) {
    XWGraph_PT graph = xwgraph_new(xwgraph_test_cmp, NULL);
    xwgraph_add_edge_repeat(graph, "4", "5", 0.35);
    xwgraph_add_edge_repeat(graph, "4", "7", 0.37);
    xwgraph_add_edge_repeat(graph, "5", "7", 0.28);
    xwgraph_add_edge_repeat(graph, "5", "7", 0.28);  /* parallel edge */
    xwgraph_add_edge_repeat(graph, "5", "7", 0.28);  /* parallel edge */
    xwgraph_add_edge_repeat(graph, "0", "7", 0.16);
    xwgraph_add_edge_repeat(graph, "1", "5", 0.32);
    xwgraph_add_edge_repeat(graph, "0", "4", 0.38);
    xwgraph_add_edge_repeat(graph, "2", "3", 0.17);
    xwgraph_add_edge_repeat(graph, "1", "7", 0.19);
    xwgraph_add_edge_repeat(graph, "1", "7", 0.19);  /* parallel edge */
    xwgraph_add_edge_repeat(graph, "0", "2", 0.26);
    xwgraph_add_edge_repeat(graph, "1", "2", 0.36);
    xwgraph_add_edge_repeat(graph, "1", "3", 0.29);
    xwgraph_add_edge_repeat(graph, "2", "7", 0.34);
    xwgraph_add_edge_repeat(graph, "2", "2", 0.01);  /* self-cycle */
    xwgraph_add_edge_repeat(graph, "2", "2", 0.02);  /* self-cycle */
    xwgraph_add_edge_repeat(graph, "6", "2", 0.40);
    xwgraph_add_edge_repeat(graph, "3", "6", 0.52);
    xwgraph_add_edge_repeat(graph, "6", "0", 0.58);
    xwgraph_add_edge_repeat(graph, "6", "4", 0.93);
    return graph;
}

void test_xwgraph() {
    /* xwgraph_new */
    {
        XWGraph_PT graph = xwgraph_new(xwgraph_test_cmp, NULL);
        xassert(graph->native_graph);
        xassert(graph->wadjsets);
        xwgraph_free(&graph);
    }

    /* xwgraph_add_vertex */
    {
        XWGraph_PT graph = xwgraph_new(xwgraph_test_cmp, NULL);

        xassert(xwgraph_add_vertex(graph, "a"));
        xassert(xwgraph_add_vertex(graph, "b"));
        xassert(xwgraph_add_vertex(graph, "c"));
        xassert(xwgraph_add_vertex(graph, "a"));
        xassert(xwgraph_add_vertex(graph, "a"));
        xassert(xwgraph_add_vertex(graph, "b"));

        xassert(xmap_size(graph->wadjsets) == 3);
        xassert(xmap_find(graph->wadjsets, "a"));
        xassert(xmap_find(graph->wadjsets, "b"));
        xassert(xmap_find(graph->wadjsets, "c"));

        xwgraph_free(&graph);
    }

    /* xwgraph_add_edge_repeat */
    {
        XWGraph_PT graph = xwgraph_new(xwgraph_test_cmp, NULL);

        /* self cycle */
        xwgraph_add_edge_repeat(graph, "a", "a", 1);
        xassert(xset_size(xwgraph_adjset(graph, "a")) == 1);
        xwgraph_add_edge_repeat(graph, "a", "a", 2);
        xassert(xset_size(xwgraph_adjset(graph, "a")) == 2);
        xwgraph_add_edge_repeat(graph, "a", "a", 3);
        xassert(xset_size(xwgraph_adjset(graph, "a")) == 3);

        /* parallel edge */
        xwgraph_add_edge_repeat(graph, "b", "c", 1);
        xassert(xset_size(xwgraph_adjset(graph, "b")) == 1);
        xwgraph_add_edge_repeat(graph, "b", "c", 2);
        xassert(xset_size(xwgraph_adjset(graph, "b")) == 2);
        xwgraph_add_edge_repeat(graph, "b", "c", 3);
        xassert(xset_size(xwgraph_adjset(graph, "b")) == 3);

        //xwgraph_to_string(graph);

        xwgraph_free(&graph);
    }

    /* xwgraph_add_edge_unique */
    {
        XWGraph_PT graph = xwgraph_new(xwgraph_test_cmp, NULL);

        /* self cycle */
        xwgraph_add_edge_unique(graph, "a", "a", 1);
        xassert(xset_size(xwgraph_adjset(graph, "a")) == 1);
        xwgraph_add_edge_unique(graph, "a", "a", 2);
        xassert(xset_size(xwgraph_adjset(graph, "a")) == 1);

        /* parallel edge */
        xwgraph_add_edge_unique(graph, "b", "c", 1);
        xassert(xset_size(xwgraph_adjset(graph, "b")) == 1);
        xwgraph_add_edge_unique(graph, "b", "c", 2);
        xassert(xset_size(xwgraph_adjset(graph, "b")) == 1);

        xwgraph_free(&graph);
    }

    /* xwgraph_mst_lazy_prim */
    {
        XWGraph_PT graph = xwgraph_test_graph1();

        XSet_PT set = xwgraph_mst_lazy_prim(graph, "0");
        xassert(xset_size(set) == 7);
        //xset_map(set, xwgraph_edge_set_print, NULL);

        xset_free(&set);
        xwgraph_free(&graph);
    }

    /* xwgraph_mst_prim */
    {
        XWGraph_PT graph = xwgraph_test_graph1();

        XSet_PT set = xwgraph_mst_prim(graph, "0");
        xassert(xset_size(set) == 7);
        //xset_map(set, xwgraph_edge_set_print, NULL);

        xset_free(&set);
        xwgraph_free(&graph);
    }

    /* xwgraph_mst_forest_prim */
    {
        XWGraph_PT graph = xwgraph_test_graph1();
        xwgraph_add_edge_repeat(graph, "8", "9", 0.93);

        XDList_PT msts = xwgraph_mst_forest_prim(graph);
        xassert(xdlist_size(msts) == 2);

        xdlist_map(msts, xwgraph_mst_forest_prim_free_apply, NULL);
        xdlist_free(&msts);
        xwgraph_free(&graph);
    }

    /* xwgraph_mst_kruskal */
    {
        XWGraph_PT graph = xwgraph_test_graph1();

        XSet_PT set = xwgraph_mst_kruskal(graph, "0");
        xassert(xset_size(set) == 7);
        //xset_map(set, xwgraph_edge_set_print, NULL);

        xset_free(&set);
        xwgraph_free(&graph);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
