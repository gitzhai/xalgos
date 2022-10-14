
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../list_s_raw/xlist_s_raw_x.h"
#include "../include/xalgos.h"

static
bool xrslist_print(void *value, void *cl) {
    printf("%s\n", (char*)value);
    return true;
}

static
int xrslist_test_cmp_true(void *value1, void *value2, void *cl) {
    return 0;
}

static
int xrslist_test_cmp_false(void *value1, void *value2, void *cl) {
    return 1;
}

static
int xrslist_test_cmp_int(void *value1, void *value2, void *cl) {
    return value1 == value2 ? 0 : 1;
}

static
int xrslist_test_cmp(void *value1, void *value2, void *cl) {
    return strcmp((char*)value1, (char*)value2);
}

static
bool xrslist_test_apply(void *value, void *cl) {
    return strcmp((char*)value, (char*)cl) == 0;
}

static
bool xrslist_test_apply_true(void *value, void *cl) {
    return true;
}

static
bool xrslist_test_apply_false(void *value, void *cl) {
    return false;
}

static
bool xrslist_test_map_apply_count(void *value, void *cl) {
    ++(*((int*)(cl)));
    return true;
}

static
void xrslist_test_check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static
XRSList_PT xrslist_random_string(int size) {
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
        XRSList_PT list = xrslist_new(tmp);
        if (!list) {
            return NULL;
        }

        for (int i = 1; i < size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xrslist_deep_free(&list);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            xrslist_push_front_repeat(&list, str);
        }

        return list;
    }
}

