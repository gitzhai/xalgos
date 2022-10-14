
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/xutils.h"
#include "../graph_directed/xgraph_directed_x.h"
#include "../include/xalgos.h"

static
bool xdigraph_unions_impl_unions_free_apply(void *ccset, void *cl) {
    xset_free((XSet_PT*)&ccset);
    return true;
}

static
int xdigraph_test_cmp(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static
bool xdigraph_equal(void *x, void *y) {
    return strcmp((char*)x, (char*)y) == 0;
}

static
bool xdigraph_list_print(void *x, void *cl) {
    printf("%s\n", (char*)x);
    return true;
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static
bool xdigraph_to_string_adjset_apply(void *nvertex, void *cl) {
    printf("%s ", (char*)nvertex);
    return true;
}

static
bool xdigraph_to_string_vertex_apply(void *vertex, void **adjset, void *cl) {
    printf("%s : ", (char*)vertex);
    xset_map((XSet_PT)(*adjset), xdigraph_to_string_adjset_apply, NULL);
    printf("\n");
    return true;
}

static
void xdigraph_to_string(XDigraph_PT graph) {
    xmap_map(graph->adjsets, xdigraph_to_string_vertex_apply, NULL);
}

XDigraph_PT xdigraph_test_graph1(void) {
    XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

    /*
    *            a ---------- c
    *            | \      / / | \        i -- l        t
    *            |  \    / /  |  \      /    /         |
    *            |     b  /   |   g -- h -- j       p--q--r
    *            |       /    | /       \    \         |
    *            f ---- d --- e          k -- m        s
    */
    xdigraph_add_edge_repeat(graph, "a", "b");
    xdigraph_add_edge_repeat(graph, "a", "c");
    xdigraph_add_edge_repeat(graph, "a", "f");
    xdigraph_add_edge_repeat(graph, "b", "c");
    xdigraph_add_edge_repeat(graph, "c", "d");
    xdigraph_add_edge_repeat(graph, "c", "e");
    xdigraph_add_edge_repeat(graph, "c", "g");
    xdigraph_add_edge_repeat(graph, "d", "e");
    xdigraph_add_edge_repeat(graph, "d", "f");
    xdigraph_add_edge_repeat(graph, "e", "g");
    xdigraph_add_edge_repeat(graph, "g", "h");
    xdigraph_add_edge_repeat(graph, "h", "i");
    xdigraph_add_edge_repeat(graph, "h", "j");
    xdigraph_add_edge_repeat(graph, "h", "k");
    xdigraph_add_edge_repeat(graph, "i", "l");
    xdigraph_add_edge_repeat(graph, "j", "l");
    xdigraph_add_edge_repeat(graph, "j", "m");
    xdigraph_add_edge_repeat(graph, "k", "m");

    xdigraph_add_edge_repeat(graph, "p", "q");
    xdigraph_add_edge_repeat(graph, "q", "r");
    xdigraph_add_edge_repeat(graph, "q", "s");
    xdigraph_add_edge_repeat(graph, "q", "t");

    //xdigraph_to_string(graph);

    return graph;
}

XDigraph_PT xdigraph_test_graph2(void) {
    XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

    /*
    *            0 --------2
    *            |    \     \
    *            | 1--- 7     6
    *            |      |   /
    *            |  3   |  /
    *            | /  \ | /
    *            5------4
    */
    xdigraph_add_edge_repeat(graph, "0", "2");
    xdigraph_add_edge_repeat(graph, "0", "5");
    xdigraph_add_edge_repeat(graph, "0", "7");
    xdigraph_add_edge_repeat(graph, "1", "7");
    xdigraph_add_edge_repeat(graph, "2", "6");
    xdigraph_add_edge_repeat(graph, "3", "4");
    xdigraph_add_edge_repeat(graph, "3", "5");
    xdigraph_add_edge_repeat(graph, "4", "5");
    xdigraph_add_edge_repeat(graph, "4", "6");
    xdigraph_add_edge_repeat(graph, "4", "7");

    //xdigraph_to_string(graph);

    return graph;
}

XDigraph_PT xdigraph_two_color_graph(void) {
    XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

    /*
    *            a ---b---c    g-h-i
    *            | \       \    \ /
    *            |  d       ---- j -- k
    *            |   \           |
    *            f ---e --- l -- m
    */
    xdigraph_add_edge_repeat(graph, "a", "b");
    xdigraph_add_edge_repeat(graph, "a", "d");
    xdigraph_add_edge_repeat(graph, "a", "f");
    xdigraph_add_edge_repeat(graph, "b", "c");
    xdigraph_add_edge_repeat(graph, "c", "j");
    xdigraph_add_edge_repeat(graph, "d", "e");
    xdigraph_add_edge_repeat(graph, "e", "f");
    xdigraph_add_edge_repeat(graph, "e", "l");
    xdigraph_add_edge_repeat(graph, "g", "h");
    xdigraph_add_edge_repeat(graph, "g", "j");
    xdigraph_add_edge_repeat(graph, "h", "i");
    xdigraph_add_edge_repeat(graph, "i", "j");
    xdigraph_add_edge_repeat(graph, "j", "k");
    xdigraph_add_edge_repeat(graph, "j", "m");
    xdigraph_add_edge_repeat(graph, "l", "m");

    //xdigraph_to_string(graph);

    return graph;
}

XDigraph_PT xdigraph_euler_path_graph(void) {
    XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

    /*
    *            0 ---------
    *            | \   \    \
    *            |  1 - 2   6
    *            |     / \ /
    *            |    3 - 4
    *            |       /
    *            |------5
    */
    xdigraph_add_edge_repeat(graph, "0", "1");
    xdigraph_add_edge_repeat(graph, "0", "2");
    xdigraph_add_edge_repeat(graph, "0", "5");
    xdigraph_add_edge_repeat(graph, "0", "6");
    xdigraph_add_edge_repeat(graph, "1", "2");
    xdigraph_add_edge_repeat(graph, "2", "3");
    xdigraph_add_edge_repeat(graph, "2", "4");
    xdigraph_add_edge_repeat(graph, "3", "4");
    xdigraph_add_edge_repeat(graph, "4", "5");
    xdigraph_add_edge_repeat(graph, "4", "6");

    //xdigraph_to_string(graph);

    return graph;
}

void test_xdigraph() {
    /* xdigraph_new */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        xassert(graph);
        xassert(graph->cmp);
        xassert_false(graph->cl);
        xassert(graph->adjsets);

        xdigraph_free(&graph);
    }

    /* xdigraph_free */
    {
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
            xdigraph_free(&graph);
        }

        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
            xdigraph_add_vertex(graph, "abc");
            xdigraph_free(&graph);
        }

        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
            xdigraph_add_edge_repeat(graph, "a", "b");
            xdigraph_free(&graph);
        }
    }

    /* xdigraph_deep_free */
    {
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
            xdigraph_deep_free(&graph);
        }

        {
            char *abc = XMEM_CALLOC(1, 5);
            memcpy(abc, "abc", 3);

            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
            xdigraph_add_vertex(graph, abc);
            xdigraph_deep_free(&graph);
        }

        {
            char *a = XMEM_CALLOC(1, 5);
            char *b = XMEM_CALLOC(1, 5);
            memcpy(a, "a", 1);
            memcpy(b, "b", 1);

            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
            xdigraph_add_edge_repeat(graph, a, b);
            xdigraph_deep_free(&graph);
        }
    }

    /* xdigraph_copy */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
        xassert(xdigraph_add_edge_repeat(graph, "a", "a"));
        xassert(xdigraph_add_edge_repeat(graph, "a", "a"));
        xassert(xdigraph_add_edge_repeat(graph, "a", "b"));
        xassert(xdigraph_add_edge_repeat(graph, "a", "b"));
        xassert(xdigraph_add_edge_repeat(graph, "d", "f"));
        xassert(xdigraph_add_edge_repeat(graph, "d", "g"));
        xassert(xdigraph_add_vertex(graph, "h"));
        //xdigraph_to_string(graph);
        //printf("\n\n");

        XDigraph_PT ngraph = xdigraph_copy(graph);
        //xdigraph_to_string(ngraph);
        xassert(xdigraph_is_edge(ngraph, "d", "f"));
        xassert(xdigraph_is_edge(ngraph, "d", "g"));
        xassert_false(xdigraph_is_edge(ngraph, "f", "d"));
        xassert_false(xdigraph_is_edge(ngraph, "g", "d"));

        xassert(xdigraph_vertex_size(graph) == xdigraph_vertex_size(ngraph));
        xassert(xdigraph_edge_size(graph) == xdigraph_edge_size(ngraph));
        xdigraph_free(&graph);
        xdigraph_free(&ngraph);
    }

    /* xdigraph_reverse */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
        xassert(xdigraph_add_edge_repeat(graph, "a", "b"));
        xassert(xdigraph_add_edge_repeat(graph, "a", "c"));
        xassert(xdigraph_add_edge_repeat(graph, "b", "c"));
        xassert(xdigraph_add_edge_repeat(graph, "d", "f"));
        xassert(xdigraph_add_edge_repeat(graph, "d", "g"));

        xassert(xdigraph_add_vertex(graph, "h"));

        xassert(xdigraph_vertex_size(graph) == 7);
        xassert(xdigraph_edge_size(graph) == 5);
        //xdigraph_to_string(graph);
        //printf("\n\n");

        XDigraph_PT ngraph = xdigraph_reverse(graph);
        //xdigraph_to_string(ngraph);

        xassert(xdigraph_is_edge(ngraph, "b", "a"));
        xassert(xdigraph_is_edge(ngraph, "c", "a"));
        xassert(xdigraph_is_edge(ngraph, "c", "b"));
        xassert(xdigraph_is_edge(ngraph, "f", "d"));
        xassert(xdigraph_is_edge(ngraph, "g", "d"));
        xassert_false(xdigraph_is_edge(ngraph, "d", "g"));
        xassert(xdigraph_is_vertex(ngraph, "h"));

        xassert(xdigraph_vertex_size(ngraph) == 7);
        xassert(xdigraph_edge_size(ngraph) == 5);

        xdigraph_free(&graph);
        xdigraph_free(&ngraph);
    }

    /* xdigraph_to_graph */
    {
        /* a->b->c->d */
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
        xassert(xdigraph_add_edge_repeat(graph, "a", "b"));
        xassert(xdigraph_add_edge_repeat(graph, "b", "c"));
        xassert(xdigraph_add_edge_repeat(graph, "c", "d"));

        XGraph_PT ngraph = xdigraph_to_graph(graph);
        xassert(xdigraph_is_edge(ngraph, "b", "a"));
        xassert(xdigraph_is_edge(ngraph, "c", "b"));
        xassert(xdigraph_is_edge(ngraph, "d", "c"));
        xassert(xdigraph_edge_size(ngraph) == 6);
        xgraph_free(&ngraph);

        xdigraph_free(&graph);
    }

    /* xdigraph_add_vertex */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        xassert(xdigraph_add_vertex(graph, "a"));
        xassert(xdigraph_add_vertex(graph, "b"));
        xassert(xdigraph_add_vertex(graph, "c"));
        xassert(xdigraph_add_vertex(graph, "a"));
        xassert(xdigraph_add_vertex(graph, "a"));
        xassert(xdigraph_add_vertex(graph, "b"));

        xassert(xdigraph_vertex_size(graph) == 3);
        xassert(xdigraph_is_vertex(graph, "a"));
        xassert(xdigraph_is_vertex(graph, "b"));
        xassert(xdigraph_is_vertex(graph, "c"));

        xdigraph_free(&graph);
    }

    /* xdigraph_remove_vertex */
    {
        /* vertex not in graph*/
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
            xdigraph_remove_vertex(graph, "a");
            xdigraph_remove_vertex(graph, "a");
            xdigraph_free(&graph);
        }

        /* only vertex add/remove operations */
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
            xassert(xdigraph_add_vertex(graph, "a"));
            xassert(xdigraph_add_vertex(graph, "b"));
            xassert(xdigraph_add_vertex(graph, "c"));
            xassert(xdigraph_add_vertex(graph, "d"));
            xassert(xdigraph_add_vertex(graph, "e"));
            xassert(xdigraph_add_vertex(graph, "f"));

            xassert(xdigraph_vertex_size(graph) == 6);
            xdigraph_remove_vertex(graph, "a");
            xdigraph_remove_vertex(graph, "a");
            xdigraph_remove_vertex(graph, "a");
            xassert(xdigraph_vertex_size(graph) == 5);
            xassert(!xdigraph_is_vertex(graph, "a"));

            xdigraph_remove_vertex(graph, "e");
            xdigraph_remove_vertex(graph, "c");
            xassert(xdigraph_vertex_size(graph) == 3);
            xassert(!xdigraph_is_vertex(graph, "e"));
            xassert(!xdigraph_is_vertex(graph, "c"));

            xassert(xdigraph_add_vertex(graph, "c"));
            xdigraph_remove_vertex(graph, "b");
            xdigraph_remove_vertex(graph, "d");
            xdigraph_remove_vertex(graph, "f");
            xassert(xdigraph_vertex_size(graph) == 1);
            xassert(!xdigraph_is_vertex(graph, "b"));
            xassert(!xdigraph_is_vertex(graph, "d"));
            xassert(!xdigraph_is_vertex(graph, "f"));
            xassert(xdigraph_is_vertex(graph, "c"));

            xdigraph_free(&graph);
        }

        /* include edge operations too */
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
            xassert(xdigraph_add_edge_repeat(graph, "a", "b"));
            xassert(xdigraph_add_edge_repeat(graph, "a", "b"));
            xassert(xdigraph_add_edge_repeat(graph, "a", "b"));
            xassert(xdigraph_add_edge_repeat(graph, "a", "c"));
            xassert(xdigraph_add_edge_repeat(graph, "a", "d"));
            xassert(xdigraph_vertex_size(graph) == 4);
            xassert(xdigraph_edge_size(graph) == 5);

            xdigraph_remove_vertex(graph, "b");
            xassert(xdigraph_vertex_size(graph) == 3);
            xassert(xdigraph_edge_size(graph) == 2);

            xdigraph_remove_vertex(graph, "a");
            xassert(xdigraph_vertex_size(graph) == 2);
            xassert(xdigraph_is_vertex(graph, "c"));
            xassert(xdigraph_is_vertex(graph, "d"));
            xassert(xdigraph_edge_size(graph) == 0);

            xdigraph_free(&graph);
        }
    }

    /* xdigraph_is_vertex */
    {
        /* vertex operations */
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
            xassert_false(xdigraph_is_vertex(graph, "a"));

            xdigraph_add_vertex(graph, "a");
            xassert(xdigraph_is_vertex(graph, "a"));

            xdigraph_remove_vertex(graph, "a");
            xassert_false(xdigraph_is_vertex(graph, "a"));

            xdigraph_free(&graph);
        }

        /* edge operations */
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
            xassert_false(xdigraph_is_vertex(graph, "a"));

            xdigraph_add_edge_repeat(graph, "a", "b");
            xassert(xdigraph_is_vertex(graph, "a"));

            xdigraph_remove_edge(graph, "a", "b");
            xassert(xdigraph_is_vertex(graph, "a"));

            xdigraph_free(&graph);
        }
    }

    /* xdigraph_vertex_size */
    {
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
            xassert(xdigraph_vertex_size(graph) == 0);

            xdigraph_add_vertex(graph, "a");
            xdigraph_add_vertex(graph, "a");
            xdigraph_add_vertex(graph, "b");
            xassert(xdigraph_vertex_size(graph) == 2);

            xdigraph_add_edge_repeat(graph, "a", "c");
            xdigraph_add_edge_repeat(graph, "a", "c");
            xdigraph_add_edge_repeat(graph, "a", "b");
            xassert(xdigraph_vertex_size(graph) == 3);

            xdigraph_remove_edge(graph, "a", "b");
            xassert(xdigraph_vertex_size(graph) == 3);

            xdigraph_remove_vertex(graph, "c");
            xassert(xdigraph_vertex_size(graph) == 2);

            xdigraph_remove_vertex(graph, "a");
            xassert(xdigraph_vertex_size(graph) == 1);

            xdigraph_free(&graph);
        }
    }

    /* xdigraph_adjset */
    {
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
            xassert_false(xdigraph_adjset(graph, "a"));

            xdigraph_add_vertex(graph, "a");
            xassert(xdigraph_adjset(graph, "a"));

            xdigraph_remove_vertex(graph, "a");
            xassert_false(xdigraph_adjset(graph, "a"));

            xdigraph_free(&graph);
        }
    }

    /* xdigraph_is_adjacent */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        xdigraph_add_vertex(graph, "a");
        xdigraph_add_vertex(graph, "b");
        xdigraph_add_edge_repeat(graph, "a", "c");

        xassert_false(xdigraph_is_adjacent(graph, "a", "b"));
        xassert(xdigraph_is_adjacent(graph, "a", "c"));

        xdigraph_free(&graph);
    }

    /* xdigraph_add_edge_unique */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        /* self cycle */
        xdigraph_add_edge_unique(graph, "a", "a");
        xassert(xset_size(xdigraph_adjset(graph, "a")) == 1);
        xdigraph_add_edge_unique(graph, "a", "a");
        xassert(xset_size(xdigraph_adjset(graph, "a")) == 1);

        /* parallel edge */
        xdigraph_add_edge_unique(graph, "b", "c");
        xassert(xset_size(xdigraph_adjset(graph, "b")) == 1);
        xdigraph_add_edge_unique(graph, "b", "c");
        xassert(xset_size(xdigraph_adjset(graph, "b")) == 1);
        xassert_false(xdigraph_is_edge(graph, "c", "b"));

        xdigraph_free(&graph);
    }

    /* xdigraph_add_edge_repeat */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        /* self cycle */
        xdigraph_add_edge_repeat(graph, "a", "a");
        xassert(xset_size(xdigraph_adjset(graph, "a")) == 1);
        xdigraph_add_edge_repeat(graph, "a", "a");
        xassert(xset_size(xdigraph_adjset(graph, "a")) == 2);
        xdigraph_add_edge_repeat(graph, "a", "a");
        xassert(xset_size(xdigraph_adjset(graph, "a")) == 3);

        /* parallel edge */
        xdigraph_add_edge_repeat(graph, "b", "c");
        xassert(xset_size(xdigraph_adjset(graph, "b")) == 1);
        xdigraph_add_edge_repeat(graph, "b", "c");
        xassert(xset_size(xdigraph_adjset(graph, "b")) == 2);
        xdigraph_add_edge_repeat(graph, "b", "c");
        xassert(xset_size(xdigraph_adjset(graph, "b")) == 3);
        xassert_false(xdigraph_is_edge(graph, "c", "b"));

        //xdigraph_to_string(graph);

        xdigraph_free(&graph);
    }

    /* xdigraph_remove_edge */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
        /* edge not exist */
        xdigraph_remove_edge(graph, "a", "b");

        /* self cycle */
        xdigraph_add_edge_repeat(graph, "a", "a");
        xdigraph_add_edge_repeat(graph, "a", "a");
        xdigraph_remove_edge(graph, "a", "a");
        xassert(xset_size(xdigraph_adjset(graph, "a")) == 1);

        /* parallel edge */
        xdigraph_add_edge_repeat(graph, "b", "c");
        xdigraph_add_edge_repeat(graph, "b", "c");
        xdigraph_remove_edge(graph, "b", "c");
        xassert(xset_size(xdigraph_adjset(graph, "b")) == 1);
        xdigraph_remove_edge(graph, "b", "c");
        xassert(xset_size(xdigraph_adjset(graph, "b")) == 0);

        xdigraph_free(&graph);
    }

    /* xdigraph_remove_edge_all */
    {
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
            /* edge not exist */
            xdigraph_remove_edge_all(graph, "a", "b");

            /* self cycle */
            xdigraph_add_edge_repeat(graph, "a", "a");
            xdigraph_add_edge_repeat(graph, "a", "a");
            xdigraph_remove_edge_all(graph, "a", "a");
            xassert(xset_size(xdigraph_adjset(graph, "a")) == 0);

            /* parallel edge */
            xdigraph_add_edge_repeat(graph, "b", "c");
            xdigraph_add_edge_repeat(graph, "b", "c");
            xdigraph_remove_edge_all(graph, "b", "c");
            xassert(xset_size(xdigraph_adjset(graph, "b")) == 0);

            xdigraph_free(&graph);
        }
    }

    /* xdigraph_is_edge */
    {
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

            xdigraph_add_edge_repeat(graph, "a", "c");
            xassert(xdigraph_is_edge(graph, "a", "c"));
            xassert_false(xdigraph_is_edge(graph, "c", "a"));
            xassert_false(xdigraph_is_edge(graph, "a", "a"));

            xdigraph_free(&graph);
        }
    }

    /* xdigraph_edge_size */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        xassert(xdigraph_edge_size(graph) == 0);

        xdigraph_add_vertex(graph, "a");
        xdigraph_add_vertex(graph, "b");
        xassert(xdigraph_edge_size(graph) == 0);

        xdigraph_add_edge_unique(graph, "a", "b");
        xassert(xdigraph_edge_size(graph) == 1);

        xdigraph_add_edge_unique(graph, "a", "b");
        xassert(xdigraph_edge_size(graph) == 1);

        xdigraph_add_edge_repeat(graph, "a", "b");
        xassert(xdigraph_edge_size(graph) == 2);

        xdigraph_add_edge_repeat(graph, "a", "b");
        xassert(xdigraph_edge_size(graph) == 3);

        xdigraph_add_edge_repeat(graph, "a", "a");
        xassert(xdigraph_edge_size(graph) == 4);

        xdigraph_add_edge_repeat(graph, "a", "a");
        xassert(xdigraph_edge_size(graph) == 5);

        xdigraph_free(&graph);
    }

    /* xdigraph_has_parallel_edge */
    /* xdigraph_parallel_edge_size */
    /* xdigraph_remove_parallel_edge */
    /* xdigraph_has_parallel_edges */
    /* xdigraph_parallel_edges_size */
    /* xdigraph_remove_parallel_edges */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        xdigraph_add_edge_repeat(graph, "a", "a");
        xdigraph_add_edge_repeat(graph, "a", "a");
        xassert_false(xdigraph_has_parallel_edge(graph, "a"));
        xassert(xdigraph_parallel_edge_size(graph, "a") == 0);

        xdigraph_add_edge_repeat(graph, "a", "b");
        xdigraph_add_edge_repeat(graph, "a", "c");
        xdigraph_add_edge_repeat(graph, "a", "d");

        xassert_false(xdigraph_has_parallel_edges(graph));
        xassert(xdigraph_parallel_edges_size(graph) == 0);

        xassert_false(xdigraph_has_parallel_edge(graph, "a"));
        xassert(xdigraph_parallel_edge_size(graph, "a") == 0);

        for (int i = 1; i <= 50; ++i) {
            xdigraph_add_edge_repeat(graph, "a", "e");
            if (i % 10 == 0) {
                xassert(xdigraph_parallel_edge_size(graph, "a") == i);
                xassert(xdigraph_parallel_edge_size(graph, "e") == 0);
                xassert_false(xdigraph_has_parallel_edge(graph, "b"));
                xassert(xdigraph_parallel_edges_size(graph) == i);
            }
        }

        for (int i = 1; i <= 50; ++i) {
            xdigraph_add_edge_repeat(graph, "b", "f");
        }

        xassert(xdigraph_parallel_edge_size(graph, "a") == 50);
        xassert(xdigraph_parallel_edge_size(graph, "b") == 50);
        xassert(xdigraph_parallel_edges_size(graph) == 100);

        xdigraph_remove_parallel_edge(graph, "a");
        xassert_false(xdigraph_has_parallel_edge(graph, "a"));
        xassert(xdigraph_parallel_edge_size(graph, "a") == 0);
        xassert(xdigraph_parallel_edge_size(graph, "b") == 50);
        xassert(xdigraph_parallel_edges_size(graph) == 50);

        xdigraph_remove_parallel_edges(graph);
        xassert_false(xdigraph_has_parallel_edges(graph));
        xassert(xdigraph_parallel_edges_size(graph) == 0);

        xdigraph_free(&graph);
    }

    /* xdigraph_has_self_cycle */
    /* xdigraph_self_cycle_size */
    /* xdigraph_remove_self_cycle */
    /* xdigraph_has_self_cycles */
    /* xdigraph_self_cycles_size */
    /* xdigraph_remove_self_cycles */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        xassert_false(xdigraph_has_self_cycles(graph));
        xassert(xdigraph_self_cycles_size(graph) == 0);

        xdigraph_add_edge_repeat(graph, "a", "b");
        xdigraph_add_edge_repeat(graph, "a", "c");
        xdigraph_add_edge_repeat(graph, "a", "b");

        xassert_false(xdigraph_has_self_cycles(graph));
        xassert(xdigraph_self_cycles_size(graph) == 0);

        xassert_false(xdigraph_has_self_cycle(graph, "a"));
        xassert(xdigraph_self_cycle_size(graph, "a") == 0);

        for (int i = 1; i <= 50; ++i) {
            xdigraph_add_edge_repeat(graph, "a", "a");
            if (i % 10 == 0) {
                xassert(xdigraph_self_cycle_size(graph, "a") == i);
                xassert_false(xdigraph_has_self_cycle(graph, "b"));
                xassert(xdigraph_self_cycles_size(graph) == i);
            }
        }

        for (int i = 1; i <= 50; ++i) {
            xdigraph_add_edge_repeat(graph, "b", "b");
        }

        xassert(xdigraph_self_cycle_size(graph, "a") == 50);
        xassert(xdigraph_self_cycle_size(graph, "b") == 50);
        xassert(xdigraph_self_cycles_size(graph) == 100);

        xdigraph_remove_self_cycle(graph, "a");
        xassert_false(xdigraph_has_self_cycle(graph, "a"));
        xassert(xdigraph_self_cycle_size(graph, "a") == 0);
        xassert(xdigraph_self_cycle_size(graph, "b") == 50);
        xassert(xdigraph_self_cycles_size(graph) == 50);

        xdigraph_remove_self_cycles(graph);
        xassert_false(xdigraph_has_self_cycles(graph));
        xassert(xdigraph_self_cycles_size(graph) == 0);

        xdigraph_free(&graph);
    }

    /* xdigraph_is_simple */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        xassert(xdigraph_is_simple(graph));

        xdigraph_add_edge_repeat(graph, "a", "b");
        xdigraph_add_edge_repeat(graph, "a", "c");
        xassert(xdigraph_is_simple(graph));

        xdigraph_add_edge_repeat(graph, "a", "a");
        xassert_false(xdigraph_is_simple(graph));

        xdigraph_remove_edge(graph, "a", "a");
        xassert(xdigraph_is_simple(graph));

        xdigraph_add_edge_repeat(graph, "a", "b");
        xassert_false(xdigraph_is_simple(graph));

        xdigraph_free(&graph);
    }

    /* xdigraph_simple_graph */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        xdigraph_add_edge_repeat(graph, "a", "b");
        xdigraph_add_edge_repeat(graph, "a", "c");
        xdigraph_add_edge_repeat(graph, "a", "a");
        xdigraph_add_edge_repeat(graph, "a", "b");
        xassert_false(xdigraph_is_simple(graph));

        {
            xdigraph_simple_graph(graph);            
            xassert(xdigraph_is_simple(graph));
        }

        xdigraph_free(&graph);
    }

    /* xdigraph_shortest_path */
    /* xdigraph_path_bfs */
    {
        XDigraph_PT graph = xdigraph_test_graph1();

        xassert(xdigraph_shortest_path(graph, "b", "r") == false);

        XDList_PT list = xdigraph_shortest_path(graph, "a", "k");
        xassert(xdlist_size(list) == 5);

        xassert(graph->cmp(xdlist_pop_front(list), "a", graph->cl) == 0);
        xassert(graph->cmp(xdlist_pop_front(list), "c", graph->cl) == 0);
        xassert(graph->cmp(xdlist_pop_front(list), "g", graph->cl) == 0);
        xassert(graph->cmp(xdlist_pop_front(list), "h", graph->cl) == 0);
        xassert(graph->cmp(xdlist_pop_front(list), "k", graph->cl) == 0);

        xdlist_free(&list);
        xdigraph_free(&graph);
    }

    /* xdigraph_shortest_paths */
    /* xdigraph_paths_bfs */
    {
        XDigraph_PT graph = xdigraph_test_graph1();
        XMap_PT paths = xdigraph_shortest_paths(graph, "a");

        {
            XDList_PT list = xdigraph_path_by_found_paths(graph, paths, "a", "h");
            xassert(xdlist_size(list) == 4);
            xassert(graph->cmp(xdlist_pop_front(list), "a", graph->cl) == 0);
            xassert(graph->cmp(xdlist_pop_front(list), "c", graph->cl) == 0);
            xassert(graph->cmp(xdlist_pop_front(list), "g", graph->cl) == 0);
            xassert(graph->cmp(xdlist_pop_front(list), "h", graph->cl) == 0);
            xdlist_free(&list);
        }

        {
            XDList_PT list = xdigraph_path_by_found_paths(graph, paths, "a", "e");
            xassert(xdlist_size(list) == 3);
            xassert(graph->cmp(xdlist_pop_front(list), "a", graph->cl) == 0);
            xassert(graph->cmp(xdlist_pop_front(list), "c", graph->cl) == 0);
            xassert(graph->cmp(xdlist_pop_front(list), "e", graph->cl) == 0);
            xdlist_free(&list);
        }

        xmap_free(&paths);
        xdigraph_free(&graph);
    }

    /* xdigraph_path_dfs */
    {
        XDigraph_PT graph = xdigraph_test_graph1();

        {
            XDList_PT list = xdigraph_path_dfs(graph, "a", "m");
            xassert(list);
            xdlist_free(&list);
        }

        {
            XDList_PT list = xdigraph_path_dfs(graph, "a", "i");
            xassert(list);
            xdlist_free(&list);
        }

        xdigraph_free(&graph);
    }

    /* xdigraph_paths_dfs */
    {
        XDigraph_PT graph = xdigraph_test_graph1();
        XMap_PT paths = xdigraph_paths_dfs(graph, "a");

        {
            XDList_PT list = xdigraph_path_by_found_paths(graph, paths, "a", "m");
            xassert(list);
            xdlist_free(&list);
        }

        {
            XDList_PT list = xdigraph_path_by_found_paths(graph, paths, "a", "i");
            xassert(list);
            xdlist_free(&list);
        }

        xmap_free(&paths);
        xdigraph_free(&graph);
    }

    /* xdigraph_is_reachable */
    {
        XDigraph_PT graph = xdigraph_test_graph1();

        xassert(xdigraph_is_reachable(graph, "d", "m"));

        xassert_false(xdigraph_is_reachable(graph, "b", "q"));

        xdigraph_free(&graph);
    }

    /* xdigraph_cycle_include */
    /* xdigraph_has_cycle_include */
    {

        /* no cycle */
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

            xdigraph_add_edge_repeat(graph, "a", "b");
            xdigraph_add_edge_repeat(graph, "a", "c");
            xdigraph_add_edge_repeat(graph, "b", "c");
            xassert_false(xdigraph_cycle_include(graph, "a"));
            xassert_false(xdigraph_has_cycle_include(graph, "a"));

            xdigraph_free(&graph);
        }

        /* cycle : a->b->a */
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
            xdigraph_add_edge_repeat(graph, "a", "a");
            xassert_false(xdigraph_cycle_include(graph, "a"));
            xassert_false(xdigraph_has_cycle_include(graph, "a"));

            xdigraph_add_edge_repeat(graph, "a", "b");
            xdigraph_add_edge_repeat(graph, "b", "a");
            XDList_PT list = xdigraph_cycle_include(graph, "a");
            xassert(xdigraph_has_cycle_include(graph, "a"));
            xassert(list);
            xassert(strcmp(xdlist_pop_front(list), "a") == 0);
            xassert(strcmp(xdlist_pop_front(list), "b") == 0);
            xassert(strcmp(xdlist_pop_front(list), "a") == 0);
            xdlist_free(&list);

            xdigraph_free(&graph);
        }

        /* cycle : a->b->c->a */
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

            xdigraph_add_edge_repeat(graph, "a", "b");
            xdigraph_add_edge_repeat(graph, "b", "c");
            xdigraph_add_edge_repeat(graph, "c", "a");

            XDList_PT list = xdigraph_cycle_include(graph, "a");
            xassert(xdigraph_has_cycle_include(graph, "a"));
            xassert(list);
            xassert(strcmp(xdlist_pop_front(list), "a") == 0);
            xassert(strcmp(xdlist_pop_front(list), "b") == 0);
            xassert(strcmp(xdlist_pop_front(list), "c") == 0);
            xassert(strcmp(xdlist_pop_front(list), "a") == 0);
            xdlist_free(&list);

            xdigraph_free(&graph);
        }

        /* cycle : a->b->c->b
        *                 \
        *                  ->d->a
        */
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

            xdigraph_add_edge_repeat(graph, "a", "b");
            xdigraph_add_edge_repeat(graph, "b", "c");
            xdigraph_add_edge_repeat(graph, "c", "b");
            xdigraph_add_edge_repeat(graph, "c", "d");
            xdigraph_add_edge_repeat(graph, "d", "a");

            XDList_PT list = xdigraph_cycle_include(graph, "a");
            xassert(xdigraph_has_cycle_include(graph, "a"));
            xassert(list);
            xassert(strcmp(xdlist_pop_front(list), "a") == 0);
            xassert(strcmp(xdlist_pop_front(list), "b") == 0);
            xassert(strcmp(xdlist_pop_front(list), "c") == 0);
            xassert(strcmp(xdlist_pop_front(list), "d") == 0);
            xassert(strcmp(xdlist_pop_front(list), "a") == 0);
            xdlist_free(&list);

            xdigraph_free(&graph);
        }

        /* a->b->c->b,  a is not in cycle, but b is */
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

            xdigraph_add_edge_repeat(graph, "a", "b");
            xdigraph_add_edge_repeat(graph, "b", "c");
            xdigraph_add_edge_repeat(graph, "c", "b");

            xassert_false(xdigraph_cycle_include(graph, "a"));
            xassert_false(xdigraph_has_cycle_include(graph, "a"));

            XDList_PT list = xdigraph_cycle_include(graph, "b");
            xassert(xdigraph_has_cycle_include(graph, "b"));
            xassert(list);
            xassert(strcmp(xdlist_pop_front(list), "b") == 0);
            xassert(strcmp(xdlist_pop_front(list), "c") == 0);
            xassert(strcmp(xdlist_pop_front(list), "b") == 0);
            xdlist_free(&list);

            xdigraph_free(&graph);
        }
    }

    /* xdigraph_any_cycle */
    /* xdigraph_has_any_cycle */
    {
        /* no cycle */
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

            xdigraph_add_edge_repeat(graph, "a", "b");
            xdigraph_add_edge_repeat(graph, "a", "c");
            xdigraph_add_edge_repeat(graph, "b", "c");
            xassert_false(xdigraph_any_cycle(graph));
            xassert_false(xdigraph_has_any_cycle(graph));

            xdigraph_free(&graph);
        }

        /* cycle : a->b->a */
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
            xdigraph_add_edge_repeat(graph, "a", "a");
            xassert_false(xdigraph_any_cycle(graph));
            xassert_false(xdigraph_has_any_cycle(graph));

            xdigraph_add_edge_repeat(graph, "a", "b");
            xdigraph_add_edge_repeat(graph, "b", "a");
            XDList_PT list = xdigraph_any_cycle(graph);
            xassert(xdigraph_has_any_cycle(graph));
            xassert(list);
            xassert(strcmp(xdlist_pop_front(list), "a") == 0);
            xassert(strcmp(xdlist_pop_front(list), "b") == 0);
            xassert(strcmp(xdlist_pop_front(list), "a") == 0);
            xdlist_free(&list);

            xdigraph_free(&graph);
        }

        /* cycle : a->b->c->a */
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

            xdigraph_add_edge_repeat(graph, "a", "b");
            xdigraph_add_edge_repeat(graph, "b", "c");
            xdigraph_add_edge_repeat(graph, "c", "a");

            XDList_PT list = xdigraph_any_cycle(graph);
            xassert(xdigraph_has_any_cycle(graph));
            xassert(list);
            xassert(strcmp(xdlist_pop_front(list), "a") == 0);
            xassert(strcmp(xdlist_pop_front(list), "b") == 0);
            xassert(strcmp(xdlist_pop_front(list), "c") == 0);
            xassert(strcmp(xdlist_pop_front(list), "a") == 0);
            xdlist_free(&list);

            xdigraph_free(&graph);
        }

        /* cycle : a->b->c->b
        *                 \
        *                  ->d->a
        */
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

            xdigraph_add_edge_repeat(graph, "a", "b");
            xdigraph_add_edge_repeat(graph, "b", "c");
            xdigraph_add_edge_repeat(graph, "c", "b");
            xdigraph_add_edge_repeat(graph, "c", "d");
            xdigraph_add_edge_repeat(graph, "d", "a");

            XDList_PT list = xdigraph_any_cycle(graph);
            xassert(xdigraph_has_any_cycle(graph));
            xassert(list);
            xassert(strcmp(xdlist_pop_front(list), "b") == 0);
            xassert(strcmp(xdlist_pop_front(list), "c") == 0);
            xassert(strcmp(xdlist_pop_front(list), "b") == 0);
            xdlist_free(&list);

            xdigraph_free(&graph);
        }
    }

    /* xdigraph_any_cycle_check_from */
    /* xdigraph_has_any_cycle_check_from */
    {
        /* no cycle */
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

            xdigraph_add_edge_repeat(graph, "a", "b");
            xdigraph_add_edge_repeat(graph, "a", "c");
            xdigraph_add_edge_repeat(graph, "b", "c");
            xassert_false(xdigraph_any_cycle_check_from(graph, "a"));
            xassert_false(xdigraph_has_any_cycle_check_from(graph, "a"));

            xdigraph_free(&graph);
        }

        /* cycle : a->b->a */
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);
            xdigraph_add_edge_repeat(graph, "a", "a");
            xassert_false(xdigraph_any_cycle_check_from(graph, "a"));
            xassert_false(xdigraph_has_any_cycle_check_from(graph, "a"));

            xdigraph_add_edge_repeat(graph, "a", "b");
            xdigraph_add_edge_repeat(graph, "b", "a");
            XDList_PT list = xdigraph_any_cycle_check_from(graph, "a");
            xassert(xdigraph_has_any_cycle_check_from(graph, "a"));
            xassert(list);
            xassert(strcmp(xdlist_pop_front(list), "a") == 0);
            xassert(strcmp(xdlist_pop_front(list), "b") == 0);
            xassert(strcmp(xdlist_pop_front(list), "a") == 0);
            xdlist_free(&list);

            xdigraph_free(&graph);
        }

        /* cycle : a->b->c->a */
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

            xdigraph_add_edge_repeat(graph, "a", "b");
            xdigraph_add_edge_repeat(graph, "b", "c");
            xdigraph_add_edge_repeat(graph, "c", "a");

            XDList_PT list = xdigraph_any_cycle_check_from(graph, "a");
            xassert(xdigraph_has_any_cycle_check_from(graph, "a"));
            xassert(list);
            xassert(strcmp(xdlist_pop_front(list), "a") == 0);
            xassert(strcmp(xdlist_pop_front(list), "b") == 0);
            xassert(strcmp(xdlist_pop_front(list), "c") == 0);
            xassert(strcmp(xdlist_pop_front(list), "a") == 0);
            xdlist_free(&list);

            xdigraph_free(&graph);
        }

        /* cycle : a->b->c->b
        *                 \
        *                  ->d->a
        */
        {
            XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

            xdigraph_add_edge_repeat(graph, "a", "b");
            xdigraph_add_edge_repeat(graph, "b", "c");
            xdigraph_add_edge_repeat(graph, "c", "b");
            xdigraph_add_edge_repeat(graph, "c", "d");
            xdigraph_add_edge_repeat(graph, "d", "a");

            XDList_PT list = xdigraph_any_cycle_check_from(graph, "a");
            xassert(xdigraph_has_any_cycle_check_from(graph, "a"));
            xassert(list);
            xassert(strcmp(xdlist_pop_front(list), "b") == 0);
            xassert(strcmp(xdlist_pop_front(list), "c") == 0);
            xassert(strcmp(xdlist_pop_front(list), "b") == 0);
            xdlist_free(&list);

            xdigraph_free(&graph);
        }
    }

    /* xdigraph_two_color_dfs */
    {
        {
            XDigraph_PT graph = xdigraph_two_color_graph();
            XDList_PT color = xdigraph_two_color_dfs(graph);
            xdlist_map(color, xdigraph_unions_impl_unions_free_apply, NULL);
            xdlist_free(&color);
            xdigraph_free(&graph);
        }

        {
            XDigraph_PT graph = xdigraph_two_color_graph();
            xdigraph_add_edge_repeat(graph, "c", "e");
            xassert_false(xdigraph_two_color_dfs(graph));
            xdigraph_free(&graph);
        }
    }

    /* xdigraph_two_color_bfs */
    {
        {
            XDigraph_PT graph = xdigraph_two_color_graph();
            XDList_PT color = xdigraph_two_color_bfs(graph);
            xdlist_map(color, xdigraph_unions_impl_unions_free_apply, NULL);
            xdlist_free(&color);
            xdigraph_free(&graph);
        }

        {
            XDigraph_PT graph = xdigraph_two_color_graph();
            xdigraph_add_edge_repeat(graph, "c", "e");
            xassert_false(xdigraph_two_color_bfs(graph));
            xdigraph_free(&graph);
        }
    }

    /* xdigraph_is_two_color */
    {
        {
            XDigraph_PT graph = xdigraph_two_color_graph();
            xassert(xdigraph_is_two_color(graph));
            xdigraph_free(&graph);
        }

        {
            XDigraph_PT graph = xdigraph_two_color_graph();
            xdigraph_add_edge_repeat(graph, "c", "e");
            xassert_false(xdigraph_is_two_color(graph));
            xdigraph_free(&graph);
        }
    }

    /* xdigraph_reachable_from_dfs */
    /* xdigraph_all_reachable_from_dfs */
    {
        /* a->b->c->d 
        *     e->c
        */
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        xdigraph_add_edge_repeat(graph, "a", "b");
        xdigraph_add_edge_repeat(graph, "b", "c");
        xdigraph_add_edge_repeat(graph, "c", "d");
        xdigraph_add_edge_repeat(graph, "e", "c");

        {
            XSet_PT set = xdigraph_reachable_from_dfs(graph, "a");
            xassert(xset_find(set, "a"));
            xassert(xset_find(set, "b"));
            xassert(xset_find(set, "c"));
            xassert(xset_find(set, "d"));
            xassert(xset_size(set) == 4);
            xset_free(&set);
        }

        {
            XSet_PT set = xdigraph_reachable_from_dfs(graph, "b");
            xassert(xset_find(set, "b"));
            xassert(xset_find(set, "c"));
            xassert(xset_find(set, "d"));
            xassert(xset_size(set) == 3);
            xset_free(&set);
        }

        {
            XSet_PT set = xdigraph_reachable_from_dfs(graph, "c");
            xassert(xset_find(set, "c"));
            xassert(xset_find(set, "d"));
            xassert(xset_size(set) == 2);
            xset_free(&set);
        }

        {
            XSet_PT set = xdigraph_reachable_from_dfs(graph, "d");
            xassert(xset_find(set, "d"));
            xassert(xset_size(set) == 1);
            xset_free(&set);
        }

        {
            XSet_PT set1 = xset_new(graph->cmp, graph->cl);
            xset_put_repeat(set1, "b");
            xset_put_repeat(set1, "e");

            XSet_PT set2 = xdigraph_all_reachable_from_dfs(graph, set1);

            xassert(xset_find(set2, "b"));
            xassert(xset_find(set2, "c"));
            xassert(xset_find(set2, "d"));
            xassert(xset_find(set2, "e"));
            xassert(xset_size(set2) == 4);
            xset_free(&set2);
            xset_free(&set1);
        }

        xdigraph_free(&graph);
    }

    /* xdigraph_pre_order_dfs */
    /* xdigraph_map_pre_order */
    /* xdigraph_post_order_dfs */
    /* xdigraph_map_post_order */
    /* xdigraph_reverse_post_order_dfs */
    /* xdigraph_map_reverse_post_order */
    {
        /* a -> b -> c -> d
        */
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        xdigraph_add_edge_repeat(graph, "a", "b");
        xdigraph_add_edge_repeat(graph, "b", "c");
        xdigraph_add_edge_repeat(graph, "c", "d");

        XDList_PT list1 = xdigraph_pre_order_dfs(graph);
        xassert(strcmp("a", xdlist_pop_front(list1)) == 0);
        xassert(strcmp("b", xdlist_pop_front(list1)) == 0);
        xassert(strcmp("c", xdlist_pop_front(list1)) == 0);
        xassert(strcmp("d", xdlist_pop_front(list1)) == 0);
        xdlist_free(&list1);

        XDList_PT list2 = xdigraph_post_order_dfs(graph);
        xassert(strcmp("d", xdlist_pop_front(list2)) == 0);
        xassert(strcmp("c", xdlist_pop_front(list2)) == 0);
        xassert(strcmp("b", xdlist_pop_front(list2)) == 0);
        xassert(strcmp("a", xdlist_pop_front(list2)) == 0);
        xdlist_free(&list2);

        XDList_PT list3 = xdigraph_reverse_post_order_dfs(graph);
        xassert(strcmp("a", xdlist_pop_front(list3)) == 0);
        xassert(strcmp("b", xdlist_pop_front(list3)) == 0);
        xassert(strcmp("c", xdlist_pop_front(list3)) == 0);
        xassert(strcmp("d", xdlist_pop_front(list3)) == 0);
        xdlist_free(&list3);

        xdigraph_free(&graph);
    }

    /* xdigraph_topologial_order_dfs */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        xdigraph_add_edge_repeat(graph, "0", "1");
        xdigraph_add_edge_repeat(graph, "0", "5");
        xdigraph_add_edge_repeat(graph, "0", "6");
        xdigraph_add_edge_repeat(graph, "2", "0");
        xdigraph_add_edge_repeat(graph, "2", "3");
        xdigraph_add_edge_repeat(graph, "3", "5");
        xdigraph_add_edge_repeat(graph, "5", "4");
        xdigraph_add_edge_repeat(graph, "6", "4");
        xdigraph_add_edge_repeat(graph, "6", "9");
        xdigraph_add_edge_repeat(graph, "7", "6");
        xdigraph_add_edge_repeat(graph, "8", "7");
        xdigraph_add_edge_repeat(graph, "9", "10");
        xdigraph_add_edge_repeat(graph, "9", "11");
        xdigraph_add_edge_repeat(graph, "9", "12");
        xdigraph_add_edge_repeat(graph, "11", "12");

        XDList_PT list = xdigraph_topologial_order_dfs(graph);
        xassert(strcmp("8", xdlist_pop_front(list)) == 0);
        xassert(strcmp("7", xdlist_pop_front(list)) == 0);
        xassert(strcmp("2", xdlist_pop_front(list)) == 0);
        xassert(strcmp("3", xdlist_pop_front(list)) == 0);
        xassert(strcmp("0", xdlist_pop_front(list)) == 0);
        xassert(strcmp("6", xdlist_pop_front(list)) == 0);
        xassert(strcmp("9", xdlist_pop_front(list)) == 0);
        xassert(strcmp("11", xdlist_pop_front(list)) == 0);
        xassert(strcmp("12", xdlist_pop_front(list)) == 0);
        xassert(strcmp("10", xdlist_pop_front(list)) == 0);
        xassert(strcmp("5", xdlist_pop_front(list)) == 0);
        xassert(strcmp("4", xdlist_pop_front(list)) == 0);
        xassert(strcmp("1", xdlist_pop_front(list)) == 0);
        xdlist_free(&list);

        xdigraph_free(&graph);
    }

    /* xdigraph_transitive_closure */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        xdigraph_add_edge_repeat(graph, "0", "1");
        xdigraph_add_edge_repeat(graph, "0", "5");
        xdigraph_add_edge_repeat(graph, "0", "6");
        xdigraph_add_edge_repeat(graph, "2", "0");
        xdigraph_add_edge_repeat(graph, "2", "3");
        xdigraph_add_edge_repeat(graph, "3", "5");
        xdigraph_add_edge_repeat(graph, "5", "4");
        xdigraph_add_edge_repeat(graph, "6", "4");

        XDigraph_PT ngraph = xdigraph_transitive_closure(graph);
        xassert(xdigraph_is_edge(ngraph, "0", "1"));
        xassert(xdigraph_is_edge(ngraph, "0", "5"));
        xassert(xdigraph_is_edge(ngraph, "0", "6"));
        xassert(xdigraph_is_edge(ngraph, "0", "4"));
        xassert(xdigraph_is_edge(ngraph, "2", "0"));
        xassert(xdigraph_is_edge(ngraph, "2", "3"));
        xassert(xdigraph_is_edge(ngraph, "2", "1"));
        xassert(xdigraph_is_edge(ngraph, "2", "5"));
        xassert(xdigraph_is_edge(ngraph, "2", "4"));
        xassert(xdigraph_is_edge(ngraph, "2", "6"));
        xassert(xdigraph_is_edge(ngraph, "3", "5"));
        xassert(xdigraph_is_edge(ngraph, "3", "4"));
        xassert(xdigraph_is_edge(ngraph, "5", "4"));
        xassert(xdigraph_is_edge(ngraph, "6", "4"));

        xdigraph_free(&ngraph);
        xdigraph_free(&graph);
    }

    /* xdigraph_strongly_connected_dfs */
    /* xdigraph_all_strongly_connected_dfs */
    /* xdigraph_is_strongly_connected_dfs */
    /* xdigraph_is_connected_dfs */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        xdigraph_add_edge_repeat(graph, "0", "1");
        xdigraph_add_edge_repeat(graph, "0", "5");
        xdigraph_add_edge_repeat(graph, "2", "0");
        xdigraph_add_edge_repeat(graph, "2", "3");
        xdigraph_add_edge_repeat(graph, "3", "2");
        xdigraph_add_edge_repeat(graph, "3", "5");
        xdigraph_add_edge_repeat(graph, "4", "2");
        xdigraph_add_edge_repeat(graph, "4", "3");
        xdigraph_add_edge_repeat(graph, "5", "4");
        xdigraph_add_edge_repeat(graph, "6", "0");
        xdigraph_add_edge_repeat(graph, "6", "4");
        xdigraph_add_edge_repeat(graph, "6", "9");
        xdigraph_add_edge_repeat(graph, "7", "6");
        xdigraph_add_edge_repeat(graph, "7", "8");
        xdigraph_add_edge_repeat(graph, "8", "7");
        xdigraph_add_edge_repeat(graph, "8", "9");
        xdigraph_add_edge_repeat(graph, "9", "10");
        xdigraph_add_edge_repeat(graph, "9", "11");
        xdigraph_add_edge_repeat(graph, "10", "12");
        xdigraph_add_edge_repeat(graph, "11", "4");
        xdigraph_add_edge_repeat(graph, "11", "12");
        xdigraph_add_edge_repeat(graph, "12", "9");

        {
            XSet_PT set = xdigraph_strongly_connected_dfs(graph, "1");
            xassert(xset_find(set, "1"));
            xassert(xset_size(set) == 1);
            xset_free(&set);
        }

        {
            XSet_PT set = xdigraph_strongly_connected_dfs(graph, "0");
            xassert(xset_find(set, "0"));
            xassert(xset_find(set, "2"));
            xassert(xset_find(set, "3"));
            xassert(xset_find(set, "4"));
            xassert(xset_find(set, "5"));
            xassert(xset_size(set) == 5);
            xset_free(&set);
        }

        {
            XSet_PT set = xdigraph_strongly_connected_dfs(graph, "9");
            xassert(xset_find(set, "9"));
            xassert(xset_find(set, "10"));
            xassert(xset_find(set, "11"));
            xassert(xset_find(set, "12"));
            xassert(xset_size(set) == 4);
            xset_free(&set);
        }
        {
            XSet_PT set = xdigraph_strongly_connected_dfs(graph, "6");
            xassert(xset_find(set, "6"));
            xassert(xset_size(set) == 1);
            xset_free(&set);
        }
        {
            XSet_PT set = xdigraph_strongly_connected_dfs(graph, "7");
            xassert(xset_find(set, "7"));
            xassert(xset_find(set, "8"));
            xassert(xset_size(set) == 2);
            xset_free(&set);
        }

        xassert_false(xdigraph_is_strongly_connected_dfs(graph));
        xassert(xdigraph_is_connected_dfs(graph));

        {
            XDList_PT list = xdigraph_all_strongly_connected_dfs(graph);
            xassert(xdlist_size(list) == 5);

            {
                XSet_PT set = xdlist_pop_front(list);
                xassert(xset_find(set, "1"));
                xassert(xset_size(set) == 1);
                xset_free(&set);
            }

            {
                XSet_PT set = xdlist_pop_front(list);
                xassert(xset_find(set, "0"));
                xassert(xset_find(set, "2"));
                xassert(xset_find(set, "3"));
                xassert(xset_find(set, "4"));
                xassert(xset_find(set, "5"));
                xassert(xset_size(set) == 5);
                xset_free(&set);
            }

            {
                XSet_PT set = xdlist_pop_front(list);
                xassert(xset_find(set, "9"));
                xassert(xset_find(set, "10"));
                xassert(xset_find(set, "11"));
                xassert(xset_find(set, "12"));
                xassert(xset_size(set) == 4);
                xset_free(&set);
            }

            {
                XSet_PT set = xdlist_pop_front(list);
                xassert(xset_find(set, "6"));
                xassert(xset_size(set) == 1);
                xset_free(&set);
            }

            {
                XSet_PT set = xdlist_pop_front(list);
                xassert(xset_find(set, "7"));
                xassert(xset_find(set, "8"));
                xassert(xset_size(set) == 2);
                xset_free(&set);
            }

            xdlist_free(&list);
        }

        xdigraph_free(&graph);
    }

