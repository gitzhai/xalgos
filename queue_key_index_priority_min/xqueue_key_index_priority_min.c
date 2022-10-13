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
#include "../tree_map/xtree_map_x.h"
#include "../tree_redblack/xtree_redblack_x.h"
#include "xqueue_key_index_priority_min_x.h"

XKeyIndexMinPQ_PT xkeyindexminpq_new(int(*key_cmp)(void *key1, void *key2, void *cl), int(*value_cmp)(void *value1, void *value2, void *cl), void *cl) {
    xassert(key_cmp);
    xassert(value_cmp);

    if (!key_cmp || !value_cmp) {
        return NULL;
    }

    {
        XKeyIndexMinPQ_PT queue = XMEM_CALLOC(1, sizeof(*queue));
        if (!queue) {
            return NULL;
        }

        queue->key_map = xmap_new(key_cmp, cl);
        if (!queue->key_map) {
            XMEM_FREE(queue);
            return NULL;
        }

        queue->value_map = xmap_new(value_cmp, cl);
        if (!queue->value_map) {
            xmap_free(&queue->key_map);
            XMEM_FREE(queue);
            return NULL;
        }

        return queue;
    }
}

XKeyIndexMinPQ_PT  xkeyindexminpq_copy(XKeyIndexMinPQ_PT queue) {
    xassert(queue);

    if (!queue) {
        return NULL;
    }

    {
        XKeyIndexMinPQ_PT nqueue = XMEM_CALLOC(1, sizeof(*nqueue));
        if (!nqueue) {
            return NULL;
        }

        {
            XMap_PT nkey_map = xmap_copy(queue->key_map);
            if (!nkey_map) {
                XMEM_FREE(nqueue);
                return NULL;
            }

            {
                XMap_PT nvalue_map = xmap_copy(queue->value_map);
                if (!nvalue_map) {
                    xmap_free(&nkey_map);
                    XMEM_FREE(nqueue);
                    return NULL;
                }
            }
        }

        return nqueue;
    }
}

bool xkeyindexminpq_push(XKeyIndexMinPQ_PT queue, void *key, void *value, void **old_value) {
    xassert(queue);
    xassert(key);
    xassert(value);

    if (!queue || !key || !value) {
        return false;
    }

    {
        /* the relation of "key" and "value" must be one to one mapping */
        void *old_key = xmap_get(queue->value_map, value);
        if (old_key) {
            if (queue->key_map->cmp(key, old_key, queue->key_map->cl) != 0) {
                return false;
            }
        }
    }

    {
        void *old_val = NULL;

        if (!xmap_put_replace(queue->key_map, key, value, &old_val)) {
            return false;
        }

        if (old_val) {
            xmap_replace_key(queue->value_map, old_val, value);
        }
        else if (!xmap_put_repeat(queue->value_map, value, key)) {
            xmap_remove(queue->key_map, key);
            return false;
        }

        if (old_value) {
            *old_value = old_val;
        }

        return true;
    }
}

bool xkeyindexminpq_pop(XKeyIndexMinPQ_PT queue, void **key, void **value) {
    xassert(queue);

    if (!queue || (xmap_size(queue->value_map) == 0)) {
        return false;
    }

    {
        void *nkey = NULL;
        void *nvalue = xmap_select(queue->value_map, 0);

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

bool xkeyindexminpq_peek(XKeyIndexMinPQ_PT queue, void **key, void **value) {
    xassert(queue);

    if (!queue || (xmap_size(queue->value_map) == 0)) {
        return false;
    }

    {
        void *nvalue = xmap_select(queue->value_map, 0);
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

void* xkeyindexminpq_get(XKeyIndexMinPQ_PT queue, void *key) {
    return xmap_get(queue ? queue->key_map : NULL, key);
}

bool xkeyindexminpq_remove(XKeyIndexMinPQ_PT queue, void *key, void **old_value) {
    xassert(queue);
    xassert(key);

    if (!queue || !key) {
        return false;
    }

    {
        void *old = NULL;
        xmap_remove_save(queue->key_map, key, &old);

        if (old) {
            xmap_remove(queue->value_map, old);

            if (old_value) {
                *old_value = old;
            }
        }

        return true;
    }
}

int xkeyindexminpq_map(XKeyIndexMinPQ_PT queue, bool (*apply)(void *value, void **key, void *cl), void *cl) {
    return xmap_map(queue ? queue->value_map : NULL, apply, cl);
}

bool xkeyindexminpq_map_break_if_true(XKeyIndexMinPQ_PT queue, bool (*apply)(void *value, void **key, void *cl), void *cl) {
    return xmap_map_break_if_true(queue ? queue->value_map : NULL, apply, cl);
}

bool xkeyindexminpq_map_break_if_false(XKeyIndexMinPQ_PT queue, bool (*apply)(void *value, void **key, void *cl), void *cl) {
    return xmap_map_break_if_false(queue ? queue->value_map : NULL, apply, cl);
}

void xkeyindexminpq_free(XKeyIndexMinPQ_PT *pqueue) {
    if (!pqueue || !*pqueue) {
        return;
    }

    xmap_free(&(*pqueue)->key_map);
    xmap_free(&(*pqueue)->value_map);
    XMEM_FREE(*pqueue);
}

void xkeyindexminpq_deep_free(XKeyIndexMinPQ_PT *pqueue) {
    if (!pqueue || !*pqueue) {
        return;
    }

    xmap_free(&(*pqueue)->key_map);
    xmap_deep_free(&(*pqueue)->value_map);
    XMEM_FREE(*pqueue);
}

void xkeyindexminpq_clear(XKeyIndexMinPQ_PT queue) {
    xmap_clear(queue ? queue->key_map : NULL);
    xmap_clear(queue ? queue->value_map : NULL);
}

void xkeyindexminpq_deep_clear(XKeyIndexMinPQ_PT queue) {
    xmap_clear(queue ? queue->key_map : NULL);
    xmap_deep_clear(queue ? queue->value_map : NULL);
}

int xkeyindexminpq_size(XKeyIndexMinPQ_PT queue) {
    return xmap_size(queue ? queue->key_map : NULL);
}

bool xkeyindexminpq_is_empty(XKeyIndexMinPQ_PT queue) {
    return xmap_is_empty(queue ? queue->key_map : NULL);
}

