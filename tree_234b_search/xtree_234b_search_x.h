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

#ifndef T234BSTREEX_INCLUDED
#define T234BSTREEX_INCLUDED

#include "xtree_234b_search.h"

struct T234BSTree_Node {
    T234BSTree_Node_PT link1;
    T234BSTree_Node_PT link2;
    T234BSTree_Node_PT link3;
    T234BSTree_Node_PT link4;

    void *key1, *key2, *key3;
    void *value1, *value2, *value3;

    int target;  /* temporary variable to record which key is the operation result : 1 : key1, 2 : key2, 3 : key3 */

    int   size;
};

struct T234BSTree {
    T234BSTree_Node_PT root;

    int (*cmp)(void *key1, void *key2, void *cl);
    void *cl;
};

#endif
