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
*
*   Refer to :
*       <<Algorithms>> Fourth Edition. chapter 2.4
*/

#ifndef TMINHEAPX_INCLUDED
#define TMINHEAPX_INCLUDED

#include "../include/xqueue_deque.h"
#include "theap_min.h"

struct TMinHeap {
    XDeque_PT   tree;   /* implemented with two branches here, M branches can be used too */

    int       (*cmp)(void *x, void *y, void *cl);
    void       *cl;
};

#endif
