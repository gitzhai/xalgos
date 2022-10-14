
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/xutils.h"
#include "../graph_weight_directed/xgraph_weight_directed_x.h"
#include "../include/xalgos.h"

static
int xwdigraph_test_cmp(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static 
bool xwdigraph_print_vertex(void *vertex, void *cl) {
    printf("%s->", (char*)vertex);
    return true;
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static
bool xwdigraph_to_string_adjset_apply(void *edge, void *cl) {
    XWEdge_PT edge1 = edge;
    printf("(%s, %s : %.2f) ", (char*)edge1->v, (char*)edge1->w, edge1->weight);
    return true;
}

static
bool xwdigraph_to_string_vertex_apply(void *vertex, void **adjset, void *cl) {
    printf("%s : ", (char*)vertex);
    xset_map((XSet_PT)(*adjset), xwdigraph_to_string_adjset_apply, NULL);
    printf("\n");
    return true;
}

static
void xwdigraph_to_string(XWDigraph_PT graph) {
    xmap_map(graph->wadjsets, xwdigraph_to_string_vertex_apply, NULL);
}

static
bool xwdigraph_spt_to_string(void *vertex, void **edge, void *cl) {
    XWEdge_PT edge1 = *edge;
    printf("(%s, %s : %.2f)\n", (char*)edge1->v, (char*)edge1->w, edge1->weight);
    return true;
}

XWDigraph_PT xwdigraph_test_graph1(void) {
    XWDigraph_PT graph = xwdigraph_new(xwdigraph_test_cmp, NULL);
    xwdigraph_add_edge_repeat(graph, "4", "5", 0.35);
    xwdigraph_add_edge_repeat(graph, "5", "4", 0.35);
    xwdigraph_add_edge_repeat(graph, "4", "7", 0.37);
    xwdigraph_add_edge_repeat(graph, "5", "7", 0.28);
    xwdigraph_add_edge_repeat(graph, "7", "5", 0.28);
    xwdigraph_add_edge_repeat(graph, "5", "1", 0.32);
    xwdigraph_add_edge_repeat(graph, "0", "4", 0.38);
    xwdigraph_add_edge_repeat(graph, "0", "2", 0.26);
    xwdigraph_add_edge_repeat(graph, "7", "3", 0.39);
    xwdigraph_add_edge_repeat(graph, "1", "3", 0.29);
    xwdigraph_add_edge_repeat(graph, "2", "7", 0.34);
    xwdigraph_add_edge_repeat(graph, "6", "2", 0.40);
    xwdigraph_add_edge_repeat(graph, "3", "6", 0.52);
    xwdigraph_add_edge_repeat(graph, "6", "0", 0.58);
    xwdigraph_add_edge_repeat(graph, "6", "4", 0.93);
    //xwdigraph_to_string(graph);
    return graph;
}

XWDigraph_PT xwdigraph_test_graph2(void) {
    XWDigraph_PT graph = xwdigraph_new(xwdigraph_test_cmp, NULL);
    xwdigraph_add_edge_repeat(graph, "4", "5", 0.35);
    xwdigraph_add_edge_repeat(graph, "5", "4", 0.35);
    xwdigraph_add_edge_repeat(graph, "4", "7", 0.37);
    xwdigraph_add_edge_repeat(graph, "5", "7", 0.28);
    xwdigraph_add_edge_repeat(graph, "7", "5", 0.27);
    xwdigraph_add_edge_repeat(graph, "5", "1", 0.33); /* parallel edeg */
    xwdigraph_add_edge_repeat(graph, "5", "1", 0.32); /* parallel edeg */
    xwdigraph_add_edge_repeat(graph, "5", "1", 0.31); /* parallel edeg */
    xwdigraph_add_edge_repeat(graph, "5", "1", 0.34); /* parallel edeg */
    xwdigraph_add_edge_repeat(graph, "0", "4", 0.38);
    xwdigraph_add_edge_repeat(graph, "0", "2", 0.11);
    xwdigraph_add_edge_repeat(graph, "7", "3", 0.39);
    xwdigraph_add_edge_repeat(graph, "1", "3", 0.29);
    xwdigraph_add_edge_repeat(graph, "2", "7", 0.34);
    xwdigraph_add_edge_repeat(graph, "2", "0", 0.12);  /* (0->2) + (2->0)  <  (0->4) : this may create a cycle 0->2->0 which has shorter dist than 0->4 */
    xwdigraph_add_edge_repeat(graph, "2", "2", 0.01);  /* self-cycle */
    xwdigraph_add_edge_repeat(graph, "2", "2", 0.02);  /* self-cycle */
    xwdigraph_add_edge_repeat(graph, "6", "2", 0.40);
    xwdigraph_add_edge_repeat(graph, "3", "6", 0.52);
    xwdigraph_add_edge_repeat(graph, "6", "0", 0.58);
    xwdigraph_add_edge_repeat(graph, "6", "4", 0.93);
    //xwdigraph_to_string(graph);
    return graph;
}

XWDigraph_PT xwdigraph_test_no_cycle(void) {
    XWDigraph_PT graph = xwdigraph_new(xwdigraph_test_cmp, NULL);
    xwdigraph_add_edge_repeat(graph, "5", "4", 0.35);
    xwdigraph_add_edge_repeat(graph, "4", "7", 0.37);
    xwdigraph_add_edge_repeat(graph, "5", "7", 0.28);
    xwdigraph_add_edge_repeat(graph, "5", "1", 0.33); /* parallel edeg */
    xwdigraph_add_edge_repeat(graph, "5", "1", 0.32); /* parallel edeg */
    xwdigraph_add_edge_repeat(graph, "5", "1", 0.31); /* parallel edeg */
    xwdigraph_add_edge_repeat(graph, "5", "1", 0.34); /* parallel edeg */
    xwdigraph_add_edge_repeat(graph, "4", "0", 0.38);
    xwdigraph_add_edge_repeat(graph, "0", "2", 0.26);
    xwdigraph_add_edge_repeat(graph, "3", "7", 0.39);
    xwdigraph_add_edge_repeat(graph, "1", "3", 0.29);
    xwdigraph_add_edge_repeat(graph, "7", "2", 0.34);
    xwdigraph_add_edge_repeat(graph, "6", "2", 0.40);
    xwdigraph_add_edge_repeat(graph, "3", "6", 0.52);
    xwdigraph_add_edge_repeat(graph, "6", "0", 0.58);
    xwdigraph_add_edge_repeat(graph, "6", "4", 0.93);
    //xwdigraph_to_string(graph);
    return graph;
}

XWDigraph_PT xwdigraph_test_negative_weight_edge(void) {
    XWDigraph_PT graph = xwdigraph_new(xwdigraph_test_cmp, NULL);
    xwdigraph_add_edge_repeat(graph, "4", "5", 0.35);
    xwdigraph_add_edge_repeat(graph, "5", "4", 0.35);
    xwdigraph_add_edge_repeat(graph, "4", "7", 0.37);
    xwdigraph_add_edge_repeat(graph, "5", "7", 0.28);
    xwdigraph_add_edge_repeat(graph, "7", "5", 0.28);
    xwdigraph_add_edge_repeat(graph, "5", "1", 0.32);
    xwdigraph_add_edge_repeat(graph, "0", "4", 0.38);
    xwdigraph_add_edge_repeat(graph, "0", "2", 0.26);
    xwdigraph_add_edge_repeat(graph, "7", "3", 0.39);
    xwdigraph_add_edge_repeat(graph, "1", "3", 0.29);
    xwdigraph_add_edge_repeat(graph, "2", "7", 0.34);
    xwdigraph_add_edge_repeat(graph, "6", "2", -1.20);
    xwdigraph_add_edge_repeat(graph, "3", "6", 0.52);
    xwdigraph_add_edge_repeat(graph, "6", "0", -1.40);
    xwdigraph_add_edge_repeat(graph, "6", "4", -1.25);
    //xwdigraph_to_string(graph);
    return graph;
}

XWDigraph_PT xwdigraph_test_negative_weight_cycle(void) {
    XWDigraph_PT graph = xwdigraph_new(xwdigraph_test_cmp, NULL);
    xwdigraph_add_edge_repeat(graph, "4", "5", 0.35);
    xwdigraph_add_edge_repeat(graph, "5", "4", -0.66);
    xwdigraph_add_edge_repeat(graph, "4", "7", 0.37);
    xwdigraph_add_edge_repeat(graph, "5", "7", 0.28);
    xwdigraph_add_edge_repeat(graph, "7", "5", 0.28);
    xwdigraph_add_edge_repeat(graph, "5", "1", 0.32);
    xwdigraph_add_edge_repeat(graph, "0", "4", 0.38);
    xwdigraph_add_edge_repeat(graph, "0", "2", 0.26);
    xwdigraph_add_edge_repeat(graph, "7", "3", 0.39);
    xwdigraph_add_edge_repeat(graph, "1", "3", 0.29);
    xwdigraph_add_edge_repeat(graph, "2", "7", 0.34);
    xwdigraph_add_edge_repeat(graph, "6", "2", 0.40);
    xwdigraph_add_edge_repeat(graph, "3", "6", 0.52);
    xwdigraph_add_edge_repeat(graph, "6", "0", 0.58);
    xwdigraph_add_edge_repeat(graph, "6", "4", 0.93);
    //xwdigraph_to_string(graph);
    return graph;
}

void test_xwdigraph() {
    /* xwdigraph_spt_dijkstra */
    {
        {
            XWDigraph_PT graph = xwdigraph_test_graph1();
            XMap_PT tree = xwdigraph_spt_dijkstra(graph, "0");
            xassert(0 < xmap_size(tree));
            //xmap_map(tree, xwdigraph_spt_to_string, NULL);
            xmap_free(&tree);
            xwdigraph_free(&graph);
        }

        {
            XWDigraph_PT graph = xwdigraph_test_graph2();
            XMap_PT tree = xwdigraph_spt_dijkstra(graph, "0");
            xassert(0 < xmap_size(tree));
            //xmap_map(tree, xwdigraph_spt_to_string, NULL);
            xmap_free(&tree);
            xwdigraph_free(&graph);
        }
    }

    /* xwdigraph_spt_no_cycle */
    {
        XWDigraph_PT graph = xwdigraph_test_no_cycle();
        XMap_PT tree = xwdigraph_spt_no_cycle(graph, "5");
        xassert(0 < xmap_size(tree));
        //xmap_map(tree, xwdigraph_spt_to_string, NULL);
        xmap_free(&tree);
        xwdigraph_free(&graph);
    }

    /* xwdigraph_shortest_path_no_cycle */
    {
        {
            XWDigraph_PT graph = xwdigraph_test_no_cycle();
            XDList_PT path = xwdigraph_shortest_path_no_cycle(graph, "5", "6");
            xassert(xdlist_size(path) == 3);

            XWEdge_PT edge = xdlist_pop_front(path);
            xassert(strcmp(edge->v, "5") == 0);
            xassert(strcmp(edge->w, "1") == 0);

            edge = xdlist_pop_front(path);
            xassert(strcmp(edge->v, "1") == 0);
            xassert(strcmp(edge->w, "3") == 0);

            edge = xdlist_pop_front(path);
            xassert(strcmp(edge->v, "3") == 0);
            xassert(strcmp(edge->w, "6") == 0);

            xdlist_free(&path);
            xwdigraph_free(&graph);
        }

        {
            XWDigraph_PT graph = xwdigraph_test_no_cycle();
            XDList_PT path = xwdigraph_shortest_path_no_cycle(graph, "7", "2");
            xassert(xdlist_size(path) == 1);

            XWEdge_PT edge = xdlist_pop_front(path);
            xassert(strcmp(edge->v, "7") == 0);
            xassert(strcmp(edge->w, "2") == 0);

            xdlist_free(&path);
            xwdigraph_free(&graph);
        }
    }

    /* xwdigraph_spt_bellman_ford */
    /* xwdigraph_negative_cycle */
    {
        {
            XWDigraph_PT graph = xwdigraph_test_negative_weight_edge();
            XMap_PT tree = xwdigraph_spt_bellman_ford(graph, "0");
            xassert(0 < xmap_size(tree));
            //xmap_map(tree, xwdigraph_spt_to_string, NULL);
            xmap_free(&tree);
            xwdigraph_free(&graph);
        }

        /* negative weight cycle */
        {
            XWDigraph_PT graph = xwdigraph_test_negative_weight_cycle();
            XMap_PT tree = xwdigraph_spt_bellman_ford_negative_cycle(graph, "0");
            xassert(0 < xmap_size(tree));
            //xmap_map(tree, xwdigraph_spt_to_string, NULL);

            XDList_PT cycle = xwdigraph_negative_cycle(graph, tree);
            //xdlist_map(cycle, xwdigraph_print_vertex, NULL);

            xdlist_free(&cycle);
            xmap_free(&tree);
            xwdigraph_free(&graph);
        }
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
