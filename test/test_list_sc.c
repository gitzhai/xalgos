
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../array_pointer/xarray_pointer_x.h"
#include "../list_s_raw/xlist_s_raw_x.h"
#include "../list_s/xlist_s_x.h"
#include "../list_sc/xlist_sc_x.h"
#include "../include/xalgos.h"

static
int xsclist_test_cmp_true(void *value1, void *value2, void *cl) {
    return 0;
}

static
int xsclist_test_cmp_false(void *value1, void *value2, void *cl) {
    return 1;
}

static 
int xsclist_test_cmp(void *value1, void *value2, void *cl) {
    return value1 == value2 ? 0 : 1;
}

static
int test_cmp(void *value1, void *value2, void *cl) {
    return strcmp((char*)value1, (char*)value2);
}

static
bool xsclist_test_apply(void *value, void *cl) {
    return strcmp((char*)value, (char*)cl) == 0;
}

static
bool xsclist_test_apply_true(void *value, void *cl) {
    return true;
}

static
bool xsclist_test_apply_false(void *value, void *cl) {
    return false;
}

static
bool xsclist_test_map_apply_count(void *value, void *cl) {
    ++(*((int*)(cl)));
    return true;
}

static
bool xsclist_test_print(void *value, void *cl) {
    printf("%s\n", (char*)value);
    return true;
}

static 
XSCList_PT xsclist_random_string(int size) {
    XSCList_PT slist = xsclist_new();
    if (!slist) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xsclist_deep_free(&slist);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }
            str[str_size - 1] = '\0';

            xsclist_push_back_repeat(slist, str);
        }
    }

    return slist;
}

static
void xsclist_test_check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}


