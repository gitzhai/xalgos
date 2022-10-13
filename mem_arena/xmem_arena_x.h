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

#ifndef XARENAX_INCLUDED
#define XARENAX_INCLUDED

#include "../include/xhash_table.h"
#include "../include/xarray_pointer.h"
#include "../include/xmem_arena.h"

typedef union XArena_Obj* XArena_Obj_PT;
union XArena_Obj {
    XArena_Obj_PT next;
    char          data[1];
};

struct XArena {
    char*         start_free;
    char*         end_free;
    
    XPArray_PT    free_list;

    XHashtab_PT   mem_track;  /* save the allocated memory pointer for free */
};

#endif
