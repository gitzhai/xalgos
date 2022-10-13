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

#ifndef XMAPX_INCLUDED
#define XMAPX_INCLUDED

#include "../include/xtree_map.h"

/* for internal use */

/* O(lgN) */
extern void xmap_replace_key(XMap_PT map, void *old_key, void *new_key);

#endif
