
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/xalgos.h"

static
int test_hash_string(void* string)
{
    char *str = (char*)string;
    int h = 0;
    for (; *str; ++str) {
        h = 5 * h + *str;
    }

    return h;
}

static
int test_cmpk(void *key1, void *key2, void *cl) {
    return strcmp((char*)key1, (char*)key2);
}

static
bool xhashset_test_map_apply_false(void *key, void *cl) {
    return false;
}

static
bool xhashset_test_map_apply_true(void *key, void *cl) {
    return true;
}

static
bool xhashset_test_map_cmp(void *key, void *cl) {
    return strcmp((char*)key, (char*)cl) == 0;
}

static
bool xhashset_test_map_apply(void *key, void *cl) {
    printf("%s\n", (char*)key);
    return true;
}

static
XHashSet_PT xhashset_random_string(int(*cmp)(void *key1, void *key2, void *cl), void *cl, int total_size, int string_length) {
    xassert(cmp);
    xassert(0 < total_size);
    xassert(0 < string_length);

    if (!cmp || (total_size <= 0) || (string_length <= 0)) {
        return NULL;
    }

    {
        XHashSet_PT set = xhashset_new(total_size, test_hash_string, cmp, cl);
        if (!set) {
            return NULL;
        }

        {
            const char charsets[] = "0123456789";

            for (int i = 0; i < total_size; i++) {
                char* str = XMEM_MALLOC(string_length + 1);
                if (!str) {
                    xhashset_deep_free(&set);
                    return NULL;
                }

                for (int i = 0; i < string_length; ++i) {
                    str[i] = charsets[rand() % (sizeof(charsets) - 1)];
                }
                str[string_length] = '\0';

                if (!xhashset_put_repeat(set, str)) {
                    xhashset_deep_free(&set);
                    return NULL;
                }
            }
        }

        return set;
    }
}

