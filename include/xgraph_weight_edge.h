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

#ifndef XWEDGE_INCLUDED
#define XWEDGE_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* define XWEdge_PT here to make direct access to member "v", "w" and weight */
typedef struct XWEdge*  XWEdge_PT;
struct XWEdge {
    void  *v;
    void  *w;
    double weight;

    int  (*cmp_vertex)(void *v, void *w, void *cl);
    void  *cl;
};

extern XWEdge_PT  xwedge_new             (void *v, void *w, double weight, int (*cmp_vertex)(void *v, void *w, void *cl), void *cl);
extern void       xwedge_free            (XWEdge_PT *edge);
extern XWEdge_PT  xwedge_copy            (XWEdge_PT edge);

extern void*      xwedge_other           (XWEdge_PT edge, void *vertex);

extern int        xwedge_cmp_undirected  (void *edge1, void *edge2, void *cl);
extern int        xwedge_cmp_directed    (void *edge1, void *edge2, void *cl);
extern int        xwedge_cmp_weight_only (void *edge1, void *edge2, void *cl);

#ifdef __cplusplus
}
#endif

#endif
