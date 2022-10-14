
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../array_pointer/xarray_pointer_x.h"
#include "../list_d_raw/xlist_d_raw_x.h"
#include "../list_d/xlist_d_x.h"
#include "../list_dc/xlist_dc_x.h"
#include "../include/xalgos.h"

static
int xdclist_test_cmp_true(void *value1, void *value2) {
    return 0;
}

static
int xdclist_test_cmp_false(void *value1, void *value2, void *cl) {
    return 1;
}

static 
int xdclist_test_cmp(void *value1, void *value2, void *cl) {
    return value1 == value2 ? 0 : 1;
}

static
int test_cmp(void *value1, void *value2, void *cl) {
    return strcmp((char*)value1, (char*)value2);
}

static
bool xdclist_test_apply(void *value, void *cl) {
    return strcmp((char*)value, (char*)cl) == 0;
}

static
bool xdclist_test_apply_true(void *value, void *cl) {
    return true;
}

static
bool xdclist_test_apply_false(void *value, void *cl) {
    return false;
}

static
bool xdclist_test_map_apply_count(void *value, void *cl) {
    ++(*((int*)(cl)));
    return true;
}

static
bool xdclist_test_print(void *value, void *cl) {
    printf("%s\n", (char*)value);
    return true;
}

static 
XDList_PT xdclist_random_string(int size) {
    XDList_PT dlist = xdclist_new();
    if (!dlist) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xdclist_deep_free(&dlist);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }
            str[str_size - 1] = '\0';

            xdclist_push_back_repeat(dlist, str);
        }
    }

    return dlist;
}

static
void xdclist_test_check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}


