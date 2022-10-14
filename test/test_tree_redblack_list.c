
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../list_s_raw/xlist_s_raw_x.h"
#include "../tree_redblack_list/xtree_redblack_list_x.h"
#include "../include/xalgos.h"

static
int test_cmpk(void *key1, void *key2, void *cl) {
    return strcmp((char*)key1, (char*)key2);
}

static
bool xlistrbtree_test_map_applykv_false(void *key, void **value, void *cl) {
    return false;
}

static
bool xlistrbtree_test_map_applykv_true(void *key, void **value, void *cl) {
    return true;
}

static
bool xlistrbtree_test_map_applyk_false(void *key, void *cl) {
    return false;
}

static
bool xlistrbtree_test_map_applyk_true(void *key, void *cl) {
    return true;
}

static
bool xlistrbtree_test_map_cmpkv(void *key, void **value, void *cl) {
    return strcmp((char*)key, (char*)cl) == 0;
}

static
bool xlistrbtree_test_map_cmpk(void *key, void *cl) {
    return strcmp((char*)key, (char*)cl) == 0;
}

static
bool xlistrbtree_test_map_applykv(void *key, void **value, void *cl) {
    printf("%s\n", (char*)key);
    return true;
}

static
bool xlistrbtree_test_map_applyk(void *key, void *cl) {
    printf("%s\n", (char*)key);
    return true;
}

static
XListRBTree_PT xlistrbtree_random_string(int(*cmp)(void *key1, void *key2, void *cl), void *cl, int total_size, int string_length) {
    xassert(cmp);
    xassert(0 < total_size);
    xassert(0 < string_length);

    if (!cmp || (total_size <= 0) || (string_length <= 0)) {
        return NULL;
    }

    {
        XListRBTree_PT tree = xlistrbtree_new(cmp, cl);
        if (!tree) {
            return NULL;
        }

        {
            const char charsets[] = "0123456789";

            for (int i = 0; i < total_size; i++) {
                char* str = XMEM_MALLOC(string_length + 1);
                if (!str) {
                    xlistrbtree_deep_free(&tree);
                    return NULL;
                }

                for (int i = 0; i < string_length; ++i) {
                    str[i] = charsets[rand() % (sizeof(charsets) - 1)];
                }
                str[string_length] = '\0';

                if (xlistrbtree_get(tree, str)) {
                    XMEM_FREE(str);
                    i--;
                    continue;
                }

                char *value = XMEM_MALLOC(string_length + 1);
                memcpy(value, str, string_length + 1);

                if (!xlistrbtree_put_repeat(tree, str, value)) {
                    xlistrbtree_deep_free(&tree);
                    return NULL;
                }
            }
        }

        return tree;
    }
}

