
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/xutils.h"
#include "../graph_undirected/xgraph_undirected_x.h"
#include "../include/xalgos.h"

static
bool xgraph_unions_impl_unions_free_apply(void *ccset, void *cl) {
    xset_free((XSet_PT*)&ccset);
    return true;
}

static
int xgraph_test_cmp(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static
bool xgraph_equal(void *x, void *y) {
    return strcmp((char*)x, (char*)y) == 0;
}

static
bool xgraph_list_print(void *x, void *cl) {
    printf("%s\n", (char*)x);
    return true;
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static
bool xgraph_to_string_adjset_apply(void *nvertex, void *cl) {
    printf("%s ", (char*)nvertex);
    return true;
}

static
bool xgraph_to_string_vertex_apply(void *vertex, void **adjset, void *cl) {
    printf("%s : ", (char*)vertex);
    xset_map((XSet_PT)(*adjset), xgraph_to_string_adjset_apply, NULL);
    printf("\n");
    return true;
}

static 
void xgraph_to_string(XGraph_PT graph) {
    xmap_map(graph->adjsets, xgraph_to_string_vertex_apply, NULL);
}

XGraph_PT xgraph_test_graph1(void) {
    XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

    /*
    *            a ---------- c
    *            | \      / / | \        i -- l        t
    *            |  \    / /  |  \      /    /         |
    *            |     b  /   |   g -- h -- j       p--q--r
    *            |       /    | /       \    \         |
    *            f ---- d --- e          k -- m        s
    */
    xgraph_add_edge_repeat(graph, "a", "b");
    xgraph_add_edge_repeat(graph, "a", "c");
    xgraph_add_edge_repeat(graph, "a", "f");
    xgraph_add_edge_repeat(graph, "b", "c");
    xgraph_add_edge_repeat(graph, "c", "d");
    xgraph_add_edge_repeat(graph, "c", "e");
    xgraph_add_edge_repeat(graph, "c", "g");
    xgraph_add_edge_repeat(graph, "d", "e");
    xgraph_add_edge_repeat(graph, "d", "f");
    xgraph_add_edge_repeat(graph, "e", "g");
    xgraph_add_edge_repeat(graph, "g", "h");
    xgraph_add_edge_repeat(graph, "h", "i");
    xgraph_add_edge_repeat(graph, "h", "j");
    xgraph_add_edge_repeat(graph, "h", "k");
    xgraph_add_edge_repeat(graph, "i", "l");
    xgraph_add_edge_repeat(graph, "j", "l");
    xgraph_add_edge_repeat(graph, "j", "m");
    xgraph_add_edge_repeat(graph, "k", "m");

    xgraph_add_edge_repeat(graph, "p", "q");
    xgraph_add_edge_repeat(graph, "q", "r");
    xgraph_add_edge_repeat(graph, "q", "s");
    xgraph_add_edge_repeat(graph, "q", "t");

    //xgraph_to_string(graph);

    return graph;
}

XGraph_PT xgraph_test_graph2(void) {
    XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

    /*
    *            0 --------2
    *            |    \     \
    *            | 1--- 7     6
    *            |      |   /
    *            |  3   |  /
    *            | /  \ | /
    *            5------4
    */
    xgraph_add_edge_repeat(graph, "0", "2");
    xgraph_add_edge_repeat(graph, "0", "5");
    xgraph_add_edge_repeat(graph, "0", "7");
    xgraph_add_edge_repeat(graph, "1", "7");
    xgraph_add_edge_repeat(graph, "2", "6");
    xgraph_add_edge_repeat(graph, "3", "4");
    xgraph_add_edge_repeat(graph, "3", "5");
    xgraph_add_edge_repeat(graph, "4", "5");
    xgraph_add_edge_repeat(graph, "4", "6");
    xgraph_add_edge_repeat(graph, "4", "7");

    //xgraph_to_string(graph);

    return graph;
}

XGraph_PT xgraph_two_color_graph(void) {
    XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

    /*
    *            a ---b---c    g-h-i
    *            | \       \    \ /
    *            |  d       ---- j -- k
    *            |   \           |     
    *            f ---e --- l -- m
    */
    xgraph_add_edge_repeat(graph, "a", "b");
    xgraph_add_edge_repeat(graph, "a", "d");
    xgraph_add_edge_repeat(graph, "a", "f");
    xgraph_add_edge_repeat(graph, "b", "c");
    xgraph_add_edge_repeat(graph, "c", "j");
    xgraph_add_edge_repeat(graph, "d", "e");
    xgraph_add_edge_repeat(graph, "e", "f");
    xgraph_add_edge_repeat(graph, "e", "l");
    xgraph_add_edge_repeat(graph, "g", "h");
    xgraph_add_edge_repeat(graph, "g", "j");
    xgraph_add_edge_repeat(graph, "h", "i");
    xgraph_add_edge_repeat(graph, "i", "j");
    xgraph_add_edge_repeat(graph, "j", "k");
    xgraph_add_edge_repeat(graph, "j", "m");
    xgraph_add_edge_repeat(graph, "l", "m");

    //xgraph_to_string(graph);

    return graph;
}

XGraph_PT xgraph_euler_path_graph(void) {
    XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

    /*
    *            0 ---------
    *            | \   \    \
    *            |  1 - 2   6 
    *            |     / \ /
    *            |    3 - 4
    *            |       / 
    *            |------5
    */
    xgraph_add_edge_repeat(graph, "0", "1");
    xgraph_add_edge_repeat(graph, "0", "2");
    xgraph_add_edge_repeat(graph, "0", "5");
    xgraph_add_edge_repeat(graph, "0", "6");
    xgraph_add_edge_repeat(graph, "1", "2");
    xgraph_add_edge_repeat(graph, "2", "3");
    xgraph_add_edge_repeat(graph, "2", "4");
    xgraph_add_edge_repeat(graph, "3", "4");
    xgraph_add_edge_repeat(graph, "4", "5");
    xgraph_add_edge_repeat(graph, "4", "6");

    //xgraph_to_string(graph);

    return graph;
}

void test_xgraph() {
    /* xgraph_new */
    {
        XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

        xassert(graph);
        xassert(graph->cmp);
        xassert_false(graph->cl);
        xassert(graph->adjsets);

        xgraph_free(&graph);
    }

    /* xgraph_free */
    {
        {
            XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);
            xgraph_free(&graph);
        }

        {
            XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);
            xgraph_add_vertex(graph, "abc");
            xgraph_free(&graph);
        }

        {
            XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);
            xgraph_add_edge_repeat(graph, "a", "b");
            xgraph_free(&graph);
        }
    }

    /* xgraph_deep_free */
    {
        {
            XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);
            xgraph_deep_free(&graph);
        }

        {
            char *abc = XMEM_CALLOC(1, 5);
            memcpy(abc, "abc", 3);

            XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);
            xgraph_add_vertex(graph, abc);
            xgraph_deep_free(&graph);
        }

        {
            char *a = XMEM_CALLOC(1, 5);
            char *b = XMEM_CALLOC(1, 5);
            memcpy(a, "a", 1);
            memcpy(b, "b", 1);

            XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);
            xgraph_add_edge_repeat(graph, a, b);
            xgraph_deep_free(&graph);
        }
    }

    /* xgraph_copy */
    {
        XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);
        xassert(xgraph_add_edge_repeat(graph, "a", "a"));
        xassert(xgraph_add_edge_repeat(graph, "a", "a"));
        xassert(xgraph_add_edge_repeat(graph, "a", "b"));
        xassert(xgraph_add_edge_repeat(graph, "a", "b"));
        xassert(xgraph_add_edge_repeat(graph, "d", "f"));
        xassert(xgraph_add_edge_repeat(graph, "d", "g"));
        xassert(xgraph_add_vertex(graph, "h"));
        //xgraph_to_string(graph);
        //printf("\n\n");

        XGraph_PT ngraph = xgraph_copy(graph);
        //xgraph_to_string(ngraph);
        xassert(xgraph_vertex_size(graph) == xgraph_vertex_size(ngraph));
        xassert(xgraph_edge_size(graph) == xgraph_edge_size(ngraph));
        xgraph_free(&graph);
        xgraph_free(&ngraph);
    }

    /* xgraph_add_vertex */
    {
        XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

        xassert(xgraph_add_vertex(graph, "a"));
        xassert(xgraph_add_vertex(graph, "b"));
        xassert(xgraph_add_vertex(graph, "c"));
        xassert(xgraph_add_vertex(graph, "a"));
        xassert(xgraph_add_vertex(graph, "a"));
        xassert(xgraph_add_vertex(graph, "b"));

        xassert(xgraph_vertex_size(graph) == 3);
        xassert(xgraph_is_vertex(graph, "a"));
        xassert(xgraph_is_vertex(graph, "b"));
        xassert(xgraph_is_vertex(graph, "c"));

        xgraph_free(&graph);
    }

    /* xgraph_remove_vertex */
    {
        /* vertex not in graph*/
        {
            XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);
            xgraph_remove_vertex(graph, "a");
            xgraph_remove_vertex(graph, "a");
            xgraph_free(&graph);
        }

        /* only vertex add/remove operations */
        {
            XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);
            xassert(xgraph_add_vertex(graph, "a"));
            xassert(xgraph_add_vertex(graph, "b"));
            xassert(xgraph_add_vertex(graph, "c"));
            xassert(xgraph_add_vertex(graph, "d"));
            xassert(xgraph_add_vertex(graph, "e"));
            xassert(xgraph_add_vertex(graph, "f"));

            xassert(xgraph_vertex_size(graph) == 6);
            xgraph_remove_vertex(graph, "a");
            xgraph_remove_vertex(graph, "a");
            xgraph_remove_vertex(graph, "a");
            xassert(xgraph_vertex_size(graph) == 5);
            xassert(!xgraph_is_vertex(graph, "a"));

            xgraph_remove_vertex(graph, "e");
            xgraph_remove_vertex(graph, "c");
            xassert(xgraph_vertex_size(graph) == 3);
            xassert(!xgraph_is_vertex(graph, "e"));
            xassert(!xgraph_is_vertex(graph, "c"));

            xassert(xgraph_add_vertex(graph, "c"));
            xgraph_remove_vertex(graph, "b");
            xgraph_remove_vertex(graph, "d");
            xgraph_remove_vertex(graph, "f");
            xassert(xgraph_vertex_size(graph) == 1);
            xassert(!xgraph_is_vertex(graph, "b"));
            xassert(!xgraph_is_vertex(graph, "d"));
            xassert(!xgraph_is_vertex(graph, "f"));
            xassert(xgraph_is_vertex(graph, "c"));

            xgraph_free(&graph);
        }

        /* include edge operations too */
        {
            XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);
            xassert(xgraph_add_edge_repeat(graph, "a", "b"));
            xassert(xgraph_add_edge_repeat(graph, "a", "b"));
            xassert(xgraph_add_edge_repeat(graph, "a", "b"));
            xassert(xgraph_add_edge_repeat(graph, "a", "c"));
            xassert(xgraph_add_edge_repeat(graph, "a", "d"));
            xassert(xgraph_vertex_size(graph) == 4);
            xassert(xgraph_edge_size(graph) == 5);

            xgraph_remove_vertex(graph, "b");
            xassert(xgraph_vertex_size(graph) == 3);
            xassert(xgraph_edge_size(graph) == 2);

            xgraph_remove_vertex(graph, "a");
            xassert(xgraph_vertex_size(graph) == 2);
            xassert(xgraph_is_vertex(graph, "c")); 
            xassert(xgraph_is_vertex(graph, "d")); 
            xassert(xgraph_edge_size(graph) == 0);

            xgraph_free(&graph);
        }
    }

    /* xgraph_is_vertex */
    {
        /* vertex operations */
        {
            XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);
            xassert_false(xgraph_is_vertex(graph, "a"));

            xgraph_add_vertex(graph, "a");
            xassert(xgraph_is_vertex(graph, "a"));

            xgraph_remove_vertex(graph, "a");
            xassert_false(xgraph_is_vertex(graph, "a"));

            xgraph_free(&graph);
        }

        /* edge operations */
        {
            XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);
            xassert_false(xgraph_is_vertex(graph, "a"));

            xgraph_add_edge_repeat(graph, "a", "b");
            xassert(xgraph_is_vertex(graph, "a"));

            xgraph_remove_edge(graph, "a", "b");
            xassert(xgraph_is_vertex(graph, "a"));

            xgraph_free(&graph);
        }
    }

    /* xgraph_vertex_size */
    {
        {
            XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);
            xassert(xgraph_vertex_size(graph) == 0);

            xgraph_add_vertex(graph, "a");
            xgraph_add_vertex(graph, "a");
            xgraph_add_vertex(graph, "b");
            xassert(xgraph_vertex_size(graph) == 2);

            xgraph_add_edge_repeat(graph, "a", "c");
            xgraph_add_edge_repeat(graph, "a", "c");
            xgraph_add_edge_repeat(graph, "a", "b");
            xassert(xgraph_vertex_size(graph) == 3);

            xgraph_remove_edge(graph, "a", "b");
            xassert(xgraph_vertex_size(graph) == 3);

            xgraph_remove_vertex(graph, "c");
            xassert(xgraph_vertex_size(graph) == 2);

            xgraph_remove_vertex(graph, "a");
            xassert(xgraph_vertex_size(graph) == 1);

            xgraph_free(&graph);
        }
    }

    /* xgraph_adjset */
    {
        {
            XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);
            xassert_false(xgraph_adjset(graph, "a"));

            xgraph_add_vertex(graph, "a");
            xassert(xgraph_adjset(graph, "a"));

            xgraph_remove_vertex(graph, "a");
            xassert_false(xgraph_adjset(graph, "a"));

            xgraph_free(&graph);
        }
    }

    /* xgraph_is_adjacent */
    {
        XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

        xgraph_add_vertex(graph, "a");
        xgraph_add_vertex(graph, "b");
        xgraph_add_edge_repeat(graph, "a", "c");

        xassert_false(xgraph_is_adjacent(graph, "a", "b"));
        xassert(xgraph_is_adjacent(graph, "a", "c"));

        xgraph_free(&graph);
    }

    /* xgraph_add_edge_unique */
    {
        XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

        /* self cycle */
        xgraph_add_edge_unique(graph, "a", "a");
        xassert(xset_size(xgraph_adjset(graph, "a")) == 1);
        xgraph_add_edge_unique(graph, "a", "a");
        xassert(xset_size(xgraph_adjset(graph, "a")) == 1);

        /* parallel edge */
        xgraph_add_edge_unique(graph, "b", "c");
        xassert(xset_size(xgraph_adjset(graph, "b")) == 1);
        xgraph_add_edge_unique(graph, "b", "c");
        xassert(xset_size(xgraph_adjset(graph, "b")) == 1);

        xgraph_free(&graph);
    }

    /* xgraph_add_edge_repeat */
    {
        XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

        /* self cycle */
        xgraph_add_edge_repeat(graph, "a", "a");
        xassert(xset_size(xgraph_adjset(graph, "a")) == 1);
        xgraph_add_edge_repeat(graph, "a", "a");
        xassert(xset_size(xgraph_adjset(graph, "a")) == 2);
        xgraph_add_edge_repeat(graph, "a", "a");
        xassert(xset_size(xgraph_adjset(graph, "a")) == 3);

        /* parallel edge */
        xgraph_add_edge_repeat(graph, "b", "c");
        xassert(xset_size(xgraph_adjset(graph, "b")) == 1);
        xgraph_add_edge_repeat(graph, "b", "c");
        xassert(xset_size(xgraph_adjset(graph, "b")) == 2);
        xgraph_add_edge_repeat(graph, "b", "c");
        xassert(xset_size(xgraph_adjset(graph, "b")) == 3);

        //xgraph_to_string(graph);

        xgraph_free(&graph);
    }

    /* xgraph_remove_edge */
    {
        XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);
        /* edge not exist */
        xgraph_remove_edge(graph, "a", "b");

        /* self cycle */
        xgraph_add_edge_repeat(graph, "a", "a");
        xgraph_add_edge_repeat(graph, "a", "a");
        xgraph_remove_edge(graph, "a", "a");
        xassert(xset_size(xgraph_adjset(graph, "a")) == 1);

        /* parallel edge */
        xgraph_add_edge_repeat(graph, "b", "c");
        xgraph_add_edge_repeat(graph, "b", "c");
        xgraph_remove_edge(graph, "b", "c");
        xassert(xset_size(xgraph_adjset(graph, "b")) == 1);
        xgraph_remove_edge(graph, "b", "c");
        xassert(xset_size(xgraph_adjset(graph, "b")) == 0);

        xgraph_free(&graph);
    }

    /* xgraph_remove_edge_all */
    {
        {
            XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);
            /* edge not exist */
            xgraph_remove_edge_all(graph, "a", "b");

            /* self cycle */
            xgraph_add_edge_repeat(graph, "a", "a");
            xgraph_add_edge_repeat(graph, "a", "a");
            xgraph_remove_edge_all(graph, "a", "a");
            xassert(xset_size(xgraph_adjset(graph, "a")) == 0);

            /* parallel edge */
            xgraph_add_edge_repeat(graph, "b", "c");
            xgraph_add_edge_repeat(graph, "b", "c");
            xgraph_remove_edge_all(graph, "b", "c");
            xassert(xset_size(xgraph_adjset(graph, "b")) == 0);

            xgraph_free(&graph);
        }
    }

    /* xgraph_is_edge */
    {
        {
            XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

            xgraph_add_edge_repeat(graph, "a", "c");
            xassert(xgraph_is_edge(graph, "a", "c"));
            xassert(xgraph_is_edge(graph, "c", "a"));
            xassert_false(xgraph_is_edge(graph, "a", "a"));

            xgraph_free(&graph);
        }
    }

    /* xgraph_edge_size */
    {
        XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

        xassert(xgraph_edge_size(graph) == 0);

        xgraph_add_vertex(graph, "a");
        xgraph_add_vertex(graph, "b");
        xassert(xgraph_edge_size(graph) == 0);

        xgraph_add_edge_unique(graph, "a", "b");
        xassert(xgraph_edge_size(graph) == 1);

        xgraph_add_edge_unique(graph, "a", "b");
        xassert(xgraph_edge_size(graph) == 1);

        xgraph_add_edge_repeat(graph, "a", "b");
        xassert(xgraph_edge_size(graph) == 2);

        xgraph_add_edge_repeat(graph, "a", "b");
        xassert(xgraph_edge_size(graph) == 3);

        xgraph_add_edge_repeat(graph, "a", "a");
        xassert(xgraph_edge_size(graph) == 4);

        xgraph_add_edge_repeat(graph, "a", "a");
        xassert(xgraph_edge_size(graph) == 5);

        xgraph_free(&graph);
    }

    /* xgraph_has_parallel_edge */
    /* xgraph_parallel_edge_size */
    /* xgraph_remove_parallel_edge */
    /* xgraph_has_parallel_edges */
    /* xgraph_parallel_edges_size */
    /* xgraph_remove_parallel_edges */
    {
        XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

        xgraph_add_edge_repeat(graph, "a", "a");
        xgraph_add_edge_repeat(graph, "a", "a");
        xassert_false(xgraph_has_parallel_edge(graph, "a"));
        xassert(xgraph_parallel_edge_size(graph, "a") == 0);

        xgraph_add_edge_repeat(graph, "a", "b");
        xgraph_add_edge_repeat(graph, "a", "c");
        xgraph_add_edge_repeat(graph, "a", "d");

        xassert_false(xgraph_has_parallel_edges(graph));
        xassert(xgraph_parallel_edges_size(graph) == 0);

        xassert_false(xgraph_has_parallel_edge(graph, "a"));
        xassert(xgraph_parallel_edge_size(graph, "a") == 0);

        for (int i = 1; i <= 50; ++i) {
            xgraph_add_edge_repeat(graph, "a", "e");
            if (i % 10 == 0) {
                xassert(xgraph_parallel_edge_size(graph, "a") == i);
                xassert(xgraph_parallel_edge_size(graph, "e") == i);
                xassert_false(xgraph_has_parallel_edge(graph, "b"));
                xassert(xgraph_parallel_edges_size(graph) == i);
            }
        }

        for (int i = 1; i <= 50; ++i) {
            xgraph_add_edge_repeat(graph, "b", "f");
        }

        xassert(xgraph_parallel_edge_size(graph, "a") == 50);
        xassert(xgraph_parallel_edge_size(graph, "b") == 50);
        xassert(xgraph_parallel_edges_size(graph) == 100);

        xgraph_remove_parallel_edge(graph, "a");
        xassert_false(xgraph_has_parallel_edge(graph, "a"));
        xassert(xgraph_parallel_edge_size(graph, "a") == 0);
        xassert(xgraph_parallel_edge_size(graph, "b") == 50);
        xassert(xgraph_parallel_edges_size(graph) == 50);

        xgraph_remove_parallel_edges(graph);
        xassert_false(xgraph_has_parallel_edges(graph));
        xassert(xgraph_parallel_edges_size(graph) == 0);

        xgraph_free(&graph);
    }

    /* xgraph_has_self_cycle */
    /* xgraph_self_cycle_size */
    /* xgraph_remove_self_cycle */
    /* xgraph_has_self_cycles */
    /* xgraph_self_cycles_size */
    /* xgraph_remove_self_cycles */
    {
        XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

        xassert_false(xgraph_has_self_cycles(graph));
        xassert(xgraph_self_cycles_size(graph) == 0);

        xgraph_add_edge_repeat(graph, "a", "b");
        xgraph_add_edge_repeat(graph, "a", "c");
        xgraph_add_edge_repeat(graph, "a", "b");

        xassert_false(xgraph_has_self_cycles(graph));
        xassert(xgraph_self_cycles_size(graph) == 0);

        xassert_false(xgraph_has_self_cycle(graph, "a"));
        xassert(xgraph_self_cycle_size(graph, "a") == 0);

        for (int i = 1; i <= 50; ++i) {
            xgraph_add_edge_repeat(graph, "a", "a");
            if (i % 10 == 0) {
                xassert(xgraph_self_cycle_size(graph, "a") == i);
                xassert_false(xgraph_has_self_cycle(graph, "b"));
                xassert(xgraph_self_cycles_size(graph) == i);
            }
        }

        for (int i = 1; i <= 50; ++i) {
            xgraph_add_edge_repeat(graph, "b", "b");
        }

        xassert(xgraph_self_cycle_size(graph, "a") == 50);
        xassert(xgraph_self_cycle_size(graph, "b") == 50);
        xassert(xgraph_self_cycles_size(graph) == 100);

        xgraph_remove_self_cycle(graph, "a");
        xassert_false(xgraph_has_self_cycle(graph, "a"));
        xassert(xgraph_self_cycle_size(graph, "a") == 0);
        xassert(xgraph_self_cycle_size(graph, "b") == 50);
        xassert(xgraph_self_cycles_size(graph) == 50);

        xgraph_remove_self_cycles(graph);
        xassert_false(xgraph_has_self_cycles(graph));
        xassert(xgraph_self_cycles_size(graph) == 0);

        xgraph_free(&graph);
    }

    /* xgraph_is_simple */
    {
        XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

        xassert(xgraph_is_simple(graph));

        xgraph_add_edge_repeat(graph, "a", "b");
        xgraph_add_edge_repeat(graph, "a", "c");
        xassert(xgraph_is_simple(graph));

        xgraph_add_edge_repeat(graph, "a", "a");
        xassert_false(xgraph_is_simple(graph));

        xgraph_remove_edge(graph, "a", "a");
        xassert(xgraph_is_simple(graph));

        xgraph_add_edge_repeat(graph, "a", "b");
        xassert_false(xgraph_is_simple(graph));

        xgraph_free(&graph);
    }

    /* xgraph_simple_graph */
    {
        XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

        xgraph_add_edge_repeat(graph, "a", "b");
        xgraph_add_edge_repeat(graph, "a", "c");
        xgraph_add_edge_repeat(graph, "a", "a");
        xgraph_add_edge_repeat(graph, "a", "b");
        xassert_false(xgraph_is_simple(graph));

        {
            xgraph_simple_graph(graph);
            xassert(xgraph_is_simple(graph));
        }

        xgraph_free(&graph);
    }

    /* xgraph_union_dfs */
    /* xgraph_map_union_dfs */
    /* xgraph_multi_unions_dfs */
    /* xgraph_unions_dfs */
    {
        XGraph_PT graph = xgraph_test_graph1();

        {
            XSet_PT ccset0 = xgraph_union_dfs(graph, "f");
            xassert(xset_size(ccset0) == 13);
            //xgraph_to_string_vertex_apply((void*)"f", (void*)&ccset0, NULL);
            xset_free(&ccset0);

            XSet_PT ccset1 = xgraph_union_dfs(graph, "m");
            xassert(xset_size(ccset1) == 13);
            //xgraph_to_string_vertex_apply((void*)"m", (void*)&ccset1, NULL);
            xset_free(&ccset1);

            XSet_PT ccset2 = xgraph_union_dfs(graph, "p");
            xassert(xset_size(ccset2) == 5);
            //xgraph_to_string_vertex_apply((void*)"p", (void*)&ccset2, NULL);
            xset_free(&ccset2);
        }

        {
            XSet_PT set = xset_new(graph->cmp, graph->cl);
            xset_put_repeat(set, "f");
            xset_put_repeat(set, "m");

            XSet_PT ccset3 = xgraph_multi_unions_dfs(graph, set);
            xassert(xset_size(ccset3) == 13);
            //xgraph_to_string_vertex_apply((void*)"f+m", (void*)&ccset3, NULL);

            xset_put_repeat(set, "p");

            XSet_PT ccset4 = xgraph_multi_unions_dfs(graph, set);
            xassert(xset_size(ccset4) == 18);
            //xgraph_to_string_vertex_apply((void*)"f+m+p", (void*)&ccset4, NULL);

            xset_free(&ccset3);
            xset_free(&ccset4);
            xset_free(&set);
        }

        {
            XDList_PT ccset5 = xgraph_unions_dfs(graph);
            xassert(xdlist_size(ccset5) == 2);

            XSet_PT set1 = xdlist_pop_front(ccset5);
            xassert((xset_size(set1) == 5) || (xset_size(set1) == 13));

            XSet_PT set2 = xdlist_pop_front(ccset5);
            xassert(xset_size(set1) + xset_size(set2) == 18);

            xset_free(&set1);
            xset_free(&set2);
            xdlist_free(&ccset5);
        }

        xgraph_free(&graph);
    }

    /* xgraph_union_bfs */
    /* xgraph_map_union_bfs */
    /* xgraph_multi_unions_bfs */
    /* xgraph_unions_bfs */
    {
        XGraph_PT graph = xgraph_test_graph1();

        {
            XSet_PT ccset0 = xgraph_union_bfs(graph, "f");
            xassert(xset_size(ccset0) == 13);
            //xgraph_to_string_vertex_apply((void*)"f", (void*)&ccset0, NULL);
            xset_free(&ccset0);

            XSet_PT ccset1 = xgraph_union_bfs(graph, "m");
            xassert(xset_size(ccset1) == 13);
            //xgraph_to_string_vertex_apply((void*)"m", (void*)&ccset1, NULL);
            xset_free(&ccset1);

            XSet_PT ccset2 = xgraph_union_bfs(graph, "p");
            xassert(xset_size(ccset2) == 5);
            //xgraph_to_string_vertex_apply((void*)"p", (void*)&ccset2, NULL);
            xset_free(&ccset2);
        }

        {
            XSet_PT set = xset_new(graph->cmp, graph->cl);
            xset_put_repeat(set, "f");
            xset_put_repeat(set, "m");

            XSet_PT ccset3 = xgraph_multi_unions_bfs(graph, set);
            xassert(xset_size(ccset3) == 13);
            //xgraph_to_string_vertex_apply((void*)"f+m", (void*)&ccset3, NULL);

            xset_put_repeat(set, "p");

            XSet_PT ccset4 = xgraph_multi_unions_bfs(graph, set);
            xassert(xset_size(ccset4) == 18);
            //xgraph_to_string_vertex_apply((void*)"f+m+p", (void*)&ccset4, NULL);

            xset_free(&ccset3);
            xset_free(&ccset4);
            xset_free(&set);
        }

        {
            XDList_PT ccset5 = xgraph_unions_bfs(graph);
            xassert(xdlist_size(ccset5) == 2);

            XSet_PT set1 = xdlist_pop_back(ccset5);
            xassert((xset_size(set1) == 5) || (xset_size(set1) == 13));

            XSet_PT set2 = xdlist_pop_back(ccset5);
            xassert(xset_size(set1) + xset_size(set2) == 18);

            xset_free(&set1);
            xset_free(&set2);
            xdlist_free(&ccset5);
        }

        xgraph_free(&graph);
    }

    /* xgraph_shortest_path */
    /* xgraph_path_bfs */
    {
        XGraph_PT graph = xgraph_test_graph1();

        xassert(xgraph_shortest_path(graph, "b", "r") == false);

        XDList_PT list = xgraph_shortest_path(graph, "a", "k");
        xassert(xdlist_size(list) == 5);

        xassert(graph->cmp(xdlist_pop_front(list), "a", graph->cl) == 0);
        xassert(graph->cmp(xdlist_pop_front(list), "c", graph->cl) == 0);
        xassert(graph->cmp(xdlist_pop_front(list), "g", graph->cl) == 0);
        xassert(graph->cmp(xdlist_pop_front(list), "h", graph->cl) == 0);
        xassert(graph->cmp(xdlist_pop_front(list), "k", graph->cl) == 0);

        xdlist_free(&list);
        xgraph_free(&graph);
    }

    /* xgraph_shortest_paths */
    /* xgraph_paths_bfs */
    {
        XGraph_PT graph = xgraph_test_graph1();
        XMap_PT paths = xgraph_shortest_paths(graph, "a");

        {
            XDList_PT list = xgraph_path_by_found_paths(graph, paths, "a", "h");
            xassert(xdlist_size(list) == 4);
            xassert(graph->cmp(xdlist_pop_front(list), "a", graph->cl) == 0);
            xassert(graph->cmp(xdlist_pop_front(list), "c", graph->cl) == 0);
            xassert(graph->cmp(xdlist_pop_front(list), "g", graph->cl) == 0);
            xassert(graph->cmp(xdlist_pop_front(list), "h", graph->cl) == 0);
            xdlist_free(&list);
        }

        {
            XDList_PT list = xgraph_path_by_found_paths(graph, paths, "a", "e");
            xassert(xdlist_size(list) == 3);
            xassert(graph->cmp(xdlist_pop_front(list), "a", graph->cl) == 0);
            xassert(graph->cmp(xdlist_pop_front(list), "c", graph->cl) == 0);
            xassert(graph->cmp(xdlist_pop_front(list), "e", graph->cl) == 0);
            xdlist_free(&list);
        }

        xmap_free(&paths);
        xgraph_free(&graph);
    }

    /* xgraph_path_dfs */
    {
        XGraph_PT graph = xgraph_test_graph1();

        {
            XDList_PT list = xgraph_path_dfs(graph, "a", "m");
            xassert(list);
            xdlist_free(&list);
        }

        {
            XDList_PT list = xgraph_path_dfs(graph, "a", "i");
            xassert(list);
            xdlist_free(&list);
        }

        xgraph_free(&graph);
    }

    /* xgraph_paths_dfs */
    {
        XGraph_PT graph = xgraph_test_graph1();
        XMap_PT paths = xgraph_paths_dfs(graph, "a");

        {
            XDList_PT list = xgraph_path_by_found_paths(graph, paths, "a", "m");
            xassert(list);
            xdlist_free(&list);
        }

        {
            XDList_PT list = xgraph_path_by_found_paths(graph, paths, "a", "i");
            xassert(list);
            xdlist_free(&list);
        }

        xmap_free(&paths);
        xgraph_free(&graph);
    }

    /* xgraph_is_connected */
    {
        XGraph_PT graph = xgraph_test_graph1();

        xassert(xgraph_is_connected(graph, "d", "m"));

        xassert_false(xgraph_is_connected(graph, "b", "q"));

        xgraph_free(&graph);
    }

    /* xgraph_has_euler_path */
    /* xgraph_euler_path */

    /* xgraph_cycle_include */
    {
        XGraph_PT graph = xgraph_test_graph1();

        {
            XDList_PT list = xgraph_cycle_include(graph, "i");
            xassert(list);
            xassert(graph->cmp(xdlist_pop_front(list), "i", graph->cl) == 0);
            xassert(graph->cmp(xdlist_pop_back(list), "i", graph->cl) == 0);
            xdlist_free(&list);
        }

        {
            XDList_PT list = xgraph_cycle_include(graph, "j");
            xassert(list);
            xassert(graph->cmp(xdlist_pop_front(list), "j", graph->cl) == 0);
            xassert(graph->cmp(xdlist_pop_back(list), "j", graph->cl) == 0);
            xdlist_free(&list);
        }

        {
            XDList_PT list = xgraph_cycle_include(graph, "m");
            xassert(list);
            xassert(graph->cmp(xdlist_pop_front(list), "m", graph->cl) == 0);
            xassert(graph->cmp(xdlist_pop_back(list), "m", graph->cl) == 0);
            xdlist_free(&list);
        }

        {
            XDList_PT list = xgraph_cycle_include(graph, "d");
            xassert(list);
            xassert(graph->cmp(xdlist_pop_front(list), "d", graph->cl) == 0);
            xassert(graph->cmp(xdlist_pop_back(list), "d", graph->cl) == 0);
            xdlist_free(&list);
        }

        {
            XDList_PT list = xgraph_cycle_include(graph, "q");
            xassert_false(list);
            xdlist_free(&list);
        }

        xgraph_free(&graph);
    }

    /* xgraph_any_cycle */
    {
        {
            XGraph_PT graph = xgraph_test_graph1();

            {
                XDList_PT list = xgraph_any_cycle(graph);
                xassert(list);
                xassert(graph->cmp(xdlist_pop_front(list), xdlist_pop_back(list), graph->cl) == 0);
                xdlist_free(&list);
            }

            xgraph_free(&graph);
        }

        {
            XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);
            xgraph_add_edge_repeat(graph, "a", "b");
            xgraph_add_edge_repeat(graph, "a", "c");
            xgraph_add_edge_repeat(graph, "a", "d");
            xgraph_add_edge_repeat(graph, "b", "e");
            xgraph_add_edge_repeat(graph, "b", "f");

            xassert_false(xgraph_any_cycle(graph));

            xgraph_free(&graph);
        }
    }

    /* xgraph_any_cycle_in_union */
    {
        XGraph_PT graph = xgraph_test_graph1();


        {
            XDList_PT list = xgraph_any_cycle_in_union(graph, "i");
            xassert(list);
            xassert(graph->cmp(xdlist_pop_front(list), xdlist_pop_back(list), graph->cl) == 0);
            xdlist_free(&list);
        }

        {
            XDList_PT list = xgraph_any_cycle_in_union(graph, "j");
            xassert(list);
            xassert(graph->cmp(xdlist_pop_front(list), xdlist_pop_back(list), graph->cl) == 0);
            xdlist_free(&list);
        }

        {
            XDList_PT list = xgraph_any_cycle_in_union(graph, "m");
            xassert(list);
            xassert(graph->cmp(xdlist_pop_front(list), xdlist_pop_back(list), graph->cl) == 0);
            xdlist_free(&list);
        }

        {
            XDList_PT list = xgraph_any_cycle_in_union(graph, "d");
            xassert(list);
            xassert(graph->cmp(xdlist_pop_front(list), xdlist_pop_back(list), graph->cl) == 0);
            xdlist_free(&list);
        }

        {
            XDList_PT list = xgraph_any_cycle_in_union(graph, "q");
            xassert_false(list);
            xdlist_free(&list);
        }

        xgraph_free(&graph);
    }
    
    /* xgraph_has_cycle_include */
    {
        XGraph_PT graph = xgraph_test_graph1();

        xassert(xgraph_has_cycle_include(graph, "a"));
        xassert(xgraph_has_cycle_include(graph, "d"));
        xassert(xgraph_has_cycle_include(graph, "g"));
        xassert(xgraph_has_cycle_include(graph, "i"));
        xassert(xgraph_has_cycle_include(graph, "m"));

        xassert_false(xgraph_has_cycle_include(graph, "t"));

        xgraph_free(&graph);
    }

    /* xgraph_has_any_cycle_in_union */
    {
        XGraph_PT graph = xgraph_test_graph1();

        xassert(xgraph_has_any_cycle_in_union(graph, "a"));
        xassert(xgraph_has_any_cycle_in_union(graph, "d"));
        xassert(xgraph_has_any_cycle_in_union(graph, "g"));
        xassert(xgraph_has_any_cycle_in_union(graph, "i"));
        xassert(xgraph_has_any_cycle_in_union(graph, "m"));

        xassert_false(xgraph_has_any_cycle_in_union(graph, "t"));

        xgraph_free(&graph);
    }

    /* xgraph_has_any_cycle */
    {
        {
            XGraph_PT graph = xgraph_test_graph1();
            xassert(xgraph_has_any_cycle(graph));
            xgraph_free(&graph);
        }

        {
            XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

            xgraph_add_edge_repeat(graph, "a", "b");
            xgraph_add_edge_repeat(graph, "a", "c");
            xgraph_add_edge_repeat(graph, "a", "d");
            xgraph_add_edge_repeat(graph, "b", "e");
            xgraph_add_edge_repeat(graph, "d", "h");
            xassert_false(xgraph_has_any_cycle(graph));

            xgraph_add_edge_repeat(graph, "b", "c");
            xassert(xgraph_has_any_cycle(graph));

            xgraph_free(&graph);
        }
    }

    /* xgraph_two_color_dfs */
    {
        {
            XGraph_PT graph = xgraph_two_color_graph();
            XDList_PT color = xgraph_two_color_dfs(graph);
            xdlist_map(color, xgraph_unions_impl_unions_free_apply, NULL);
            xdlist_free(&color);
            xgraph_free(&graph);
        }

        {
            XGraph_PT graph = xgraph_two_color_graph();
            xgraph_add_edge_repeat(graph, "c", "e");
            xassert_false(xgraph_two_color_dfs(graph));
            xgraph_free(&graph);
        }
    }

    /* xgraph_two_color_bfs */
    {
        {
            XGraph_PT graph = xgraph_two_color_graph();
            XDList_PT color = xgraph_two_color_bfs(graph);
            xdlist_map(color, xgraph_unions_impl_unions_free_apply, NULL);
            xdlist_free(&color);
            xgraph_free(&graph);
        }

        {
            XGraph_PT graph = xgraph_two_color_graph();
            xgraph_add_edge_repeat(graph, "c", "e");
            xassert_false(xgraph_two_color_bfs(graph));
            xgraph_free(&graph);
        }
    }

    /* xgraph_is_two_color */
    {
        {
            XGraph_PT graph = xgraph_two_color_graph();
            xassert(xgraph_is_two_color(graph));
            xgraph_free(&graph);
        }

        {
            XGraph_PT graph = xgraph_two_color_graph();
            xgraph_add_edge_repeat(graph, "c", "e");
            xassert_false(xgraph_is_two_color(graph));
            xgraph_free(&graph);
        }
    }

