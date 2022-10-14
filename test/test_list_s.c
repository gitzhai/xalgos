
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../array_pointer/xarray_pointer_x.h"
#include "../list_s_raw/xlist_s_raw_x.h"
#include "../list_s/xlist_s_x.h"
#include "../include/xalgos.h"

static
int xslist_test_cmp_true(void *value1, void *value2, void *cl) {
    return 0;
}

static
int xslist_test_cmp_false(void *value1, void *value2, void *cl) {
    return 1;
}

static
int xslist_test_cmp(void *value1, void *value2, void *cl) {
    return strcmp((char*)value1, (char*)value2);
}

static
bool xslist_test_apply(void *value, void *cl) {
    return strcmp((char*)value, (char*)cl) == 0;
}

static
bool xslist_test_apply_true(void *value, void *cl) {
    return true;
}

static
bool xslist_test_apply_false(void *value, void *cl) {
    return false;
}

static
bool xslist_test_map_apply_count(void *value, void *cl) {
    ++(*((int*)(cl)));
    return true;
}

static
bool xslist_test_print(void *value, void *cl) {
    printf("%s\n", (char*)value);
    return true;
}

static 
XSList_PT xslist_random_string(int size) {
    XSList_PT slist = xslist_new();
    if (!slist) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xslist_deep_free(&slist);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }
            str[str_size - 1] = '\0';

            xslist_push_back_repeat(slist, str);
        }
    }

    return slist;
}

static
void xslist_test_check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}


