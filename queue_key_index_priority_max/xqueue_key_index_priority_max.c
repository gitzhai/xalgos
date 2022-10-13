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

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "../queue_key_index_priority_min/xqueue_key_index_priority_min_x.h"
#include "../include/xqueue_key_index_priority_max.h"

XKeyIndexMaxPQ_PT xkeyindexmaxpq_new(int(*key_cmp)(void *key1, void *key2, void *cl), int(*value_cmp)(void *value1, void *value2, void *cl), void *cl) {
    return xkeyindexminpq_new(key_cmp, value_cmp, cl);
}

XKeyIndexMaxPQ_PT  xkeyindexmaxpq_copy(XKeyIndexMaxPQ_PT queue) {
    return xkeyindexminpq_copy(queue);
}

bool xkeyindexmaxpq_push(XKeyIndexMaxPQ_PT queue, void *key, void *value, void **old_value) {
    return xkeyindexminpq_push(queue, key, value, old_value);
}

bool xkeyindexmaxpq_pop(XKeyIndexMaxPQ_PT queue, void **key, void **value) {
    xassert(queue);

    if (!queue || (xmap_size(queue->value_map) == 0)) {
        return false;
    }

    {
        void *nkey = NULL;
        void *nvalue = xmap_select(queue->value_map, (xmap_size(queue->value_map) -1));

        xmap_remove_save(queue->value_map, nvalue, &nkey);
        xmap_remove(queue->key_map, nkey);

        if (key) {
            *key = nkey;
        }
        if (value) {
            *value = nvalue;
        }
    }

    return true;
}

bool xkeyindexmaxpq_peek(XKeyIndexMaxPQ_PT queue, void **key, void **value) {
    xassert(queue);

    if (!queue || (xmap_size(queue->value_map) == 0)) {
        return false;
    }

    {
        void *nvalue = xmap_select(queue->value_map, (xmap_size(queue->value_map) - 1));
        void *nkey = xmap_get(queue->value_map, nvalue);

        if (key) {
            *key = nkey;
        }
        if (value) {
            *value = nvalue;
        }

        return true;
    }
}

void* xkeyindexmaxpq_get(XKeyIndexMaxPQ_PT queue, void *key) {
    return xmap_get(queue ? queue->key_map : NULL, key);
}

bool xkeyindexmaxpq_remove(XKeyIndexMaxPQ_PT queue, void *key, void **old_value) {
    return xkeyindexminpq_remove(queue, key, old_value);
}

int xkeyindexmaxpq_map(XKeyIndexMaxPQ_PT queue, bool (*apply)(void *value, void **key, void *cl), void *cl) {
    return xmap_map(queue ? queue->value_map : NULL, apply, cl);
}

bool xkeyindexmaxpq_map_break_if_true(XKeyIndexMaxPQ_PT queue, bool (*apply)(void *value, void **key, void *cl), void *cl) {
    return xmap_map_break_if_true(queue ? queue->value_map : NULL, apply, cl);
}

bool xkeyindexmaxpq_map_break_if_false(XKeyIndexMaxPQ_PT queue, bool (*apply)(void *value, void **key, void *cl), void *cl) {
    return xmap_map_break_if_false(queue ? queue->value_map : NULL, apply, cl);
}

void xkeyindexmaxpq_free(XKeyIndexMaxPQ_PT *pqueue) {
    xkeyindexminpq_free(pqueue);
}

void xkeyindexmaxpq_deep_free(XKeyIndexMaxPQ_PT *pqueue) {
    xkeyindexminpq_deep_free(pqueue);
}

void xkeyindexmaxpq_clear(XKeyIndexMaxPQ_PT queue) {
    xkeyindexminpq_clear(queue);
}

void xkeyindexmaxpq_deep_clear(XKeyIndexMaxPQ_PT queue) {
    xkeyindexminpq_deep_clear(queue);
}

int xkeyindexmaxpq_size(XKeyIndexMaxPQ_PT queue) {
    return xmap_size(queue ? queue->key_map : NULL);
}

bool xkeyindexmaxpq_is_empty(XKeyIndexMaxPQ_PT queue) {
    return xmap_is_empty(queue ? queue->key_map : NULL);
}
