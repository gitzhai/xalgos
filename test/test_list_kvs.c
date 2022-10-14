
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../array_pointer/xarray_pointer_x.h"
#include "../list_s_raw/xlist_s_raw_x.h"
#include "../list_s/xlist_s_x.h"
#include "../list_kvs/xlist_kvs_x.h"
#include "../include/xalgos.h"

static
int xkvslist_test_cmp_true(void *key1, void *key2, void *cl) {
    return 0;
}

static
int xkvslist_test_cmp_false(void *key1, void *key2, void *cl) {
    return 1;
}

static 
int xkvslist_test_cmp(void *key1, void *key2, void *cl) {
    return key1 == key2 ? 0 : 1;
}

static
int test_cmp(void *key1, void *key2, void *cl) {
    return strcmp((char*)key1, (char*)key2);
}

static
bool xkvslist_test_apply_key(void *key, void *cl) {
    return strcmp((char*)key, (char*)cl) == 0;
}

static
bool xkvslist_test_applykv(void *key, void **value, void *cl) {
    return strcmp((char*)key, (char*)cl) == 0;
}

static
bool xkvslist_test_applykv2(void *key, void *value, void *cl) {
    return strcmp((char*)key, (char*)cl) == 0;
}

static
bool xkvslist_test_applyk_true(void *key, void *cl) {
    return true;
}

static
bool xkvslist_test_applyk_false(void *key, void *cl) {
    return false;
}

static
bool xkvslist_test_applykv_true(void *key, void **value, void *cl) {
    return true;
}

static
bool xkvslist_test_applykv_false(void *key, void **value, void *cl) {
    return false;
}

static
bool xkvslist_test_applykv_true2(void *key, void *value, void *cl) {
    return true;
}

static
bool xkvslist_test_applykv_false2(void *key, void *value, void *cl) {
    return false;
}

static
bool xkvslist_test_map_apply_count(void *key, void **value, void *cl) {
    ++(*((int*)(cl)));
    return true;
}

static
bool xkvslist_test_print(void *key, void *value, void *cl) {
    printf("%s:%s\n", (char*)key, (char*)value);
    return true;
}

static 
XKVSList_PT tkvslist_random_string(int size) {
    XKVSList_PT slist = xkvslist_new();
    if (!slist) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xkvslist_deep_free(&slist);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }
            str[str_size - 1] = '\0';

            xkvslist_push_back_repeat(slist, str, NULL);
        }
    }

    return slist;
}

static
void xkvslist_test_check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}