static
void xlistrbtree_printnode(char* key, char* value, bool color, int h) {
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
void xlistrbtree_print_impl(XListRBTree_Node_PT node, int h) {
    if (!node) {
        xlistrbtree_printnode("-", "-", true, h);
        return;
    }

    xlistrbtree_print_impl(node->right, h + 3);

    xlistrbtree_printnode((char*)node->key, (char*)node->values->value, node->color, h);

    xlistrbtree_print_impl(node->left, h + 3);
}

static
void xlistrbtree_print(XListRBTree_PT tree, int start_blank) {
    xassert(tree);
    xassert(0 <= start_blank);

    if (start_blank < 0) {
        return;
    }

    if (!tree) {
        xlistrbtree_printnode("-", "-", false, start_blank);
        return;
    }

    xlistrbtree_print_impl(tree->root, start_blank + 3);
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

void test_xlistrbtree() {
    /* xlistrbtree_new */
    {
        /* cmp == NULL */
        {
            bool except = false;

            XEXCEPT_TRY
                xlistrbtree_new(NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* normal */
        {
            XListRBTree_PT tree = NULL;

            tree = xlistrbtree_new(test_cmpk, NULL);

            xassert(tree);
            xassert(tree->cmp);
            xassert_false(tree->root);

            xassert(xlistrbtree_size(tree) == 0);

            xlistrbtree_free(&tree);
        }
    }

    /* xlistrbtree_copy */
    {
        XListRBTree_PT tree = xlistrbtree_random_string(test_cmpk, NULL, 50, 3);
        XListRBTree_PT ntree = xlistrbtree_copy(tree);

        xassert(xlistrbtree_is_rbtree(tree));
        xassert(xlistrbtree_is_rbtree(ntree));

        xlistrbtree_deep_free(&tree);
        xlistrbtree_free(&ntree);
    }

    /* xlistrbtree_deep_copy */
    {
        XListRBTree_PT tree = xlistrbtree_random_string(test_cmpk, NULL, 50, 3);
        XListRBTree_PT ntree = xlistrbtree_deep_copy(tree, 4, 4);

        xassert(xlistrbtree_is_rbtree(tree));
        xassert(xlistrbtree_is_rbtree(ntree));

        xlistrbtree_deep_free(&tree);
        xlistrbtree_deep_free(&ntree);
    }

    /* xlistrbtree_put_repeat */
    {
        /* tree == NULL */
        {
            bool except = false;

            XEXCEPT_TRY
                xlistrbtree_put_repeat(NULL, "abc", "def");
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* key == NULL */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            bool except = false;

            XEXCEPT_TRY
                xlistrbtree_put_repeat(tree, NULL, "def");
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xlistrbtree_free(&tree);
        }

        /* add one (key, value) pair :
        *
        *         5
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 1);
            xassert_false(tree->root->parent);
            xassert_false(tree->root->left);
            xassert_false(tree->root->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add two (key, value) pairs :
        *     1. second key is less than first key
        *
        *             5
        *   5  ->    /
        *          3(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->right);
            xassert_false(tree->root->parent);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->values->value, "3v") == 0);
            xassert(tree->root->left->color == false);
            xassert(tree->root->left->size == 1);
            xassert(strcmp(tree->root->left->parent->key, "5") == 0);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add two (key, value) pairs :
        *     2. second key is larger than first key
        *
        *            5 
        *   5  ->     \
        *             8(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->parent);
            xassert_false(tree->root->left);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == false);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add three (key, value) pairs :
        *     1. 1 is less than 3
        *
        *             5            5        3            3(R)         3
        *   5  ->    /     ->    /    ->   /  \     ->   /  \   ->   / \
        *          3(R)        3(R)      1(R) 5(R)      1    5      1   5
        *                      /
        *                    1(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));

            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->values->value, "3v") == 0);
            xassert_false(tree->root->parent);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->values->value, "1v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert(strcmp(tree->root->left->parent->key, "3") == 0);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "5") == 0);
            xassert(strcmp(tree->root->right->values->value, "5v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert(strcmp(tree->root->right->parent->key, "3") == 0);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add three (key, value) pairs :
        *     2.  4 is bigger than 3
        *
        *             5            5          5          4            4
        *   5  ->    /     ->    /    ->     /    ->   /   \    ->   / \
        *          3(R)        3(R)        4(R)      3(R)  5(R)     3   5
        *                        \         /
        *                       4(R)     3(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "4", "4v"));

            xassert(strcmp(tree->root->key, "4") == 0);
            xassert(strcmp(tree->root->values->value, "4v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->values->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "5") == 0);
            xassert(strcmp(tree->root->right->values->value, "5v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add three (key, value) pairs :
        *     1. 6 is less than 8
        *
        *             5            5           5             6          6(R)       6
        *   5  ->      \     ->     \    ->     \     ->   /   \   ->   /  \  ->  / \ 
        *             8(R)         8(R)        6(R)      5(R)  8(R)    5    8    5   8
        *                          /              \   
        *                        6(R)            8(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "6", "6v"));

            xassert(strcmp(tree->root->key, "6") == 0);
            xassert(strcmp(tree->root->values->value, "6v") == 0);
            xassert_false(tree->root->parent);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "5") == 0);
            xassert(strcmp(tree->root->left->values->value, "5v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert(strcmp(tree->root->left->parent->key, "6") == 0);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert(strcmp(tree->root->right->parent->key, "6") == 0);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add three (key, value) pairs :
        *     1. 9 is greater than 8
        *
        *             5            5              8          8(R)       8
        *   5  ->      \     ->     \    ->     /   \   ->   /  \  ->  / \
        *             8(R)         8(R)       5(R)  9(R)    5    9    5   9
        *                             \
        *                            9(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "9", "9v"));

            xassert(strcmp(tree->root->key, "8") == 0);
            xassert(strcmp(tree->root->values->value, "8v") == 0);
            xassert_false(tree->root->parent);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "5") == 0);
            xassert(strcmp(tree->root->left->values->value, "5v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert(strcmp(tree->root->left->parent->key, "8") == 0);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->values->value, "9v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert(strcmp(tree->root->right->parent->key, "8") == 0);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add three (key, value) pairs :
        *     3. 8 is bigger than 5
        *
        *             5            5             5
        *   5  ->    /     ->    /   \     ->   / \
        *          3(R)        3(R)  8(R)      3   8
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->values->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add four (key, value) pairs :
        *     1. 1 is less than 3
        *
        *             5            5
        *   5  ->    / \     ->   / \
        *           3   8        3   8
        *                       /
        *                     1(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->values->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->values->value, "1v") == 0);
            xassert(tree->root->left->left->color == false);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xlistrbtree_free(&tree);
        }

        /* add four (key, value) pairs :
        *     2. 4 is bigger than 3
        *
        *             5            5
        *   5  ->    / \     ->   / \
        *           3   8        3   8
        *                         \
        *                        4(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "4", "4v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->values->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->left);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->right->key, "4") == 0);
            xassert(strcmp(tree->root->left->right->values->value, "4v") == 0);
            xassert(tree->root->left->right->color == false);
            xassert(tree->root->left->right->size == 1);
            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add four (key, value) pairs :
        *     3. 6 is less than 8
        *
        *             5            5
        *   5  ->    / \     ->   / \
        *           3   8        3   8
        *                           /
        *                         6(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "6", "6v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->values->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->right->left->key, "6") == 0);
            xassert(strcmp(tree->root->right->left->values->value, "6v") == 0);
            xassert(tree->root->right->left->color == false);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add four (key, value) pairs :
        *     4. 9 is bigger than 8
        *
        *             5            5
        *   5  ->    / \     ->   / \
        *           3   8        3   8
        *                             \
        *                            9(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "9", "9v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->values->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->left);

            xassert(strcmp(tree->root->right->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->right->values->value, "9v") == 0);
            xassert(tree->root->right->right->color == false);
            xassert(tree->root->right->right->size == 1);
            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add five (key, value) pairs :
        *     1. 0 is less than 1
        *
        *             5            5            5            5             5
        *   5  ->    / \     ->   / \     ->   / \     ->  /   \    ->   /   \
        *           3   8        3   8        3   8       1     8      1(R)   8
        *                       /            /          /   \          /  \
        *                     1(R)          1(R)      0(R)  3(R)      0    3
        *                                  /
        *                                0(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
            xassert(xlistrbtree_put_repeat(tree, "0", "0v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 5);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->values->value, "1v") == 0);
            xassert(tree->root->left->color == false);
            xassert(tree->root->left->size == 3);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "0") == 0);
            xassert(strcmp(tree->root->left->left->values->value, "0v") == 0);
            xassert(tree->root->left->left->color == true);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "3") == 0);
            xassert(strcmp(tree->root->left->right->values->value, "3v") == 0);
            xassert(tree->root->left->right->color == true);
            xassert(tree->root->left->right->size == 1);
            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add five (key, value) pairs :
        *     2. 15 is bigger than 1
        *
        *             5            5            5            5             5             5
        *   5  ->    / \     ->   / \     ->   / \     ->  /   \    ->   /   \     ->  /   \
        *           3   8        3   8        3   8       3     8      15     8      15(R)  8
        *                       /            /           /            /  \           / \
        *                     1(R)          1(R)       15(R)        1(R) 3(R)       1   3
        *                                    \         /
        *                                    15(R)   1(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
            xassert(xlistrbtree_put_repeat(tree, "15", "15v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 5);

            xassert(strcmp(tree->root->left->key, "15") == 0);
            xassert(strcmp(tree->root->left->values->value, "15v") == 0);
            xassert(tree->root->left->color == false);
            xassert(tree->root->left->size == 3);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->values->value, "1v") == 0);
            xassert(tree->root->left->left->color == true);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "3") == 0);
            xassert(strcmp(tree->root->left->right->values->value, "3v") == 0);
            xassert(tree->root->left->right->color == true);
            xassert(tree->root->left->right->size == 1);
            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add five (key, value) pairs :
        *     6 is less than 7
        *
        *             5            5            5            5             5
        *   5  ->    / \     ->   / \     ->   / \     ->  /   \    ->   /   \
        *           3   8        3   8        3   8       3     7       3    7(R)
        *                           /            /            /   \          /  \
        *                         7(R)         7(R)         6(R)  8(R)      6    8
        *                                      /
        *                                    6(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "7", "7v"));
            xassert(xlistrbtree_put_repeat(tree, "6", "6v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 5);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->values->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);

            xassert(strcmp(tree->root->right->key, "7") == 0);
            xassert(strcmp(tree->root->right->values->value, "7v") == 0);
            xassert(tree->root->right->color == false);
            xassert(tree->root->right->size == 3);

            xassert(strcmp(tree->root->right->left->key, "6") == 0);
            xassert(strcmp(tree->root->right->left->values->value, "6v") == 0);
            xassert(tree->root->right->left->color == true);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(strcmp(tree->root->right->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->right->values->value, "8v") == 0);
            xassert(tree->root->right->right->color == true);
            xassert(tree->root->right->right->size == 1);
            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add five (key, value) pairs :
        *     7 is greater than 6
        *
        *             5            5            5            5             5
        *   5  ->    / \     ->   / \     ->   / \     ->  /   \    ->   /   \
        *           3   8        3   8        3   8       3     7       3    7(R)
        *                           /            /            /   \          /  \
        *                         6(R)         6(R)         6(R)  8(R)      6    8
        *                                        \
        *                                       7(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "6", "6v"));
            xassert(xlistrbtree_put_repeat(tree, "7", "7v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 5);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->values->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);

            xassert(strcmp(tree->root->right->key, "7") == 0);
            xassert(strcmp(tree->root->right->values->value, "7v") == 0);
            xassert(tree->root->right->color == false);
            xassert(tree->root->right->size == 3);

            xassert(strcmp(tree->root->right->left->key, "6") == 0);
            xassert(strcmp(tree->root->right->left->values->value, "6v") == 0);
            xassert(tree->root->right->left->color == true);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(strcmp(tree->root->right->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->right->values->value, "8v") == 0);
            xassert(tree->root->right->right->color == true);
            xassert(tree->root->right->right->size == 1);
            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add five (key, value) pairs :
        *     3. 4 is bigger than 3
        *
        *             5            5            5            5
        *   5  ->    / \     ->   / \     ->   / \     ->   / \
        *           3   8        3   8        3   8       3(R) 8
        *                       /            /  \         / \
        *                     1(R)         1(R) 4(R)     1   4
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
            xassert(xlistrbtree_put_repeat(tree, "4", "4v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 5);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->values->value, "3v") == 0);
            xassert(tree->root->left->color == false);
            xassert(tree->root->left->size == 3);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->values->value, "1v") == 0);
            xassert(tree->root->left->left->color == true);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "4") == 0);
            xassert(strcmp(tree->root->left->right->values->value, "4v") == 0);
            xassert(tree->root->left->right->color == true);
            xassert(tree->root->left->right->size == 1);
            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add five (key, value) pairs :
        *     4. 6 is less than 8
        *
        *             5            5            5
        *   5  ->    / \     ->   / \     ->   / \
        *           3   8        3   8        3   8
        *                       /            /    /
        *                     1(R)         1(R) 6(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
            xassert(xlistrbtree_put_repeat(tree, "6", "6v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 5);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->values->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->values->value, "1v") == 0);
            xassert(tree->root->left->left->color == false);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->right->left->key, "6") == 0);
            xassert(strcmp(tree->root->right->left->values->value, "6v") == 0);
            xassert(tree->root->right->left->color == false);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add five (key, value) pairs :
        *     5. 9 is bigger than 8
        *
        *             5            5            5
        *   5  ->    / \     ->   / \     ->   / \
        *           3   8        3   8        3   8
        *                       /            /     \
        *                     1(R)         1(R)   9(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
            xassert(xlistrbtree_put_repeat(tree, "9", "9v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 5);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->values->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->left);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->values->value, "1v") == 0);
            xassert(tree->root->left->left->color == false);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->right->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->right->values->value, "9v") == 0);
            xassert(tree->root->right->right->color == false);
            xassert(tree->root->right->right->size == 1);
            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add more (key, value) pairs :
        *     1. 4 is bigger than 3
        *     2. 6 is lesser than 8
        *
        *             5            5            5               5                  5
        *   5  ->    / \     ->   / \     ->   / \    ->    /       \     ->    /     \     ->
        *           3   8        3   8        3   8        3         8        3(R)     8
        *                       /            /     \     /   \        \       /  \      \
        *                     1(R)         1(R)   9(R) 1(R)  4(R)    9(R)    1    4     9(R)
        *
        *        5                      5                5(R)              5
        *     /     \                /     \            /    \           /    \
        *    3(R)    8        ->   3(R)    8(R)    ->  3      8    ->   3      8
        *   / \    /   \          /  \     /  \       / \    / \       / \    / \
        *  1   4  6(R) 9(R)      1    4   6    9     1   4  6   9     1   4  6   9
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
            xassert(xlistrbtree_put_repeat(tree, "9", "9v"));
            xassert(xlistrbtree_put_repeat(tree, "4", "4v"));
            xassert(xlistrbtree_put_repeat(tree, "6", "6v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 7);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->values->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 3);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 3);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->values->value, "1v") == 0);
            xassert(tree->root->left->left->color == true);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "4") == 0);
            xassert(strcmp(tree->root->left->right->values->value, "4v") == 0);
            xassert(tree->root->left->right->color == true);
            xassert(tree->root->left->right->size == 1);
            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(strcmp(tree->root->right->left->key, "6") == 0);
            xassert(strcmp(tree->root->right->left->values->value, "6v") == 0);
            xassert(tree->root->right->left->color == true);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(strcmp(tree->root->right->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->right->values->value, "9v") == 0);
            xassert(tree->root->right->right->color == true);
            xassert(tree->root->right->right->size == 1);
            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* add more (key, value) pairs :
        *          9
        *        /   \
        *       5(R) 95
        *      / \
        *     3   8
        *    /
        *  1(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "9", "9v"));
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "95", "95v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));

            xassert(strcmp(tree->root->key, "9") == 0);
            xassert(strcmp(tree->root->values->value, "9v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 6);

            xassert(strcmp(tree->root->left->key, "5") == 0);
            xassert(strcmp(tree->root->left->values->value, "5v") == 0);
            xassert(tree->root->left->color == false);
            xassert(tree->root->left->size == 4);

            xassert(strcmp(tree->root->right->key, "95") == 0);
            xassert(strcmp(tree->root->right->values->value, "95v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);

            xassert(strcmp(tree->root->left->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->left->values->value, "3v") == 0);
            xassert(tree->root->left->left->color == true);
            xassert(tree->root->left->left->size == 2);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "8") == 0);
            xassert(strcmp(tree->root->left->right->values->value, "8v") == 0);
            xassert(tree->root->left->right->color == true);
            xassert(tree->root->left->right->size == 1);
            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(strcmp(tree->root->left->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->left->values->value, "1v") == 0);
            xassert(tree->root->left->left->left->color == false);
            xassert(tree->root->left->left->left->size == 1);
            xassert_false(tree->root->left->left->left->left);
            xassert_false(tree->root->left->left->left->right);

            /* add more (key, value) pairs :
            *     4. 4 is bigger than 3
            *
            *          9              9             9                5                  5(R)               5  
            *        /   \          /   \         /   \           /      \            /     \           /     \
            *       5(R) 95   ->  5(R)  95  ->  5(R)   95   ->  3(R)     9(R)   ->   3       9     ->  3       9 
            *      / \            /  \          /  \            /  \     /  \       / \     / \       / \     / \
            *     3   8          3    8       3(R)  8          1    4   8   95     1   4   8   95    1   4   8   95
            *    /             /   \          / \
            *  1(R)          1(R)  4(R)      1   4 
            */
            xassert(xlistrbtree_put_repeat(tree, "4", "4v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 7);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->values->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 3);

            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->values->value, "9v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 3);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->values->value, "1v") == 0);
            xassert(tree->root->left->left->color == true);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "4") == 0);
            xassert(strcmp(tree->root->left->right->values->value, "4v") == 0);
            xassert(tree->root->left->right->color == true);
            xassert(tree->root->left->right->size == 1);
            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(strcmp(tree->root->right->left->key, "8") == 0);
            xassert(strcmp(tree->root->right->left->values->value, "8v") == 0);
            xassert(tree->root->right->left->color == true);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(strcmp(tree->root->right->right->key, "95") == 0);
            xassert(strcmp(tree->root->right->right->values->value, "95v") == 0);
            xassert(tree->root->right->right->color == true);
            xassert(tree->root->right->right->size == 1);
            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }
        
        /* add many more (key, value) pairs : */
        {
            XListRBTree_PT tree = xlistrbtree_random_string(test_cmpk, NULL, 5000, 10);
            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_deep_free(&tree);
        }
    }

    /* xlistrbtree_put_unique */
    {
        XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);

        xassert(xlistrbtree_put_unique(tree, "1", NULL));
        xassert(xlistrbtree_size(tree) == 1);

        xassert(xlistrbtree_put_unique(tree, "1", NULL));
        xassert(xlistrbtree_put_unique(tree, "1", NULL));
        xassert(xlistrbtree_put_unique(tree, "1", NULL));
        xassert(xlistrbtree_put_unique(tree, "1", NULL));
        xassert(xlistrbtree_size(tree) == 1);

        xassert(xlistrbtree_put_unique(tree, "2", NULL));
        xassert(xlistrbtree_put_unique(tree, "3", NULL));
        xassert(xlistrbtree_put_unique(tree, "2", NULL));
        xassert(xlistrbtree_size(tree) == 3);

        xassert(xlistrbtree_put_unique(tree, "7", NULL));
        xassert(xlistrbtree_put_unique(tree, "4", NULL));
        xassert(xlistrbtree_put_unique(tree, "5", NULL));
        xassert(xlistrbtree_put_unique(tree, "6", NULL));
        xassert(xlistrbtree_put_unique(tree, "7", NULL));
        xassert(xlistrbtree_put_unique(tree, "5", NULL));
        xassert(xlistrbtree_put_unique(tree, "8", NULL));
        xassert(xlistrbtree_put_unique(tree, "7", NULL));
        xassert(xlistrbtree_put_unique(tree, "5", NULL));
        xassert(xlistrbtree_put_unique(tree, "9", NULL));
        xassert(xlistrbtree_size(tree) == 9);

        xassert(xlistrbtree_is_rbtree(tree));
        xlistrbtree_free(&tree);
    }

    /* xlistrbtree_put_replace */
    {
        XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);

        xassert(xlistrbtree_put_replace(tree, "1", NULL, NULL));
        xassert(xlistrbtree_size(tree) == 1);

        xassert(xlistrbtree_put_replace(tree, "1", "2", NULL));
        xassert(xlistrbtree_put_replace(tree, "1", "3", NULL));
        xassert(xlistrbtree_put_replace(tree, "1", "2", NULL));
        xassert(xlistrbtree_put_replace(tree, "1", "4", NULL));
        xassert(xlistrbtree_size(tree) == 1);
        xassert(strcmp(xlistrbtree_get(tree, "1"), "4") == 0);

        xassert(xlistrbtree_put_replace(tree, "2", "2", NULL));
        xassert(xlistrbtree_put_replace(tree, "3", NULL, NULL));
        xassert(xlistrbtree_put_replace(tree, "2", "1", NULL));
        xassert(strcmp(xlistrbtree_get(tree, "2"), "1") == 0);
        xassert(xlistrbtree_size(tree) == 3);

        xassert(xlistrbtree_put_replace(tree, "4", NULL, NULL));
        xassert(xlistrbtree_put_replace(tree, "5", NULL, NULL));
        xassert(xlistrbtree_put_replace(tree, "6", NULL, NULL));
        xassert(xlistrbtree_put_replace(tree, "7", NULL, NULL));
        xassert(xlistrbtree_size(tree) == 7);
        xassert(xlistrbtree_put_replace(tree, "2", "3", NULL));
        xassert(xlistrbtree_size(tree) == 7);
        xassert(xlistrbtree_put_replace(tree, "8", NULL, NULL));
        xassert(xlistrbtree_put_replace(tree, "9", NULL, NULL));
        xassert(xlistrbtree_size(tree) == 9);
        xassert(strcmp(xlistrbtree_get(tree, "2"), "3") == 0);
        xassert(xlistrbtree_size(tree) == 9);

        xassert(xlistrbtree_is_rbtree(tree));
        xlistrbtree_free(&tree);
    }

    /* xlistrbtree_put_deep_replace */
    {
        XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
        char *value1 = XMEM_CALLOC(1, 10);
        char *value2 = XMEM_CALLOC(1, 10);
        char *value3 = XMEM_CALLOC(1, 10);
        memcpy(value1, "1", 1);
        memcpy(value2, "2", 1);
        memcpy(value3, "3", 1);

        xassert(xlistrbtree_put_deep_replace(tree, "1", NULL));
        xassert(xlistrbtree_put_deep_replace(tree, "2", NULL));
        xassert(xlistrbtree_put_deep_replace(tree, "3", NULL));
        xassert(xlistrbtree_put_deep_replace(tree, "4", NULL));
        xassert(xlistrbtree_put_deep_replace(tree, "5", NULL));
        xassert(xlistrbtree_put_deep_replace(tree, "6", NULL));
        xassert(xlistrbtree_put_deep_replace(tree, "7", NULL));
        xassert(xlistrbtree_put_deep_replace(tree, "8", NULL));
        xassert(xlistrbtree_put_deep_replace(tree, "9", NULL));

        xassert(xlistrbtree_put_deep_replace(tree, "1", value1));
        xassert(strcmp(xlistrbtree_get(tree, "1"), "1") == 0);
        xassert(xlistrbtree_put_deep_replace(tree, "1", value2));
        xassert(strcmp(xlistrbtree_get(tree, "1"), "2") == 0);
        xassert(xlistrbtree_put_deep_replace(tree, "1", value3));
        xassert(strcmp(xlistrbtree_get(tree, "1"), "3") == 0);
        xassert(xlistrbtree_put_deep_replace(tree, "1", "5"));
        xassert(strcmp(xlistrbtree_get(tree, "1"), "5") == 0);

        xassert(xlistrbtree_size(tree) == 9);

        xassert(xlistrbtree_is_rbtree(tree));
        xlistrbtree_free(&tree);
    }

    /* xlistrbtree_key_unique */
    {
        XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
        xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
        xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
        xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
        xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
        xassert(xlistrbtree_put_repeat(tree, "9", "9v"));
        xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
        xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
        xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
        xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
        xassert(xlistrbtree_put_repeat(tree, "9", "9v"));
        xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
        xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
        xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
        xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
        xassert(xlistrbtree_put_repeat(tree, "9", "9v"));

        xlistrbtree_key_unique(tree, "5");
        xassert(xlistrbtree_is_rbtree(tree));
        xassert(xlistrbtree_keys_size(tree, "5", "5") == 1);

        xlistrbtree_key_unique(tree, "5");
        xassert(xlistrbtree_is_rbtree(tree));
        xassert(xlistrbtree_keys_size(tree, "3", "3") == 1);

        xlistrbtree_key_unique(tree, "5");
        xassert(xlistrbtree_is_rbtree(tree));
        xassert(xlistrbtree_keys_size(tree, "8", "8") == 1);

        xlistrbtree_free(&tree);
    }

    /* xlistrbtree_unique */
    {
        XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
        xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
        xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
        xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
        xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
        xassert(xlistrbtree_put_repeat(tree, "9", "9v"));
        xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
        xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
        xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
        xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
        xassert(xlistrbtree_put_repeat(tree, "9", "9v"));
        xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
        xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
        xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
        xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
        xassert(xlistrbtree_put_repeat(tree, "9", "9v"));

        xlistrbtree_unique(tree);
        xassert(xlistrbtree_is_rbtree(tree));
        xassert(xlistrbtree_keys_size(tree, "5", "5") == 1);
        xassert(xlistrbtree_keys_size(tree, "3", "3") == 1);
        xassert(xlistrbtree_keys_size(tree, "8", "8") == 1);

        xlistrbtree_free(&tree);
    }

    {
        XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);

        xassert(xlistrbtree_put_repeat(tree, "a", NULL));
        xassert(xlistrbtree_put_repeat(tree, "b", NULL));
        xassert(xlistrbtree_put_repeat(tree, "c", NULL));
        xassert(xlistrbtree_put_repeat(tree, "d", NULL));
        xassert(xlistrbtree_put_repeat(tree, "e", NULL));
        xassert(xlistrbtree_put_repeat(tree, "f", NULL));
        xassert(xlistrbtree_put_repeat(tree, "g", "vg"));
        xassert(xlistrbtree_put_repeat(tree, "h", NULL));
        xassert(xlistrbtree_put_repeat(tree, "i", NULL));
        xassert(xlistrbtree_put_repeat(tree, "j", NULL));
        xassert(xlistrbtree_put_repeat(tree, "k", "vk"));
        xassert(xlistrbtree_put_repeat(tree, "l", NULL));
        xassert(xlistrbtree_put_repeat(tree, "m", NULL));
        xassert(xlistrbtree_put_repeat(tree, "n", NULL));
        xassert(xlistrbtree_put_repeat(tree, "p", NULL));
        xassert(xlistrbtree_put_repeat(tree, "q", NULL));
        xassert(xlistrbtree_put_repeat(tree, "r", "vr"));
        xassert(xlistrbtree_put_repeat(tree, "t", NULL));
        xassert(xlistrbtree_put_repeat(tree, "u", NULL));
        xassert(xlistrbtree_put_repeat(tree, "v", NULL));
        xassert(xlistrbtree_put_repeat(tree, "w", NULL));
        xassert(xlistrbtree_put_repeat(tree, "x", NULL));
        xassert(xlistrbtree_put_repeat(tree, "y", NULL));
        xassert(xlistrbtree_put_repeat(tree, "z", NULL));

        /* xlistrbtree_min */
        xassert(strcmp(xlistrbtree_min(tree), "a") == 0);
        /* xlistrbtree_max */
        xassert(strcmp(xlistrbtree_max(tree), "z") == 0);

        /* xlistrbtree_floor */
        xassert(strcmp(xlistrbtree_floor(tree, "o"), "n") == 0);
        /* xlistrbtree_ceiling */
        xassert(strcmp(xlistrbtree_ceiling(tree, "s"), "t") == 0);

        /* xlistrbtree_select */
        xassert(strcmp(xlistrbtree_select(tree, 0), "a") == 0);
        xassert(strcmp(xlistrbtree_select(tree, 5), "f") == 0);
        xassert(strcmp(xlistrbtree_select(tree, 15), "q") == 0);
        xassert(strcmp(xlistrbtree_select(tree, 23), "z") == 0);

        /* xlistrbtree_rank */
        xassert(xlistrbtree_rank(tree, "e") == 4);
        xassert(xlistrbtree_rank(tree, "m") == 12);
        xassert(xlistrbtree_rank(tree, "y") == 22);

        /* xlistrbtree_get */
        xassert(strcmp(xlistrbtree_get(tree, "g"), "vg") == 0);
        xassert(strcmp(xlistrbtree_get(tree, "k"), "vk") == 0);
        xassert(strcmp(xlistrbtree_get(tree, "r"), "vr") == 0);

        /* xlistrbtree_find */
        xassert_false(xlistrbtree_find(tree, "s"));
        xassert(xlistrbtree_find(tree, "y"));

        xassert(xlistrbtree_put_repeat(tree, "g", "vg2"));
        xassert(xlistrbtree_put_repeat(tree, "k", "vk2"));
        xassert(xlistrbtree_put_repeat(tree, "g", "vg3"));
        xassert(xlistrbtree_put_repeat(tree, "k", "vk3"));
        xassert(xlistrbtree_put_repeat(tree, "g", "vg4"));

        /* xlistrbtree_get_all */
        {
            XSList_PT list = xlistrbtree_get_all(tree, "g");
            xassert(xslist_size(list) == 4);
            xslist_free(&list);
        }

        /* xlistrbtree_keys */
        {
            {
                XSList_PT list = xlistrbtree_keys(tree, "g", "r");
                xassert(xslist_size(list) == 11);
                xslist_free(&list);
            }

            {
                XSList_PT list = xlistrbtree_keys(tree, "r", "g");
                xassert(xslist_size(list) == 11);
                xslist_free(&list);
            }
        }

        xassert(xlistrbtree_is_rbtree(tree));

        /* xlistrbtree_clear */
        xlistrbtree_clear(tree);
        xlistrbtree_free(&tree);
    }

    /* xlistrbtree_remove_min */
    {
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_remove_min(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }

        {
            /*      h
            *      /   ->   h
            *    d(R)
            */
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_put_repeat(tree, "d", NULL);
            xlistrbtree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 1);
            xassert(strcmp(tree->root->key, "h") == 0);
            xassert_false(tree->root->left);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_min(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }

        {
            /*      h
            *        \     ->   m
            *        m(R)
            */
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_put_repeat(tree, "m", NULL);
            xlistrbtree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 1);
            xassert(strcmp(tree->root->key, "m") == 0);
            xassert_false(tree->root->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_min(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }

        {
            /*      h             h          h
            *      / \    ->    /   \    ->   \  
            *     d   m       d(m)  m(R)      m(R)
            */
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_put_repeat(tree, "d", NULL);
            xlistrbtree_put_repeat(tree, "m", NULL);
            xlistrbtree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->left);
            xassert_false(tree->root->right->color);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }

        {
            /*      h          h
            *      / \        / \
            *     d   m   -> d   m
            *    /
            *   a(R)
            */
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_put_repeat(tree, "d", NULL);
            xlistrbtree_put_repeat(tree, "m", NULL);
            xlistrbtree_put_repeat(tree, "a", NULL);
            xlistrbtree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 3);
            xassert(tree->root->left->size == 1);
            xassert(tree->root->left->color);
            xassert(tree->root->right->color);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }

        {
            /*      h            h
            *      / \         /   \
            *     d   m   ->  f     m
            *      \
            *     f(R)
            */
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_put_repeat(tree, "d", NULL);
            xlistrbtree_put_repeat(tree, "m", NULL);
            xlistrbtree_put_repeat(tree, "f", NULL);
            xlistrbtree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 3);
            xassert(tree->root->left->size == 1);
            xassert(tree->root->left->color);
            xassert(tree->root->right->color);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }

        {
            /*       h              h             h
            *      /  \           /   \         /   \
            *    d(R)  m   ->    d     m   ->  d     m
            *    /  \          /   \            \
            *   a    f       a(R)  f(R)         f(R) 
            */
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_put_repeat(tree, "d", NULL);
            xlistrbtree_put_repeat(tree, "m", NULL);
            xlistrbtree_put_repeat(tree, "a", NULL);
            xlistrbtree_put_repeat(tree, "f", NULL);
            xlistrbtree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 4);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->left);
            xassert(tree->root->left->color);
            xassert_false(tree->root->left->right->color);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }

        {
            /*        h              h
            *       /  \           /  \ 
            *     d(R)  m  ->    d(R)  m
            *     /  \           /  \
            *    b    f         b    f
            *   /
            *  a(R)
            */
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_put_repeat(tree, "d", NULL);
            xlistrbtree_put_repeat(tree, "m", NULL);
            xlistrbtree_put_repeat(tree, "b", NULL);
            xlistrbtree_put_repeat(tree, "f", NULL);
            xlistrbtree_put_repeat(tree, "a", NULL);
            xlistrbtree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 5);
            xassert(tree->root->left->size == 3);
            xassert(tree->root->left->left);
            xassert_false(tree->root->left->color);
            xassert(tree->root->left->left->color);
            xassert(tree->root->left->right->color);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }

        {
            /*             h                              h                             h                                h                           h
            *         /        \                     /        \                    /        \                       /        \                  /        \
            *       d           m                 d(R)        m(R)               d          m(R)                  d          m(R)             d          m(R)
            *     /   \       /   \      ->      /   \       /   \      ->     /   \       /   \      ->        /   \       /   \    ->     /   \       /   \
            *    b     f     j     p            b     f     j     p          b(R) f(R)    j     p             b     f(R)    j     p        b    f(R)   j     p
            *   / \   / \   / \   / \          / \   / \   / \   / \         / \   / \   / \   / \          /   \    / \   / \   / \        \    / \   / \   / \
            *  a   c e   g i   k o   q        a   c e   g i   k o   q       a   c e   g i   k o   q      a(R)  c(R) e   g i   k o   q      c(R) e   g i   k o   q
            */
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_put_repeat(tree, "d", NULL);
            xlistrbtree_put_repeat(tree, "m", NULL);
            xlistrbtree_put_repeat(tree, "b", NULL);
            xlistrbtree_put_repeat(tree, "f", NULL);
            xlistrbtree_put_repeat(tree, "j", NULL);
            xlistrbtree_put_repeat(tree, "p", NULL);
            xlistrbtree_put_repeat(tree, "a", NULL);
            xlistrbtree_put_repeat(tree, "c", NULL);
            xlistrbtree_put_repeat(tree, "e", NULL);
            xlistrbtree_put_repeat(tree, "g", NULL);
            xlistrbtree_put_repeat(tree, "i", NULL);
            xlistrbtree_put_repeat(tree, "k", NULL);
            xlistrbtree_put_repeat(tree, "o", NULL);
            xlistrbtree_put_repeat(tree, "q", NULL);
            xlistrbtree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 14);
            xassert(tree->root->left->size == 6);
            xassert(tree->root->left->left->color);
            xassert_false(tree->root->right->color);
            xassert_false(tree->root->left->left->right->color);
            xassert_false(tree->root->left->right->color);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xlistrbtree_remove_min(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }
    }

    /* xlistrbtree_remove_max */
    {
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_remove_max(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }

        {
            /*      h
            *      /   ->   d
            *    d(R)
            */
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_put_repeat(tree, "d", NULL);
            xlistrbtree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 1);
            xassert(strcmp(tree->root->key, "d") == 0);
            xassert_false(tree->root->left);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_max(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }

        {
            /*      h
            *        \     ->   h
            *        m(R)
            */
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_put_repeat(tree, "m", NULL);
            xlistrbtree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 1);
            xassert(strcmp(tree->root->key, "h") == 0);
            xassert_false(tree->root->right);

            xlistrbtree_remove_min(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }

        {
            /*      h             h            h
            *      / \    ->    /   \    ->   /
            *     d   m       d(m)  m(R)    d(R)
            */
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_put_repeat(tree, "d", NULL);
            xlistrbtree_put_repeat(tree, "m", NULL);
            xlistrbtree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->right);
            xassert_false(tree->root->left->color);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }

        {
            /*      h          h
            *      / \        / \
            *     d   m   -> d   m
            *          \
            *          n(R)
            */
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_put_repeat(tree, "d", NULL);
            xlistrbtree_put_repeat(tree, "m", NULL);
            xlistrbtree_put_repeat(tree, "n", NULL);
            xlistrbtree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 3);
            xassert(tree->root->right->size == 1);
            xassert(tree->root->left->color);
            xassert(tree->root->right->color);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }

        {
            /*      h            h
            *      / \          / \
            *     d   m   ->   d   k
            *        /
            *      k(R)
            */
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_put_repeat(tree, "d", NULL);
            xlistrbtree_put_repeat(tree, "m", NULL);
            xlistrbtree_put_repeat(tree, "k", NULL);
            xlistrbtree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 3);
            xassert(tree->root->right->size == 1);
            xassert(tree->root->left->color);
            xassert(tree->root->right->color);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }

        {
            /*      h             h
            *      / \          /   \
            *     d   m   ->   d     m
            *    /     \      /
            *  a(R)    n(R) a(R)
            */
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_put_repeat(tree, "d", NULL);
            xlistrbtree_put_repeat(tree, "m", NULL);
            xlistrbtree_put_repeat(tree, "a", NULL);
            xlistrbtree_put_repeat(tree, "n", NULL);
            xlistrbtree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 4);
            xassert(tree->root->right->size == 1);
            xassert(tree->root->left->color);
            xassert_false(tree->root->left->left->color);
            xassert(tree->root->right->color);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }

        {
            /*       h               h             h
            *      /  \            /   \         /   \
            *     d  m(R)   ->    d     m   ->  d     m
            *        /  \             /   \          /
            *       k    n         k(R)  n(R)      k(R)
            */
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_put_repeat(tree, "d", NULL);
            xlistrbtree_put_repeat(tree, "m", NULL);
            xlistrbtree_put_repeat(tree, "k", NULL);
            xlistrbtree_put_repeat(tree, "n", NULL);
            xlistrbtree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 4);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->right);
            xassert(tree->root->right->color);
            xassert_false(tree->root->right->left->color);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }

        {
            /*        h          h
            *       /  \        /  \
            *      d  m(R)  -> d   m(R)
            *          /  \       /   \
            *         k    s     k     o
            *             /
            *           o(R)
            */
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_put_repeat(tree, "d", NULL);
            xlistrbtree_put_repeat(tree, "m", NULL);
            xlistrbtree_put_repeat(tree, "k", NULL);
            xlistrbtree_put_repeat(tree, "s", NULL);
            xlistrbtree_put_repeat(tree, "o", NULL);
            xlistrbtree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 5);
            xassert(tree->root->right->size == 3);
            xassert(tree->root->right->left);
            xassert_false(tree->root->right->color);
            xassert(tree->root->right->left->color);
            xassert(tree->root->right->right->color);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }

        {
            /*             h                              h                             h                                h                           h
            *         /        \                     /        \                    /        \                       /        \                  /        \
            *       d           m                 d(R)        m(R)              d(R)         m                   d(R)          m              d(R)         m
            *     /   \       /   \      ->      /   \       /   \      ->     /   \       /   \      ->        /   \       /    \    ->     /   \       /   \
            *    b     f     j     p            b     f     j     p           b     f    j(R)  p(R)            b     f     j(R)   p         b     f    j(R)   p
            *   / \   / \   / \   / \          / \   / \   / \   / \         / \   / \   / \   / \            / \   / \   / \   /   \      / \   / \   / \   /
            *  a   c e   g i   k o   q        a   c e   g i   k o   q       a   c e   g i   k o   q          a   c e   g i   k o(R) q(R)  a   c e   g i   k o(R)
            */
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xlistrbtree_put_repeat(tree, "h", NULL);
            xlistrbtree_put_repeat(tree, "d", NULL);
            xlistrbtree_put_repeat(tree, "m", NULL);
            xlistrbtree_put_repeat(tree, "b", NULL);
            xlistrbtree_put_repeat(tree, "f", NULL);
            xlistrbtree_put_repeat(tree, "j", NULL);
            xlistrbtree_put_repeat(tree, "p", NULL);
            xlistrbtree_put_repeat(tree, "a", NULL);
            xlistrbtree_put_repeat(tree, "c", NULL);
            xlistrbtree_put_repeat(tree, "e", NULL);
            xlistrbtree_put_repeat(tree, "g", NULL);
            xlistrbtree_put_repeat(tree, "i", NULL);
            xlistrbtree_put_repeat(tree, "k", NULL);
            xlistrbtree_put_repeat(tree, "o", NULL);
            xlistrbtree_put_repeat(tree, "q", NULL);
            xlistrbtree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 14);
            xassert(tree->root->right->size == 6);
            xassert_false(tree->root->left->color);
            xassert_false(tree->root->right->left->color);
            xassert(tree->root->right->right->color);
            xassert_false(tree->root->right->right->left->color);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xlistrbtree_remove_max(tree);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }
    }

    /* xlistrbtree_index_replace */
    {
    }

    /* xlistrbtree_index_deep_replace */
    {
    }

    /* xlistrbtree_find */
    {
    }

    /* xlistrbtree_find_replace */
    {
    }

    /* xlistrbtree_find_deep_replace */
    {
    }

    /* xlistrbtree_clear */
    {
    }

    /* xlistrbtree_deep_clear */
    {
    }

    /* xlistrbtree_free */
    {
    }

    /* xlistrbtree_deep_free */
    {
    }

    /* xlistrbtree_remove */
    {
        /* remove one key :
        *
        *         5
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xlistrbtree_remove(tree, "5");
            xassert(xlistrbtree_size(tree) == 0);
            xassert_false(tree->root);
            xlistrbtree_free(&tree);
        }

        /* remove one of two keys :
        *     1. remove the bigger one
        *
        *      5       3
        *     /    ->
        *   3(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xlistrbtree_remove(tree, "5");
            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->values->value, "3v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 1);
            xassert_false(tree->root->left);
            xassert_false(tree->root->right);
            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* remove one of two keys :
        *     2. remove the less one
        *
        *      5       5
        *     /    ->
        *   3(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xlistrbtree_remove(tree, "3");
            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 1);
            xassert_false(tree->root->left);
            xassert_false(tree->root->right);
            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* remove one of three keys :
        *     1. remove 1
        *
        *     3           3
        *    / \    ->     \
        *   1   5         5(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xlistrbtree_remove(tree, "1");

            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->values->value, "3v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->left);

            xassert(strcmp(tree->root->right->key, "5") == 0);
            xassert(strcmp(tree->root->right->values->value, "5v") == 0);
            xassert(tree->root->right->color == false);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);
            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* remove one of three keys :
        *     2. remove 3
        *
        *     3           5
        *    / \    ->   /
        *   1   5      1(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));

            xlistrbtree_remove(tree, "3");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->right);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->values->value, "1v") == 0);
            xassert_false(tree->root->left->color);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* remove one of three keys :
        *     2. remove 5
        *
        *     3           3
        *    / \    ->   /
        *   1   5      1(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));

            xlistrbtree_remove(tree, "5");

            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->values->value, "3v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->right);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->values->value, "1v") == 0);
            xassert(tree->root->left->color == false);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* remove one of four keys :
        *     1. remove 1
        *
        *       5           5
        *      / \         / \
        *     3   8   ->  3   8
        *    /
        *   1(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));

            xlistrbtree_remove(tree, "1");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->values->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* remove one of four keys :
        *     2. remove 3
        *
        *       5           5
        *      / \         / \
        *     3   8   ->  1   8
        *    /
        *   1(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));

            xlistrbtree_remove(tree, "3");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->values->value, "1v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* remove one of four keys :
        *     3. remove 5
        *
        *        5           3
        *       / \         / \
        *      3   8   ->  1   8
        *     /
        *   1(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));

            xlistrbtree_remove(tree, "5");

            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->values->value, "3v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->values->value, "1v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* remove one of four keys :
        *     4. remove 8
        *
        *        5           3
        *       / \         / \
        *      3   8   ->  1   5
        *     /
        *   1(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));

            xlistrbtree_remove(tree, "8");

            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->values->value, "3v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->values->value, "1v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "5") == 0);
            xassert(strcmp(tree->root->right->values->value, "5v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* remove one of four keys :
        *     5. remove 7
        *
        *        5           5
        *       / \         / \
        *      3   8   ->  3   8
        *         /
        *        7(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "7", "7v"));

            xlistrbtree_remove(tree, "7");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->values->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* remove one of five keys :
        *     1. remove 0
        *
        *       5             5
        *     /   \         /   \
        *   1(R)   8  ->   1     8
        *   /  \            \
        *  0    3           3(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "0", "0v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));

            xlistrbtree_remove(tree, "0");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->values->value, "1v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->left);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->right->key, "3") == 0);
            xassert(strcmp(tree->root->left->right->values->value, "3v") == 0);
            xassert_false(tree->root->left->right->color);
            xassert(tree->root->left->right->size == 1);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* remove one of five keys :
        *     2. remove 1
        *
        *       5             5
        *     /   \         /   \
        *   1(R)   8  ->   3     8
        *   /  \          /
        *  0    3        0(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "0", "0v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));

            xlistrbtree_remove(tree, "1");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->values->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "0") == 0);
            xassert(strcmp(tree->root->left->left->values->value, "0v") == 0);
            xassert(tree->root->left->left->color == false);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* remove one of five keys :
        *     3. remove 3
        *
        *       5             5
        *     /   \         /   \
        *   1(R)   8  ->   1     8
        *   /  \          /
        *  0    3        0(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "0", "0v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));

            xlistrbtree_remove(tree, "3");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->values->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->values->value, "1v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "0") == 0);
            xassert(strcmp(tree->root->left->left->values->value, "0v") == 0);
            xassert(tree->root->left->left->color == false);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* remove one of five keys :
        *     4. remove 5
        *
        *       5             1
        *     /   \         /   \
        *   1(R)   8  ->   0     8
        *   /  \                /
        *  0    3              3(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "0", "0v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));

            xlistrbtree_remove(tree, "5");

            xassert(strcmp(tree->root->key, "1") == 0);
            xassert(strcmp(tree->root->values->value, "1v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "0") == 0);
            xassert(strcmp(tree->root->left->values->value, "0v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->right->left->key, "3") == 0);
            xassert(strcmp(tree->root->right->left->values->value, "3v") == 0);
            xassert(tree->root->right->left->color == false);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* remove one of five keys :
        *     5. remove 8
        *
        *       5             1
        *     /   \         /   \
        *   1(R)   8  ->   0     5
        *   /  \                /
        *  0    3              3(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "0", "0v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));

            xlistrbtree_remove(tree, "8");

            xassert(strcmp(tree->root->key, "1") == 0);
            xassert(strcmp(tree->root->values->value, "1v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "0") == 0);
            xassert(strcmp(tree->root->left->values->value, "0v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "5") == 0);
            xassert(strcmp(tree->root->right->values->value, "5v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->right->left->key, "3") == 0);
            xassert(strcmp(tree->root->right->left->values->value, "3v") == 0);
            xassert(tree->root->right->left->color == false);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* remove one of five keys :
        *     6. remove 6
        *
        *        6               8
        *      /   \           /   \
        *     5     9   ->    5     9
        *    /     /         /
        *  1(R)   8(R)     1(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "6", "6v"));
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "9", "9v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));

            xlistrbtree_remove(tree, "6");

            xassert(strcmp(tree->root->key, "8") == 0);
            xassert(strcmp(tree->root->values->value, "8v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "5") == 0);
            xassert(strcmp(tree->root->left->values->value, "5v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->values->value, "9v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->values->value, "1v") == 0);
            xassert(tree->root->left->left->color == false);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* remove one of five keys :
        *     6. remove 9
        *
        *        6               6
        *      /   \           /   \
        *     5     9   ->    5     8
        *    /     /         /
        *  1(R)   8(R)     1(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "6", "6v"));
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "9", "9v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));

            xlistrbtree_remove(tree, "9");

            xassert(strcmp(tree->root->key, "6") == 0);
            xassert(strcmp(tree->root->values->value, "6v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "5") == 0);
            xassert(strcmp(tree->root->left->values->value, "5v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->values->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->values->value, "1v") == 0);
            xassert(tree->root->left->left->color == false);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* remove one of five keys :
        *     6. remove 8
        *
        *        6               6
        *      /   \           /   \
        *     5     9   ->    5     9
        *    /     /         /
        *  1(R)   8(R)     1(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "6", "6v"));
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "9", "9v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));

            xlistrbtree_remove(tree, "8");

            xassert(strcmp(tree->root->key, "6") == 0);
            xassert(strcmp(tree->root->values->value, "6v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "5") == 0);
            xassert(strcmp(tree->root->left->values->value, "5v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->values->value, "9v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->values->value, "1v") == 0);
            xassert(tree->root->left->left->color == false);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(xlistrbtree_is_rbtree(tree));
            xlistrbtree_free(&tree);
        }

        /* remove one of more keys :
        *     1. remove 5
        *
        *       5                     8
        *     /    \               /     \
        *    3      9      ->    3(R)     9
        *   / \    / \           / \       \
        *  1   4  8  95         1   4      95(R)
        */
        {
            XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
            xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
            xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
            xassert(xlistrbtree_put_repeat(tree, "9", "9v"));
            xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
            xassert(xlistrbtree_put_repeat(tree, "4", "4v"));
            xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
            xassert(xlistrbtree_put_repeat(tree, "95", "95v"));

            xlistrbtree_remove(tree, "5");

            xassert(strcmp(tree->root->key, "8") == 0);
            xassert(strcmp(tree->root->values->value, "8v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 6);

xassert(strcmp(tree->root->left->key, "3") == 0);
xassert(strcmp(tree->root->left->values->value, "3v") == 0);
xassert(tree->root->left->color == false);
xassert(tree->root->left->size == 3);

xassert(strcmp(tree->root->right->key, "9") == 0);
xassert(strcmp(tree->root->right->values->value, "9v") == 0);
xassert(tree->root->right->color == true);
xassert(tree->root->right->size == 2);
xassert_false(tree->root->right->left);
xassert(tree->root->right->right);

xassert(strcmp(tree->root->left->left->key, "1") == 0);
xassert(strcmp(tree->root->left->left->values->value, "1v") == 0);
xassert(tree->root->left->left->color == true);
xassert(tree->root->left->left->size == 1);
xassert_false(tree->root->left->left->left);
xassert_false(tree->root->left->left->right);

xassert(strcmp(tree->root->left->right->key, "4") == 0);
xassert(strcmp(tree->root->left->right->values->value, "4v") == 0);
xassert(tree->root->left->right->color == true);
xassert(tree->root->left->right->size == 1);
xassert_false(tree->root->left->right->left);
xassert_false(tree->root->left->right->right);

xassert(strcmp(tree->root->right->right->key, "95") == 0);
xassert(strcmp(tree->root->right->right->values->value, "95v") == 0);
xassert(tree->root->right->right->color == false);
xassert(tree->root->right->right->size == 1);
xassert_false(tree->root->right->left);

xassert(xlistrbtree_is_rbtree(tree));
xlistrbtree_free(&tree);
        }

        /* remove one of more keys :
        *     2. remove 95
        *
        *       5                     5
        *     /    \               /     \
        *    3      9      ->    3(R)     9
        *   / \    / \           / \     /
        *  1   4  8  95         1   4   8(R)
        */
        {
        XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
        xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
        xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
        xassert(xlistrbtree_put_repeat(tree, "9", "9v"));
        xassert(xlistrbtree_put_repeat(tree, "1", "1v"));
        xassert(xlistrbtree_put_repeat(tree, "4", "4v"));
        xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
        xassert(xlistrbtree_put_repeat(tree, "95", "95v"));

        xlistrbtree_remove(tree, "95");

        xassert(strcmp(tree->root->key, "5") == 0);
        xassert(strcmp(tree->root->values->value, "5v") == 0);
        xassert(tree->root->color == true);
        xassert(tree->root->size == 6);

        xassert(strcmp(tree->root->left->key, "3") == 0);
        xassert(strcmp(tree->root->left->values->value, "3v") == 0);
        xassert(tree->root->left->color == false);
        xassert(tree->root->left->size == 3);

        xassert(strcmp(tree->root->right->key, "9") == 0);
        xassert(strcmp(tree->root->right->values->value, "9v") == 0);
        xassert(tree->root->right->color == true);
        xassert(tree->root->right->size == 2);
        xassert_false(tree->root->right->right);

        xassert(strcmp(tree->root->left->left->key, "1") == 0);
        xassert(strcmp(tree->root->left->left->values->value, "1v") == 0);
        xassert(tree->root->left->left->color == true);
        xassert(tree->root->left->left->size == 1);
        xassert_false(tree->root->left->left->left);
        xassert_false(tree->root->left->left->right);

        xassert(strcmp(tree->root->left->right->key, "4") == 0);
        xassert(strcmp(tree->root->left->right->values->value, "4v") == 0);
        xassert(tree->root->left->right->color == true);
        xassert(tree->root->left->right->size == 1);
        xassert_false(tree->root->left->right->left);
        xassert_false(tree->root->left->right->right);

        xassert(strcmp(tree->root->right->left->key, "8") == 0);
        xassert(strcmp(tree->root->right->left->values->value, "8v") == 0);
        xassert(tree->root->right->left->color == false);
        xassert(tree->root->right->left->size == 1);
        xassert_false(tree->root->right->left->left);
        xassert_false(tree->root->right->left->right);

        xassert(xlistrbtree_is_rbtree(tree));
        xlistrbtree_free(&tree);
        }
    }

    /* xlistrbtree_remove_all */
    {
        XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);
        xassert(xlistrbtree_put_repeat(tree, "a", NULL));
        xassert(xlistrbtree_put_repeat(tree, "b", NULL));
        xassert(xlistrbtree_put_repeat(tree, "c", NULL));
        xassert(xlistrbtree_put_repeat(tree, "d", NULL));
        xassert(xlistrbtree_put_repeat(tree, "e", NULL));
        xassert(xlistrbtree_put_repeat(tree, "a", NULL));
        xassert(xlistrbtree_put_repeat(tree, "b", NULL));
        xassert(xlistrbtree_put_repeat(tree, "c", NULL));
        xassert(xlistrbtree_put_repeat(tree, "d", NULL));
        xassert(xlistrbtree_put_repeat(tree, "e", NULL));
        xassert(xlistrbtree_put_repeat(tree, "a", NULL));
        xassert(xlistrbtree_put_repeat(tree, "b", NULL));
        xassert(xlistrbtree_put_repeat(tree, "c", NULL));
        xassert(xlistrbtree_put_repeat(tree, "d", NULL));
        xassert(xlistrbtree_put_repeat(tree, "e", NULL));
        xassert(xlistrbtree_put_repeat(tree, "a", NULL));
        xassert(xlistrbtree_put_repeat(tree, "b", NULL));
        xassert(xlistrbtree_put_repeat(tree, "c", NULL));
        xassert(xlistrbtree_put_repeat(tree, "d", NULL));
        xassert(xlistrbtree_put_repeat(tree, "e", NULL));
        xassert(xlistrbtree_put_repeat(tree, "a", NULL));
        xassert(xlistrbtree_put_repeat(tree, "b", NULL));
        xassert(xlistrbtree_put_repeat(tree, "c", NULL));
        xassert(xlistrbtree_put_repeat(tree, "d", NULL));
        xassert(xlistrbtree_put_repeat(tree, "e", NULL));

        xlistrbtree_remove_all(tree, "a");
        xassert(xlistrbtree_is_rbtree(tree));
        xassert(xlistrbtree_keys_size(tree, "a", "a") == 0);
        xlistrbtree_remove_all(tree, "b");
        xassert(xlistrbtree_is_rbtree(tree));
        xassert(xlistrbtree_keys_size(tree, "b", "b") == 0);
        xlistrbtree_remove_all(tree, "c");
        xassert(xlistrbtree_is_rbtree(tree));
        xassert(xlistrbtree_keys_size(tree, "c", "c") == 0);

        xlistrbtree_free(&tree);
    }

    /* xlistrbtree_deep_remove_all */
    {
        XListRBTree_PT tree = xlistrbtree_random_string(test_cmpk, NULL, 500, 3);

        xlistrbtree_deep_remove_all(tree, "500");
        xassert(xlistrbtree_is_rbtree(tree));
        xlistrbtree_deep_remove_all(tree, "501");
        xassert(xlistrbtree_is_rbtree(tree));
        xlistrbtree_deep_remove_all(tree, "502");
        xassert(xlistrbtree_is_rbtree(tree));
        xlistrbtree_deep_remove_all(tree, "503");
        xassert(xlistrbtree_is_rbtree(tree));
        xlistrbtree_deep_remove_all(tree, "504");
        xassert(xlistrbtree_is_rbtree(tree));
        xlistrbtree_deep_remove_all(tree, "505");
        xassert(xlistrbtree_is_rbtree(tree));
        xlistrbtree_deep_remove_all(tree, "506");
        xassert(xlistrbtree_is_rbtree(tree));
        xlistrbtree_deep_remove_all(tree, "507");
        xassert(xlistrbtree_is_rbtree(tree));

        xlistrbtree_deep_free(&tree);
    }

    {
        XListRBTree_PT tree = xlistrbtree_random_string(test_cmpk, NULL, 10, 3);

        /* xlistrbtree_map_preorder */
        //xlistrbtree_map_preorder(tree, xlistrbtree_test_map_applykv, NULL);

        /* xlistrbtree_map_inorder */
        //xlistrbtree_map_inorder(tree, xlistrbtree_test_map_applykv, NULL);

        /* xlistrbtree_map_postorder */
        //xlistrbtree_map_postorder(tree, xlistrbtree_test_map_applykv, NULL);

        /* xlistrbtree_map_levelorder */
        //xlistrbtree_map_levelorder(tree, xlistrbtree_test_map_applykv, NULL);

        /* xlistrbtree_map_min_to_max */
        //xlistrbtree_map_min_to_max(tree, xlistrbtree_test_map_applykv, NULL);

        /* xlistrbtree_map_max_to_min */
        //xlistrbtree_map_max_to_min(tree, xlistrbtree_test_map_applykv, NULL);

        xlistrbtree_deep_free(&tree);
    }

    {
        XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);

        xassert(xlistrbtree_put_repeat(tree, "a", NULL));
        xassert(xlistrbtree_put_repeat(tree, "b", NULL));
        xassert(xlistrbtree_put_repeat(tree, "c", NULL));
        xassert(xlistrbtree_put_repeat(tree, "d", NULL));
        xassert(xlistrbtree_put_repeat(tree, "e", NULL));

        /* xlistrbtree_map_min_to_max_break_if_true */
        xassert_false(xlistrbtree_map_min_to_max_break_if_true(tree, xlistrbtree_test_map_applykv_false, NULL));
        xassert_false(xlistrbtree_map_min_to_max_break_if_true(tree, xlistrbtree_test_map_cmpkv, "x"));
        xassert(xlistrbtree_map_min_to_max_break_if_true(tree, xlistrbtree_test_map_cmpkv, "a"));

        /* xlistrbtree_map_min_to_max_break_if_false */
        xassert_false(xlistrbtree_map_min_to_max_break_if_false(tree, xlistrbtree_test_map_applykv_true, NULL));
        xassert(xlistrbtree_map_min_to_max_break_if_false(tree, xlistrbtree_test_map_cmpkv, "a"));
        xassert(xlistrbtree_map_min_to_max_break_if_false(tree, xlistrbtree_test_map_cmpkv, "b"));

        /* xlistrbtree_map_max_to_min_break_if_true */
        xassert_false(xlistrbtree_map_max_to_min_break_if_true(tree, xlistrbtree_test_map_applykv_false, NULL));
        xassert_false(xlistrbtree_map_max_to_min_break_if_true(tree, xlistrbtree_test_map_cmpkv, "x"));
        xassert(xlistrbtree_map_max_to_min_break_if_true(tree, xlistrbtree_test_map_cmpkv, "a"));

        /* xlistrbtree_map_max_to_min_break_if_false */
        xassert_false(xlistrbtree_map_max_to_min_break_if_false(tree, xlistrbtree_test_map_applykv_true, NULL));
        xassert(xlistrbtree_map_max_to_min_break_if_false(tree, xlistrbtree_test_map_cmpkv, "a"));
        xassert(xlistrbtree_map_max_to_min_break_if_false(tree, xlistrbtree_test_map_cmpkv, "b"));

        xlistrbtree_free(&tree);
    }
        
    /* xlistrbtree_swap */
    {
    }
        
    /* xlistrbtree_merge_repeat */
    {
        XListRBTree_PT tree1 = xlistrbtree_new(test_cmpk, NULL);
        XListRBTree_PT tree2 = xlistrbtree_new(test_cmpk, NULL);

        xassert(xlistrbtree_put_repeat(tree1, "a", "a1"));
        xassert(xlistrbtree_put_repeat(tree1, "b", "b1"));
        xassert(xlistrbtree_put_repeat(tree1, "c", "c1"));
        xassert(xlistrbtree_put_repeat(tree1, "d", "d1"));
        xassert(xlistrbtree_put_repeat(tree1, "e", "e1"));
        xassert(xlistrbtree_put_repeat(tree1, "f", "f1"));
        xassert(xlistrbtree_put_repeat(tree1, "g", "g1"));

        xassert(xlistrbtree_put_repeat(tree2, "a", "a2"));
        xassert(xlistrbtree_put_repeat(tree2, "b", "b2"));
        xassert(xlistrbtree_put_repeat(tree2, "c", "c2"));
        xassert(xlistrbtree_put_repeat(tree2, "d", "d2"));
        xassert(xlistrbtree_put_repeat(tree2, "e", "e2"));
        xassert(xlistrbtree_put_repeat(tree2, "f", "f2"));
        xassert(xlistrbtree_put_repeat(tree2, "g", "g2"));

        xlistrbtree_merge_repeat(tree1, &tree2);

        xassert(xlistrbtree_size(tree1) == 14);

        //xlistrbtree_print(tree1, 2);

        xlistrbtree_free(&tree1);
    }
        
    /* xlistrbtree_merge_unique */
    {
        XListRBTree_PT tree1 = xlistrbtree_new(test_cmpk, NULL);
        XListRBTree_PT tree2 = xlistrbtree_new(test_cmpk, NULL);

        xassert(xlistrbtree_put_repeat(tree1, "a", "a1"));
        xassert(xlistrbtree_put_repeat(tree1, "b", "b1"));
        xassert(xlistrbtree_put_repeat(tree1, "c", "c1"));
        xassert(xlistrbtree_put_repeat(tree1, "d", "d1"));
        xassert(xlistrbtree_put_repeat(tree1, "e", "e1"));
        xassert(xlistrbtree_put_repeat(tree1, "f", "f1"));
        xassert(xlistrbtree_put_repeat(tree1, "g", "g1"));

        xassert(xlistrbtree_put_repeat(tree2, "a", "a2"));
        xassert(xlistrbtree_put_repeat(tree2, "b", "b2"));
        xassert(xlistrbtree_put_repeat(tree2, "c", "c2"));
        xassert(xlistrbtree_put_repeat(tree2, "d", "d2"));
        xassert(xlistrbtree_put_repeat(tree2, "e", "e2"));
        xassert(xlistrbtree_put_repeat(tree2, "f", "f2"));
        xassert(xlistrbtree_put_repeat(tree2, "g", "g2"));
        xassert(xlistrbtree_put_repeat(tree2, "h", "h2"));

        xlistrbtree_merge_unique(tree1, &tree2);

        xassert(xlistrbtree_size(tree1) == 8);

        //xlistrbtree_print(tree1, 2);

        xlistrbtree_free(&tree1);
    }
      
    /* xlistrbtree_keys_size */
    {
        XListRBTree_PT tree = xlistrbtree_new(test_cmpk, NULL);

        xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
        xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
        xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
        xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
        xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
        xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
        xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
        xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
        xassert(xlistrbtree_put_repeat(tree, "5", "5v"));
        xassert(xlistrbtree_put_repeat(tree, "3", "3v"));
        xassert(xlistrbtree_put_repeat(tree, "8", "8v"));
        xassert(xlistrbtree_keys_size(tree, "5", "5") == 1);

        xlistrbtree_free(&tree);
    }
        
    /* xlistrbtree_size */
    {
    }
        
    /* xlistrbtree_is_empty */
    {
    }
        
    /* xlistrbtree_height */
    {
    }

    /* xlistrbtree_is_rbtree */
    {
        XListRBTree_PT tree = xlistrbtree_random_string(test_cmpk, NULL, 50, 2);
        xassert(xlistrbtree_is_rbtree(tree));

        xlistrbtree_deep_remove(tree, "10");
        xlistrbtree_deep_remove(tree, "11");
        xlistrbtree_deep_remove(tree, "12");
        xlistrbtree_deep_remove(tree, "13");
        xlistrbtree_deep_remove(tree, "14");
        xlistrbtree_deep_remove(tree, "15");
        xlistrbtree_deep_remove(tree, "16");
        xlistrbtree_deep_remove(tree, "17");
        xlistrbtree_deep_remove(tree, "18");
        xlistrbtree_deep_remove(tree, "19");
        xassert(xlistrbtree_is_rbtree(tree));

        xlistrbtree_deep_remove_min(tree);
        xlistrbtree_deep_remove_max(tree);
        xlistrbtree_deep_remove_min(tree);
        xlistrbtree_deep_remove_max(tree);
        xlistrbtree_deep_remove_min(tree);
        xlistrbtree_deep_remove_max(tree);
        xlistrbtree_deep_remove_min(tree);
        xlistrbtree_deep_remove_max(tree);
        xlistrbtree_deep_remove_min(tree);
        xassert(xlistrbtree_is_rbtree(tree));

        xlistrbtree_deep_remove(tree, "20");
        xlistrbtree_deep_remove(tree, "21");
        xlistrbtree_deep_remove(tree, "22");
        xlistrbtree_deep_remove(tree, "23");
        xlistrbtree_deep_remove(tree, "24");
        xlistrbtree_deep_remove(tree, "25");
        xlistrbtree_deep_remove(tree, "26");
        xlistrbtree_deep_remove(tree, "27");
        xlistrbtree_deep_remove(tree, "28");
        xlistrbtree_deep_remove(tree, "29");
        xassert(xlistrbtree_is_rbtree(tree));

        xlistrbtree_deep_remove_max(tree);
        xlistrbtree_deep_remove_min(tree);
        xlistrbtree_deep_remove_max(tree);
        xlistrbtree_deep_remove_min(tree);
        xlistrbtree_deep_remove_max(tree);
        xlistrbtree_deep_remove_min(tree);
        xlistrbtree_deep_remove_max(tree);
        xlistrbtree_deep_remove_min(tree);
        xlistrbtree_deep_remove_max(tree);
        xassert(xlistrbtree_is_rbtree(tree));

        xlistrbtree_deep_remove(tree, "50");
        xlistrbtree_deep_remove(tree, "51");
        xlistrbtree_deep_remove(tree, "52");
        xlistrbtree_deep_remove(tree, "53");
        xlistrbtree_deep_remove(tree, "54");
        xlistrbtree_deep_remove(tree, "55");
        xlistrbtree_deep_remove(tree, "56");
        xlistrbtree_deep_remove(tree, "57");
        xlistrbtree_deep_remove(tree, "58");
        xlistrbtree_deep_remove(tree, "59");
        xassert(xlistrbtree_is_rbtree(tree));

        xlistrbtree_deep_remove_min(tree);
        xlistrbtree_deep_remove_max(tree);
        xlistrbtree_deep_remove_min(tree);
        xlistrbtree_deep_remove_max(tree);
        xlistrbtree_deep_remove_min(tree);
        xlistrbtree_deep_remove_max(tree);
        xlistrbtree_deep_remove_min(tree);
        xlistrbtree_deep_remove_max(tree);
        xlistrbtree_deep_remove_min(tree);
        xlistrbtree_deep_remove_max(tree);
        xassert(xlistrbtree_is_rbtree(tree));

        xlistrbtree_deep_remove(tree, "70");
        xlistrbtree_deep_remove(tree, "71");
        xlistrbtree_deep_remove(tree, "72");
        xlistrbtree_deep_remove(tree, "73");
        xlistrbtree_deep_remove(tree, "74");
        xlistrbtree_deep_remove(tree, "75");
        xlistrbtree_deep_remove(tree, "76");
        xlistrbtree_deep_remove(tree, "77");
        xlistrbtree_deep_remove(tree, "78");
        xlistrbtree_deep_remove(tree, "79");
        xassert(xlistrbtree_is_rbtree(tree));

        xlistrbtree_deep_free(&tree);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
