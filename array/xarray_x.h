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

#ifndef XARRAYX_INCLUDED
#define XARRAYX_INCLUDED

#include "../include/xarray.h"

struct XArray {
    int   size;         /* element numbers can be filled into array */
    int   elem_size;    /* element size */
    char *datas;        /* memory to save the datas */
};

/* used to transfer one kind of interface to another */
typedef struct XArray_Apply_Paras  XArray_Apply_Paras_T;
typedef struct XArray_Apply_Paras* XArray_Apply_Paras_PT;

struct XArray_Apply_Paras {
    XArray_PT   array;
    int       (*apply)(void *x, void *y, int elem_size, void *cl);
    void       *cl;
};

extern XArray_PT   xarray_copyn_impl       (XArray_PT array, int start, int count, bool (*apply)(void *x, int elem_size, void *cl), void *cl);

#endif
