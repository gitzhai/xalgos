
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/xutils.h"
#include "../list_d_raw/xlist_d_raw_x.h"
#include "../hash_kvtable/xhash_kvtable_x.h"
#include "../include/xalgos.h"

static
int test_hash_string(void *str)
{
    char *nstr = (char*)str;
    int h = 0;
    for (; *nstr; ++nstr)
        h = 5 * h + *nstr;

    return h;
}

static
int xkvhashtab_equal(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static 
bool xkvhashtab_copy_check(void *key, void **value, void *cl)
{
    XKVHashtab_PT ntable = (XKVHashtab_PT)cl;
    xassert(xkvhashtab_get(ntable, key));
    return true;
}

static
bool xkvhashtab_map_apply_true(void *key, void **value, void *cl)
{
    return true;
}

static
bool xkvhashtab_map_apply_false(void *key, void **value, void *cl)
{
    return false;
}

static
bool xkvhashtab_map_aab_false(void *key, void **value, void *cl)
{
    if (!strcmp((char*)key, "aab")) {
        return false;
    }

    return true;
}

XKVHashtab_PT xkvhashtab_random_string(int hint, int(*hash)(void *key), int(*cmp)(void *key1, void *key2, void *cl), void *cl, int total_size, int string_length) {
    xassert(0 < hint);
    xassert(cmp);
    xassert(hash);
    xassert(0 <= total_size);
    xassert(0 < string_length);

    if ((hint <= 0) || !cmp || !hash || (total_size < 0) || (string_length <= 0)) {
        return NULL;
    }

    {
        XKVHashtab_PT table = xkvhashtab_new(hint, hash, cmp, cl);
        if (!table) {
            return NULL;
        }

        {
            const char charsets[] = "0123456789";

            for (int i = 0; i < total_size; i++) {
                char* key = NULL;
                char* value = NULL;

                {
                    key = XMEM_MALLOC(string_length + 1);
                    if (!key) {
                        xkvhashtab_deep_free(&table);
                        return NULL;
                    }

                    for (int i = 0; i < string_length; ++i) {
                        key[i] = charsets[rand() % (sizeof(charsets) - 1)];
                    }
                    key[string_length] = '\0';
                }

                {
                    value = XMEM_MALLOC(string_length + 1);
                    if (!value) {
                        XMEM_FREE(key);
                        xkvhashtab_deep_free(&table);
                        return NULL;
                    }

                    for (int i = 0; i < string_length; ++i) {
                        value[i] = charsets[rand() % (sizeof(charsets) - 1)];
                    }
                    value[string_length] = '\0';
                }

                if (!xkvhashtab_put_repeat(table, key, value)) {
                    xkvhashtab_deep_free(&table);
                    return NULL;
                }
            }
        }

        return table;
    }
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}


void test_xkvhashtab() {

    /* xkvhashtab_new */
    {
        XKVHashtab_PT table = NULL;

        {
            bool except = false;

            XEXCEPT_TRY
                table = xkvhashtab_new(-1, test_hash_string, xkvhashtab_equal, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                table = xkvhashtab_new(10, test_hash_string, NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                table = xkvhashtab_new(10, NULL, xkvhashtab_equal, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                table = xkvhashtab_new(-1, NULL, NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            table = xkvhashtab_new(0, test_hash_string, xkvhashtab_equal, NULL);
            xassert(table);
            xassert(table->slot == 53);
            xkvhashtab_free(&table);
        }

        {
            table = xkvhashtab_new(6151, test_hash_string, xkvhashtab_equal, NULL);
            xassert(table);
            xassert(table->slot == 6151);
            xkvhashtab_free(&table);
        }

#if 0
        /* need too much time and free memory to test it */
        {
            table = xkvhashtab_new(1000000, test_hash_string, xkvhashtab_equal, NULL);
            xassert(table);
            xassert(table->slot == 1572869);
            xkvhashtab_free(&table);
        }

        {
            table = xkvhashtab_new(1000000000, test_hash_string, xkvhashtab_equal, NULL);
            xassert(table);
            xassert(table->slot == 100663319);
            xkvhashtab_free(&table);
        }
#endif
    }

    /* xkvhashtab_copy */
    {
        {
            xassert_false(xkvhashtab_copy(NULL));
        }

        {
            XKVHashtab_PT table = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);
            XKVHashtab_PT ntable = xkvhashtab_copy(table);

            xassert(ntable->size == 0);
            xassert(ntable->slot == table->slot);

            xkvhashtab_free(&ntable);
            xkvhashtab_free(&table);
        }

        {
            XKVHashtab_PT table = xkvhashtab_random_string(100, test_hash_string, xkvhashtab_equal, NULL, 100 * 2, 8);
            XKVHashtab_PT ntable = xkvhashtab_copy(table);

            xassert(ntable->size == table->size);
            xassert(ntable->slot == table->slot);

            xkvhashtab_map(table, xkvhashtab_copy_check, ntable);

            xkvhashtab_free(&ntable);
            xkvhashtab_deep_free(&table);
        }
    }

    /* xkvhashtab_deep_copy */
    {
        {
            XKVHashtab_PT table = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);

            xassert_false(xkvhashtab_deep_copy(NULL, 10, 3));

            {
                bool except = false;

                XEXCEPT_TRY
                    xkvhashtab_deep_copy(table, -3, 3);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }

            {
                bool except = false;

                XEXCEPT_TRY
                    xkvhashtab_deep_copy(table, 2, -5);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }

            {
                bool except = false;

                XEXCEPT_TRY
                    xkvhashtab_deep_copy(NULL, 0, -5);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
            
            xkvhashtab_free(&table);
        }

        {
            XKVHashtab_PT table = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);
            XKVHashtab_PT ntable = xkvhashtab_deep_copy(table, 10, 10);

            xassert(ntable->size == 0);
            xassert(ntable->slot == table->slot);

            xkvhashtab_deep_free(&ntable);
            xkvhashtab_deep_free(&table);
        }

        {
            XKVHashtab_PT table = xkvhashtab_random_string(100, test_hash_string, xkvhashtab_equal, NULL, 100 * 2, 8);
            XKVHashtab_PT ntable = xkvhashtab_deep_copy(table, 9, 9);

            xassert(ntable->size == table->size);
            xassert(ntable->slot == table->slot);

            xkvhashtab_map(table, xkvhashtab_copy_check, ntable);

            xkvhashtab_deep_free(&ntable);
            xkvhashtab_deep_free(&table);
        }

        {
            XKVHashtab_PT table = xkvhashtab_random_string(100, test_hash_string, xkvhashtab_equal, NULL, 100 * 2, 8);
            XKVHashtab_PT ntable = xkvhashtab_deep_copy(table, 9, 0);

            xassert(ntable->size == table->size);
            xassert(ntable->slot == table->slot);

            xkvhashtab_map(table, xkvhashtab_copy_check, ntable);

            xkvhashtab_deep_free(&ntable);
            xkvhashtab_deep_free(&table);
        }
    }

    /* xkvhashtab_put_repeat */
    {
        XKVHashtab_PT table = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);
        {
            xassert(-1 == xkvhashtab_put_repeat(NULL, "abc", "def"));

            {
                bool except = false;

                XEXCEPT_TRY
                    xkvhashtab_put_repeat(table, NULL, "def");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
        }

        {
            xassert(1 == xkvhashtab_put_repeat(table, "aaa", NULL));
            xassert(1 == xkvhashtab_put_repeat(table, "aab", NULL));
            xassert(1 == xkvhashtab_put_repeat(table, "aac", NULL));
            xassert(1 == xkvhashtab_put_repeat(table, "aaa", "def"));
            xassert(1 == xkvhashtab_put_repeat(table, "aab", "deg"));
            xassert(1 == xkvhashtab_put_repeat(table, "aac", "deh"));
        }

        xassert(xkvhashtab_size(table) == 6);
        xassert(xkvhashtab_key_size(table, "aab") == 2);

        {
            XRDList_PT node = NULL;
            node = xkvhashtab_get(table, "aab");
            xassert(!strcmp(xpair_second(node->value), "deg"));

            xkvhashtab_remove(table, "aab");
            xassert(xkvhashtab_key_size(table, "aab") == 1);
            node = xkvhashtab_get(table, "aab");
            xassert_false(xpair_second(node->value));
        }

        xkvhashtab_free(&table);
    }

    /* xkvhashtab_put_unique */
    {
        XKVHashtab_PT table = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);
        {
            xassert(-1 == xkvhashtab_put_unique(NULL, "abc", "def"));

            {
                bool except = false;

                XEXCEPT_TRY
                    xkvhashtab_put_unique(table, NULL, "def");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
        }

        {
            xassert(1 == xkvhashtab_put_unique(table, "aaa", NULL));
            xassert(1 == xkvhashtab_put_unique(table, "aab", NULL));
            xassert(1 == xkvhashtab_put_unique(table, "aac", NULL));
            xassert(0 == xkvhashtab_put_unique(table, "aaa", "def"));
            xassert(0 == xkvhashtab_put_unique(table, "aab", "deg"));
            xassert(0 == xkvhashtab_put_unique(table, "aac", "deh"));
        }

        xassert(xkvhashtab_size(table) == 3);

        {
            XRDList_PT node = NULL;
            node = xkvhashtab_get(table, "aaa");
            xassert_false(xpair_second(node->value));
        }

        xkvhashtab_free(&table);
    }

    /* xkvhashtab_put_replace */
    {
        XKVHashtab_PT table = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);
        {
            xassert(-1 == xkvhashtab_put_replace(NULL, "abc", "def"));

            {
                bool except = false;

                XEXCEPT_TRY
                    xkvhashtab_put_unique(table, NULL, "def");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
        }

        {
            xassert(1 == xkvhashtab_put_replace(table, "aaa", NULL));
            xassert(1 == xkvhashtab_put_replace(table, "aab", NULL));
            xassert(1 == xkvhashtab_put_replace(table, "aac", NULL));
            xassert(0 == xkvhashtab_put_replace(table, "aaa", "def"));
            xassert(0 == xkvhashtab_put_replace(table, "aab", "deg"));
            xassert(0 == xkvhashtab_put_replace(table, "aac", "deh"));
        }

        xassert(xkvhashtab_size(table) == 3);

        {
            XRDList_PT node = NULL;
            node = xkvhashtab_get(table, "aaa");
            xassert(!strcmp(xpair_second(node->value), "def"));
        }

        xkvhashtab_free(&table);
    }

    /* xkvhashtab_put_deep_replace */
    {
        XKVHashtab_PT table = xkvhashtab_random_string(100, test_hash_string, xkvhashtab_equal, NULL, 100 * 2, 8);

        {
            char* abc = XMEM_CALLOC(1, 4);
            abc[0] = 'a';
            abc[1] = 'b';
            abc[2] = 'c';

            char* abc2 = XMEM_CALLOC(1, 4);
            abc2[0] = 'a';
            abc2[1] = 'b';
            abc2[2] = 'c';

            char* def1 = XMEM_CALLOC(1, 4);
            def1[0] = 'a';
            def1[1] = 'b';
            def1[2] = 'c';

            char* def2 = XMEM_CALLOC(1, 4);
            def2[0] = 'a';
            def2[1] = 'b';
            def2[2] = 'b';

            xassert(1 == xkvhashtab_put_deep_replace(table, abc, def1));
            xassert(0 == xkvhashtab_put_deep_replace(table, abc2, def2));

            {
                XRDList_PT node = NULL;
                node = xkvhashtab_get(table, abc2);
                xassert(!strcmp(xpair_second(node->value), "abb"));
            }
        }

        xkvhashtab_deep_free(&table);
    }

    /* xkvhashtab_remove */
    {
        XKVHashtab_PT table = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);
        {
            xassert_false(xkvhashtab_remove(NULL, "abc"));

            {
                bool except = false;

                XEXCEPT_TRY
                    xkvhashtab_remove(table, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
        }

        {
            xkvhashtab_put_repeat(table, "aaa", NULL);
            xkvhashtab_put_repeat(table, "aab", NULL);
            xkvhashtab_put_repeat(table, "aac", NULL);

            xassert(xkvhashtab_remove(table, "aaa"));
            xassert(xkvhashtab_size(table) == 2);
            xassert(xkvhashtab_key_size(table, "aaa") == 0);

            xkvhashtab_put_repeat(table, "aaa", NULL);
            xkvhashtab_put_repeat(table, "aaa", "def");
            xkvhashtab_put_repeat(table, "aab", "deg");
            xkvhashtab_put_repeat(table, "aac", "deh");

            xassert(xkvhashtab_remove(table, "aaa"));
            xassert(xkvhashtab_size(table) == 5);
            xassert(xkvhashtab_key_size(table, "aaa") == 1);
        }

        {
            XRDList_PT node = NULL;
            node = xkvhashtab_get(table, "aaa");
            xassert_false(xpair_second(node->value));

            xkvhashtab_remove(table, "aaa");
            xassert(xkvhashtab_key_size(table, "aaa") == 0);
            xassert_false(xkvhashtab_get(table, "aaa"));
        }

        xkvhashtab_free(&table);
    }

    /* xkvhashtab_remove_all */
    {
        XKVHashtab_PT table = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);
        {
            xassert(-1 == xkvhashtab_remove_all(NULL, "abc"));

            {
                bool except = false;

                XEXCEPT_TRY
                    xkvhashtab_remove_all(table, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
        }

        {
            xkvhashtab_put_repeat(table, "aaa", NULL);
            xkvhashtab_put_repeat(table, "aab", NULL);
            xkvhashtab_put_repeat(table, "aac", NULL);

            xassert(xkvhashtab_remove_all(table, "aaa") == 1);
            xassert(xkvhashtab_size(table) == 2);
            xassert(xkvhashtab_key_size(table, "aaa") == 0);

            xkvhashtab_put_repeat(table, "aaa", NULL);
            xkvhashtab_put_repeat(table, "aaa", "def");
            xkvhashtab_put_repeat(table, "aab", "deg");
            xkvhashtab_put_repeat(table, "aac", "deh");

            xassert(xkvhashtab_remove_all(table, "aaa") == 2);
            xassert(xkvhashtab_size(table) == 4);
            xassert(xkvhashtab_key_size(table, "aaa") == 0);
        }

        {
            xassert_false(xkvhashtab_get(table, "aaa"));
        }

        xkvhashtab_free(&table);
    }

    /* xkvhashtab_clear */
    /* xkvhashtab_free */
    {
        XKVHashtab_PT table = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);

        xkvhashtab_put_repeat(table, "aaa", NULL);
        xkvhashtab_put_repeat(table, "aab", NULL);
        xkvhashtab_put_repeat(table, "aac", NULL);

        xkvhashtab_clear(NULL);
        xassert(xkvhashtab_size(table) == 3);

        xkvhashtab_clear(table);
        xassert(xkvhashtab_size(table) == 0);

        xkvhashtab_free(&table);
    }

    /* xkvhashtab_deep_clear */
    /* xkvhashtab_deep_free */
    {
        XKVHashtab_PT table = xkvhashtab_random_string(100, test_hash_string, xkvhashtab_equal, NULL, 100 * 2, 8);
        xkvhashtab_deep_clear(NULL);
        xassert(xkvhashtab_size(table) == 200);

        xkvhashtab_deep_clear(table);
        xassert(xkvhashtab_size(table) == 0);

        xkvhashtab_deep_free(&table);
    }

    /* xkvhashtab_swap */
    {
        XKVHashtab_PT table1 = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);
        XKVHashtab_PT table2 = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);

        {
            bool except = false;

            XEXCEPT_TRY
                xkvhashtab_swap(NULL, table2);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                xkvhashtab_swap(table1, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                xkvhashtab_swap(NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        xkvhashtab_put_repeat(table1, "aaa1", NULL);
        xkvhashtab_put_repeat(table1, "aab1", NULL);
        xkvhashtab_put_repeat(table1, "aac1", NULL);

        xkvhashtab_put_repeat(table2, "aaa2", NULL);
        xkvhashtab_put_repeat(table2, "aab2", NULL);
        xkvhashtab_put_repeat(table2, "aac2", NULL);
        xkvhashtab_put_repeat(table2, "aad2", NULL);

        xassert(xkvhashtab_swap(table1, table2));

        xassert(table1->size == 4);
        xassert(xkvhashtab_get(table1, "aaa2"));

        xassert(table2->size == 3);
        xassert(xkvhashtab_get(table2, "aaa1"));

        xkvhashtab_free(&table1);
        xkvhashtab_free(&table2);
    }
    
    /* xkvhashtab_map */
    {
        XKVHashtab_PT table = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);
        {
            xassert(-1 == xkvhashtab_map(NULL, xkvhashtab_map_apply_true, NULL));

            {
                bool except = false;

                XEXCEPT_TRY
                    xkvhashtab_map(table, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
        }

        {
            xkvhashtab_put_repeat(table, "aaa", NULL);
            xkvhashtab_put_repeat(table, "aab", NULL);
            xkvhashtab_put_repeat(table, "aac", NULL);

            xassert(3 == xkvhashtab_map(table, xkvhashtab_map_apply_true, NULL));
            xassert(0 == xkvhashtab_map(table, xkvhashtab_map_apply_false, NULL));
            xassert(2 == xkvhashtab_map(table, xkvhashtab_map_aab_false, NULL));
        }

        xkvhashtab_free(&table);
    }

    /* xkvhashtab_map_break_if_true */
    {
        XKVHashtab_PT table = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);
        {
            xassert_false(xkvhashtab_map_break_if_true(NULL, xkvhashtab_map_apply_true, NULL));

            {
                bool except = false;

                XEXCEPT_TRY
                    xkvhashtab_map_break_if_true(table, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
        }

        {
            xkvhashtab_put_repeat(table, "aaa", NULL);
            xkvhashtab_put_repeat(table, "aab", NULL);
            xkvhashtab_put_repeat(table, "aac", NULL);

            xassert(xkvhashtab_map_break_if_true(table, xkvhashtab_map_apply_true, NULL));
            xassert_false(xkvhashtab_map_break_if_true(table, xkvhashtab_map_apply_false, NULL));
            xassert(xkvhashtab_map_break_if_true(table, xkvhashtab_map_aab_false, NULL));
        }

        xkvhashtab_free(&table);
    }


    /* xkvhashtab_map_break_if_false */
    {
        XKVHashtab_PT table = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);
        {
            xassert(xkvhashtab_map_break_if_false(NULL, xkvhashtab_map_apply_true, NULL));

            {
                bool except = false;

                XEXCEPT_TRY
                    xkvhashtab_map_break_if_false(table, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
        }

        {
            xkvhashtab_put_repeat(table, "aaa", NULL);
            xkvhashtab_put_repeat(table, "aab", NULL);
            xkvhashtab_put_repeat(table, "aac", NULL);

            xassert_false(xkvhashtab_map_break_if_false(table, xkvhashtab_map_apply_true, NULL));
            xassert(xkvhashtab_map_break_if_false(table, xkvhashtab_map_apply_false, NULL));
            xassert(xkvhashtab_map_break_if_false(table, xkvhashtab_map_aab_false, NULL));
        }

        xkvhashtab_free(&table);
    }

    /* xkvhashtab_get */
    /* xkvhashtab_find */
    {
        XKVHashtab_PT table = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);
        {
            xassert_false(xkvhashtab_get(NULL, "abc"));

            {
                bool except = false;

                XEXCEPT_TRY
                    xkvhashtab_get(table, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
        }

        {
            xkvhashtab_put_repeat(table, "aaa", NULL);
            xkvhashtab_put_repeat(table, "aab", NULL);
            xkvhashtab_put_repeat(table, "aac", NULL);

            xassert(xkvhashtab_get(table, "aac"));
            xassert_false(xkvhashtab_get(table, "bbb"));
        }

        {
            xassert(xkvhashtab_find(table, "aac"));
            xassert_false(xkvhashtab_find(table, "bbb"));
        }

        xkvhashtab_free(&table);
    }

    /* xkvhashtab_size */
    {
        XKVHashtab_PT table = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);
        xassert(xkvhashtab_size(table) == 0);

        xkvhashtab_put_repeat(table, "aaa", NULL);
        xkvhashtab_put_repeat(table, "aab", NULL);
        xkvhashtab_put_repeat(table, "aac", NULL);

        xassert(xkvhashtab_size(table) == 3);
        xkvhashtab_remove(table, "aaa");
        xassert(xkvhashtab_size(table) == 2);
        xkvhashtab_remove(table, "aab");
        xassert(xkvhashtab_size(table) == 1);
        xkvhashtab_remove(table, "aac");
        xassert(xkvhashtab_size(table) == 0);

        xkvhashtab_free(&table);
    }

    /* xkvhashtab_is_empty */
    {
        XKVHashtab_PT table = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);
        xassert(xkvhashtab_is_empty(table));

        xkvhashtab_put_repeat(table, "aaa", NULL);

        xassert_false(xkvhashtab_is_empty(table));

        xkvhashtab_put_repeat(table, "aab", NULL);
        xkvhashtab_put_repeat(table, "aac", NULL);

        xassert_false(xkvhashtab_is_empty(table));

        xkvhashtab_remove(table, "aaa");
        xkvhashtab_remove(table, "aab");
        xassert_false(xkvhashtab_is_empty(table));
        xkvhashtab_remove(table, "aac");
        xassert(xkvhashtab_is_empty(table));

        xkvhashtab_free(&table);
    }

    /* xkvhashtab_loading_factor */
    {
        XKVHashtab_PT table1 = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);

        xkvhashtab_put_repeat(table1, "aaa1", NULL);
        xkvhashtab_put_repeat(table1, "aab1", NULL);
        xkvhashtab_put_repeat(table1, "aac1", NULL);

        xassert((xkvhashtab_loading_factor(table1) - (double)3 / xutils_hash_buckets_num(100)) < 0.0001);

        xkvhashtab_free(&table1);
    }

    /* xkvhashtab_bucket_size */
    {
        XKVHashtab_PT table1 = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);

        xkvhashtab_put_repeat(table1, "aaa1", NULL);
        xkvhashtab_put_repeat(table1, "aab1", NULL);
        xkvhashtab_put_repeat(table1, "aac1", NULL);

        xassert(xkvhashtab_bucket_size(table1) == xutils_hash_buckets_num(100));

        xkvhashtab_free(&table1);
    }

    /* xkvhashtab_max_bucket_size */
    {
        XKVHashtab_PT table1 = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);

        xassert(xkvhashtab_max_bucket_size(table1) == 100663319);

        xkvhashtab_free(&table1);
    }
        
    /* xkvhashtab_elems_in_bucket */
    {
        XKVHashtab_PT table1 = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);

        {
            bool except = false;

            XEXCEPT_TRY
                xkvhashtab_elems_in_bucket(NULL, 1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                xkvhashtab_elems_in_bucket(table1, -1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                xkvhashtab_elems_in_bucket(table1, 1000);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                xkvhashtab_elems_in_bucket(NULL, 1000);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        xkvhashtab_free(&table1);
    }
    
    /* xkvhashtab_key_size */
    {
        XKVHashtab_PT table1 = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);

        {
            bool except = false;

            XEXCEPT_TRY
                xkvhashtab_key_size(NULL, "abc");
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                xkvhashtab_key_size(table1, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                xkvhashtab_key_size(NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        xkvhashtab_put_repeat(table1, "aaa1", NULL);
        xkvhashtab_put_repeat(table1, "aaa1", NULL);
        xkvhashtab_put_repeat(table1, "aac1", NULL);
        xkvhashtab_put_repeat(table1, "aaa1", NULL);

        xassert(xkvhashtab_key_size(table1, "aaa1") == 3);

        xkvhashtab_free(&table1);
    }

    /* xkvhashtab_resize */
    {
        XKVHashtab_PT table1 = xkvhashtab_new(100, test_hash_string, xkvhashtab_equal, NULL);

        {
            bool except = false;

            XEXCEPT_TRY
                xkvhashtab_resize(NULL, 1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                xkvhashtab_resize(table1, 0);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                xkvhashtab_resize(NULL, 0);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        xkvhashtab_put_repeat(table1, "aaa1", NULL);
        xkvhashtab_put_repeat(table1, "aaa1", NULL);
        xkvhashtab_put_repeat(table1, "aac1", NULL);
        xkvhashtab_put_repeat(table1, "aaa1", NULL);

        xkvhashtab_resize(table1, 1000);

        xassert(table1->slot != xutils_hash_buckets_num(100));

        xkvhashtab_free(&table1);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
