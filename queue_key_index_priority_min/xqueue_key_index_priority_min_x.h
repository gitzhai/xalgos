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

#ifndef XKEYINDEXMINPQX_INCLUDED
#define XKEYINDEXMINPQX_INCLUDED

#include "../include/xtree_map.h"
#include "../include/xqueue_key_index_priority_min.h"

struct XKeyIndexMinPQ {
    XMap_PT key_map;   /* {key, value} */
    XMap_PT value_map; /* {value, key } */
};

#endif
