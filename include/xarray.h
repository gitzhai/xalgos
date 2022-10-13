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

#ifndef XARRAY_INCLUDED
#define XARRAY_INCLUDED

#include <stdbool.h>
#include "xarray_int.h"
#include "xarray_pointer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XArray* XArray_PT;

/* O(1) */
extern XArray_PT   xarray_new                  (int size, int elem_size);

/* O(N) */
extern XArray_PT   xarray_copy                 (XArray_PT array);
extern XArray_PT   xarray_copyn                (XArray_PT array, int count);

/* O(N) */
extern int         xarray_vload                (XArray_PT array, void *x, ...);
extern int         xarray_aload                (XArray_PT array, XPArray_PT xs);

/* O(1) */
extern bool        xarray_put                  (XArray_PT array, int i, void *data, void *old_data);

/* O(N) */
extern bool        xarray_put_expand           (XArray_PT array, int i, void *data, void *old_data);
extern bool        xarray_put_fix_expand       (XArray_PT array, int i, void *data, void *old_data, int new_size);

/* O(N) */
extern bool        xarray_fill                 (XArray_PT array, void *data);
extern bool        xarray_scope_fill           (XArray_PT array, int start, int end, void *data);

/* O(1) */
extern void*       xarray_get                  (XArray_PT array, int i);

/* O(1) */
extern void*       xarray_front                (XArray_PT array);
extern void*       xarray_back                 (XArray_PT array);

/* O(N) */
extern int         xarray_map                  (XArray_PT array, bool (*apply)(void *x, int elem_size, void *cl), void *cl);
extern bool        xarray_map_break_if_true    (XArray_PT array, bool (*apply)(void *x, int elem_size, void *cl), void *cl);
extern bool        xarray_map_break_if_false   (XArray_PT array, bool (*apply)(void *x, int elem_size, void *cl), void *cl);

/* O(1) */
extern void        xarray_free                 (XArray_PT *parray);

/* O(N) */
extern void        xarray_clear                (XArray_PT array);

/* O(1) */
extern bool        xarray_remove               (XArray_PT array, int i, void *old_data);

/* O(1) */
extern int         xarray_size                 (XArray_PT array);
extern int         xarray_elem_size            (XArray_PT array);
extern bool        xarray_is_empty             (XArray_PT array);

/* O(N) */
extern bool        xarray_resize               (XArray_PT array, int new_size);
extern bool        xarray_remove_resize        (XArray_PT array, int i, void *old_data);

/* O(1) */
extern bool        xarray_swap                 (XArray_PT array1, XArray_PT array2);

/* O(1) */
extern bool        xarray_exch                 (XArray_PT array, int i, int j);

/* O(N) */
extern bool        xarray_is_sorted            (XArray_PT array, int (*cmp)(void *x, void *y, int elem_size, void *cl), void *cl);

/* O(NlgN) */
extern bool        xarray_quick_sort           (XArray_PT array, int (*cmp)(void *x, void *y, int elem_size, void *cl), void *cl);

/* O(NlgN) */
extern XPArray_PT  xarray_pointer_sort         (XArray_PT array, int (*cmp)(void *x, void *y, int elem_size, void *cl), void *cl);
/* O(N) */
extern bool        xarray_pointer_inplace_sort (XArray_PT array, XPArray_PT pointer_array);

/* O(NlgN) */
extern XIArray_PT  xarray_index_sort           (XArray_PT array, int (*cmp)(void *x, void *y, int elem_size, void *cl), void *cl);
/* O(N) */
extern bool        xarray_index_inplace_sort   (XArray_PT array, XIArray_PT index_array);

/* O(lgN) */
extern int         xarray_binary_search        (XArray_PT array, void *data, int (*cmp)(void *x, void *y, int elem_size, void *cl), void *cl);

#ifdef __cplusplus
}
#endif

#endif