void test_xkvslist() {

        /* xkvslist_new */
        {
            XKVSList_PT slist = xkvslist_new();
            xassert(slist);
            xassert(slist->head == NULL);
            xassert(slist->tail == NULL);
            xassert(slist->size == 0);
            xkvslist_free(&slist);
        }

        /* xkvslist_vload_repeat */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_vload_repeat(NULL, "key", "value", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* key == NULL */
            {
                XKVSList_PT slist = xkvslist_new();
                /* first key == NULL */
                xassert(xkvslist_vload_repeat(slist, NULL) == 0);
                /* second key == NULL */
                xassert(xkvslist_vload_repeat(slist, "k1", "v1", NULL, "v2", NULL) == 1);
                xkvslist_free(&slist);
            }

            /* value == NULL */
            {
                XKVSList_PT slist = xkvslist_new();
                /* first value == NULL */
                xassert(xkvslist_vload_repeat(slist, "k1", NULL) == 0);
                /* second value == NULL */
                xassert(xkvslist_vload_repeat(slist, "k1", "v1", "k2", NULL) == 1);
                xkvslist_free(&slist);
            }

            /* normal */
            {
                XKVSList_PT slist = xkvslist_new();
                xassert(xkvslist_vload_repeat(slist, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL) == 4);
                xassert(strcmp(xpair_second(slist->head->value), "v1") == 0);
                xkvslist_free(&slist);
            }
        }

        /* xkvslist_vload_unique */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_vload_unique(NULL, "key", "value", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* key == NULL */
            {
                XKVSList_PT slist = xkvslist_new();
                /* first key == NULL */
                xassert(xkvslist_vload_unique(slist, NULL) == 0);
                /* second key == NULL */
                xassert(xkvslist_vload_unique(slist, "k1", "v1", NULL, "v2", NULL) == 1);
                xkvslist_free(&slist);
            }

            /* value == NULL */
            {
                XKVSList_PT slist = xkvslist_new();
                /* first value == NULL */
                xassert(xkvslist_vload_unique(slist, "k1", NULL) == 0);
                /* second value == NULL */
                xassert(xkvslist_vload_unique(slist, "k1", "v1", "k2", NULL) == 1);
                xkvslist_free(&slist);
            }

            /* repeat key */
            {
                XKVSList_PT slist = xkvslist_new();
                xassert(xkvslist_vload_unique(slist, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL) == 3);
                xassert(strcmp(xpair_second(slist->head->value), "v1") == 0);
                xkvslist_free(&slist);
            }
        }

        /* xkvslist_vload_unique_if */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_vload_unique_if(NULL, xkvslist_test_cmp, NULL, "k", "v", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }
            /* key == NULL */
            {
                XKVSList_PT slist = xkvslist_new();
                /* first key == NULL */
                xassert(xkvslist_vload_unique_if(slist, xkvslist_test_cmp, NULL, NULL) == 0);
                /* second key == NULL */
                xassert(xkvslist_vload_unique(slist, "k1", "v1", NULL, "v2", NULL) == 1);
                xkvslist_free(&slist);
            }

            /* value == NULL */
            {
                XKVSList_PT slist = xkvslist_new();
                /* first value == NULL */
                xassert(xkvslist_vload_unique_if(slist, xkvslist_test_cmp, NULL, "k1", NULL) == 0);
                /* second value == NULL */
                xassert(xkvslist_vload_unique_if(slist, xkvslist_test_cmp, NULL, "k1", "v1", "k2", NULL, NULL) == 1);
                xkvslist_free(&slist);
            }

            /* repeat key */
            {
                XKVSList_PT slist = xkvslist_new();
                xassert(xkvslist_vload_unique_if(slist, xkvslist_test_cmp, NULL, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL) == 3);
                xassert(strcmp(xpair_second(slist->head->value), "v1") == 0);
                xkvslist_free(&slist);
            }
        }

        /* xkvslist_vload_replace */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_vload_replace(NULL, "k", "v", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }
            /* key == NULL */
            {
                XKVSList_PT slist = xkvslist_new();
                /* first key == NULL */
                xassert(xkvslist_vload_replace(slist, NULL) == 0);
                /* second key == NULL */
                xassert(xkvslist_vload_replace(slist, "k1", "v1", NULL, "v2", NULL) == 1);
                xkvslist_free(&slist);
            }

            /* value == NULL */
            {
                XKVSList_PT slist = xkvslist_new();
                /* first value == NULL */
                xassert(xkvslist_vload_replace(slist, "k1", NULL) == 0);
                /* second value == NULL */
                xassert(xkvslist_vload_replace(slist, "k1", "v1", "k2", NULL) == 1);
                xkvslist_free(&slist);
            }

            /* repeat key */
            {
                XKVSList_PT slist = xkvslist_new();
                xassert(xkvslist_vload_replace(slist, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL) == 3);
                xassert(strcmp(xpair_second(slist->head->value), "v12") == 0);
                xkvslist_free(&slist);
            }
        }

        /* xkvslist_vload_replace_if */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_vload_replace_if(NULL, xkvslist_test_cmp, NULL, "k", "v", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
            }

            /* key == NULL */
            {
                XKVSList_PT slist = xkvslist_new();
                /* first key == NULL */
                xassert(xkvslist_vload_replace_if(slist, xkvslist_test_cmp, NULL, NULL) == 0);
                /* second key == NULL */
                xassert(xkvslist_vload_replace_if(slist, xkvslist_test_cmp, NULL, "k1", "v1", NULL, "v2", NULL) == 1);
                xkvslist_free(&slist);
            }

            /* value == NULL */
            {
                XKVSList_PT slist = xkvslist_new();
                /* first value == NULL */
                xassert(xkvslist_vload_replace_if(slist, xkvslist_test_cmp, NULL, "k1", NULL) == 0);
                /* second value == NULL */
                xassert(xkvslist_vload_replace_if(slist, xkvslist_test_cmp, NULL, "k1", "v1", "k2", NULL) == 1);
                xkvslist_free(&slist);
            }

            /* repeat key */
            {
                XKVSList_PT slist = xkvslist_new();
                xassert(xkvslist_vload_replace_if(slist, xkvslist_test_cmp, NULL, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL) == 3);
                xassert(strcmp(xpair_second(slist->head->value), "v12") == 0);
                xkvslist_free(&slist);
            }
        }

        /* xkvslist_aload_repeat */
        {
            /* slist == NULL */
            {
                XPArray_PT array = xparray_new(10);
                xparray_put(array, 0, "k", NULL);
                xparray_put(array, 1, "v", NULL);
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_aload_repeat(NULL, array);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xparray_free(&array);
            }

            /* xs == NULL */
            {
                XKVSList_PT slist = xkvslist_new();
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_aload_repeat(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xkvslist_free(&slist);
            }

            /* key == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XKVSList_PT slist = xkvslist_new();

                /* first key == NULL */
                xassert(xkvslist_aload_repeat(slist, array) == 0);

                /* second key == NULL */
                xparray_vload(array, "k1", "v1", NULL);
                xassert(xkvslist_aload_repeat(slist, array) == 1);

                xkvslist_free(&slist);
                xparray_free(&array);
            }

            /* value == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XKVSList_PT slist = xkvslist_new();

                /* first value == NULL */
                xparray_vload(array, "k1", NULL);
                xassert(xkvslist_aload_repeat(slist, array) == 0);

                /* second value == NULL */
                xparray_vload(array, "k1", "v1", "k2", NULL);
                xassert(xkvslist_aload_repeat(slist, array) == 1);

                xkvslist_free(&slist);
                xparray_free(&array);
            }

            /* normal */
            {
                XPArray_PT array = xparray_new(10);
                XKVSList_PT slist = xkvslist_new();

                xparray_vload(array, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL);
                xassert(xkvslist_aload_repeat(slist, array) == 4);
                xassert(strcmp(xpair_second(slist->head->value), "v1") == 0);

                xkvslist_free(&slist);
                xparray_free(&array);
            }
        }

        /* xkvslist_aload_unique */
        {
            /* slist == NULL */
            {
                XPArray_PT array = xparray_new(10);
                xparray_put(array, 0, "k", NULL);
                xparray_put(array, 1, "v", NULL);

                bool except = false;
                XEXCEPT_TRY
                    xkvslist_aload_unique(NULL, array);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xparray_free(&array);
            }
            /* xs == NULL */
            {
                XKVSList_PT slist = xkvslist_new();
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_aload_unique(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xkvslist_free(&slist);
            }
            /* key == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XKVSList_PT slist = xkvslist_new();

                /* first key == NULL */
                xassert(xkvslist_aload_unique(slist, array) == 0);

                /* second key == NULL */
                xparray_vload(array, "k1", "v1", NULL);
                xassert(xkvslist_aload_unique(slist, array) == 1);

                xkvslist_free(&slist);
                xparray_free(&array);
            }

            /* value == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XKVSList_PT slist = xkvslist_new();

                /* first value == NULL */
                xparray_vload(array, "k1", NULL);
                xassert(xkvslist_aload_unique(slist, array) == 0);

                /* second value == NULL */
                xparray_vload(array, "k1", "v1", "k2", NULL);
                xassert(xkvslist_aload_unique(slist, array) == 1);

                xkvslist_free(&slist);
                xparray_free(&array);
            }

            /* repeat key */
            {
                XPArray_PT array = xparray_new(10);
                XKVSList_PT slist = xkvslist_new();

                xparray_vload(array, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL);
                xassert(xkvslist_aload_unique(slist, array) == 3);
                xassert(strcmp(xpair_second(slist->head->value), "v1") == 0);

                xkvslist_free(&slist);
                xparray_free(&array);
            }
        }

        /* xkvslist_aload_unique_if */
        {
            /* cmp == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XKVSList_PT slist = xkvslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvslist_aload_unique_if(slist, array, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvslist_free(&slist);
                xparray_free(&array);
            }

            /* below 3 scenarios tested already in xkvslist_aload_unique */
            /* xs == NULL */
            /* key == NULL */
            /* value == NULL */

            /* repeat key */
            {
                XPArray_PT array = xparray_new(10);
                XKVSList_PT slist = xkvslist_new();

                xparray_vload(array, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL);
                xassert(xkvslist_aload_unique_if(slist, array, xkvslist_test_cmp, NULL) == 3);
                xassert(strcmp(xpair_second(slist->head->value), "v1") == 0);

                xkvslist_free(&slist);
                xparray_free(&array);
            }
        }

        /* xkvslist_aload_replace */
        {
            /* slist == NULL */
            {
                XPArray_PT array = xparray_new(10);
                xparray_put(array, 0, "k", NULL);
                xparray_put(array, 1, "v", NULL);

                bool except = false;
                XEXCEPT_TRY
                    xkvslist_aload_replace(NULL, array);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xparray_free(&array);
            }
            /* xs == NULL */
            {
                XKVSList_PT slist = xkvslist_new();
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_aload_replace(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xkvslist_free(&slist);
            }

            /* key == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XKVSList_PT slist = xkvslist_new();

                /* first key == NULL */
                xassert(xkvslist_aload_replace(slist, array) == 0);

                /* second key == NULL */
                xparray_vload(array, "k1", "v1", NULL);
                xassert(xkvslist_aload_replace(slist, array) == 1);

                xkvslist_free(&slist);
                xparray_free(&array);
            }

            /* value == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XKVSList_PT slist = xkvslist_new();

                /* first value == NULL */
                xparray_vload(array, "k1", NULL);
                xassert(xkvslist_aload_replace(slist, array) == 0);

                /* second value == NULL */
                xparray_vload(array, "k1", "v1", "k2", NULL);
                xassert(xkvslist_aload_replace(slist, array) == 1);

                xkvslist_free(&slist);
                xparray_free(&array);
            }

            /* repeat key */
            {
                XPArray_PT array = xparray_new(10);
                XKVSList_PT slist = xkvslist_new();

                xparray_vload(array, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL);
                xassert(xkvslist_aload_replace(slist, array) == 3);
                xassert(strcmp(xpair_second(slist->head->value), "v12") == 0);

                xkvslist_free(&slist);
                xparray_free(&array);
            }
        }

        /* xkvslist_aload_replace_if */
        {
            /* cmp == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XKVSList_PT slist = xkvslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvslist_aload_replace_if(slist, array, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);

                xkvslist_free(&slist);
                xparray_free(&array);
            }

            /* below 3 scenarios tested already in xkvslist_aload_replace */
            /* xs == NULL */
            /* key == NULL */
            /* value == NULL */

            /* repeat key */
            {
                XPArray_PT array = xparray_new(10);
                XKVSList_PT slist = xkvslist_new();

                xparray_vload(array, "k1", "v1", "k2", "v2", "k1", "v12", "k3", "v3", NULL);
                xassert(xkvslist_aload_replace_if(slist, array, xkvslist_test_cmp, NULL) == 3);
                xassert(strcmp(xpair_second(slist->head->value), "v12") == 0);

                xkvslist_free(&slist);
                xparray_free(&array);
            }
        }

        /* xkvslist_copyn_if_impl */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_copyn_if_impl(NULL, 0, 0, 1, false, xkvslist_test_cmp, NULL, NULL, NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* count < 0 */
            {
                XKVSList_PT slist = xkvslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvslist_copyn_if_impl(slist, 0, 0, -1, false, xkvslist_test_cmp, NULL, NULL, xkvslist_test_applykv_true2, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvslist_free(&slist);
            }

            /* deep == true && key_size < 0 */
            {
                XKVSList_PT slist = xkvslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvslist_copyn_if_impl(slist, -1, 0, 1, true, xkvslist_test_cmp, NULL, NULL, xkvslist_test_applykv_true2, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvslist_free(&slist);
            }

            /* deep == true && key_size == 0 */
            {
                XKVSList_PT slist = xkvslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvslist_copyn_if_impl(slist, 0, 0, 1, true, xkvslist_test_cmp, NULL, NULL, xkvslist_test_applykv_true2, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvslist_free(&slist);
            }
            /* slist is empty */
            {
                XKVSList_PT slist = xkvslist_new();

                {
                    XKVSList_PT nslist = xkvslist_copyn_if_impl(slist, 0, 0, 1, false, NULL, NULL, NULL, xkvslist_test_applykv_true2, NULL);
                    xassert(nslist->size == 0);
                    xkvslist_free(&nslist);
                }

                xkvslist_free(&slist);
            }

            /* count == 0 */
            {
                XKVSList_PT slist = xkvslist_new();
                xkvslist_vload_repeat(slist, "k1", "v1", "k2", "v2", NULL);

                {
                    XKVSList_PT nslist = xkvslist_copyn_if_impl(slist, 0, 0, 0, false, NULL, NULL, NULL, xkvslist_test_applykv_true2, NULL);
                    xassert(nslist->size == 0);
                    xkvslist_free(&nslist);
                }

                xkvslist_free(&slist);
            }

            /* slist->size < count */
            {
                XKVSList_PT slist = xkvslist_new();
                xkvslist_vload_repeat(slist, "k1", "v1", "k1", "v2", NULL);

                {
                    XKVSList_PT nslist = xkvslist_copyn_if_impl(slist, 0, 0, 3, false, NULL, NULL, NULL, xkvslist_test_applykv_true2, NULL);
                    xassert(nslist->size == 2);
                    xkvslist_free(&nslist);
                }

                xkvslist_free(&slist);
            }

            /* count < slist->size */
            {
                XKVSList_PT slist = xkvslist_new();
                xkvslist_vload_repeat(slist, "k1", "v1", "k1", "v2", NULL);

                {
                    XKVSList_PT nslist = xkvslist_copyn_if_impl(slist, 0, 0, 1, false, NULL, NULL, NULL, xkvslist_test_applykv_true2, NULL);
                    xassert(nslist->size == 1);
                    xassert(strcmp(xpair_second(nslist->head->value), "v1") == 0);
                    xkvslist_free(&nslist);
                }

                xkvslist_free(&slist);
            }

            /* no any key match */
            {
                XKVSList_PT slist = xkvslist_new();
                xkvslist_vload_repeat(slist, "k1", "v1", "k2", "v2", NULL);

                {
                    XKVSList_PT nslist = xkvslist_copyn_if_impl(slist, 0, 0, 1, false, NULL, NULL, NULL, xkvslist_test_applykv_false2, NULL);
                    xassert(nslist->size == 0);
                    xkvslist_free(&nslist);
                }

                xkvslist_free(&slist);
            }

            /* normal */
            {
                XKVSList_PT slist = xkvslist_new();
                xkvslist_vload_repeat(slist, "k1", "v1", "k1", "v2", "k3", "v3", NULL);

                {
                    XKVSList_PT nslist = xkvslist_copyn_if_impl(slist, 0, 0, 2, false, NULL, NULL, NULL, xkvslist_test_applykv_true2, NULL);
                    xassert(nslist->size == 2);
                    xassert(strcmp(xpair_second(nslist->head->value), "v1") == 0);
                    xassert(strcmp(xpair_second(nslist->head->next->value), "v2") == 0);
                    xkvslist_free(&nslist);
                }

                {
                    XKVSList_PT nslist = xkvslist_copyn_if_impl(slist, 0, 0, 2, false, NULL, NULL, NULL, xkvslist_test_applykv2, "k3");
                    xassert(nslist->size == 1);
                    xassert(strcmp(xpair_first(nslist->head->value), "k3") == 0);
                    xassert(strcmp(xpair_second(nslist->head->value), "v3") == 0);
                    xkvslist_free(&nslist);
                }

                {
                    XKVSList_PT nslist = xkvslist_copyn_if_impl(slist, 0, 0, 2, false, test_cmp, NULL, NULL, NULL, "k3");
                    xassert(nslist->size == 1);
                    xassert(strcmp(xpair_first(nslist->head->value), "k3") == 0);
                    xassert(strcmp(xpair_second(nslist->head->value), "v3") == 0);
                    xkvslist_free(&nslist);
                }

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_copyn_equal_if */
        {
            /* key == NULL */
            {
                XKVSList_PT slist = xkvslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvslist_copyn_equal_if(slist, 1, NULL, xkvslist_test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_deep_copyn_equal_if */
        {
            /* key == NULL */
            {
                XKVSList_PT slist = xkvslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvslist_deep_copyn_equal_if(slist, 1, 1, 1, NULL, xkvslist_test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);

                xkvslist_free(&slist);
            }
        }

        /* all below copy functions tested by xkvslist_copyn_if_impl already */
        /* xkvslist_copy */
        /* xkvslist_copy_equal_if */
        /* xkvslist_copy_apply_if */
        /* xkvslist_copy_apply_key_if */
        /* xkvslist_copyn */
        /* xkvslist_copyn_equal_if */
        /* xkvslist_copyn_apply_if */
        /* xkvslist_copyn_apply_key_if */
        /* xkvslist_deep_copy */
        /* xkvslist_deep_copy_equal_if */
        /* xkvslist_deep_copy_apply_if */
        /* xkvslist_deep_copy_apply_key_if */
        /* xkvslist_deep_copyn */
        /* xkvslist_deep_copyn_equal_if */
        /* xkvslist_deep_copyn_apply_if */
        /* xkvslist_deep_copyn_apply_key_if */


        /* xkvslist_push_front_repeat */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_push_front_repeat(NULL, "k", "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* key == NULL */
            {
                XKVSList_PT slist = xkvslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvslist_push_front_repeat(slist, NULL, "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvslist_free(&slist);
            }
            /* normal */
            {
                XKVSList_PT slist = xkvslist_new();

                char *str = "key1";

                xkvslist_push_front_repeat(slist, str, "value1");
                xkvslist_push_front_repeat(slist, "key2", "value2");
                xkvslist_push_front_repeat(slist, str, "value12");
                xkvslist_push_front_repeat(slist, "key3", "value3");

                xassert(strcmp(xpair_first(slist->head->value), "key3") == 0);
                xassert(strcmp(xpair_second(slist->head->value), "value3") == 0);
                xassert(strcmp(xpair_first(slist->head->next->next->value), "key2") == 0);
                xassert(strcmp(xpair_second(slist->head->next->next->value), "value2") == 0);
                xassert(strcmp(xpair_first(slist->tail->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->tail->value), "value1") == 0);

                xassert(slist->size == 4);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_pop_front */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_pop_front(NULL, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
            }
            /* slist is empty */
            {
                XKVSList_PT slist = xkvslist_new();
                xassert(xkvslist_pop_front(slist, NULL, NULL) == false);
                xkvslist_free(&slist);
            }

            {
                XKVSList_PT slist = xkvslist_new();

                xkvslist_push_back_repeat(slist, "key1", "value1");
                xkvslist_push_back_repeat(slist, "key2", "value2");
                xkvslist_push_back_repeat(slist, "key3", "value3");

                {
                    void *key = NULL;
                    void *value = NULL;

                    xassert(xkvslist_pop_front(slist, &key, &value));
                    xassert(strcmp(key, "key1") == 0);
                    xassert(strcmp(value, "value1") == 0);

                    xassert(xkvslist_pop_front(slist, &key, NULL));
                    xassert(strcmp(key, "key2") == 0);

                    xassert(xkvslist_pop_front(slist, NULL, &value));
                    xassert(strcmp(value, "value3") == 0);

                    xassert(slist->size == 0);
                    xassert(slist->head == NULL);
                    xassert(slist->tail == NULL);
                }

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_front */
        {
            /* slist is empty */
            {
                XKVSList_PT slist = xkvslist_new();
                xassert_false(xkvslist_front(slist));
                xkvslist_free(&slist);
            }

            /* normal */
            {
                XKVSList_PT slist = xkvslist_new();

                xkvslist_push_back_repeat(slist, "key1", "value1");
                xkvslist_push_back_repeat(slist, "key2", "value2");
                xkvslist_push_back_repeat(slist, "key3", "value3");

                xassert(strcmp(xpair_first(xkvslist_front(slist)), "key1") == 0);
                xassert(strcmp(xpair_second(xkvslist_front(slist)), "value1") == 0);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_back */
        {
            /* slist is empty */
            {
                XKVSList_PT slist = xkvslist_new();
                xassert_false(xkvslist_back(slist));
                xkvslist_free(&slist);
            }

            /* normal */
            {
                XKVSList_PT slist = xkvslist_new();

                xkvslist_push_back_repeat(slist, "key1", "value1");
                xkvslist_push_back_repeat(slist, "key2", "value2");
                xkvslist_push_back_repeat(slist, "key3", "value3");

                xassert(strcmp(xpair_first(xkvslist_back(slist)), "key3") == 0);
                xassert(strcmp(xpair_second(xkvslist_back(slist)), "value3") == 0);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_front */
        {
            /* slist is empty */
            {
                XKVSList_PT slist = xkvslist_new();
                xassert_false(xkvslist_front(slist));
                xkvslist_free(&slist);
            }

            /* normal */
            {
                XKVSList_PT slist = xkvslist_new();

                xkvslist_push_back_repeat(slist, "key1", "value1");
                xkvslist_push_back_repeat(slist, "key2", "value2");
                xkvslist_push_back_repeat(slist, "key3", "value3");

                xassert(strcmp(xpair_first(xkvslist_front(slist)), "key1") == 0);
                xassert(strcmp(xpair_second(xkvslist_front(slist)), "value1") == 0);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_back */
        {
            /* slist is empty */
            {
                XKVSList_PT slist = xkvslist_new();
                xassert_false(xkvslist_back(slist));
                xkvslist_free(&slist);
            }

            /* normal */
            {
                XKVSList_PT slist = xkvslist_new();

                xkvslist_push_back_repeat(slist, "key1", "value1");
                xkvslist_push_back_repeat(slist, "key2", "value2");
                xkvslist_push_back_repeat(slist, "key3", "value3");

                xassert(strcmp(xpair_first(xkvslist_back(slist)), "key3") == 0);
                xassert(strcmp(xpair_second(xkvslist_back(slist)), "value3") == 0);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_front_node */
        {
            /* slist is empty */
            {
                XKVSList_PT slist = xkvslist_new();
                xassert_false(xkvslist_front_node(slist));
                xkvslist_free(&slist);
            }

            /* normal */
            {
                XKVSList_PT slist = xkvslist_new();

                xkvslist_push_back_repeat(slist, "key1", "value1");
                xkvslist_push_back_repeat(slist, "key2", "value2");
                xkvslist_push_back_repeat(slist, "key3", "value3");

                xassert(strcmp(xpair_first(xkvslist_front_node(slist)->value), "key1") == 0);
                xassert(strcmp(xpair_second(xkvslist_front_node(slist)->value), "value1") == 0);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_back_node */
        {
            /* slist is empty */
            {
                XKVSList_PT slist = xkvslist_new();
                xassert_false(xkvslist_back_node(slist));
                xkvslist_free(&slist);
            }

            /* normal */
            {
                XKVSList_PT slist = xkvslist_new();

                xkvslist_push_back_repeat(slist, "key1", "value1");
                xkvslist_push_back_repeat(slist, "key2", "value2");
                xkvslist_push_back_repeat(slist, "key3", "value3");

                xassert(strcmp(xpair_first(xkvslist_back_node(slist)->value), "key3") == 0);
                xassert(strcmp(xpair_second(xkvslist_back_node(slist)->value), "value3") == 0);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_push_back_repeat */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_push_back_repeat(NULL, "k", "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* key == NULL */
            {
                XKVSList_PT slist = xkvslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvslist_push_back_repeat(slist, NULL, "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvslist_free(&slist);
            }
            /* normal */
            {
                XKVSList_PT slist = xkvslist_new();

                char *str = "key1";

                xkvslist_push_back_repeat(slist, str, "value1");
                xkvslist_push_back_repeat(slist, "key2", "value2");
                xkvslist_push_back_repeat(slist, str, "value12");
                xkvslist_push_back_repeat(slist, "key3", "value3");

                xassert(strcmp(xpair_first(slist->head->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->head->value), "value1") == 0);
                xassert(strcmp(xpair_first(slist->head->next->next->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->head->next->next->value), "value12") == 0);
                xassert(strcmp(xpair_first(slist->tail->value), "key3") == 0);
                xassert(strcmp(xpair_second(slist->tail->value), "value3") == 0);

                xassert(slist->size == 4);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_push_front_unique */
        {
            XKVSList_PT slist = xkvslist_new();

            char *str = "key1"; 

            xkvslist_push_front_unique(slist, str, "value1");
            xkvslist_push_front_unique(slist, "key2", "value2");
            xkvslist_push_front_unique(slist, str, "value12");
            xkvslist_push_front_unique(slist, "key3", "value3");

            xassert(strcmp(xpair_first(slist->head->value), "key3") == 0);
            xassert(strcmp(xpair_second(slist->head->value), "value3") == 0);
            xassert(strcmp(xpair_first(slist->head->next->next->value), "key1") == 0);
            xassert(strcmp(xpair_second(slist->head->next->next->value), "value1") == 0);
            xassert(strcmp(xpair_first(slist->tail->value), "key1") == 0);
            xassert(strcmp(xpair_second(slist->tail->value), "value1") == 0);
            xassert(slist->size == 3);

            xkvslist_free(&slist);
        }

        /* xkvslist_push_front_unique_if */
        {
            XKVSList_PT slist = xkvslist_new();

            xkvslist_push_front_unique_if(slist, "key1", "value1", test_cmp, NULL);
            xkvslist_push_front_unique_if(slist, "key2", "value2", test_cmp, NULL);
            xkvslist_push_front_unique_if(slist, "key1", "value12", test_cmp, NULL);
            xkvslist_push_front_unique_if(slist, "key3", "value3", test_cmp, NULL);

            xassert(strcmp(xpair_first(slist->head->value), "key3") == 0);
            xassert(strcmp(xpair_second(slist->head->value), "value3") == 0);
            xassert(strcmp(xpair_first(slist->head->next->next->value), "key1") == 0);
            xassert(strcmp(xpair_second(slist->head->next->next->value), "value1") == 0);
            xassert(strcmp(xpair_first(slist->tail->value), "key1") == 0);
            xassert(strcmp(xpair_second(slist->tail->value), "value1") == 0);
            xassert(slist->size == 3);

            xkvslist_free(&slist);
        }

        /* xkvslist_push_front_replace */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_push_front_replace(NULL, "k", "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* key == NULL */
            {
                XKVSList_PT slist = xkvslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvslist_push_front_replace(slist, NULL, "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvslist_free(&slist);
            }
            /* repeat key */
            {
                XKVSList_PT slist = xkvslist_new();

                char *str1 = "key1";

                xkvslist_push_front_replace(slist, str1, "value1");
                xkvslist_push_front_replace(slist, "key2", "value2");
                xkvslist_push_front_replace(slist, str1, "value12");
                xkvslist_push_front_replace(slist, "key3", "value3");

                xassert(strcmp(xpair_first(slist->head->value), "key3") == 0);
                xassert(strcmp(xpair_second(slist->head->value), "value3") == 0);
                xassert(strcmp(xpair_first(slist->head->next->next->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->head->next->next->value), "value12") == 0);
                xassert(strcmp(xpair_first(slist->tail->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->tail->value), "value12") == 0);
                xassert(slist->size == 3);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_push_front_deep_replace */
        {
            /* below scenarios tested by xkvslist_push_front_replace already */
            /* slist == NULL */
            /* key == NULL */

            /* repeat key */
            {
                XKVSList_PT slist = xkvslist_new();

                char *str1 = "key1";
                char *str2 = XMEM_CALLOC(1, 7);
                memcpy(str2, "value1", 6);

                xkvslist_push_front_deep_replace(slist, str1, str2);
                xkvslist_push_front_deep_replace(slist, "key2", "value2");
                xkvslist_push_front_deep_replace(slist, str1, "value12");
                xkvslist_push_front_deep_replace(slist, "key3", "value3");

                xassert(strcmp(xpair_first(slist->head->value), "key3") == 0);
                xassert(strcmp(xpair_second(slist->head->value), "value3") == 0);
                xassert(strcmp(xpair_first(slist->head->next->next->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->head->next->next->value), "value12") == 0);
                xassert(strcmp(xpair_first(slist->tail->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->tail->value), "value12") == 0);
                xassert(slist->size == 3);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_push_front_replace_if */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_push_front_replace_if(NULL, "k", "v", test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
            }

            /* key == NULL */
            {
                XKVSList_PT slist = xkvslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvslist_push_front_replace_if(slist, NULL, "v", test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvslist_free(&slist);
            }

            /* repeat key */
            {
                XKVSList_PT slist = xkvslist_new();

                xkvslist_push_front_replace_if(slist, "key1", "value1", test_cmp, NULL);
                xkvslist_push_front_replace_if(slist, "key2", "value2", test_cmp, NULL);
                xkvslist_push_front_replace_if(slist, "key1", "value12", test_cmp, NULL);
                xkvslist_push_front_replace_if(slist, "key3", "value3", test_cmp, NULL);

                xassert(strcmp(xpair_first(slist->head->value), "key3") == 0);
                xassert(strcmp(xpair_second(slist->head->value), "value3") == 0);
                xassert(strcmp(xpair_first(slist->head->next->next->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->head->next->next->value), "value12") == 0);
                xassert(strcmp(xpair_first(slist->tail->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->tail->value), "value12") == 0);
                xassert(slist->size == 3);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_push_front_deep_replace_if */
        {
            /* below scenarios tested by xkvslist_push_front_replace_if already */
            /* slist == NULL */
            /* key == NULL */
            /* cmp == NULL */

            /* repeat key */
            {
                XKVSList_PT slist = xkvslist_new();

                char *key = XMEM_CALLOC(1, 7);
                char *value = XMEM_CALLOC(1, 7);
                memcpy(key, "key1", 4);
                memcpy(value, "value1", 6);

                xkvslist_push_front_deep_replace_if(slist, key, value, test_cmp, NULL);
                xkvslist_push_front_deep_replace_if(slist, "key2", "value2", test_cmp, NULL);
                xkvslist_push_front_deep_replace_if(slist, "key1", "value12", test_cmp, NULL);
                xkvslist_push_front_deep_replace_if(slist, "key3", "value3", test_cmp, NULL);

                xassert(strcmp(xpair_first(slist->head->value), "key3") == 0);
                xassert(strcmp(xpair_second(slist->head->value), "value3") == 0);
                xassert(strcmp(xpair_first(slist->head->next->next->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->head->next->next->value), "value12") == 0);
                xassert(strcmp(xpair_first(slist->tail->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->tail->value), "value12") == 0);
                xassert(slist->size == 3);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_push_back_unique */
        {
            XKVSList_PT slist = xkvslist_new();

            char *str = "key1";

            xkvslist_push_back_unique(slist, str, "value1");
            xkvslist_push_back_unique(slist, "key2", "value2");
            xkvslist_push_back_unique(slist, str, "value12");
            xkvslist_push_back_unique(slist, "key3", "value3");

            xassert(strcmp(xpair_first(slist->head->value), "key1") == 0);
            xassert(strcmp(xpair_second(slist->head->value), "value1") == 0);
            xassert(strcmp(xpair_first(slist->head->next->next->value), "key3") == 0);
            xassert(strcmp(xpair_second(slist->head->next->next->value), "value3") == 0);
            xassert(strcmp(xpair_first(slist->tail->value), "key3") == 0);
            xassert(strcmp(xpair_second(slist->tail->value), "value3") == 0);
            xassert(slist->size == 3);

            xkvslist_free(&slist);
        }

        /* xkvslist_push_back_unique_if */
        {
            XKVSList_PT slist = xkvslist_new();

            xkvslist_push_back_unique_if(slist, "key1", "value1", test_cmp, NULL);
            xkvslist_push_back_unique_if(slist, "key2", "value2", test_cmp, NULL);
            xkvslist_push_back_unique_if(slist, "key1", "value12", test_cmp, NULL);
            xkvslist_push_back_unique_if(slist, "key3", "value3", test_cmp, NULL);

            xassert(strcmp(xpair_first(slist->head->value), "key1") == 0);
            xassert(strcmp(xpair_second(slist->head->value), "value1") == 0);
            xassert(strcmp(xpair_first(slist->head->next->next->value), "key3") == 0);
            xassert(strcmp(xpair_second(slist->head->next->next->value), "value3") == 0);
            xassert(strcmp(xpair_first(slist->tail->value), "key3") == 0);
            xassert(strcmp(xpair_second(slist->tail->value), "value3") == 0);
            xassert(slist->size == 3);

            xkvslist_free(&slist);
        }

        /* xkvslist_push_back_replace */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_push_back_replace(NULL, "k", "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
            }

            /* key == NULL */
            {
                XKVSList_PT slist = xkvslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvslist_push_back_replace(slist, NULL, "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvslist_free(&slist);
            }
            /* repeat key */
            {
                XKVSList_PT slist = xkvslist_new();

                char *str1 = "key1";

                xkvslist_push_back_replace(slist, str1, "value1");
                xkvslist_push_back_replace(slist, "key2", "value2");
                xkvslist_push_back_replace(slist, str1, "value12");
                xkvslist_push_back_replace(slist, "key3", "value3");

                xassert(strcmp(xpair_first(slist->head->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->head->value), "value12") == 0);
                xassert(strcmp(xpair_first(slist->head->next->next->value), "key3") == 0);
                xassert(strcmp(xpair_second(slist->head->next->next->value), "value3") == 0);
                xassert(strcmp(xpair_first(slist->tail->value), "key3") == 0);
                xassert(strcmp(xpair_second(slist->tail->value), "value3") == 0);
                xassert(slist->size == 3);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_push_back_deep_replace */
        {
            /* below scenarios tested by xkvslist_push_back_replace already */
            /* slist == NULL */
            /* key == NULL */

            /* repeat key */
            {
                XKVSList_PT slist = xkvslist_new();

                char *str1 = "key1";
                char *str2 = XMEM_CALLOC(1, 7);
                memcpy(str2, "value1", 6);

                xkvslist_push_back_deep_replace(slist, str1, str2);
                xkvslist_push_back_deep_replace(slist, "key2", "value2");
                xkvslist_push_back_deep_replace(slist, str1, "value12");
                xkvslist_push_back_deep_replace(slist, "key3", "value3");

                xassert(strcmp(xpair_first(slist->head->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->head->value), "value12") == 0);
                xassert(strcmp(xpair_first(slist->head->next->next->value), "key3") == 0);
                xassert(strcmp(xpair_second(slist->head->next->next->value), "value3") == 0);
                xassert(strcmp(xpair_first(slist->tail->value), "key3") == 0);
                xassert(strcmp(xpair_second(slist->tail->value), "value3") == 0);
                xassert(slist->size == 3);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_push_back_replace_if */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_push_back_replace_if(NULL, "k", "v", test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* key == NULL */
            {
                XKVSList_PT slist = xkvslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvslist_push_back_replace_if(slist, NULL, "v", test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xkvslist_free(&slist);
            }

            /* repeat key */
            {
                XKVSList_PT slist = xkvslist_new();

                xkvslist_push_back_replace_if(slist, "key1", "value1", test_cmp, NULL);
                xkvslist_push_back_replace_if(slist, "key2", "value2", test_cmp, NULL);
                xkvslist_push_back_replace_if(slist, "key1", "value12", test_cmp, NULL);
                xkvslist_push_back_replace_if(slist, "key3", "value3", test_cmp, NULL);

                xassert(strcmp(xpair_first(slist->head->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->head->value), "value12") == 0);
                xassert(strcmp(xpair_first(slist->head->next->next->value), "key3") == 0);
                xassert(strcmp(xpair_second(slist->head->next->next->value), "value3") == 0);
                xassert(strcmp(xpair_first(slist->tail->value), "key3") == 0);
                xassert(strcmp(xpair_second(slist->tail->value), "value3") == 0);
                xassert(slist->size == 3);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_push_back_deep_replace_if */
        {
            /* below scenarios tested by xkvslist_push_back_replace_if already */
            /* slist == NULL */
            /* key == NULL */
            /* cmp == NULL */

            /* repeat key */
            {
                XKVSList_PT slist = xkvslist_new();

                char *key = XMEM_CALLOC(1, 7);
                char *value = XMEM_CALLOC(1, 7);
                memcpy(key, "key1", 4);
                memcpy(value, "value1", 6);

                xkvslist_push_back_deep_replace_if(slist, key, value, test_cmp, NULL);
                xkvslist_push_back_deep_replace_if(slist, "key2", "value2", test_cmp, NULL);
                xkvslist_push_back_deep_replace_if(slist, "key1", "value12", test_cmp, NULL);
                xkvslist_push_back_deep_replace_if(slist, "key3", "value3", test_cmp, NULL);

                xassert(strcmp(xpair_first(slist->head->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->head->value), "value12") == 0);
                xassert(strcmp(xpair_first(slist->head->next->next->value), "key3") == 0);
                xassert(strcmp(xpair_second(slist->head->next->next->value), "value3") == 0);
                xassert(strcmp(xpair_first(slist->tail->value), "key3") == 0);
                xassert(strcmp(xpair_second(slist->tail->value), "value3") == 0);
                xassert(slist->size == 3);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_insert_after */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_insert_after(NULL, NULL, "k", "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
            }

            /* key == NULL */
            {
                XKVSList_PT slist = xkvslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xkvslist_insert_after(slist, NULL, NULL, "v");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);

                xkvslist_free(&slist);
            }
            {
                XKVSList_PT slist = xkvslist_new();

                xkvslist_push_back_repeat(slist, "key1", "value1");
                xkvslist_push_back_repeat(slist, "key2", "value2");
                xkvslist_push_back_repeat(slist, "key3", "value3");

                {
                    xkvslist_insert_after(slist, NULL, "key2", "value22");
                    /*
                     *  key1, value1
                     *  key2, value2
                     *  key3, value3
                     *  key2, value22
                     */
                    xassert(strcmp(xpair_first(slist->tail->value), "key2") == 0);
                    xassert(strcmp(xpair_second(slist->tail->value), "value22") == 0);
                }

                {
                    XRSList_PT node = xkvslist_front_node(slist)->next->next;
                    xkvslist_insert_after(slist, node, "key2", "value23");
                    /*
                    * key1, value1
                    * key2, value2
                    * key3, value3
                    * key2, value22
                    * key2, value23
                    */
                    xassert(strcmp(xpair_first(slist->head->next->next->next->value), "key2") == 0);
                    xassert(strcmp(xpair_second(slist->head->next->next->next->value), "value23") == 0);
                }

                xassert(strcmp(xpair_first(slist->head->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->head->value), "value1") == 0);
                xassert(slist->size == 5);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_remove_after */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_remove_after(NULL, NULL, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }
            /* slist is empty */
            {
                XKVSList_PT slist = xkvslist_new();
                xassert_false(xkvslist_remove_after(slist, slist->head, NULL, NULL));
                xkvslist_free(&slist);
            }

            /* node is NULL */
            {
                XKVSList_PT slist = xkvslist_new();
                xassert_false(xkvslist_remove_after(slist, slist->head, NULL, NULL));
                xkvslist_free(&slist);
            }

            /* node->next is NULl */
            {
                XKVSList_PT slist = xkvslist_new();
                xkvslist_vload_repeat(slist, "k", "v", NULL);

                xassert_false(xkvslist_remove_after(slist, slist->tail, NULL, NULL));
                xkvslist_free(&slist);
            }

            {
                XKVSList_PT slist = xkvslist_new();

                void *key = NULL;
                void *value = NULL;

                xkvslist_push_back_repeat(slist, "key1", "value1");
                xkvslist_push_back_repeat(slist, "key2", "value2");
                xkvslist_push_back_repeat(slist, "key3", "value3");
                xkvslist_push_back_repeat(slist, "key4", "value4");
                xkvslist_push_back_repeat(slist, "key5", "value5");

                {
                    XRSList_PT node = xkvslist_front_node(slist)->next->next;
                    xkvslist_remove_after(slist, node, &key, &value);
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
                    XRSList_PT node = xkvslist_front_node(slist)->next->next;
                    xkvslist_remove_after(slist, node, &key, &value);
                    /*
                    *  key1, value1
                    *  key2, value2
                    *  key3, value3
                    */
                    xassert(strcmp(key, "key5") == 0);
                    xassert(strcmp(value, "value5") == 0);
                }

                xassert(strcmp(xpair_first(slist->head->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->head->value), "value1") == 0);
                xassert(strcmp(xpair_first(slist->tail->value), "key3") == 0);
                xassert(strcmp(xpair_second(slist->tail->value), "value3") == 0);
                xassert(slist->size == 3);

                xkvslist_free(&slist);
            }
        }

        /* xkvslist_remove_if_impl */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_remove_if_impl(NULL, false, false, NULL, NULL, NULL, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* slist is empty */
            {
                XKVSList_PT slist = xkvslist_new();
                xassert(xkvslist_remove_if_impl(slist, false, false, xkvslist_test_cmp_false, NULL, NULL, NULL, NULL) == 0);
                xkvslist_free(&slist);
            }

            /* no any key match */
            {
                XKVSList_PT slist = xkvslist_new();
                xkvslist_push_back_repeat(slist, "key1", "value1");
                xkvslist_push_back_repeat(slist, "key2", "value2");
                xkvslist_push_back_repeat(slist, "key1", "value12");

                xassert(xkvslist_remove_if_impl(slist, false, false, xkvslist_test_cmp_false, NULL, NULL, NULL, NULL) == 0);
                xassert(xkvslist_remove_if_impl(slist, false, false, NULL, NULL, NULL, xkvslist_test_applykv_false, NULL) == 0);

                xassert(xkvslist_remove_if_impl(slist, false, false, test_cmp, NULL, NULL, NULL, "key3") == 0);
                xassert(xkvslist_remove_if_impl(slist, false, false, NULL, NULL, NULL, xkvslist_test_applykv, "key3") == 0);

                xkvslist_free(&slist);
            }

            /* remove one */
            {
                XKVSList_PT slist = xkvslist_new();
                xkvslist_push_back_repeat(slist, "key1", "value1");
                xkvslist_push_back_repeat(slist, "key1", "value12");
                xkvslist_push_back_repeat(slist, "key1", "value13");
                xkvslist_push_back_repeat(slist, "key1", "value14");

                xassert(xkvslist_remove_if_impl(slist, false, true, test_cmp, NULL, NULL, NULL, "key1") == 1);
                xassert(strcmp(xpair_second(slist->head->value), "value12") == 0);
                xassert(xkvslist_remove_if_impl(slist, false, true, NULL, NULL, NULL, xkvslist_test_applykv, "key1") == 1);
                xassert(strcmp(xpair_second(slist->head->value), "value13") == 0);

                xkvslist_free(&slist);
            }

            /* remove all */
            {
                XKVSList_PT slist = xkvslist_new();
                xkvslist_push_back_repeat(slist, "key1", "value1");
                xkvslist_push_back_repeat(slist, "key2", "value2");
                xkvslist_push_back_repeat(slist, "key1", "value12");
                xkvslist_push_back_repeat(slist, "key1", "value13");
                xkvslist_push_back_repeat(slist, "key2", "value2");
                xkvslist_push_back_repeat(slist, "key1", "value14");

                xassert(xkvslist_remove_if_impl(slist, false, false, test_cmp, NULL, NULL, NULL, "key1") == 4);
                xassert(xkvslist_find(slist, "key1") == NULL);
                xassert(xkvslist_remove_if_impl(slist, false, false, NULL, NULL, NULL, xkvslist_test_applykv, "key2") == 2);
                xassert(xkvslist_find(slist, "key2") == NULL);
                xassert(slist->size == 0);

                xkvslist_free(&slist);
            }

            /* deep remove */
            {
                XKVSList_PT slist = xkvslist_new();
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

                xkvslist_push_back_repeat(slist, key1, value1);
                xkvslist_push_back_repeat(slist, key2, value2);
                xkvslist_push_back_repeat(slist, key3, value3);

                xassert(xkvslist_remove_if_impl(slist, true, false, test_cmp, NULL, NULL, NULL, "key1") == 2);
                xassert(slist->size == 1);

                xkvslist_deep_free(&slist);
            }
        }

        /* all below remove functions tested by xkvslist_remove_if_impl already */

        /* xkvslist_remove */
        {
            /* key == NULL */
            {
                XKVSList_PT slist = xkvslist_new();
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_remove(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xkvslist_free(&slist);
            }
        }

        /* xkvslist_remove_all */
        {
            /* key == NULL */
            {
                XKVSList_PT slist = xkvslist_new();
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_remove_all(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xkvslist_free(&slist);
            }
        }

        /* xkvslist_deep_remove */
        {
            /* key == NULL */
            {
                XKVSList_PT slist = xkvslist_new();
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_deep_remove(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xkvslist_free(&slist);
            }

            /* *key == NULL */
            {
                XKVSList_PT slist = xkvslist_new();
                void *str = NULL;
                void **key = &str;

                bool except = false;
                XEXCEPT_TRY
                    xkvslist_deep_remove(slist, key);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xkvslist_free(&slist);
            }
        }

        /* xkvslist_remove_apply_if */
        /* xkvslist_remove_apply_key_if */
        /* xkvslist_remove_equal_if */
        /* xkvslist_remove_apply_break_if */
        /* xkvslist_remove_apply_key_break_if */
        /* xkvslist_remove_equal_break_if */
        /* xkvslist_deep_remove_apply_if */
        /* xkvslist_deep_remove_apply_key_if */
        /* xkvslist_deep_remove_equal_if */
        /* xkvslist_deep_remove_apply_break_if */
        /* xkvslist_deep_remove_apply_key_break_if */
        /* xkvslist_deep_remove_equal_break_if */

        /* below two free functions tested by other functions already */
        /* xkvslist_free */
        /* xkvslist_deep_free */


        /* xkvslist_clear */
        {
        }

        /* xkvslist_deep_clear */
        {
        }

        /* xkvslist_swap */
        {
            /* slist1 == NULL */
            {
                XKVSList_PT slist2 = xkvslist_new();
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_swap(NULL, slist2);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xkvslist_free(&slist2);
            }

            /* slist2 == NULL */
            {
                XKVSList_PT slist1 = xkvslist_new();
                bool except = false;
                XEXCEPT_TRY
                    xkvslist_swap(slist1, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xkvslist_free(&slist1);
            }

            /* normal */
            {
                XKVSList_PT slist1 = xkvslist_new();
                XKVSList_PT slist2 = xkvslist_new();

                xkvslist_push_back_repeat(slist1, "key1", "value1");
                xkvslist_push_back_repeat(slist1, "key2", "value2");
                xkvslist_push_back_repeat(slist1, "key3", "value3");
                xkvslist_push_back_repeat(slist1, "key4", "value4");

                xkvslist_push_back_repeat(slist2, "key5", "value5");
                xkvslist_push_back_repeat(slist2, "key6", "value6");
                xkvslist_push_back_repeat(slist2, "key7", "value7");
                xkvslist_push_back_repeat(slist2, "key8", "value8");
                xkvslist_push_back_repeat(slist2, "key9", "value9");

                xassert(xkvslist_swap(slist1, slist2));

                xassert(strcmp(xpair_first(slist1->head->value), "key5") == 0);
                xassert(strcmp(xpair_second(slist1->head->value), "value5") == 0);
                xassert(strcmp(xpair_first(slist1->tail->value), "key9") == 0);
                xassert(strcmp(xpair_second(slist1->tail->value), "value9") == 0);

                xassert(strcmp(xpair_first(slist2->head->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist2->head->value), "value1") == 0);
                xassert(strcmp(xpair_first(slist2->tail->value), "key4") == 0);
                xassert(strcmp(xpair_second(slist2->tail->value), "value4") == 0);

                xassert(slist1->size == 5);
                xassert(slist2->size == 4);

                xkvslist_free(&slist1);
                xkvslist_free(&slist2);
            }
        }

        /* xkvslist_merge */
        {
            {
                XKVSList_PT slist1 = xkvslist_new();
                XKVSList_PT slist2 = xkvslist_new();

                xkvslist_push_back_repeat(slist1, "key1", "value1");
                xkvslist_push_back_repeat(slist1, "key2", "value2");
                xkvslist_push_back_repeat(slist2, "key3", "value32");
                xkvslist_push_back_repeat(slist2, "key4", "value42");

                xassert(xkvslist_merge(slist1, &slist2));

                xassert_false(slist2);

                xassert(strcmp(xpair_first(slist1->head->value), "key1") == 0);
                xassert(strcmp(xpair_first(slist1->tail->value), "key4") == 0);

                xassert(strcmp(xpair_first(slist1->head->next->next->value), "key3") == 0);

                xassert(slist1->size == 4);

                xkvslist_free(&slist1);
            }
        }

        /* xkvslist_reverse */
        {
            XKVSList_PT slist = xkvslist_new();

            xkvslist_reverse(slist);

            xkvslist_push_back_repeat(slist, "key1", "value1");
            xkvslist_push_back_repeat(slist, "key2", "value2");
            xkvslist_push_back_repeat(slist, "key3", "value3");
            xkvslist_push_back_repeat(slist, "key4", "value4");

            xkvslist_reverse(slist);

            xassert(strcmp(xpair_first(slist->head->value), "key4") == 0);
            xassert(strcmp(xpair_second(slist->head->value), "value4") == 0);
            xassert(strcmp(xpair_first(slist->head->next->value), "key3") == 0);
            xassert(strcmp(xpair_second(slist->head->next->value), "value3") == 0);
            xassert(strcmp(xpair_first(slist->head->next->next->value), "key2") == 0);
            xassert(strcmp(xpair_second(slist->head->next->next->value), "value2") == 0);
            xassert(strcmp(xpair_first(slist->tail->value), "key1") == 0);
            xassert(strcmp(xpair_second(slist->tail->value), "value1") == 0);

            xassert(slist->size == 4);

            xkvslist_free(&slist);
        }

        /* xkvslist_map */
        {
            XKVSList_PT slist = xkvslist_new();

            xkvslist_push_back_repeat(slist, "key1", "value1");
            xkvslist_push_back_repeat(slist, "key2", "value2");
            xkvslist_push_back_repeat(slist, "key3", "value3");
            xkvslist_push_back_repeat(slist, "key4", "value4");

            int count = 0;

            xassert(xkvslist_map(slist, xkvslist_test_map_apply_count, (void*)&count) == 4);
            xassert(count == 4);

            xkvslist_free(&slist);
        }

        /* xkvslist_map_break_if_true */

        /* xkvslist_map_break_if_false */

        /* xkvslist_map_key */

        /* xkvslist_map_key_break_if_true */

        /* xkvslist_map_key_break_if_false */

        /* xkvslist_find */
        {
            XKVSList_PT slist = xkvslist_new();

            xkvslist_push_back_repeat(slist, "key1", "value1");
            xkvslist_push_back_repeat(slist, "key2", "value2");
            xkvslist_push_back_repeat(slist, "key3", "value3");
            xkvslist_push_back_repeat(slist, "key4", "value4");

            XRSList_PT node = xkvslist_find(slist, "key2");
            xassert(strcmp(xpair_first(node->value), "key2") == 0);
            xassert(strcmp(xpair_second(node->value), "value2") == 0);

            xkvslist_free(&slist);
        }

        /* xkvslist_find_equal_if */

        /* xkvslist_find_apply_if */

        /* xkvslist_find_apply_key_if */

        /* xkvslist_find_apply_key_if */

        /* xkvslist_to_array */
        {
            XKVSList_PT slist = xkvslist_new();

            xkvslist_push_back_repeat(slist, "key1", "value1");
            xkvslist_push_back_repeat(slist, "key2", "value2");
            xkvslist_push_back_repeat(slist, "key3", "value3");
            xkvslist_push_back_repeat(slist, "key4", "value4");

            XPArray_PT array = xkvslist_to_array(slist);

            xassert(array->size == 8);
            xassert(strcmp((char*)xparray_get(array, 2), "key2") == 0);

            xparray_free(&array);
            xkvslist_free(&slist);
        }

        /* xkvslist_keys_to_array */
        /* xkvslist_values_to_array */

        /* xkvslist_size */

        /* xkvslist_is_empty */

        /* xkvslist_unique */
        {
            XKVSList_PT slist = xkvslist_new();

            xkvslist_push_back_repeat(slist, "key2", "value2");
            xkvslist_push_back_repeat(slist, "key1", "value12");
            xkvslist_push_back_repeat(slist, "key1", "value11");
            xkvslist_push_back_repeat(slist, "key3", "value3");
            xkvslist_push_back_repeat(slist, "key2", "value21");
            xkvslist_push_back_repeat(slist, "key4", "value4");

            xkvslist_unique(slist, test_cmp, NULL);
            xassert(slist->size == 4);

            xassert(strcmp(xpair_first(slist->head->value), "key1") == 0);
            xassert(strcmp(xpair_first(slist->head->next->value), "key2") == 0);
            xassert(strcmp(xpair_first(slist->head->next->next->value), "key3") == 0);
            xassert(strcmp(xpair_first(slist->head->next->next->next->value), "key4") == 0);

            xkvslist_free(&slist);
        }

        /* xkvslist_deep_unique */
        {
#if 0
            {
                XKVSList_PT slist = tkvslist_random_string(1000);
                xkvslist_deep_unique(slist, test_cmp);
                xkvslist_deep_free(&slist);
            }
#endif
        }

        /* xkvslist_sort */
        {
            {
                XKVSList_PT slist = xkvslist_new();

                xkvslist_push_back_repeat(slist, "key2", "value2");
                xkvslist_push_back_repeat(slist, "key1", "value12");
                xkvslist_push_back_repeat(slist, "key1", "value11");
                xkvslist_push_back_repeat(slist, "key3", "value3");
                xkvslist_push_back_repeat(slist, "key5", "value5");
                xkvslist_push_back_repeat(slist, "key4", "value4");

                xkvslist_sort(slist, test_cmp, NULL);

                xassert(strcmp(xpair_first(slist->head->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->head->value), "value12") == 0);
                xassert(strcmp(xpair_first(slist->head->next->value), "key1") == 0);
                xassert(strcmp(xpair_second(slist->head->next->value), "value11") == 0);
                xassert(strcmp(xpair_first(slist->head->next->next->value), "key2") == 0);
                xassert(strcmp(xpair_first(slist->head->next->next->next->value), "key3") == 0);
                xassert(strcmp(xpair_first(slist->head->next->next->next->next->value), "key4") == 0);
                xassert(strcmp(xpair_first(slist->head->next->next->next->next->next->value), "key5") == 0);

                xkvslist_free(&slist);
            }
#if 0
            {
                XKVSList_PT slist = tkvslist_random_string(1000);
                xkvslist_sort(slist, test_cmp);
                xkvslist_deep_free(&slist);
            }
#endif
        }

        /* xkvslist_sorted */
        {
            /* tested by xkvslist_sort already */
        }

        {
            int count = 0;
            xmem_leak(xkvslist_test_check_mem_leak, &count);
            xassert(count == 0);
        }
}


