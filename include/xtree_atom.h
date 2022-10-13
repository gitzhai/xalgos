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

#ifndef XTREEATOM_INCLUDED
#define XTREEATOM_INCLUDED

#include <stdbool.h>
#include "xtree_set.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XSet_PT XAtom_PT;

/* O(1) */
extern XAtom_PT    xatom_new                (int (*cmp)(void *atom1, void *atom2, void *cl), void *cl);

/* */
extern XAtom_PT    xatom_copy               (XAtom_PT  atom);

/* O(1) */
extern char*       xatom_chars              (XAtom_PT  atom, char *str, int len);
extern char*       xatom_string             (XAtom_PT  atom, char *str);
extern char*       xatom_int                (XAtom_PT  atom, long n);

/* O(1) */
extern bool        xatom_vload              (XAtom_PT  atom, char *str, ...);
/* O(N) */
extern bool        xatom_aload              (XAtom_PT  atom, char **strs, int len);

/* O(N) */
extern void        xatom_map                (XAtom_PT  atom, bool (*apply)(void *atom_str, void *cl), void *cl);

/* O(lgN) */
extern void        xatom_remove             (XAtom_PT  atom, char *atom_str);

/* O(NlgN) */
extern void        xatom_clear              (XAtom_PT  atom);
extern void        xatom_free               (XAtom_PT* patom);

/* O(1) */
extern int         xatom_size               (XAtom_PT  atom);
extern bool        xatom_is_empty           (XAtom_PT  atom);

/* O(lgN) */
extern bool        xatom_is_atom            (XAtom_PT  atom, char *str);

#ifdef __cplusplus
}
#endif

#endif