#if 0
    /* xgraph_has_euler_path */
    {
        {
            XGraph_PT graph = xgraph_euler_path_graph();
            xassert(xgraph_has_euler_path(graph, "0", "0"));
            xgraph_free(&graph);
        }

        {
            XGraph_PT graph = xgraph_euler_path_graph();
            xgraph_remove_edge(graph, "0", "1");
            xassert_false(xgraph_has_euler_path(graph, "0", "0"));
            xgraph_free(&graph);
        }

        {
            XGraph_PT graph = xgraph_euler_path_graph();
            xgraph_remove_edge(graph, "0", "5");
            xassert(xgraph_has_euler_path(graph, "0", "5"));
            {
                XDList_PT list = xgraph_euler_path(graph, "0", "5");
                //xdlist_map(list, xgraph_list_print, NULL);
                xdlist_free(&list);
            }
            xgraph_free(&graph);
        }

        {
            XGraph_PT graph = xgraph_euler_path_graph();
            xgraph_remove_edge(graph, "0", "2");
            xassert(xgraph_has_euler_path(graph, "0", "2"));
            {
                XDList_PT list = xgraph_euler_path(graph, "0", "2");
                //xdlist_map(list, xgraph_list_print, NULL);
                xdlist_free(&list);
            }
            xgraph_free(&graph);
        }
    }

    /* xgraph_euler_path */
    {
        XGraph_PT graph = xgraph_euler_path_graph();
        XDList_PT list = xgraph_euler_path(graph, "0", "0");
        //xdlist_map(list, xgraph_list_print, NULL);
        xdlist_free(&list);
        xgraph_free(&graph);
    }

    /* xgraph_is_complete */
    {
        {
            XGraph_PT graph = xgraph_two_color_graph();
            xassert_false(xgraph_is_complete(graph));
            xgraph_free(&graph);
        }

        {
            XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);
            xgraph_add_edge_repeat(graph, "a", "b");
            xgraph_add_edge_repeat(graph, "a", "c");
            xgraph_add_edge_repeat(graph, "a", "d");
            xgraph_add_edge_repeat(graph, "b", "c");
            xgraph_add_edge_repeat(graph, "b", "d");
            xgraph_add_edge_repeat(graph, "c", "d");
            xassert(xgraph_is_complete(graph));

            /* self cycle */
            xgraph_add_edge_repeat(graph, "a", "a");
            xassert(xgraph_is_complete(graph));

            /* parallel edge */
            xgraph_add_edge_repeat(graph, "a", "b");
            xassert(xgraph_is_complete(graph));

            xgraph_add_edge_repeat(graph, "c", "e");
            xassert_false(xgraph_is_complete(graph));

            xgraph_free(&graph);
        }
    }
