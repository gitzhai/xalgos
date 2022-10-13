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

#ifndef XARENA_INCLUDED
#define XARENA_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XArena* XArena_PT;

extern XArena_PT xarena_new    (void);

extern void*     xarena_alloc  (XArena_PT  arena, int nbytes);
extern void*     xarena_calloc (XArena_PT  arena, int count, int nbytes);
extern void*     xarena_realloc(XArena_PT  arena, void* p, int old_sz, int new_sz);

extern void      xarena_freep  (XArena_PT  arena, void** p, int n);

extern void      xarena_clear  (XArena_PT  arena);
extern void      xarena_free   (XArena_PT* parena);

#ifdef __cplusplus
}
#endif

#endif
