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

#ifndef XKEYINDEXMAXPQ_INCLUDED
#define XKEYINDEXMAXPQ_INCLUDED

#include "xqueue_key_index_priority_min.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XKeyIndexMinPQ_PT XKeyIndexMaxPQ_PT;

/* O(1) */
extern XKeyIndexMaxPQ_PT  xkeyindexmaxpq_new                (int (*key_cmp)(void *key1, void *key2, void *cl), int(*value_cmp)(void *value1, void *value2, void *cl), void *cl);

/* O(NlgN) */
extern XKeyIndexMaxPQ_PT  xkeyindexmaxpq_copy               (XKeyIndexMaxPQ_PT queue);

/* O(lgN) */
extern bool               xkeyindexmaxpq_push               (XKeyIndexMaxPQ_PT queue, void* key, void *value, void **old_value);
extern bool               xkeyindexmaxpq_pop                (XKeyIndexMaxPQ_PT queue, void **key, void **value);

/* O(lgN) */
extern bool               xkeyindexmaxpq_peek               (XKeyIndexMaxPQ_PT queue, void **key, void **value);

/* O(lgN) */
extern void*              xkeyindexmaxpq_get                (XKeyIndexMaxPQ_PT queue, void *key);

/* O(lgN) */
extern bool               xkeyindexmaxpq_remove             (XKeyIndexMaxPQ_PT queue, void *key, void **old_value);

/* O(N) */
extern int                xkeyindexmaxpq_map                (XKeyIndexMaxPQ_PT queue, bool (*apply)(void *value, void **key, void *cl), void *cl);
extern bool               xkeyindexmaxpq_map_break_if_true  (XKeyIndexMaxPQ_PT queue, bool (*apply)(void *value, void **key, void *cl), void *cl);
extern bool               xkeyindexmaxpq_map_break_if_false (XKeyIndexMaxPQ_PT queue, bool (*apply)(void *value, void **key, void *cl), void *cl);

/* O(NlgN) */
extern void               xkeyindexmaxpq_free               (XKeyIndexMaxPQ_PT *pqueue);
extern void               xkeyindexmaxpq_free_apply         (XKeyIndexMaxPQ_PT *pqueue, bool (*apply)(void *value, void **key, void *cl), void *cl);
extern void               xkeyindexmaxpq_deep_free          (XKeyIndexMaxPQ_PT *pqueue);

/* O(NlgN) */
extern void               xkeyindexmaxpq_clear              (XKeyIndexMaxPQ_PT queue);
extern void               xkeyindexmaxpq_clear_apply        (XKeyIndexMaxPQ_PT queue, bool (*apply)(void *value, void **key, void *cl), void *cl);
extern void               xkeyindexmaxpq_deep_clear         (XKeyIndexMaxPQ_PT queue);

/* O(1) */
extern int                xkeyindexmaxpq_size               (XKeyIndexMaxPQ_PT queue);
extern bool               xkeyindexmaxpq_is_empty           (XKeyIndexMaxPQ_PT queue);

#ifdef __cplusplus
}
#endif

#endif
