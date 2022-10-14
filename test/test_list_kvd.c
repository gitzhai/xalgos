
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../array_pointer/xarray_pointer_x.h"
#include "../list_d_raw/xlist_d_raw_x.h"
#include "../list_d/xlist_d_x.h"
#include "../list_kvd/xlist_kvd_x.h"
#include "../include/xalgos.h"

static
int xkvdlist_test_cmp_true(void *key1, void *key2, void *cl) {
    return 0;
}

static
int xkvdlist_test_cmp_false(void *key1, void *key2, void *cl) {
    return 1;
}

static 
int xkvdlist_test_cmp(void *key1, void *key2, void *cl) {
    return key1 == key2 ? 0 : 1;
}

static
int test_cmp(void *key1, void *key2, void *cl) {
    return strcmp((char*)key1, (char*)key2);
}

static
bool xkvdlist_test_apply_key(void *key, void *cl) {
    return strcmp((char*)key, (char*)cl) == 0;
}

static
bool xkvdlist_test_applykv(void *key, void **value, void *cl) {
    return strcmp((char*)key, (char*)cl) == 0;
}

static
bool xkvdlist_test_applykv2(void *key, void *value, void *cl) {
    return strcmp((char*)key, (char*)cl) == 0;
}

static
bool xkvdlist_test_applyk_true(void *key, void *cl) {
    return true;
}

static
bool xkvdlist_test_applyk_false(void *key, void *cl) {
    return false;
}

static
bool xkvdlist_test_applykv_true(void *key, void **value, void *cl) {
    return true;
}

static
bool xkvdlist_test_applykv_false(void *key, void **value, void *cl) {
    return false;
}

static
bool xkvdlist_test_applykv_true2(void *key, void *value, void *cl) {
    return true;
}

static
bool xkvdlist_test_applykv_false2(void *key, void *value, void *cl) {
    return false;
}

static
bool xkvdlist_test_map_apply_count(void *key, void **value, void *cl) {
    ++(*((int*)(cl)));
    return true;
}

static
bool xkvdlist_test_print(void *key, void *value, void *cl) {
    printf("%s:%s\n", (char*)key, (char*)value);
    return true;
}

static 
XKVDList_PT tkvdlist_random_string(int size) {
    XKVDList_PT dlist = xkvdlist_new();
    if (!dlist) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xkvdlist_deep_free(&dlist);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }
            str[str_size - 1] = '\0';

            xkvdlist_push_back_repeat(dlist, str, NULL);
        }
    }

    return dlist;
}

static
void xkvdlist_test_check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}


