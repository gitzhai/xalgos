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

#include "../include/xmem.h"
#include "../include/xgraph_weight_edge.h"

XWEdge_PT xwedge_new(void *v, void *w, double weight, int (*cmp_vertex)(void *v, void *w, void *cl), void *cl) {
    XWEdge_PT edge = (XWEdge_PT)XMEM_CALLOC(1, sizeof(*edge));
    if (!edge) {
        return NULL;
    }

    edge->v = v;
    edge->w = w;
    edge->weight = weight;

    edge->cmp_vertex = cmp_vertex;
    edge->cl = cl;

    return edge;
}

void xwedge_free(XWEdge_PT *edge) {
    XMEM_FREE(*edge);
}

XWEdge_PT xwedge_copy(XWEdge_PT edge) {
    XWEdge_PT nedge = (XWEdge_PT)XMEM_CALLOC(1, sizeof(*edge));
    if (!nedge) {
        return NULL;
    }

    nedge->v = edge->v;
    nedge->w = edge->w;
    nedge->weight = edge->weight;

    nedge->cmp_vertex = edge->cmp_vertex;
    nedge->cl = edge->cl;

    return nedge;
}

void* xwedge_other(XWEdge_PT edge, void *vertex) {
    return edge->cmp_vertex(vertex, edge->v, edge->cl) == 0 ? edge->w : edge->v;
}

/* {v, w, weight} == {w, v, weight} */
int xwedge_cmp_undirected(void *edge1, void *edge2, void *cl) {
    XWEdge_PT pedge1 = (XWEdge_PT)edge1;
    XWEdge_PT pedge2 = (XWEdge_PT)edge2;

    if (pedge1->weight < pedge2->weight) {
        return -1;
    }
    else if (pedge2->weight < pedge1->weight) {
        return 1;
    }
    else {
        int ret = pedge1->cmp_vertex(pedge1->v, pedge2->v, pedge1->cl);
        if (ret == 0) {
            return pedge1->cmp_vertex(pedge1->w, pedge2->w, pedge1->cl);
        }
        else {
            if (pedge1->cmp_vertex(pedge1->v, pedge2->w, pedge1->cl) == 0) {
                return pedge1->cmp_vertex(pedge1->w, pedge2->v, pedge1->cl);
            }
            return ret;
        }
    }
}

/* {v, w, weight} != {w, v, weight} */
int xwedge_cmp_directed(void *edge1, void *edge2, void *cl) {
    XWEdge_PT pedge1 = (XWEdge_PT)edge1;
    XWEdge_PT pedge2 = (XWEdge_PT)edge2;

    if (pedge1->weight < pedge2->weight) {
        return -1;
    }
    else if (pedge2->weight < pedge1->weight) {
        return 1;
    }
    else {
        int ret = pedge1->cmp_vertex(pedge1->v, pedge2->v, pedge1->cl);
        if (ret == 0) {
            return pedge1->cmp_vertex(pedge1->w, pedge2->w, pedge1->cl);
        }
        else {
            return ret;
        }
    }
}

int xwedge_cmp_weight_only(void *edge1, void *edge2, void *cl) {
    XWEdge_PT pedge1 = (XWEdge_PT)edge1;
    XWEdge_PT pedge2 = (XWEdge_PT)edge2;

    if (pedge1->weight < pedge2->weight) {
        return -1;
    }
    else if (pedge2->weight < pedge1->weight) {
        return 1;
    }
    else {
        return 0;
    }
}
