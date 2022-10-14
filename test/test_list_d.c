
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../array_pointer/xarray_pointer_x.h"
#include "../list_d_raw/xlist_d_raw_x.h"
#include "../list_d/xlist_d_x.h"
#include "../include/xalgos.h"

static
int xdlist_test_cmp_true(void *value1, void *value2, void *cl) {
    return 0;
}

static
int xdlist_test_cmp_false(void *value1, void *value2, void *cl) {
    return 1;
}

static 
int xdlist_test_cmp(void *value1, void *value2, void *cl) {
    return value1 == value2 ? 0 : 1;
}

static
int test_cmp(void *value1, void *value2, void *cl) {
    return strcmp((char*)value1, (char*)value2);
}

static
bool xdlist_test_apply(void *value, void *cl) {
    return strcmp((char*)value, (char*)cl) == 0;
}

static
bool xdlist_test_apply_true(void *value, void *cl) {
    return true;
}

static
bool xdlist_test_apply_false(void *value, void *cl) {
    return false;
}

static
bool xdlist_test_map_apply_count(void *value, void *cl) {
    ++(*((int*)(cl)));
    return true;
}

static
bool xdlist_test_print(void *value, void *cl) {
    printf("%s\n", (char*)value);
    return true;
}

static 
XDList_PT xdlist_random_string(int size) {
    XDList_PT dlist = xdlist_new();
    if (!dlist) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xdlist_deep_free(&dlist);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }
            str[str_size - 1] = '\0';

            xdlist_push_back_repeat(dlist, str);
        }
    }

    return dlist;
}

static
void xdlist_test_check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}