void test_xkvdlist() {

        /* xkvdlist_new */
        {
            XKVDList_PT dlist = xkvdlist_new();
            xassert(dlist);
            xassert(dlist->head == NULL);
            xassert(dlist->tail == NULL);
            xassert(dlist->size == 0);
            xkvdlist_free(&dlist);
        }

        /* xkvdlist_vload_repeat */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_vload_repeat(NULL, "k", "v", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
            }
            /* key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                /* first key == NULL */
                xassert(xkvdlist_vload_repeat(dlist, NULL) == 0);
                /* second key == NULL */
                xassert(xkvdlist_vload_repeat(dlist, "k1", "v1", NULL, "v2", NULL) == 1);
                xkvdlist_free(&dlist);
            }

            /* value == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                /* first value == NULL */
                xassert(xkvdlist_vload_repeat(dlist, "k1", NULL) == 0);
                /* second value == NULL */
                xassert(xkvdlist_vload_repeat(dlist, "k1", "v1", "k2", NULL, NULL) == 1);
                xkvdlist_free(&dlist);
            }

            /* normal */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xassert(xkvdlist_vload_repeat(dlist, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL) == 4);
                xassert(strcmp(xpair_second(dlist->head->value), "v1") == 0);
                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_vload_unique */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_vload_unique(NULL, "k", "v", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }
            /* key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                /* first key == NULL */
                xassert(xkvdlist_vload_unique(dlist, NULL) == 0);
                /* second key == NULL */
                xassert(xkvdlist_vload_unique(dlist, "k1", "v1", NULL, "v2", NULL) == 1);
                xkvdlist_free(&dlist);
            }

            /* value == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                /* first value == NULL */
                xassert(xkvdlist_vload_unique(dlist, "k1", NULL) == 0);
                /* second value == NULL */
                xassert(xkvdlist_vload_unique(dlist, "k1", "v1", "k2", NULL, NULL) == 1);
                xkvdlist_free(&dlist);
            }

            /* repeat key */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xassert(xkvdlist_vload_unique(dlist, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL) == 3);
                xassert(strcmp(xpair_second(dlist->head->value), "v1") == 0);
                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_vload_unique_if */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_vload_unique_if(NULL, xkvdlist_test_cmp, NULL, "k", "v", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }
            /* key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                /* first key == NULL */
                xassert(xkvdlist_vload_unique_if(dlist, xkvdlist_test_cmp, NULL, NULL) == 0);
                /* second key == NULL */
                xassert(xkvdlist_vload_unique(dlist, "k1", "v1", NULL, "v2", NULL) == 1);
                xkvdlist_free(&dlist);
            }

            /* value == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                /* first value == NULL */
                xassert(xkvdlist_vload_unique_if(dlist, xkvdlist_test_cmp, NULL, "k1", NULL) == 0);
                /* second value == NULL */
                xassert(xkvdlist_vload_unique_if(dlist, xkvdlist_test_cmp, NULL, "k1", "v1", "k2", NULL, NULL) == 1);
                xkvdlist_free(&dlist);
            }

            /* repeat key */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xassert(xkvdlist_vload_unique_if(dlist, xkvdlist_test_cmp, NULL, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL) == 3);
                xassert(strcmp(xpair_second(dlist->head->value), "v1") == 0);
                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_vload_replace */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_vload_replace(NULL, "k", "v", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }
            /* key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                /* first key == NULL */
                xassert(xkvdlist_vload_replace(dlist, NULL) == 0);
                /* second key == NULL */
                xassert(xkvdlist_vload_replace(dlist, "k1", "v1", NULL, "v2", NULL) == 1);
                xkvdlist_free(&dlist);
            }

            /* value == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                /* first value == NULL */
                xassert(xkvdlist_vload_replace(dlist, "k1", NULL) == 0);
                /* second value == NULL */
                xassert(xkvdlist_vload_replace(dlist, "k1", "v1", "k2", NULL, NULL) == 1);
                xkvdlist_free(&dlist);
            }

            /* repeat key */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xassert(xkvdlist_vload_replace(dlist, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL) == 3);
                xassert(strcmp(xpair_second(dlist->head->value), "v12") == 0);
                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_vload_replace_if */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_vload_replace_if(NULL, xkvdlist_test_cmp, NULL, "k", "v", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
            }
            /* key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                /* first key == NULL */
                xassert(xkvdlist_vload_replace_if(dlist, xkvdlist_test_cmp, NULL, NULL) == 0);
                /* second key == NULL */
                xassert(xkvdlist_vload_replace_if(dlist, xkvdlist_test_cmp, NULL, "k1", "v1", NULL, "v2", NULL) == 1);
                xkvdlist_free(&dlist);
            }

            /* value == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                /* first value == NULL */
                xassert(xkvdlist_vload_replace_if(dlist, xkvdlist_test_cmp, NULL, "k1", NULL) == 0);
                /* second value == NULL */
                xassert(xkvdlist_vload_replace_if(dlist, xkvdlist_test_cmp, NULL, "k1", "v1", "k2", NULL, NULL) == 1);
                xkvdlist_free(&dlist);
            }

            /* repeat key */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xassert(xkvdlist_vload_replace_if(dlist, xkvdlist_test_cmp, NULL, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL) == 3);
                xassert(strcmp(xpair_second(dlist->head->value), "v12") == 0);
                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_aload_repeat */
        {
            /* dlist == NULL */
            {
                XPArray_PT array = xparray_new(10);
                xparray_put(array, 0, "k", NULL);
                xparray_put(array, 1, "v", NULL);
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_aload_repeat(NULL, array);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xparray_free(&array);
            }

            /* xs == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_aload_repeat(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xkvdlist_free(&dlist);
            }
            /* key == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XKVDList_PT dlist = xkvdlist_new();

                /* first key == NULL */
                xassert(xkvdlist_aload_repeat(dlist, array) == 0);

                /* second key == NULL */
                xparray_vload(array, "k1", "v1", NULL);
                xassert(xkvdlist_aload_repeat(dlist, array) == 1);

                xkvdlist_free(&dlist);
                xparray_free(&array);
            }

            /* value == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XKVDList_PT dlist = xkvdlist_new();

                /* first value == NULL */
                xparray_vload(array, "k1", NULL);
                xassert(xkvdlist_aload_repeat(dlist, array) == 0);

                /* second value == NULL */
                xparray_vload(array, "k1", "v1", "k2", NULL);
                xassert(xkvdlist_aload_repeat(dlist, array) == 1);

                xkvdlist_free(&dlist);
                xparray_free(&array);
            }

            /* normal */
            {
                XPArray_PT array = xparray_new(10);
                XKVDList_PT dlist = xkvdlist_new();

                xparray_vload(array, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL);
                xassert(xkvdlist_aload_repeat(dlist, array) == 4);
                xassert(strcmp(xpair_second(dlist->head->value), "v1") == 0);

                xkvdlist_free(&dlist);
                xparray_free(&array);
            }
        }

        /* xkvdlist_aload_unique */
        {
            /* dlist == NULL */
            {
                XPArray_PT array = xparray_new(10);
                xparray_put(array, 0, "k", NULL);
                xparray_put(array, 1, "v", NULL);

                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_aload_unique(NULL, array);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xparray_free(&array);
            }
            /* xs == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_aload_unique(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xkvdlist_free(&dlist);
            }
            /* key == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XKVDList_PT dlist = xkvdlist_new();

                /* first key == NULL */
                xassert(xkvdlist_aload_unique(dlist, array) == 0);

                /* second key == NULL */
                xparray_vload(array, "k1", "v1", NULL);
                xassert(xkvdlist_aload_unique(dlist, array) == 1);

                xkvdlist_free(&dlist);
                xparray_free(&array);
            }

            /* value == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XKVDList_PT dlist = xkvdlist_new();

                /* first value == NULL */
                xparray_vload(array, "k1", NULL);
                xassert(xkvdlist_aload_unique(dlist, array) == 0);

                /* second value == NULL */
                xparray_vload(array, "k1", "v1", "k2", NULL);
                xassert(xkvdlist_aload_unique(dlist, array) == 1);

                xkvdlist_free(&dlist);
                xparray_free(&array);
            }

            /* repeat key */
            {
                XPArray_PT array = xparray_new(10);
                XKVDList_PT dlist = xkvdlist_new();

                xparray_vload(array, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL);
                xassert(xkvdlist_aload_unique(dlist, array) == 3);
                xassert(strcmp(xpair_second(dlist->head->value), "v1") == 0);

                xkvdlist_free(&dlist);
                xparray_free(&array);
            }
        }

        /* xkvdlist_aload_unique_if */
        {
            /* cmp == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XKVDList_PT dlist = xkvdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_aload_unique_if(dlist, array, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvdlist_free(&dlist);
                xparray_free(&array);
            }
            /* below 3 scenarios tested already in xkvdlist_aload_unique */
            /* xs == NULL */
            /* key == NULL */
            /* value == NULL */

            /* repeat key */
            {
                XPArray_PT array = xparray_new(10);
                XKVDList_PT dlist = xkvdlist_new();

                xparray_vload(array, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL);
                xassert(xkvdlist_aload_unique_if(dlist, array, xkvdlist_test_cmp, NULL) == 3);
                xassert(strcmp(xpair_second(dlist->head->value), "v1") == 0);

                xkvdlist_free(&dlist);
                xparray_free(&array);
            }
        }

        /* xkvdlist_aload_replace */
        {
            /* dlist == NULL */
            {
                XPArray_PT array = xparray_new(10);
                xparray_put(array, 0, "k", NULL);
                xparray_put(array, 1, "v", NULL);

                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_aload_replace(NULL, array);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xparray_free(&array);
            }
            /* xs == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_aload_replace(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xkvdlist_free(&dlist);
            }
            /* key == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XKVDList_PT dlist = xkvdlist_new();

                /* first key == NULL */
                xassert(xkvdlist_aload_replace(dlist, array) == 0);

                /* second key == NULL */
                xparray_vload(array, "k1", "v1", NULL);
                xassert(xkvdlist_aload_replace(dlist, array) == 1);

                xkvdlist_free(&dlist);
                xparray_free(&array);
            }

            /* value == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XKVDList_PT dlist = xkvdlist_new();

                /* first value == NULL */
                xparray_vload(array, "k1", NULL);
                xassert(xkvdlist_aload_replace(dlist, array) == 0);

                /* second value == NULL */
                xparray_vload(array, "k1", "v1", "k2", NULL);
                xassert(xkvdlist_aload_replace(dlist, array) == 1);

                xkvdlist_free(&dlist);
                xparray_free(&array);
            }

            /* repeat key */
            {
                XPArray_PT array = xparray_new(10);
                XKVDList_PT dlist = xkvdlist_new();

                xparray_vload(array, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL);
                xassert(xkvdlist_aload_replace(dlist, array) == 3);
                xassert(strcmp(xpair_second(dlist->head->value), "v12") == 0);

                xkvdlist_free(&dlist);
                xparray_free(&array);
            }
        }

        /* xkvdlist_aload_replace_if */
        {
            /* cmp == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XKVDList_PT dlist = xkvdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_aload_replace_if(dlist, array, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);

                xkvdlist_free(&dlist);
                xparray_free(&array);
            }
            /* below 3 scenarios tested already in xkvdlist_aload_replace */
            /* xs == NULL */
            /* key == NULL */
            /* value == NULL */

            /* repeat key */
            {
                XPArray_PT array = xparray_new(10);
                XKVDList_PT dlist = xkvdlist_new();

                xparray_vload(array, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL);
                xassert(xkvdlist_aload_replace_if(dlist, array, xkvdlist_test_cmp, NULL) == 3);
                xassert(strcmp(xpair_second(dlist->head->value), "v12") == 0);

                xkvdlist_free(&dlist);
                xparray_free(&array);
            }
        }

        /* xkvdlist_copyn_if_impl */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_copyn_if_impl(NULL, 0, 0, 1, false, xkvdlist_test_cmp, NULL, NULL, xkvdlist_test_applykv_true2, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* count < 0 */
            {
                XKVDList_PT dlist = xkvdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_copyn_if_impl(dlist, 0, 0, -1, false, xkvdlist_test_cmp, NULL, NULL, xkvdlist_test_applykv_true2, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvdlist_free(&dlist);
            }

            /* deep == true && key_size < 0 */
            {
                XKVDList_PT dlist = xkvdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_copyn_if_impl(dlist, -1, 0, 1, true, xkvdlist_test_cmp, NULL, NULL, xkvdlist_test_applykv_true2, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvdlist_free(&dlist);
            }

            /* deep == true && key_size == 0 */
            {
                XKVDList_PT dlist = xkvdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_copyn_if_impl(dlist, 0, 0, 1, true, xkvdlist_test_cmp, NULL, NULL, xkvdlist_test_applykv_true2, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvdlist_free(&dlist);
            }
            /* dlist is empty */
            {
                XKVDList_PT dlist = xkvdlist_new();

                {
                    XKVDList_PT ndlist = xkvdlist_copyn_if_impl(dlist, 0, 0, 1, false, NULL, NULL, NULL, xkvdlist_test_applykv_true2, NULL);
                    xassert(ndlist->size == 0);
                    xkvdlist_free(&ndlist);
                }

                xkvdlist_free(&dlist);
            }

            /* count == 0 */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xkvdlist_vload_repeat(dlist, "k1", "v1", "k2", "v2", NULL);

                {
                    XKVDList_PT ndlist = xkvdlist_copyn_if_impl(dlist, 0, 0, 0, false, NULL, NULL, NULL, xkvdlist_test_applykv_true2, NULL);
                    xassert(ndlist->size == 0);
                    xkvdlist_free(&ndlist);
                }

                xkvdlist_free(&dlist);
            }

            /* dlist->size < count */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xkvdlist_vload_repeat(dlist, "k1", "v1", "k1", "v2", NULL);

                {
                    XKVDList_PT ndlist = xkvdlist_copyn_if_impl(dlist, 0, 0, 3, false, NULL, NULL, NULL, xkvdlist_test_applykv_true2, NULL);
                    xassert(ndlist->size == 2);
                    xkvdlist_free(&ndlist);
                }

                xkvdlist_free(&dlist);
            }

            /* count < dlist->size */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xkvdlist_vload_repeat(dlist, "k1", "v1", "k1", "v2", NULL);

                {
                    XKVDList_PT ndlist = xkvdlist_copyn_if_impl(dlist, 0, 0, 1, false, NULL, NULL, NULL, xkvdlist_test_applykv_true2, NULL);
                    xassert(ndlist->size == 1);
                    xassert(strcmp(xpair_second(ndlist->head->value), "v1") == 0);
                    xkvdlist_free(&ndlist);
                }

                xkvdlist_free(&dlist);
            }

            /* no any key match */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xkvdlist_vload_repeat(dlist, "k1", "v1", "k2", "v2", NULL);

                {
                    XKVDList_PT ndlist = xkvdlist_copyn_if_impl(dlist, 0, 0, 1, false, NULL, NULL, NULL, xkvdlist_test_applykv_false2, NULL);
                    xassert(ndlist->size == 0);
                    xkvdlist_free(&ndlist);
                }

                xkvdlist_free(&dlist);
            }

            /* normal */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xkvdlist_vload_repeat(dlist, "k1", "v1", "k1", "v2", "k3", "v3", NULL);

                {
                    XKVDList_PT ndlist = xkvdlist_copyn_if_impl(dlist, 0, 0, 2, false, NULL, NULL, NULL, xkvdlist_test_applykv_true2, NULL);
                    xassert(ndlist->size == 2);
                    xassert(strcmp(xpair_second(ndlist->head->value), "v1") == 0);
                    xassert(strcmp(xpair_second(ndlist->head->next->value), "v2") == 0);
                    xkvdlist_free(&ndlist);
                }

                {
                    XKVDList_PT ndlist = xkvdlist_copyn_if_impl(dlist, 0, 0, 2, false, NULL, NULL, NULL, xkvdlist_test_applykv2, "k3");
                    xassert(ndlist->size == 1);
                    xassert(strcmp(xpair_first(ndlist->head->value), "k3") == 0);
                    xassert(strcmp(xpair_second(ndlist->head->value), "v3") == 0);
                    xkvdlist_free(&ndlist);
                }

                {
                    XKVDList_PT ndlist = xkvdlist_copyn_if_impl(dlist, 0, 0, 2, false, test_cmp, NULL, NULL, NULL, "k3");
                    xassert(ndlist->size == 1);
                    xassert(strcmp(xpair_first(ndlist->head->value), "k3") == 0);
                    xassert(strcmp(xpair_second(ndlist->head->value), "v3") == 0);
                    xkvdlist_free(&ndlist);
                }

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_copyn_equal_if */
        {
            /* key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_copyn_equal_if(dlist, 1, NULL, xkvdlist_test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_deep_copyn_equal_if */
        {
            /* key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_deep_copyn_equal_if(dlist, 1, 1, 1, NULL, xkvdlist_test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);

                xkvdlist_free(&dlist);
            }
        }

        /* all below copy functions tested by xkvdlist_copyn_if_impl already */
        /* xkvdlist_copy */
        /* xkvdlist_copy_equal_if */
        /* xkvdlist_copy_apply_if */
        /* xkvdlist_copy_apply_key_if */
        /* xkvdlist_copyn */
        /* xkvdlist_copyn_equal_if */
        /* xkvdlist_copyn_apply_if */
        /* xkvdlist_copyn_apply_key_if */
        /* xkvdlist_deep_copy */
        /* xkvdlist_deep_copy_equal_if */
        /* xkvdlist_deep_copy_apply_if */
        /* xkvdlist_deep_copy_apply_key_if */
        /* xkvdlist_deep_copyn */
        /* xkvdlist_deep_copyn_equal_if */
        /* xkvdlist_deep_copyn_apply_if */
        /* xkvdlist_deep_copyn_apply_key_if */


        /* xkvdlist_push_front_repeat */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_push_front_repeat(NULL, "k", "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_push_front_repeat(dlist, NULL, "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvdlist_free(&dlist);
            }
            /* normal */
            {
                XKVDList_PT dlist = xkvdlist_new();

                char *str = "key1";

                xkvdlist_push_front_repeat(dlist, str, "value1");
                xkvdlist_push_front_repeat(dlist, "key2", "value2");
                xkvdlist_push_front_repeat(dlist, str, "value12");
                xkvdlist_push_front_repeat(dlist, "key3", "value3");

                xassert(strcmp(xpair_first(dlist->head->value), "key3") == 0);
                xassert(strcmp(xpair_second(dlist->head->value), "value3") == 0);
                xassert(strcmp(xpair_first(dlist->head->next->next->value), "key2") == 0);
                xassert(strcmp(xpair_second(dlist->head->next->next->value), "value2") == 0);
                xassert(strcmp(xpair_first(dlist->tail->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->tail->value), "value1") == 0);

                xassert(dlist->size == 4);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_pop_front */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_pop_front(NULL, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
            }
            /* dlist is empty */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xassert(xkvdlist_pop_front(dlist, NULL, NULL) == false);
                xkvdlist_free(&dlist);
            }

            {
                XKVDList_PT dlist = xkvdlist_new();

                xkvdlist_push_back_repeat(dlist, "key1", "value1");
                xkvdlist_push_back_repeat(dlist, "key2", "value2");
                xkvdlist_push_back_repeat(dlist, "key3", "value3");

                {
                    void *key = NULL;
                    void *value = NULL;

                    xassert(xkvdlist_pop_front(dlist, &key, &value));
                    xassert(strcmp(key, "key1") == 0);
                    xassert(strcmp(value, "value1") == 0);

                    xassert(xkvdlist_pop_front(dlist, &key, NULL));
                    xassert(strcmp(key, "key2") == 0);

                    xassert(xkvdlist_pop_front(dlist, NULL, &value));
                    xassert(strcmp(value, "value3") == 0);

                    xassert(dlist->size == 0);
                    xassert(dlist->head == NULL);
                    xassert(dlist->tail == NULL);
                }

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_front */
        {
            /* dlist is empty */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xassert_false(xkvdlist_front(dlist));
                xkvdlist_free(&dlist);
            }

            /* normal */
            {
                XKVDList_PT dlist = xkvdlist_new();

                xkvdlist_push_back_repeat(dlist, "key1", "value1");
                xkvdlist_push_back_repeat(dlist, "key2", "value2");
                xkvdlist_push_back_repeat(dlist, "key3", "value3");

                xassert(strcmp(xpair_first(xkvdlist_front(dlist)), "key1") == 0);
                xassert(strcmp(xpair_second(xkvdlist_front(dlist)), "value1") == 0);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_back */
        {
            /* dlist is empty */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xassert_false(xkvdlist_back(dlist));
                xkvdlist_free(&dlist);
            }

            /* normal */
            {
                XKVDList_PT dlist = xkvdlist_new();

                xkvdlist_push_back_repeat(dlist, "key1", "value1");
                xkvdlist_push_back_repeat(dlist, "key2", "value2");
                xkvdlist_push_back_repeat(dlist, "key3", "value3");

                xassert(strcmp(xpair_first(xkvdlist_back(dlist)), "key3") == 0);
                xassert(strcmp(xpair_second(xkvdlist_back(dlist)), "value3") == 0);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_front_node */
        {
            /* dlist is empty */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xassert_false(xkvdlist_front_node(dlist));
                xkvdlist_free(&dlist);
            }

            /* normal */
            {
                XKVDList_PT dlist = xkvdlist_new();

                xkvdlist_push_back_repeat(dlist, "key1", "value1");
                xkvdlist_push_back_repeat(dlist, "key2", "value2");
                xkvdlist_push_back_repeat(dlist, "key3", "value3");

                xassert(strcmp(xpair_first(xkvdlist_front_node(dlist)->value), "key1") == 0);
                xassert(strcmp(xpair_second(xkvdlist_front_node(dlist)->value), "value1") == 0);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_back_node */
        {
            /* dlist is empty */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xassert_false(xkvdlist_back_node(dlist));
                xkvdlist_free(&dlist);
            }

            /* normal */
            {
                XKVDList_PT dlist = xkvdlist_new();

                xkvdlist_push_back_repeat(dlist, "key1", "value1");
                xkvdlist_push_back_repeat(dlist, "key2", "value2");
                xkvdlist_push_back_repeat(dlist, "key3", "value3");

                xassert(strcmp(xpair_first(xkvdlist_back_node(dlist)->value), "key3") == 0);
                xassert(strcmp(xpair_second(xkvdlist_back_node(dlist)->value), "value3") == 0);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_push_back_repeat */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_push_back_repeat(NULL, "k", "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_push_back_repeat(dlist, NULL, "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvdlist_free(&dlist);
            }
            /* normal */
            {
                XKVDList_PT dlist = xkvdlist_new();

                char *str = "key1";

                xkvdlist_push_back_repeat(dlist, str, "value1");
                xkvdlist_push_back_repeat(dlist, "key2", "value2");
                xkvdlist_push_back_repeat(dlist, str, "value12");
                xkvdlist_push_back_repeat(dlist, "key3", "value3");

                xassert(strcmp(xpair_first(dlist->head->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->head->value), "value1") == 0);
                xassert(strcmp(xpair_first(dlist->head->next->next->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->head->next->next->value), "value12") == 0);
                xassert(strcmp(xpair_first(dlist->tail->value), "key3") == 0);
                xassert(strcmp(xpair_second(dlist->tail->value), "value3") == 0);

                xassert(dlist->size == 4);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_pop_back */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_pop_back(NULL, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }
            /* dlist is empty */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xassert(xkvdlist_pop_back(dlist, NULL, NULL) == false);
                xkvdlist_free(&dlist);
            }

            {
                XKVDList_PT dlist = xkvdlist_new();

                xkvdlist_push_back_repeat(dlist, "key1", "value1");
                xkvdlist_push_back_repeat(dlist, "key2", "value2");
                xkvdlist_push_back_repeat(dlist, "key3", "value3");

                {
                    void *key = NULL;
                    void *value = NULL;

                    xassert(xkvdlist_pop_back(dlist, &key, &value));
                    xassert(strcmp(key, "key3") == 0);
                    xassert(strcmp(value, "value3") == 0);

                    xassert(xkvdlist_pop_back(dlist, &key, NULL));
                    xassert(strcmp(key, "key2") == 0);

                    xassert(xkvdlist_pop_back(dlist, NULL, &value));
                    xassert(strcmp(value, "value1") == 0);

                    xassert(dlist->size == 0);
                    xassert(dlist->head == NULL);
                    xassert(dlist->tail == NULL);
                }

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_push_front_unique */
        {
            XKVDList_PT dlist = xkvdlist_new();

            char *str = "key1"; 

            xkvdlist_push_front_unique(dlist, str, "value1");
            xkvdlist_push_front_unique(dlist, "key2", "value2");
            xkvdlist_push_front_unique(dlist, str, "value12");
            xkvdlist_push_front_unique(dlist, "key3", "value3");

            xassert(strcmp(xpair_first(dlist->head->value), "key3") == 0);
            xassert(strcmp(xpair_second(dlist->head->value), "value3") == 0);
            xassert(strcmp(xpair_first(dlist->head->next->next->value), "key1") == 0);
            xassert(strcmp(xpair_second(dlist->head->next->next->value), "value1") == 0);
            xassert(strcmp(xpair_first(dlist->tail->value), "key1") == 0);
            xassert(strcmp(xpair_second(dlist->tail->value), "value1") == 0);
            xassert(dlist->size == 3);

            xkvdlist_free(&dlist);
        }

        /* xkvdlist_push_front_unique_if */
        {
            XKVDList_PT dlist = xkvdlist_new();

            xkvdlist_push_front_unique_if(dlist, "key1", "value1", test_cmp, NULL);
            xkvdlist_push_front_unique_if(dlist, "key2", "value2", test_cmp, NULL);
            xkvdlist_push_front_unique_if(dlist, "key1", "value12", test_cmp, NULL);
            xkvdlist_push_front_unique_if(dlist, "key3", "value3", test_cmp, NULL);

            xassert(strcmp(xpair_first(dlist->head->value), "key3") == 0);
            xassert(strcmp(xpair_second(dlist->head->value), "value3") == 0);
            xassert(strcmp(xpair_first(dlist->head->next->next->value), "key1") == 0);
            xassert(strcmp(xpair_second(dlist->head->next->next->value), "value1") == 0);
            xassert(strcmp(xpair_first(dlist->tail->value), "key1") == 0);
            xassert(strcmp(xpair_second(dlist->tail->value), "value1") == 0);
            xassert(dlist->size == 3);

            xkvdlist_free(&dlist);
        }

        /* xkvdlist_push_front_replace */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_push_front_replace(NULL, "k", "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_push_front_replace(dlist, NULL, "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvdlist_free(&dlist);
            }
            /* repeat key */
            {
                XKVDList_PT dlist = xkvdlist_new();

                char *str1 = "key1";

                xkvdlist_push_front_replace(dlist, str1, "value1");
                xkvdlist_push_front_replace(dlist, "key2", "value2");
                xkvdlist_push_front_replace(dlist, str1, "value12");
                xkvdlist_push_front_replace(dlist, "key3", "value3");

                xassert(strcmp(xpair_first(dlist->head->value), "key3") == 0);
                xassert(strcmp(xpair_second(dlist->head->value), "value3") == 0);
                xassert(strcmp(xpair_first(dlist->head->next->next->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->head->next->next->value), "value12") == 0);
                xassert(strcmp(xpair_first(dlist->tail->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->tail->value), "value12") == 0);
                xassert(dlist->size == 3);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_push_front_deep_replace */
        {
            /* below scenarios tested by xkvdlist_push_front_replace already */
            /* dlist == NULL */
            /* key == NULL */

            /* repeat key */
            {
                XKVDList_PT dlist = xkvdlist_new();

                char *str1 = "key1";
                char *str2 = XMEM_CALLOC(1, 7);
                memcpy(str2, "value1", 6);

                xkvdlist_push_front_deep_replace(dlist, str1, str2);
                xkvdlist_push_front_deep_replace(dlist, "key2", "value2");
                xkvdlist_push_front_deep_replace(dlist, str1, "value12");
                xkvdlist_push_front_deep_replace(dlist, "key3", "value3");

                xassert(strcmp(xpair_first(dlist->head->value), "key3") == 0);
                xassert(strcmp(xpair_second(dlist->head->value), "value3") == 0);
                xassert(strcmp(xpair_first(dlist->head->next->next->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->head->next->next->value), "value12") == 0);
                xassert(strcmp(xpair_first(dlist->tail->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->tail->value), "value12") == 0);
                xassert(dlist->size == 3);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_push_front_replace_if */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_push_front_replace_if(NULL, "k", "v", test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
            }

            /* key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_push_front_replace_if(dlist, NULL, "v", test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvdlist_free(&dlist);
            }

            /* repeat key */
            {
                XKVDList_PT dlist = xkvdlist_new();

                xkvdlist_push_front_replace_if(dlist, "key1", "value1", test_cmp, NULL);
                xkvdlist_push_front_replace_if(dlist, "key2", "value2", test_cmp, NULL);
                xkvdlist_push_front_replace_if(dlist, "key1", "value12", test_cmp, NULL);
                xkvdlist_push_front_replace_if(dlist, "key3", "value3", test_cmp, NULL);

                xassert(strcmp(xpair_first(dlist->head->value), "key3") == 0);
                xassert(strcmp(xpair_second(dlist->head->value), "value3") == 0);
                xassert(strcmp(xpair_first(dlist->head->next->next->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->head->next->next->value), "value12") == 0);
                xassert(strcmp(xpair_first(dlist->tail->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->tail->value), "value12") == 0);
                xassert(dlist->size == 3);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_push_front_deep_replace_if */
        {
            /* below scenarios tested by xkvdlist_push_front_replace_if already */
            /* dlist == NULL */
            /* key == NULL */
            /* cmp == NULL */

            /* repeat key */
            {
                XKVDList_PT dlist = xkvdlist_new();

                char *key = XMEM_CALLOC(1, 7);
                char *value = XMEM_CALLOC(1, 7);
                memcpy(key, "key1", 4);
                memcpy(value, "value1", 6);

                xkvdlist_push_front_deep_replace_if(dlist, key, value, test_cmp, NULL);
                xkvdlist_push_front_deep_replace_if(dlist, "key2", "value2", test_cmp, NULL);
                xkvdlist_push_front_deep_replace_if(dlist, "key1", "value12", test_cmp, NULL);
                xkvdlist_push_front_deep_replace_if(dlist, "key3", "value3", test_cmp, NULL);

                xassert(strcmp(xpair_first(dlist->head->value), "key3") == 0);
                xassert(strcmp(xpair_second(dlist->head->value), "value3") == 0);
                xassert(strcmp(xpair_first(dlist->head->next->next->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->head->next->next->value), "value12") == 0);
                xassert(strcmp(xpair_first(dlist->tail->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->tail->value), "value12") == 0);
                xassert(dlist->size == 3);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_push_back_unique */
        {
            XKVDList_PT dlist = xkvdlist_new();

            char *str = "key1";

            xkvdlist_push_back_unique(dlist, str, "value1");
            xkvdlist_push_back_unique(dlist, "key2", "value2");
            xkvdlist_push_back_unique(dlist, str, "value12");
            xkvdlist_push_back_unique(dlist, "key3", "value3");

            xassert(strcmp(xpair_first(dlist->head->value), "key1") == 0);
            xassert(strcmp(xpair_second(dlist->head->value), "value1") == 0);
            xassert(strcmp(xpair_first(dlist->head->next->next->value), "key3") == 0);
            xassert(strcmp(xpair_second(dlist->head->next->next->value), "value3") == 0);
            xassert(strcmp(xpair_first(dlist->tail->value), "key3") == 0);
            xassert(strcmp(xpair_second(dlist->tail->value), "value3") == 0);
            xassert(dlist->size == 3);

            xkvdlist_free(&dlist);
        }

        /* xkvdlist_push_back_unique_if */
        {
            XKVDList_PT dlist = xkvdlist_new();

            xkvdlist_push_back_unique_if(dlist, "key1", "value1", test_cmp, NULL);
            xkvdlist_push_back_unique_if(dlist, "key2", "value2", test_cmp, NULL);
            xkvdlist_push_back_unique_if(dlist, "key1", "value12", test_cmp, NULL);
            xkvdlist_push_back_unique_if(dlist, "key3", "value3", test_cmp, NULL);

            xassert(strcmp(xpair_first(dlist->head->value), "key1") == 0);
            xassert(strcmp(xpair_second(dlist->head->value), "value1") == 0);
            xassert(strcmp(xpair_first(dlist->head->next->next->value), "key3") == 0);
            xassert(strcmp(xpair_second(dlist->head->next->next->value), "value3") == 0);
            xassert(strcmp(xpair_first(dlist->tail->value), "key3") == 0);
            xassert(strcmp(xpair_second(dlist->tail->value), "value3") == 0);
            xassert(dlist->size == 3);

            xkvdlist_free(&dlist);
        }

        /* xkvdlist_push_back_replace */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_push_back_replace(NULL, "k", "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
            }

            /* key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_push_back_replace(dlist, NULL, "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvdlist_free(&dlist);
            }
            /* repeat key */
            {
                XKVDList_PT dlist = xkvdlist_new();

                char *str1 = "key1";

                xkvdlist_push_back_replace(dlist, str1, "value1");
                xkvdlist_push_back_replace(dlist, "key2", "value2");
                xkvdlist_push_back_replace(dlist, str1, "value12");
                xkvdlist_push_back_replace(dlist, "key3", "value3");

                xassert(strcmp(xpair_first(dlist->head->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->head->value), "value12") == 0);
                xassert(strcmp(xpair_first(dlist->head->next->next->value), "key3") == 0);
                xassert(strcmp(xpair_second(dlist->head->next->next->value), "value3") == 0);
                xassert(strcmp(xpair_first(dlist->tail->value), "key3") == 0);
                xassert(strcmp(xpair_second(dlist->tail->value), "value3") == 0);
                xassert(dlist->size == 3);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_push_back_deep_replace */
        {
            /* below scenarios tested by xkvdlist_push_back_replace already */
            /* dlist == NULL */
            /* key == NULL */

            /* repeat key */
            {
                XKVDList_PT dlist = xkvdlist_new();

                char *str1 = "key1";
                char *str2 = XMEM_CALLOC(1, 7);
                memcpy(str2, "value1", 6);

                xkvdlist_push_back_deep_replace(dlist, str1, str2);
                xkvdlist_push_back_deep_replace(dlist, "key2", "value2");
                xkvdlist_push_back_deep_replace(dlist, str1, "value12");
                xkvdlist_push_back_deep_replace(dlist, "key3", "value3");

                xassert(strcmp(xpair_first(dlist->head->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->head->value), "value12") == 0);
                xassert(strcmp(xpair_first(dlist->head->next->next->value), "key3") == 0);
                xassert(strcmp(xpair_second(dlist->head->next->next->value), "value3") == 0);
                xassert(strcmp(xpair_first(dlist->tail->value), "key3") == 0);
                xassert(strcmp(xpair_second(dlist->tail->value), "value3") == 0);
                xassert(dlist->size == 3);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_push_back_replace_if */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_push_back_replace_if(NULL, "k", "v", test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_push_back_replace_if(dlist, NULL, "v", test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvdlist_free(&dlist);
            }

            /* repeat key */
            {
                XKVDList_PT dlist = xkvdlist_new();

                xkvdlist_push_back_replace_if(dlist, "key1", "value1", test_cmp, NULL);
                xkvdlist_push_back_replace_if(dlist, "key2", "value2", test_cmp, NULL);
                xkvdlist_push_back_replace_if(dlist, "key1", "value12", test_cmp, NULL);
                xkvdlist_push_back_replace_if(dlist, "key3", "value3", test_cmp, NULL);

                xassert(strcmp(xpair_first(dlist->head->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->head->value), "value12") == 0);
                xassert(strcmp(xpair_first(dlist->head->next->next->value), "key3") == 0);
                xassert(strcmp(xpair_second(dlist->head->next->next->value), "value3") == 0);
                xassert(strcmp(xpair_first(dlist->tail->value), "key3") == 0);
                xassert(strcmp(xpair_second(dlist->tail->value), "value3") == 0);
                xassert(dlist->size == 3);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_push_back_deep_replace_if */
        {
            /* below scenarios tested by xkvdlist_push_back_replace_if already */
            /* dlist == NULL */
            /* key == NULL */
            /* cmp == NULL */

            /* repeat key */
            {
                XKVDList_PT dlist = xkvdlist_new();

                char *key = XMEM_CALLOC(1, 7);
                char *value = XMEM_CALLOC(1, 7);
                memcpy(key, "key1", 4);
                memcpy(value, "value1", 6);

                xkvdlist_push_back_deep_replace_if(dlist, key, value, test_cmp, NULL);
                xkvdlist_push_back_deep_replace_if(dlist, "key2", "value2", test_cmp, NULL);
                xkvdlist_push_back_deep_replace_if(dlist, "key1", "value12", test_cmp, NULL);
                xkvdlist_push_back_deep_replace_if(dlist, "key3", "value3", test_cmp, NULL);

                xassert(strcmp(xpair_first(dlist->head->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->head->value), "value12") == 0);
                xassert(strcmp(xpair_first(dlist->head->next->next->value), "key3") == 0);
                xassert(strcmp(xpair_second(dlist->head->next->next->value), "value3") == 0);
                xassert(strcmp(xpair_first(dlist->tail->value), "key3") == 0);
                xassert(strcmp(xpair_second(dlist->tail->value), "value3") == 0);
                xassert(dlist->size == 3);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_insert_before */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_insert_before(NULL, NULL, "k", "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_insert_before(dlist, NULL, NULL, "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvdlist_free(&dlist);
            }
            {
                XKVDList_PT dlist = xkvdlist_new();

                xkvdlist_push_back_repeat(dlist, "key1", "value1");
                xkvdlist_push_back_repeat(dlist, "key2", "value2");
                xkvdlist_push_back_repeat(dlist, "key3", "value3");

                {
                    xkvdlist_insert_before(dlist, NULL, "key2", "value22");
                    /* key2, value22
                     * key1, value1
                     * key2, value2
                     * key3, value3
                     */
                    xassert(strcmp(xpair_first(dlist->head->value), "key2") == 0);
                    xassert(strcmp(xpair_second(dlist->head->value), "value22") == 0);
                }

                {
                    XRDList_PT node = xkvdlist_front_node(dlist)->next->next;
                    xkvdlist_insert_before(dlist, node, "key2", "value23");
                    /*
                     * key2, value22
                     * key1, value1
                     * key2, value23
                     * key2, value2
                     * key3, value3
                     */
                    xassert(strcmp(xpair_first(dlist->head->next->next->value), "key2") == 0);
                    xassert(strcmp(xpair_second(dlist->head->next->next->value), "value23") == 0);
                }

                xassert(strcmp(xpair_first(dlist->tail->value), "key3") == 0);
                xassert(strcmp(xpair_second(dlist->tail->value), "value3") == 0);
                xassert(dlist->size == 5);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_insert_after */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_insert_after(NULL, NULL, "k", "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
            }

            /* key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_insert_after(dlist, NULL, NULL, "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);

                xkvdlist_free(&dlist);
            }
            {
                XKVDList_PT dlist = xkvdlist_new();

                xkvdlist_push_back_repeat(dlist, "key1", "value1");
                xkvdlist_push_back_repeat(dlist, "key2", "value2");
                xkvdlist_push_back_repeat(dlist, "key3", "value3");

                {
                    xkvdlist_insert_after(dlist, NULL, "key2", "value22");
                    /*
                     *  key1, value1
                     *  key2, value2
                     *  key3, value3
                     *  key2, value22
                     */
                    xassert(strcmp(xpair_first(dlist->tail->value), "key2") == 0);
                    xassert(strcmp(xpair_second(dlist->tail->value), "value22") == 0);
                }

                {
                    XRDList_PT node = xkvdlist_front_node(dlist)->next->next;
                    xkvdlist_insert_after(dlist, node, "key2", "value23");
                    /*
                    * key1, value1
                    * key2, value2
                    * key3, value3
                    * key2, value22
                    * key2, value23
                    */
                    xassert(strcmp(xpair_first(dlist->head->next->next->next->value), "key2") == 0);
                    xassert(strcmp(xpair_second(dlist->head->next->next->next->value), "value23") == 0);
                }

                xassert(strcmp(xpair_first(dlist->head->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->head->value), "value1") == 0);
                xassert(dlist->size == 5);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_remove_before */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_remove_before(NULL, NULL, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
            }
            /* dlist is empty */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xassert_false(xkvdlist_remove_before(dlist, NULL, NULL, NULL));
                xkvdlist_free(&dlist);
            }

            /* node is NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xassert_false(xkvdlist_remove_before(dlist, NULL, NULL, NULL));
                xkvdlist_free(&dlist);
            }

            /* node->prev is NULl */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xkvdlist_vload_repeat(dlist, "k", "v", NULL);
                XRDList_PT node = xkvdlist_front_node(dlist);

                xassert_false(xkvdlist_remove_before(dlist, node, NULL, NULL));
                xkvdlist_free(&dlist);
            }

            {
                XKVDList_PT dlist = xkvdlist_new();

                void *key = NULL;
                void *value = NULL;

                xassert_false(xkvdlist_remove_before(dlist, NULL, &key, &value));

                xkvdlist_push_back_repeat(dlist, "key1", "value1");
                xkvdlist_push_back_repeat(dlist, "key2", "value2");
                xkvdlist_push_back_repeat(dlist, "key3", "value3");
                xkvdlist_push_back_repeat(dlist, "key4", "value4");
                xkvdlist_push_back_repeat(dlist, "key5", "value5");

                {
                    XRDList_PT node = xkvdlist_front_node(dlist)->next->next;
                    xkvdlist_remove_before(dlist, node, &key, &value);
                    /*
                    *  key1, value1
                    *  key3, value3
                    *  key4, value4
                    *  key5, value5
                    */
                    xassert(strcmp(key, "key2") == 0);
                    xassert(strcmp(value, "value2") == 0);
                }

                {
                    XRDList_PT node = xkvdlist_front_node(dlist)->next;
                    xkvdlist_remove_before(dlist, node, &key, &value);
                    /*
                    *  key3, value3
                    *  key4, value4
                    *  key5, value5
                    */
                    xassert(strcmp(key, "key1") == 0);
                    xassert(strcmp(value, "value1") == 0);
                }

                xassert(strcmp(xpair_first(dlist->head->value), "key3") == 0);
                xassert(strcmp(xpair_second(dlist->head->value), "value3") == 0);
                xassert(strcmp(xpair_first(dlist->tail->value), "key5") == 0);
                xassert(strcmp(xpair_second(dlist->tail->value), "value5") == 0);
                xassert(dlist->size == 3);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_remove_after */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_remove_after(NULL, NULL, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }
            /* dlist is empty */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xassert_false(xkvdlist_remove_after(dlist, dlist->head, NULL, NULL));
                xkvdlist_free(&dlist);
            }

            /* node is NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xassert_false(xkvdlist_remove_after(dlist, dlist->head, NULL, NULL));
                xkvdlist_free(&dlist);
            }

            /* node->next is NULl */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xkvdlist_vload_repeat(dlist, "k", "v", NULL);

                xassert_false(xkvdlist_remove_after(dlist, dlist->tail, NULL, NULL));
                xkvdlist_free(&dlist);
            }

            {
                XKVDList_PT dlist = xkvdlist_new();

                void *key = NULL;
                void *value = NULL;

                xkvdlist_push_back_repeat(dlist, "key1", "value1");
                xkvdlist_push_back_repeat(dlist, "key2", "value2");
                xkvdlist_push_back_repeat(dlist, "key3", "value3");
                xkvdlist_push_back_repeat(dlist, "key4", "value4");
                xkvdlist_push_back_repeat(dlist, "key5", "value5");

                {
                    XRDList_PT node = xkvdlist_back_node(dlist)->prev->prev;
                    xkvdlist_remove_after(dlist, node, &key, &value);
                    /*
                    *  key1, value1
                    *  key2, value2
                    *  key3, value3
                    *  key5, value5
                    */
                    xassert(strcmp(key, "key4") == 0);
                    xassert(strcmp(value, "value4") == 0);
                }

                {
                    XRDList_PT node = xkvdlist_back_node(dlist)->prev;
                    xkvdlist_remove_after(dlist, node, &key, &value);
                    /*
                    *  key1, value1
                    *  key2, value2
                    *  key3, value3
                    */
                    xassert(strcmp(key, "key5") == 0);
                    xassert(strcmp(value, "value5") == 0);
                }

                xassert(strcmp(xpair_first(dlist->head->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->head->value), "value1") == 0);
                xassert(strcmp(xpair_first(dlist->tail->value), "key3") == 0);
                xassert(strcmp(xpair_second(dlist->tail->value), "value3") == 0);
                xassert(dlist->size == 3);

                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_remove_if_impl */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_remove_if_impl(NULL, false, false, NULL, NULL, NULL, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* dlist is empty */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xassert(xkvdlist_remove_if_impl(dlist, false, false, xkvdlist_test_cmp_false, NULL, NULL, NULL, NULL) == 0);
                xkvdlist_free(&dlist);
            }

            /* no any key match */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xkvdlist_push_back_repeat(dlist, "key1", "value1");
                xkvdlist_push_back_repeat(dlist, "key2", "value2");
                xkvdlist_push_back_repeat(dlist, "key1", "value12");

                xassert(xkvdlist_remove_if_impl(dlist, false, false, xkvdlist_test_cmp_false, NULL, NULL, NULL, NULL) == 0);
                xassert(xkvdlist_remove_if_impl(dlist, false, false, NULL, NULL, NULL, xkvdlist_test_applykv_false, NULL) == 0);

                xassert(xkvdlist_remove_if_impl(dlist, false, false, test_cmp, NULL, NULL, NULL, "key3") == 0);
                xassert(xkvdlist_remove_if_impl(dlist, false, false, NULL, NULL, NULL, xkvdlist_test_applykv, "key3") == 0);

                xkvdlist_free(&dlist);
            }

            /* remove one */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xkvdlist_push_back_repeat(dlist, "key1", "value1");
                xkvdlist_push_back_repeat(dlist, "key1", "value12");
                xkvdlist_push_back_repeat(dlist, "key1", "value13");
                xkvdlist_push_back_repeat(dlist, "key1", "value14");

                xassert(xkvdlist_remove_if_impl(dlist, false, true, test_cmp, NULL, NULL, NULL, "key1") == 1);
                xassert(strcmp(xpair_second(dlist->head->value), "value12") == 0);
                xassert(xkvdlist_remove_if_impl(dlist, false, true, NULL, NULL, NULL, xkvdlist_test_applykv, "key1") == 1);
                xassert(strcmp(xpair_second(dlist->head->value), "value13") == 0);

                xkvdlist_free(&dlist);
            }

            /* remove all */
            {
                XKVDList_PT dlist = xkvdlist_new();
                xkvdlist_push_back_repeat(dlist, "key1", "value1");
                xkvdlist_push_back_repeat(dlist, "key2", "value2");
                xkvdlist_push_back_repeat(dlist, "key1", "value12");
                xkvdlist_push_back_repeat(dlist, "key1", "value13");
                xkvdlist_push_back_repeat(dlist, "key2", "value2");
                xkvdlist_push_back_repeat(dlist, "key1", "value14");

                xassert(xkvdlist_remove_if_impl(dlist, false, false, test_cmp, NULL, NULL, NULL, "key1") == 4);
                xassert(xkvdlist_find(dlist, "key1") == NULL);
                xassert(xkvdlist_remove_if_impl(dlist, false, false, NULL, NULL, NULL, xkvdlist_test_applykv, "key2") == 2);
                xassert(xkvdlist_find(dlist, "key2") == NULL);
                xassert(dlist->size == 0);

                xkvdlist_free(&dlist);
            }

            /* deep remove */
            {
                XKVDList_PT dlist = xkvdlist_new();
                char *key1 = XMEM_CALLOC(1, 10);
                char *value1 = XMEM_CALLOC(1, 10);
                char *key2 = XMEM_CALLOC(1, 10);
                char *value2 = XMEM_CALLOC(1, 10);
                char *key3 = XMEM_CALLOC(1, 10);
                char *value3 = XMEM_CALLOC(1, 10);

                memcpy(key1, "key1", 4);
                memcpy(value1, "value1", 6);
                memcpy(key2, "key2", 4);
                memcpy(value2, "value2", 6);
                memcpy(key3, "key1", 4);
                memcpy(value3, "value12", 6);

                xkvdlist_push_back_repeat(dlist, key1, value1);
                xkvdlist_push_back_repeat(dlist, key2, value2);
                xkvdlist_push_back_repeat(dlist, key3, value3);

                xassert(xkvdlist_remove_if_impl(dlist, true, false, test_cmp, NULL, NULL, NULL, "key1") == 2);
                xassert(dlist->size == 1);

                xkvdlist_deep_free(&dlist);
            }
        }

        /* all below remove functions tested by xkvdlist_remove_if_impl already */

        /* xkvdlist_remove */
        {
            /* key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_remove(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_remove_all */
        {
            /* key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_remove_all(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_deep_remove */
        {
            /* key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_deep_remove(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xkvdlist_free(&dlist);
            }

            /* *key == NULL */
            {
                XKVDList_PT dlist = xkvdlist_new();
                void *str = NULL;
                void **key = &str;

                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_deep_remove(dlist, key);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xkvdlist_free(&dlist);
            }
        }

        /* xkvdlist_remove_apply_if */
        /* xkvdlist_remove_apply_key_if */
        /* xkvdlist_remove_equal_if */
        /* xkvdlist_remove_apply_break_if */
        /* xkvdlist_remove_apply_key_break_if */
        /* xkvdlist_remove_equal_break_if */
        /* xkvdlist_deep_remove_apply_if */
        /* xkvdlist_deep_remove_apply_key_if */
        /* xkvdlist_deep_remove_equal_if */
        /* xkvdlist_deep_remove_apply_break_if */
        /* xkvdlist_deep_remove_apply_key_break_if */
        /* xkvdlist_deep_remove_equal_break_if */

        /* below two free functions tested by other functions already */
        /* xkvdlist_free */
        /* xkvdlist_deep_free */


        /* xkvdlist_clear */
        {
        }

        /* xkvdlist_deep_clear */
        {
        }

        /* xkvdlist_swap */
        {
            /* dlist1 == NULL */
            {
                XKVDList_PT dlist2 = xkvdlist_new();
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_swap(NULL, dlist2);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xkvdlist_free(&dlist2);
            }

            /* dlist2 == NULL */
            {
                XKVDList_PT dlist1 = xkvdlist_new();
                bool except = false;
                XEXCEPT_TRY
                    xkvdlist_swap(dlist1, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xkvdlist_free(&dlist1);
            }

            /* normal */
            {
                XKVDList_PT dlist1 = xkvdlist_new();
                XKVDList_PT dlist2 = xkvdlist_new();

                xkvdlist_push_back_repeat(dlist1, "key1", "value1");
                xkvdlist_push_back_repeat(dlist1, "key2", "value2");
                xkvdlist_push_back_repeat(dlist1, "key3", "value3");
                xkvdlist_push_back_repeat(dlist1, "key4", "value4");

                xkvdlist_push_back_repeat(dlist2, "key5", "value5");
                xkvdlist_push_back_repeat(dlist2, "key6", "value6");
                xkvdlist_push_back_repeat(dlist2, "key7", "value7");
                xkvdlist_push_back_repeat(dlist2, "key8", "value8");
                xkvdlist_push_back_repeat(dlist2, "key9", "value9");

                xassert(xkvdlist_swap(dlist1, dlist2));

                xassert(strcmp(xpair_first(dlist1->head->value), "key5") == 0);
                xassert(strcmp(xpair_second(dlist1->head->value), "value5") == 0);
                xassert(strcmp(xpair_first(dlist1->tail->value), "key9") == 0);
                xassert(strcmp(xpair_second(dlist1->tail->value), "value9") == 0);

                xassert(strcmp(xpair_first(dlist2->head->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist2->head->value), "value1") == 0);
                xassert(strcmp(xpair_first(dlist2->tail->value), "key4") == 0);
                xassert(strcmp(xpair_second(dlist2->tail->value), "value4") == 0);

                xassert(dlist1->size == 5);
                xassert(dlist2->size == 4);

                xkvdlist_free(&dlist1);
                xkvdlist_free(&dlist2);
            }
        }

        /* xkvdlist_merge */
        {
            {
                XKVDList_PT dlist1 = xkvdlist_new();
                XKVDList_PT dlist2 = xkvdlist_new();

                xkvdlist_push_back_repeat(dlist1, "key1", "value1");
                xkvdlist_push_back_repeat(dlist1, "key2", "value2");
                xkvdlist_push_back_repeat(dlist2, "key3", "value32");
                xkvdlist_push_back_repeat(dlist2, "key4", "value42");

                xassert(xkvdlist_merge(dlist1, &dlist2));

                xassert_false(dlist2);

                xassert(strcmp(xpair_first(dlist1->head->value), "key1") == 0);
                xassert(strcmp(xpair_first(dlist1->tail->value), "key4") == 0);

                xassert(strcmp(xpair_first(dlist1->head->next->next->value), "key3") == 0);
                xassert(strcmp(xpair_first(dlist1->head->next->next->prev->value), "key2") == 0);

                xassert(strcmp(xpair_first(dlist1->tail->prev->prev->value), "key2") == 0);
                xassert(strcmp(xpair_first(dlist1->tail->prev->prev->next->value), "key3") == 0);

                xassert(dlist1->size == 4);

                xkvdlist_free(&dlist1);
            }
        }


        /* xkvdlist_reverse */
        {
            XKVDList_PT dlist = xkvdlist_new();

            xkvdlist_reverse(dlist);

            xkvdlist_push_back_repeat(dlist, "key1", "value1");
            xkvdlist_push_back_repeat(dlist, "key2", "value2");
            xkvdlist_push_back_repeat(dlist, "key3", "value3");
            xkvdlist_push_back_repeat(dlist, "key4", "value4");

            xkvdlist_reverse(dlist);

            xassert(strcmp(xpair_first(dlist->head->value), "key4") == 0);
            xassert(strcmp(xpair_second(dlist->head->value), "value4") == 0);
            xassert(strcmp(xpair_first(dlist->head->next->value), "key3") == 0);
            xassert(strcmp(xpair_second(dlist->head->next->value), "value3") == 0);
            xassert(strcmp(xpair_first(dlist->head->next->next->value), "key2") == 0);
            xassert(strcmp(xpair_second(dlist->head->next->next->value), "value2") == 0);
            xassert(strcmp(xpair_first(dlist->tail->value), "key1") == 0);
            xassert(strcmp(xpair_second(dlist->tail->value), "value1") == 0);

            xassert(dlist->size == 4);

            xkvdlist_free(&dlist);
        }

        /* xkvdlist_map */
        {
            XKVDList_PT dlist = xkvdlist_new();

            xkvdlist_push_back_repeat(dlist, "key1", "value1");
            xkvdlist_push_back_repeat(dlist, "key2", "value2");
            xkvdlist_push_back_repeat(dlist, "key3", "value3");
            xkvdlist_push_back_repeat(dlist, "key4", "value4");

            int count = 0;

            xassert(xkvdlist_map(dlist, xkvdlist_test_map_apply_count, (void*)&count) == 4);
            xassert(count == 4);

            xkvdlist_free(&dlist);
        }

        /* xkvdlist_map_break_if_true */

        /* xkvdlist_map_break_if_false */

        /* xkvdlist_map_key */

        /* xkvdlist_map_key_break_if_true */

        /* xkvdlist_map_key_break_if_false */

        /* xkvdlist_find */
        {
            XKVDList_PT dlist = xkvdlist_new();

            xkvdlist_push_back_repeat(dlist, "key1", "value1");
            xkvdlist_push_back_repeat(dlist, "key2", "value2");
            xkvdlist_push_back_repeat(dlist, "key3", "value3");
            xkvdlist_push_back_repeat(dlist, "key4", "value4");

            XRDList_PT node = xkvdlist_find(dlist, "key2");
            xassert(strcmp(xpair_first(node->value), "key2") == 0);
            xassert(strcmp(xpair_second(node->value), "value2") == 0);

            xkvdlist_free(&dlist);
        }

        /* xkvdlist_find_equal_if */

        /* xkvdlist_find_apply_if */

        /* xkvdlist_find_apply_key_if */

        /* xkvdlist_find_apply_key_if */

        /* xkvdlist_to_array */
        {
            XKVDList_PT dlist = xkvdlist_new();

            xkvdlist_push_back_repeat(dlist, "key1", "value1");
            xkvdlist_push_back_repeat(dlist, "key2", "value2");
            xkvdlist_push_back_repeat(dlist, "key3", "value3");
            xkvdlist_push_back_repeat(dlist, "key4", "value4");

            XPArray_PT array = xkvdlist_to_array(dlist);

            xassert(array->size == 8);
            xassert(strcmp((char*)xparray_get(array, 2), "key2") == 0);

            xparray_free(&array);
            xkvdlist_free(&dlist);
        }

        /* xkvdlist_keys_to_array */

        /* xkvdlist_values_to_array */

        /* xkvdlist_nodes_to_array */

        /* xkvdlist_size */

        /* xkvdlist_is_empty */

        /* xkvdlist_unique */
        {
            XKVDList_PT dlist = xkvdlist_new();

            xkvdlist_push_back_repeat(dlist, "key2", "value2");
            xkvdlist_push_back_repeat(dlist, "key1", "value12");
            xkvdlist_push_back_repeat(dlist, "key1", "value11");
            xkvdlist_push_back_repeat(dlist, "key3", "value3");
            xkvdlist_push_back_repeat(dlist, "key2", "value21");
            xkvdlist_push_back_repeat(dlist, "key4", "value4");

            xkvdlist_unique(dlist, test_cmp, NULL);
            xassert(dlist->size == 4);

            xassert(strcmp(xpair_first(dlist->head->value), "key1") == 0);
            xassert(strcmp(xpair_first(dlist->head->next->value), "key2") == 0);
            xassert(strcmp(xpair_first(dlist->head->next->next->value), "key3") == 0);
            xassert(strcmp(xpair_first(dlist->head->next->next->next->value), "key4") == 0);

            xkvdlist_free(&dlist);
        }

        /* xkvdlist_deep_unique */
        {
#if 0
            {
                XKVDList_PT dlist = tkvdlist_random_string(100);
                xkvdlist_deep_unique(dlist, test_cmp);
                xkvdlist_deep_free(&dlist);
            }
#endif
        }

        /* xkvdlist_sort */
        {
            {
                XKVDList_PT dlist = xkvdlist_new();

                xkvdlist_push_back_repeat(dlist, "key2", "value2");
                xkvdlist_push_back_repeat(dlist, "key1", "value12");
                xkvdlist_push_back_repeat(dlist, "key1", "value11");
                xkvdlist_push_back_repeat(dlist, "key3", "value3");
                xkvdlist_push_back_repeat(dlist, "key5", "value5");
                xkvdlist_push_back_repeat(dlist, "key4", "value4");

                xkvdlist_sort(dlist, test_cmp, NULL);

                xassert(strcmp(xpair_first(dlist->head->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->head->value), "value12") == 0);
                xassert(strcmp(xpair_first(dlist->head->next->value), "key1") == 0);
                xassert(strcmp(xpair_second(dlist->head->next->value), "value11") == 0);
                xassert(strcmp(xpair_first(dlist->head->next->next->value), "key2") == 0);
                xassert(strcmp(xpair_first(dlist->head->next->next->next->value), "key3") == 0);
                xassert(strcmp(xpair_first(dlist->head->next->next->next->next->value), "key4") == 0);
                xassert(strcmp(xpair_first(dlist->head->next->next->next->next->next->value), "key5") == 0);

                xkvdlist_free(&dlist);
            }
#if 0
            {
                XKVDList_PT dlist = tkvdlist_random_string(1000);
                xkvdlist_sort(dlist, test_cmp);
                xkvdlist_deep_free(&dlist);
            }
#endif
        }

        /* xkvdlist_sorted */
        {
            /* tested by xkvdlist_sort already */
        }

        {
            int count = 0;
            xmem_leak(xkvdlist_test_check_mem_leak, &count);
            xassert(count == 0);
        }
}


