
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../list_d_raw/xlist_d_raw_x.h"
#include "../include/xalgos.h"

static
int xrdlist_test_cmp_false(void *value1, void *value2, void *cl) {
    return 1;
}

static 
int xrdlist_test_cmp_int(void *value1, void *value2, void *cl) {
    return value1 == value2 ? 0 : 1;
}

static
int xrdlist_test_cmp(void *value1, void *value2, void *cl) {
    return strcmp((char*)value1, (char*)value2);
}

static
bool xrdlist_test_apply(void *value, void *cl) {
    return strcmp((char*)value, (char*)cl) == 0;
}

static
bool xrdlist_test_apply_true(void *value, void *cl) {
    return true;
}

static
bool xrdlist_test_apply_false(void *value, void *cl) {
    return false;
}

static
bool xrdlist_test_map_apply_count(void *value, void *cl) {
    ++(*((int*)(cl)));
    return true;
}

static
bool xrdlist_print(void *value, void *cl) {
    printf("%s\n", (char*)value);
    return true;
}

static
void xrdlist_test_check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static
XRDList_PT xrdlist_random_string(int size) {
    const int str_size = 4;
    const char charsets[] = "0123456789";

    char* tmp = XMEM_CALLOC(1, str_size);
    if (!tmp) {
        return NULL;
    }
    for (int j = 0; j < str_size - 1; ++j) {
        tmp[j] = charsets[rand() % (sizeof(charsets) - 1)];
    }

    {
        XRDList_PT list = xrdlist_new(tmp);
        if (!list) {
            return NULL;
        }

        for (int i = 1; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xrdlist_deep_free(&list);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            xrdlist_push_front_repeat(&list, str);
        }

        return list;
    }
}