static
void xhashset_printnode(char* key, char* value, bool color, int h) {
    for (int i = 0; i < h; i++) {
        printf(" ");
    }
    if (value) {
        color ? printf("%s:%s\n", key, value) : printf("%s(R):%s\n", key, value);
    }
    else {
        color ? printf("%s:-\n", key) : printf("%s(R):-\n", key);
    }

    return;
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

void test_xhashset() {
    /* xhashset_new */
    {
    }

    /* xhashset_copy */
    {
        XHashSet_PT set = xhashset_random_string(test_cmpk, NULL, 50, 3);
        XHashSet_PT nset = xhashset_copy(set);
        xhashset_deep_free(&set);
        xhashset_free(&nset);
    }

    /* xhashset_deep_copy */
    {
        XHashSet_PT set = xhashset_random_string(test_cmpk, NULL, 50, 3);
        XHashSet_PT nset = xhashset_deep_copy(set, 3);
        xhashset_deep_free(&set);
        xhashset_deep_free(&nset);
    }

    /* xhashset_put_repeat */
    {
        XHashSet_PT set = xhashset_new(100, test_hash_string, test_cmpk, NULL);
        xhashset_put_repeat(set, "a");
        xhashset_put_repeat(set, "b");
        xhashset_put_repeat(set, "c");
        xhashset_put_repeat(set, "a");
        xhashset_put_repeat(set, "b");
        xhashset_put_repeat(set, "c");
        xhashset_put_repeat(set, "a");
        xhashset_put_repeat(set, "b");
        xhashset_put_repeat(set, "c");
        xhashset_put_repeat(set, "a");
        xhashset_put_repeat(set, "b");
        xhashset_put_repeat(set, "c");
        xhashset_put_repeat(set, "a");
        xhashset_put_repeat(set, "b");
        xhashset_put_repeat(set, "c");

        xassert(xhashset_count(set, "a") == 5);
        xassert(xhashset_count(set, "b") == 5);
        xassert(xhashset_count(set, "b") == 5);

        xhashset_free(&set);
    }

    /* xhashset_put_unique */
    {
        XHashSet_PT set = xhashset_new(100, test_hash_string, test_cmpk, NULL);
        xhashset_put_unique(set, "a");
        xhashset_put_unique(set, "b");
        xhashset_put_unique(set, "c");
        xhashset_put_unique(set, "a");
        xhashset_put_unique(set, "b");
        xhashset_put_unique(set, "c");
        xhashset_put_unique(set, "a");
        xhashset_put_unique(set, "b");
        xhashset_put_unique(set, "c");
        xhashset_put_unique(set, "a");
        xhashset_put_unique(set, "b");
        xhashset_put_unique(set, "c");
        xhashset_put_unique(set, "a");
        xhashset_put_unique(set, "b");
        xhashset_put_unique(set, "c");

        xassert(xhashset_count(set, "a") == 1);
        xassert(xhashset_count(set, "b") == 1);
        xassert(xhashset_count(set, "c") == 1);

        xhashset_free(&set);
    }
     
    /* xhashset_find */
    {
        XHashSet_PT set = xhashset_new(100, test_hash_string, test_cmpk, NULL);

        xassert(xhashset_put_repeat(set, "a"));
        xassert(xhashset_put_repeat(set, "b"));
        xassert(xhashset_put_repeat(set, "c"));
        xassert(xhashset_put_repeat(set, "d"));        

        xassert_false(xhashset_find(set, "s"));
        xassert(xhashset_find(set, "d"));

        xhashset_free(&set);
    }

    /* xhashset_deep_clear */
    {
        XHashSet_PT set = xhashset_random_string(test_cmpk, NULL, 50, 3);
        xhashset_deep_clear(set);
        xassert(xhashset_size(set) == 0);
        xhashset_free(&set);
    }

    /* xhashset_remove */
    /* xhashset_remove_all */
    {
        XHashSet_PT set = xhashset_new(100, test_hash_string, test_cmpk, NULL);
        xhashset_put_repeat(set, "a");
        xhashset_put_repeat(set, "b");
        xhashset_put_repeat(set, "c");
        xhashset_put_repeat(set, "a");
        xhashset_put_repeat(set, "b");
        xhashset_put_repeat(set, "c");
        xhashset_put_repeat(set, "a");
        xhashset_put_repeat(set, "b");
        xhashset_put_repeat(set, "c");
        xhashset_put_repeat(set, "a");
        xhashset_put_repeat(set, "b");
        xhashset_put_repeat(set, "c");
        xhashset_put_repeat(set, "a");
        xhashset_put_repeat(set, "b");
        xhashset_put_repeat(set, "c");

        xhashset_remove(set, "a");
        xassert(xhashset_count(set, "a") == 4);

        xhashset_remove(set, "a");
        xassert(xhashset_count(set, "a") == 3);

        xhashset_remove(set, "a");
        xassert(xhashset_count(set, "a") == 2);

        xhashset_remove(set, "a");
        xassert(xhashset_count(set, "a") == 1);

        xhashset_remove(set, "a");
        xassert(xhashset_count(set, "a") == 0);

        xhashset_remove(set, "a");
        xassert(xhashset_count(set, "a") == 0);

        xhashset_remove_all(set, "b");
        xassert(xhashset_count(set, "b") == 0);

        xhashset_clear(set);
        xassert(xhashset_size(set) == 0);

        xhashset_free(&set);
    }

    /* xhashset_deep_remove */
    /* xhashset_deep_remove_all */
    {
        XHashSet_PT set = xhashset_random_string(test_cmpk, NULL, 500, 2);
        xhashset_deep_remove(set, "0");
        xhashset_deep_remove(set, "1");
        xhashset_deep_remove(set, "2");
        xhashset_deep_remove(set, "3");

        xhashset_deep_remove_all(set, "4");
        xhashset_deep_remove_all(set, "5");
        xhashset_deep_remove_all(set, "6");
        xhashset_deep_remove_all(set, "7");

        xassert(xhashset_count(set, "4") == 0);
        xassert(xhashset_count(set, "5") == 0);
        xassert(xhashset_count(set, "6") == 0);
        xassert(xhashset_count(set, "7") == 0);

        xhashset_deep_free(&set);
    }

    {
        XHashSet_PT set = xhashset_random_string(test_cmpk, NULL, 10, 3);

        //xhashset_map_min_to_max(set, xhashset_test_map_applykv, NULL);
        //xhashset_map_max_to_min(set, xhashset_test_map_applykv, NULL);

        xhashset_deep_free(&set);
    }

    {
        XHashSet_PT set = xhashset_new(100, test_hash_string, test_cmpk, NULL);

        xassert(xhashset_put_repeat(set, "a"));
        xassert(xhashset_put_repeat(set, "b"));
        xassert(xhashset_put_repeat(set, "c"));
        xassert(xhashset_put_repeat(set, "d"));
        xassert(xhashset_put_repeat(set, "e"));

        /* xhashset_map_min_to_max_break_if_true */
        xassert_false(xhashset_map_break_if_true(set, xhashset_test_map_apply_false, NULL));
        xassert_false(xhashset_map_break_if_true(set, xhashset_test_map_cmp, "x"));
        xassert(xhashset_map_break_if_true(set, xhashset_test_map_cmp, "a"));

        /* xhashset_map_min_to_max_break_if_false */
        xassert_false(xhashset_map_break_if_false(set, xhashset_test_map_apply_true, NULL));
        xassert(xhashset_map_break_if_false(set, xhashset_test_map_cmp, "a"));
        xassert(xhashset_map_break_if_false(set, xhashset_test_map_cmp, "b"));

        xhashset_free(&set);
    }

    /* xhashset_swap */
    {
    }

    /* xhashset_size */
    {
    }

    /* xhashset_is_empty */
    {
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
