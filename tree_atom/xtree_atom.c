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
*       <<C Interfaces and Implementations>> David R. Hanson, chapter 3
*/

#include <string.h>
#include <limits.h>
#include <stdarg.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../utils/xutils.h"
#include "../tree_set/xtree_set_x.h"
#include "../include/xtree_atom.h"

XAtom_PT xatom_new(int (*cmp)(void *atom1, void *atom2, void *cl), void *cl) {
    return xset_new(cmp, cl);
}

static
bool xatom_copy_apply(void *elem, void *cl) {
    return NULL != xatom_string((XAtom_PT)cl, (char*)elem);
}

XAtom_PT xatom_copy(XAtom_PT  atom) {
    XAtom_PT natom = xatom_new(atom ? atom->cmp : NULL, atom ? atom->cl : NULL);
    if (!natom) {
        return NULL;
    }

    if (xset_map_break_if_false(atom, xatom_copy_apply, (void*)natom)) {
        xatom_free(&atom);
        return NULL;
    }

    return natom;
}

char* xatom_chars(XAtom_PT atom, char *str, int len) {
    xassert(atom);
    xassert(str);
    xassert(0 < len);

    if (!atom || !str || (len <= 0)) {
        return NULL;
    }

    {
        char *new_elem = XMEM_CALLOC(1, len + 1);
        if (!new_elem) {
            return NULL;
        }
        memcpy(new_elem, str, len);

        {
            /* try to find the equal atom at first */
            char *elem = xset_get(atom, (void*)new_elem);
            if (!elem) {
                /* equal atom not found, save the new atom */
                if (!xset_put_repeat(atom, (void*)new_elem)) {
                    XMEM_FREE(new_elem);
                    return NULL;
                }
                return new_elem;
            }

            XMEM_FREE(new_elem);
            return elem;
        }
    }
}

char* xatom_string(XAtom_PT atom, char *str) {
    xassert(atom);
    xassert(str);

    if (!atom || !str) {
        return NULL;
    }

    {
        /* try to find the equal atom at first */
        char *elem = xset_get(atom, (void*)str);
        if (!elem) {
            /* equal atom not found, create a new one */
            int len = (int)strlen(str);
            char *new_elem = XMEM_CALLOC(1, len + 1);
            if (!new_elem) {
                return NULL;
            }

            memcpy(new_elem, str, len);

            /* save the new atom */
            if (!xset_put_repeat(atom, (void*)new_elem)) {
                XMEM_FREE(new_elem);
                return NULL;
            }

            return new_elem;
        }

        return elem;
    }
}

char* xatom_int(XAtom_PT atom, long n) {
    xassert(atom);

    if (!atom) {
        return NULL;
    }

    {
        char str[128] = { 0 };
        char *s = str + sizeof str;
        unsigned long m = 0;

        if (n == LONG_MIN) {
            m = LONG_MAX + 1UL;
        }
        else if (n < 0) {
            m = -n;
        }
        else {
            m = n;
        }

        do {
            *--s = m % 10 + '0';
        } while (0 < (m /= 10));

        if (n < 0) {
            *--s = '-';
        }

        return xatom_chars(atom, s, (int)((str + sizeof str) - s));
    }
}

bool xatom_vload(XAtom_PT atom, char *str, ...) {
    xassert(atom);
    xassert(str);

    if (!atom || !str) {
        return false;
    }

    {
        va_list ap;
        va_start(ap, str);

        while (str) {
            char* ptr = xatom_string(atom, str);
            if (!ptr) {
                return false;
            }

            str = va_arg(ap, char *);
        }

        va_end(ap);
    }

    return true;
}

bool xatom_aload(XAtom_PT atom, char **strs, int len) {
    xassert(atom);
    xassert(strs);
    xassert(*strs);
    xassert(0 <= len);

    if (!atom || !strs || !*strs || (len < 0)) {
        return false;
    }

    for (int i = 0; i < len; i++) {
        char* ptr = xatom_string(atom, strs[i]);
        if (!ptr) {
            return false;
        }
    }

    return true;
}

void xatom_map(XAtom_PT atom, bool (*apply)(void *atom_str, void *cl), void *cl) {
    xset_map(atom, apply, cl);
}

void xatom_remove(XAtom_PT atom, char *str) {
    xset_deep_remove(atom, str);
}

void xatom_clear(XAtom_PT atom) {
    xset_deep_clear(atom);
}

void xatom_free(XAtom_PT* patom) {
    xset_deep_free(patom);
}

int xatom_size(XAtom_PT atom) {
    return xset_size(atom);
}

bool xatom_is_empty(XAtom_PT atom) {
    return xset_is_empty(atom);
}

bool xatom_is_atom(XAtom_PT atom, char *str) {
    void *elem = xset_get(atom, str);
    if (!elem) {
        return false;
    }

    return elem == (void*)str;
}