#endif

    /* xgraph_is_tree */
    
    /* xgraph_tree_dfs */
    {
        XGraph_PT graph = xgraph_euler_path_graph();
        XMTree_PT tree = xgraph_tree_dfs(graph, "0");
        //xmtree_to_string(tree);
        xmtree_free(&tree);
        xgraph_free(&graph);
    }

    /* xgraph_forest_dfs */
    {
        XGraph_PT graph = xgraph_test_graph1();
        XDList_PT forest = xgraph_forest_dfs(graph);
        xassert(xdlist_size(forest) == 2);
        XMTree_PT tree1 = xdlist_pop_back(forest);
        xassert(tree1);
        XMTree_PT tree2 = xdlist_pop_back(forest);
        xassert(tree2);
        xdlist_free(&forest);
        xmtree_free(&tree1);
        xmtree_free(&tree2);
        xgraph_free(&graph);
    }

    /* xgraph_tree_bfs */
    {
        XGraph_PT graph = xgraph_euler_path_graph();
        XMTree_PT tree = xgraph_tree_bfs(graph, "0");
        //xmtree_to_string(tree);
        xmtree_free(&tree);
        xgraph_free(&graph);
    }

    /* xgraph_forest_bfs */
    {
        XGraph_PT graph = xgraph_test_graph1();
        XDList_PT forest = xgraph_forest_bfs(graph);
        xassert(xdlist_size(forest) == 2);
        XMTree_PT tree1 = xdlist_pop_back(forest);
        xassert(tree1);
        XMTree_PT tree2 = xdlist_pop_back(forest);
        xassert(tree2);
        xdlist_free(&forest);
        xmtree_free(&tree1);
        xmtree_free(&tree2);
        xgraph_free(&graph);
    }

    /* xgraph_degree */
    {
        XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

        xgraph_add_edge_repeat(graph, "a", "b");
        xgraph_add_edge_repeat(graph, "a", "c");
        xgraph_add_edge_repeat(graph, "a", "d");
        xgraph_add_edge_repeat(graph, "a", "b");
        xassert(xgraph_degree(graph, "a") == 4);

        xgraph_remove_vertex(graph, "b");
        xassert(xgraph_degree(graph, "a") == 2);

        xgraph_free(&graph);
    }

    /* xgraph_max_degree */
    {
        XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

        xgraph_add_edge_repeat(graph, "a", "b");
        xgraph_add_edge_repeat(graph, "a", "c");
        xgraph_add_edge_repeat(graph, "a", "d");
        xgraph_add_edge_repeat(graph, "a", "b");
        xassert(xgraph_max_degree(graph) == 4);

        xgraph_remove_vertex(graph, "b");
        xassert(xgraph_max_degree(graph) == 2);

        xgraph_free(&graph);
    }

    /* xgraph_average_degree */
    {
        XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

        xgraph_add_edge_repeat(graph, "a", "b");
        xgraph_add_edge_repeat(graph, "a", "c");
        xgraph_add_edge_repeat(graph, "a", "d");
        xgraph_add_edge_repeat(graph, "a", "b");

        xassert(xgraph_average_degree(graph) == 2 * 4 / 4);

        xgraph_free(&graph);
    }

    /* xgraph_density */
    {
        XGraph_PT graph = xgraph_test_graph1();
        xassert(xgraph_density(graph) == xgraph_average_degree(graph));
        xgraph_free(&graph);
    }

    /* xgraph_is_dense */
    {
        XGraph_PT graph = xgraph_new(xgraph_test_cmp, NULL);

        xgraph_add_edge_repeat(graph, "a", "b");
        xgraph_add_edge_repeat(graph, "a", "c");
        xgraph_add_edge_repeat(graph, "a", "d");
        xgraph_add_edge_repeat(graph, "b", "c");
        xgraph_add_edge_repeat(graph, "b", "d");
        xgraph_add_edge_repeat(graph, "c", "d");
        xgraph_add_edge_repeat(graph, "a", "b");
        xgraph_add_edge_repeat(graph, "a", "c");
        xgraph_add_edge_repeat(graph, "a", "d");
        xgraph_add_edge_repeat(graph, "b", "c");
        xgraph_add_edge_repeat(graph, "b", "d");
        xgraph_add_edge_repeat(graph, "c", "d");
        xgraph_add_edge_repeat(graph, "a", "b");
        xgraph_add_edge_repeat(graph, "a", "c");
        xgraph_add_edge_repeat(graph, "a", "d");
        xgraph_add_edge_repeat(graph, "b", "c");
        xgraph_add_edge_repeat(graph, "b", "d");
        xgraph_add_edge_repeat(graph, "c", "d");

        //xassert(xgraph_is_complete(graph));
        xassert(xgraph_is_dense(graph));

        xgraph_free(&graph);
    }

    /* xgraph_is_sparse */
    {
        XGraph_PT graph = xgraph_test_graph1();
        xassert(xgraph_is_sparse(graph));
        xgraph_free(&graph);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
