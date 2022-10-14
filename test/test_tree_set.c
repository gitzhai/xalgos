
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../list_s_raw/xlist_s_raw_x.h"
#include "../tree_set/xtree_set_x.h"
#include "../include/xalgos.h"

static
int test_cmpk(void *key1, void *key2, void *cl) {
    return strcmp((char*)key1, (char*)key2);
}

static
bool xset_test_map_apply_false(void *key, void *cl) {
    return false;
}

static
bool xset_test_map_apply_true(void *key, void *cl) {
    return true;
}

static
bool xset_test_map_cmp(void *key, void *cl) {
    return strcmp((char*)key, (char*)cl) == 0;
}

static
bool xset_test_map_apply(void *key, void *cl) {
    printf("%s\n", (char*)key);
    return true;
}

static
XSet_PT xset_random_string(int(*cmp)(void *key1, void *key2, void *cl), void *cl, int total_size, int string_length) {
    xassert(cmp);
    xassert(0 < total_size);
    xassert(0 < string_length);

    if (!cmp || (total_size <= 0) || (string_length <= 0)) {
        return NULL;
    }

    {
        XSet_PT tree = xset_new(cmp, cl);
        if (!tree) {
            return NULL;
        }

        {
            const char charsets[] = "0123456789";

            for (int i = 0; i < total_size; i++) {
                char* str = XMEM_MALLOC(string_length + 1);
                if (!str) {
                    xset_deep_free(&tree);
                    return NULL;
                }

                for (int i = 0; i < string_length; ++i) {
                    str[i] = charsets[rand() % (sizeof(charsets) - 1)];
                }
                str[string_length] = '\0';

                if (!xset_put_repeat(tree, str)) {
                    xset_deep_free(&tree);
                    return NULL;
                }
            }
        }

        return tree;
    }
}

