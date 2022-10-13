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

#ifndef XHASHTAB_INCLUDED
#define XHASHTAB_INCLUDED

#include <stdbool.h>
#include "xlist_s.h"
#include "xhash_rbtree.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XRBTreeHash_PT XHashtab_PT;

/* O(1) */
extern XHashtab_PT  xhashtab_new                (int hint, int (*hash)(void *key), int (*cmp)(void *key1, void *key2, void *cl), void *cl);

/* O(NlgN) */
extern XHashtab_PT  xhashtab_copy               (XHashtab_PT table);
extern XHashtab_PT  xhashtab_deep_copy          (XHashtab_PT table, int key_size);

/* O(1) */
extern bool         xhashtab_put_repeat         (XHashtab_PT table, void *key);
extern bool         xhashtab_put_unique         (XHashtab_PT table, void *key);

/* O(lgN) */
extern void*        xhashtab_get                (XHashtab_PT table, void *key);
extern bool         xhashtab_find               (XHashtab_PT table, void *key);

/* O(NlgN) */
extern XSList_PT    xhashtab_get_all            (XHashtab_PT table, void *key);

/* O(lgN) */
extern bool         xhashtab_remove             (XHashtab_PT table, void *key);
extern bool         xhashtab_deep_remove        (XHashtab_PT table, void *key);

/* O(NlgN) */
extern int          xhashtab_remove_all         (XHashtab_PT table, void *key);
extern int          xhashtab_deep_remove_all    (XHashtab_PT table, void *key);

/* O(NlgN) */
extern void         xhashtab_clear              (XHashtab_PT table);
extern void         xhashtab_deep_clear         (XHashtab_PT table);

/* O(NlgN) */
extern void         xhashtab_free               (XHashtab_PT *table);
extern void         xhashtab_deep_free          (XHashtab_PT *table);

/* O(NlgN) */
extern int          xhashtab_map                (XHashtab_PT table, bool (*apply)(void *key, void *cl), void *cl);
extern bool         xhashtab_map_break_if_true  (XHashtab_PT table, bool (*apply)(void *key, void *cl), void *cl);
extern bool         xhashtab_map_break_if_false (XHashtab_PT table, bool (*apply)(void *key, void *cl), void *cl);

/* O(1) */
extern bool         xhashtab_swap               (XHashtab_PT table1, XHashtab_PT table2);

/* O(1) */
extern int          xhashtab_size               (XHashtab_PT table);
extern bool         xhashtab_is_empty           (XHashtab_PT table);

/* O(1) */
extern double       xhashtab_loading_factor     (XHashtab_PT table);

/* O(1) */
extern int          xhashtab_bucket_size        (XHashtab_PT table);

/* O(1) */
extern int          xhashtab_elems_in_bucket    (XHashtab_PT table, int bucket);

/* O(NlgN) */
extern int          xhashtab_key_size           (XHashtab_PT table, void *key);

#ifdef __cplusplus
}
#endif

#endif