void test_xsclist() {

        /* xsclist_new */
        {
            XSCList_PT slist = xsclist_new();
            xassert(slist);
            xassert(slist->head == NULL);
            xassert(slist->tail == NULL);
            xassert(slist->size == 0);
            xsclist_free(&slist);
        }

        /* xsclist_vload_repeat */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xsclist_vload_repeat(NULL, "value", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XSCList_PT slist = xsclist_new();
                xassert(xsclist_vload_repeat(slist, NULL) == 0);
                xsclist_free(&slist);
            }

            /* normal */
            {
                XSCList_PT slist = xsclist_new();
                xassert(xsclist_vload_repeat(slist, "k1", "k2", "k1", "k3", NULL) == 4);
                xassert(strcmp(slist->head->value, "k1") == 0);
                xsclist_free(&slist);
            }
        }

        /* xsclist_vload_unique */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xsclist_vload_unique(NULL, "value", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XSCList_PT slist = xsclist_new();
                xassert(xsclist_vload_unique(slist, NULL) == 0);
                xsclist_free(&slist);
            }

            /* repeat value */
            {
                XSCList_PT slist = xsclist_new();
                xassert(xsclist_vload_unique(slist, "k1", "k2", "k1", "k3", NULL) == 3);
                xassert(strcmp(slist->head->value, "k1") == 0);
                xsclist_free(&slist);
            }
        }

        /* xsclist_vload_unique_if */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xsclist_vload_unique_if(NULL, xsclist_test_cmp, NULL, "k", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }
            
            /* value == NULL */
            {
                XSCList_PT slist = xsclist_new();
                xassert(xsclist_vload_unique_if(slist, xsclist_test_cmp, NULL, NULL) == 0);
                xsclist_free(&slist);
            }

            /* repeat value */
            {
                XSCList_PT slist = xsclist_new();
                xassert(xsclist_vload_unique_if(slist, xsclist_test_cmp, NULL, "k1", "k2", "k1", "k3", NULL) == 3);
                xassert(strcmp(slist->head->value, "k1") == 0);
                xsclist_free(&slist);
            }
        }

        /* xsclist_vload_replace_if */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xsclist_vload_replace_if(NULL, xsclist_test_cmp, NULL, "k", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XSCList_PT slist = xsclist_new();
                xassert(xsclist_vload_replace_if(slist, xsclist_test_cmp, NULL, NULL) == 0);
                xsclist_free(&slist);
            }

            /* repeat value */
            {
                XSCList_PT slist = xsclist_new();
                xassert(xsclist_vload_replace_if(slist, xsclist_test_cmp, NULL, "k1", "k2", "k1", "k3", NULL) == 3);
                xassert(strcmp(slist->head->value, "k1") == 0);
                xsclist_free(&slist);
            }
        }

        /* xsclist_aload_repeat */
        {
            /* slist == NULL */
            {
                XPArray_PT array = xparray_new(10);
                xparray_put(array, 0, "k", NULL);
                bool except = false;
                XEXCEPT_TRY
                    xsclist_aload_repeat(NULL, array);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xparray_free(&array);
            }

            /* xs == NULL */
            {
                XSCList_PT slist = xsclist_new();
                bool except = false;
                XEXCEPT_TRY
                    xsclist_aload_repeat(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xsclist_free(&slist);
            }

            /* value == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XSCList_PT slist = xsclist_new();

                xassert(xsclist_aload_repeat(slist, array) == 0);

                xsclist_free(&slist);
                xparray_free(&array);
            }

            /* normal */
            {
                XPArray_PT array = xparray_new(10);
                XSCList_PT slist = xsclist_new();

                xparray_vload(array, "k1", "k2", "k1", "k3", NULL);
                xassert(xsclist_aload_repeat(slist, array) == 4);
                xassert(strcmp(slist->head->value, "k1") == 0);

                xsclist_free(&slist);
                xparray_free(&array);
            }
        }

        /* xsclist_aload_unique */
        {
            /* slist == NULL */
            {
                XPArray_PT array = xparray_new(10);
                xparray_put(array, 0, "k", NULL);

                bool except = false;
                XEXCEPT_TRY
                    xsclist_aload_unique(NULL, array);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xparray_free(&array);
            }

            /* xs == NULL */
            {
                XSCList_PT slist = xsclist_new();
                bool except = false;
                XEXCEPT_TRY
                    xsclist_aload_unique(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xsclist_free(&slist);
            }

            /* value == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XSCList_PT slist = xsclist_new();

                xassert(xsclist_aload_unique(slist, array) == 0);

                xsclist_free(&slist);
                xparray_free(&array);
            }

            /* repeat value */
            {
                XPArray_PT array = xparray_new(10);
                XSCList_PT slist = xsclist_new();

                xparray_vload(array, "k1", "k2", "k1", "k3", NULL);
                xassert(xsclist_aload_unique(slist, array) == 3);
                xassert(strcmp(slist->head->value, "k1") == 0);

                xsclist_free(&slist);
                xparray_free(&array);
            }
        }

        /* xsclist_aload_unique_if */
        {
            /* cmp == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XSCList_PT slist = xsclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xsclist_aload_unique_if(slist, array, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xsclist_free(&slist);
                xparray_free(&array);
            }

            /* below 3 scenarios tested already in xsclist_aload_unique */
            /* xs == NULL */
            /* value == NULL */
            /* value == NULL */

            /* repeat value */
            {
                XPArray_PT array = xparray_new(10);
                XSCList_PT slist = xsclist_new();

                xparray_vload(array, "k1", "k2", "k1", "k3", NULL);
                xassert(xsclist_aload_unique_if(slist, array, xsclist_test_cmp, NULL) == 3);
                xassert(strcmp(slist->head->value, "k1") == 0);

                xsclist_free(&slist);
                xparray_free(&array);
            }
        }

        /* xsclist_aload_replace_if */
        {
            /* cmp == NULL */
            {
                XPArray_PT array = xparray_new(10);
                XSCList_PT slist = xsclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xsclist_aload_replace_if(slist, array, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);

                xsclist_free(&slist);
                xparray_free(&array);
            }

            /* below 3 scenarios tested already in xsclist_aload_replace */
            /* xs == NULL */
            /* value == NULL */
            /* value == NULL */

            /* repeat value */
            {
                XPArray_PT array = xparray_new(10);
                XSCList_PT slist = xsclist_new();

                xparray_vload(array, "k1", "k2", "k1", "k3", NULL);
                xassert(xsclist_aload_replace_if(slist, array, xsclist_test_cmp, NULL) == 3);
                xassert(strcmp(slist->head->value, "k1") == 0);

                xsclist_free(&slist);
                xparray_free(&array);
            }
        }

        /* xsclist_copyn_if_impl */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xsclist_copyn_if_impl(NULL, 0, 1, false, xsclist_test_cmp, NULL, NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* count < 0 */
            {
                XSCList_PT slist = xsclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xsclist_copyn_if_impl(slist, 0, -1, false, xsclist_test_cmp, NULL, xsclist_test_apply_true, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xsclist_free(&slist);
            }

            /* deep == true && value_size < 0 */
            {
                XSCList_PT slist = xsclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xsclist_copyn_if_impl(slist, -1, 1, true, xsclist_test_cmp, NULL, xsclist_test_apply_true, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xsclist_free(&slist);
            }

            /* deep == true && value_size == 0 */
            {
                XSCList_PT slist = xsclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xsclist_copyn_if_impl(slist, 0, 1, true, xsclist_test_cmp, NULL, xsclist_test_apply_true, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xsclist_free(&slist);
            }

            /* slist is empty */
            {
                XSCList_PT slist = xsclist_new();

                {
                    XSCList_PT nslist = xsclist_copyn_if_impl(slist, 0, 1, false, NULL, NULL, xsclist_test_apply_true, NULL);
                    xassert(nslist->size == 0);
                    xsclist_free(&nslist);
                }

                xsclist_free(&slist);
            }

            /* count == 0 */
            {
                XSCList_PT slist = xsclist_new();
                xsclist_vload_repeat(slist, "k1", "k2", NULL);

                {
                    XSCList_PT nslist = xsclist_copyn_if_impl(slist, 0, 0, false, NULL, NULL, xsclist_test_apply_true, NULL);
                    xassert(nslist->size == 0);
                    xsclist_free(&nslist);
                }

                xsclist_free(&slist);
            }

            /* slist->size < count */
            {
                XSCList_PT slist = xsclist_new();
                xsclist_vload_repeat(slist, "k1", "k1", NULL);

                {
                    XSCList_PT nslist = xsclist_copyn_if_impl(slist, 0, 3, false, NULL, NULL, xsclist_test_apply_true, NULL);
                    xassert(nslist->size == 2);
                    xsclist_free(&nslist);
                }

                xsclist_free(&slist);
            }

            /* count < slist->size */
            {
                XSCList_PT slist = xsclist_new();
                xsclist_vload_repeat(slist, "k1", "k1", NULL);

                {
                    XSCList_PT nslist = xsclist_copyn_if_impl(slist, 0, 1, false, NULL, NULL, xsclist_test_apply_true, NULL);
                    xassert(nslist->size == 1);
                    xassert(strcmp(nslist->head->value, "k1") == 0);
                    xsclist_free(&nslist);
                }

                xsclist_free(&slist);
            }

            /* no any value match */
            {
                XSCList_PT slist = xsclist_new();
                xsclist_vload_repeat(slist, "k1", "k2", NULL);

                {
                    XSCList_PT nslist = xsclist_copyn_if_impl(slist, 0, 1, false, NULL, NULL, xsclist_test_apply_false, NULL);
                    xassert(nslist->size == 0);
                    xsclist_free(&nslist);
                }

                xsclist_free(&slist);
            }

            /* normal */
            {
                XSCList_PT slist = xsclist_new();
                xsclist_vload_repeat(slist, "k1", "k1", "k3", NULL);

                {
                    XSCList_PT nslist = xsclist_copyn_if_impl(slist, 0, 2, false, NULL, NULL, xsclist_test_apply_true, NULL);
                    xassert(nslist->size == 2);
                    xassert(strcmp(nslist->head->value, "k1") == 0);
                    xassert(strcmp(nslist->head->next->value, "k1") == 0);
                    xsclist_free(&nslist);
                }

                {
                    XSCList_PT nslist = xsclist_copyn_if_impl(slist, 0, 2, false, NULL, NULL, xsclist_test_apply, "k3");
                    xassert(nslist->size == 1);
                    xassert(strcmp(nslist->head->value, "k3") == 0);
                    xsclist_free(&nslist);
                }

                {
                    XSCList_PT nslist = xsclist_copyn_if_impl(slist, 0, 2, false, test_cmp, NULL, NULL, "k3");
                    xassert(nslist->size == 1);
                    xassert(strcmp(nslist->head->value, "k3") == 0);
                    xsclist_free(&nslist);
                }

                xsclist_free(&slist);
            }
        }

        /* xsclist_copyn_equal_if */
        {
            /* value == NULL */
            {
                XSCList_PT slist = xsclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xsclist_copyn_equal_if(slist, 1, NULL, xsclist_test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xsclist_free(&slist);
            }
        }

        /* xsclist_deep_copyn_equal_if */
        {
            /* value == NULL */
            {
                XSCList_PT slist = xsclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xsclist_deep_copyn_equal_if(slist, 1, 1, NULL, xsclist_test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);

                xsclist_free(&slist);
            }
        }
        
        /* xsclist_copy */
        /* xsclist_copy_equal_if */
        /* xsclist_copy_apply_if */
        /* xsclist_copy_apply_value_if */
        /* xsclist_copyn */
        /* xsclist_copyn_equal_if */
        /* xsclist_copyn_apply_if */
        /* xsclist_copyn_apply_value_if */
        /* xsclist_deep_copy */
        /* xsclist_deep_copy_equal_if */
        /* xsclist_deep_copy_apply_if */
        /* xsclist_deep_copy_apply_value_if */
        /* xsclist_deep_copyn */
        /* xsclist_deep_copyn_equal_if */
        /* xsclist_deep_copyn_apply_if */
        /* xsclist_deep_copyn_apply_value_if */
        {
            /* tested by xsclist_copyn_if_impl already */
        }

        /* xsclist_push_front_repeat */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xsclist_push_front_repeat(NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XSCList_PT slist = xsclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xsclist_push_front_repeat(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xsclist_free(&slist);
            }

            /* normal */
            {
                XSCList_PT slist = xsclist_new();

                char *str = "value1";

                xsclist_push_front_repeat(slist, str);
                xsclist_push_front_repeat(slist, "value2");
                xsclist_push_front_repeat(slist, str);
                xsclist_push_front_repeat(slist, "value3");

                xassert(strcmp(slist->head->value, "value3") == 0);
                xassert(strcmp(slist->head->next->value, "value1") == 0);
                xassert(strcmp(slist->head->next->next->value, "value2") == 0);
                xassert(strcmp(slist->head->next->next->next->value, "value1") == 0);
                xassert(slist->head->next->next->next->next == slist->head);
                xassert(strcmp(slist->tail->value, "value1") == 0);

                xassert(slist->size == 4);

                xsclist_free(&slist);
            }
        }

        /* xsclist_pop_front */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xsclist_pop_front(NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* slist is empty */
            {
                XSCList_PT slist = xsclist_new();
                xassert(xsclist_pop_front(slist) == false);
                xsclist_free(&slist);
            }

            {
                XSCList_PT slist = xsclist_new();

                xsclist_push_back_repeat(slist, "value1");
                xsclist_push_back_repeat(slist, "value2");
                xsclist_push_back_repeat(slist, "value3");

                {
                    void *value = NULL;

                    xassert(value = xsclist_pop_front(slist));
                    xassert(strcmp(value, "value1") == 0);

                    xassert(value = xsclist_pop_front(slist));
                    xassert(strcmp(value, "value2") == 0);

                    xassert(xsclist_pop_front(slist));

                    xassert(slist->size == 0);
                    xassert(slist->head == NULL);
                    xassert(slist->tail == NULL);
                }

                xsclist_free(&slist);
            }
        }

        /* xsclist_front */
        {
            /* slist is empty */
            {
                XSCList_PT slist = xsclist_new();
                xassert_false(xsclist_front(slist));
                xsclist_free(&slist);
            }

            /* normal */
            {
                XSCList_PT slist = xsclist_new();

                xsclist_push_back_repeat(slist, "value1");
                xsclist_push_back_repeat(slist, "value2");
                xsclist_push_back_repeat(slist, "value3");

                xassert(strcmp(xsclist_front(slist), "value1") == 0);

                xsclist_free(&slist);
            }
        }

        /* xsclist_back */
        {
            /* slist is empty */
            {
                XSCList_PT slist = xsclist_new();
                xassert_false(xsclist_back(slist));
                xsclist_free(&slist);
            }

            /* normal */
            {
                XSCList_PT slist = xsclist_new();

                xsclist_push_back_repeat(slist, "value1");
                xsclist_push_back_repeat(slist, "value2");
                xsclist_push_back_repeat(slist, "value3");

                xassert(strcmp(xsclist_back(slist), "value3") == 0);

                xsclist_free(&slist);
            }
        }

        /* xsclist_front_node */
        {
            /* slist is empty */
            {
                XSCList_PT slist = xsclist_new();
                xassert_false(xsclist_front_node(slist));
                xsclist_free(&slist);
            }

            /* normal */
            {
                XSCList_PT slist = xsclist_new();

                xsclist_push_back_repeat(slist, "value1");
                xsclist_push_back_repeat(slist, "value2");
                xsclist_push_back_repeat(slist, "value3");

                xassert(strcmp(xsclist_front_node(slist)->value, "value1") == 0);

                xsclist_free(&slist);
            }
        }

        /* xsclist_back_node */
        {
            /* slist is empty */
            {
                XSCList_PT slist = xsclist_new();
                xassert_false(xsclist_back_node(slist));
                xsclist_free(&slist);
            }

            /* normal */
            {
                XSCList_PT slist = xsclist_new();

                xsclist_push_back_repeat(slist, "value1");
                xsclist_push_back_repeat(slist, "value2");
                xsclist_push_back_repeat(slist, "value3");

                xassert(strcmp(xsclist_back_node(slist)->value, "value3") == 0);

                xsclist_free(&slist);
            }
        }

        /* xsclist_push_back_repeat */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xsclist_push_back_repeat(NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XSCList_PT slist = xsclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xsclist_push_back_repeat(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xsclist_free(&slist);
            }

            /* normal */
            {
                XSCList_PT slist = xsclist_new();

                char *str = "value1";

                xsclist_push_back_repeat(slist, str);
                xsclist_push_back_repeat(slist, "value2");
                xsclist_push_back_repeat(slist, str);
                xsclist_push_back_repeat(slist, "value3");

                xassert(strcmp(slist->head->value, "value1") == 0);
                xassert(strcmp(slist->head->next->value, "value2") == 0);
                xassert(strcmp(slist->head->next->next->value, "value1") == 0);
                xassert(strcmp(slist->head->next->next->next->value, "value3") == 0);
                xassert(slist->head->next->next->next->next == slist->head);
                xassert(strcmp(slist->tail->value, "value3") == 0);

                xassert(slist->size == 4);

                xsclist_free(&slist);
            }
        }

        /* xsclist_push_front_unique */
        {
            XSCList_PT slist = xsclist_new();

            char *str = "value1"; 

            xsclist_push_front_unique(slist, str);
            xsclist_push_front_unique(slist, "value2");
            xsclist_push_front_unique(slist, str);
            xsclist_push_front_unique(slist, "value3");

            xassert(strcmp(slist->head->value, "value3") == 0);
            xassert(strcmp(slist->head->next->value, "value2") == 0);
            xassert(strcmp(slist->head->next->next->value, "value1") == 0);
            xassert(slist->head->next->next->next == slist->head);
            xassert(strcmp(slist->tail->value, "value1") == 0);
            xassert(slist->size == 3);

            xsclist_free(&slist);
        }

        /* xsclist_push_front_unique_if */
        {
            XSCList_PT slist = xsclist_new();

            xsclist_push_front_unique_if(slist, "value1", test_cmp, NULL);
            xsclist_push_front_unique_if(slist, "value2", test_cmp, NULL);
            xsclist_push_front_unique_if(slist, "value1", test_cmp, NULL);
            xsclist_push_front_unique_if(slist, "value3", test_cmp, NULL);

            xassert(strcmp(slist->head->value, "value3") == 0);
            xassert(strcmp(slist->head->next->value, "value2") == 0);
            xassert(strcmp(slist->head->next->next->value, "value1") == 0);
            xassert(strcmp(slist->tail->value, "value1") == 0);
            xassert(slist->head->next->next->next==slist->head);
            xassert(slist->size == 3);

            xsclist_free(&slist);
        }

        /* xsclist_push_front_replace_if */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xsclist_push_front_replace_if(NULL, "k", test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
            }

            /* value == NULL */
            {
                XSCList_PT slist = xsclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xsclist_push_front_replace_if(slist, NULL, test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xsclist_free(&slist);
            }
            
            /* repeat value */
            {
                XSCList_PT slist = xsclist_new();

                xsclist_push_front_replace_if(slist, "value1", test_cmp, NULL);
                xsclist_push_front_replace_if(slist, "value2", test_cmp, NULL);
                xsclist_push_front_replace_if(slist, "value1", test_cmp, NULL);
                xsclist_push_front_replace_if(slist, "value3", test_cmp, NULL);

                xassert(strcmp(slist->head->value, "value3") == 0);
                xassert(strcmp(slist->head->next->value, "value2") == 0);
                xassert(strcmp(slist->head->next->next->value, "value1") == 0);
                xassert(strcmp(slist->tail->value, "value1") == 0);
                xassert(slist->head->next->next->next == slist->head);
                xassert(slist->size == 3);

                xsclist_free(&slist);
            }
        }

        /* xsclist_push_front_deep_replace_if */
        {
            /* below scenarios tested by xsclist_push_front_replace_if already */
            /* slist == NULL */
            /* value == NULL */
            /* cmp == NULL */

            /* repeat value */
            {
                XSCList_PT slist = xsclist_new();

                char *value = XMEM_CALLOC(1, 7);
                memcpy(value, "value1", 6);

                xsclist_push_front_deep_replace_if(slist, value, test_cmp, NULL);
                xsclist_push_front_deep_replace_if(slist, "value2", test_cmp, NULL);
                xsclist_push_front_deep_replace_if(slist, "value1", test_cmp, NULL);
                xsclist_push_front_deep_replace_if(slist, "value3", test_cmp, NULL);

                xassert(strcmp(slist->head->value, "value3") == 0);
                xassert(strcmp(slist->head->next->value, "value2") == 0);
                xassert(strcmp(slist->head->next->next->value, "value1") == 0);
                xassert(strcmp(slist->tail->value, "value1") == 0);
                xassert(slist->head->next->next->next == slist->head);
                xassert(slist->size == 3);

                xsclist_free(&slist);
            }
        }

        /* xsclist_push_back_unique */
        {
            XSCList_PT slist = xsclist_new();

            char *str = "value1";

            xsclist_push_back_unique(slist, str);
            xsclist_push_back_unique(slist, "value2");
            xsclist_push_back_unique(slist, str);
            xsclist_push_back_unique(slist, "value3");

            xassert(strcmp(slist->head->value, "value1") == 0);
            xassert(strcmp(slist->head->next->value, "value2") == 0);
            xassert(strcmp(slist->head->next->next->value, "value3") == 0);
            xassert(strcmp(slist->tail->value, "value3") == 0);
            xassert(slist->head->next->next->next == slist->head);
            xassert(slist->size == 3);

            xsclist_free(&slist);
        }

        /* xsclist_push_back_unique_if */
        {
            XSCList_PT slist = xsclist_new();

            xsclist_push_back_unique_if(slist, "value1", test_cmp, NULL);
            xsclist_push_back_unique_if(slist, "value2", test_cmp, NULL);
            xsclist_push_back_unique_if(slist, "value1", test_cmp, NULL);
            xsclist_push_back_unique_if(slist, "value3", test_cmp, NULL);

            xassert(strcmp(slist->head->value, "value1") == 0);
            xassert(strcmp(slist->head->next->value, "value2") == 0);
            xassert(strcmp(slist->head->next->next->value, "value3") == 0);
            xassert(strcmp(slist->tail->value, "value3") == 0);
            xassert(slist->head->next->next->next == slist->head);
            xassert(slist->size == 3);

            xsclist_free(&slist);
        }

        /* xsclist_push_back_replace_if */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xsclist_push_back_replace_if(NULL, "k", test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XSCList_PT slist = xsclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xsclist_push_back_replace_if(slist, NULL, test_cmp, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xsclist_free(&slist);
            }

            /* repeat value */
            {
                XSCList_PT slist = xsclist_new();

                xsclist_push_back_replace_if(slist, "value1", test_cmp, NULL);
                xsclist_push_back_replace_if(slist, "value2", test_cmp, NULL);
                xsclist_push_back_replace_if(slist, "value1", test_cmp, NULL);
                xsclist_push_back_replace_if(slist, "value3", test_cmp, NULL);

                xassert(strcmp(slist->head->value, "value1") == 0);
                xassert(strcmp(slist->head->next->value, "value2") == 0);
                xassert(strcmp(slist->head->next->next->value, "value3") == 0);
                xassert(strcmp(slist->tail->value, "value3") == 0);
                xassert(slist->head->next->next->next == slist->head);
                xassert(slist->size == 3);

                xsclist_free(&slist);
            }
        }

        /* xsclist_push_back_deep_replace_if */
        {
            /* below scenarios tested by xsclist_push_back_replace_if already */
            /* slist == NULL */
            /* value == NULL */
            /* cmp == NULL */

            /* repeat value */
            {
                XSCList_PT slist = xsclist_new();

                char *value = XMEM_CALLOC(1, 7);
                memcpy(value, "value1", 6);

                xsclist_push_back_deep_replace_if(slist, value, test_cmp, NULL);
                xsclist_push_back_deep_replace_if(slist, "value2", test_cmp, NULL);
                xsclist_push_back_deep_replace_if(slist, "value1", test_cmp, NULL);
                xsclist_push_back_deep_replace_if(slist, "value3", test_cmp, NULL);

                xassert(strcmp(slist->head->value, "value1") == 0);
                xassert(strcmp(slist->head->next->value, "value2") == 0);
                xassert(strcmp(slist->head->next->next->value, "value3") == 0);
                xassert(strcmp(slist->tail->value, "value3") == 0);
                xassert(slist->head->next->next->next == slist->head);
                xassert(slist->size == 3);

                xsclist_free(&slist);
            }
        }

        /* xsclist_insert_after */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xsclist_insert_after(NULL, NULL, "k");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* value == NULL */
            {
                XSCList_PT slist = xsclist_new();

                bool except = false;
                XEXCEPT_TRY
                    xsclist_insert_after(slist, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);

                xsclist_free(&slist);
            }

            {
                XSCList_PT slist = xsclist_new();

                xsclist_push_back_repeat(slist, "value1");
                xsclist_push_back_repeat(slist, "value2");
                xsclist_push_back_repeat(slist, "value3");

                {
                    xsclist_insert_after(slist, NULL, "value2");
                    /*
                     *  value1
                     *  value2
                     *  value3
                     *  value2
                     */
                    xassert(strcmp(slist->head->next->next->next->value, "value2") == 0);
                    xassert(slist->head->next->next->next->next == slist->head);
                    xassert(strcmp(slist->tail->value, "value2") == 0);
                }

                {
                    XRSList_PT node = xsclist_front_node(slist)->next->next;
                    xsclist_insert_after(slist, node, "value4");
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

                xsclist_free(&slist);
            }
        }

        /* xsclist_remove_after */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xsclist_remove_after(NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* slist is empty */
            {
                XSCList_PT slist = xsclist_new();
                xassert_false(xsclist_remove_after(slist, slist->head));
                xsclist_free(&slist);
            }

            /* node is NULL */
            {
                XSCList_PT slist = xsclist_new();
                xassert_false(xsclist_remove_after(slist, slist->head));
                xsclist_free(&slist);
            }

            /* node->next is NULl */
            {
                XSCList_PT slist = xsclist_new();
                xsclist_vload_repeat(slist, "k", NULL);

                xassert_false(xsclist_remove_after(slist, slist->tail));
                xsclist_free(&slist);
            }

            {
                XSCList_PT slist = xsclist_new();

                void *value = NULL;

                xsclist_push_back_repeat(slist, "value1");
                xsclist_push_back_repeat(slist, "value2");
                xsclist_push_back_repeat(slist, "value3");
                xsclist_push_back_repeat(slist, "value4");
                xsclist_push_back_repeat(slist, "value5");

                {
                    XRSList_PT node = xsclist_front_node(slist)->next->next;
                    value = xsclist_remove_after(slist, node);
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
                    XRSList_PT node = xsclist_front_node(slist)->next->next;
                    value = xsclist_remove_after(slist, node);
                    /*
                    *  value1, value1
                    *  value2, value2
                    *  value3, value3
                    */
                    xassert(strcmp(value, "value5") == 0);
                    xassert(strcmp(slist->head->next->next->value, "value3") == 0);
                    xassert(slist->head->next->next->next == slist->head);
                    xassert(slist->size == 3);
                }

                xassert(strcmp(slist->head->value, "value1") == 0);
                xassert(strcmp(slist->tail->value, "value3") == 0);

                xsclist_free(&slist);
            }
        }

        /* xsclist_remove_if_impl */
        {
            /* slist == NULL */
            {
                bool except = false;
                XEXCEPT_TRY
                    xsclist_remove_if_impl(NULL, false, false, NULL, NULL, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
            }

            /* slist is empty */
            {
                XSCList_PT slist = xsclist_new();
                xassert(xsclist_remove_if_impl(slist, false, false, NULL, NULL, xsclist_test_apply_false, NULL) == 0);
                xsclist_free(&slist);
            }

            /* no any value match */
            {
                XSCList_PT slist = xsclist_new();
                xsclist_push_back_repeat(slist, "value1");
                xsclist_push_back_repeat(slist, "value2");
                xsclist_push_back_repeat(slist, "value1");

                xassert(xsclist_remove_if_impl(slist, false, false, xsclist_test_cmp_false, NULL, NULL, NULL) == 0);
                xassert(xsclist_remove_if_impl(slist, false, false, NULL, NULL, xsclist_test_apply_false, NULL) == 0);

                xassert(xsclist_remove_if_impl(slist, false, false, test_cmp, NULL, NULL, "value3") == 0);
                xassert(xsclist_remove_if_impl(slist, false, false, NULL, NULL, xsclist_test_apply, "value3") == 0);

                xsclist_free(&slist);
            }

            /* remove one */
            {
                XSCList_PT slist = xsclist_new();
                xsclist_push_back_repeat(slist, "value1");
                xsclist_push_back_repeat(slist, "value2");
                xsclist_push_back_repeat(slist, "value3");
                xsclist_push_back_repeat(slist, "value4");

                xassert(xsclist_remove_if_impl(slist, false, true, test_cmp, NULL, NULL, "value1") == 1);
                xassert(xsclist_remove_if_impl(slist, false, true, NULL, NULL, xsclist_test_apply, "value2") == 1);
                xassert(slist->size == 2);
                xassert(strcmp(slist->head->value, "value3") == 0);
                xassert(strcmp(slist->tail->value, "value4") == 0);
                xassert(slist->head->next->next == slist->head);

                xsclist_free(&slist);
            }

            /* remove all */
            {
                XSCList_PT slist = xsclist_new();
                xsclist_push_back_repeat(slist, "value1");
                xsclist_push_back_repeat(slist, "value2");
                xsclist_push_back_repeat(slist, "value1");
                xsclist_push_back_repeat(slist, "value1");
                xsclist_push_back_repeat(slist, "value2");
                xsclist_push_back_repeat(slist, "value1");

                xassert(xsclist_remove_if_impl(slist, false, false, test_cmp, NULL, NULL, "value1") == 4);
                xassert(xsclist_find(slist, "value1") == NULL);
                xassert(xsclist_remove_if_impl(slist, false, false, NULL, NULL, xsclist_test_apply, "value2") == 2);
                xassert(xsclist_find(slist, "value2") == NULL);
                xassert(slist->size == 0);

                xsclist_free(&slist);
            }

            /* deep remove */
            {
                XSCList_PT slist = xsclist_new();
                char *value1 = XMEM_CALLOC(1, 10);
                char *value2 = XMEM_CALLOC(1, 10);
                char *value3 = XMEM_CALLOC(1, 10);

                memcpy(value1, "value1", 6);
                memcpy(value2, "value2", 6);
                memcpy(value3, "value1", 6);

                xsclist_push_back_repeat(slist, value1);
                xsclist_push_back_repeat(slist, value2);
                xsclist_push_back_repeat(slist, value3);

                xassert(xsclist_remove_if_impl(slist, true, false, test_cmp, NULL, NULL, "value1") == 2);
                xassert(slist->size == 1);
                xassert(strcmp(slist->head->value, "value2") == 0);
                xassert(strcmp(slist->tail->value, "value2") == 0);
                xassert(slist->head->next == slist->head);

                xsclist_deep_free(&slist);
            }
        }

        /* all below remove functions tested by xsclist_remove_if_impl already */

        /* xsclist_remove */
        {
            /* value == NULL */
            {
                XSCList_PT slist = xsclist_new();
                bool except = false;
                XEXCEPT_TRY
                    xsclist_remove(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xsclist_free(&slist);
            }
        }

        /* xsclist_remove_all */
        {
            /* value == NULL */
            {
                XSCList_PT slist = xsclist_new();
                bool except = false;
                XEXCEPT_TRY
                    xsclist_remove_all(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xsclist_free(&slist);
            }
        }

        /* xsclist_deep_remove */
        {
            /* value == NULL */
            {
                XSCList_PT slist = xsclist_new();
                bool except = false;
                XEXCEPT_TRY
                    xsclist_deep_remove(slist, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                xassert(except);
                xsclist_free(&slist);
            }

            /* *value == NULL */
            {
                XSCList_PT slist = xsclist_new();
                void *str = NULL;
                void **value = &str;

                bool except = false;
                XEXCEPT_TRY
                    xsclist_deep_remove(slist, value);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xsclist_free(&slist);
            }
        }

        /* xsclist_remove_apply_if */
        /* xsclist_remove_apply_value_if */
        /* xsclist_remove_equal_if */
        /* xsclist_remove_apply_break_if */
        /* xsclist_remove_apply_value_break_if */
        /* xsclist_remove_equal_break_if */
        /* xsclist_deep_remove_apply_if */
        /* xsclist_deep_remove_apply_value_if */
        /* xsclist_deep_remove_equal_if */
        /* xsclist_deep_remove_apply_break_if */
        /* xsclist_deep_remove_apply_value_break_if */
        /* xsclist_deep_remove_equal_break_if */

        /* below two free functions tested by other functions already */
        /* xsclist_free */
        /* xsclist_deep_free */


        /* xsclist_clear */
        /* xsclist_deep_clear */
        {
            /* tested by xsclist_free already */
        }

        /* xsclist_swap */
        {
            /* slist1 == NULL */
            {
                XSCList_PT slist2 = xsclist_new();
                bool except = false;
                XEXCEPT_TRY
                    xsclist_swap(NULL, slist2);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xsclist_free(&slist2);
            }

            /* slist2 == NULL */
            {
                XSCList_PT slist1 = xsclist_new();
                bool except = false;
                XEXCEPT_TRY
                    xsclist_swap(slist1, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY
                    xassert(except);
                xsclist_free(&slist1);
            }

            /* normal */
            {
                XSCList_PT slist1 = xsclist_new();
                XSCList_PT slist2 = xsclist_new();

                xsclist_push_back_repeat(slist1, "value1");
                xsclist_push_back_repeat(slist1, "value2");
                xsclist_push_back_repeat(slist1, "value3");
                xsclist_push_back_repeat(slist1, "value4");

                xsclist_push_back_repeat(slist2, "value5");
                xsclist_push_back_repeat(slist2, "value6");
                xsclist_push_back_repeat(slist2, "value7");
                xsclist_push_back_repeat(slist2, "value8");
                xsclist_push_back_repeat(slist2, "value9");

                xassert(xsclist_swap(slist1, slist2));

                xassert(strcmp(slist1->head->value, "value5") == 0);
                xassert(strcmp(slist1->tail->value, "value9") == 0);

                xassert(strcmp(slist2->head->value, "value1") == 0);
                xassert(strcmp(slist2->tail->value, "value4") == 0);

                xassert(slist1->size == 5);
                xassert(slist2->size == 4);

                xsclist_free(&slist1);
                xsclist_free(&slist2);
            }
        }

        /* xsclist_merge */
        {
            {
                XSCList_PT slist1 = xsclist_new();
                XSCList_PT slist2 = xsclist_new();

                xsclist_push_back_repeat(slist1, "value1");
                xsclist_push_back_repeat(slist1, "value2");
                xsclist_push_back_repeat(slist2, "value1");
                xsclist_push_back_repeat(slist2, "value4");

                xassert(xsclist_merge(slist1, &slist2));

                xassert_false(slist2);

                xassert(strcmp(slist1->head->value, "value1") == 0);
                xassert(strcmp(slist1->tail->value, "value4") == 0);

                xassert(strcmp(slist1->head->next->next->value, "value1") == 0);

                xassert(slist1->size == 4);

                xsclist_free(&slist1);
            }
        }

        /* xsclist_reverse */
        {
            XSCList_PT slist = xsclist_new();

            xsclist_reverse(slist);

            xsclist_push_back_repeat(slist, "value1");
            xsclist_push_back_repeat(slist, "value2");
            xsclist_push_back_repeat(slist, "value3");
            xsclist_push_back_repeat(slist, "value4");

            xsclist_reverse(slist);

            xassert(strcmp(slist->head->value, "value4") == 0);
            xassert(strcmp(slist->head->next->value, "value3") == 0);
            xassert(strcmp(slist->head->next->next->value, "value2") == 0);
            xassert(strcmp(slist->tail->value, "value1") == 0);

            xassert(slist->size == 4);

            xsclist_free(&slist);
        }

        /* xsclist_map */
        /* xsclist_map_break_if_true */
        /* xsclist_map_break_if_false */
        {
            XSCList_PT slist = xsclist_new();

            xsclist_push_back_repeat(slist, "value1");
            xsclist_push_back_repeat(slist, "value2");
            xsclist_push_back_repeat(slist, "value3");
            xsclist_push_back_repeat(slist, "value4");

            /* xsclist_map */
            int count = 0;
            xassert(xsclist_map(slist, xsclist_test_map_apply_count, (void*)&count) == 4);
            xassert(count == 4);

            /* xsclist_map_break_if_true */
            xassert(xsclist_map_break_if_true(slist, xsclist_test_apply, "value3"));
            xassert_false(xsclist_map_break_if_true(slist, xsclist_test_apply, "value5"));

            /* xsclist_map_break_if_false */
            xassert(xsclist_map_break_if_false(slist, xsclist_test_apply, "value3"));
            xassert_false(xsclist_map_break_if_false(slist, xsclist_test_apply_true, "value3"));

            xsclist_free(&slist);
        }

        /* xsclist_find */
        /* xsclist_find_equal_if */
        /* xsclist_find_apply_if */
        {
            XSCList_PT slist = xsclist_new();

            xsclist_push_back_repeat(slist, "value1");
            xsclist_push_back_repeat(slist, "value2");
            xsclist_push_back_repeat(slist, "value3");
            xsclist_push_back_repeat(slist, "value4");

            /* xsclist_find */
            XRSList_PT node = xsclist_find(slist, "value2");
            xassert(strcmp(node->value, "value2") == 0);

            /* xsclist_find_equal_if */
            node = xsclist_find_equal_if(slist, "value3", test_cmp, NULL);
            xassert(strcmp(node->value, "value3") == 0);

            /* xsclist_find_apply_if */
            node = xsclist_find_apply_if(slist, xsclist_test_apply, "value4");
            xassert(strcmp(node->value, "value4") == 0);

            xsclist_free(&slist);
        }

        /* xsclist_to_array */
        {
            XSCList_PT slist = xsclist_new();

            xsclist_push_back_repeat(slist, "value1");
            xsclist_push_back_repeat(slist, "value2");
            xsclist_push_back_repeat(slist, "value3");
            xsclist_push_back_repeat(slist, "value4");

            XPArray_PT array = xsclist_to_array(slist);

            xassert(array->size == 4);
            xassert(strcmp((char*)xparray_get(array, 0), "value1") == 0);
            xassert(strcmp((char*)xparray_get(array, 1), "value2") == 0);
            xassert(strcmp((char*)xparray_get(array, 2), "value3") == 0);
            xassert(strcmp((char*)xparray_get(array, 3), "value4") == 0);

            xparray_free(&array);
            xsclist_free(&slist);
        }

        /* xsclist_size */
        /* xsclist_is_empty */

        /* xsclist_unique */
        {
            XSCList_PT slist = xsclist_new();

            xsclist_push_back_repeat(slist, "value2");
            xsclist_push_back_repeat(slist, "value1");
            xsclist_push_back_repeat(slist, "value1");
            xsclist_push_back_repeat(slist, "value3");
            xsclist_push_back_repeat(slist, "value2");
            xsclist_push_back_repeat(slist, "value4");

            xassert(slist->size == 6);
            xsclist_unique(slist, test_cmp, NULL);
            xassert(slist->size == 4);

            xassert(strcmp(slist->head->value, "value1") == 0);
            xassert(strcmp(slist->head->next->value, "value2") == 0);
            xassert(strcmp(slist->head->next->next->value, "value3") == 0);
            xassert(strcmp(slist->head->next->next->next->value, "value4") == 0);
            xassert(slist->head->next->next->next->next == slist->head);
            xassert(strcmp(slist->tail->value, "value4") == 0);

            xsclist_free(&slist);
        }

        /* xsclist_deep_unique */
        {
            XSCList_PT slist = xsclist_random_string(1000);
            xsclist_deep_unique(slist, test_cmp, NULL);
            xsclist_deep_free(&slist);
        }

        /* xsclist_sort */
        {
            {
                XSCList_PT slist = xsclist_new();

                xsclist_push_back_repeat(slist, "value2");
                xsclist_push_back_repeat(slist, "value1");
                xsclist_push_back_repeat(slist, "value1");
                xsclist_push_back_repeat(slist, "value3");
                xsclist_push_back_repeat(slist, "value5");
                xsclist_push_back_repeat(slist, "value4");

                xsclist_sort(slist, test_cmp, NULL);

                xassert(strcmp(slist->head->value, "value1") == 0);
                xassert(strcmp(slist->head->next->value, "value1") == 0);
                xassert(strcmp(slist->head->next->next->value, "value2") == 0);
                xassert(strcmp(slist->head->next->next->next->value, "value3") == 0);
                xassert(strcmp(slist->head->next->next->next->next->value, "value4") == 0);
                xassert(strcmp(slist->head->next->next->next->next->next->value, "value5") == 0);
                xassert(slist->head->next->next->next->next->next->next == slist->head);
                xassert(strcmp(slist->tail->value, "value5") == 0);
                xassert(slist->size == 6);

                xsclist_free(&slist);
            }

            {
                XSCList_PT slist = xsclist_random_string(1000);
                xsclist_sort(slist, test_cmp, NULL);
                //xsclist_map(slist, xsclist_test_print, NULL);
                xsclist_deep_free(&slist);
            }
        }

        /* xsclist_is_sorted */
        {
            /* tested by xsclist_sort already */
        }

        {
            int count = 0;
            xmem_leak(xsclist_test_check_mem_leak, &count);
            xassert(count == 0);
        }
}


