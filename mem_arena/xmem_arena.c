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
*       1. <<C Interfaces and Implementations>> David R. Hanson, chapter 6
*       2. SGI STL library : stl_alloc.h
*/

#include <stddef.h>
#include <string.h>

#include "../include/xassert.h"
#include "../include/xexcept.h"
#include "../utils/xutils.h"
#include "../include/xmem.h"
#include "../array_pointer/xarray_pointer_x.h"
#include "xmem_arena_x.h"

const XExcept_T xg_arena_failed = { "Arena allocation failed" };

static 
int xarena_round_up(int bytes) { 
    return (((bytes) + XUTILS_ARENA_MIN_ALIGN_SIZE - 1) & ~(XUTILS_ARENA_MIN_ALIGN_SIZE - 1));
}

static 
int xarena_index(int bytes) {
    return (((bytes) + XUTILS_ARENA_MIN_ALIGN_SIZE - 1) / XUTILS_ARENA_MIN_ALIGN_SIZE - 1);
}

/* allocate memory in large chunks in order to avoid fragmenting the malloc heap too much.
 * assume that size is properly aligned.
 */
static 
char* xarena_chunk_alloc(XArena_PT arena, int size, int* nobjs)
{
    int total_bytes = size * (*nobjs);
    int bytes_left = (int)(arena->end_free - arena->start_free);

    if (total_bytes <= bytes_left) {
        char *result = arena->start_free;
        arena->start_free += total_bytes;
        return result;
    }
    else if (size <= bytes_left) {
        char *result = arena->start_free;
        *nobjs = (int)(bytes_left / size);
        total_bytes = size * (*nobjs);
        arena->start_free += total_bytes;
        return result;
    }
    else {
        int bytes_to_get = 2 * total_bytes;

        // Try to make use of the left-over piece when free memory is smaller than "size".
        if (0 < bytes_left) {
            int index = xarena_index(bytes_left);
            XArena_Obj_PT obj = (XArena_Obj_PT)xparray_get_impl(arena->free_list, index);
            ((XArena_Obj_PT)arena->start_free)->next = obj;
            xparray_put_impl(arena->free_list, index, (void*)arena->start_free);
        }

        /* try to allocate new block of memorys */
        arena->start_free = (char*)XMEM_MALLOC(bytes_to_get);
        if (!arena->start_free) {
            /* failed to allocate new block of memorys, try to get memory from the exist blocks which is bigger than the size needed */
            for (int i = size; i <= XUTILS_ARENA_MAX_BYTES; i += XUTILS_ARENA_MIN_ALIGN_SIZE) {
                int index = xarena_index(i);
                XArena_Obj_PT p = (XArena_Obj_PT)xparray_get_impl(arena->free_list, index);
                if (p) {
                    xparray_put_impl(arena->free_list, index, (void*)p->next);
                    arena->start_free = (char*)p;
                    arena->end_free = arena->start_free + i;
                    return(xarena_chunk_alloc(arena, size, nobjs));
                }
            }

            arena->end_free = NULL;
            arena->start_free = NULL;
            xassert(false);

            *nobjs = 0;
            return NULL;
        }

        {
            if(!xhashtab_put_repeat(arena->mem_track, arena->start_free)) {
                xassert(false);
            }

            arena->end_free = arena->start_free + bytes_to_get;

            return(xarena_chunk_alloc(arena, size, nobjs));
        }
    }
}

/* Returns an object of size nbytes, and optionally adds to size nbytes free list.
 * assume that nbytes is properly aligned.
 */
static 
void* xarena_refill(XArena_PT arena, int nbytes)
{
    int nobjs[1] = { 32 };
    char* chunk = xarena_chunk_alloc(arena, nbytes, nobjs);
    if (1 == nobjs[0]) {
        return chunk;
    }

    /* Build free list in chunk */
    {
        XArena_Obj_PT current_obj = NULL;
        XArena_Obj_PT next_obj = (XArena_Obj_PT)(chunk + nbytes);

        xparray_put_impl(arena->free_list, xarena_index(nbytes), (void*)next_obj);

        for (int i = 1; /*blank here*/; i++) {
            current_obj = next_obj;
            next_obj = (XArena_Obj_PT)((char*)next_obj + nbytes);
            if (nobjs[0] - 1 == i) {
                current_obj->next = NULL;
                break;
            }
            else {
                current_obj->next = next_obj;
            }
        }
    }

    return chunk;
}

XArena_PT xarena_new(void) {
    XArena_PT arena = XMEM_CALLOC(1, sizeof(*arena));
    if (!arena) {
        return NULL;
    }

    arena->free_list = xparray_new(XUTILS_ARENA_MAX_BYTES / XUTILS_ARENA_MIN_ALIGN_SIZE);
    if (!arena->free_list) {
        XMEM_FREE(arena);
        return NULL;
    }

    arena->mem_track = xhashtab_new(XUTILS_HASH_SLOTS_DEFAULT_HINT, xutils_hash_pointer, xutils_pointer_equal, NULL);
    if (!arena->mem_track) {
        xparray_free(&arena->free_list);
        XMEM_FREE(arena);
        return NULL;
    }

    arena->start_free = NULL;
    arena->end_free = NULL;

    return arena;
}

