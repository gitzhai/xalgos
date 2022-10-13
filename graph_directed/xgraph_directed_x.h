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

#ifndef XDIGRAPHX_INCLUDED
#define XDIGRAPHX_INCLUDED

#include "../graph_undirected/xgraph_undirected_x.h"
#include "../include/xgraph_directed.h"

typedef XGraph_3Paras_T  XDigraph_3Paras_T;
typedef XGraph_3Paras_PT XDigraph_3Paras_PT;

typedef XGraph_6Paras_T  XDigraph_6Paras_T;
typedef XGraph_6Paras_PT XDigraph_6Paras_PT;

extern XDList_PT xdigraph_pre_order_dfs          (XDigraph_PT graph);
extern XDList_PT xdigraph_post_order_dfs         (XDigraph_PT graph);
extern XDList_PT xdigraph_reverse_post_order_dfs (XDigraph_PT graph);

#endif