void test_xdlist() {

        /* xdlist_new */
        {
            XDList_PT dlist = xdlist_new();
            xassert(dlist);
            xassert(dlist->head == NULL);
            xassert(dlist->tail == NULL);
            xassert(dlist->size == 0);
            xdlist_free(&dlist);
        }

        /* xdlist_vload_repeat */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdlist_vload_repeat(NULL, "value", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XDList_PT dlist = xdlist_new();
                xassert(xdlist_vload_repeat(dlist, NULL) == 0);
                xdlist_free(&dlist);
            }

            /* normal */
            {
                XDList_PT dlist = xdlist_new();
                xassert(xdlist_vload_repeat(dlist, "k1", "k2", "k1", "k3", NULL) == 4);
                xassert(strcmp(dlist->head->value, "k1") == 0);
                xdlist_free(&dlist);
            }
        }

        /* xdlist_vload_unique */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdlist_vload_unique(NULL, "value", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XDList_PT dlist = xdlist_new();
                xassert(xdlist_vload_unique(dlist, NULL) == 0);
                xdlist_free(&dlist);
            }

            /* repeat value */
            {
                XDList_PT dlist = xdlist_new();
                xassert(xdlist_vload_unique(dlist, "k1", "k2", "k1", "k3", NULL) == 3);
                xassert(strcmp(dlist->head->value, "k1") == 0);
                xdlist_free(&dlist);
            }
        }

        /* xdlist_vload_unique_if */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdlist_vload_unique_if(NULL, xdlist_test_cmp, NULL, "k", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }
            /* value == NULL */
            {
                XDList_PT dlist = xdlist_new();
                xassert(xdlist_vload_unique_if(dlist, xdlist_test_cmp, NULL, NULL) == 0);
                xdlist_free(&dlist);
            }

            /* repeat value */
            {
                XDList_PT dlist = xdlist_new();
                xassert(xdlist_vload_unique_if(dlist, xdlist_test_cmp, NULL, "k1", "k2", "k1", "k3", NULL) == 3);
                xassert(strcmp(dlist->head->value, "k1") == 0);
                xdlist_free(&dlist);
            }
        }

        /* xdlist_vload_replace_if */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdlist_vload_replace_if(NULL, xdlist_test_cmp, NULL, "k", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }
            /* value == NULL */
            {
                XDList_PT dlist = xdlist_new();
                xassert(xdlist_vload_replace_if(dlist, xdlist_test_cmp, NULL, NULL) == 0);
                xdlist_free(&dlist);
            }

            /* repeat value */
            {
                XDList_PT dlist = xdlist_new();
                xassert(xdlist_vload_replace_if(dlist, xdlist_test_cmp, NULL, "k1", "k2", "k1", "k3", NULL) == 3);
                xassert(strcmp(dlist->head->value, "k1") == 0);
                xdlist_free(&dlist);
            }
        }

        /* xdlist_aload_repeat */
        {
            /* dlist == NULL */
            {
                XPArray_PT array = xparray_new(10);
                xparray_put(array, 0, "k", NULL);
                bool except = false;
                XEXCEPT_TRY
                    xdlist_aload_repeat(NULL, array);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xparray_free(&array);
            }

            /* xs == NULL */
            {
                XDList_PT dlist = xdlist_new();
                bool except = false;
                XEXCEPT_TRY
                    xdlist_aload_repeat(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xdlist_free(&dlist);
            }

            /* value == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XDList_PT dlist = xdlist_new();

                xassert(xdlist_aload_repeat(dlist, array) == 0);

                xdlist_free(&dlist);
                xparray_free(&array);
            }

            /* normal */
            {
                XPArray_PT array = xparray_new(10);
                XDList_PT dlist = xdlist_new();

                xparray_vload(array, "k1", "k2", "k1", "k3", NULL);
                xassert(xdlist_aload_repeat(dlist, array) == 4);
                xassert(strcmp(dlist->head->value, "k1") == 0);

                xdlist_free(&dlist);
                xparray_free(&array);
            }
        }

        /* xdlist_aload_unique */
        {
            /* dlist == NULL */
            {
                XPArray_PT array = xparray_new(10);
                xparray_put(array, 0, "k", NULL);

                bool except = false;
                XEXCEPT_TRY
                    xdlist_aload_unique(NULL, array);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xparray_free(&array);
            }

            /* xs == NULL */
            {
                XDList_PT dlist = xdlist_new();
                bool except = false;
                XEXCEPT_TRY
                    xdlist_aload_unique(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xdlist_free(&dlist);
            }

            /* value == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XDList_PT dlist = xdlist_new();

                xassert(xdlist_aload_unique(dlist, array) == 0);

                xdlist_free(&dlist);
                xparray_free(&array);
            }

            /* repeat value */
            {
                XPArray_PT array = xparray_new(10);
                XDList_PT dlist = xdlist_new();

                xparray_vload(array, "k1", "k2", "k1", "k3", NULL);
                xassert(xdlist_aload_unique(dlist, array) == 3);
                xassert(strcmp(dlist->head->value, "k1") == 0);

                xdlist_free(&dlist);
                xparray_free(&array);
            }
        }

        /* xdlist_aload_unique_if */
        {
            /* cmp == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XDList_PT dlist = xdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdlist_aload_unique_if(dlist, array, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdlist_free(&dlist);
                xparray_free(&array);
            }

            /* below 3 scenarios tested already in xdlist_aload_unique */
            /* xs == NULL */
            /* value == NULL */
            /* value == NULL */

            /* repeat value */
            {
                XPArray_PT array = xparray_new(10);
                XDList_PT dlist = xdlist_new();

                xparray_vload(array, "k1", "k2", "k1", "k3", NULL);
                xassert(xdlist_aload_unique_if(dlist, array, xdlist_test_cmp, NULL) == 3);
                xassert(strcmp(dlist->head->value, "k1") == 0);

                xdlist_free(&dlist);
                xparray_free(&array);
            }
        }

        /* xdlist_aload_replace_if */
        {
            /* cmp == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XDList_PT dlist = xdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdlist_aload_replace_if(dlist, array, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);

                xdlist_free(&dlist);
                xparray_free(&array);
            }

            /* below 3 scenarios tested already in xdlist_aload_replace */
            /* xs == NULL */
            /* value == NULL */
            /* value == NULL */

            /* repeat value */
            {
                XPArray_PT array = xparray_new(10);
                XDList_PT dlist = xdlist_new();

                xparray_vload(array, "k1", "k2", "k1", "k3", NULL);
                xassert(xdlist_aload_replace_if(dlist, array, xdlist_test_cmp, NULL) == 3);
                xassert(strcmp(dlist->head->value, "k1") == 0);

                xdlist_free(&dlist);
                xparray_free(&array);
            }
        }

        /* xdlist_copyn_if_impl */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdlist_copyn_if_impl(NULL, 0, 1, false, xdlist_test_cmp, NULL, NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* count < 0 */
            {
                XDList_PT dlist = xdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdlist_copyn_if_impl(dlist, 0, -1, false, xdlist_test_cmp, NULL, xdlist_test_apply_true, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdlist_free(&dlist);
            }

            /* deep == true && value_size < 0 */
            {
                XDList_PT dlist = xdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdlist_copyn_if_impl(dlist, -1, 1, true, xdlist_test_cmp, NULL, xdlist_test_apply_true, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdlist_free(&dlist);
            }

            /* deep == true && value_size == 0 */
            {
                XDList_PT dlist = xdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdlist_copyn_if_impl(dlist, 0, 1, true, xdlist_test_cmp, NULL, xdlist_test_apply_true, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdlist_free(&dlist);
            }

            /* dlist is empty */
            {
                XDList_PT dlist = xdlist_new();

                {
                    XDList_PT ndlist = xdlist_copyn_if_impl(dlist, 0, 1, false, NULL, NULL, xdlist_test_apply_true, NULL);
                    xassert(ndlist->size == 0);
                    xdlist_free(&ndlist);
                }

                xdlist_free(&dlist);
            }

            /* count == 0 */
            {
                XDList_PT dlist = xdlist_new();
                xdlist_vload_repeat(dlist, "k1", "k2", NULL);

                {
                    XDList_PT ndlist = xdlist_copyn_if_impl(dlist, 0, 0, false, NULL, NULL, xdlist_test_apply_true, NULL);
                    xassert(ndlist->size == 0);
                    xdlist_free(&ndlist);
                }

                xdlist_free(&dlist);
            }

            /* dlist->size < count */
            {
                XDList_PT dlist = xdlist_new();
                xdlist_vload_repeat(dlist, "k1", "k1", NULL);

                {
                    XDList_PT ndlist = xdlist_copyn_if_impl(dlist, 0, 3, false, NULL, NULL, xdlist_test_apply_true, NULL);
                    xassert(ndlist->size == 2);
                    xdlist_free(&ndlist);
                }

                xdlist_free(&dlist);
            }

            /* count < dlist->size */
            {
                XDList_PT dlist = xdlist_new();
                xdlist_vload_repeat(dlist, "k1", "k1", NULL);

                {
                    XDList_PT ndlist = xdlist_copyn_if_impl(dlist, 0, 1, false, NULL, NULL, xdlist_test_apply_true, NULL);
                    xassert(ndlist->size == 1);
                    xassert(strcmp(ndlist->head->value, "k1") == 0);
                    xdlist_free(&ndlist);
                }

                xdlist_free(&dlist);
            }

            /* no any value match */
            {
                XDList_PT dlist = xdlist_new();
                xdlist_vload_repeat(dlist, "k1", "k2", NULL);

                {
                    XDList_PT ndlist = xdlist_copyn_if_impl(dlist, 0, 1, false, NULL, NULL, xdlist_test_apply_false, NULL);
                    xassert(ndlist->size == 0);
                    xdlist_free(&ndlist);
                }

                xdlist_free(&dlist);
            }

            /* normal */
            {
                XDList_PT dlist = xdlist_new();
                xdlist_vload_repeat(dlist, "k1", "k1", "k3", NULL);

                {
                    XDList_PT ndlist = xdlist_copyn_if_impl(dlist, 0, 2, false, NULL, NULL, xdlist_test_apply_true, NULL);
                    xassert(ndlist->size == 2);
                    xassert(strcmp(ndlist->head->value, "k1") == 0);
                    xassert(strcmp(ndlist->head->next->value, "k1") == 0);
                    xdlist_free(&ndlist);
                }

                {
                    XDList_PT ndlist = xdlist_copyn_if_impl(dlist, 0, 2, false, NULL, NULL, xdlist_test_apply, "k3");
                    xassert(ndlist->size == 1);
                    xassert(strcmp(ndlist->head->value, "k3") == 0);
                    xdlist_free(&ndlist);
                }

                {
                    XDList_PT ndlist = xdlist_copyn_if_impl(dlist, 0, 2, false, test_cmp, NULL, NULL, "k3");
                    xassert(ndlist->size == 1);
                    xassert(strcmp(ndlist->head->value, "k3") == 0);
                    xdlist_free(&ndlist);
                }

                xdlist_free(&dlist);
            }
        }

        /* xdlist_copyn_equal_if */
        {
            /* value == NULL */
            {
                XDList_PT dlist = xdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdlist_copyn_equal_if(dlist, 1, NULL, xdlist_test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdlist_free(&dlist);
            }
        }

        /* xdlist_deep_copyn_equal_if */
        {
            /* value == NULL */
            {
                XDList_PT dlist = xdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdlist_deep_copyn_equal_if(dlist, 1, 1, NULL, xdlist_test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);

                xdlist_free(&dlist);
            }
        }
                
        /* xdlist_copy */
        /* xdlist_copy_equal_if */
        /* xdlist_copy_apply_if */
        /* xdlist_copy_apply_value_if */
        /* xdlist_copyn */
        /* xdlist_copyn_equal_if */
        /* xdlist_copyn_apply_if */
        /* xdlist_copyn_apply_value_if */
        /* xdlist_deep_copy */
        /* xdlist_deep_copy_equal_if */
        /* xdlist_deep_copy_apply_if */
        /* xdlist_deep_copy_apply_value_if */
        /* xdlist_deep_copyn */
        /* xdlist_deep_copyn_equal_if */
        /* xdlist_deep_copyn_apply_if */
        /* xdlist_deep_copyn_apply_value_if */
        {
            /* tested by xdlist_copyn_if_impl already */
        }

        /* xdlist_push_front_repeat */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdlist_push_front_repeat(NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XDList_PT dlist = xdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdlist_push_front_repeat(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdlist_free(&dlist);
            }

            /* normal */
            {
                XDList_PT dlist = xdlist_new();

                char *str = "value1";

                xdlist_push_front_repeat(dlist, str);
                xdlist_push_front_repeat(dlist, "value2");
                xdlist_push_front_repeat(dlist, str);
                xdlist_push_front_repeat(dlist, "value3");

                xassert(strcmp(dlist->head->value, "value3") == 0);
                xassert(strcmp(dlist->head->next->value, "value1") == 0);
                xassert(strcmp(dlist->head->next->next->value, "value2") == 0);
                xassert(strcmp(dlist->head->next->next->next->value, "value1") == 0);
                xassert_false(dlist->head->next->next->next->next);
                xassert(strcmp(dlist->tail->value, "value1") == 0);

                xassert(dlist->size == 4);

                xdlist_free(&dlist);
            }
        }

        /* xdlist_pop_front */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdlist_pop_front(NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* dlist is empty */
            {
                XDList_PT dlist = xdlist_new();
                xassert(xdlist_pop_front(dlist) == false);
                xdlist_free(&dlist);
            }

            {
                XDList_PT dlist = xdlist_new();

                xdlist_push_back_repeat(dlist, "value1");
                xdlist_push_back_repeat(dlist, "value2");
                xdlist_push_back_repeat(dlist, "value3");

                {
                    void *value = NULL;

                    xassert(value = xdlist_pop_front(dlist));
                    xassert(strcmp(value, "value1") == 0);

                    xassert(value = xdlist_pop_front(dlist));
                    xassert(strcmp(value, "value2") == 0);

                    xassert(xdlist_pop_front(dlist));

                    xassert(dlist->size == 0);
                    xassert(dlist->head == NULL);
                    xassert(dlist->tail == NULL);
                }

                xdlist_free(&dlist);
            }
        }

        /* xdlist_push_back_repeat */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdlist_push_back_repeat(NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XDList_PT dlist = xdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdlist_push_back_repeat(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdlist_free(&dlist);
            }

            /* normal */
            {
                XDList_PT dlist = xdlist_new();

                char *str = "value1";

                xdlist_push_back_repeat(dlist, str);
                xdlist_push_back_repeat(dlist, "value2");
                xdlist_push_back_repeat(dlist, str);
                xdlist_push_back_repeat(dlist, "value3");

                xassert(strcmp(dlist->head->value, "value1") == 0);
                xassert(strcmp(dlist->head->next->value, "value2") == 0);
                xassert(strcmp(dlist->head->next->next->value, "value1") == 0);
                xassert(strcmp(dlist->head->next->next->next->value, "value3") == 0);
                xassert_false(dlist->head->next->next->next->next);
                xassert(strcmp(dlist->tail->value, "value3") == 0);

                xassert(dlist->size == 4);

                xdlist_free(&dlist);
            }
        }

        /* xdlist_pop_back */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdlist_pop_back(NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* dlist is empty */
            {
                XDList_PT dlist = xdlist_new();
                xassert(xdlist_pop_back(dlist) == false);
                xdlist_free(&dlist);
            }

            {
                XDList_PT dlist = xdlist_new();

                xdlist_push_back_repeat(dlist, "value1");
                xdlist_push_back_repeat(dlist, "value2");
                xdlist_push_back_repeat(dlist, "value3");

                {
                    void *value = NULL;

                    xassert(value = xdlist_pop_back(dlist));
                    xassert(strcmp(value, "value3") == 0);

                    xassert(value = xdlist_pop_back(dlist));
                    xassert(strcmp(value, "value2") == 0);

                    xassert(xdlist_pop_back(dlist));

                    xassert(dlist->size == 0);
                    xassert(dlist->head == NULL);
                    xassert(dlist->tail == NULL);
                }

                xdlist_free(&dlist);
            }
        }

        /* xdlist_front */
        {
            /* dlist is empty */
            {
                XDList_PT dlist = xdlist_new();
                xassert_false(xdlist_front(dlist));
                xdlist_free(&dlist);
            }

            /* normal */
            {
                XDList_PT dlist = xdlist_new();

                xdlist_push_back_repeat(dlist, "value1");
                xdlist_push_back_repeat(dlist, "value2");
                xdlist_push_back_repeat(dlist, "value3");

                xassert(strcmp(xdlist_front(dlist), "value1") == 0);

                xdlist_free(&dlist);
            }
        }

        /* xdlist_back */
        {
            /* dlist is empty */
            {
                XDList_PT dlist = xdlist_new();
                xassert_false(xdlist_back(dlist));
                xdlist_free(&dlist);
            }

            /* normal */
            {
                XDList_PT dlist = xdlist_new();

                xdlist_push_back_repeat(dlist, "value1");
                xdlist_push_back_repeat(dlist, "value2");
                xdlist_push_back_repeat(dlist, "value3");

                xassert(strcmp(xdlist_back(dlist), "value3") == 0);

                xdlist_free(&dlist);
            }
        }

        /* xdlist_front_node */
        {
            /* dlist is empty */
            {
                XDList_PT dlist = xdlist_new();
                xassert_false(xdlist_front_node(dlist));
                xdlist_free(&dlist);
            }

            /* normal */
            {
                XDList_PT dlist = xdlist_new();

                xdlist_push_back_repeat(dlist, "value1");
                xdlist_push_back_repeat(dlist, "value2");
                xdlist_push_back_repeat(dlist, "value3");

                xassert(strcmp(xdlist_front_node(dlist)->value, "value1") == 0);

                xdlist_free(&dlist);
            }
        }

        /* xdlist_back_node */
        {
            /* dlist is empty */
            {
                XDList_PT dlist = xdlist_new();
                xassert_false(xdlist_back_node(dlist));
                xdlist_free(&dlist);
            }

            /* normal */
            {
                XDList_PT dlist = xdlist_new();

                xdlist_push_back_repeat(dlist, "value1");
                xdlist_push_back_repeat(dlist, "value2");
                xdlist_push_back_repeat(dlist, "value3");

                xassert(strcmp(xdlist_back_node(dlist)->value, "value3") == 0);

                xdlist_free(&dlist);
            }
        }

        /* xdlist_push_front_unique */
        {
            XDList_PT dlist = xdlist_new();

            char *str = "value1"; 

            xdlist_push_front_unique(dlist, str);
            xdlist_push_front_unique(dlist, "value2");
            xdlist_push_front_unique(dlist, str);
            xdlist_push_front_unique(dlist, "value3");

            xassert(strcmp(dlist->head->value, "value3") == 0);
            xassert(strcmp(dlist->head->next->value, "value2") == 0);
            xassert(strcmp(dlist->head->next->next->value, "value1") == 0);
            xassert_false(dlist->head->next->next->next);
            xassert(strcmp(dlist->tail->value, "value1") == 0);
            xassert(dlist->size == 3);

            xdlist_free(&dlist);
        }

        /* xdlist_push_front_unique_if */
        {
            XDList_PT dlist = xdlist_new();

            xdlist_push_front_unique_if(dlist, "value1", test_cmp, NULL);
            xdlist_push_front_unique_if(dlist, "value2", test_cmp, NULL);
            xdlist_push_front_unique_if(dlist, "value1", test_cmp, NULL);
            xdlist_push_front_unique_if(dlist, "value3", test_cmp, NULL);

            xassert(strcmp(dlist->head->value, "value3") == 0);
            xassert(strcmp(dlist->head->next->value, "value2") == 0);
            xassert(strcmp(dlist->head->next->next->value, "value1") == 0);
            xassert(strcmp(dlist->tail->value, "value1") == 0);
            xassert_false(dlist->head->next->next->next);
            xassert(dlist->size == 3);

            xdlist_free(&dlist);
        }

        /* xdlist_push_front_replace_if */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdlist_push_front_replace_if(NULL, "k", test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
            }

            /* value == NULL */
            {
                XDList_PT dlist = xdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdlist_push_front_replace_if(dlist, NULL, test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdlist_free(&dlist);
            }
            
            /* repeat value */
            {
                XDList_PT dlist = xdlist_new();

                xdlist_push_front_replace_if(dlist, "value1", test_cmp, NULL);
                xdlist_push_front_replace_if(dlist, "value2", test_cmp, NULL);
                xdlist_push_front_replace_if(dlist, "value1", test_cmp, NULL);
                xdlist_push_front_replace_if(dlist, "value3", test_cmp, NULL);

                xassert(strcmp(dlist->head->value, "value3") == 0);
                xassert(strcmp(dlist->head->next->value, "value2") == 0);
                xassert(strcmp(dlist->head->next->next->value, "value1") == 0);
                xassert(strcmp(dlist->tail->value, "value1") == 0);
                xassert_false(dlist->head->next->next->next);
                xassert(dlist->size == 3);

                xdlist_free(&dlist);
            }
        }

        /* xdlist_push_front_deep_replace_if */
        {
            /* below scenarios tested by xdlist_push_front_replace_if already */
            /* dlist == NULL */
            /* value == NULL */
            /* cmp == NULL */

            /* repeat value */
            {
                XDList_PT dlist = xdlist_new();

                char *value = XMEM_CALLOC(1, 7);
                memcpy(value, "value1", 6);

                xdlist_push_front_deep_replace_if(dlist, value, test_cmp, NULL);
                xdlist_push_front_deep_replace_if(dlist, "value2", test_cmp, NULL);
                xdlist_push_front_deep_replace_if(dlist, "value1", test_cmp, NULL);
                xdlist_push_front_deep_replace_if(dlist, "value3", test_cmp, NULL);

                xassert(strcmp(dlist->head->value, "value3") == 0);
                xassert(strcmp(dlist->head->next->value, "value2") == 0);
                xassert(strcmp(dlist->head->next->next->value, "value1") == 0);
                xassert(strcmp(dlist->tail->value, "value1") == 0);
                xassert_false(dlist->head->next->next->next);
                xassert(dlist->size == 3);

                xdlist_free(&dlist);
            }
        }

        /* xdlist_push_back_unique */
        {
            XDList_PT dlist = xdlist_new();

            char *str = "value1";

            xdlist_push_back_unique(dlist, str);
            xdlist_push_back_unique(dlist, "value2");
            xdlist_push_back_unique(dlist, str);
            xdlist_push_back_unique(dlist, "value3");

            xassert(strcmp(dlist->head->value, "value1") == 0);
            xassert(strcmp(dlist->head->next->value, "value2") == 0);
            xassert(strcmp(dlist->head->next->next->value, "value3") == 0);
            xassert(strcmp(dlist->tail->value, "value3") == 0);
            xassert_false(dlist->head->next->next->next);
            xassert(dlist->size == 3);

            xdlist_free(&dlist);
        }

        /* xdlist_push_back_unique_if */
        {
            XDList_PT dlist = xdlist_new();

            xdlist_push_back_unique_if(dlist, "value1", test_cmp, NULL);
            xdlist_push_back_unique_if(dlist, "value2", test_cmp, NULL);
            xdlist_push_back_unique_if(dlist, "value1", test_cmp, NULL);
            xdlist_push_back_unique_if(dlist, "value3", test_cmp, NULL);

            xassert(strcmp(dlist->head->value, "value1") == 0);
            xassert(strcmp(dlist->head->next->value, "value2") == 0);
            xassert(strcmp(dlist->head->next->next->value, "value3") == 0);
            xassert(strcmp(dlist->tail->value, "value3") == 0);
            xassert_false(dlist->head->next->next->next);
            xassert(dlist->size == 3);

            xdlist_free(&dlist);
        }

        /* xdlist_push_back_replace_if */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdlist_push_back_replace_if(NULL, "k", test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XDList_PT dlist = xdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdlist_push_back_replace_if(dlist, NULL, test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdlist_free(&dlist);
            }

            /* repeat value */
            {
                XDList_PT dlist = xdlist_new();

                xdlist_push_back_replace_if(dlist, "value1", test_cmp, NULL);
                xdlist_push_back_replace_if(dlist, "value2", test_cmp, NULL);
                xdlist_push_back_replace_if(dlist, "value1", test_cmp, NULL);
                xdlist_push_back_replace_if(dlist, "value3", test_cmp, NULL);

                xassert(strcmp(dlist->head->value, "value1") == 0);
                xassert(strcmp(dlist->head->next->value, "value2") == 0);
                xassert(strcmp(dlist->head->next->next->value, "value3") == 0);
                xassert(strcmp(dlist->tail->value, "value3") == 0);
                xassert_false(dlist->head->next->next->next);
                xassert(dlist->size == 3);

                xdlist_free(&dlist);
            }
        }

        /* xdlist_push_back_deep_replace_if */
        {
            /* below scenarios tested by xdlist_push_back_replace_if already */
            /* dlist == NULL */
            /* value == NULL */
            /* cmp == NULL */

            /* repeat value */
            {
                XDList_PT dlist = xdlist_new();

                char *value = XMEM_CALLOC(1, 7);
                memcpy(value, "value1", 6);

                xdlist_push_back_deep_replace_if(dlist, value, test_cmp, NULL);
                xdlist_push_back_deep_replace_if(dlist, "value2", test_cmp, NULL);
                xdlist_push_back_deep_replace_if(dlist, "value1", test_cmp, NULL);
                xdlist_push_back_deep_replace_if(dlist, "value3", test_cmp, NULL);

                xassert(strcmp(dlist->head->value, "value1") == 0);
                xassert(strcmp(dlist->head->next->value, "value2") == 0);
                xassert(strcmp(dlist->head->next->next->value, "value3") == 0);
                xassert(strcmp(dlist->tail->value, "value3") == 0);
                xassert_false(dlist->head->next->next->next);
                xassert(dlist->size == 3);

                xdlist_free(&dlist);
            }
        }

        /* xdlist_insert_before */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdlist_insert_before(NULL, NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XDList_PT dlist = xdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdlist_insert_before(dlist, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdlist_free(&dlist);
            }

            {
                XDList_PT dlist = xdlist_new();

                xdlist_push_back_repeat(dlist, "value1");
                xdlist_push_back_repeat(dlist, "value2");
                xdlist_push_back_repeat(dlist, "value3");

                {
                    xdlist_insert_before(dlist, NULL, "value2");
                    /*
                     *  value2
                     *  value1
                     *  value2
                     *  value3
                     */
                    xassert(strcmp(dlist->head->value, "value2") == 0);
                    xassert(strcmp(dlist->head->next->prev->value, "value2") == 0);
                    xassert(strcmp(dlist->head->next->value, "value1") == 0);
                }

                {
                    XRDList_PT node = xdlist_front_node(dlist)->next->next;
                    xdlist_insert_before(dlist, node, "value4");
                    /*
                    * value2
                    * value1
                    * value4
                    * value2
                    * value3
                    */
                    xassert(strcmp(dlist->head->next->next->value, "value4") == 0);
                    xassert(strcmp(dlist->head->next->next->next->prev->value, "value4") == 0);
                }

                xassert(strcmp(dlist->head->value, "value2") == 0);
                xassert(dlist->size == 5);

                xdlist_free(&dlist);
            }
        }

        /* xdlist_insert_after */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdlist_insert_after(NULL, NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XDList_PT dlist = xdlist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdlist_insert_after(dlist, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdlist_free(&dlist);
            }

            {
                XDList_PT dlist = xdlist_new();

                xdlist_push_back_repeat(dlist, "value1");
                xdlist_push_back_repeat(dlist, "value2");
                xdlist_push_back_repeat(dlist, "value3");

                {
                    xdlist_insert_after(dlist, NULL, "value2");
                    /*
                     *  value1
                     *  value2
                     *  value3
                     *  value2
                     */
                    xassert(strcmp(dlist->head->next->next->next->value, "value2") == 0);
                    xassert_false(dlist->head->next->next->next->next);
                    xassert(strcmp(dlist->tail->value, "value2") == 0);
                }

                {
                    XRDList_PT node = xdlist_front_node(dlist)->next->next;
                    xdlist_insert_after(dlist, node, "value4");
                    /*
                    * value1
                    * value2
                    * value3
                    * value4
                    * value2
                    */
                    xassert(strcmp(dlist->head->next->next->next->value, "value4") == 0);
                    xassert(strcmp(dlist->head->next->next->next->next->value, "value2") == 0);
                }

                xassert(strcmp(dlist->head->value, "value1") == 0);
                xassert(dlist->size == 5);

                xdlist_free(&dlist);
            }
        }

        /* xdlist_remove_before */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdlist_remove_before(NULL, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* dlist is empty */
            {
                XDList_PT dlist = xdlist_new();
                xassert_false(xdlist_remove_before(dlist, dlist->head, NULL));
                xdlist_free(&dlist);
            }

            /* node is NULL */
            {
                XDList_PT dlist = xdlist_new();
                xassert_false(xdlist_remove_before(dlist, dlist->head, NULL));
                xdlist_free(&dlist);
            }

            /* node->prev is NULl */
            {
                XDList_PT dlist = xdlist_new();
                xdlist_vload_repeat(dlist, "k", NULL);

                xassert_false(xdlist_remove_before(dlist, dlist->head, NULL));
                xdlist_free(&dlist);
            }

            {
                XDList_PT dlist = xdlist_new();

                void *value = NULL;

                xdlist_push_back_repeat(dlist, "value1");
                xdlist_push_back_repeat(dlist, "value2");
                xdlist_push_back_repeat(dlist, "value3");
                xdlist_push_back_repeat(dlist, "value4");
                xdlist_push_back_repeat(dlist, "value5");

                {
                    XRDList_PT node = xdlist_front_node(dlist)->next->next;
                    xdlist_remove_before(dlist, node, &value);
                    /*
                    *  value1
                    *  value3
                    *  value4
                    *  value5
                    */
                    xassert(strcmp(value, "value2") == 0);
                    xassert(strcmp(dlist->head->next->next->next->value, "value5") == 0);
                    xassert(dlist->size == 4);
                }

                {
                    XRDList_PT node = xdlist_front_node(dlist)->next->next;
                    xdlist_remove_before(dlist, node, &value);
                    /*
                    *  value1
                    *  value4
                    *  value5
                    */
                    xassert(strcmp(value, "value3") == 0);
                    xassert(strcmp(dlist->head->next->next->value, "value5") == 0);
                    xassert_false(dlist->head->next->next->next);
                    xassert(dlist->size == 3);
                }

                xassert(strcmp(dlist->head->value, "value1") == 0);
                xassert(strcmp(dlist->tail->value, "value5") == 0);

                xdlist_free(&dlist);
            }
        }

        /* xdlist_remove_after */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdlist_remove_after(NULL, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* dlist is empty */
            {
                XDList_PT dlist = xdlist_new();
                xassert_false(xdlist_remove_after(dlist, dlist->head, NULL));
                xdlist_free(&dlist);
            }

            /* node is NULL */
            {
                XDList_PT dlist = xdlist_new();
                xassert_false(xdlist_remove_after(dlist, dlist->head, NULL));
                xdlist_free(&dlist);
            }

            /* node->next is NULl */
            {
                XDList_PT dlist = xdlist_new();
                xdlist_vload_repeat(dlist, "k", NULL);

                xassert_false(xdlist_remove_after(dlist, dlist->tail, NULL));
                xdlist_free(&dlist);
            }

            {
                XDList_PT dlist = xdlist_new();

                void *value = NULL;

                xdlist_push_back_repeat(dlist, "value1");
                xdlist_push_back_repeat(dlist, "value2");
                xdlist_push_back_repeat(dlist, "value3");
                xdlist_push_back_repeat(dlist, "value4");
                xdlist_push_back_repeat(dlist, "value5");

                {
                    XRDList_PT node = xdlist_front_node(dlist)->next->next;
                    xdlist_remove_after(dlist, node, &value);
                    /*
                    *  value1
                    *  value2
                    *  value3
                    *  value5
                    */
                    xassert(strcmp(value, "value4") == 0);
                    xassert(strcmp(dlist->head->next->next->next->value, "value5") == 0);
                    xassert(dlist->size == 4);
                }

                {
                    XRDList_PT node = xdlist_front_node(dlist)->next->next;
                    xdlist_remove_after(dlist, node, &value);
                    /*
                    *  value1, value1
                    *  value2, value2
                    *  value3, value3
                    */
                    xassert(strcmp(value, "value5") == 0);
                    xassert(strcmp(dlist->head->next->next->value, "value3") == 0);
                    xassert_false(dlist->head->next->next->next);
                    xassert(dlist->size == 3);
                }

                xassert(strcmp(dlist->head->value, "value1") == 0);
                xassert(strcmp(dlist->tail->value, "value3") == 0);

                xdlist_free(&dlist);
            }
        }

        /* xdlist_remove_if_impl */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdlist_remove_if_impl(NULL, false, false, NULL, NULL, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* dlist is empty */
            {
                XDList_PT dlist = xdlist_new();
                xassert(xdlist_remove_if_impl(dlist, false, false, NULL, NULL, xdlist_test_apply_false, NULL) == 0);
                xdlist_free(&dlist);
            }

            /* no any value match */
            {
                XDList_PT dlist = xdlist_new();
                xdlist_push_back_repeat(dlist, "value1");
                xdlist_push_back_repeat(dlist, "value2");
                xdlist_push_back_repeat(dlist, "value1");

                xassert(xdlist_remove_if_impl(dlist, false, false, xdlist_test_cmp_false, NULL, NULL, NULL) == 0);
                xassert(xdlist_remove_if_impl(dlist, false, false, NULL, NULL, xdlist_test_apply_false, NULL) == 0);

                xassert(xdlist_remove_if_impl(dlist, false, false, test_cmp, NULL, NULL, "value3") == 0);
                xassert(xdlist_remove_if_impl(dlist, false, false, NULL, NULL, xdlist_test_apply, "value3") == 0);

                xdlist_free(&dlist);
            }

            /* remove one */
            {
                XDList_PT dlist = xdlist_new();
                xdlist_push_back_repeat(dlist, "value1");
                xdlist_push_back_repeat(dlist, "value2");
                xdlist_push_back_repeat(dlist, "value3");
                xdlist_push_back_repeat(dlist, "value4");

                xassert(xdlist_remove_if_impl(dlist, false, true, test_cmp, NULL, NULL, "value1") == 1);
                xassert(xdlist_remove_if_impl(dlist, false, true, NULL, NULL, xdlist_test_apply, "value2") == 1);
                xassert(dlist->size == 2);
                xassert(strcmp(dlist->head->value, "value3") == 0);
                xassert(strcmp(dlist->tail->value, "value4") == 0);
                xassert_false(dlist->head->next->next);

                xdlist_free(&dlist);
            }

            /* remove all */
            {
                XDList_PT dlist = xdlist_new();
                xdlist_push_back_repeat(dlist, "value1");
                xdlist_push_back_repeat(dlist, "value2");
                xdlist_push_back_repeat(dlist, "value1");
                xdlist_push_back_repeat(dlist, "value1");
                xdlist_push_back_repeat(dlist, "value2");
                xdlist_push_back_repeat(dlist, "value1");

                xassert(xdlist_remove_if_impl(dlist, false, false, test_cmp, NULL, NULL, "value1") == 4);
                xassert(xdlist_find(dlist, "value1") == NULL);
                xassert(xdlist_remove_if_impl(dlist, false, false, NULL, NULL, xdlist_test_apply, "value2") == 2);
                xassert(xdlist_find(dlist, "value2") == NULL);
                xassert(dlist->size == 0);

                xdlist_free(&dlist);
            }

            /* deep remove */
            {
                XDList_PT dlist = xdlist_new();
                char *value1 = XMEM_CALLOC(1, 10);
                char *value2 = XMEM_CALLOC(1, 10);
                char *value3 = XMEM_CALLOC(1, 10);

                memcpy(value1, "value1", 6);
                memcpy(value2, "value2", 6);
                memcpy(value3, "value1", 6);

                xdlist_push_back_repeat(dlist, value1);
                xdlist_push_back_repeat(dlist, value2);
                xdlist_push_back_repeat(dlist, value3);

                xassert(xdlist_remove_if_impl(dlist, true, false, test_cmp, NULL, NULL, "value1") == 2);
                xassert(dlist->size == 1);
                xassert(strcmp(dlist->head->value, "value2") == 0);
                xassert(strcmp(dlist->tail->value, "value2") == 0);
                xassert_false(dlist->head->next);

                xdlist_deep_free(&dlist);
            }
        }

        /* all below remove functions tested by xdlist_remove_if_impl already */

        /* xdlist_remove */
        {
            /* value == NULL */
            {
                XDList_PT dlist = xdlist_new();
                bool except = false;
                XEXCEPT_TRY
                    xdlist_remove(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xdlist_free(&dlist);
            }
        }

        /* xdlist_remove_all */
        {
            /* value == NULL */
            {
                XDList_PT dlist = xdlist_new();
                bool except = false;
                XEXCEPT_TRY
                    xdlist_remove_all(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xdlist_free(&dlist);
            }
        }

        /* xdlist_deep_remove */
        {
            /* value == NULL */
            {
                XDList_PT dlist = xdlist_new();
                bool except = false;
                XEXCEPT_TRY
                    xdlist_deep_remove(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xdlist_free(&dlist);
            }

            /* *value == NULL */
            {
                XDList_PT dlist = xdlist_new();
                void *str = NULL;
                void **value = &str;

                bool except = false;
                XEXCEPT_TRY
                    xdlist_deep_remove(dlist, value);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xdlist_free(&dlist);
            }
        }

        /* xdlist_remove_apply_if */
        /* xdlist_remove_apply_value_if */
        /* xdlist_remove_equal_if */
        /* xdlist_remove_apply_break_if */
        /* xdlist_remove_apply_value_break_if */
        /* xdlist_remove_equal_break_if */
        /* xdlist_deep_remove_apply_if */
        /* xdlist_deep_remove_apply_value_if */
        /* xdlist_deep_remove_equal_if */
        /* xdlist_deep_remove_apply_break_if */
        /* xdlist_deep_remove_apply_value_break_if */
        /* xdlist_deep_remove_equal_break_if */

        /* below two free functions tested by other functions already */
        /* xdlist_free */
        /* xdlist_deep_free */


        /* xdlist_clear */
        /* xdlist_deep_clear */
        {
            /* tested by xslist_free already */
        }

        /* xdlist_swap */
        {
            /* dlist1 == NULL */
            {
                XDList_PT dlist2 = xdlist_new();
                bool except = false;
                XEXCEPT_TRY
                    xdlist_swap(NULL, dlist2);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xdlist_free(&dlist2);
            }

            /* dlist2 == NULL */
            {
                XDList_PT dlist1 = xdlist_new();
                bool except = false;
                XEXCEPT_TRY
                    xdlist_swap(dlist1, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xdlist_free(&dlist1);
            }

            /* normal */
            {
                XDList_PT dlist1 = xdlist_new();
                XDList_PT dlist2 = xdlist_new();

                xdlist_push_back_repeat(dlist1, "value1");
                xdlist_push_back_repeat(dlist1, "value2");
                xdlist_push_back_repeat(dlist1, "value3");
                xdlist_push_back_repeat(dlist1, "value4");

                xdlist_push_back_repeat(dlist2, "value5");
                xdlist_push_back_repeat(dlist2, "value6");
                xdlist_push_back_repeat(dlist2, "value7");
                xdlist_push_back_repeat(dlist2, "value8");
                xdlist_push_back_repeat(dlist2, "value9");

                xassert(xdlist_swap(dlist1, dlist2));

                xassert(strcmp(dlist1->head->value, "value5") == 0);
                xassert(strcmp(dlist1->tail->value, "value9") == 0);

                xassert(strcmp(dlist2->head->value, "value1") == 0);
                xassert(strcmp(dlist2->tail->value, "value4") == 0);

                xassert(dlist1->size == 5);
                xassert(dlist2->size == 4);

                xdlist_free(&dlist1);
                xdlist_free(&dlist2);
            }
        }

        /* xdlist_merge */
        {
            {
                XDList_PT dlist1 = xdlist_new();
                XDList_PT dlist2 = xdlist_new();

                xdlist_push_back_repeat(dlist1, "value1");
                xdlist_push_back_repeat(dlist1, "value2");
                xdlist_push_back_repeat(dlist2, "value1");
                xdlist_push_back_repeat(dlist2, "value4");

                xassert(xdlist_merge(dlist1, &dlist2));

                xassert_false(dlist2);

                xassert(strcmp(dlist1->head->value, "value1") == 0);
                xassert(strcmp(dlist1->tail->value, "value4") == 0);

                xassert(strcmp(dlist1->head->next->next->value, "value1") == 0);

                xassert(dlist1->size == 4);

                xdlist_free(&dlist1);
            }
        }

        /* xdlist_reverse */
        {
            XDList_PT dlist = xdlist_new();

            xdlist_reverse(dlist);

            xdlist_push_back_repeat(dlist, "value1");
            xdlist_push_back_repeat(dlist, "value2");
            xdlist_push_back_repeat(dlist, "value3");
            xdlist_push_back_repeat(dlist, "value4");

            xdlist_reverse(dlist);

            xassert(strcmp(dlist->head->value, "value4") == 0);
            xassert(strcmp(dlist->head->next->value, "value3") == 0);
            xassert(strcmp(dlist->head->next->next->value, "value2") == 0);
            xassert(strcmp(dlist->tail->value, "value1") == 0);

            xassert(dlist->size == 4);

            xdlist_free(&dlist);
        }

        /* xdlist_map */
        /* xdlist_map_break_if_true */
        /* xdlist_map_break_if_false */
        {
            XDList_PT dlist = xdlist_new();

            xdlist_push_back_repeat(dlist, "value1");
            xdlist_push_back_repeat(dlist, "value2");
            xdlist_push_back_repeat(dlist, "value3");
            xdlist_push_back_repeat(dlist, "value4");

            /* xdlist_map */
            int count = 0;
            xassert(xdlist_map(dlist, xdlist_test_map_apply_count, (void*)&count) == 4);
            xassert(count == 4);

            /* xdlist_map_break_if_true */
            xassert(xdlist_map_break_if_true(dlist, xdlist_test_apply, "value3"));
            xassert_false(xdlist_map_break_if_true(dlist, xdlist_test_apply, "value5"));

            /* xdlist_map_break_if_false */
            xassert(xdlist_map_break_if_false(dlist, xdlist_test_apply, "value3"));
            xassert_false(xdlist_map_break_if_false(dlist, xdlist_test_apply_true, "value3"));

            xdlist_free(&dlist);
        }

        /* xdlist_find */
        /* xdlist_find_equal_if */
        /* xdlist_find_apply_if */
        {
            XDList_PT dlist = xdlist_new();

            xdlist_push_back_repeat(dlist, "value1");
            xdlist_push_back_repeat(dlist, "value2");
            xdlist_push_back_repeat(dlist, "value3");
            xdlist_push_back_repeat(dlist, "value4");

            /* xdlist_find */
            XRDList_PT node = xdlist_find(dlist, "value2");
            xassert(strcmp(node->value, "value2") == 0);

            /* xdlist_find_equal_if */
            node = xdlist_find_equal_if(dlist, "value3", test_cmp, NULL);
            xassert(strcmp(node->value, "value3") == 0);

            /* xdlist_find_apply_if */
            node = xdlist_find_apply_if(dlist, xdlist_test_apply, "value4");
            xassert(strcmp(node->value, "value4") == 0);

            xdlist_free(&dlist);
        }

        /* xdlist_to_array */
        {
            XDList_PT dlist = xdlist_new();

            xdlist_push_back_repeat(dlist, "value1");
            xdlist_push_back_repeat(dlist, "value2");
            xdlist_push_back_repeat(dlist, "value3");
            xdlist_push_back_repeat(dlist, "value4");

            XPArray_PT array = xdlist_to_array(dlist);

            xassert(array->size == 4);
            xassert(strcmp((char*)xparray_get(array, 0), "value1") == 0);
            xassert(strcmp((char*)xparray_get(array, 1), "value2") == 0);
            xassert(strcmp((char*)xparray_get(array, 2), "value3") == 0);
            xassert(strcmp((char*)xparray_get(array, 3), "value4") == 0);

            xparray_free(&array);
            xdlist_free(&dlist);
        }

        /* xdlist_size */
        /* xdlist_is_empty */

        /* xdlist_unique */
        {
            XDList_PT dlist = xdlist_new();

            xdlist_push_back_repeat(dlist, "value2");
            xdlist_push_back_repeat(dlist, "value1");
            xdlist_push_back_repeat(dlist, "value1");
            xdlist_push_back_repeat(dlist, "value3");
            xdlist_push_back_repeat(dlist, "value2");
            xdlist_push_back_repeat(dlist, "value4");

            xassert(dlist->size == 6);
            xdlist_unique(dlist, test_cmp, NULL);
            xassert(dlist->size == 4);

            xassert(strcmp(dlist->head->value, "value1") == 0);
            xassert(strcmp(dlist->head->next->value, "value2") == 0);
            xassert(strcmp(dlist->head->next->next->value, "value3") == 0);
            xassert(strcmp(dlist->head->next->next->next->value, "value4") == 0);
            xassert_false(dlist->head->next->next->next->next);
            xassert(strcmp(dlist->tail->value, "value4") == 0);

            xdlist_free(&dlist);
        }

        /* xdlist_deep_unique */
        {
            XDList_PT dlist = xdlist_random_string(1000);
            xdlist_deep_unique(dlist, test_cmp, NULL);
            //xdlist_map(dlist, xdlist_test_print, NULL);
            xdlist_deep_free(&dlist);
        }

        /* xdlist_sort */
        {
            {
                XDList_PT dlist = xdlist_new();

                xdlist_push_back_repeat(dlist, "value2");
                xdlist_push_back_repeat(dlist, "value1");
                xdlist_push_back_repeat(dlist, "value1");
                xdlist_push_back_repeat(dlist, "value3");
                xdlist_push_back_repeat(dlist, "value5");
                xdlist_push_back_repeat(dlist, "value4");

                xdlist_sort(dlist, test_cmp, NULL);

                xassert(strcmp(dlist->head->value, "value1") == 0);
                xassert(strcmp(dlist->head->next->value, "value1") == 0);
                xassert(strcmp(dlist->head->next->next->value, "value2") == 0);
                xassert(strcmp(dlist->head->next->next->next->value, "value3") == 0);
                xassert(strcmp(dlist->head->next->next->next->next->value, "value4") == 0);
                xassert(strcmp(dlist->head->next->next->next->next->next->value, "value5") == 0);
                xassert_false(dlist->head->next->next->next->next->next->next);
                xassert(strcmp(dlist->head->next->next->next->next->next->prev->value, "value4") == 0);
                xassert(strcmp(dlist->head->next->next->next->next->prev->value, "value3") == 0);
                xassert(strcmp(dlist->head->next->next->next->prev->value, "value2") == 0);
                xassert(strcmp(dlist->head->next->next->prev->value, "value1") == 0);
                xassert(strcmp(dlist->head->next->prev->value, "value1") == 0);
                xassert_false(dlist->head->prev);

                xdlist_free(&dlist);
            }

            {
                XDList_PT dlist = xdlist_random_string(1000);
                xdlist_sort(dlist, test_cmp, NULL);
                //xdlist_map(dlist, xdlist_test_print, NULL);
                xdlist_deep_free(&dlist);
            }
        }

        /* xdlist_is_sorted */
        {
            /* tested by xdlist_sort already */
        }

        {
            int count = 0;
            xmem_leak(xdlist_test_check_mem_leak, &count);
            xassert(count == 0);
        }
}