void* xarena_alloc(XArena_PT arena, int nbytes) {
    xassert(arena);
    xassert(0 < nbytes);

    if (!arena || (nbytes <= 0)) {
#ifndef XMEM_NO_CHECK
        XEXCEPT_RAISE(xg_arena_failed);
#else
        return NULL;
#endif
    }

    if (XUTILS_ARENA_MAX_BYTES < nbytes) {
        void* ret = XMEM_MALLOC(nbytes);
        if (ret) {
            if (!xhashtab_put_repeat(arena->mem_track, ret)) {
                xassert(false);
            }
        }
        return ret;
    }
    else {
        XArena_Obj_PT result = (XArena_Obj_PT)xparray_get_impl(arena->free_list, xarena_index(nbytes));
        if (!result) {
            return xarena_refill(arena, xarena_round_up(nbytes));
        }
        else {
            xparray_put_impl(arena->free_list, xarena_index(nbytes), result->next);
            return result;
        }
    }
}

void* xarena_calloc(XArena_PT arena, int count, int nbytes) {
    xassert(arena);
    xassert(0 < count);
    xassert(0 < nbytes);

    if (!arena || (count <= 0) || (nbytes <= 0)) {
#ifndef XMEM_NO_CHECK
        XEXCEPT_RAISE(xg_arena_failed);
#else
        return NULL;
#endif
    }

    {
        int total = count * nbytes;
        if (XUTILS_ARENA_MAX_BYTES < total) {
            void *ret = XMEM_CALLOC(count, nbytes);
            if (ret ) {
                if (!xhashtab_put_repeat(arena->mem_track, ret)) {
                    xassert(false);
                }
            }
            return ret;
        }
        else {
            void *ret = xarena_alloc(arena, total);
            if (!ret) {
                return NULL;
            }

            memset(ret, 0, total);

            return ret;
        }
    }
}

void* xarena_realloc(XArena_PT arena, void* p,  int old_sz, int new_sz) {
    xassert(arena);
    xassert(p);
    xassert(0 < old_sz);
    xassert(0 < new_sz);

    if (!arena || !p || (old_sz <= 0) || (new_sz <= 0)) {
#ifndef XMEM_NO_CHECK
        XEXCEPT_RAISE(xg_arena_failed);
#else
        return p;
#endif
    }

    {
        if ((XUTILS_ARENA_MAX_BYTES < old_sz) && (XUTILS_ARENA_MAX_BYTES < new_sz)) {
            /* new p may equal to old p, so delete it from the mem_track at first */
            xhashtab_remove(arena->mem_track, p);

            XMEM_RESIZE(p, new_sz);
            if (!p) {
                xassert(false);
                return NULL;
            }

            if (!xhashtab_put_repeat(arena->mem_track, p)) {
                xassert(false);
            }

            return p;
        }

        if (xarena_round_up(old_sz) == xarena_round_up(new_sz)) {
            return p;
        }

        {
            void *ptr = xarena_calloc(arena, 1, new_sz);
            if (!ptr) {
                xassert(false);
                return p;
            }

            memcpy(ptr, p, (old_sz < new_sz ? old_sz : new_sz));

            xarena_freep(arena, p, old_sz);

            return ptr;
        }
    }
}

void xarena_freep(XArena_PT arena, void** p, int nbytes)
{
    xassert(arena);
    xassert(p);
    xassert(*p);
    xassert(0 < nbytes);

    if (!arena || !p || !*p || (nbytes <= 0)) {
        return;
    }

    if (XUTILS_ARENA_MAX_BYTES < nbytes) {
        xhashtab_remove(arena->mem_track, *p);
        XMEM_FREE(*p);
    }
    else {
        XArena_Obj_PT q = (XArena_Obj_PT)*p;
        q->next = (XArena_Obj_PT)xparray_get_impl(arena->free_list, xarena_index(nbytes));
        xparray_put_impl(arena->free_list, xarena_index(nbytes), q);
    }

    return;
}

static
bool xarena_clear_free(void *member, void *cl) {
    XMEM_FREE(member);
    return true;
}

void xarena_clear(XArena_PT arena) {
    xassert(arena);

    if (!arena) {
        return;
    }

    xparray_clear(arena->free_list);

    xhashtab_map(arena->mem_track, xarena_clear_free, NULL);
    xhashtab_clear(arena->mem_track);

    arena->start_free = NULL;
    arena->end_free = NULL;

    return;
}

void xarena_free(XArena_PT *parena) {
    xassert(parena);
    xassert(*parena);

    if (!parena || !*parena) {
        return;
    }

    xhashtab_deep_free(&(*parena)->mem_track);

    xparray_free(&(*parena)->free_list);

    XMEM_FREE(*parena);

    return;
}