void test_xdclist() {

        /* xdclist_new */
        {
            XDList_PT dlist = xdclist_new();
            xassert(dlist);
            xassert(dlist->head == NULL);
            xassert(dlist->tail == NULL);
            xassert(dlist->size == 0);
            xdclist_free(&dlist);
        }

        /* xdclist_vload_repeat */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdclist_vload_repeat(NULL, "value", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XDList_PT dlist = xdclist_new();
                xassert(xdclist_vload_repeat(dlist, NULL) == 0);
                xdclist_free(&dlist);
            }

            /* normal */
            {
                XDList_PT dlist = xdclist_new();
                xassert(xdclist_vload_repeat(dlist, "k1", "k2", "k1", "k3", NULL) == 4);
                xassert(strcmp(dlist->head->value, "k1") == 0);
                xdclist_free(&dlist);
            }
        }

        /* xdclist_vload_unique */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdclist_vload_unique(NULL, "value", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XDList_PT dlist = xdclist_new();
                xassert(xdclist_vload_unique(dlist, NULL) == 0);
                xdclist_free(&dlist);
            }

            /* repeat value */
            {
                XDList_PT dlist = xdclist_new();
                xassert(xdclist_vload_unique(dlist, "k1", "k2", "k1", "k3", NULL) == 3);
                xassert(strcmp(dlist->head->value, "k1") == 0);
                xdclist_free(&dlist);
            }
        }

        /* xdclist_vload_unique_if */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdclist_vload_unique_if(NULL, xdclist_test_cmp, NULL, "k", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }
            /* value == NULL */
            {
                XDList_PT dlist = xdclist_new();
                xassert(xdclist_vload_unique_if(dlist, xdclist_test_cmp, NULL, NULL) == 0);
                xdclist_free(&dlist);
            }

            /* repeat value */
            {
                XDList_PT dlist = xdclist_new();
                xassert(xdclist_vload_unique_if(dlist, xdclist_test_cmp, NULL, "k1", "k2", "k1", "k3", NULL) == 3);
                xassert(strcmp(dlist->head->value, "k1") == 0);
                xdclist_free(&dlist);
            }
        }

        /* xdclist_vload_replace_if */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdclist_vload_replace_if(NULL, xdclist_test_cmp, NULL, "k", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }
            /* value == NULL */
            {
                XDList_PT dlist = xdclist_new();
                xassert(xdclist_vload_replace_if(dlist, xdclist_test_cmp, NULL, NULL) == 0);
                xdclist_free(&dlist);
            }

            /* repeat value */
            {
                XDList_PT dlist = xdclist_new();
                xassert(xdclist_vload_replace_if(dlist, xdclist_test_cmp, NULL, "k1", "k2", "k1", "k3", NULL) == 3);
                xassert(strcmp(dlist->head->value, "k1") == 0);
                xdclist_free(&dlist);
            }
        }

        /* xdclist_aload_repeat */
        {
            /* dlist == NULL */
            {
                XPArray_PT array = xparray_new(10);
                xparray_put(array, 0, "k", NULL);
                bool except = false;
                XEXCEPT_TRY
                    xdclist_aload_repeat(NULL, array);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xparray_free(&array);
            }

            /* xs == NULL */
            {
                XDList_PT dlist = xdclist_new();
                bool except = false;
                XEXCEPT_TRY
                    xdclist_aload_repeat(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xdclist_free(&dlist);
            }

            /* value == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XDList_PT dlist = xdclist_new();

                xassert(xdclist_aload_repeat(dlist, array) == 0);

                xdclist_free(&dlist);
                xparray_free(&array);
            }

            /* normal */
            {
                XPArray_PT array = xparray_new(10);
                XDList_PT dlist = xdclist_new();

                xparray_vload(array, "k1", "k2", "k1", "k3", NULL);
                xassert(xdclist_aload_repeat(dlist, array) == 4);
                xassert(strcmp(dlist->head->value, "k1") == 0);

                xdclist_free(&dlist);
                xparray_free(&array);
            }
        }

        /* xdclist_aload_unique */
        {
            /* dlist == NULL */
            {
                XPArray_PT array = xparray_new(10);
                xparray_put(array, 0, "k", NULL);

                bool except = false;
                XEXCEPT_TRY
                    xdclist_aload_unique(NULL, array);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xparray_free(&array);
            }

            /* xs == NULL */
            {
                XDList_PT dlist = xdclist_new();
                bool except = false;
                XEXCEPT_TRY
                    xdclist_aload_unique(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xdclist_free(&dlist);
            }

            /* value == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XDList_PT dlist = xdclist_new();

                xassert(xdclist_aload_unique(dlist, array) == 0);

                xdclist_free(&dlist);
                xparray_free(&array);
            }

            /* repeat value */
            {
                XPArray_PT array = xparray_new(10);
                XDList_PT dlist = xdclist_new();

                xparray_vload(array, "k1", "k2", "k1", "k3", NULL);
                xassert(xdclist_aload_unique(dlist, array) == 3);
                xassert(strcmp(dlist->head->value, "k1") == 0);

                xdclist_free(&dlist);
                xparray_free(&array);
            }
        }

        /* xdclist_aload_unique_if */
        {
            /* cmp == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XDList_PT dlist = xdclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdclist_aload_unique_if(dlist, array, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdclist_free(&dlist);
                xparray_free(&array);
            }

            /* below 3 scenarios tested already in xdclist_aload_unique */
            /* xs == NULL */
            /* value == NULL */
            /* value == NULL */

            /* repeat value */
            {
                XPArray_PT array = xparray_new(10);
                XDList_PT dlist = xdclist_new();

                xparray_vload(array, "k1", "k2", "k1", "k3", NULL);
                xassert(xdclist_aload_unique_if(dlist, array, xdclist_test_cmp, NULL) == 3);
                xassert(strcmp(dlist->head->value, "k1") == 0);

                xdclist_free(&dlist);
                xparray_free(&array);
            }
        }

        /* xdclist_aload_replace_if */
        {
            /* cmp == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XDList_PT dlist = xdclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdclist_aload_replace_if(dlist, array, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);

                xdclist_free(&dlist);
                xparray_free(&array);
            }

            /* below 3 scenarios tested already in xdclist_aload_replace */
            /* xs == NULL */
            /* value == NULL */
            /* value == NULL */

            /* repeat value */
            {
                XPArray_PT array = xparray_new(10);
                XDList_PT dlist = xdclist_new();

                xparray_vload(array, "k1", "k2", "k1", "k3", NULL);
                xassert(xdclist_aload_replace_if(dlist, array, xdclist_test_cmp, NULL) == 3);
                xassert(strcmp(dlist->head->value, "k1") == 0);

                xdclist_free(&dlist);
                xparray_free(&array);
            }
        }

        /* xdclist_copyn_if_impl */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdclist_copyn_if_impl(NULL, 0, 1, false, xdclist_test_cmp, NULL, NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* count < 0 */
            {
                XDList_PT dlist = xdclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdclist_copyn_if_impl(dlist, 0, -1, false, xdclist_test_cmp, NULL, xdclist_test_apply_true, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdclist_free(&dlist);
            }

            /* deep == true && value_size < 0 */
            {
                XDList_PT dlist = xdclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdclist_copyn_if_impl(dlist, -1, 1, true, xdclist_test_cmp, NULL, xdclist_test_apply_true, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdclist_free(&dlist);
            }

            /* deep == true && value_size == 0 */
            {
                XDList_PT dlist = xdclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdclist_copyn_if_impl(dlist, 0, 1, true, xdclist_test_cmp, NULL, xdclist_test_apply_true, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdclist_free(&dlist);
            }

            /* dlist is empty */
            {
                XDList_PT dlist = xdclist_new();

                {
                    XDList_PT ndlist = xdclist_copyn_if_impl(dlist, 0, 1, false, NULL, NULL, xdclist_test_apply_true, NULL);
                    xassert(ndlist->size == 0);
                    xdclist_free(&ndlist);
                }

                xdclist_free(&dlist);
            }

            /* count == 0 */
            {
                XDList_PT dlist = xdclist_new();
                xdclist_vload_repeat(dlist, "k1", "k2", NULL);

                {
                    XDList_PT ndlist = xdclist_copyn_if_impl(dlist, 0, 0, false, NULL, NULL, xdclist_test_apply_true, NULL);
                    xassert(ndlist->size == 0);
                    xdclist_free(&ndlist);
                }

                xdclist_free(&dlist);
            }

            /* dlist->size < count */
            {
                XDList_PT dlist = xdclist_new();
                xdclist_vload_repeat(dlist, "k1", "k1", NULL);

                {
                    XDList_PT ndlist = xdclist_copyn_if_impl(dlist, 0, 3, false, NULL, NULL, xdclist_test_apply_true, NULL);
                    xassert(ndlist->size == 2);
                    xdclist_free(&ndlist);
                }

                xdclist_free(&dlist);
            }

            /* count < dlist->size */
            {
                XDList_PT dlist = xdclist_new();
                xdclist_vload_repeat(dlist, "k1", "k1", NULL);

                {
                    XDList_PT ndlist = xdclist_copyn_if_impl(dlist, 0, 1, false, NULL, NULL, xdclist_test_apply_true, NULL);
                    xassert(ndlist->size == 1);
                    xassert(strcmp(ndlist->head->value, "k1") == 0);
                    xdclist_free(&ndlist);
                }

                xdclist_free(&dlist);
            }

            /* no any value match */
            {
                XDList_PT dlist = xdclist_new();
                xdclist_vload_repeat(dlist, "k1", "k2", NULL);

                {
                    XDList_PT ndlist = xdclist_copyn_if_impl(dlist, 0, 1, false, NULL, NULL, xdclist_test_apply_false, NULL);
                    xassert(ndlist->size == 0);
                    xdclist_free(&ndlist);
                }

                xdclist_free(&dlist);
            }

            /* normal */
            {
                XDList_PT dlist = xdclist_new();
                xdclist_vload_repeat(dlist, "k1", "k1", "k3", NULL);

                {
                    XDList_PT ndlist = xdclist_copyn_if_impl(dlist, 0, 2, false, NULL, NULL, xdclist_test_apply_true, NULL);
                    xassert(ndlist->size == 2);
                    xassert(strcmp(ndlist->head->value, "k1") == 0);
                    xassert(strcmp(ndlist->head->next->value, "k1") == 0);
                    xdclist_free(&ndlist);
                }

                {
                    XDList_PT ndlist = xdclist_copyn_if_impl(dlist, 0, 2, false, NULL, NULL, xdclist_test_apply, "k3");
                    xassert(ndlist->size == 1);
                    xassert(strcmp(ndlist->head->value, "k3") == 0);
                    xdclist_free(&ndlist);
                }

                {
                    XDList_PT ndlist = xdclist_copyn_if_impl(dlist, 0, 2, false, test_cmp, NULL, NULL, "k3");
                    xassert(ndlist->size == 1);
                    xassert(strcmp(ndlist->head->value, "k3") == 0);
                    xdclist_free(&ndlist);
                }

                xdclist_free(&dlist);
            }
        }

        /* xdclist_copyn_equal_if */
        {
            /* value == NULL */
            {
                XDList_PT dlist = xdclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdclist_copyn_equal_if(dlist, 1, NULL, xdclist_test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdclist_free(&dlist);
            }
        }

        /* xdclist_deep_copyn_equal_if */
        {
            /* value == NULL */
            {
                XDList_PT dlist = xdclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdclist_deep_copyn_equal_if(dlist, 1, 1, NULL, xdclist_test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);

                xdclist_free(&dlist);
            }
        }
                
        /* xdclist_copy */
        /* xdclist_copy_equal_if */
        /* xdclist_copy_apply_if */
        /* xdclist_copy_apply_value_if */
        /* xdclist_copyn */
        /* xdclist_copyn_equal_if */
        /* xdclist_copyn_apply_if */
        /* xdclist_copyn_apply_value_if */
        /* xdclist_deep_copy */
        /* xdclist_deep_copy_equal_if */
        /* xdclist_deep_copy_apply_if */
        /* xdclist_deep_copy_apply_value_if */
        /* xdclist_deep_copyn */
        /* xdclist_deep_copyn_equal_if */
        /* xdclist_deep_copyn_apply_if */
        /* xdclist_deep_copyn_apply_value_if */
        {
            /* tested by xdclist_copyn_if_impl already */
        }

        /* xdclist_push_front_repeat */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdclist_push_front_repeat(NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XDList_PT dlist = xdclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdclist_push_front_repeat(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdclist_free(&dlist);
            }

            /* normal */
            {
                XDList_PT dlist = xdclist_new();

                char *str = "value1";

                xdclist_push_front_repeat(dlist, str);
                xdclist_push_front_repeat(dlist, "value2");
                xdclist_push_front_repeat(dlist, str);
                xdclist_push_front_repeat(dlist, "value3");

                xassert(strcmp(dlist->head->value, "value3") == 0);
                xassert(strcmp(dlist->head->next->value, "value1") == 0);
                xassert(strcmp(dlist->head->next->next->value, "value2") == 0);
                xassert(strcmp(dlist->head->next->next->next->value, "value1") == 0);
                xassert(dlist->head->next->next->next->next==dlist->head);
                xassert(strcmp(dlist->tail->value, "value1") == 0);

                xassert(dlist->size == 4);

                xdclist_free(&dlist);
            }
        }

        /* xdclist_pop_front */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdclist_pop_front(NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* dlist is empty */
            {
                XDList_PT dlist = xdclist_new();
                xassert(xdclist_pop_front(dlist) == false);
                xdclist_free(&dlist);
            }

            {
                XDList_PT dlist = xdclist_new();

                xdclist_push_back_repeat(dlist, "value1");
                xdclist_push_back_repeat(dlist, "value2");
                xdclist_push_back_repeat(dlist, "value3");

                {
                    void *value = NULL;

                    xassert(value = xdclist_pop_front(dlist));
                    xassert(strcmp(value, "value1") == 0);

                    xassert(value = xdclist_pop_front(dlist));
                    xassert(strcmp(value, "value2") == 0);

                    xassert(xdclist_pop_front(dlist));

                    xassert(dlist->size == 0);
                    xassert(dlist->head == NULL);
                    xassert(dlist->tail == NULL);
                }

                xdclist_free(&dlist);
            }
        }

        /* xdclist_push_back_repeat */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdclist_push_back_repeat(NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XDList_PT dlist = xdclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdclist_push_back_repeat(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdclist_free(&dlist);
            }

            /* normal */
            {
                XDList_PT dlist = xdclist_new();

                char *str = "value1";

                xdclist_push_back_repeat(dlist, str);
                xdclist_push_back_repeat(dlist, "value2");
                xdclist_push_back_repeat(dlist, str);
                xdclist_push_back_repeat(dlist, "value3");

                xassert(strcmp(dlist->head->value, "value1") == 0);
                xassert(strcmp(dlist->head->next->value, "value2") == 0);
                xassert(strcmp(dlist->head->next->next->value, "value1") == 0);
                xassert(strcmp(dlist->head->next->next->next->value, "value3") == 0);
                xassert(dlist->head->next->next->next->next == dlist->head);
                xassert(strcmp(dlist->tail->value, "value3") == 0);

                xassert(dlist->size == 4);

                xdclist_free(&dlist);
            }
        }

        /* xdclist_pop_back */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdclist_pop_back(NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* dlist is empty */
            {
                XDList_PT dlist = xdclist_new();
                xassert(xdclist_pop_back(dlist) == false);
                xdclist_free(&dlist);
            }

            {
                XDList_PT dlist = xdclist_new();

                xdclist_push_back_repeat(dlist, "value1");
                xdclist_push_back_repeat(dlist, "value2");
                xdclist_push_back_repeat(dlist, "value3");

                {
                    void *value = NULL;

                    xassert(value = xdclist_pop_back(dlist));
                    xassert(strcmp(value, "value3") == 0);

                    xassert(value = xdclist_pop_back(dlist));
                    xassert(strcmp(value, "value2") == 0);

                    xassert(xdclist_pop_back(dlist));

                    xassert(dlist->size == 0);
                    xassert(dlist->head == NULL);
                    xassert(dlist->tail == NULL);
                }

                xdclist_free(&dlist);
            }
        }

        /* xdclist_front */
        {
            /* dlist is empty */
            {
                XDList_PT dlist = xdclist_new();
                xassert_false(xdclist_front(dlist));
                xdclist_free(&dlist);
            }

            /* normal */
            {
                XDList_PT dlist = xdclist_new();

                xdclist_push_back_repeat(dlist, "value1");
                xdclist_push_back_repeat(dlist, "value2");
                xdclist_push_back_repeat(dlist, "value3");

                xassert(strcmp(xdclist_front(dlist), "value1") == 0);

                xdclist_free(&dlist);
            }
        }

        /* xdclist_back */
        {
            /* dlist is empty */
            {
                XDList_PT dlist = xdclist_new();
                xassert_false(xdclist_back(dlist));
                xdclist_free(&dlist);
            }

            /* normal */
            {
                XDList_PT dlist = xdclist_new();

                xdclist_push_back_repeat(dlist, "value1");
                xdclist_push_back_repeat(dlist, "value2");
                xdclist_push_back_repeat(dlist, "value3");

                xassert(strcmp(xdclist_back(dlist), "value3") == 0);

                xdclist_free(&dlist);
            }
        }

        /* xdclist_front_node */
        {
            /* dlist is empty */
            {
                XDList_PT dlist = xdclist_new();
                xassert_false(xdclist_front_node(dlist));
                xdclist_free(&dlist);
            }

            /* normal */
            {
                XDList_PT dlist = xdclist_new();

                xdclist_push_back_repeat(dlist, "value1");
                xdclist_push_back_repeat(dlist, "value2");
                xdclist_push_back_repeat(dlist, "value3");

                xassert(strcmp(xdclist_front_node(dlist)->value, "value1") == 0);

                xdclist_free(&dlist);
            }
        }

        /* xdclist_back_node */
        {
            /* dlist is empty */
            {
                XDList_PT dlist = xdclist_new();
                xassert_false(xdclist_back_node(dlist));
                xdclist_free(&dlist);
            }

            /* normal */
            {
                XDList_PT dlist = xdclist_new();

                xdclist_push_back_repeat(dlist, "value1");
                xdclist_push_back_repeat(dlist, "value2");
                xdclist_push_back_repeat(dlist, "value3");

                xassert(strcmp(xdclist_back_node(dlist)->value, "value3") == 0);

                xdclist_free(&dlist);
            }
        }

        /* xdclist_push_front_unique */
        {
            XDList_PT dlist = xdclist_new();

            char *str = "value1"; 

            xdclist_push_front_unique(dlist, str);
            xdclist_push_front_unique(dlist, "value2");
            xdclist_push_front_unique(dlist, str);
            xdclist_push_front_unique(dlist, "value3");

            xassert(strcmp(dlist->head->value, "value3") == 0);
            xassert(strcmp(dlist->head->next->value, "value2") == 0);
            xassert(strcmp(dlist->head->next->next->value, "value1") == 0);
            xassert(dlist->head->next->next->next == dlist->head);
            xassert(strcmp(dlist->tail->value, "value1") == 0);
            xassert(dlist->size == 3);

            xdclist_free(&dlist);
        }

        /* xdclist_push_front_unique_if */
        {
            XDList_PT dlist = xdclist_new();

            xdclist_push_front_unique_if(dlist, "value1", test_cmp, NULL);
            xdclist_push_front_unique_if(dlist, "value2", test_cmp, NULL);
            xdclist_push_front_unique_if(dlist, "value1", test_cmp, NULL);
            xdclist_push_front_unique_if(dlist, "value3", test_cmp, NULL);

            xassert(strcmp(dlist->head->value, "value3") == 0);
            xassert(strcmp(dlist->head->next->value, "value2") == 0);
            xassert(strcmp(dlist->head->next->next->value, "value1") == 0);
            xassert(strcmp(dlist->tail->value, "value1") == 0);
            xassert(dlist->head->next->next->next == dlist->head);
            xassert(dlist->size == 3);

            xdclist_free(&dlist);
        }

        /* xdclist_push_front_replace_if */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdclist_push_front_replace_if(NULL, "k", test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
            }

            /* value == NULL */
            {
                XDList_PT dlist = xdclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdclist_push_front_replace_if(dlist, NULL, test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdclist_free(&dlist);
            }
            
            /* repeat value */
            {
                XDList_PT dlist = xdclist_new();

                xdclist_push_front_replace_if(dlist, "value1", test_cmp, NULL);
                xdclist_push_front_replace_if(dlist, "value2", test_cmp, NULL);
                xdclist_push_front_replace_if(dlist, "value1", test_cmp, NULL);
                xdclist_push_front_replace_if(dlist, "value3", test_cmp, NULL);

                xassert(strcmp(dlist->head->value, "value3") == 0);
                xassert(strcmp(dlist->head->next->value, "value2") == 0);
                xassert(strcmp(dlist->head->next->next->value, "value1") == 0);
                xassert(strcmp(dlist->tail->value, "value1") == 0);
                xassert(dlist->head->next->next->next == dlist->head);
                xassert(dlist->size == 3);

                xdclist_free(&dlist);
            }
        }

        /* xdclist_push_front_deep_replace_if */
        {
            /* below scenarios tested by xdclist_push_front_replace_if already */
            /* dlist == NULL */
            /* value == NULL */
            /* cmp == NULL */

            /* repeat value */
            {
                XDList_PT dlist = xdclist_new();

                char *value = XMEM_CALLOC(1, 7);
                memcpy(value, "value1", 6);

                xdclist_push_front_deep_replace_if(dlist, value, test_cmp, NULL);
                xdclist_push_front_deep_replace_if(dlist, "value2", test_cmp, NULL);
                xdclist_push_front_deep_replace_if(dlist, "value1", test_cmp, NULL);
                xdclist_push_front_deep_replace_if(dlist, "value3", test_cmp, NULL);

                xassert(strcmp(dlist->head->value, "value3") == 0);
                xassert(strcmp(dlist->head->next->value, "value2") == 0);
                xassert(strcmp(dlist->head->next->next->value, "value1") == 0);
                xassert(strcmp(dlist->tail->value, "value1") == 0);
                xassert(dlist->head->next->next->next == dlist->head);
                xassert(dlist->size == 3);

                xdclist_free(&dlist);
            }
        }

        /* xdclist_push_back_unique */
        {
            XDList_PT dlist = xdclist_new();

            char *str = "value1";

            xdclist_push_back_unique(dlist, str);
            xdclist_push_back_unique(dlist, "value2");
            xdclist_push_back_unique(dlist, str);
            xdclist_push_back_unique(dlist, "value3");

            xassert(strcmp(dlist->head->value, "value1") == 0);
            xassert(strcmp(dlist->head->next->value, "value2") == 0);
            xassert(strcmp(dlist->head->next->next->value, "value3") == 0);
            xassert(strcmp(dlist->tail->value, "value3") == 0);
            xassert(dlist->head->next->next->next == dlist->head);
            xassert(dlist->size == 3);

            xdclist_free(&dlist);
        }

        /* xdclist_push_back_unique_if */
        {
            XDList_PT dlist = xdclist_new();

            xdclist_push_back_unique_if(dlist, "value1", test_cmp, NULL);
            xdclist_push_back_unique_if(dlist, "value2", test_cmp, NULL);
            xdclist_push_back_unique_if(dlist, "value1", test_cmp, NULL);
            xdclist_push_back_unique_if(dlist, "value3", test_cmp, NULL);

            xassert(strcmp(dlist->head->value, "value1") == 0);
            xassert(strcmp(dlist->head->next->value, "value2") == 0);
            xassert(strcmp(dlist->head->next->next->value, "value3") == 0);
            xassert(strcmp(dlist->tail->value, "value3") == 0);
            xassert(dlist->head->next->next->next == dlist->head);
            xassert(dlist->size == 3);

            xdclist_free(&dlist);
        }

        /* xdclist_push_back_replace_if */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdclist_push_back_replace_if(NULL, "k", test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XDList_PT dlist = xdclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdclist_push_back_replace_if(dlist, NULL, test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdclist_free(&dlist);
            }

            /* repeat value */
            {
                XDList_PT dlist = xdclist_new();

                xdclist_push_back_replace_if(dlist, "value1", test_cmp, NULL);
                xdclist_push_back_replace_if(dlist, "value2", test_cmp, NULL);
                xdclist_push_back_replace_if(dlist, "value1", test_cmp, NULL);
                xdclist_push_back_replace_if(dlist, "value3", test_cmp, NULL);

                xassert(strcmp(dlist->head->value, "value1") == 0);
                xassert(strcmp(dlist->head->next->value, "value2") == 0);
                xassert(strcmp(dlist->head->next->next->value, "value3") == 0);
                xassert(strcmp(dlist->tail->value, "value3") == 0);
                xassert(dlist->head->next->next->next == dlist->head);
                xassert(dlist->size == 3);

                xdclist_free(&dlist);
            }
        }

        /* xdclist_push_back_deep_replace_if */
        {
            /* below scenarios tested by xdclist_push_back_replace_if already */
            /* dlist == NULL */
            /* value == NULL */
            /* cmp == NULL */

            /* repeat value */
            {
                XDList_PT dlist = xdclist_new();

                char *value = XMEM_CALLOC(1, 7);
                memcpy(value, "value1", 6);

                xdclist_push_back_deep_replace_if(dlist, value, test_cmp, NULL);
                xdclist_push_back_deep_replace_if(dlist, "value2", test_cmp, NULL);
                xdclist_push_back_deep_replace_if(dlist, "value1", test_cmp, NULL);
                xdclist_push_back_deep_replace_if(dlist, "value3", test_cmp, NULL);

                xassert(strcmp(dlist->head->value, "value1") == 0);
                xassert(strcmp(dlist->head->next->value, "value2") == 0);
                xassert(strcmp(dlist->head->next->next->value, "value3") == 0);
                xassert(strcmp(dlist->tail->value, "value3") == 0);
                xassert(dlist->head->next->next->next == dlist->head);
                xassert(dlist->size == 3);

                xdclist_free(&dlist);
            }
        }

        /* xdclist_insert_before */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdclist_insert_before(NULL, NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XDList_PT dlist = xdclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdclist_insert_before(dlist, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdclist_free(&dlist);
            }

            {
                XDList_PT dlist = xdclist_new();

                xdclist_push_back_repeat(dlist, "value1");
                xdclist_push_back_repeat(dlist, "value2");
                xdclist_push_back_repeat(dlist, "value3");

                {
                    xdclist_insert_before(dlist, NULL, "value2");
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
                    XRDList_PT node = xdclist_front_node(dlist)->next->next;
                    xdclist_insert_before(dlist, node, "value4");
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

                xdclist_free(&dlist);
            }
        }

        /* xdclist_insert_after */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdclist_insert_after(NULL, NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XDList_PT dlist = xdclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xdclist_insert_after(dlist, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xdclist_free(&dlist);
            }

            {
                XDList_PT dlist = xdclist_new();

                xdclist_push_back_repeat(dlist, "value1");
                xdclist_push_back_repeat(dlist, "value2");
                xdclist_push_back_repeat(dlist, "value3");

                {
                    xdclist_insert_after(dlist, NULL, "value2");
                    /*
                     *  value1
                     *  value2
                     *  value3
                     *  value2
                     */
                    xassert(strcmp(dlist->head->next->next->next->value, "value2") == 0);
                    xassert(dlist->head->next->next->next->next == dlist->head);
                    xassert(strcmp(dlist->tail->value, "value2") == 0);
                }

                {
                    XRDList_PT node = xdclist_front_node(dlist)->next->next;
                    xdclist_insert_after(dlist, node, "value4");
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

                xdclist_free(&dlist);
            }
        }

        /* xdclist_remove_before */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdclist_remove_before(NULL, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* dlist is empty */
            {
                XDList_PT dlist = xdclist_new();
                xassert_false(xdclist_remove_before(dlist, dlist->head, NULL));
                xdclist_free(&dlist);
            }

            /* node is NULL */
            {
                XDList_PT dlist = xdclist_new();
                xassert_false(xdclist_remove_before(dlist, dlist->head, NULL));
                xdclist_free(&dlist);
            }

            /* node->prev is NULl */
            {
                XDList_PT dlist = xdclist_new();
                xdclist_vload_repeat(dlist, "k", NULL);

                xassert_false(xdclist_remove_before(dlist, dlist->head, NULL));
                xdclist_free(&dlist);
            }

            {
                XDList_PT dlist = xdclist_new();

                void *value = NULL;

                xdclist_push_back_repeat(dlist, "value1");
                xdclist_push_back_repeat(dlist, "value2");
                xdclist_push_back_repeat(dlist, "value3");
                xdclist_push_back_repeat(dlist, "value4");
                xdclist_push_back_repeat(dlist, "value5");

                {
                    XRDList_PT node = xdclist_front_node(dlist)->next->next;
                    xdclist_remove_before(dlist, node, &value);
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
                    XRDList_PT node = xdclist_front_node(dlist)->next->next;
                    xdclist_remove_before(dlist, node, &value);
                    /*
                    *  value1
                    *  value4
                    *  value5
                    */
                    xassert(strcmp(value, "value3") == 0);
                    xassert(strcmp(dlist->head->next->next->value, "value5") == 0);
                    xassert(dlist->head->next->next->next == dlist->head);
                    xassert(dlist->size == 3);
                }

                xassert(strcmp(dlist->head->value, "value1") == 0);
                xassert(strcmp(dlist->tail->value, "value5") == 0);

                xdclist_free(&dlist);
            }
        }

        /* xdclist_remove_after */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdclist_remove_after(NULL, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* dlist is empty */
            {
                XDList_PT dlist = xdclist_new();
                xassert_false(xdclist_remove_after(dlist, dlist->head, NULL));
                xdclist_free(&dlist);
            }

            /* node is NULL */
            {
                XDList_PT dlist = xdclist_new();
                xassert_false(xdclist_remove_after(dlist, dlist->head, NULL));
                xdclist_free(&dlist);
            }

            /* node->next is NULl */
            {
                XDList_PT dlist = xdclist_new();
                xdclist_vload_repeat(dlist, "k", NULL);

                xassert_false(xdclist_remove_after(dlist, dlist->tail, NULL));
                xdclist_free(&dlist);
            }

            {
                XDList_PT dlist = xdclist_new();

                void *value = NULL;

                xdclist_push_back_repeat(dlist, "value1");
                xdclist_push_back_repeat(dlist, "value2");
                xdclist_push_back_repeat(dlist, "value3");
                xdclist_push_back_repeat(dlist, "value4");
                xdclist_push_back_repeat(dlist, "value5");

                {
                    XRDList_PT node = xdclist_front_node(dlist)->next->next;
                    xdclist_remove_after(dlist, node, &value);
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
                    XRDList_PT node = xdclist_front_node(dlist)->next->next;
                    xdclist_remove_after(dlist, node, &value);
                    /*
                    *  value1, value1
                    *  value2, value2
                    *  value3, value3
                    */
                    xassert(strcmp(value, "value5") == 0);
                    xassert(strcmp(dlist->head->next->next->value, "value3") == 0);
                    xassert(dlist->head->next->next->next == dlist->head);
                    xassert(dlist->size == 3);
                }

                xassert(strcmp(dlist->head->value, "value1") == 0);
                xassert(strcmp(dlist->tail->value, "value3") == 0);

                xdclist_free(&dlist);
            }
        }

        /* xdclist_remove_if_impl */
        {
            /* dlist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xdclist_remove_if_impl(NULL, false, false, NULL, NULL, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* dlist is empty */
            {
                XDList_PT dlist = xdclist_new();
                xassert(xdclist_remove_if_impl(dlist, false, false, NULL, NULL, xdclist_test_apply_false, NULL) == 0);
                xdclist_free(&dlist);
            }

            /* no any value match */
            {
                XDList_PT dlist = xdclist_new();
                xdclist_push_back_repeat(dlist, "value1");
                xdclist_push_back_repeat(dlist, "value2");
                xdclist_push_back_repeat(dlist, "value1");

                xassert(xdclist_remove_if_impl(dlist, false, false, xdclist_test_cmp_false, NULL, NULL, NULL) == 0);
                xassert(xdclist_remove_if_impl(dlist, false, false, NULL, NULL, xdclist_test_apply_false, NULL) == 0);

                xassert(xdclist_remove_if_impl(dlist, false, false, test_cmp, NULL, NULL, "value3") == 0);
                xassert(xdclist_remove_if_impl(dlist, false, false, NULL, NULL, xdclist_test_apply, "value3") == 0);

                xdclist_free(&dlist);
            }

            /* remove one */
            {
                XDList_PT dlist = xdclist_new();
                xdclist_push_back_repeat(dlist, "value1");
                xdclist_push_back_repeat(dlist, "value2");
                xdclist_push_back_repeat(dlist, "value3");
                xdclist_push_back_repeat(dlist, "value4");

                xassert(xdclist_remove_if_impl(dlist, false, true, test_cmp, NULL, NULL, "value1") == 1);
                xassert(xdclist_remove_if_impl(dlist, false, true, NULL, NULL, xdclist_test_apply, "value2") == 1);
                xassert(dlist->size == 2);
                xassert(strcmp(dlist->head->value, "value3") == 0);
                xassert(strcmp(dlist->tail->value, "value4") == 0);
                xassert(dlist->head->next->next == dlist->head);

                xdclist_free(&dlist);
            }

            /* remove all */
            {
                XDList_PT dlist = xdclist_new();
                xdclist_push_back_repeat(dlist, "value1");
                xdclist_push_back_repeat(dlist, "value2");
                xdclist_push_back_repeat(dlist, "value1");
                xdclist_push_back_repeat(dlist, "value1");
                xdclist_push_back_repeat(dlist, "value2");
                xdclist_push_back_repeat(dlist, "value1");

                xassert(xdclist_remove_if_impl(dlist, false, false, test_cmp, NULL, NULL, "value1") == 4);
                xassert(xdclist_find(dlist, "value1") == NULL);
                xassert(xdclist_remove_if_impl(dlist, false, false, NULL, NULL, xdclist_test_apply, "value2") == 2);
                xassert(xdclist_find(dlist, "value2") == NULL);
                xassert(dlist->size == 0);

                xdclist_free(&dlist);
            }

            /* deep remove */
            {
                XDList_PT dlist = xdclist_new();
                char *value1 = XMEM_CALLOC(1, 10);
                char *value2 = XMEM_CALLOC(1, 10);
                char *value3 = XMEM_CALLOC(1, 10);

                memcpy(value1, "value1", 6);
                memcpy(value2, "value2", 6);
                memcpy(value3, "value1", 6);

                xdclist_push_back_repeat(dlist, value1);
                xdclist_push_back_repeat(dlist, value2);
                xdclist_push_back_repeat(dlist, value3);

                xassert(xdclist_remove_if_impl(dlist, true, false, test_cmp, NULL, NULL, "value1") == 2);
                xassert(dlist->size == 1);
                xassert(strcmp(dlist->head->value, "value2") == 0);
                xassert(strcmp(dlist->tail->value, "value2") == 0);
                xassert(dlist->head->next == dlist->head);

                xdclist_deep_free(&dlist);
            }
        }

        /* all below remove functions tested by xdclist_remove_if_impl already */

        /* xdclist_remove */
        {
            /* value == NULL */
            {
                XDList_PT dlist = xdclist_new();
                bool except = false;
                XEXCEPT_TRY
                    xdclist_remove(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xdclist_free(&dlist);
            }
        }

        /* xdclist_remove_all */
        {
            /* value == NULL */
            {
                XDList_PT dlist = xdclist_new();
                bool except = false;
                XEXCEPT_TRY
                    xdclist_remove_all(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xdclist_free(&dlist);
            }
        }

        /* xdclist_deep_remove */
        {
            /* value == NULL */
            {
                XDList_PT dlist = xdclist_new();
                bool except = false;
                XEXCEPT_TRY
                    xdclist_deep_remove(dlist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xdclist_free(&dlist);
            }

            /* *value == NULL */
            {
                XDList_PT dlist = xdclist_new();
                void *str = NULL;
                void **value = &str;

                bool except = false;
                XEXCEPT_TRY
                    xdclist_deep_remove(dlist, value);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xdclist_free(&dlist);
            }
        }

        /* xdclist_remove_apply_if */
        /* xdclist_remove_apply_value_if */
        /* xdclist_remove_equal_if */
        /* xdclist_remove_apply_break_if */
        /* xdclist_remove_apply_value_break_if */
        /* xdclist_remove_equal_break_if */
        /* xdclist_deep_remove_apply_if */
        /* xdclist_deep_remove_apply_value_if */
        /* xdclist_deep_remove_equal_if */
        /* xdclist_deep_remove_apply_break_if */
        /* xdclist_deep_remove_apply_value_break_if */
        /* xdclist_deep_remove_equal_break_if */

        /* below two free functions tested by other functions already */
        /* xdclist_free */
        /* xdclist_deep_free */


        /* xdclist_clear */
        /* xdclist_deep_clear */
        {
            /* tested by xslist_free already */
        }

        /* xdclist_swap */
        {
            /* dlist1 == NULL */
            {
                XDList_PT dlist2 = xdclist_new();
                bool except = false;
                XEXCEPT_TRY
                    xdclist_swap(NULL, dlist2);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xdclist_free(&dlist2);
            }

            /* dlist2 == NULL */
            {
                XDList_PT dlist1 = xdclist_new();
                bool except = false;
                XEXCEPT_TRY
                    xdclist_swap(dlist1, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xdclist_free(&dlist1);
            }

            /* normal */
            {
                XDList_PT dlist1 = xdclist_new();
                XDList_PT dlist2 = xdclist_new();

                xdclist_push_back_repeat(dlist1, "value1");
                xdclist_push_back_repeat(dlist1, "value2");
                xdclist_push_back_repeat(dlist1, "value3");
                xdclist_push_back_repeat(dlist1, "value4");

                xdclist_push_back_repeat(dlist2, "value5");
                xdclist_push_back_repeat(dlist2, "value6");
                xdclist_push_back_repeat(dlist2, "value7");
                xdclist_push_back_repeat(dlist2, "value8");
                xdclist_push_back_repeat(dlist2, "value9");

                xassert(xdclist_swap(dlist1, dlist2));

                xassert(strcmp(dlist1->head->value, "value5") == 0);
                xassert(strcmp(dlist1->tail->value, "value9") == 0);

                xassert(strcmp(dlist2->head->value, "value1") == 0);
                xassert(strcmp(dlist2->tail->value, "value4") == 0);

                xassert(dlist1->size == 5);
                xassert(dlist2->size == 4);

                xdclist_free(&dlist1);
                xdclist_free(&dlist2);
            }
        }

        /* xdclist_merge */
        {
            {
                XDList_PT dlist1 = xdclist_new();
                XDList_PT dlist2 = xdclist_new();

                xdclist_push_back_repeat(dlist1, "value1");
                xdclist_push_back_repeat(dlist1, "value2");
                xdclist_push_back_repeat(dlist2, "value1");
                xdclist_push_back_repeat(dlist2, "value4");

                xassert(xdclist_merge(dlist1, &dlist2));

                xassert_false(dlist2);

                xassert(strcmp(dlist1->head->value, "value1") == 0);
                xassert(strcmp(dlist1->tail->value, "value4") == 0);

                xassert(strcmp(dlist1->head->next->next->value, "value1") == 0);

                xassert(dlist1->size == 4);

                xdclist_free(&dlist1);
            }
        }

        /* xdclist_reverse */
        {
            XDList_PT dlist = xdclist_new();

            xdclist_reverse(dlist);

            xdclist_push_back_repeat(dlist, "value1");
            xdclist_push_back_repeat(dlist, "value2");
            xdclist_push_back_repeat(dlist, "value3");
            xdclist_push_back_repeat(dlist, "value4");

            xdclist_reverse(dlist);

            xassert(strcmp(dlist->head->value, "value4") == 0);
            xassert(strcmp(dlist->head->next->value, "value3") == 0);
            xassert(strcmp(dlist->head->next->next->value, "value2") == 0);
            xassert(strcmp(dlist->tail->value, "value1") == 0);

            xassert(dlist->size == 4);

            xdclist_free(&dlist);
        }

        /* xdclist_rotate */
        {
            XDList_PT dlist = xdclist_new();

            xdclist_push_back_repeat(dlist, "value0");
            xdclist_push_back_repeat(dlist, "value1");
            xdclist_push_back_repeat(dlist, "value2");
            xdclist_push_back_repeat(dlist, "value3");
            xdclist_push_back_repeat(dlist, "value4");
            xdclist_push_back_repeat(dlist, "value5");

            xdclist_rotate(dlist, 3);
            xassert(strcmp(dlist->head->value, "value3") == 0);
            xassert(strcmp(dlist->tail->value, "value2") == 0);

            xdclist_rotate(dlist, -3);
            xassert(strcmp(dlist->head->value, "value0") == 0);
            xassert(strcmp(dlist->tail->value, "value5") == 0);

            xdclist_rotate(dlist, 10);
            xassert(strcmp(dlist->head->value, "value4") == 0);
            xassert(strcmp(dlist->tail->value, "value3") == 0);

            xdclist_rotate(dlist, -10);
            xassert(strcmp(dlist->head->value, "value0") == 0);
            xassert(strcmp(dlist->tail->value, "value5") == 0);

            xdclist_free(&dlist);
        }

        /* xdclist_map */
        /* xdclist_map_break_if_true */
        /* xdclist_map_break_if_false */
        {
            XDList_PT dlist = xdclist_new();

            xdclist_push_back_repeat(dlist, "value1");
            xdclist_push_back_repeat(dlist, "value2");
            xdclist_push_back_repeat(dlist, "value3");
            xdclist_push_back_repeat(dlist, "value4");

            /* xdclist_map */
            int count = 0;
            xassert(xdclist_map(dlist, xdclist_test_map_apply_count, (void*)&count) == 4);
            xassert(count == 4);

            /* xdclist_map_break_if_true */
            xassert(xdclist_map_break_if_true(dlist, xdclist_test_apply, "value3"));
            xassert_false(xdclist_map_break_if_true(dlist, xdclist_test_apply, "value5"));

            /* xdclist_map_break_if_false */
            xassert(xdclist_map_break_if_false(dlist, xdclist_test_apply, "value3"));
            xassert_false(xdclist_map_break_if_false(dlist, xdclist_test_apply_true, "value3"));

            xdclist_free(&dlist);
        }

        /* xdclist_find */
        /* xdclist_find_equal_if */
        /* xdclist_find_apply_if */
        {
            XDList_PT dlist = xdclist_new();

            xdclist_push_back_repeat(dlist, "value1");
            xdclist_push_back_repeat(dlist, "value2");
            xdclist_push_back_repeat(dlist, "value3");
            xdclist_push_back_repeat(dlist, "value4");

            /* xdclist_find */
            XRDList_PT node = xdclist_find(dlist, "value2");
            xassert(strcmp(node->value, "value2") == 0);

            /* xdclist_find_equal_if */
            node = xdclist_find_equal_if(dlist, "value3", test_cmp, NULL);
            xassert(strcmp(node->value, "value3") == 0);

            /* xdclist_find_apply_if */
            node = xdclist_find_apply_if(dlist, xdclist_test_apply, "value4");
            xassert(strcmp(node->value, "value4") == 0);

            xdclist_free(&dlist);
        }

        /* xdclist_to_array */
        {
            XDList_PT dlist = xdclist_new();

            xdclist_push_back_repeat(dlist, "value1");
            xdclist_push_back_repeat(dlist, "value2");
            xdclist_push_back_repeat(dlist, "value3");
            xdclist_push_back_repeat(dlist, "value4");

            XPArray_PT array = xdclist_to_array(dlist);

            xassert(array->size == 4);
            xassert(strcmp((char*)xparray_get(array, 0), "value1") == 0);
            xassert(strcmp((char*)xparray_get(array, 1), "value2") == 0);
            xassert(strcmp((char*)xparray_get(array, 2), "value3") == 0);
            xassert(strcmp((char*)xparray_get(array, 3), "value4") == 0);

            xparray_free(&array);
            xdclist_free(&dlist);
        }

        /* xdclist_size */
        /* xdclist_is_empty */

        /* xdclist_unique */
        {
            XDList_PT dlist = xdclist_new();

            xdclist_push_back_repeat(dlist, "value2");
            xdclist_push_back_repeat(dlist, "value1");
            xdclist_push_back_repeat(dlist, "value1");
            xdclist_push_back_repeat(dlist, "value3");
            xdclist_push_back_repeat(dlist, "value2");
            xdclist_push_back_repeat(dlist, "value4");

            xassert(dlist->size == 6);
            xdclist_unique(dlist, test_cmp, NULL);
            xassert(dlist->size == 4);

            xassert(strcmp(dlist->head->value, "value1") == 0);
            xassert(strcmp(dlist->head->next->value, "value2") == 0);
            xassert(strcmp(dlist->head->next->next->value, "value3") == 0);
            xassert(strcmp(dlist->head->next->next->next->value, "value4") == 0);
            xassert(dlist->head->next->next->next->next == dlist->head);
            xassert(strcmp(dlist->tail->value, "value4") == 0);

            xdclist_free(&dlist);
        }

        /* xdclist_deep_unique */
        {
            XDList_PT dlist = xdclist_random_string(1000);
            xdclist_deep_unique(dlist, test_cmp, NULL);
            //xdclist_map(dlist, xdclist_test_print, NULL);
            xdclist_deep_free(&dlist);
        }

        /* xdclist_sort */
        {
            {
                XDList_PT dlist = xdclist_new();

                xdclist_push_back_repeat(dlist, "value2");
                xdclist_push_back_repeat(dlist, "value1");
                xdclist_push_back_repeat(dlist, "value1");
                xdclist_push_back_repeat(dlist, "value3");
                xdclist_push_back_repeat(dlist, "value5");
                xdclist_push_back_repeat(dlist, "value4");

                xdclist_sort(dlist, test_cmp, NULL);

                xassert(strcmp(dlist->head->value, "value1") == 0);
                xassert(strcmp(dlist->head->next->value, "value1") == 0);
                xassert(strcmp(dlist->head->next->next->value, "value2") == 0);
                xassert(strcmp(dlist->head->next->next->next->value, "value3") == 0);
                xassert(strcmp(dlist->head->next->next->next->next->value, "value4") == 0);
                xassert(strcmp(dlist->head->next->next->next->next->next->value, "value5") == 0);
                xassert(dlist->head->next->next->next->next->next->next == dlist->head);
                xassert(strcmp(dlist->head->next->next->next->next->next->prev->value, "value4") == 0);
                xassert(strcmp(dlist->head->next->next->next->next->prev->value, "value3") == 0);
                xassert(strcmp(dlist->head->next->next->next->prev->value, "value2") == 0);
                xassert(strcmp(dlist->head->next->next->prev->value, "value1") == 0);
                xassert(strcmp(dlist->head->next->prev->value, "value1") == 0);
                xassert(dlist->head->prev == dlist->tail);

                xdclist_free(&dlist);
            }

            {
                XDList_PT dlist = xdclist_random_string(1000);
                xdclist_sort(dlist, test_cmp, NULL);
                //xdclist_map(dlist, xdclist_test_print, NULL);
                xdclist_deep_free(&dlist);
            }
        }

        /* xdclist_is_sorted */
        {
            /* tested by xdclist_sort already */
        }

        {
            int count = 0;
            xmem_leak(xdclist_test_check_mem_leak, &count);
            xassert(count == 0);
        }
}


