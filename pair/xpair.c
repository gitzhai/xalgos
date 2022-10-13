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

#include <stddef.h>
#include <string.h>

#include "../include/xmem.h"
#include "../include/xpair.h"

XPair_PT xpair_new(void *first, void *second) {
    XPair_PT pair = XMEM_CALLOC(1, sizeof(*pair));
    if (!pair) {
        return NULL;
    }

    pair->first = first;
    pair->second = second;

    return pair;
}

XPair_PT xpair_copy(XPair_PT pair) {
    XPair_PT npair = XMEM_CALLOC(1, sizeof(*pair));
    if (!npair) {
        return NULL;
    }

    npair->first = pair->first;
    npair->second = pair->second;

    return npair;
}

XPair_PT xpair_deep_copy(XPair_PT pair, int first_size, int second_size) {
    XPair_PT npair = XMEM_CALLOC(1, sizeof(*pair));
    if (!npair) {
        return NULL;
    }

    if (0 < first_size) {
        npair->first = XMEM_CALLOC(1, first_size);
        if (!npair->first) {
            xpair_deep_free(&npair);
            return NULL;
        }
        memcpy(npair->first, pair->first, first_size);
    }

    if (0 < second_size) {
        npair->second = XMEM_CALLOC(1, second_size);
        if (!npair->second) {
            xpair_deep_free(&npair);
            return NULL;
        }
        memcpy(npair->second, pair->second, second_size);
    }

    return npair;
}

void* xpair_first(XPair_PT pair) {
    return pair->first;
}

void* xpair_second(XPair_PT pair) {
    return pair->second;
}

void xpair_set_first(XPair_PT pair, void* first) {
    pair->first = first;
}

void xpair_set_second(XPair_PT pair, void *second) {
    pair->second = second;
}

void xpair_free(XPair_PT *pair) {
    XMEM_FREE(*pair);
}

void xpair_free_apply(XPair_PT *pair, bool (*apply)(void *first, void **second, void *cl), void *cl) {
    apply((*pair)->first, &(*pair)->second, cl);
    XMEM_FREE(*pair);
}

void xpair_deep_free(XPair_PT *pair) {
    XMEM_FREE((*pair)->first);
    XMEM_FREE((*pair)->second);
    XMEM_FREE(*pair);
}

void xpair_free_first(XPair_PT pair) {
    XMEM_FREE(pair->first);
}

void xpair_free_second(XPair_PT pair) {
    XMEM_FREE(pair->second);
}
