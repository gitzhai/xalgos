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
#include "../include/xtriple.h"

XTriple_PT xtriple_new(void *first, void *second, void *third) {
    XTriple_PT triple = XMEM_CALLOC(1, sizeof(*triple));
    if (!triple) {
        return NULL;
    }

    triple->first = first;
    triple->second = second;
    triple->third = third;

    return triple;
}

XTriple_PT xtriple_copy(XTriple_PT triple) {
    XTriple_PT ntriple = XMEM_CALLOC(1, sizeof(*triple));
    if (!ntriple) {
        return NULL;
    }

    ntriple->first = triple->first;
    ntriple->second = triple->second;
    ntriple->third = triple->third;

    return ntriple;
}

XTriple_PT xtriple_deep_copy(XTriple_PT triple, int first_size, int second_size, int third_size) {
    XTriple_PT ntriple = XMEM_CALLOC(1, sizeof(*triple));
    if (!ntriple) {
        return NULL;
    }

    if (0 < first_size) {
        ntriple->first = XMEM_CALLOC(1, first_size);
        if (!ntriple->first) {
            xtriple_deep_free(&ntriple);
            return NULL;
        }
        memcpy(ntriple->first, triple->first, first_size);
    }

    if (0 < second_size) {
        ntriple->second = XMEM_CALLOC(1, second_size);
        if (!ntriple->second) {
            xtriple_deep_free(&ntriple);
            return NULL;
        }
        memcpy(ntriple->second, triple->second, second_size);
    }

    if (0 < third_size) {
        ntriple->third = XMEM_CALLOC(1, third_size);
        if (!ntriple->third) {
            xtriple_deep_free(&ntriple);
            return NULL;
        }
        memcpy(ntriple->third, triple->third, second_size);
    }

    return ntriple;
}

void* xtriple_first(XTriple_PT triple) {
    return triple->first;
}

void* xtriple_second(XTriple_PT triple) {
    return triple->second;
}

void* xtriple_third(XTriple_PT triple) {
    return triple->third;
}

void xtriple_set_first(XTriple_PT triple, void* first) {
    triple->first = first;
}

void xtriple_set_second(XTriple_PT triple, void *second) {
    triple->second = second;
}

void xtriple_set_third(XTriple_PT triple, void *third) {
    triple->third = third;
}

void xtriple_free(XTriple_PT *triple) {
    XMEM_FREE(*triple);
}

void xtriple_deep_free(XTriple_PT *triple) {
    XMEM_FREE((*triple)->first);
    XMEM_FREE((*triple)->second);
    XMEM_FREE((*triple)->third);
    XMEM_FREE(*triple);
}

void xtriple_free_first(XTriple_PT triple) {
    XMEM_FREE(triple->first);
}

void xtriple_free_second(XTriple_PT triple) {
    XMEM_FREE(triple->second);
}

void xtriple_free_third(XTriple_PT triple) {
    XMEM_FREE(triple->third);
}
