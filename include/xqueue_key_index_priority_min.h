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

#ifndef XKEYINDEXMINPQ_INCLUDED
#define XKEYINDEXMINPQ_INCLUDED

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XKeyIndexMinPQ *XKeyIndexMinPQ_PT;

/* O(1) */
extern XKeyIndexMinPQ_PT  xkeyindexminpq_new                (int (*key_cmp)(void *key1, void *key2, void *cl), int(*value_cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(NlgN) */
extern XKeyIndexMinPQ_PT  xkeyindexminpq_copy               (XKeyIndexMinPQ_PT queue);

/* O(lgN) */
extern bool               xkeyindexminpq_push               (XKeyIndexMinPQ_PT queue, void* key, void *value, void **old_value);
extern bool               xkeyindexminpq_pop                (XKeyIndexMinPQ_PT queue, void **key, void **value);

/* O(lgN) */
extern bool               xkeyindexminpq_peek               (XKeyIndexMinPQ_PT queue, void **key, void **value);

/* O(lgN) */
extern void*              xkeyindexminpq_get                (XKeyIndexMinPQ_PT queue, void *key);

/* O(lgN) */
extern bool               xkeyindexminpq_remove             (XKeyIndexMinPQ_PT queue, void *key, void **old_value);

/* O(N) */
extern int                xkeyindexminpq_map                (XKeyIndexMinPQ_PT queue, bool (*apply)(void *value, void **key, void *cl), void *cl);
extern bool               xkeyindexminpq_map_break_if_true  (XKeyIndexMinPQ_PT queue, bool (*apply)(void *value, void **key, void *cl), void *cl);
extern bool               xkeyindexminpq_map_break_if_false (XKeyIndexMinPQ_PT queue, bool (*apply)(void *value, void **key, void *cl), void *cl);

/* O(NlgN) */
extern void               xkeyindexminpq_free               (XKeyIndexMinPQ_PT *pqueue);
extern void               xkeyindexminpq_deep_free          (XKeyIndexMinPQ_PT *pqueue);

/* O(NlgN) */
extern void               xkeyindexminpq_clear              (XKeyIndexMinPQ_PT queue);
extern void               xkeyindexminpq_deep_clear         (XKeyIndexMinPQ_PT queue);

/* O(1) */
extern int                xkeyindexminpq_size               (XKeyIndexMinPQ_PT queue);
extern bool               xkeyindexminpq_is_empty           (XKeyIndexMinPQ_PT queue);

#ifdef __cplusplus
}
#endif

#endif