void test_xrslist() {

    /* xrslist_new */
    {
        XRSList_PT slist = xrslist_new("abc");
        xassert(slist);
        xassert_false(slist->next);
        xassert(strcmp(slist->value, "abc") == 0);
        xassert(xrslist_size(slist) == 1);
        xrslist_free(&slist);
    }

    /* xrslist_copy */
    /* xrslist_deep_copy */
    {
        XRSList_PT slist = xrslist_new("k1");
        xrslist_push_front_repeat(&slist, "k1");
        xrslist_push_front_repeat(&slist, "k2");
        xrslist_push_front_repeat(&slist, "k3");
        xassert(xrslist_size(slist) == 4);

        /* xrslist_copy */
        {
            XRSList_PT nslist = xrslist_copy(slist);
            xassert(strcmp(nslist->value, "k3") == 0);
            xassert(strcmp(nslist->next->value, "k2") == 0);
            xassert(strcmp(nslist->next->next->value, "k1") == 0);
            xassert(strcmp(nslist->next->next->next->value, "k1") == 0);
            xassert_false(nslist->next->next->next->next);
            xassert(xrslist_size(nslist) == 4);
            xrslist_free(&nslist);
        }

        /* xrslist_copy */
        {
            XRSList_PT slist = xrslist_copy(NULL);
            xassert_false(slist);
        }

        /* xrslist_deep_copy */
        {
            XRSList_PT nslist = xrslist_deep_copy(slist, 3);
            xassert(strcmp(nslist->value, "k3") == 0);
            xassert(nslist->value != slist->value);
            xassert(strcmp(nslist->next->value, "k2") == 0);
            xassert(nslist->next->value != slist->next->value);
            xassert(strcmp(nslist->next->next->value, "k1") == 0);
            xassert(nslist->next->next->value != slist->next->next->value);
            xassert(strcmp(nslist->next->next->next->value, "k1") == 0);
            xassert(nslist->next->next->next->value != slist->next->next->next->value);
            xassert_false(nslist->next->next->next->next);
            xassert(xrslist_size(nslist) == 4);
            xrslist_deep_free(&nslist);
        }

        xrslist_free(&slist);
    }

    /* xrslist_push_front_repeat */
    /* xrslist_pop_front */
    {
        XRSList_PT slist = xrslist_new("k1");
        xrslist_push_front_repeat(&slist, "k2");
        {
            xassert(strcmp(slist->value, "k2") == 0);
            xassert(strcmp(slist->next->value, "k1") == 0);
            xassert_false(slist->next->next);
            xassert(xrslist_size(slist) == 2);
        }

        xrslist_push_front_repeat(&slist, "k1");
        {
            xassert(strcmp(slist->value, "k1") == 0);
            xassert(strcmp(slist->next->value, "k2") == 0);
            xassert(strcmp(slist->next->next->value, "k1") == 0);
            xassert_false(slist->next->next->next);
            xassert(xrslist_size(slist) == 3);
        }

        xrslist_push_front_repeat(&slist, "k3");
        {
            xassert(strcmp(slist->value, "k3") == 0);
            xassert(strcmp(slist->next->value, "k1") == 0);
            xassert(strcmp(slist->next->next->value, "k2") == 0);
            xassert(strcmp(slist->next->next->next->value, "k1") == 0);
            xassert_false(slist->next->next->next->next);
            xassert(xrslist_size(slist) == 4);
        }

        /* xrslist_pop_front */
        {
            void *value = NULL;

            xassert(value = xrslist_pop_front(&slist));
            xassert(strcmp(value, "k3") == 0);
            xassert(strcmp(slist->value, "k1") == 0);
            xassert(xrslist_size(slist) == 3);

            xassert(value = xrslist_pop_front(&slist));
            xassert(strcmp(value, "k1") == 0);
            xassert(strcmp(slist->value, "k2") == 0);
            xassert(xrslist_size(slist) == 2);

            xassert(value = xrslist_pop_front(&slist));
            xassert(strcmp(value, "k2") == 0);
            xassert(strcmp(slist->value, "k1") == 0);
            xassert(xrslist_size(slist) == 1);

            xassert(value = xrslist_pop_front(&slist));
            xassert(strcmp(value, "k1") == 0);
            xassert_false(slist);
            xassert(xrslist_size(slist) == 0);
        }

        xassert_false(slist);
    }

    /* xrslist_pop_kth */
    {
        XRSList_PT slist = xrslist_new("k6");
        xrslist_push_front_repeat(&slist, "k5");
        xrslist_push_front_repeat(&slist, "k4");
        xrslist_push_front_repeat(&slist, "k3");
        xrslist_push_front_repeat(&slist, "k2");
        xrslist_push_front_repeat(&slist, "k1");
        xrslist_push_front_repeat(&slist, "k0");

        void *value = NULL;
        value = xrslist_pop_kth(&slist, 3);
        xassert(strcmp(value, "k3") == 0);

        value = xrslist_pop_kth(&slist, 3);
        xassert(strcmp(value, "k4") == 0);

        value = xrslist_pop_kth(&slist, 3);
        xassert(strcmp(value, "k5") == 0);

        value = xrslist_pop_kth(&slist, 3);
        xassert(strcmp(value, "k6") == 0);

        xassert_false(value = xrslist_pop_kth(&slist, 3));

        value = xrslist_pop_kth(&slist, 1);
        xassert(strcmp(value, "k1") == 0);
        xassert(strcmp(slist->next->value, "k2") == 0);

        value = xrslist_pop_kth(&slist, 0);
        xassert(strcmp(value, "k0") == 0);

        xassert(xrslist_size(slist) == 1);
        xrslist_free(&slist);
    }

    /* xrslist_push_front_unique */
    {
        XRSList_PT slist = xrslist_new("k1");
        xrslist_push_front_unique(&slist, "k2");
        xassert(xrslist_push_front_unique(&slist, "k1") == 0);
        xrslist_push_front_unique(&slist, "k3");

        xassert(strcmp(slist->value, "k3") == 0);
        xassert(strcmp(slist->next->value, "k2") == 0);
        xassert(strcmp(slist->next->next->value, "k1") == 0);
        xassert_false(slist->next->next->next);
        xassert(xrslist_size(slist) == 3);

        xrslist_free(&slist);
    }

    /* xrslist_push_front_unique_if */
    {
        XRSList_PT slist = xrslist_new("k1");
        xrslist_push_front_unique_if(&slist, "k2", xrslist_test_cmp, NULL);
        xrslist_push_front_unique_if(&slist, "k1", xrslist_test_cmp, NULL);
        xrslist_push_front_unique_if(&slist, "k3", xrslist_test_cmp, NULL);

        xassert(strcmp(slist->value, "k3") == 0);
        xassert(strcmp(slist->next->value, "k2") == 0);
        xassert(strcmp(slist->next->next->value, "k1") == 0);
        xassert_false(slist->next->next->next);
        xassert(xrslist_size(slist) == 3);

        xrslist_free(&slist);
    }

    /* xrslist_push_front_replace_if */
    {
        XRSList_PT slist = xrslist_new("k1");
        xrslist_push_front_replace_if(&slist, "k2", xrslist_test_cmp, NULL);
        xrslist_push_front_replace_if(&slist, "k1", xrslist_test_cmp, NULL);
        xrslist_push_front_replace_if(&slist, "k3", xrslist_test_cmp, NULL);
        xassert(xrslist_size(slist) == 3);

        xassert(strcmp(slist->value, "k3") == 0);
        xassert(strcmp(slist->next->value, "k2") == 0);
        xassert(strcmp(slist->next->next->value, "k1") == 0);
        xassert_false(slist->next->next->next);

        xrslist_free(&slist);
    }

    /* xrslist_push_front_deep_replace_if */
    {
        char *value = XMEM_CALLOC(1, 7);
        memcpy(value, "k1", 3);

        /* repeat value */
        {
            XRSList_PT slist = xrslist_new(value);

            xrslist_push_front_deep_replace_if(&slist, "k2", xrslist_test_cmp, NULL);
            xrslist_push_front_deep_replace_if(&slist, "k1", xrslist_test_cmp, NULL);
            xrslist_push_front_deep_replace_if(&slist, "k3", xrslist_test_cmp, NULL);

            xassert(strcmp(slist->value, "k3") == 0);
            xassert(strcmp(slist->next->value, "k2") == 0);
            xassert(strcmp(slist->next->next->value, "k1") == 0);
            xassert_false(slist->next->next->next);
            xassert(xrslist_size(slist) == 3);

            xrslist_free(&slist);
        }
    }

    /* xrslist_front */
    {
        {
            xassert_false(xrslist_front(NULL));
        }

        {
            XRSList_PT slist = xrslist_new("k1");
            xrslist_push_front_repeat(&slist, "k2");
            xrslist_push_front_repeat(&slist, "k3");

            char *str = (char*)xrslist_front(slist);
            xassert(strcmp(str, "k3") == 0);
            xassert(xrslist_size(slist) == 3);
            xrslist_free(&slist);
        }
    }

    /* xrslist_insert_after */
    {
        XRSList_PT slist = xrslist_new("k1");
        xrslist_push_front_repeat(&slist, "k2");
        xrslist_push_front_repeat(&slist, "k3");

        {
            XRSList_PT node = slist->next;
            xrslist_insert_after(slist, node, "k4");
            xassert(xrslist_size(slist) == 4);
            /*
            * k3
            * k2
            * k4
            * k1
            */
            xassert(strcmp(slist->next->next->value, "k4") == 0);
            xassert(strcmp(slist->next->next->next->value, "k1") == 0);
        }

        {
            XRSList_PT node = slist->next->next->next;
            xrslist_insert_after(slist, node, "k5");
            xassert(xrslist_size(slist) == 5);
            /*
            * k3
            * k2
            * k4
            * k1
            * k5
            */
            xassert(strcmp(slist->next->next->next->value, "k1") == 0);
            xassert(strcmp(slist->next->next->next->next->value, "k5") == 0);
            xassert_false(slist->next->next->next->next->next);
        }

        xassert(strcmp(slist->value, "k3") == 0);

        xrslist_free(&slist);
    }

    /* xrslist_remove_after */
    {
        void *value = NULL;

        XRSList_PT slist = xrslist_new("k1");
        xrslist_push_front_repeat(&slist, "k2");
        xrslist_push_front_repeat(&slist, "k3");
        xrslist_push_front_repeat(&slist, "k4");
        xrslist_push_front_repeat(&slist, "k5");

        {
            XRSList_PT node = slist->next->next;
            xrslist_remove_after(slist, node, &value);
            xassert(xrslist_size(slist) == 4);
            /*
            *  k5
            *  k4
            *  k3
            *  k1
            */
            xassert(strcmp(value, "k2") == 0);
            xassert(strcmp(slist->next->next->value, "k3") == 0);
            xassert(strcmp(slist->next->next->next->value, "k1") == 0);
        }

        {
            XRSList_PT node = slist->next->next;
            xrslist_remove_after(slist, node, &value);
            xassert(xrslist_size(slist) == 3);
            /*
            *  k5
            *  k4
            *  k3
            */
            xassert(strcmp(value, "k1") == 0);
            xassert(strcmp(slist->next->next->value, "k3") == 0);
            xassert_false(slist->next->next->next);
        }

        xassert(strcmp(slist->value, "k5") == 0);
        xassert(strcmp(slist->next->value, "k4") == 0);

        xrslist_free(&slist);
    }

    /* xrslist_remove_if_impl */
    {
        /* no any value match */
        {
            XRSList_PT slist = xrslist_new("k1");
            xrslist_push_front_repeat(&slist, "k2");
            xrslist_push_front_repeat(&slist, "k1");

            xassert(xrslist_remove_if_impl(&slist, false, false, xrslist_test_cmp_false, NULL, NULL, NULL) == 0);
            xassert(xrslist_remove_if_impl(&slist, false, false, NULL, NULL, xrslist_test_apply_false, NULL) == 0);
            xassert(xrslist_size(slist) == 3);

            xassert(xrslist_remove_if_impl(&slist, false, false, xrslist_test_cmp, NULL, NULL, "k3") == 0);
            xassert(xrslist_remove_if_impl(&slist, false, false, NULL, NULL, xrslist_test_apply, "k3") == 0);
            xassert(xrslist_size(slist) == 3);

            xrslist_free(&slist);
        }

        /* remove one */
        {
            XRSList_PT slist = xrslist_new("k1");
            xrslist_push_front_repeat(&slist, "k2");
            xrslist_push_front_repeat(&slist, "k1");
            xrslist_push_front_repeat(&slist, "k2");
            xrslist_push_front_repeat(&slist, "k1");

            xassert(xrslist_remove_if_impl(&slist, false, true, xrslist_test_cmp, NULL, NULL, "k1") == 1);
            xassert(strcmp(slist->value, "k2") == 0);
            xassert(xrslist_size(slist) == 4);
            xassert(xrslist_remove_if_impl(&slist, false, true, NULL, NULL, xrslist_test_apply, "k1") == 1);
            xassert(strcmp(slist->value, "k2") == 0);
            xassert(strcmp(slist->next->value, "k2") == 0);
            xassert(strcmp(slist->next->next->value, "k1") == 0);
            xassert(xrslist_size(slist) == 3);
            xassert(slist);
            xassert(slist->next);
            xassert(slist->next->next);
            xassert_false(slist->next->next->next);

            xrslist_free(&slist);
        }

        /* remove all */
        {
            XRSList_PT slist = xrslist_new("k1");
            xrslist_push_front_repeat(&slist, "k2");
            xrslist_push_front_repeat(&slist, "k1");
            xrslist_push_front_repeat(&slist, "k1");
            xrslist_push_front_repeat(&slist, "k2");
            xrslist_push_front_repeat(&slist, "k1");

            xassert(xrslist_remove_if_impl(&slist, false, false, xrslist_test_cmp, NULL, NULL, "k1") == 4);
            xassert(xrslist_size(slist) == 2);
            xassert(xrslist_find(slist, "k1") == NULL);
            xassert(strcmp(slist->value, "k2") == 0);
            xassert(strcmp(slist->next->value, "k2") == 0);

            xassert(xrslist_remove_if_impl(&slist, false, false, NULL, NULL, xrslist_test_apply, "k2") == 2);
            xassert(xrslist_find(slist, "k2") == NULL);
            xassert_false(slist);
        }

        /* deep remove */
        {
            char *value1 = XMEM_CALLOC(1, 10);
            char *value2 = XMEM_CALLOC(1, 10);
            char *value3 = XMEM_CALLOC(1, 10);

            memcpy(value1, "k1", 3);
            memcpy(value2, "k2", 3);
            memcpy(value3, "k1", 3);

            XRSList_PT slist = xrslist_new(value1);
            xrslist_push_front_repeat(&slist, value2);
            xrslist_push_front_repeat(&slist, value3);

            xassert(xrslist_remove_if_impl(&slist, true, false, xrslist_test_cmp, NULL, NULL, "k1") == 2);
            xassert(xrslist_size(slist) == 1);
            xassert(slist);
            xassert(strcmp(slist->value, "k2") == 0);
            xassert_false(slist->next);

            xrslist_deep_free(&slist);
        }
    }

    /* xrslist_remove */
    /* xrslist_deep_remove */
    /* xrslist_remove_all */
    /* xrslist_remove_apply_if */
    /* xrslist_deep_remove_apply_if */
    /* xrslist_remove_equal_if */
    /* xrslist_deep_remove_equal_if */
    /* xrslist_remove_apply_break_if */
    /* xrslist_deep_remove_apply_break_if */
    /* xrslist_remove_equal_break_if */
    /* xrslist_deep_remove_equal_break_if */
    {
        /* tested by xrslist_remove_if_impl already */
    }

    /* xrslist_free */
    /* xrslist_deep_free */
    {
        /* tested by previous cases already */
    }

    /* xrslist_free_except_front */
    {
        XRSList_PT slist = xrslist_new("k1");
        xassert_false(xrslist_free_except_front(NULL));
        xassert_false(xrslist_free_except_front(slist));

        xrslist_push_front_repeat(&slist, "k2");
        xrslist_push_front_repeat(&slist, "k3");
        xrslist_push_front_repeat(&slist, "k4");

        xrslist_free_except_front(slist);
        xassert(strcmp(slist->value, "k4") == 0);
        xassert_false(slist->next);
        xrslist_free(&slist);
    }

    /* xrslist_swap */
    {
        XRSList_PT slist1 = xrslist_new("k1");
        XRSList_PT slist2 = xrslist_new("k5");

        xrslist_push_front_repeat(&slist1, "k2");
        xrslist_push_front_repeat(&slist1, "k3");
        xrslist_push_front_repeat(&slist1, "k4");

        xrslist_push_front_repeat(&slist2, "k6");
        xrslist_push_front_repeat(&slist2, "k7");
        xrslist_push_front_repeat(&slist2, "k8");
        xrslist_push_front_repeat(&slist2, "k9");

        xassert(xrslist_swap(&slist1, &slist2));
        xassert(xrslist_size(slist1) == 5);
        xassert(xrslist_size(slist2) == 4);

        xassert(strcmp(slist1->value, "k9") == 0);
        xassert(strcmp(slist1->next->next->next->next->value, "k5") == 0);

        xassert(strcmp(slist2->value, "k4") == 0);
        xassert(strcmp(slist2->next->next->next->value, "k1") == 0);

        xrslist_free(&slist1);
        xrslist_free(&slist2);
    }

    /* xrslist_reverse */
    {
        XRSList_PT slist = xrslist_new("k1");
        xrslist_push_front_repeat(&slist, "k2");
        xrslist_push_front_repeat(&slist, "k3");
        xrslist_push_front_repeat(&slist, "k4");

        xrslist_reverse(&slist);
        xassert(xrslist_size(slist) == 4);

        xassert(strcmp(slist->value, "k1") == 0);
        xassert(strcmp(slist->next->value, "k2") == 0);
        xassert(strcmp(slist->next->next->value, "k3") == 0);
        xassert(strcmp(slist->next->next->next->value, "k4") == 0);
        xassert_false(slist->next->next->next->next);

        xrslist_free(&slist);
    }

    /* xrslist_map */
    {
        XRSList_PT slist = xrslist_new("k1");

        xrslist_push_front_repeat(&slist, "k2");
        xrslist_push_front_repeat(&slist, "k3");
        xrslist_push_front_repeat(&slist, "k4");

        int count = 0;
        xassert(xrslist_map(slist, xrslist_test_map_apply_count, (void*)&count) == 4);
        xassert(count == 4);

        xrslist_free(&slist);
    }

    /* xrslist_map_break_if_true */
    {
        XRSList_PT slist = xrslist_new("k1");
        xrslist_push_front_repeat(&slist, "k2");
        xrslist_push_front_repeat(&slist, "k3");
        xrslist_push_front_repeat(&slist, "k4");

        xassert(xrslist_map_break_if_true(slist, xrslist_test_apply, "k1"));
        xassert(xrslist_map_break_if_true(slist, xrslist_test_apply, "k2"));
        xassert(xrslist_map_break_if_true(slist, xrslist_test_apply, "k3"));
        xassert(xrslist_map_break_if_true(slist, xrslist_test_apply, "k4"));
        xassert_false(xrslist_map_break_if_true(slist, xrslist_test_apply, "k5"));

        xrslist_free(&slist);
    }

    /* xrslist_map_break_if_false */
    {
        XRSList_PT slist = xrslist_new("k1");
        xrslist_push_front_repeat(&slist, "k2");
        xrslist_push_front_repeat(&slist, "k3");
        xrslist_push_front_repeat(&slist, "k4");

        xassert(xrslist_map_break_if_false(slist, xrslist_test_apply, "k5"));

        xrslist_free(&slist);
    }

    /* xrslist_find */
    {
        XRSList_PT slist = xrslist_new("k1");
        xrslist_push_front_repeat(&slist, "k2");
        xrslist_push_front_repeat(&slist, "k3");
        xrslist_push_front_repeat(&slist, "k4");

        XRSList_PT node = xrslist_find(slist, "k2");
        xassert(strcmp(node->value, "k2") == 0);

        xrslist_free(&slist);
    }

    /* xrslist_find_equal_if */
    {
        XRSList_PT slist = xrslist_new("k1");
        xrslist_push_front_repeat(&slist, "k2");
        xrslist_push_front_repeat(&slist, "k3");
        xrslist_push_front_repeat(&slist, "k4");

        XRSList_PT node = xrslist_find_equal_if(slist, "k2", xrslist_test_cmp_int, NULL);
        xassert(strcmp(node->value, "k2") == 0);

        xrslist_free(&slist);
    }

    /* xrslist_find_apply_if */
    {
        XRSList_PT slist = xrslist_new("k1");
        xrslist_push_front_repeat(&slist, "k2");
        xrslist_push_front_repeat(&slist, "k3");
        xrslist_push_front_repeat(&slist, "k4");

        XRSList_PT node = xrslist_find_apply_if(slist, xrslist_test_apply, "k2");
        xassert(strcmp(node->value, "k2") == 0);

        xrslist_free(&slist);
    }

    /* xrslist_get_kth */
    {
        XRSList_PT slist = xrslist_new("k1");
        xrslist_push_front_repeat(&slist, "k2");
        xrslist_push_front_repeat(&slist, "k3");
        xrslist_push_front_repeat(&slist, "k4");

        xassert(strcmp(xrslist_get_kth(slist, 0)->value, "k4") == 0);
        xassert(strcmp(xrslist_get_kth(slist, 1)->value, "k3") == 0);
        xassert(strcmp(xrslist_get_kth(slist, 2)->value, "k2") == 0);
        xassert(strcmp(xrslist_get_kth(slist, 3)->value, "k1") == 0);

        xassert_false(xrslist_get_kth(slist, 4));
        xassert_false(xrslist_get_kth(slist, 10));

        xrslist_free(&slist);
    }

    /* xrslist_merge */
    {
        XRSList_PT slist1 = xrslist_new("k2");
        XRSList_PT slist2 = xrslist_new("k4");

        xrslist_push_front_repeat(&slist1, "k1");
        xrslist_push_front_repeat(&slist2, "k3");

        xassert(xrslist_merge(slist1, &slist2));
        xassert(xrslist_size(slist1) == 4);

        xassert(strcmp(slist1->value, "k1") == 0);
        xassert(strcmp(slist1->next->value, "k2") == 0);
        xassert(strcmp(slist1->next->next->value, "k3") == 0);
        xassert(strcmp(slist1->next->next->next->value, "k4") == 0);
        xassert_false(slist1->next->next->next->next);

        xrslist_free(&slist1);
    }

    /* xrslist_size */
    {
        /* tested by previous cases already */
    }

    /* xrslist_sort */
    {
        {
            XRSList_PT slist = xrslist_new("value2");

            xrslist_push_front_repeat(&slist, "value1");
            xrslist_push_front_repeat(&slist, "value1");
            xrslist_push_front_repeat(&slist, "value3");
            xrslist_push_front_repeat(&slist, "value5");
            xrslist_push_front_repeat(&slist, "value4");

            xrslist_sort(&slist, xrslist_test_cmp, NULL);

            xassert(strcmp(slist->value, "value1") == 0);
            xassert(strcmp(slist->next->value, "value1") == 0);
            xassert(strcmp(slist->next->next->value, "value2") == 0);
            xassert(strcmp(slist->next->next->next->value, "value3") == 0);
            xassert(strcmp(slist->next->next->next->next->value, "value4") == 0);
            xassert(strcmp(slist->next->next->next->next->next->value, "value5") == 0);
            xassert_false(slist->next->next->next->next->next->next);

            xrslist_free(&slist);
        }

        {
            XRSList_PT slist = xrslist_random_string(1000);
            xrslist_sort(&slist, xrslist_test_cmp, NULL);
            //xrslist_map(slist, xrslist_print, NULL);
            xrslist_deep_free(&slist);
        }
    }

    /* xrslist_is_sorted */
    {
        /* tested in xrslist_sort already */
    }

    {
        int count = 0;
        xmem_leak(xrslist_test_check_mem_leak, &count);
        xassert(count == 0);
    }
}