#if 0

    /* xdigraph_degree */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        xdigraph_add_edge_repeat(graph, "a", "b");
        xdigraph_add_edge_repeat(graph, "a", "c");
        xdigraph_add_edge_repeat(graph, "a", "d");
        xdigraph_add_edge_repeat(graph, "a", "b");
        xassert(xdigraph_degree(graph, "a") == 4);

        xdigraph_remove_vertex(graph, "b");
        xassert(xdigraph_degree(graph, "a") == 2);

        xdigraph_free(&graph);
    }

    /* xdigraph_max_degree */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        xdigraph_add_edge_repeat(graph, "a", "b");
        xdigraph_add_edge_repeat(graph, "a", "c");
        xdigraph_add_edge_repeat(graph, "a", "d");
        xdigraph_add_edge_repeat(graph, "a", "b");
        xassert(xdigraph_max_degree(graph) == 4);

        xdigraph_remove_vertex(graph, "b");
        xassert(xdigraph_max_degree(graph) == 2);

        xdigraph_free(&graph);
    }

    /* xdigraph_average_degree */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        xdigraph_add_edge_repeat(graph, "a", "b");
        xdigraph_add_edge_repeat(graph, "a", "c");
        xdigraph_add_edge_repeat(graph, "a", "d");
        xdigraph_add_edge_repeat(graph, "a", "b");

        xassert(xdigraph_average_degree(graph) == 2 * 4 / 4);

        xdigraph_free(&graph);
    }

    /* xdigraph_density */
    {
        XDigraph_PT graph = xdigraph_test_graph1();
        xassert(xdigraph_density(graph) == xdigraph_average_degree(graph));
        xdigraph_free(&graph);
    }

    /* xdigraph_is_dense */
    {
        XDigraph_PT graph = xdigraph_new(xdigraph_test_cmp, NULL);

        xdigraph_add_edge_repeat(graph, "a", "b");
        xdigraph_add_edge_repeat(graph, "a", "c");
        xdigraph_add_edge_repeat(graph, "a", "d");
        xdigraph_add_edge_repeat(graph, "b", "c");
        xdigraph_add_edge_repeat(graph, "b", "d");
        xdigraph_add_edge_repeat(graph, "c", "d");
        xdigraph_add_edge_repeat(graph, "a", "b");
        xdigraph_add_edge_repeat(graph, "a", "c");
        xdigraph_add_edge_repeat(graph, "a", "d");
        xdigraph_add_edge_repeat(graph, "b", "c");
        xdigraph_add_edge_repeat(graph, "b", "d");
        xdigraph_add_edge_repeat(graph, "c", "d");
        xdigraph_add_edge_repeat(graph, "a", "b");
        xdigraph_add_edge_repeat(graph, "a", "c");
        xdigraph_add_edge_repeat(graph, "a", "d");
        xdigraph_add_edge_repeat(graph, "b", "c");
        xdigraph_add_edge_repeat(graph, "b", "d");
        xdigraph_add_edge_repeat(graph, "c", "d");

        //xassert(xdigraph_is_complete(graph));
        xassert(xdigraph_is_dense(graph));

        xdigraph_free(&graph);
    }

    /* xdigraph_is_sparse */
    {
        XDigraph_PT graph = xdigraph_test_graph1();
        xassert(xdigraph_is_sparse(graph));
        xdigraph_free(&graph);
    }
#endif

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
