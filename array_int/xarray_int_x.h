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

#ifndef XIARRAYX_INCLUDED
#define XIARRAYX_INCLUDED

#include "../include/xarray_int.h"

struct XIArray {
    int   *datas;        /* memory to save the datas */

    int    size;         /* capacity : how many int elements can be saved */
};

/* O(N) */
extern XIArray_PT  xiarray_scope_copy              (XIArray_PT array, int start, int end);
extern bool        xiarray_scope_index_copy        (XIArray_PT array, int start, int end, XIArray_PT darray, int dstart);
extern bool        xiarray_scope_index_copy_resize (XIArray_PT array, int start, int end, XIArray_PT darray, int dstart);

/* O(NlgN) */
extern bool        xiarray_quick_sort_if           (XIArray_PT array, int(*cmp)(int x, int y, void *cl), void *cl);

/* O(1) */
static inline
int xiarray_get_impl (XIArray_PT array, int i) {
    return array->datas[i];
}

/* O(1) */
static inline
void xiarray_put_impl (XIArray_PT array, int i, int data) {
    array->datas[i] = data;
}

#endif
