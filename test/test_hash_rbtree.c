
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/xutils.h"
#include "../hash_rbtree/xhash_rbtree_x.h"
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
int xrbtreehash_equal(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static 
bool xrbtreehash_copy_check(void *key, void **value, void *cl)
{
    XRBTreeHash_PT ntable = (XRBTreeHash_PT)cl;
    xassert(xrbtreehash_find(ntable, key));
    return true;
}

static
bool xrbtreehash_map_apply_true(void *key, void **value, void *cl)
{
    return true;
}

static
bool xrbtreehash_map_apply_false(void *key, void **value, void *cl)
{
    return false;
}

static
bool xrbtreehash_map_aab_false(void *key, void **value, void *cl)
{
    if (!strcmp((char*)key, "aab")) {
        return false;
    }

    return true;
}

XRBTreeHash_PT xrbtreehash_random_string(int hint, int(*hash)(void *key), int(*cmp)(void *key1, void *key2, void *cl), void *cl, int total_size, int string_length) {
    xassert(0 < hint);
    xassert(cmp);
    xassert(hash);
    xassert(0 <= total_size);
    xassert(0 < string_length);

    if ((hint <= 0) || !cmp || !hash || (total_size < 0) || (string_length <= 0)) {
        return NULL;
    }

    {
        XRBTreeHash_PT table = xrbtreehash_new(hint, hash, cmp, cl);
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
                        xrbtreehash_deep_free(&table);
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
                        xrbtreehash_deep_free(&table);
                        return NULL;
                    }

                    for (int i = 0; i < string_length; ++i) {
                        value[i] = charsets[rand() % (sizeof(charsets) - 1)];
                    }
                    value[string_length] = '\0';
                }

                if (!xrbtreehash_put_repeat(table, key, value)) {
                    xrbtreehash_deep_free(&table);
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


void test_xrbtreehash() {

    /* xrbtreehash_new */
    {
        XRBTreeHash_PT table = NULL;

        {
            bool except = false;

            XEXCEPT_TRY
                table = xrbtreehash_new(-1, test_hash_string, xrbtreehash_equal, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                table = xrbtreehash_new(10, test_hash_string, NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                table = xrbtreehash_new(10, NULL, xrbtreehash_equal, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                table = xrbtreehash_new(-1, NULL, NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            table = xrbtreehash_new(0, test_hash_string, xrbtreehash_equal, NULL);
            xassert(table);
            xassert(table->slot == 53);
            xrbtreehash_free(&table);
        }

        {
            table = xrbtreehash_new(6151, test_hash_string, xrbtreehash_equal, NULL);
            xassert(table);
            xassert(table->slot == 6151);
            xrbtreehash_free(&table);
        }
    }

    /* xrbtreehash_copy */
    {
        {
            XRBTreeHash_PT table = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);
            XRBTreeHash_PT ntable = xrbtreehash_copy(table);

            xassert(ntable->size == 0);
            xassert(ntable->slot == table->slot);

            xrbtreehash_free(&ntable);
            xrbtreehash_free(&table);
        }

        {
            XRBTreeHash_PT table = xrbtreehash_random_string(100, test_hash_string, xrbtreehash_equal, NULL, 100 * 2, 8);
            XRBTreeHash_PT ntable = xrbtreehash_copy(table);

            xassert(ntable->size == table->size);
            xassert(ntable->slot == table->slot);

            xrbtreehash_map(table, xrbtreehash_copy_check, ntable);

            xrbtreehash_free(&ntable);
            xrbtreehash_deep_free(&table);
        }
    }

    /* xrbtreehash_deep_copy */
    {
        {
            XRBTreeHash_PT table = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);

            {
                bool except = false;

                XEXCEPT_TRY
                    xrbtreehash_deep_copy(table, -3, 3);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }

            {
                bool except = false;

                XEXCEPT_TRY
                    xrbtreehash_deep_copy(table, 2, -5);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }

            {
                bool except = false;

                XEXCEPT_TRY
                    xrbtreehash_deep_copy(NULL, 0, -5);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
            
            xrbtreehash_free(&table);
        }

        {
            XRBTreeHash_PT table = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);
            XRBTreeHash_PT ntable = xrbtreehash_deep_copy(table, 10, 10);

            xassert(ntable->size == 0);
            xassert(ntable->slot == table->slot);

            xrbtreehash_deep_free(&ntable);
            xrbtreehash_deep_free(&table);
        }

        {
            XRBTreeHash_PT table = xrbtreehash_random_string(100, test_hash_string, xrbtreehash_equal, NULL, 100 * 2, 8);
            XRBTreeHash_PT ntable = xrbtreehash_deep_copy(table, 9, 9);

            xassert(ntable->size == table->size);
            xassert(ntable->slot == table->slot);

            xrbtreehash_map(table, xrbtreehash_copy_check, ntable);

            xrbtreehash_deep_free(&ntable);
            xrbtreehash_deep_free(&table);
        }

        {
            XRBTreeHash_PT table = xrbtreehash_random_string(100, test_hash_string, xrbtreehash_equal, NULL, 100 * 2, 8);
            XRBTreeHash_PT ntable = xrbtreehash_deep_copy(table, 9, 0);

            xassert(ntable->size == table->size);
            xassert(ntable->slot == table->slot);

            xrbtreehash_map(table, xrbtreehash_copy_check, ntable);

            xrbtreehash_deep_free(&ntable);
            xrbtreehash_deep_free(&table);
        }
    }

    /* xrbtreehash_put_repeat */
    {
        XRBTreeHash_PT table = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);
        {
            {
                bool except = false;

                XEXCEPT_TRY
                    xrbtreehash_put_repeat(table, NULL, "def");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
        }

        {
            xassert(1 == xrbtreehash_put_repeat(table, "aaa", NULL));
            xassert(1 == xrbtreehash_put_repeat(table, "aab", NULL));
            xassert(1 == xrbtreehash_put_repeat(table, "aac", NULL));
            xassert(1 == xrbtreehash_put_repeat(table, "aaa", "def"));
            xassert(1 == xrbtreehash_put_repeat(table, "aab", "deg"));
            xassert(1 == xrbtreehash_put_repeat(table, "aac", "deh"));
        }

        xassert(xrbtreehash_size(table) == 6);
        xassert(xrbtreehash_key_size(table, "aab") == 2);

        {
            xrbtreehash_remove(table, "aab");
            xassert(xrbtreehash_key_size(table, "aab") == 1);
        }

        xrbtreehash_free(&table);
    }

    /* xrbtreehash_put_unique */
    {
        XRBTreeHash_PT table = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);
        {
            {
                bool except = false;

                XEXCEPT_TRY
                    xrbtreehash_put_unique(table, NULL, "def");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
        }

        {
            xassert(xrbtreehash_put_unique(table, "aaa", NULL));
            xassert(xrbtreehash_put_unique(table, "aab", NULL));
            xassert(xrbtreehash_put_unique(table, "aac", NULL));
            xassert(xrbtreehash_put_unique(table, "aaa", "def"));
            xassert(xrbtreehash_put_unique(table, "aab", "deg"));
            xassert(xrbtreehash_put_unique(table, "aac", "deh"));
        }

        xassert(xrbtreehash_size(table) == 3);

        {
            xassert(xrbtreehash_find(table, "aaa"));
            xassert_false(xrbtreehash_get(table, "aaa"));
        }

        xrbtreehash_free(&table);
    }

    /* xrbtreehash_put_replace */
    {
        XRBTreeHash_PT table = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);
        {
            {
                bool except = false;

                XEXCEPT_TRY
                    xrbtreehash_put_unique(table, NULL, "def");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
        }

        {
            xassert(xrbtreehash_put_replace(table, "aaa", NULL));
            xassert(xrbtreehash_put_replace(table, "aab", NULL));
            xassert(xrbtreehash_put_replace(table, "aac", NULL));
            xassert(xrbtreehash_put_replace(table, "aaa", "def"));
            xassert(xrbtreehash_put_replace(table, "aab", "deg"));
            xassert(xrbtreehash_put_replace(table, "aac", "deh"));
        }

        xassert(xrbtreehash_size(table) == 3);

        {
            void *str = xrbtreehash_get(table, "aaa");
            xassert(!strcmp((char*)str, "def"));
        }

        xrbtreehash_free(&table);
    }

    /* xrbtreehash_put_deep_replace */
    {
        XRBTreeHash_PT table = xrbtreehash_random_string(100, test_hash_string, xrbtreehash_equal, NULL, 100 * 2, 8);

        {
            char* abc = XMEM_CALLOC(1, 4);
            abc[0] = 'a';
            abc[1] = 'b';
            abc[2] = 'c';

            char* def1 = XMEM_CALLOC(1, 4);
            def1[0] = 'a';
            def1[1] = 'b';
            def1[2] = 'c';

            char* def2 = XMEM_CALLOC(1, 4);
            def2[0] = 'a';
            def2[1] = 'b';
            def2[2] = 'b';

            xassert(xrbtreehash_put_deep_replace(table, abc, def1));
            {
                void *str = xrbtreehash_get(table, abc);
                xassert(!strcmp((char*)str, "abc"));
            }

            xassert(xrbtreehash_put_deep_replace(table, abc, def2));
            {
                void *str = xrbtreehash_get(table, abc);
                xassert(!strcmp((char*)str, "abb"));
            }
        }

        xrbtreehash_deep_free(&table);
    }

    /* xrbtreehash_remove */
    {
        XRBTreeHash_PT table = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);
        {
            {
                bool except = false;

                XEXCEPT_TRY
                    xrbtreehash_remove(table, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
        }

        {
            xrbtreehash_put_repeat(table, "aaa", NULL);
            xrbtreehash_put_repeat(table, "aab", NULL);
            xrbtreehash_put_repeat(table, "aac", NULL);

            xassert(xrbtreehash_remove(table, "aaa"));
            xassert(xrbtreehash_size(table) == 2);
            xassert(xrbtreehash_key_size(table, "aaa") == 0);

            xrbtreehash_put_repeat(table, "aaa", NULL);
            xrbtreehash_put_repeat(table, "aaa", "def");
            xrbtreehash_put_repeat(table, "aab", "deg");
            xrbtreehash_put_repeat(table, "aac", "deh");

            xassert(xrbtreehash_remove(table, "aaa"));
            xassert(xrbtreehash_size(table) == 5);
            xassert(xrbtreehash_key_size(table, "aaa") == 1);
        }

        {
            xrbtreehash_remove(table, "aaa");
            xassert(xrbtreehash_key_size(table, "aaa") == 0);
            xassert_false(xrbtreehash_get(table, "aaa"));
        }

        xrbtreehash_free(&table);
    }

    /* xrbtreehash_deep_remove */

    /* xrbtreehash_remove_all */
    {
        XRBTreeHash_PT table = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);
        {
            {
                bool except = false;

                XEXCEPT_TRY
                    xrbtreehash_remove_all(table, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
        }

        {
            xrbtreehash_put_repeat(table, "aaa", NULL);
            xrbtreehash_put_repeat(table, "aab", NULL);
            xrbtreehash_put_repeat(table, "aac", NULL);

            xassert(xrbtreehash_remove_all(table, "aaa") == 1);
            xassert(xrbtreehash_size(table) == 2);
            xassert(xrbtreehash_key_size(table, "aaa") == 0);

            xrbtreehash_put_repeat(table, "aaa", NULL);
            xrbtreehash_put_repeat(table, "aaa", "def");
            xrbtreehash_put_repeat(table, "aab", "deg");
            xrbtreehash_put_repeat(table, "aac", "deh");

            xassert(xrbtreehash_remove_all(table, "aaa") == 2);
            xassert(xrbtreehash_size(table) == 4);
            xassert(xrbtreehash_key_size(table, "aaa") == 0);
        }

        {
            xassert_false(xrbtreehash_get(table, "aaa"));
        }

        xrbtreehash_free(&table);
    }

    /* xrbtreehash_deep_remove_all */

    /* xrbtreehash_clear */
    /* xrbtreehash_free */
    {
        XRBTreeHash_PT table = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);

        xrbtreehash_put_repeat(table, "aaa", NULL);
        xrbtreehash_put_repeat(table, "aab", NULL);
        xrbtreehash_put_repeat(table, "aac", NULL);

        xassert(xrbtreehash_size(table) == 3);

        xrbtreehash_clear(table);
        xassert(xrbtreehash_size(table) == 0);

        xrbtreehash_free(&table);
    }

    /* xrbtreehash_deep_clear */
    /* xrbtreehash_deep_free */
    {
        XRBTreeHash_PT table = xrbtreehash_random_string(100, test_hash_string, xrbtreehash_equal, NULL, 100 * 2, 8);
        xassert(xrbtreehash_size(table) == 200);

        xrbtreehash_deep_clear(table);
        xassert(xrbtreehash_size(table) == 0);

        xrbtreehash_deep_free(&table);
    }

    /* xrbtreehash_swap */
    {
        XRBTreeHash_PT table1 = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);
        XRBTreeHash_PT table2 = xrbtreehash_new(101, test_hash_string, xrbtreehash_equal, NULL);

        {
            bool except = false;

            XEXCEPT_TRY
                xrbtreehash_swap(NULL, table2);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                xrbtreehash_swap(table1, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                xrbtreehash_swap(NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        xrbtreehash_put_repeat(table1, "aaa1", NULL);
        xrbtreehash_put_repeat(table1, "aab1", NULL);
        xrbtreehash_put_repeat(table1, "aac1", NULL);

        xrbtreehash_put_repeat(table2, "aaa2", NULL);
        xrbtreehash_put_repeat(table2, "aab2", NULL);
        xrbtreehash_put_repeat(table2, "aac2", NULL);
        xrbtreehash_put_repeat(table2, "aad2", NULL);

        xassert(xrbtreehash_swap(table1, table2));

        xassert(table1->size == 4);
        xassert(xrbtreehash_find(table1, "aaa2"));

        xassert(table2->size == 3);
        xassert(xrbtreehash_find(table2, "aaa1"));

        xrbtreehash_free(&table1);
        xrbtreehash_free(&table2);
    }
    
    /* xrbtreehash_map */
    {
        XRBTreeHash_PT table = xrbtreehash_new(0, test_hash_string, xrbtreehash_equal, NULL);
        {
            {
                bool except = false;

                XEXCEPT_TRY
                    xrbtreehash_map(table, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
        }

        {
            xrbtreehash_put_repeat(table, "aaa", NULL);
            xrbtreehash_put_repeat(table, "aab", NULL);
            xrbtreehash_put_repeat(table, "aac", NULL);

            xassert(3 == xrbtreehash_map(table, xrbtreehash_map_apply_true, NULL));
            xassert(0 == xrbtreehash_map(table, xrbtreehash_map_apply_false, NULL));
            xassert(2 == xrbtreehash_map(table, xrbtreehash_map_aab_false, NULL));
        }

        xrbtreehash_free(&table);
    }

    /* xrbtreehash_map_break_if_true */
    {
        XRBTreeHash_PT table = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);
        {
            {
                bool except = false;

                XEXCEPT_TRY
                    xrbtreehash_map_break_if_true(table, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
        }

        {
            xrbtreehash_put_repeat(table, "aaa", NULL);
            xrbtreehash_put_repeat(table, "aab", NULL);
            xrbtreehash_put_repeat(table, "aac", NULL);

            xassert(xrbtreehash_map_break_if_true(table, xrbtreehash_map_apply_true, NULL));
            xassert_false(xrbtreehash_map_break_if_true(table, xrbtreehash_map_apply_false, NULL));
            xassert(xrbtreehash_map_break_if_true(table, xrbtreehash_map_aab_false, NULL));
        }

        xrbtreehash_free(&table);
    }


    /* xrbtreehash_map_break_if_false */
    {
        XRBTreeHash_PT table = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);
        {
            {
                bool except = false;

                XEXCEPT_TRY
                    xrbtreehash_map_break_if_false(table, NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
        }

        {
            xrbtreehash_put_repeat(table, "aaa", NULL);
            xrbtreehash_put_repeat(table, "aab", NULL);
            xrbtreehash_put_repeat(table, "aac", NULL);

            xassert_false(xrbtreehash_map_break_if_false(table, xrbtreehash_map_apply_true, NULL));
            xassert(xrbtreehash_map_break_if_false(table, xrbtreehash_map_apply_false, NULL));
            xassert(xrbtreehash_map_break_if_false(table, xrbtreehash_map_aab_false, NULL));
        }

        xrbtreehash_free(&table);
    }

    /* xrbtreehash_get */
    /* xrbtreehash_find */
    {
        XRBTreeHash_PT table = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);
        {
            {
                bool except = false;

                XEXCEPT_TRY
                    xrbtreehash_get(table, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }
        }

        {
            xrbtreehash_put_repeat(table, "aaa", NULL);
            xrbtreehash_put_repeat(table, "aab", NULL);
            xrbtreehash_put_repeat(table, "aac", NULL);

            xassert_false(xrbtreehash_get(table, "aac"));
            xassert_false(xrbtreehash_get(table, "bbb"));
        }

        {
            xassert(xrbtreehash_find(table, "aac"));
            xassert_false(xrbtreehash_find(table, "bbb"));
        }

        xrbtreehash_free(&table);
    }

    /* xrbtreehash_size */
    {
        XRBTreeHash_PT table = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);
        xassert(xrbtreehash_size(table) == 0);

        xrbtreehash_put_repeat(table, "aaa", NULL);
        xrbtreehash_put_repeat(table, "aab", NULL);
        xrbtreehash_put_repeat(table, "aac", NULL);

        xassert(xrbtreehash_size(table) == 3);
        xrbtreehash_remove(table, "aaa");
        xassert(xrbtreehash_size(table) == 2);
        xrbtreehash_remove(table, "aab");
        xassert(xrbtreehash_size(table) == 1);
        xrbtreehash_remove(table, "aac");
        xassert(xrbtreehash_size(table) == 0);

        xrbtreehash_free(&table);
    }

    /* xrbtreehash_is_empty */
    {
        XRBTreeHash_PT table = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);
        xassert(xrbtreehash_is_empty(table));

        xrbtreehash_put_repeat(table, "aaa", NULL);

        xassert_false(xrbtreehash_is_empty(table));

        xrbtreehash_put_repeat(table, "aab", NULL);
        xrbtreehash_put_repeat(table, "aac", NULL);

        xassert_false(xrbtreehash_is_empty(table));

        xrbtreehash_remove(table, "aaa");
        xrbtreehash_remove(table, "aab");
        xassert_false(xrbtreehash_is_empty(table));
        xrbtreehash_remove(table, "aac");
        xassert(xrbtreehash_is_empty(table));

        xrbtreehash_free(&table);
    }

    /* xrbtreehash_loading_factor */
    {
        XRBTreeHash_PT table1 = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);

        xrbtreehash_put_repeat(table1, "aaa1", NULL);
        xrbtreehash_put_repeat(table1, "aab1", NULL);
        xrbtreehash_put_repeat(table1, "aac1", NULL);

        xassert((xrbtreehash_loading_factor(table1) - (double)3 / xutils_hash_buckets_num(100)) < 0.0001);

        xrbtreehash_free(&table1);
    }

    /* xrbtreehash_bucket_size */
    {
        XRBTreeHash_PT table1 = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);

        xrbtreehash_put_repeat(table1, "aaa1", NULL);
        xrbtreehash_put_repeat(table1, "aab1", NULL);
        xrbtreehash_put_repeat(table1, "aac1", NULL);

        xassert(xrbtreehash_bucket_size(table1) == xutils_hash_buckets_num(100));

        xrbtreehash_free(&table1);
    }

    /* xrbtreehash_elems_in_bucket */
    {
        XRBTreeHash_PT table1 = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);

        {
            bool except = false;

            XEXCEPT_TRY
                xrbtreehash_elems_in_bucket(NULL, 1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                xrbtreehash_elems_in_bucket(table1, -1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                xrbtreehash_elems_in_bucket(table1, 1000);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                xrbtreehash_elems_in_bucket(NULL, 1000);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        xrbtreehash_free(&table1);
    }
    
    /* xrbtreehash_key_size */
    {
        XRBTreeHash_PT table1 = xrbtreehash_new(100, test_hash_string, xrbtreehash_equal, NULL);

        {
            bool except = false;

            XEXCEPT_TRY
                xrbtreehash_key_size(NULL, "abc");
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                xrbtreehash_key_size(table1, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                xrbtreehash_key_size(NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        xrbtreehash_put_repeat(table1, "aaa1", NULL);
        xrbtreehash_put_repeat(table1, "aaa1", NULL);
        xrbtreehash_put_repeat(table1, "aac1", NULL);
        xrbtreehash_put_repeat(table1, "aaa1", NULL);

        xassert(xrbtreehash_key_size(table1, "aaa1") == 3);

        xrbtreehash_free(&table1);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