void test_xslist() {

        /* xslist_new */
        {
            XSList_PT slist = xslist_new();
            xassert(slist);
            xassert_false(slist->head);
            xassert_false(slist->tail);
            xassert_false(slist->size);
            xslist_free(&slist);
        }

        /* xslist_new_with_rslist */
        {
            XRSList_PT list = xrslist_new("a");
            xrslist_push_front_repeat(&list, "b");
            xrslist_push_front_repeat(&list, "c");

            XSList_PT slist = xslist_new_with_rslist(list);
            xassert(strcmp(slist->head->value, "c") == 0);
            xassert(strcmp(slist->tail->value, "a") == 0);
            xassert(slist->size == 3);
            xslist_free(&slist);
        }

        /* xslist_vload_repeat */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xslist_vload_repeat(NULL, "value", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XSList_PT slist = xslist_new();
                xassert(xslist_vload_repeat(slist, NULL) == 0);
                xslist_free(&slist);
            }

            /* normal */
            {
                XSList_PT slist = xslist_new();
                xassert(xslist_vload_repeat(slist, "k1", "k2", "k1", "k3", NULL) == 4);
                xassert(strcmp(slist->head->value, "k1") == 0);
                xassert(strcmp(slist->tail->value, "k3") == 0);
                xassert(slist->size == 4);
                xslist_free(&slist);
            }
        }

        /* xslist_vload_unique */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xslist_vload_unique(NULL, "value", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XSList_PT slist = xslist_new();
                xassert(xslist_vload_unique(slist, NULL) == 0);
                xslist_free(&slist);
            }

            /* repeat value */
            {
                XSList_PT slist = xslist_new();
                xassert(xslist_vload_unique(slist, "k1", "k2", "k1", "k3", NULL) == 3);
                xassert(strcmp(slist->head->value, "k1") == 0);
                xassert(strcmp(slist->tail->value, "k3") == 0);
                xassert(slist->size == 3);
                xslist_free(&slist);
            }
        }

        /* xslist_vload_unique_if */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xslist_vload_unique_if(NULL, xslist_test_cmp, NULL, "k", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }
            
            /* value == NULL */
            {
                XSList_PT slist = xslist_new();
                xassert(xslist_vload_unique_if(slist, xslist_test_cmp, NULL, NULL) == 0);
                xslist_free(&slist);
            }

            /* repeat value */
            {
                XSList_PT slist = xslist_new();
                xassert(xslist_vload_unique_if(slist, xslist_test_cmp, NULL, "k1", "k2", "k1", "k3", NULL) == 3);
                xassert(strcmp(slist->head->value, "k1") == 0);
                xassert(strcmp(slist->tail->value, "k3") == 0);
                xassert(slist->size == 3);
                xslist_free(&slist);
            }
        }

        /* xslist_vload_replace_if */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xslist_vload_replace_if(NULL, xslist_test_cmp, NULL, "k", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XSList_PT slist = xslist_new();
                xassert(xslist_vload_replace_if(slist, xslist_test_cmp, NULL, NULL) == 0);
                xslist_free(&slist);
            }

            /* repeat value */
            {
                XSList_PT slist = xslist_new();
                xassert(xslist_vload_replace_if(slist, xslist_test_cmp, NULL, "k1", "k2", "k1", "k3", NULL) == 3);
                xassert(strcmp(slist->head->value, "k1") == 0);
                xassert(strcmp(slist->tail->value, "k3") == 0);
                xassert(slist->size == 3);
                xslist_free(&slist);
            }
        }

        /* xslist_aload_repeat */
        {
            /* slist == NULL */
            {
                XPArray_PT array = xparray_new(10);
                xparray_put(array, 0, "k", NULL);
                bool except = false;
                XEXCEPT_TRY
                    xslist_aload_repeat(NULL, array);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xparray_free(&array);
            }

            /* xs == NULL */
            {
                XSList_PT slist = xslist_new();
                bool except = false;
                XEXCEPT_TRY
                    xslist_aload_repeat(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xslist_free(&slist);
            }

            /* value == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XSList_PT slist = xslist_new();

                xassert(xslist_aload_repeat(slist, array) == 0);

                xslist_free(&slist);
                xparray_free(&array);
            }

            /* normal */
            {
                XPArray_PT array = xparray_new(10);
                XSList_PT slist = xslist_new();

                xparray_vload(array, "k1", "k2", "k1", "k3", NULL);
                xassert(xslist_aload_repeat(slist, array) == 4);
                xassert(strcmp(slist->head->value, "k1") == 0);
                xassert(strcmp(slist->tail->value, "k3") == 0);
                xassert(slist->size == 4);

                xslist_free(&slist);
                xparray_free(&array);
            }
        }

        /* xslist_aload_unique */
        {
            /* slist == NULL */
            {
                XPArray_PT array = xparray_new(10);
                xparray_put(array, 0, "k", NULL);

                bool except = false;
                XEXCEPT_TRY
                    xslist_aload_unique(NULL, array);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xparray_free(&array);
            }

            /* xs == NULL */
            {
                XSList_PT slist = xslist_new();
                bool except = false;
                XEXCEPT_TRY
                    xslist_aload_unique(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xslist_free(&slist);
            }

            /* value == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XSList_PT slist = xslist_new();

                xassert(xslist_aload_unique(slist, array) == 0);

                xslist_free(&slist);
                xparray_free(&array);
            }

            /* repeat value */
            {
                XPArray_PT array = xparray_new(10);
                XSList_PT slist = xslist_new();

                xparray_vload(array, "k1", "k2", "k1", "k3", NULL);
                xassert(xslist_aload_unique(slist, array) == 3);
                xassert(strcmp(slist->head->value, "k1") == 0);
                xassert(strcmp(slist->tail->value, "k3") == 0);
                xassert(slist->size == 3);

                xslist_free(&slist);
                xparray_free(&array);
            }
        }

        /* xslist_aload_unique_if */
        {
            /* cmp == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XSList_PT slist = xslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xslist_aload_unique_if(slist, array, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xslist_free(&slist);
                xparray_free(&array);
            }

            /* below 3 scenarios tested already in xslist_aload_unique */
            /* xs == NULL */
            /* value == NULL */
            /* value == NULL */

            /* repeat value */
            {
                XPArray_PT array = xparray_new(10);
                XSList_PT slist = xslist_new();

                xparray_vload(array, "k1", "k2", "k1", "k3", NULL);
                xassert(xslist_aload_unique_if(slist, array, xslist_test_cmp, NULL) == 3);
                xassert(strcmp(slist->head->value, "k1") == 0);
                xassert(strcmp(slist->tail->value, "k3") == 0);
                xassert(slist->size == 3);

                xslist_free(&slist);
                xparray_free(&array);
            }
        }

        /* xslist_aload_replace_if */
        {
            /* cmp == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XSList_PT slist = xslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xslist_aload_replace_if(slist, array, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);

                xslist_free(&slist);
                xparray_free(&array);
            }

            /* below 3 scenarios tested already in xslist_aload_replace */
            /* xs == NULL */
            /* value == NULL */
            /* value == NULL */

            /* repeat value */
            {
                XPArray_PT array = xparray_new(10);
                XSList_PT slist = xslist_new();

                xparray_vload(array, "k1", "k2", "k1", "k3", NULL);
                xassert(xslist_aload_replace_if(slist, array, xslist_test_cmp, NULL) == 3);
                xassert(strcmp(slist->head->value, "k1") == 0);
                xassert(strcmp(slist->tail->value, "k3") == 0);
                xassert(slist->size == 3);

                xslist_free(&slist);
                xparray_free(&array);
            }
        }

        /* xslist_copyn_if_impl */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xslist_copyn_if_impl(NULL, 0, 1, false, xslist_test_cmp, NULL, NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* count < 0 */
            {
                XSList_PT slist = xslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xslist_copyn_if_impl(slist, 0, -1, false, xslist_test_cmp, NULL, xslist_test_apply_true, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xslist_free(&slist);
            }

            /* deep == true && value_size < 0 */
            {
                XSList_PT slist = xslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xslist_copyn_if_impl(slist, -1, 1, true, xslist_test_cmp, NULL, xslist_test_apply_true, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xslist_free(&slist);
            }

            /* deep == true && value_size == 0 */
            {
                XSList_PT slist = xslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xslist_copyn_if_impl(slist, 0, 1, true, xslist_test_cmp, NULL, xslist_test_apply_true, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xslist_free(&slist);
            }

            /* slist is empty */
            {
                XSList_PT slist = xslist_new();

                {
                    XSList_PT nslist = xslist_copyn_if_impl(slist, 0, 1, false, NULL, NULL, xslist_test_apply_true, NULL);
                    xassert(nslist->size == 0);
                    xslist_free(&nslist);
                }

                xslist_free(&slist);
            }

            /* count == 0 */
            {
                XSList_PT slist = xslist_new();
                xslist_vload_repeat(slist, "k1", "k2", NULL);

                {
                    XSList_PT nslist = xslist_copyn_if_impl(slist, 0, 0, false, NULL, NULL, xslist_test_apply_true, NULL);
                    xassert(nslist->size == 0);
                    xslist_free(&nslist);
                }

                xslist_free(&slist);
            }

            /* slist->size < count */
            {
                XSList_PT slist = xslist_new();
                xslist_vload_repeat(slist, "k1", "k1", NULL);

                {
                    XSList_PT nslist = xslist_copyn_if_impl(slist, 0, 3, false, NULL, NULL, xslist_test_apply_true, NULL);
                    xassert(nslist->size == 2);
                    xslist_free(&nslist);
                }

                xslist_free(&slist);
            }

            /* count < slist->size */
            {
                XSList_PT slist = xslist_new();
                xslist_vload_repeat(slist, "k1", "k1", NULL);

                {
                    XSList_PT nslist = xslist_copyn_if_impl(slist, 0, 1, false, NULL, NULL, xslist_test_apply_true, NULL);
                    xassert(nslist->size == 1);
                    xassert(strcmp(nslist->head->value, "k1") == 0);
                    xslist_free(&nslist);
                }

                xslist_free(&slist);
            }

            /* no any value match */
            {
                XSList_PT slist = xslist_new();
                xslist_vload_repeat(slist, "k1", "k2", NULL);

                {
                    XSList_PT nslist = xslist_copyn_if_impl(slist, 0, 1, false, NULL, NULL, xslist_test_apply_false, NULL);
                    xassert(nslist->size == 0);
                    xslist_free(&nslist);
                }

                xslist_free(&slist);
            }

            /* normal */
            {
                XSList_PT slist = xslist_new();
                xslist_vload_repeat(slist, "k1", "k1", "k3", NULL);

                {
                    XSList_PT nslist = xslist_copyn_if_impl(slist, 0, 2, false, NULL, NULL, xslist_test_apply_true, NULL);
                    xassert(nslist->size == 2);
                    xassert(strcmp(nslist->head->value, "k1") == 0);
                    xassert(strcmp(nslist->head->next->value, "k1") == 0);
                    xslist_free(&nslist);
                }

                {
                    XSList_PT nslist = xslist_copyn_if_impl(slist, 0, 2, false, NULL, NULL, xslist_test_apply, "k3");
                    xassert(nslist->size == 1);
                    xassert(strcmp(nslist->head->value, "k3") == 0);
                    xslist_free(&nslist);
                }

                {
                    XSList_PT nslist = xslist_copyn_if_impl(slist, 0, 2, false, xslist_test_cmp, NULL, NULL, "k3");
                    xassert(nslist->size == 1);
                    xassert(strcmp(nslist->head->value, "k3") == 0);
                    xslist_free(&nslist);
                }

                xslist_free(&slist);
            }
        }

        /* xslist_copyn_equal_if */
        {
            /* value == NULL */
            {
                XSList_PT slist = xslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xslist_copyn_equal_if(slist, 1, NULL, xslist_test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xslist_free(&slist);
            }
        }

        /* xslist_deep_copyn_equal_if */
        {
            /* value == NULL */
            {
                XSList_PT slist = xslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xslist_deep_copyn_equal_if(slist, 1, 1, NULL, xslist_test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);

                xslist_free(&slist);
            }
        }
        
        /* xslist_copy */
        /* xslist_copy_equal_if */
        /* xslist_copy_apply_if */
        /* xslist_copy_apply_value_if */
        /* xslist_copyn */
        /* xslist_copyn_equal_if */
        /* xslist_copyn_apply_if */
        /* xslist_copyn_apply_value_if */
        /* xslist_deep_copy */
        /* xslist_deep_copy_equal_if */
        /* xslist_deep_copy_apply_if */
        /* xslist_deep_copy_apply_value_if */
        /* xslist_deep_copyn */
        /* xslist_deep_copyn_equal_if */
        /* xslist_deep_copyn_apply_if */
        /* xslist_deep_copyn_apply_value_if */
        {
            /* tested by xslist_copyn_if_impl already */
        }

        /* xslist_push_front_repeat */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xslist_push_front_repeat(NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XSList_PT slist = xslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xslist_push_front_repeat(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xslist_free(&slist);
            }

            /* normal */
            {
                XSList_PT slist = xslist_new();

                char *str = "value1";

                xslist_push_front_repeat(slist, str);
                xslist_push_front_repeat(slist, "value2");
                xslist_push_front_repeat(slist, str);
                xslist_push_front_repeat(slist, "value3");

                xassert(strcmp(slist->head->value, "value3") == 0);
                xassert(strcmp(slist->head->next->value, "value1") == 0);
                xassert(strcmp(slist->head->next->next->value, "value2") == 0);
                xassert(strcmp(slist->head->next->next->next->value, "value1") == 0);
                xassert_false(slist->head->next->next->next->next);
                xassert(strcmp(slist->tail->value, "value1") == 0);

                xassert(slist->size == 4);

                xslist_free(&slist);
            }
        }

        /* xslist_pop_front */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xslist_pop_front(NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* slist is empty */
            {
                XSList_PT slist = xslist_new();
                xassert(xslist_pop_front(slist) == false);
                xslist_free(&slist);
            }

            {
                XSList_PT slist = xslist_new();

                xslist_push_back_repeat(slist, "value1");
                xslist_push_back_repeat(slist, "value2");
                xslist_push_back_repeat(slist, "value3");

                {
                    void *value = NULL;

                    xassert(value = xslist_pop_front(slist));
                    xassert(strcmp(value, "value1") == 0);

                    xassert(value = xslist_pop_front(slist));
                    xassert(strcmp(value, "value2") == 0);

                    xassert(xslist_pop_front(slist));

                    xassert(slist->size == 0);
                    xassert(slist->head == NULL);
                    xassert(slist->tail == NULL);
                }

                xslist_free(&slist);
            }
        }

        /* xslist_front */
        {
            xassert_false(xslist_front(NULL));

            /* slist is empty */
            {
                XSList_PT slist = xslist_new();
                xassert_false(xslist_front(slist));
                xslist_free(&slist);
            }

            /* normal */
            {
                XSList_PT slist = xslist_new();

                xslist_push_back_repeat(slist, "value1");
                xslist_push_back_repeat(slist, "value2");
                xslist_push_back_repeat(slist, "value3");

                xassert(strcmp(xslist_front(slist), "value1") == 0);

                xslist_free(&slist);
            }
        }

        /* xslist_back */
        {
            xassert_false(xslist_back(NULL));

            /* slist is empty */
            {
                XSList_PT slist = xslist_new();
                xassert_false(xslist_back(slist));
                xslist_free(&slist);
            }

            /* normal */
            {
                XSList_PT slist = xslist_new();

                xslist_push_back_repeat(slist, "value1");
                xslist_push_back_repeat(slist, "value2");
                xslist_push_back_repeat(slist, "value3");

                xassert(strcmp(xslist_back(slist), "value3") == 0);

                xslist_free(&slist);
            }
        }

        /* xslist_front_node */
        {
            xassert_false(xslist_front_node(NULL));

            /* slist is empty */
            {
                XSList_PT slist = xslist_new();
                xassert_false(xslist_front_node(slist));
                xslist_free(&slist);
            }

            /* normal */
            {
                XSList_PT slist = xslist_new();

                xslist_push_back_repeat(slist, "value1");
                xslist_push_back_repeat(slist, "value2");
                xslist_push_back_repeat(slist, "value3");

                xassert(strcmp(xslist_front_node(slist)->value, "value1") == 0);

                xslist_free(&slist);
            }
        }

        /* xslist_back_node */
        {
            xassert_false(xslist_back_node(NULL));

            /* slist is empty */
            {
                XSList_PT slist = xslist_new();
                xassert_false(xslist_back_node(slist));
                xslist_free(&slist);
            }

            /* normal */
            {
                XSList_PT slist = xslist_new();

                xslist_push_back_repeat(slist, "value1");
                xslist_push_back_repeat(slist, "value2");
                xslist_push_back_repeat(slist, "value3");

                xassert(strcmp(xslist_back_node(slist)->value, "value3") == 0);

                xslist_free(&slist);
            }
        }

        /* xslist_push_back_repeat */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xslist_push_back_repeat(NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XSList_PT slist = xslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xslist_push_back_repeat(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xslist_free(&slist);
            }

            /* normal */
            {
                XSList_PT slist = xslist_new();

                char *str = "value1";

                xslist_push_back_repeat(slist, str);
                xslist_push_back_repeat(slist, "value2");
                xslist_push_back_repeat(slist, str);
                xslist_push_back_repeat(slist, "value3");

                xassert(strcmp(slist->head->value, "value1") == 0);
                xassert(strcmp(slist->head->next->value, "value2") == 0);
                xassert(strcmp(slist->head->next->next->value, "value1") == 0);
                xassert(strcmp(slist->head->next->next->next->value, "value3") == 0);
                xassert_false(slist->head->next->next->next->next);
                xassert(strcmp(slist->tail->value, "value3") == 0);

                xassert(slist->size == 4);
                xslist_free(&slist);
            }
        }

        /* xslist_push_back_rslist */
        {
            xassert_false(xslist_push_back_rslist(NULL, NULL));

            {
                XRSList_PT rslist = xrslist_new("k1");
                xrslist_push_front_repeat(&rslist, "k2");
                xrslist_push_front_repeat(&rslist, "k3");
                xrslist_push_front_repeat(&rslist, "k4");

                XSList_PT slist = xslist_new();
                xslist_push_back_rslist(slist, rslist);

                xassert(strcmp(slist->head->value, "k4") == 0);
                xassert(strcmp(slist->head->next->value, "k3") == 0);
                xassert(strcmp(slist->head->next->next->value, "k2") == 0);
                xassert(strcmp(slist->head->next->next->next->value, "k1") == 0);
                xassert_false(slist->head->next->next->next->next);
                xassert(strcmp(slist->tail->value, "k1") == 0);

                xassert(slist->size == 4);
                xslist_free(&slist);
            }

            {
                XRSList_PT rslist = xrslist_new("k1");
                xrslist_push_front_repeat(&rslist, "k2");
                xrslist_push_front_repeat(&rslist, "k3");
                xrslist_push_front_repeat(&rslist, "k4");

                XSList_PT slist = xslist_new();
                xslist_push_back_repeat(slist, "k5");
                xslist_push_back_repeat(slist, "k6");

                xslist_push_back_rslist(slist, rslist);

                xassert(strcmp(slist->head->value, "k5") == 0);
                xassert(strcmp(slist->head->next->next->value, "k4") == 0);
                xassert(strcmp(slist->head->next->next->next->value, "k3") == 0);
                xassert(strcmp(slist->tail->value, "k1") == 0);

                xassert(slist->size == 6);
                xslist_free(&slist);
            }
        }

        /* xslist_push_front_unique */
        {
            XSList_PT slist = xslist_new();

            char *str = "value1"; 

            xslist_push_front_unique(slist, str);
            xslist_push_front_unique(slist, "value2");
            xslist_push_front_unique(slist, str);
            xslist_push_front_unique(slist, "value3");

            xassert(strcmp(slist->head->value, "value3") == 0);
            xassert(strcmp(slist->head->next->value, "value2") == 0);
            xassert(strcmp(slist->head->next->next->value, "value1") == 0);
            xassert_false(slist->head->next->next->next);
            xassert(strcmp(slist->tail->value, "value1") == 0);
            xassert(slist->size == 3);

            xslist_free(&slist);
        }

        /* xslist_push_front_unique_if */
        {
            XSList_PT slist = xslist_new();

            xslist_push_front_unique_if(slist, "value1", xslist_test_cmp, NULL);
            xslist_push_front_unique_if(slist, "value2", xslist_test_cmp, NULL);
            xslist_push_front_unique_if(slist, "value1", xslist_test_cmp, NULL);
            xslist_push_front_unique_if(slist, "value3", xslist_test_cmp, NULL);

            xassert(strcmp(slist->head->value, "value3") == 0);
            xassert(strcmp(slist->head->next->value, "value2") == 0);
            xassert(strcmp(slist->head->next->next->value, "value1") == 0);
            xassert(strcmp(slist->tail->value, "value1") == 0);
            xassert_false(slist->head->next->next->next);
            xassert(slist->size == 3);

            xslist_free(&slist);
        }

        /* xslist_push_front_replace_if */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xslist_push_front_replace_if(NULL, "k", xslist_test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
            }

            /* value == NULL */
            {
                XSList_PT slist = xslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xslist_push_front_replace_if(slist, NULL, xslist_test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xslist_free(&slist);
            }
            
            /* repeat value */
            {
                XSList_PT slist = xslist_new();

                xslist_push_front_replace_if(slist, "value1", xslist_test_cmp, NULL);
                xslist_push_front_replace_if(slist, "value2", xslist_test_cmp, NULL);
                xslist_push_front_replace_if(slist, "value1", xslist_test_cmp, NULL);
                xslist_push_front_replace_if(slist, "value3", xslist_test_cmp, NULL);

                xassert(strcmp(slist->head->value, "value3") == 0);
                xassert(strcmp(slist->head->next->value, "value2") == 0);
                xassert(strcmp(slist->head->next->next->value, "value1") == 0);
                xassert(strcmp(slist->tail->value, "value1") == 0);
                xassert_false(slist->head->next->next->next);
                xassert(slist->size == 3);

                xslist_free(&slist);
            }
        }

        /* xslist_push_front_deep_replace_if */
        {
            /* below scenarios tested by xslist_push_front_replace_if already */
            /* slist == NULL */
            /* value == NULL */
            /* cmp == NULL */

            /* repeat value */
            {
                XSList_PT slist = xslist_new();

                char *value = XMEM_CALLOC(1, 7);
                memcpy(value, "value1", 6);

                xslist_push_front_deep_replace_if(slist, value, xslist_test_cmp, NULL);
                xslist_push_front_deep_replace_if(slist, "value2", xslist_test_cmp, NULL);
                xslist_push_front_deep_replace_if(slist, "value1", xslist_test_cmp, NULL);
                xslist_push_front_deep_replace_if(slist, "value3", xslist_test_cmp, NULL);

                xassert(strcmp(slist->head->value, "value3") == 0);
                xassert(strcmp(slist->head->next->value, "value2") == 0);
                xassert(strcmp(slist->head->next->next->value, "value1") == 0);
                xassert(strcmp(slist->tail->value, "value1") == 0);
                xassert_false(slist->head->next->next->next);
                xassert(slist->size == 3);

                xslist_free(&slist);
            }
        }

        /* xslist_push_back_unique */
        {
            XSList_PT slist = xslist_new();

            char *str = "value1";

            xslist_push_back_unique(slist, str);
            xslist_push_back_unique(slist, "value2");
            xslist_push_back_unique(slist, str);
            xslist_push_back_unique(slist, "value3");

            xassert(strcmp(slist->head->value, "value1") == 0);
            xassert(strcmp(slist->head->next->value, "value2") == 0);
            xassert(strcmp(slist->head->next->next->value, "value3") == 0);
            xassert(strcmp(slist->tail->value, "value3") == 0);
            xassert_false(slist->head->next->next->next);
            xassert(slist->size == 3);

            xslist_free(&slist);
        }

        /* xslist_push_back_unique_if */
        {
            XSList_PT slist = xslist_new();

            xslist_push_back_unique_if(slist, "value1", xslist_test_cmp, NULL);
            xslist_push_back_unique_if(slist, "value2", xslist_test_cmp, NULL);
            xslist_push_back_unique_if(slist, "value1", xslist_test_cmp, NULL);
            xslist_push_back_unique_if(slist, "value3", xslist_test_cmp, NULL);

            xassert(strcmp(slist->head->value, "value1") == 0);
            xassert(strcmp(slist->head->next->value, "value2") == 0);
            xassert(strcmp(slist->head->next->next->value, "value3") == 0);
            xassert(strcmp(slist->tail->value, "value3") == 0);
            xassert_false(slist->head->next->next->next);
            xassert(slist->size == 3);

            xslist_free(&slist);
        }

        /* xslist_push_back_replace_if */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xslist_push_back_replace_if(NULL, "k", xslist_test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XSList_PT slist = xslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xslist_push_back_replace_if(slist, NULL, xslist_test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xslist_free(&slist);
            }

            /* repeat value */
            {
                XSList_PT slist = xslist_new();

                xslist_push_back_replace_if(slist, "value1", xslist_test_cmp, NULL);
                xslist_push_back_replace_if(slist, "value2", xslist_test_cmp, NULL);
                xslist_push_back_replace_if(slist, "value1", xslist_test_cmp, NULL);
                xslist_push_back_replace_if(slist, "value3", xslist_test_cmp, NULL);

                xassert(strcmp(slist->head->value, "value1") == 0);
                xassert(strcmp(slist->head->next->value, "value2") == 0);
                xassert(strcmp(slist->head->next->next->value, "value3") == 0);
                xassert(strcmp(slist->tail->value, "value3") == 0);
                xassert_false(slist->head->next->next->next);
                xassert(slist->size == 3);

                xslist_free(&slist);
            }
        }

        /* xslist_push_back_deep_replace_if */
        {
            /* below scenarios tested by xslist_push_back_replace_if already */
            /* slist == NULL */
            /* value == NULL */
            /* cmp == NULL */

            /* repeat value */
            {
                XSList_PT slist = xslist_new();

                char *value = XMEM_CALLOC(1, 7);
                memcpy(value, "value1", 6);

                xslist_push_back_deep_replace_if(slist, value, xslist_test_cmp, NULL);
                xslist_push_back_deep_replace_if(slist, "value2", xslist_test_cmp, NULL);
                xslist_push_back_deep_replace_if(slist, "value1", xslist_test_cmp, NULL);
                xslist_push_back_deep_replace_if(slist, "value3", xslist_test_cmp, NULL);

                xassert(strcmp(slist->head->value, "value1") == 0);
                xassert(strcmp(slist->head->next->value, "value2") == 0);
                xassert(strcmp(slist->head->next->next->value, "value3") == 0);
                xassert(strcmp(slist->tail->value, "value3") == 0);
                xassert_false(slist->head->next->next->next);
                xassert(slist->size == 3);

                xslist_free(&slist);
            }
        }

        /* xslist_insert_after */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xslist_insert_after(NULL, NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XSList_PT slist = xslist_new();

                bool except = false;
                XEXCEPT_TRY
                    xslist_insert_after(slist, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xslist_free(&slist);
            }

            {
                XSList_PT slist = xslist_new();

                xslist_push_back_repeat(slist, "value1");
                xslist_push_back_repeat(slist, "value2");
                xslist_push_back_repeat(slist, "value3");

                {
                    xslist_insert_after(slist, NULL, "value2");
                    /*
                     *  value1
                     *  value2
                     *  value3
                     *  value2
                     */
                    xassert(strcmp(slist->head->next->next->next->value, "value2") == 0);
                    xassert_false(slist->head->next->next->next->next);
                    xassert(strcmp(slist->tail->value, "value2") == 0);
                }

                {
                    XRSList_PT node = xslist_front_node(slist)->next->next;
                    xslist_insert_after(slist, node, "value4");
                    /*
                    * value1
                    * value2
                    * value3
                    * value4
                    * value2
                    */
                    xassert(strcmp(slist->head->next->next->next->value, "value4") == 0);
                    xassert(strcmp(slist->head->next->next->next->next->value, "value2") == 0);
                }

                xassert(strcmp(slist->head->value, "value1") == 0);
                xassert(slist->size == 5);

                xslist_free(&slist);
            }
        }

        /* xslist_remove_after */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xslist_remove_after(NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* slist is empty */
            {
                XSList_PT slist = xslist_new();
                xassert_false(xslist_remove_after(slist, slist->head));
                xslist_free(&slist);
            }

            /* node is NULL */
            {
                XSList_PT slist = xslist_new();
                xassert_false(xslist_remove_after(slist, slist->head));
                xslist_free(&slist);
            }

            /* node->next is NULl */
            {
                XSList_PT slist = xslist_new();
                xslist_vload_repeat(slist, "k", NULL);

                xassert_false(xslist_remove_after(slist, slist->tail));
                xslist_free(&slist);
            }

            {
                XSList_PT slist = xslist_new();

                void *value = NULL;

                xslist_push_back_repeat(slist, "value1");
                xslist_push_back_repeat(slist, "value2");
                xslist_push_back_repeat(slist, "value3");
                xslist_push_back_repeat(slist, "value4");
                xslist_push_back_repeat(slist, "value5");

                {
                    XRSList_PT node = xslist_front_node(slist)->next->next;
                    value = xslist_remove_after(slist, node);
                    /*
                    *  value1
                    *  value2
                    *  value3
                    *  value5
                    */
                    xassert(strcmp(value, "value4") == 0);
                    xassert(strcmp(slist->head->next->next->next->value, "value5") == 0);
                    xassert(slist->size == 4);
                }

                {
                    XRSList_PT node = xslist_front_node(slist)->next->next;
                    value = xslist_remove_after(slist, node);
                    /*
                    *  value1, value1
                    *  value2, value2
                    *  value3, value3
                    */
                    xassert(strcmp(value, "value5") == 0);
                    xassert(strcmp(slist->head->next->next->value, "value3") == 0);
                    xassert_false(slist->head->next->next->next);
                    xassert(slist->size == 3);
                }

                xassert(strcmp(slist->head->value, "value1") == 0);
                xassert(strcmp(slist->tail->value, "value3") == 0);

                xslist_free(&slist);
            }
        }

        /* xslist_remove_if_impl */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xslist_remove_if_impl(NULL, false, false, NULL, NULL, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* slist is empty */
            {
                XSList_PT slist = xslist_new();
                xassert(xslist_remove_if_impl(slist, false, false, NULL, NULL, xslist_test_apply_false, NULL) == 0);
                xslist_free(&slist);
            }

            /* no any value match */
            {
                XSList_PT slist = xslist_new();
                xslist_push_back_repeat(slist, "value1");
                xslist_push_back_repeat(slist, "value2");
                xslist_push_back_repeat(slist, "value1");

                xassert(xslist_remove_if_impl(slist, false, false, xslist_test_cmp_false, NULL, NULL, NULL) == 0);
                xassert(xslist_remove_if_impl(slist, false, false, NULL, NULL, xslist_test_apply_false, NULL) == 0);

                xassert(xslist_remove_if_impl(slist, false, false, xslist_test_cmp, NULL, NULL, "value3") == 0);
                xassert(xslist_remove_if_impl(slist, false, false, NULL, NULL, xslist_test_apply, "value3") == 0);

                xslist_free(&slist);
            }

            /* remove one */
            {
                XSList_PT slist = xslist_new();
                xslist_push_back_repeat(slist, "value1");
                xslist_push_back_repeat(slist, "value2");
                xslist_push_back_repeat(slist, "value3");
                xslist_push_back_repeat(slist, "value4");

                xassert(xslist_remove_if_impl(slist, false, true, xslist_test_cmp, NULL, NULL, "value1") == 1);
                xassert(xslist_remove_if_impl(slist, false, true, NULL, NULL, xslist_test_apply, "value2") == 1);
                xassert(slist->size == 2);
                xassert(strcmp(slist->head->value, "value3") == 0);
                xassert(strcmp(slist->tail->value, "value4") == 0);
                xassert_false(slist->head->next->next);

                xslist_free(&slist);
            }

            /* remove all */
            {
                XSList_PT slist = xslist_new();
                xslist_push_back_repeat(slist, "value1");
                xslist_push_back_repeat(slist, "value2");
                xslist_push_back_repeat(slist, "value1");
                xslist_push_back_repeat(slist, "value1");
                xslist_push_back_repeat(slist, "value2");
                xslist_push_back_repeat(slist, "value1");

                xassert(xslist_remove_if_impl(slist, false, false, xslist_test_cmp, NULL, NULL, "value1") == 4);
                xassert(xslist_find(slist, "value1") == NULL);
                xassert(xslist_remove_if_impl(slist, false, false, NULL, NULL, xslist_test_apply, "value2") == 2);
                xassert(xslist_find(slist, "value2") == NULL);
                xassert(slist->size == 0);

                xslist_free(&slist);
            }

            /* deep remove */
            {
                XSList_PT slist = xslist_new();
                char *value1 = XMEM_CALLOC(1, 10);
                char *value2 = XMEM_CALLOC(1, 10);
                char *value3 = XMEM_CALLOC(1, 10);

                memcpy(value1, "value1", 6);
                memcpy(value2, "value2", 6);
                memcpy(value3, "value1", 6);

                xslist_push_back_repeat(slist, value1);
                xslist_push_back_repeat(slist, value2);
                xslist_push_back_repeat(slist, value3);

                xassert(xslist_remove_if_impl(slist, true, false, xslist_test_cmp, NULL, NULL, "value1") == 2);
                xassert(slist->size == 1);
                xassert(strcmp(slist->head->value, "value2") == 0);
                xassert(strcmp(slist->tail->value, "value2") == 0);
                xassert_false(slist->head->next);

                xslist_deep_free(&slist);
            }
        }

        /* all below remove functions tested by xslist_remove_if_impl already */

        /* xslist_remove */
        {
            /* value == NULL */
            {
                XSList_PT slist = xslist_new();
                bool except = false;
                XEXCEPT_TRY
                    xslist_remove(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xslist_free(&slist);
            }
        }

        /* xslist_remove_all */
        {
            /* value == NULL */
            {
                XSList_PT slist = xslist_new();
                bool except = false;
                XEXCEPT_TRY
                    xslist_remove_all(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xslist_free(&slist);
            }
        }

        /* xslist_deep_remove */
        {
            /* value == NULL */
            {
                XSList_PT slist = xslist_new();
                bool except = false;
                XEXCEPT_TRY
                    xslist_deep_remove(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xslist_free(&slist);
            }

            /* *value == NULL */
            {
                XSList_PT slist = xslist_new();
                void *str = NULL;
                void **value = &str;

                bool except = false;
                XEXCEPT_TRY
                    xslist_deep_remove(slist, value);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xslist_free(&slist);
            }
        }

        /* xslist_remove_apply_if */
        /* xslist_remove_apply_value_if */
        /* xslist_remove_equal_if */
        /* xslist_remove_apply_break_if */
        /* xslist_remove_apply_value_break_if */
        /* xslist_remove_equal_break_if */
        /* xslist_deep_remove_apply_if */
        /* xslist_deep_remove_apply_value_if */
        /* xslist_deep_remove_equal_if */
        /* xslist_deep_remove_apply_break_if */
        /* xslist_deep_remove_apply_value_break_if */
        /* xslist_deep_remove_equal_break_if */

        /* below two free functions tested by other functions already */
        /* xslist_free */
        /* xslist_deep_free */


        /* xslist_clear */
        /* xslist_deep_clear */
        {
            /* tested by xslist_free already */
        }

        /* xslist_swap */
        {
            /* slist1 == NULL */
            {
                XSList_PT slist2 = xslist_new();
                bool except = false;
                XEXCEPT_TRY
                    xslist_swap(NULL, slist2);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xslist_free(&slist2);
            }

            /* slist2 == NULL */
            {
                XSList_PT slist1 = xslist_new();
                bool except = false;
                XEXCEPT_TRY
                    xslist_swap(slist1, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xslist_free(&slist1);
            }

            /* normal */
            {
                XSList_PT slist1 = xslist_new();
                XSList_PT slist2 = xslist_new();

                xslist_push_back_repeat(slist1, "value1");
                xslist_push_back_repeat(slist1, "value2");
                xslist_push_back_repeat(slist1, "value3");
                xslist_push_back_repeat(slist1, "value4");

                xslist_push_back_repeat(slist2, "value5");
                xslist_push_back_repeat(slist2, "value6");
                xslist_push_back_repeat(slist2, "value7");
                xslist_push_back_repeat(slist2, "value8");
                xslist_push_back_repeat(slist2, "value9");

                xassert(xslist_swap(slist1, slist2));

                xassert(strcmp(slist1->head->value, "value5") == 0);
                xassert(strcmp(slist1->tail->value, "value9") == 0);

                xassert(strcmp(slist2->head->value, "value1") == 0);
                xassert(strcmp(slist2->tail->value, "value4") == 0);

                xassert(slist1->size == 5);
                xassert(slist2->size == 4);

                xslist_free(&slist1);
                xslist_free(&slist2);
            }
        }

        /* xslist_merge */
        {
            {
                XSList_PT slist1 = xslist_new();
                XSList_PT slist2 = xslist_new();

                xslist_push_back_repeat(slist1, "value1");
                xslist_push_back_repeat(slist1, "value2");
                xslist_push_back_repeat(slist2, "value1");
                xslist_push_back_repeat(slist2, "value4");

                xassert(xslist_merge(slist1, &slist2));

                xassert_false(slist2);

                xassert(strcmp(slist1->head->value, "value1") == 0);
                xassert(strcmp(slist1->tail->value, "value4") == 0);

                xassert(strcmp(slist1->head->next->next->value, "value1") == 0);

                xassert(slist1->size == 4);

                xslist_free(&slist1);
            }
        }

        /* xslist_reverse */
        {
            XSList_PT slist = xslist_new();

            xslist_reverse(slist);

            xslist_push_back_repeat(slist, "value1");
            xslist_push_back_repeat(slist, "value2");
            xslist_push_back_repeat(slist, "value3");
            xslist_push_back_repeat(slist, "value4");

            xslist_reverse(slist);

            xassert(strcmp(slist->head->value, "value4") == 0);
            xassert(strcmp(slist->head->next->value, "value3") == 0);
            xassert(strcmp(slist->head->next->next->value, "value2") == 0);
            xassert(strcmp(slist->tail->value, "value1") == 0);

            xassert(slist->size == 4);

            xslist_free(&slist);
        }

        /* xslist_map */
        /* xslist_map_break_if_true */
        /* xslist_map_break_if_false */
        {
            XSList_PT slist = xslist_new();

            xslist_push_back_repeat(slist, "value1");
            xslist_push_back_repeat(slist, "value2");
            xslist_push_back_repeat(slist, "value3");
            xslist_push_back_repeat(slist, "value4");

            /* xslist_map */
            int count = 0;
            xassert(xslist_map(slist, xslist_test_map_apply_count, (void*)&count) == 4);
            xassert(count == 4);

            /* xslist_map_break_if_true */
            xassert(xslist_map_break_if_true(slist, xslist_test_apply, "value3"));
            xassert_false(xslist_map_break_if_true(slist, xslist_test_apply, "value5"));

            /* xslist_map_break_if_false */
            xassert(xslist_map_break_if_false(slist, xslist_test_apply, "value3"));
            xassert_false(xslist_map_break_if_false(slist, xslist_test_apply_true, "value3"));

            xslist_free(&slist);
        }

        /* xslist_find */
        /* xslist_find_equal_if */
        /* xslist_find_apply_if */
        {
            XSList_PT slist = xslist_new();

            xslist_push_back_repeat(slist, "value1");
            xslist_push_back_repeat(slist, "value2");
            xslist_push_back_repeat(slist, "value3");
            xslist_push_back_repeat(slist, "value4");

            /* xslist_find */
            XRSList_PT node = xslist_find(slist, "value2");
            xassert(strcmp(node->value, "value2") == 0);

            /* xslist_find_equal_if */
            node = xslist_find_equal_if(slist, "value3", xslist_test_cmp, NULL);
            xassert(strcmp(node->value, "value3") == 0);

            /* xslist_find_apply_if */
            node = xslist_find_apply_if(slist, xslist_test_apply, "value4");
            xassert(strcmp(node->value, "value4") == 0);

            xslist_free(&slist);
        }

        /* xslist_to_array */
        {
            XSList_PT slist = xslist_new();

            xslist_push_back_repeat(slist, "value1");
            xslist_push_back_repeat(slist, "value2");
            xslist_push_back_repeat(slist, "value3");
            xslist_push_back_repeat(slist, "value4");

            XPArray_PT array = xslist_to_array(slist);

            xassert(array->size == 4);
            xassert(strcmp((char*)xparray_get(array, 0), "value1") == 0);
            xassert(strcmp((char*)xparray_get(array, 1), "value2") == 0);
            xassert(strcmp((char*)xparray_get(array, 2), "value3") == 0);
            xassert(strcmp((char*)xparray_get(array, 3), "value4") == 0);

            xparray_free(&array);
            xslist_free(&slist);
        }

        /* xslist_size */
        /* xslist_is_empty */

        /* xslist_unique */
        {
            XSList_PT slist = xslist_new();

            xslist_push_back_repeat(slist, "value2");
            xslist_push_back_repeat(slist, "value1");
            xslist_push_back_repeat(slist, "value1");
            xslist_push_back_repeat(slist, "value3");
            xslist_push_back_repeat(slist, "value2");
            xslist_push_back_repeat(slist, "value4");

            xassert(slist->size == 6);
            xslist_unique(slist, xslist_test_cmp, NULL);
            xassert(slist->size == 4);

            xassert(strcmp(slist->head->value, "value1") == 0);
            xassert(strcmp(slist->head->next->value, "value2") == 0);
            xassert(strcmp(slist->head->next->next->value, "value3") == 0);
            xassert(strcmp(slist->head->next->next->next->value, "value4") == 0);
            xassert_false(slist->head->next->next->next->next);
            xassert(strcmp(slist->tail->value, "value4") == 0);

            xslist_free(&slist);
        }

        /* xslist_deep_unique */
        {
            XSList_PT slist = xslist_random_string(1000);
            xslist_deep_unique(slist, xslist_test_cmp, NULL);
            xslist_deep_free(&slist);
        }

        /* xslist_sort */
        {
            {
                XSList_PT slist = xslist_new();

                xslist_push_back_repeat(slist, "value2");
                xslist_push_back_repeat(slist, "value1");
                xslist_push_back_repeat(slist, "value1");
                xslist_push_back_repeat(slist, "value3");
                xslist_push_back_repeat(slist, "value5");
                xslist_push_back_repeat(slist, "value4");

                xslist_sort(slist, xslist_test_cmp, NULL);

                xassert(strcmp(slist->head->value, "value1") == 0);
                xassert(strcmp(slist->head->next->value, "value1") == 0);
                xassert(strcmp(slist->head->next->next->value, "value2") == 0);
                xassert(strcmp(slist->head->next->next->next->value, "value3") == 0);
                xassert(strcmp(slist->head->next->next->next->next->value, "value4") == 0);
                xassert(strcmp(slist->head->next->next->next->next->next->value, "value5") == 0);
                xassert_false(slist->head->next->next->next->next->next->next);
                xassert(strcmp(slist->tail->value, "value5") == 0);
                xassert(slist->size == 6);

                xslist_free(&slist);
            }

            {
                XSList_PT slist = xslist_random_string(1000);
                xslist_sort(slist, xslist_test_cmp, NULL);
                //xslist_map(slist, xslist_test_print, NULL);
                xslist_deep_free(&slist);
            }
        }

        /* xslist_is_sorted */
        {
            /* tested by xslist_sort already */
        }

        {
            int count = 0;
            xmem_leak(xslist_test_check_mem_leak, &count);
            xassert(count == 0);
        }
}