void test_xrdlist() {

    /* xrdlist_new */
    {
        XRDList_PT dlist = xrdlist_new("abc");
        xassert(dlist);
        xassert_false(dlist->prev);
        xassert_false(dlist->next);
        xassert(strcmp(dlist->value, "abc") == 0);
        xassert(xrdlist_size(dlist) == 1);
        xrdlist_free(&dlist);
    }

    /* xrdlist_copy */
    {
        XRDList_PT dlist = xrdlist_new("k1");
        xrdlist_push_front_repeat(&dlist, "k1");
        xrdlist_push_front_repeat(&dlist, "k2");
        xrdlist_push_front_repeat(&dlist, "k3");
        xassert(xrdlist_size(dlist) == 4);

        {
            XRDList_PT ndlist = xrdlist_copy(dlist);

            xassert(strcmp(ndlist->value, "k3") == 0);
            xassert(strcmp(ndlist->next->value, "k2") == 0);
            xassert(strcmp(ndlist->next->next->value, "k1") == 0);
            xassert(strcmp(ndlist->next->next->next->value, "k1") == 0);
            xassert_false(ndlist->next->next->next->next);

            xassert_false(ndlist->prev);
            xassert(strcmp(ndlist->next->prev->value, "k3") == 0);
            xassert(strcmp(ndlist->next->next->prev->value, "k2") == 0);
            xassert(strcmp(ndlist->next->next->next->prev->value, "k1") == 0);

            xassert(xrdlist_size(ndlist) == 4);
            xrdlist_free(&ndlist);
        }

        xrdlist_free(&dlist);
    }

    /* xrdlist_deep_copy */
    {
        XRDList_PT dlist = xrdlist_new("k1");
        xrdlist_push_front_repeat(&dlist, "k1");
        xrdlist_push_front_repeat(&dlist, "k2");
        xrdlist_push_front_repeat(&dlist, "k3");
        xassert(xrdlist_size(dlist) == 4);

        {
            XRDList_PT ndlist = xrdlist_deep_copy(dlist, 3);
            xassert(strcmp(ndlist->value, "k3") == 0);
            xassert(ndlist->value != dlist->value);
            xassert(strcmp(ndlist->next->value, "k2") == 0);
            xassert(strcmp(ndlist->next->next->value, "k1") == 0);
            xassert(strcmp(ndlist->next->next->next->value, "k1") == 0);
            xassert_false(ndlist->next->next->next->next);

            xassert_false(ndlist->prev);
            xassert(strcmp(ndlist->next->prev->value, "k3") == 0);
            xassert(strcmp(ndlist->next->next->prev->value, "k2") == 0);
            xassert(strcmp(ndlist->next->next->next->prev->value, "k1") == 0);

            xassert(xrdlist_size(ndlist) == 4);
            xrdlist_deep_free(&ndlist);
        }

        xrdlist_free(&dlist);
    }

    /* xrdlist_push_front_repeat */
    {
        XRDList_PT dlist = xrdlist_new("k1");
        xrdlist_push_front_repeat(&dlist, "k2");
        xrdlist_push_front_repeat(&dlist, "k1");
        xrdlist_push_front_repeat(&dlist, "k3");

        xassert(strcmp(dlist->value, "k3") == 0);
        xassert(strcmp(dlist->next->value, "k1") == 0);
        xassert(strcmp(dlist->next->next->value, "k2") == 0);
        xassert(strcmp(dlist->next->next->next->value, "k1") == 0);
        xassert_false(dlist->next->next->next->next);

        xassert_false(dlist->prev);
        xassert(strcmp(dlist->next->prev->value, "k3") == 0);
        xassert(strcmp(dlist->next->next->prev->value, "k1") == 0);
        xassert(strcmp(dlist->next->next->next->prev->value, "k2") == 0);

        xassert(xrdlist_size(dlist) == 4);
        xrdlist_free(&dlist);
    }

    /* xrdlist_pop_front */
    {
        XRDList_PT dlist = xrdlist_new("k1");
        xrdlist_push_front_repeat(&dlist, "k2");
        xrdlist_push_front_repeat(&dlist, "k1");
        xrdlist_push_front_repeat(&dlist, "k3");

        {
            void *value = NULL;

            xassert(value = xrdlist_pop_front(&dlist));
            xassert(strcmp(value, "k3") == 0);
            xassert(xrdlist_size(dlist) == 3);

            xassert(value = xrdlist_pop_front(&dlist));
            xassert(strcmp(value, "k1") == 0);
            xassert(xrdlist_size(dlist) == 2);

            xassert(value = xrdlist_pop_front(&dlist));
            xassert(strcmp(value, "k2") == 0);
            xassert(xrdlist_size(dlist) == 1);

            xassert(value = xrdlist_pop_front(&dlist));
            xassert(strcmp(value, "k1") == 0);
            xassert_false(dlist);
        }
    }

    /* xrdlist_push_front_unique */
    {
        XRDList_PT dlist = xrdlist_new("k1");
        xrdlist_push_front_unique(&dlist, "k2");
        xassert(xrdlist_push_front_unique(&dlist, "k1") == 0);
        xrdlist_push_front_unique(&dlist, "k3");
        xassert(xrdlist_size(dlist) == 3);

        xassert(strcmp(dlist->value, "k3") == 0);
        xassert(strcmp(dlist->next->value, "k2") == 0);
        xassert(strcmp(dlist->next->next->value, "k1") == 0);
        xassert_false(dlist->next->next->next);

        xassert_false(dlist->prev);
        xassert(strcmp(dlist->next->prev->value, "k3") == 0);
        xassert(strcmp(dlist->next->next->prev->value, "k2") == 0);

        xrdlist_free(&dlist);
    }

    /* xrdlist_push_front_unique_if */
    {
        XRDList_PT dlist = xrdlist_new("k1");

        xrdlist_push_front_unique_if(&dlist, "k2", xrdlist_test_cmp, NULL);
        xrdlist_push_front_unique_if(&dlist, "k1", xrdlist_test_cmp, NULL);
        xrdlist_push_front_unique_if(&dlist, "k3", xrdlist_test_cmp, NULL);
        xassert(xrdlist_size(dlist) == 3);

        xassert(strcmp(dlist->value, "k3") == 0);
        xassert(strcmp(dlist->next->value, "k2") == 0);
        xassert(strcmp(dlist->next->next->value, "k1") == 0);
        xassert_false(dlist->next->next->next);

        xassert_false(dlist->prev);
        xassert(strcmp(dlist->next->prev->value, "k3") == 0);
        xassert(strcmp(dlist->next->next->prev->value, "k2") == 0);

        xrdlist_free(&dlist);
    }

    /* xrdlist_push_front_replace_if */
    {
        XRDList_PT dlist = xrdlist_new("k1");
        xrdlist_push_front_replace_if(&dlist, "k2", xrdlist_test_cmp, NULL);
        xrdlist_push_front_replace_if(&dlist, "k1", xrdlist_test_cmp, NULL);
        xrdlist_push_front_replace_if(&dlist, "k3", xrdlist_test_cmp, NULL);
        xassert(xrdlist_size(dlist) == 3);

        xassert(strcmp(dlist->value, "k3") == 0);
        xassert(strcmp(dlist->next->value, "k2") == 0);
        xassert(strcmp(dlist->next->next->value, "k1") == 0);
        xassert_false(dlist->next->next->next);

        xassert_false(dlist->prev);
        xassert(strcmp(dlist->next->prev->value, "k3") == 0);
        xassert(strcmp(dlist->next->next->prev->value, "k2") == 0);

        xrdlist_free(&dlist);
    }

    /* xrdlist_push_front_deep_replace_if */
    {
        char *value = XMEM_CALLOC(1, 7);
        memcpy(value, "k1", 2);

        /* repeat value */
        {
            XRDList_PT dlist = xrdlist_new(value);

            xrdlist_push_front_deep_replace_if(&dlist, "k2", xrdlist_test_cmp, NULL);
            xrdlist_push_front_deep_replace_if(&dlist, "k1", xrdlist_test_cmp, NULL);
            xrdlist_push_front_deep_replace_if(&dlist, "k3", xrdlist_test_cmp, NULL);
            xassert(xrdlist_size(dlist) == 3);

            xassert(strcmp(dlist->value, "k3") == 0);
            xassert(strcmp(dlist->next->value, "k2") == 0);
            xassert(strcmp(dlist->next->next->value, "k1") == 0);
            xassert_false(dlist->next->next->next);

            xassert_false(dlist->prev);
            xassert(strcmp(dlist->next->prev->value, "k3") == 0);
            xassert(strcmp(dlist->next->next->prev->value, "k2") == 0);

            xrdlist_free(&dlist);
        }
    }

    /* xrdlist_front */
    {
        {
            xassert_false(xrdlist_front(NULL));
        }

        {
            XRDList_PT dlist = xrdlist_new("k1");
            xrdlist_push_front_repeat(&dlist, "k2");
            xrdlist_push_front_repeat(&dlist, "k3");

            char *str = (char*)xrdlist_front(dlist);
            xassert(strcmp(str, "k3") == 0);
            xassert(xrdlist_size(dlist) == 3);
            xrdlist_free(&dlist);
        }
    }

    /* xrdlist_insert_before */
    {
        XRDList_PT dlist = xrdlist_new("k1");
        xrdlist_push_front_repeat(&dlist, "k2");
        xrdlist_push_front_repeat(&dlist, "k3");

        {
            XRDList_PT node = dlist;
            xrdlist_insert_before(&dlist, node, "k4");
            xassert(xrdlist_size(dlist) == 4);
            /*
            * k4
            * k3
            * k2
            * k1
            */
            xassert(strcmp(dlist->value, "k4") == 0);
            xassert_false(dlist->prev);
            xassert(strcmp(dlist->next->value, "k3") == 0);
            xassert(strcmp(dlist->next->prev->value, "k4") == 0);
        }

        {
            XRDList_PT node = dlist->next;
            xrdlist_insert_before(&dlist, node, "k5");
            xassert(xrdlist_size(dlist) == 5);
            /*
            * k4
            * k5
            * k3
            * k2
            * k1
            */
            xassert(strcmp(dlist->next->value, "k5") == 0);
            xassert(strcmp(dlist->next->next->prev->value, "k5") == 0);
            xassert(strcmp(dlist->next->next->value, "k3") == 0);
        }

        xrdlist_free(&dlist);
    }

    /* xrdlist_insert_after */
    {
        XRDList_PT dlist = xrdlist_new("k1");
        xrdlist_push_front_repeat(&dlist, "k2");
        xrdlist_push_front_repeat(&dlist, "k3");

        {
            XRDList_PT node = dlist->next->next;
            xrdlist_insert_after(dlist, node, "k2");
            xassert(xrdlist_size(dlist) == 4);
            /*
            * k3
            * k2
            * k1
            * k2
            */
            xassert(strcmp(dlist->next->next->next->value, "k2") == 0);
            xassert(strcmp(dlist->next->next->next->prev->value, "k1") == 0);
        }

        xassert(strcmp(dlist->value, "k3") == 0);

        xrdlist_free(&dlist);
    }

    /* xrdlist_remove_before */
    {
        void *value = NULL;

        XRDList_PT dlist = xrdlist_new("k1");
        xrdlist_push_front_repeat(&dlist, "k2");
        xrdlist_push_front_repeat(&dlist, "k3");
        xrdlist_push_front_repeat(&dlist, "k4");
        xrdlist_push_front_repeat(&dlist, "k5");

        {
            XRDList_PT node = dlist->next->next->next->next;
            xrdlist_remove_before(&dlist, node, &value);
            xassert(xrdlist_size(dlist) == 4);
            /*
            *  k5
            *  k4
            *  k3
            *  k1
            */
            xassert(strcmp(value, "k2") == 0);
            xassert(strcmp(dlist->next->next->next->value, "k1") == 0);
            xassert(strcmp(dlist->next->next->next->prev->value, "k3") == 0);
        }

        {
            XRDList_PT node = dlist->next->next;
            xrdlist_remove_before(&dlist, node, &value);
            xassert(xrdlist_size(dlist) == 3);
            /*
            *  k5
            *  k3
            *  k1
            */
            xassert(strcmp(value, "k4") == 0);
            xassert(strcmp(dlist->next->value, "k3") == 0);
            xassert(strcmp(dlist->next->prev->value, "k5") == 0);
        }

        xassert(strcmp(dlist->value, "k5") == 0);
        xassert(strcmp(dlist->next->value, "k3") == 0);
        xassert(strcmp(dlist->next->next->value, "k1") == 0);
        xassert_false(dlist->next->next->next);

        xrdlist_free(&dlist);
    }

    /* xrdlist_remove_after */
    {
        void *value = NULL;

        XRDList_PT dlist = xrdlist_new("k1");
        xrdlist_push_front_repeat(&dlist, "k2");
        xrdlist_push_front_repeat(&dlist, "k3");
        xrdlist_push_front_repeat(&dlist, "k4");
        xrdlist_push_front_repeat(&dlist, "k5");

        {
            XRDList_PT node = dlist->next->next;
            xrdlist_remove_after(dlist, node, &value);
            xassert(xrdlist_size(dlist) == 4);
            /*
            *  k5
            *  k4
            *  k3
            *  k1
            */
            xassert(strcmp(value, "k2") == 0);
            xassert(strcmp(dlist->next->next->next->value, "k1") == 0);
            xassert(strcmp(dlist->next->next->next->prev->value, "k3") == 0);
        }

        {
            XRDList_PT node = dlist->next->next;
            xrdlist_remove_after(dlist, node, &value);
            xassert(xrdlist_size(dlist) == 3);
            /*
            *  k5
            *  k4
            *  k3
            */
            xassert(strcmp(value, "k1") == 0);
        }

        xassert(strcmp(dlist->value, "k5") == 0);
        xassert(strcmp(dlist->next->value, "k4") == 0);
        xassert(strcmp(dlist->next->next->value, "k3") == 0);
        xassert_false(dlist->next->next->next);

        xrdlist_free(&dlist);
    }

    /* xrdlist_remove_if_impl */
    {
        /* no any value match */
        {
            XRDList_PT dlist = xrdlist_new("k1");
            xrdlist_push_front_repeat(&dlist, "k2");
            xrdlist_push_front_repeat(&dlist, "k1");

            xassert(xrdlist_remove_if_impl(&dlist, false, false, xrdlist_test_cmp_false, NULL, NULL, NULL) == 0);
            xassert(xrdlist_remove_if_impl(&dlist, false, false, NULL, NULL, xrdlist_test_apply_false, NULL) == 0);
            xassert(xrdlist_size(dlist) == 3);

            xassert(xrdlist_remove_if_impl(&dlist, false, false, xrdlist_test_cmp, NULL, NULL, "k3") == 0);
            xassert(xrdlist_remove_if_impl(&dlist, false, false, NULL, NULL, xrdlist_test_apply, "k3") == 0);
            xassert(xrdlist_size(dlist) == 3);

            xrdlist_free(&dlist);
        }

        /* remove one */
        {
            XRDList_PT dlist = xrdlist_new("k1");
            xrdlist_push_front_repeat(&dlist, "k2");
            xrdlist_push_front_repeat(&dlist, "k1");
            xrdlist_push_front_repeat(&dlist, "k3");
            xrdlist_push_front_repeat(&dlist, "k1");

            xassert(xrdlist_remove_if_impl(&dlist, false, true, xrdlist_test_cmp, NULL, NULL, "k1") == 1);
            xassert_false(dlist->prev);
            xassert(strcmp(dlist->value, "k3") == 0);
            xassert(xrdlist_size(dlist) == 4);
            xassert(xrdlist_remove_if_impl(&dlist, false, true, NULL, NULL, xrdlist_test_apply, "k1") == 1);
            xassert(strcmp(dlist->value, "k3") == 0);
            xassert(strcmp(dlist->next->value, "k2") == 0);
            xassert(strcmp(dlist->next->prev->value, "k3") == 0);
            xassert(xrdlist_size(dlist) == 3);
            xassert(dlist);
            xassert(dlist->next);
            xassert(dlist->next->next);
            xassert_false(dlist->next->next->next);

            xrdlist_free(&dlist);
        }

        /* remove all */
        {
            XRDList_PT dlist = xrdlist_new("k1");
            xrdlist_push_front_repeat(&dlist, "k2");
            xrdlist_push_front_repeat(&dlist, "k1");
            xrdlist_push_front_repeat(&dlist, "k1");
            xrdlist_push_front_repeat(&dlist, "k2");
            xrdlist_push_front_repeat(&dlist, "k1");

            xassert(xrdlist_remove_if_impl(&dlist, false, false, xrdlist_test_cmp, NULL, NULL, "k1") == 4);
            xassert(xrdlist_size(dlist) == 2);
            xassert(xrdlist_find(dlist, "k1") == NULL);
            xassert_false(dlist->prev);
            xassert(strcmp(dlist->value, "k2") == 0);
            xassert(strcmp(dlist->next->value, "k2") == 0);
            xassert(strcmp(dlist->next->prev->value, "k2") == 0);
            xassert_false(dlist->next->next);

            xassert(xrdlist_remove_if_impl(&dlist, false, false, NULL, NULL, xrdlist_test_apply, "k2") == 2);
            xassert(xrdlist_find(dlist, "k2") == NULL);
            xassert_false(dlist);
        }

        /* deep remove */
        {
            char *value1 = XMEM_CALLOC(1, 10);
            char *value2 = XMEM_CALLOC(1, 10);
            char *value3 = XMEM_CALLOC(1, 10);

            memcpy(value1, "k1", 2);
            memcpy(value2, "k2", 2);
            memcpy(value3, "k1", 2);

            XRDList_PT dlist = xrdlist_new(value1);
            xrdlist_push_front_repeat(&dlist, value2);
            xrdlist_push_front_repeat(&dlist, value3);

            xassert(xrdlist_remove_if_impl(&dlist, true, false, xrdlist_test_cmp, NULL, NULL, "k1") == 2);
            xassert(xrdlist_size(dlist) == 1);
            xassert(dlist);
            xassert_false(dlist->next);
            xassert_false(dlist->prev);

            xassert(strcmp(dlist->value, "k2") == 0);

            xrdlist_deep_free(&dlist);
        }
    }

    /* xrdlist_remove */
    /* xrdlist_deep_remove */
    /* xrdlist_remove_all */
    /* xrdlist_remove_apply_if */
    /* xrdlist_deep_remove_apply_if */
    /* xrdlist_remove_equal_if */
    /* xrdlist_deep_remove_equal_if */
    /* xrdlist_remove_apply_break_if */
    /* xrdlist_deep_remove_apply_break_if */
    /* xrdlist_remove_equal_break_if */
    /* xrdlist_deep_remove_equal_break_if */
    {
        /* tested by xrdlist_remove_if_impl already */
    }

    /* xrdlist_free */
    /* xrdlist_deep_free */
    {
        /* tested by previous cases already */
    }

    /* xrdlist_swap */
    {
        XRDList_PT dlist1 = xrdlist_new("k1");
        XRDList_PT dlist2 = xrdlist_new("k5");

        xrdlist_push_front_repeat(&dlist1, "k2");
        xrdlist_push_front_repeat(&dlist1, "k3");
        xrdlist_push_front_repeat(&dlist1, "k4");

        xrdlist_push_front_repeat(&dlist2, "k6");
        xrdlist_push_front_repeat(&dlist2, "k7");
        xrdlist_push_front_repeat(&dlist2, "k8");
        xrdlist_push_front_repeat(&dlist2, "k9");

        xassert(xrdlist_swap(&dlist1, &dlist2));
        xassert(xrdlist_size(dlist1) == 5);
        xassert(xrdlist_size(dlist2) == 4);

        xassert(strcmp(dlist1->value, "k9") == 0);
        xassert(strcmp(dlist1->next->next->next->next->value, "k5") == 0);

        xassert(strcmp(dlist2->value, "k4") == 0);
        xassert(strcmp(dlist2->next->next->next->value, "k1") == 0);

        xrdlist_free(&dlist1);
        xrdlist_free(&dlist2);
    }

    /* xrdlist_reverse */
    {
        XRDList_PT dlist = xrdlist_new("k1");
        xrdlist_push_front_repeat(&dlist, "k2");
        xrdlist_push_front_repeat(&dlist, "k3");
        xrdlist_push_front_repeat(&dlist, "k4");

        xrdlist_reverse(&dlist);
        xassert(xrdlist_size(dlist) == 4);

        xassert(strcmp(dlist->value, "k1") == 0);
        xassert(strcmp(dlist->next->value, "k2") == 0);
        xassert(strcmp(dlist->next->next->value, "k3") == 0);
        xassert(strcmp(dlist->next->next->next->value, "k4") == 0);
        xassert_false(dlist->next->next->next->next);
        xassert(strcmp(dlist->next->next->next->prev->value, "k3") == 0);
        xassert(strcmp(dlist->next->next->prev->value, "k2") == 0);
        xassert(strcmp(dlist->next->prev->value, "k1") == 0);
        xassert_false(dlist->prev);

        xrdlist_free(&dlist);
    }

    /* xrdlist_map */
    {
        XRDList_PT dlist = xrdlist_new("k1");

        xrdlist_push_front_repeat(&dlist, "k2");
        xrdlist_push_front_repeat(&dlist, "k3");
        xrdlist_push_front_repeat(&dlist, "k4");

        int count = 0;
        xassert(xrdlist_map(dlist, xrdlist_test_map_apply_count, (void*)&count) == 4);
        xassert(count == 4);

        xrdlist_free(&dlist);
    }

    /* xrdlist_map_break_if_true */
    {
        XRDList_PT dlist = xrdlist_new("k1");
        xrdlist_push_front_repeat(&dlist, "k2");
        xrdlist_push_front_repeat(&dlist, "k3");
        xrdlist_push_front_repeat(&dlist, "k4");

        xassert(xrdlist_map_break_if_true(dlist, xrdlist_test_apply, "k1"));
        xassert(xrdlist_map_break_if_true(dlist, xrdlist_test_apply, "k2"));
        xassert(xrdlist_map_break_if_true(dlist, xrdlist_test_apply, "k3"));
        xassert(xrdlist_map_break_if_true(dlist, xrdlist_test_apply, "k4"));
        xassert_false(xrdlist_map_break_if_true(dlist, xrdlist_test_apply, "k5"));

        xrdlist_free(&dlist);
    }

    /* xrdlist_map_break_if_false */
    {
        XRDList_PT dlist = xrdlist_new("k1");
        xrdlist_push_front_repeat(&dlist, "k2");
        xrdlist_push_front_repeat(&dlist, "k3");
        xrdlist_push_front_repeat(&dlist, "k4");

        xassert(xrdlist_map_break_if_false(dlist, xrdlist_test_apply, "k5"));

        xrdlist_free(&dlist);
    }

    /* xrdlist_find */
    {
        XRDList_PT dlist = xrdlist_new("k1");
        xrdlist_push_front_repeat(&dlist, "k2");
        xrdlist_push_front_repeat(&dlist, "k3");
        xrdlist_push_front_repeat(&dlist, "k4");

        XRDList_PT node = xrdlist_find(dlist, "k2");
        xassert(strcmp(node->value, "k2") == 0);

        xrdlist_free(&dlist);
    }

    /* xrdlist_find_equal_if */
    {
        XRDList_PT dlist = xrdlist_new("k1");
        xrdlist_push_front_repeat(&dlist, "k2");
        xrdlist_push_front_repeat(&dlist, "k3");
        xrdlist_push_front_repeat(&dlist, "k4");

        XRDList_PT node = xrdlist_find_equal_if(dlist, "k2", xrdlist_test_cmp_int, NULL);
        xassert(strcmp(node->value, "k2") == 0);

        xrdlist_free(&dlist);
    }

    /* xrdlist_find_apply_if */
    {
        XRDList_PT dlist = xrdlist_new("k1");
        xrdlist_push_front_repeat(&dlist, "k2");
        xrdlist_push_front_repeat(&dlist, "k3");
        xrdlist_push_front_repeat(&dlist, "k4");

        XRDList_PT node = xrdlist_find_apply_if(dlist, xrdlist_test_apply, "k2");
        xassert(strcmp(node->value, "k2") == 0);

        xrdlist_free(&dlist);
    }

    /* xrdlist_merge */
    {
        XRDList_PT dlist1 = xrdlist_new("k2");
        XRDList_PT dlist2 = xrdlist_new("k4");

        xrdlist_push_front_repeat(&dlist1, "k1");
        xrdlist_push_front_repeat(&dlist2, "k3");

        xassert(xrdlist_merge(dlist1, &dlist2));
        xassert(xrdlist_size(dlist1) == 4);

        xassert(strcmp(dlist1->value, "k1") == 0);
        xassert(strcmp(dlist1->next->value, "k2") == 0);
        xassert(strcmp(dlist1->next->next->value, "k3") == 0);
        xassert(strcmp(dlist1->next->next->prev->value, "k2") == 0);
        xassert(strcmp(dlist1->next->next->next->value, "k4") == 0);
        xassert_false(dlist1->next->next->next->next);

        xrdlist_free(&dlist1);
    }

    /* xrdlist_size */
    {
        /* tested by previous cases already */
    }

    /* xrdlist_sort */
    {
        {
            XRDList_PT slist = xrdlist_new("value2");

            xrdlist_push_front_repeat(&slist, "value1");
            xrdlist_push_front_repeat(&slist, "value1");
            xrdlist_push_front_repeat(&slist, "value3");
            xrdlist_push_front_repeat(&slist, "value5");
            xrdlist_push_front_repeat(&slist, "value4");

            xrdlist_sort(&slist, xrdlist_test_cmp, NULL);

            xassert(strcmp(slist->value, "value1") == 0);
            xassert(strcmp(slist->next->value, "value1") == 0);
            xassert(strcmp(slist->next->next->value, "value2") == 0);
            xassert(strcmp(slist->next->next->next->value, "value3") == 0);
            xassert(strcmp(slist->next->next->next->next->value, "value4") == 0);
            xassert(strcmp(slist->next->next->next->next->next->value, "value5") == 0);
            xassert_false(slist->next->next->next->next->next->next);
            xassert(strcmp(slist->next->next->next->next->next->prev->value, "value4") == 0);
            xassert(strcmp(slist->next->next->next->next->prev->value, "value3") == 0);
            xassert(strcmp(slist->next->next->next->prev->value, "value2") == 0);
            xassert(strcmp(slist->next->next->prev->value, "value1") == 0);
            xassert(strcmp(slist->next->prev->value, "value1") == 0);
            xassert_false(slist->prev);

            xrdlist_free(&slist);
        }

        {
            XRDList_PT slist = xrdlist_random_string(100);
            xrdlist_sort(&slist, xrdlist_test_cmp, NULL);
            //xrdlist_map(slist, xrdlist_print, NULL);
            xrdlist_deep_free(&slist);
        }
    }

    /* xrdlist_is_sorted */
    {
        /* tested in xrdlist_sort already */
    }

    {
        int count = 0;
        xmem_leak(xrdlist_test_check_mem_leak, &count);
        xassert(count == 0);
    }
}


