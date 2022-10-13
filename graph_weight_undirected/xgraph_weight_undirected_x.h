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

#ifndef XWGRAPHX_INCLUDED
#define XWGRAPHX_INCLUDED

#include "../include/xtree_map.h"
#include "../graph_undirected/xgraph_undirected_x.h"
#include "../include/xgraph_weight_undirected.h"

struct XWGraph {
    XGraph_PT native_graph;     /* can re-use all the methods of XGraph_PT */

    XMap_PT   wadjsets;       /* key   : vertex;
                               * value : XSet_PT  {XWEdge_PT, XWEdge_PT, ...} */
};

/* used for internal implementations */
typedef struct XWGraph_3Paras  XWGraph_3Paras_T;
typedef struct XWGraph_3Paras* XWGraph_3Paras_PT;

struct XWGraph_3Paras {
    XWGraph_PT graph;

    void     *para1;  /* para1 - para3 are used for internal function call */
    void     *para2;
    void     *para3;
};

typedef struct XWGraph_6Paras  XWGraph_6Paras_T;
typedef struct XWGraph_6Paras* XWGraph_6Paras_PT;

struct XWGraph_6Paras {
    XWGraph_PT graph;

    void     *para1;  /* para1 - para6 are used for internal function call */
    void     *para2;
    void     *para3;
    void     *para4;
    void     *para5;
    void     *para6;
};

#endif
