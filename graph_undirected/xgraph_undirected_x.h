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

#ifndef XGRAPHX_INCLUDED
#define XGRAPHX_INCLUDED

#include "../include/xtree_atom.h"
#include "../include/xtree_map.h"
#include "../include/xgraph_undirected.h"

struct XGraph {
    XMap_PT  adjsets;   /* key : vertex;  value : XSet_PT (including all vertexes adjacent to the key vertex) */

    int (*cmp)(void *vertex1, void *vertex2, void *cl);
    void *cl;
};

/* used for internal implementations */
typedef struct XGraph_3Paras  XGraph_3Paras_T;
typedef struct XGraph_3Paras* XGraph_3Paras_PT;

struct XGraph_3Paras {
    XGraph_PT graph;

    void     *para1;  /* para1 - para3 are used for internal function call */
    void     *para2;
    void     *para3;
};

typedef struct XGraph_6Paras  XGraph_6Paras_T;
typedef struct XGraph_6Paras* XGraph_6Paras_PT;

struct XGraph_6Paras {
    XGraph_PT graph;

    void     *para1;  /* para1 - para6 are used for internal function call */
    void     *para2;
    void     *para3;
    void     *para4;
    void     *para5;
    void     *para6;
};

extern XDList_PT xgraph_path_by_found_paths_impl    (XGraph_PT graph, XMap_PT paths, void *svertex, void *tvertex);

#endif