static
void xset_printnode(char* key, char* value, bool color, int h) {
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
void xset_print_impl(XSet_Node_PT node, int h) {
    if (!node) {
        xset_printnode("-", "-", true, h);
        return;
    }

    xset_print_impl(node->right, h + 3);

    xset_printnode((char*)node->key, (char*)node->values->value, node->color, h);

    xset_print_impl(node->left, h + 3);
}

static
void xset_print(XSet_PT tree, int start_blank) {
    xassert(tree);
    xassert(0 <= start_blank);

    if (start_blank < 0) {
        return;
    }

    if (!tree) {
        xset_printnode("-", "-", false, start_blank);
        return;
    }

    xset_print_impl(tree->root, start_blank + 3);
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

void test_xset() {
    /* xset_new */
    {
        /* tested by xlistrbtree_new already */
    }

    /* xset_copy */
    {
        XSet_PT set = xset_random_string(test_cmpk, NULL, 50, 3);
        XSet_PT nset = xset_copy(set);
        xset_deep_free(&set);
        xset_free(&nset);
    }

    /* xset_deep_copy */
    {
        XSet_PT set = xset_random_string(test_cmpk, NULL, 50, 3);
        XSet_PT nset = xset_deep_copy(set, 3);
        xset_deep_free(&set);
        xset_deep_free(&nset);
    }

    /* xset_put_repeat */
    {
        XSet_PT set = xset_new(test_cmpk, NULL);
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");

        xassert(xset_elems_size(set, "a", "a") == 5);
        xassert(xset_elems_size(set, "b", "b") == 5);
        xassert(xset_elems_size(set, "c", "c") == 5);

        xset_free(&set);
    }

    /* xset_put_unique */
    {
        XSet_PT set = xset_new(test_cmpk, NULL);
        xset_put_unique(set, "a");
        xset_put_unique(set, "b");
        xset_put_unique(set, "c");
        xset_put_unique(set, "a");
        xset_put_unique(set, "b");
        xset_put_unique(set, "c");
        xset_put_unique(set, "a");
        xset_put_unique(set, "b");
        xset_put_unique(set, "c");
        xset_put_unique(set, "a");
        xset_put_unique(set, "b");
        xset_put_unique(set, "c");
        xset_put_unique(set, "a");
        xset_put_unique(set, "b");
        xset_put_unique(set, "c");

        xassert(xset_elems_size(set, "a", "a") == 1);
        xassert(xset_elems_size(set, "b", "b") == 1);
        xassert(xset_elems_size(set, "c", "c") == 1);

        xset_free(&set);
    }

    /* xset_key_unique */
    {
        XSet_PT set = xset_new(test_cmpk, NULL);
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");

        xset_elem_unique(set, "a");
        xset_elem_unique(set, "b");
        xset_elem_unique(set, "c");

        xassert(xset_elems_size(set, "a", "a") == 1);
        xassert(xset_elems_size(set, "b", "b") == 1);
        xassert(xset_elems_size(set, "c", "c") == 1);

        xset_free(&set);
    }

    /* xset_unique */
    {
        XSet_PT set = xset_new(test_cmpk, NULL);
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");

        xset_unique(set);

        xassert(xset_elems_size(set, "a", "a") == 1);
        xassert(xset_elems_size(set, "b", "b") == 1);
        xassert(xset_elems_size(set, "c", "c") == 1);

        xset_free(&set);
    }

    {
        XSet_PT tree = xset_new(test_cmpk, NULL);

        xassert(xset_put_repeat(tree, "a"));
        xassert(xset_put_repeat(tree, "b"));
        xassert(xset_put_repeat(tree, "c"));
        xassert(xset_put_repeat(tree, "d"));
        xassert(xset_put_repeat(tree, "e"));
        xassert(xset_put_repeat(tree, "f"));
        xassert(xset_put_repeat(tree, "g"));
        xassert(xset_put_repeat(tree, "h"));
        xassert(xset_put_repeat(tree, "i"));
        xassert(xset_put_repeat(tree, "j"));
        xassert(xset_put_repeat(tree, "k"));
        xassert(xset_put_repeat(tree, "l"));
        xassert(xset_put_repeat(tree, "m"));
        xassert(xset_put_repeat(tree, "n"));
        xassert(xset_put_repeat(tree, "p"));
        xassert(xset_put_repeat(tree, "q"));
        xassert(xset_put_repeat(tree, "r"));
        xassert(xset_put_repeat(tree, "t"));
        xassert(xset_put_repeat(tree, "u"));
        xassert(xset_put_repeat(tree, "v"));
        xassert(xset_put_repeat(tree, "w"));
        xassert(xset_put_repeat(tree, "x"));
        xassert(xset_put_repeat(tree, "y"));
        xassert(xset_put_repeat(tree, "z"));
        
        /* xset_select */
        xassert(strcmp(xset_select(tree, 0), "a") == 0);
        xassert(strcmp(xset_select(tree, 5), "f") == 0);
        xassert(strcmp(xset_select(tree, 15), "q") == 0);
        xassert(strcmp(xset_select(tree, 23), "z") == 0);        

        /* xset_find */
        xassert_false(xset_find(tree, "s"));
        xassert(xset_find(tree, "y"));

        xassert(xset_put_repeat(tree, "g"));
        xassert(xset_put_repeat(tree, "k"));
        xassert(xset_put_repeat(tree, "g"));
        xassert(xset_put_repeat(tree, "k"));
        xassert(xset_put_repeat(tree, "g"));

        /* xset_keys */
        {
            {
                XSList_PT list = xset_elems(tree, "g", "r");
                xassert(xslist_size(list) == 16);
                xslist_free(&list);
            }

            {
                XSList_PT list = xset_elems(tree, "r", "g");
                xassert(xslist_size(list) == 16);
                xslist_free(&list);
            }
        }

        /* xset_clear */
        xset_clear(tree);
        xset_free(&tree);
    }

    /* xset_deep_clear */
    {
        XSet_PT set = xset_random_string(test_cmpk, NULL, 50, 3);
        xset_deep_clear(set);
        xassert(xset_size(set) == 0);
        xset_free(&set);
    }

    /* xset_remove */
    /* xset_remove_all */
    {
        XSet_PT set = xset_new(test_cmpk, NULL);
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");
        xset_put_repeat(set, "a");
        xset_put_repeat(set, "b");
        xset_put_repeat(set, "c");

        xset_remove(set, "a");
        xassert(xset_elems_size(set, "a", "a") == 4);
        xset_remove(set, "a");
        xassert(xset_elems_size(set, "a", "a") == 3);
        xset_remove(set, "a");
        xassert(xset_elems_size(set, "a", "a") == 2);
        xset_remove(set, "a");
        xassert(xset_elems_size(set, "a", "a") == 1);
        xset_remove(set, "a");
        xassert(xset_elems_size(set, "a", "a") == 0);
        xset_remove(set, "a");
        xassert(xset_elems_size(set, "a", "a") == 0);

        xset_remove_all(set, "b");
        xassert(xset_elems_size(set, "b", "b") == 0);

        xset_free(&set);
    }

    /* xset_deep_remove */
    /* xset_deep_remove_all */
    {
        XSet_PT set = xset_random_string(test_cmpk, NULL, 500, 2);
        xset_deep_remove(set, "0");
        xset_deep_remove(set, "1");
        xset_deep_remove(set, "2");
        xset_deep_remove(set, "3");

        xset_deep_remove_all(set, "4");
        xset_deep_remove_all(set, "5");
        xset_deep_remove_all(set, "6");
        xset_deep_remove_all(set, "7");
        xassert(xset_elems_size(set, "4", "4") == 0);
        xassert(xset_elems_size(set, "5", "5") == 0);
        xassert(xset_elems_size(set, "6", "6") == 0);
        xassert(xset_elems_size(set, "7", "7") == 0);

        xset_deep_free(&set);
    }

    {
        XSet_PT tree = xset_random_string(test_cmpk, NULL, 10, 3);

        //xset_map_min_to_max(tree, xset_test_map_applykv, NULL);
        //xset_map_max_to_min(tree, xset_test_map_applykv, NULL);

        xset_deep_free(&tree);
    }

    {
        {
            XSet_PT tree = xset_new(test_cmpk, NULL);

            /* xset_map_min_to_max_break_if_true */
            xassert_false(xset_map_break_if_true(tree, xset_test_map_cmp, NULL));

            /* xset_map_min_to_max_break_if_false */
            xassert_false(xset_map_break_if_false(tree, xset_test_map_cmp, NULL));

            xset_free(&tree);
        }

        {
            XSet_PT tree = xset_new(test_cmpk, NULL);

            xassert(xset_put_repeat(tree, "a"));
            xassert(xset_put_repeat(tree, "b"));
            xassert(xset_put_repeat(tree, "c"));
            xassert(xset_put_repeat(tree, "d"));
            xassert(xset_put_repeat(tree, "e"));

            /* xset_map_min_to_max_break_if_true */
            xassert_false(xset_map_break_if_true(tree, xset_test_map_apply_false, NULL));
            xassert_false(xset_map_break_if_true(tree, xset_test_map_cmp, "x"));
            xassert(xset_map_break_if_true(tree, xset_test_map_cmp, "a"));

            /* xset_map_min_to_max_break_if_false */
            xassert_false(xset_map_break_if_false(tree, xset_test_map_apply_true, NULL));
            xassert(xset_map_break_if_false(tree, xset_test_map_cmp, "a"));
            xassert(xset_map_break_if_false(tree, xset_test_map_cmp, "b"));

            xset_free(&tree);
        }
    }

    /* xset_swap */
    {
    }

    /* xset_merge_repeat */
    {
        XSet_PT tree1 = xset_new(test_cmpk, NULL);
        XSet_PT tree2 = xset_new(test_cmpk, NULL);

        xassert(xset_put_repeat(tree1, "a"));
        xassert(xset_put_repeat(tree1, "b"));
        xassert(xset_put_repeat(tree1, "c"));
        xassert(xset_put_repeat(tree1, "d"));
        xassert(xset_put_repeat(tree1, "e"));
        xassert(xset_put_repeat(tree1, "f"));
        xassert(xset_put_repeat(tree1, "g"));

        xassert(xset_put_repeat(tree1, "a"));
        xassert(xset_put_repeat(tree1, "b"));
        xassert(xset_put_repeat(tree1, "c"));
        xassert(xset_put_repeat(tree1, "d"));
        xassert(xset_put_repeat(tree1, "e"));
        xassert(xset_put_repeat(tree1, "f"));
        xassert(xset_put_repeat(tree1, "g"));

        xassert(xset_put_repeat(tree2, "a"));
        xassert(xset_put_repeat(tree2, "b"));
        xassert(xset_put_repeat(tree2, "c"));
        xassert(xset_put_repeat(tree2, "d"));
        xassert(xset_put_repeat(tree2, "e"));
        xassert(xset_put_repeat(tree2, "f"));
        xassert(xset_put_repeat(tree2, "g"));

        xset_merge_repeat(tree1, &tree2);

        xassert(xset_size(tree1) == 21);

        xassert(xset_elems_size(tree1, "a", "a") == 3);
        xassert(xset_elems_size(tree1, "c", "c") == 3);
        xassert(xset_elems_size(tree1, "e", "e") == 3);
        xassert(xset_elems_size(tree1, "g", "g") == 3);
        //xset_print(tree1, 2);

        xset_free(&tree1);
    }

    /* xset_merge_unique */
    {
        XSet_PT tree1 = xset_new(test_cmpk, NULL);
        XSet_PT tree2 = xset_new(test_cmpk, NULL);

        xassert(xset_put_repeat(tree1, "a"));
        xassert(xset_put_repeat(tree1, "b"));
        xassert(xset_put_repeat(tree1, "c"));
        xassert(xset_put_repeat(tree1, "d"));
        xassert(xset_put_repeat(tree1, "e"));
        xassert(xset_put_repeat(tree1, "f"));
        xassert(xset_put_repeat(tree1, "g"));
        
        xassert(xset_put_repeat(tree2, "a"));
        xassert(xset_put_repeat(tree2, "b"));
        xassert(xset_put_repeat(tree2, "c"));
        xassert(xset_put_repeat(tree2, "d"));
        xassert(xset_put_repeat(tree2, "e"));
        xassert(xset_put_repeat(tree2, "f"));
        xassert(xset_put_repeat(tree2, "g"));
        xassert(xset_put_repeat(tree2, "h"));

        xassert(xset_put_repeat(tree2, "a"));
        xassert(xset_put_repeat(tree2, "b"));
        xassert(xset_put_repeat(tree2, "c"));
        xassert(xset_put_repeat(tree2, "d"));
        xassert(xset_put_repeat(tree2, "e"));
        xassert(xset_put_repeat(tree2, "f"));
        xassert(xset_put_repeat(tree2, "g"));

        xset_merge_unique(tree1, &tree2);

        xassert(xset_size(tree1) == 8);

        xassert(xset_elems_size(tree1, "a", "a") == 1);
        xassert(xset_elems_size(tree1, "c", "c") == 1);
        xassert(xset_elems_size(tree1, "e", "e") == 1);
        xassert(xset_elems_size(tree1, "g", "g") == 1);
        //xset_print(tree1, 2);

        xset_free(&tree1);
    }

    /* xset_elems_size */
    {
    }

    /* xset_size */
    {
    }

    /* xset_is_empty */
    {
    }

    /* xset_height */
    {
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
