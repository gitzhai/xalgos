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

#ifndef XWDIGRAPHX_INCLUDED
#define XWDIGRAPHX_INCLUDED

#include "../graph_weight_undirected/xgraph_weight_undirected_x.h"
#include "../include/xgraph_weight_directed.h"

typedef XWGraph_3Paras_T  XWDigraph_3Paras_T;
typedef XWGraph_3Paras_PT XWDigraph_3Paras_PT;

typedef XWGraph_6Paras_T  XWDigraph_6Paras_T;
typedef XWGraph_6Paras_PT XWDigraph_6Paras_PT;

typedef struct XWDigraph_9Paras  XWDigraph_9Paras_T;
typedef struct XWDigraph_9Paras* XWDigraph_9Paras_PT;

struct XWDigraph_9Paras {
    XWDigraph_PT graph;

    void     *para1;  /* para1 - para9 are used for internal function call */
    void     *para2;
    void     *para3;
    void     *para4;
    void     *para5;
    void     *para6;
    void     *para7;
    void     *para8;
    void     *para9;
};

#endif
