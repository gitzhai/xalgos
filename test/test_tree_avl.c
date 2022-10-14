
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../tree_avl/xtree_avl_x.h"
#include "../include/xalgos.h"

static
int test_cmpk(void *key1, void *key2, void *cl) {
    return strcmp((char*)key1, (char*)key2);
}

static
bool xavltree_test_map_applykv_false(void *key, void **value, void *cl) {
    return false;
}

static
bool xavltree_test_map_applykv_true(void *key, void **value, void *cl) {
    return true;
}

static
bool xavltree_test_map_applyk_false(void *key, void *cl) {
    return false;
}

static
bool xavltree_test_map_applyk_true(void *key, void *cl) {
    return true;
}

static
bool xavltree_test_map_cmpkv(void *key, void **value, void *cl) {
    return strcmp((char*)key, (char*)cl) == 0;
}

static
bool xavltree_test_map_cmpk(void *key, void *cl) {
    return strcmp((char*)key, (char*)cl) == 0;
}

static
bool xavltree_test_map_applykv(void *key, void **value, void *cl) {
    printf("%s\n", (char*)key);
    return true;
}

static
bool xavltree_test_map_applyk(void *key, void *cl) {
    printf("%s\n", (char*)key);
    return true;
}

static
XAVLTree_PT xavltree_random_string(int(*cmp)(void *key1, void *key2, void *cl), void *cl, int total_size, int string_length, bool unique) {
    xassert(cmp);
    xassert(0 < total_size);
    xassert(0 < string_length);

    if (!cmp || (total_size <= 0) || (string_length <= 0)) {
        return NULL;
    }

    {
        XAVLTree_PT tree = xavltree_new(cmp, cl);
        if (!tree) {
            return NULL;
        }

        {
            const char charsets[] = "123456789";

            for (int i = 0; i < total_size; i++) {
                char* str = XMEM_MALLOC(string_length + 1);
                if (!str) {
                    xavltree_deep_free(&tree);
                    return NULL;
                }

                for (int i = 0; i < string_length; ++i) {
                    str[i] = charsets[rand() % (sizeof(charsets) - 1)];
                }
                str[string_length] = '\0';

                if (unique) {
                    if (!xavltree_put_unique(tree, str, NULL)) {
                        xavltree_deep_free(&tree);
                        return NULL;
                    }
                }
                else {
                    if (!xavltree_put_repeat(tree, str, NULL)) {
                        xavltree_deep_free(&tree);
                        return NULL;
                    }
                }
            }
        }

        return tree;
    }
}

static
void xavltree_printnode(char* key, int weight, int h) {
    for (int i = 0; i < h; i++) {
        printf(" ");
    }
    printf("%s:%d\n", key, weight);
}

static
void xavltree_print_impl(XAVLTree_Node_PT node, int h) {
    if (!node) {
        return;
    }

    xavltree_print_impl(node->right, h + 5);

    xavltree_printnode((char*)node->key, node->height, h);

    xavltree_print_impl(node->left, h + 5);
}

static
void xavltree_print(XAVLTree_PT tree, int start_blank) {
    xassert(tree);
    xassert(0 <= start_blank);

    if (start_blank < 0) {
        return;
    }

    if (!tree) {
        xavltree_printnode("-", 0, start_blank);
        return;
    }

    xavltree_print_impl(tree->root, start_blank + 5);
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

void test_xavltree() {
    /* xavltree_new */
    {
        /* cmp == NULL */
        {
            bool except = false;

            XEXCEPT_TRY
                xavltree_new(NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        /* normal */
        {
            XAVLTree_PT tree = NULL;

            tree = xavltree_new(test_cmpk, NULL);

            xassert(tree);
            xassert(tree->cmp);
            xassert_false(tree->root);

            xassert(xavltree_size(tree) == 0);

            xavltree_free(&tree);
        }
    }

    /* xavltree_put_repeat */
    {
        /* tree == NULL */
        {
            bool except = false;

            XEXCEPT_TRY
                xavltree_put_repeat(NULL, "abc", "def");
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        /* key == NULL */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            bool except = false;

            XEXCEPT_TRY
                xavltree_put_repeat(tree, NULL, "def");
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xavltree_free(&tree);
        }

        /* add one (key, value) pair :
        *
        *         5
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 1);
            xassert_false(tree->root->parent);
            xassert_false(tree->root->left);
            xassert_false(tree->root->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add two (key, value) pairs :
        *     1. second key is less than first key
        *
        *             5
        *   5  ->    /
        *           3
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->right);
            xassert_false(tree->root->parent);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 1);

            xassert(strcmp(tree->root->left->parent->key, "5") == 0);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add two (key, value) pairs :
        *     2. second key is larger than first key
        *
        *            5
        *   5  ->     \
        *              8
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 2);

            xassert_false(tree->root->parent);
            xassert_false(tree->root->left);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 1);

            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add three (key, value) pairs :
        *     1. 1 is less than 3
        *
        *             5            5        3
        *   5  ->    /     ->    /    ->   /  \
        *           3           3         1    5
        *                      /
        *                     1
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));

            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->value, "3v") == 0);
            xassert_false(tree->root->parent);
            xassert(tree->root->size == 3);


            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert(tree->root->left->size == 1);

            xassert(strcmp(tree->root->left->parent->key, "3") == 0);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "5") == 0);
            xassert(strcmp(tree->root->right->value, "5v") == 0);
            xassert(tree->root->right->size == 1);

            xassert(strcmp(tree->root->right->parent->key, "3") == 0);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add three (key, value) pairs :
        *     2.  4 is bigger than 3
        *
        *             5            5          5          4
        *   5  ->    /     ->    /    ->     /    ->   /   \
        *           3           3           4         3     5
        *                        \         /
        *                         4       3
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "4", "4v"));

            xassert(strcmp(tree->root->key, "4") == 0);
            xassert(strcmp(tree->root->value, "4v") == 0);
            xassert(tree->root->size == 3);


            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 1);

            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "5") == 0);
            xassert(strcmp(tree->root->right->value, "5v") == 0);
            xassert(tree->root->right->size == 1);

            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add three (key, value) pairs :
        *     1. 6 is less than 8
        *
        *             5            5           5             6
        *   5  ->      \     ->     \    ->     \     ->   /   \
        *               8            8           6        5     8
        *                           /             \
        *                          6               8
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "6", "6v"));

            xassert(strcmp(tree->root->key, "6") == 0);
            xassert(strcmp(tree->root->value, "6v") == 0);
            xassert_false(tree->root->parent);
            xassert(tree->root->size == 3);


            xassert(strcmp(tree->root->left->key, "5") == 0);
            xassert(strcmp(tree->root->left->value, "5v") == 0);
            xassert(tree->root->left->size == 1);

            xassert(strcmp(tree->root->left->parent->key, "6") == 0);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 1);

            xassert(strcmp(tree->root->right->parent->key, "6") == 0);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add three (key, value) pairs :
        *     1. 9 is greater than 8
        *
        *             5            5              8
        *   5  ->      \     ->     \    ->     /   \
        *               8            8         5     9
        *                             \
        *                              9
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "9", "9v"));

            xassert(strcmp(tree->root->key, "8") == 0);
            xassert(strcmp(tree->root->value, "8v") == 0);
            xassert_false(tree->root->parent);
            xassert(tree->root->size == 3);


            xassert(strcmp(tree->root->left->key, "5") == 0);
            xassert(strcmp(tree->root->left->value, "5v") == 0);
            xassert(tree->root->left->size == 1);

            xassert(strcmp(tree->root->left->parent->key, "8") == 0);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->value, "9v") == 0);
            xassert(tree->root->right->size == 1);

            xassert(strcmp(tree->root->right->parent->key, "8") == 0);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add three (key, value) pairs :
        *     3. 8 is bigger than 5
        *
        *             5            5
        *   5  ->    /     ->    /   \
        *           3           3     8
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 3);


            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 1);

            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 1);

            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add four (key, value) pairs :
        *     1. 1 is less than 3
        *
        *             5            5
        *   5  ->    / \     ->   / \
        *           3   8        3   8
        *                       /
        *                      1
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 4);


            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 2);

            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 1);

            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->size == 1);

            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xavltree_free(&tree);
        }

        /* add four (key, value) pairs :
        *     2. 4 is bigger than 3
        *
        *             5            5
        *   5  ->    / \     ->   / \
        *           3   8        3   8
        *                         \
        *                          4
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "4", "4v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 4);


            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 2);

            xassert_false(tree->root->left->left);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 1);

            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->right->key, "4") == 0);
            xassert(strcmp(tree->root->left->right->value, "4v") == 0);
            xassert(tree->root->left->right->size == 1);

            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add four (key, value) pairs :
        *     3. 6 is less than 8
        *
        *             5            5
        *   5  ->    / \     ->   / \
        *           3   8        3   8
        *                           /
        *                          6
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "6", "6v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 4);


            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 1);

            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 2);

            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->right->left->key, "6") == 0);
            xassert(strcmp(tree->root->right->left->value, "6v") == 0);
            xassert(tree->root->right->left->size == 1);

            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add four (key, value) pairs :
        *     4. 9 is bigger than 8
        *
        *             5            5
        *   5  ->    / \     ->   / \
        *           3   8        3   8
        *                             \
        *                              9
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "9", "9v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 4);


            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 1);

            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 2);

            xassert_false(tree->root->right->left);

            xassert(strcmp(tree->root->right->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->right->value, "9v") == 0);
            xassert(tree->root->right->right->size == 1);

            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add five (key, value) pairs :
        *     1. 0 is less than 1
        *
        *             5            5            5            5
        *   5  ->    / \     ->   / \     ->   / \     ->  /   \
        *           3   8        3   8        3   8       1     8
        *                       /            /          /   \
        *                      1            1          0     3
        *                                  /
        *                                 0
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "0", "0v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 5);


            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert(tree->root->left->size == 3);


            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 1);

            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "0") == 0);
            xassert(strcmp(tree->root->left->left->value, "0v") == 0);
            xassert(tree->root->left->left->size == 1);

            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "3") == 0);
            xassert(strcmp(tree->root->left->right->value, "3v") == 0);
            xassert(tree->root->left->right->size == 1);

            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add five (key, value) pairs :
        *     2. 15 is bigger than 1
        *
        *             5            5            5            5             5
        *   5  ->    / \     ->   / \     ->   / \     ->  /   \    ->   /   \
        *           3   8        3   8        3   8       3     8      15     8
        *                       /            /           /            /  \
        *                      1            1          15            1    3
        *                                    \         /
        *                                    15      1
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "15", "15v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 5);


            xassert(strcmp(tree->root->left->key, "15") == 0);
            xassert(strcmp(tree->root->left->value, "15v") == 0);
            xassert(tree->root->left->size == 3);


            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 1);

            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->size == 1);

            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "3") == 0);
            xassert(strcmp(tree->root->left->right->value, "3v") == 0);
            xassert(tree->root->left->right->size == 1);

            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add five (key, value) pairs :
        *     6 is less than 7
        *
        *             5            5            5            5
        *   5  ->    / \     ->   / \     ->   / \     ->  /   \
        *           3   8        3   8        3   8       3     7
        *                           /            /            /   \
        *                          7            7            6     8
        *                                      /
        *                                     6
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "7", "7v"));
            xassert(xavltree_put_repeat(tree, "6", "6v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 5);


            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 1);


            xassert(strcmp(tree->root->right->key, "7") == 0);
            xassert(strcmp(tree->root->right->value, "7v") == 0);
            xassert(tree->root->right->size == 3);


            xassert(strcmp(tree->root->right->left->key, "6") == 0);
            xassert(strcmp(tree->root->right->left->value, "6v") == 0);
            xassert(tree->root->right->left->size == 1);

            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(strcmp(tree->root->right->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->right->value, "8v") == 0);
            xassert(tree->root->right->right->size == 1);

            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add five (key, value) pairs :
        *     7 is greater than 6
        *
        *             5            5            5            5
        *   5  ->    / \     ->   / \     ->   / \     ->  /   \
        *           3   8        3   8        3   8       3     7
        *                           /            /            /   \
        *                          6            6            6     8
        *                                        \
        *                                         7
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "6", "6v"));
            xassert(xavltree_put_repeat(tree, "7", "7v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 5);


            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 1);


            xassert(strcmp(tree->root->right->key, "7") == 0);
            xassert(strcmp(tree->root->right->value, "7v") == 0);
            xassert(tree->root->right->size == 3);


            xassert(strcmp(tree->root->right->left->key, "6") == 0);
            xassert(strcmp(tree->root->right->left->value, "6v") == 0);
            xassert(tree->root->right->left->size == 1);

            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(strcmp(tree->root->right->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->right->value, "8v") == 0);
            xassert(tree->root->right->right->size == 1);

            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add five (key, value) pairs :
        *     3. 4 is bigger than 3
        *
        *             5            5            5
        *   5  ->    / \     ->   / \     ->   / \
        *           3   8        3   8        3   8
        *                       /            /  \
        *                      1            1    4
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "4", "4v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 5);


            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 3);


            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 1);

            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->size == 1);

            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "4") == 0);
            xassert(strcmp(tree->root->left->right->value, "4v") == 0);
            xassert(tree->root->left->right->size == 1);

            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add five (key, value) pairs :
        *     4. 6 is less than 8
        *
        *             5            5            5
        *   5  ->    / \     ->   / \     ->   / \
        *           3   8        3   8        3   8
        *                       /            /    /
        *                      1            1    6
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "6", "6v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 5);


            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 2);


            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 2);

            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->size == 1);

            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->right->left->key, "6") == 0);
            xassert(strcmp(tree->root->right->left->value, "6v") == 0);
            xassert(tree->root->right->left->size == 1);

            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add five (key, value) pairs :
        *     5. 9 is bigger than 8
        *
        *             5            5            5
        *   5  ->    / \     ->   / \     ->   / \
        *           3   8        3   8        3   8
        *                       /            /     \
        *                      1            1       9
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "9", "9v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 5);


            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 2);

            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 2);

            xassert_false(tree->root->right->left);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->size == 1);

            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->right->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->right->value, "9v") == 0);
            xassert(tree->root->right->right->size == 1);

            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add more (key, value) pairs :
        *     1. 4 is bigger than 3
        *     2. 6 is lesser than 8
        *
        *             5            5            5               5                  5
        *   5  ->    / \     ->   / \     ->   / \    ->    /       \     ->    /     \
        *           3   8        3   8        3   8        3         8         3       8
        *                       /            /     \     /   \        \       /  \    / \
        *                      1            1       9   1     4        9     1    4  6   9
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "9", "9v"));
            xassert(xavltree_put_repeat(tree, "4", "4v"));
            xassert(xavltree_put_repeat(tree, "6", "6v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 7);


            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 3);


            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 3);


            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->size == 1);

            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "4") == 0);
            xassert(strcmp(tree->root->left->right->value, "4v") == 0);
            xassert(tree->root->left->right->size == 1);

            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(strcmp(tree->root->right->left->key, "6") == 0);
            xassert(strcmp(tree->root->right->left->value, "6v") == 0);
            xassert(tree->root->right->left->size == 1);

            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(strcmp(tree->root->right->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->right->value, "9v") == 0);
            xassert(tree->root->right->right->size == 1);

            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add more (key, value) pairs :
        *          9                   5
        *        /   \               /   \
        *       5     95    ->      3     9
        *      / \                 /     / \
        *     3   8               1     8   95
        *    /
        *   1
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "9", "9v"));
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "95", "95v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 6);


            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 2);


            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->value, "9v") == 0);
            xassert(tree->root->right->size == 3);


            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->size == 1);

            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->right->left->key, "8") == 0);
            xassert(strcmp(tree->root->right->left->value, "8v") == 0);
            xassert(tree->root->right->left->size == 1);

            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(strcmp(tree->root->right->right->key, "95") == 0);
            xassert(strcmp(tree->root->right->right->value, "95v") == 0);
            xassert(tree->root->right->right->size == 1);

            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add more (key, value) pairs :
        *          9                   5
        *        /   \               /   \
        *       5     95    ->      3      9
        *      / \                   \    / \
        *     3   8                   4  8   95
        *      \
        *       4
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "9", "9v"));
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "95", "95v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "4", "4v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 6);


            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 2);


            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->value, "9v") == 0);
            xassert(tree->root->right->size == 3);


            xassert(strcmp(tree->root->left->right->key, "4") == 0);
            xassert(strcmp(tree->root->left->right->value, "4v") == 0);
            xassert(tree->root->left->right->size == 1);

            xassert_false(tree->root->left->left);

            xassert(strcmp(tree->root->right->left->key, "8") == 0);
            xassert(strcmp(tree->root->right->left->value, "8v") == 0);
            xassert(tree->root->right->left->size == 1);

            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(strcmp(tree->root->right->right->key, "95") == 0);
            xassert(strcmp(tree->root->right->right->value, "95v") == 0);
            xassert(tree->root->right->right->size == 1);

            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add more (key, value) pairs :
        *          9                   9                  7
        *        /   \               /   \              /   \
        *       5     95    ->      7     95   ->      5     9
        *      / \                 /                  / \     \
        *     3   7               5                  3   6     95
        *        /               / \
        *       6               3   6
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "9", "9v"));
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "95", "95v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "7", "7v"));
            xassert(xavltree_put_repeat(tree, "6", "6v"));

            xassert(strcmp(tree->root->key, "7") == 0);
            xassert(strcmp(tree->root->value, "7v") == 0);
            xassert(tree->root->size == 6);


            xassert(strcmp(tree->root->left->key, "5") == 0);
            xassert(strcmp(tree->root->left->value, "5v") == 0);
            xassert(tree->root->left->size == 3);


            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->value, "9v") == 0);
            xassert(tree->root->right->size == 2);


            xassert(strcmp(tree->root->left->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->left->value, "3v") == 0);
            xassert(tree->root->left->left->size == 1);

            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "6") == 0);
            xassert(strcmp(tree->root->left->right->value, "6v") == 0);
            xassert(tree->root->left->right->size == 1);

            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(strcmp(tree->root->right->right->key, "95") == 0);
            xassert(strcmp(tree->root->right->right->value, "95v") == 0);
            xassert(tree->root->right->right->size == 1);

            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add more (key, value) pairs :
        *          9                   9                  7
        *        /   \               /   \              /   \
        *       5     95    ->      7     95   ->      5     9
        *      / \                 / \                /     /  \
        *     3   7               5   8              3     8   95
        *          \             /
        *           8           3
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "9", "9v"));
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "95", "95v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "7", "7v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));

            xassert(strcmp(tree->root->key, "7") == 0);
            xassert(strcmp(tree->root->value, "7v") == 0);
            xassert(tree->root->size == 6);


            xassert(strcmp(tree->root->left->key, "5") == 0);
            xassert(strcmp(tree->root->left->value, "5v") == 0);
            xassert(tree->root->left->size == 2);


            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->value, "9v") == 0);
            xassert(tree->root->right->size == 3);


            xassert(strcmp(tree->root->left->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->left->value, "3v") == 0);
            xassert(tree->root->left->left->size == 1);

            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->right->left->key, "8") == 0);
            xassert(strcmp(tree->root->right->left->value, "8v") == 0);
            xassert(tree->root->right->left->size == 1);

            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(strcmp(tree->root->right->right->key, "95") == 0);
            xassert(strcmp(tree->root->right->right->value, "95v") == 0);
            xassert(tree->root->right->right->size == 1);

            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add more (key, value) pairs :
        *         2                                 6
        *     /       \                         /       \
        *    1         6                       2         8
        *   /      /      \         -->       /  \      /  \
        *  0      4        8                 1    4    7    9
        *        / \      / \               /    / \         \
        *       3   5    7   9             0    3   5         95
        *                     \
        *                     95
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "2", "2v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "6", "6v"));
            xassert(xavltree_put_repeat(tree, "0", "0v"));
            xassert(xavltree_put_repeat(tree, "4", "4v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "7", "7v"));
            xassert(xavltree_put_repeat(tree, "9", "9v"));
            xassert(xavltree_put_repeat(tree, "95", "95v"));

            xassert(strcmp(tree->root->key, "6") == 0);
            xassert(strcmp(tree->root->value, "6v") == 0);
            xassert(tree->root->size == 11);


            xassert(strcmp(tree->root->left->key, "2") == 0);
            xassert(tree->root->left->size == 6);


            xassert(strcmp(tree->root->left->left->key, "1") == 0);

            xassert(strcmp(tree->root->left->left->left->key, "0") == 0);


            xassert(strcmp(tree->root->left->right->key, "4") == 0);

            xassert(strcmp(tree->root->left->right->left->key, "3") == 0);

            xassert(strcmp(tree->root->left->right->right->key, "5") == 0);


            xassert(strcmp(tree->root->right->key, "8") == 0);


            xassert(strcmp(tree->root->right->left->key, "7") == 0);


            xassert(strcmp(tree->root->right->right->key, "9") == 0);

            xassert(strcmp(tree->root->right->right->right->key, "95") == 0);


            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* add many more (key, value) pairs : */
        {
            XAVLTree_PT tree = xavltree_random_string(test_cmpk, NULL, 5000, 3, false);
            //xavltree_print(tree, 5);
            xassert(xavltree_is_avltree(tree));
            xavltree_deep_free(&tree);
        }
    }

    /* xavltree_put_unique */
    {
        XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);

            xassert(xavltree_put_unique(tree, "1", NULL));
            xassert(xavltree_size(tree) == 1);

            xassert(xavltree_put_unique(tree, "1", NULL));
            xassert(xavltree_put_unique(tree, "1", NULL));
            xassert(xavltree_put_unique(tree, "1", NULL));
            xassert(xavltree_put_unique(tree, "1", NULL));
            xassert(xavltree_size(tree) == 1);

            xassert(xavltree_put_unique(tree, "2", NULL));
            xassert(xavltree_put_unique(tree, "3", NULL));
            xassert(xavltree_put_unique(tree, "2", NULL));
            xassert(xavltree_size(tree) == 3);

            xassert(xavltree_put_unique(tree, "7", NULL));
            xassert(xavltree_put_unique(tree, "4", NULL));
            xassert(xavltree_put_unique(tree, "5", NULL));
            xassert(xavltree_put_unique(tree, "6", NULL));
            xassert(xavltree_put_unique(tree, "7", NULL));
            xassert(xavltree_put_unique(tree, "5", NULL));
            xassert(xavltree_put_unique(tree, "8", NULL));
            xassert(xavltree_put_unique(tree, "7", NULL));
            xassert(xavltree_put_unique(tree, "5", NULL));
            xassert(xavltree_put_unique(tree, "9", NULL));
            xassert(xavltree_size(tree) == 9);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
    }

    /* xavltree_put_replace */
    {
        XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);

        xassert(xavltree_put_replace(tree, "1", NULL, NULL));
        xassert(xavltree_size(tree) == 1);

        xassert(xavltree_put_replace(tree, "1", "2", NULL));
        xassert(xavltree_put_replace(tree, "1", "3", NULL));
        xassert(xavltree_put_replace(tree, "1", "2", NULL));
        xassert(xavltree_put_replace(tree, "1", "4", NULL));
        xassert(xavltree_size(tree) == 1);
        xassert(strcmp(xavltree_get(tree, "1"), "4") == 0);

        xassert(xavltree_put_replace(tree, "2", "2", NULL));
        xassert(xavltree_put_replace(tree, "3", NULL, NULL));
        xassert(xavltree_put_replace(tree, "2", "1", NULL));
        xassert(strcmp(xavltree_get(tree, "2"), "1") == 0);
        xassert(xavltree_size(tree) == 3);

        xassert(xavltree_put_replace(tree, "4", NULL, NULL));
        xassert(xavltree_put_replace(tree, "5", NULL, NULL));
        xassert(xavltree_put_replace(tree, "6", NULL, NULL));
        xassert(xavltree_put_replace(tree, "7", NULL, NULL));
        xassert(xavltree_put_replace(tree, "2", "3", NULL));
        xassert(xavltree_put_replace(tree, "8", NULL, NULL));
        xassert(xavltree_put_replace(tree, "9", NULL, NULL));
        xassert(strcmp(xavltree_get(tree, "2"), "3") == 0);
        xassert(xavltree_size(tree) == 9);

        xassert(xavltree_is_avltree(tree));
        xavltree_free(&tree);
    }

    /* xavltree_put_deep_replace */
    {
        XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
        char *value1 = XMEM_CALLOC(1, 10);
        char *value2 = XMEM_CALLOC(1, 10);
        char *value3 = XMEM_CALLOC(1, 10);
        memcpy(value1, "1", 1);
        memcpy(value2, "2", 1);
        memcpy(value3, "3", 1);

        xassert(xavltree_put_deep_replace(tree, "1", NULL));
        xassert(xavltree_put_deep_replace(tree, "2", NULL));
        xassert(xavltree_put_deep_replace(tree, "3", NULL));
        xassert(xavltree_put_deep_replace(tree, "4", NULL));
        xassert(xavltree_put_deep_replace(tree, "5", NULL));
        xassert(xavltree_put_deep_replace(tree, "6", NULL));
        xassert(xavltree_put_deep_replace(tree, "7", NULL));
        xassert(xavltree_put_deep_replace(tree, "8", NULL));
        xassert(xavltree_put_deep_replace(tree, "9", NULL));

        xassert(xavltree_put_deep_replace(tree, "1", value1));
        xassert(strcmp(xavltree_get(tree, "1"), "1") == 0);
        xassert(xavltree_put_deep_replace(tree, "1", value2));
        xassert(strcmp(xavltree_get(tree, "1"), "2") == 0);
        xassert(xavltree_put_deep_replace(tree, "1", value3));
        xassert(strcmp(xavltree_get(tree, "1"), "3") == 0);
        xassert(xavltree_put_deep_replace(tree, "1", "5"));
        xassert(strcmp(xavltree_get(tree, "1"), "5") == 0);

        xassert(xavltree_size(tree) == 9);

        xassert(xavltree_is_avltree(tree));
        xavltree_free(&tree);
    }

    {
        XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);

        xassert(xavltree_put_repeat(tree, "a", NULL));
        xassert(xavltree_put_repeat(tree, "b", NULL));
        xassert(xavltree_put_repeat(tree, "c", NULL));
        xassert(xavltree_put_repeat(tree, "d", NULL));
        xassert(xavltree_put_repeat(tree, "e", NULL));
        xassert(xavltree_put_repeat(tree, "f", NULL));
        xassert(xavltree_put_repeat(tree, "g", "vg"));
        xassert(xavltree_put_repeat(tree, "h", NULL));
        xassert(xavltree_put_repeat(tree, "i", NULL));
        xassert(xavltree_put_repeat(tree, "j", NULL));
        xassert(xavltree_put_repeat(tree, "k", "vk"));
        xassert(xavltree_put_repeat(tree, "l", NULL));
        xassert(xavltree_put_repeat(tree, "m", NULL));
        xassert(xavltree_put_repeat(tree, "n", NULL));
        xassert(xavltree_put_repeat(tree, "p", NULL));
        xassert(xavltree_put_repeat(tree, "q", NULL));
        xassert(xavltree_put_repeat(tree, "r", "vr"));
        xassert(xavltree_put_repeat(tree, "t", NULL));
        xassert(xavltree_put_repeat(tree, "u", NULL));
        xassert(xavltree_put_repeat(tree, "v", NULL));
        xassert(xavltree_put_repeat(tree, "w", NULL));
        xassert(xavltree_put_repeat(tree, "v", NULL));
        xassert(xavltree_put_repeat(tree, "y", NULL));
        xassert(xavltree_put_repeat(tree, "z", NULL));

        /* xavltree_min */
        xassert(strcmp(xavltree_min(tree), "a") == 0);
        /* xavltree_max */
        xassert(strcmp(xavltree_max(tree), "z") == 0);

        /* xavltree_floor */
        xassert(strcmp(xavltree_floor(tree, "o"), "n") == 0);
        /* xavltree_ceiling */
        xassert(strcmp(xavltree_ceiling(tree, "s"), "t") == 0);

        /* xavltree_select */
        xassert(strcmp(xavltree_select(tree, 0), "a") == 0);
        xassert(strcmp(xavltree_select(tree, 5), "f") == 0);
        xassert(strcmp(xavltree_select(tree, 15), "q") == 0);
        xassert(strcmp(xavltree_select(tree, 23), "z") == 0);

        /* xavltree_rank */
        xassert(xavltree_rank(tree, "e") == 4);
        xassert(xavltree_rank(tree, "m") == 12);
        xassert(xavltree_rank(tree, "y") == 22);

        /* xavltree_get */
        xassert(strcmp(xavltree_get(tree, "g"), "vg") == 0);
        xassert(strcmp(xavltree_get(tree, "k"), "vk") == 0);
        xassert(strcmp(xavltree_get(tree, "r"), "vr") == 0);

        /* xavltree_find */
        xassert_false(xavltree_find(tree, "x"));
        xassert(xavltree_find(tree, "y"));

        xassert(xavltree_put_repeat(tree, "g", "vg2"));
        xassert(xavltree_put_repeat(tree, "k", "vk2"));
        xassert(xavltree_put_repeat(tree, "g", "vg3"));
        xassert(xavltree_put_repeat(tree, "k", "vk3"));
        xassert(xavltree_put_repeat(tree, "g", "vg4"));

        xassert(xavltree_is_avltree(tree));

        /* xavltree_clear */
        xavltree_clear(tree);
        xavltree_free(&tree);
    }

    /* xavltree_remove_min */
    {
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xavltree_put_repeat(tree, "h", NULL);
            xavltree_remove_min(tree);
            xassert_false(tree->root);
            xavltree_free(&tree);
        }

        {
            /*      h
            *      /   ->   h
            *     d
            */
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xavltree_put_repeat(tree, "h", NULL);
            xavltree_put_repeat(tree, "d", NULL);
            xavltree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 1);
            xassert(strcmp(tree->root->key, "h") == 0);
            xassert_false(tree->root->left);

            xassert(xavltree_is_avltree(tree));
            xavltree_remove_min(tree);
            xassert_false(tree->root);
            xavltree_free(&tree);
        }

        {
            /*      h
            *        \     ->   m
            *         m
            */
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xavltree_put_repeat(tree, "h", NULL);
            xavltree_put_repeat(tree, "m", NULL);
            xavltree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 1);
            xassert(strcmp(tree->root->key, "m") == 0);
            xassert_false(tree->root->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_remove_min(tree);
            xassert_false(tree->root);
            xavltree_free(&tree);
        }

        {
            /*      h             h          h
            *      / \    ->    /   \    ->   \
            *     d   m        d     m         m
            */
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xavltree_put_repeat(tree, "h", NULL);
            xavltree_put_repeat(tree, "d", NULL);
            xavltree_put_repeat(tree, "m", NULL);
            xavltree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->left);

            xassert(xavltree_is_avltree(tree));
            xavltree_remove_min(tree);
            xavltree_remove_min(tree);
            xassert_false(tree->root);
            xavltree_free(&tree);
        }

        {
            /*      h          h
            *      / \        / \
            *     d   m   -> d   m
            *    /
            *   a
            */
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xavltree_put_repeat(tree, "h", NULL);
            xavltree_put_repeat(tree, "d", NULL);
            xavltree_put_repeat(tree, "m", NULL);
            xavltree_put_repeat(tree, "a", NULL);
            xavltree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 3);
            xassert(tree->root->left->size == 1);

            xassert(xavltree_is_avltree(tree));
            xavltree_remove_min(tree);
            xavltree_remove_min(tree);
            xavltree_remove_min(tree);
            xassert_false(tree->root);
            xavltree_free(&tree);
        }

        {
            /*      h            h
            *      / \         /   \
            *     d   m   ->  f     m
            *      \
            *       f
            */
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xavltree_put_repeat(tree, "h", NULL);
            xavltree_put_repeat(tree, "d", NULL);
            xavltree_put_repeat(tree, "m", NULL);
            xavltree_put_repeat(tree, "f", NULL);
            xavltree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 3);
            xassert(tree->root->left->size == 1);

            xassert(xavltree_is_avltree(tree));
            xavltree_remove_min(tree);
            xavltree_remove_min(tree);
            xavltree_remove_min(tree);
            xassert_false(tree->root);
            xavltree_free(&tree);
        }

        {
            /*       h              h
            *      /  \           /   \
            *     d    m   ->    d     m
            *    /  \             \
            *   a    f             f
            */
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xavltree_put_repeat(tree, "h", NULL);
            xavltree_put_repeat(tree, "d", NULL);
            xavltree_put_repeat(tree, "m", NULL);
            xavltree_put_repeat(tree, "a", NULL);
            xavltree_put_repeat(tree, "f", NULL);
            xavltree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 4);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->left);

            xassert(xavltree_is_avltree(tree));
            xavltree_remove_min(tree);
            xavltree_remove_min(tree);
            xavltree_remove_min(tree);
            xavltree_remove_min(tree);
            xassert_false(tree->root);
            xavltree_free(&tree);
        }

        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            for (int i = 0; i < 1000; ++i) {
                xavltree_put_repeat(tree, "a", NULL);
                xavltree_put_repeat(tree, "b", NULL);
                xavltree_put_repeat(tree, "c", NULL);
            }

            for (int i = 0; i < 1000; ++i) {
                xavltree_remove_min(tree);
                xavltree_remove_min(tree);
                xavltree_remove_min(tree);
                if (i % 500 == 0) {
                    xassert(xavltree_is_avltree(tree));
                }
            }

            xassert(xavltree_size(tree) == 0);
            xavltree_deep_free(&tree);
        }
    }

    /* xavltree_remove_max */
    {
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xavltree_put_repeat(tree, "h", NULL);
            xavltree_remove_max(tree);
            xassert_false(tree->root);
            xavltree_free(&tree);
        }

        {
            /*      h
            *      /   ->   d
            *     d
            */
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xavltree_put_repeat(tree, "h", NULL);
            xavltree_put_repeat(tree, "d", NULL);
            xavltree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 1);
            xassert(strcmp(tree->root->key, "d") == 0);
            xassert_false(tree->root->left);

            xassert(xavltree_is_avltree(tree));
            xavltree_remove_max(tree);
            xassert_false(tree->root);
            xavltree_free(&tree);
        }

        {
            /*      h
            *        \     ->   h
            *         m
            */
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xavltree_put_repeat(tree, "h", NULL);
            xavltree_put_repeat(tree, "m", NULL);
            xavltree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 1);
            xassert(strcmp(tree->root->key, "h") == 0);
            xassert_false(tree->root->right);

            xavltree_remove_min(tree);
            xassert_false(tree->root);
            xavltree_free(&tree);
        }

        {
            /*      h             h            h
            *      / \    ->    /   \    ->   /
            *     d   m        d     m       d
            */
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xavltree_put_repeat(tree, "h", NULL);
            xavltree_put_repeat(tree, "d", NULL);
            xavltree_put_repeat(tree, "m", NULL);
            xavltree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_remove_max(tree);
            xavltree_remove_max(tree);
            xassert_false(tree->root);
            xavltree_free(&tree);
        }

        {
            /*      h          h
            *      / \        / \
            *     d   m   -> d   m
            *          \
            *           n
            */
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xavltree_put_repeat(tree, "h", NULL);
            xavltree_put_repeat(tree, "d", NULL);
            xavltree_put_repeat(tree, "m", NULL);
            xavltree_put_repeat(tree, "n", NULL);
            xavltree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 3);
            xassert(tree->root->right->size == 1);

            xassert(xavltree_is_avltree(tree));
            xavltree_remove_max(tree);
            xavltree_remove_max(tree);
            xavltree_remove_max(tree);
            xassert_false(tree->root);
            xavltree_free(&tree);
        }

        {
            /*      h            h
            *      / \          / \
            *     d   m   ->   d   k
            *        /
            *       k
            */
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xavltree_put_repeat(tree, "h", NULL);
            xavltree_put_repeat(tree, "d", NULL);
            xavltree_put_repeat(tree, "m", NULL);
            xavltree_put_repeat(tree, "k", NULL);
            xavltree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 3);
            xassert(tree->root->right->size == 1);

            xassert(xavltree_is_avltree(tree));
            xavltree_remove_max(tree);
            xavltree_remove_max(tree);
            xavltree_remove_max(tree);
            xassert_false(tree->root);
            xavltree_free(&tree);
        }

        {
            /*      h             h
            *      / \          /   \
            *     d   m   ->   d     m
            *    /     \      /
            *   a       n    a
            */
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xavltree_put_repeat(tree, "h", NULL);
            xavltree_put_repeat(tree, "d", NULL);
            xavltree_put_repeat(tree, "m", NULL);
            xavltree_put_repeat(tree, "a", NULL);
            xavltree_put_repeat(tree, "n", NULL);
            xavltree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 4);
            xassert(tree->root->right->size == 1);

            xassert(xavltree_is_avltree(tree));
            xavltree_remove_max(tree);
            xavltree_remove_max(tree);
            xavltree_remove_max(tree);
            xavltree_remove_max(tree);
            xassert_false(tree->root);
            xavltree_free(&tree);
        }

        {
            /*       h               h
            *      /  \            /   \
            *     d    m   ->    d      m
            *         /  \             /
            *        k    n           k
            */
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xavltree_put_repeat(tree, "h", NULL);
            xavltree_put_repeat(tree, "d", NULL);
            xavltree_put_repeat(tree, "m", NULL);
            xavltree_put_repeat(tree, "k", NULL);
            xavltree_put_repeat(tree, "n", NULL);
            xavltree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 4);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_remove_max(tree);
            xavltree_remove_max(tree);
            xavltree_remove_max(tree);
            xavltree_remove_max(tree);
            xassert_false(tree->root);
            xavltree_free(&tree);
        }

        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            for (int i = 0; i < 500; ++i) {
                xavltree_put_repeat(tree, "a", NULL);
                xavltree_put_repeat(tree, "b", NULL);
                xavltree_put_repeat(tree, "c", NULL);
            }

            for (int i = 0; i < 500; ++i) {
                xavltree_remove_max(tree);
                xavltree_remove_max(tree);
                xavltree_remove_max(tree);
                if (i % 100 == 0) {
                    xassert(xavltree_is_avltree(tree));
                }
            }

            xassert(xavltree_size(tree) == 0);
            xavltree_deep_free(&tree);
        }
    }

    /* xavltree_find */
    {
    }

    /* xavltree_clear */
    {
    }

    /* xavltree_deep_clear */
    {
    }

    /* xavltree_free */
    {
    }

    /* xavltree_deep_free */
    {
    }

    /* xavltree_remove */
    {
        /* remove one key :
        *
        *         5
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xavltree_remove(tree, "5");
            xassert(xavltree_size(tree) == 0);
            xassert_false(tree->root);
            xavltree_free(&tree);
        }

        /* remove one of two keys :
        *     1. remove the bigger one
        *
        *      5       3
        *     /    ->
        *    3
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xavltree_remove(tree, "5");
            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->value, "3v") == 0);
            xassert(tree->root->size == 1);
            xassert_false(tree->root->left);
            xassert_false(tree->root->right);
            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of two keys :
        *     2. remove the less one
        *
        *      5       5
        *     /    ->
        *    3
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xavltree_remove(tree, "3");
            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 1);
            xassert_false(tree->root->left);
            xassert_false(tree->root->right);
            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of three keys :
        *     1. remove 1
        *
        *     3           3
        *    / \    ->     \
        *   1   5           5
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xavltree_remove(tree, "1");

            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->value, "3v") == 0);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->left);

            xassert(strcmp(tree->root->right->key, "5") == 0);
            xassert(strcmp(tree->root->right->value, "5v") == 0);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);
            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of three keys :
        *     2. remove 3
        *
        *     3           1
        *    / \    ->     \
        *   1   5           5
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "5", "5v"));

            xavltree_remove(tree, "3");

            xassert(strcmp(tree->root->key, "1") == 0);
            xassert(strcmp(tree->root->value, "1v") == 0);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->left);

            xassert(strcmp(tree->root->right->key, "5") == 0);
            xassert(strcmp(tree->root->right->value, "5v") == 0);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of three keys :
        *     2. remove 5
        *
        *     3           3
        *    / \    ->   /
        *   1   5       1
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "5", "5v"));

            xavltree_remove(tree, "5");

            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->value, "3v") == 0);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->right);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of four keys :
        *     1. remove 1
        *
        *       5           5
        *      / \         / \
        *     3   8   ->  3   8
        *    /
        *   1
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));

            xavltree_remove(tree, "1");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of four keys :
        *     2. remove 3
        *
        *       5           5
        *      / \         / \
        *     3   8   ->  1   8
        *    /
        *   1
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));

            xavltree_remove(tree, "3");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of four keys :
        *     3. remove 5
        *
        *        5           3
        *       / \         / \
        *      3   8   ->  1   8
        *     /
        *   1
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));

            xavltree_remove(tree, "5");

            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->value, "3v") == 0);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of four keys :
        *     4. remove 8
        *
        *        5           3
        *       / \         / \
        *      3   8   ->  1   5
        *     /
        *    1
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));

            xavltree_remove(tree, "8");

            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->value, "3v") == 0);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "5") == 0);
            xassert(strcmp(tree->root->right->value, "5v") == 0);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of four keys :
        *     5. remove 7
        *
        *        5           5
        *       / \         / \
        *      3   8   ->  3   8
        *         /
        *        7
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "7", "7v"));

            xavltree_remove(tree, "7");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of five keys :
        *     1. remove 0
        *
        *       5             5
        *     /   \         /   \
        *    1     8  ->   1     8
        *   /  \            \
        *  0    3            3
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "0", "0v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));

            xavltree_remove(tree, "0");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->left);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->right->key, "3") == 0);
            xassert(strcmp(tree->root->left->right->value, "3v") == 0);
            xassert(tree->root->left->right->size == 1);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of five keys :
        *     2. remove 1
        *
        *       5             5
        *     /   \         /   \
        *    1     8  ->   0     8
        *   /  \            \
        *  0    3            3
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "0", "0v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));

            xavltree_remove(tree, "1");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "0") == 0);
            xassert(strcmp(tree->root->left->value, "0v") == 0);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->left);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->right->key, "3") == 0);
            xassert(strcmp(tree->root->left->right->value, "3v") == 0);
            xassert(tree->root->left->right->size == 1);
            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of five keys :
        *     3. remove 3
        *
        *       5             5
        *     /   \         /   \
        *    1     8  ->   1     8
        *   /  \          /
        *  0    3        0
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "0", "0v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));

            xavltree_remove(tree, "3");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "0") == 0);
            xassert(strcmp(tree->root->left->left->value, "0v") == 0);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of five keys :
        *     4. remove 5
        *
        *       5             3
        *     /   \         /   \
        *    1     8  ->   1     8
        *   /  \          /
        *  0    3        0
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "0", "0v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));

            xavltree_remove(tree, "5");

            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->value, "3v") == 0);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "0") == 0);
            xassert(strcmp(tree->root->left->left->value, "0v") == 0);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of five keys :
        *     5. remove 8
        *
        *       5             1
        *     /   \         /   \
        *    1     8  ->   0     5
        *   /  \                /
        *  0    3              3
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "0", "0v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));

            xavltree_remove(tree, "8");

            xassert(strcmp(tree->root->key, "1") == 0);
            xassert(strcmp(tree->root->value, "1v") == 0);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "0") == 0);
            xassert(strcmp(tree->root->left->value, "0v") == 0);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "5") == 0);
            xassert(strcmp(tree->root->right->value, "5v") == 0);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->right->left->key, "3") == 0);
            xassert(strcmp(tree->root->right->left->value, "3v") == 0);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of five keys :
        *     6. remove 6
        *
        *        6               5
        *      /   \           /   \
        *     5     9   ->    1     9
        *    /     /               /
        *   1     8               8
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "6", "6v"));
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "9", "9v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));

            xavltree_remove(tree, "6");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->value, "9v") == 0);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->right->left->key, "8") == 0);
            xassert(strcmp(tree->root->right->left->value, "8v") == 0);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of five keys :
        *     6. remove 9
        *
        *        6               6
        *      /   \           /   \
        *     5     9   ->    5     8
        *    /     /         /
        *   1     8         1
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "6", "6v"));
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "9", "9v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));

            xavltree_remove(tree, "9");

            xassert(strcmp(tree->root->key, "6") == 0);
            xassert(strcmp(tree->root->value, "6v") == 0);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "5") == 0);
            xassert(strcmp(tree->root->left->value, "5v") == 0);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of five keys :
        *     6. remove 8
        *
        *        6               6
        *      /   \           /   \
        *     5     9   ->    5     9
        *    /     /         /
        *   1     8         1
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "6", "6v"));
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "9", "9v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));

            xavltree_remove(tree, "8");

            xassert(strcmp(tree->root->key, "6") == 0);
            xassert(strcmp(tree->root->value, "6v") == 0);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "5") == 0);
            xassert(strcmp(tree->root->left->value, "5v") == 0);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->value, "9v") == 0);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of more keys :
        *     1. remove 5
        *
        *       5                     4
        *     /    \               /      \
        *    3      9      ->     3        9
        *   / \    / \           /        / \
        *  1   4  8  95         1        8  95
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "9", "9v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "4", "4v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "95", "95v"));

            xavltree_remove(tree, "5");

            xassert(strcmp(tree->root->key, "4") == 0);
            xassert(strcmp(tree->root->value, "4v") == 0);
            xassert(tree->root->size == 6);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 2);

            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->value, "9v") == 0);
            xassert(tree->root->right->size == 3);
            xassert(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->right->left->key, "8") == 0);
            xassert(strcmp(tree->root->right->left->value, "8v") == 0);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(strcmp(tree->root->right->right->key, "95") == 0);
            xassert(strcmp(tree->root->right->right->value, "95v") == 0);
            xassert(tree->root->right->right->size == 1);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        /* remove one of more keys :
        *     2. remove 95
        *
        *       5                     5
        *     /    \               /     \
        *    3      9      ->     3       9
        *   / \    / \           / \     /
        *  1   4  8  95         1   4   8
        */
        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            xassert(xavltree_put_repeat(tree, "5", "5v"));
            xassert(xavltree_put_repeat(tree, "3", "3v"));
            xassert(xavltree_put_repeat(tree, "9", "9v"));
            xassert(xavltree_put_repeat(tree, "1", "1v"));
            xassert(xavltree_put_repeat(tree, "4", "4v"));
            xassert(xavltree_put_repeat(tree, "8", "8v"));
            xassert(xavltree_put_repeat(tree, "95", "95v"));

            xavltree_remove(tree, "95");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->size == 6);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->size == 3);

            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->value, "9v") == 0);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "4") == 0);
            xassert(strcmp(tree->root->left->right->value, "4v") == 0);
            xassert(tree->root->left->right->size == 1);
            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(strcmp(tree->root->right->left->key, "8") == 0);
            xassert(strcmp(tree->root->right->left->value, "8v") == 0);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(xavltree_is_avltree(tree));
            xavltree_free(&tree);
        }

        {
            XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);
            for (int i = 0; i < 1000; ++i) {
                xavltree_put_repeat(tree, "a", NULL);
                xavltree_put_repeat(tree, "b", NULL);
                xavltree_put_repeat(tree, "c", NULL);
                xavltree_put_repeat(tree, "d", NULL);
                xavltree_put_repeat(tree, "e", NULL);
                xavltree_put_repeat(tree, "f", NULL);
            }

            for (int i = 0; i < 1000; ++i) {
                xavltree_remove(tree, "a");
                xavltree_remove(tree, "b");
                xavltree_remove(tree, "c");
                xavltree_remove(tree, "d");
                xavltree_remove(tree, "e");
                xavltree_remove(tree, "f");
                if (i % 100 == 0) {
                    xassert(xavltree_is_avltree(tree));
                }
            }

            xassert(xavltree_size(tree) == 0);
            xavltree_free(&tree);
        }
    }

    {
        XAVLTree_PT tree = xavltree_random_string(test_cmpk, NULL, 10, 2, false);
        //xavltree_print(tree, 5);

        /* xavltree_map_preorder */
        //xavltree_map_preorder(tree, xavltree_test_map_applykv, NULL);

        /* xavltree_map_inorder */
        //xavltree_map_inorder(tree, xavltree_test_map_applykv, NULL);

        /* xavltree_map_postorder */
        //xavltree_map_postorder(tree, xavltree_test_map_applykv, NULL);

        /* xavltree_map_levelorder */
        //xavltree_map_levelorder(tree, xavltree_test_map_applykv, NULL);

        /* xavltree_map_min_to_max */
        //xavltree_map_min_to_max(tree, xavltree_test_map_applykv, NULL);

        /* xavltree_map_max_to_min */
        //xavltree_map_max_to_min(tree, xavltree_test_map_applykv, NULL);

        /* xavltree_map_key_min_to_max */
        //xavltree_map_key_min_to_max(tree, xavltree_test_map_applyk, NULL);

        /* xavltree_map_key_max_to_min */
        //xavltree_map_key_max_to_min(tree, xavltree_test_map_applyk, NULL);

        xavltree_deep_free(&tree);
    }

    {
        XAVLTree_PT tree = xavltree_new(test_cmpk, NULL);

        xassert(xavltree_put_repeat(tree, "a", NULL));
        xassert(xavltree_put_repeat(tree, "a", NULL));
        xassert(xavltree_put_repeat(tree, "a", NULL));
        xassert(xavltree_put_repeat(tree, "a", NULL));
        xassert(xavltree_put_repeat(tree, "a", NULL));

        /* xavltree_map_min_to_max_break_if_true */
        xassert_false(xavltree_map_min_to_max_break_if_true(tree, xavltree_test_map_applykv_false, NULL));
        xassert_false(xavltree_map_min_to_max_break_if_true(tree, xavltree_test_map_cmpkv, "x"));
        xassert(xavltree_map_min_to_max_break_if_true(tree, xavltree_test_map_cmpkv, "a"));

        /* xavltree_map_min_to_max_break_if_false */
        xassert_false(xavltree_map_min_to_max_break_if_false(tree, xavltree_test_map_applykv_true, NULL));
        xassert_false(xavltree_map_min_to_max_break_if_false(tree, xavltree_test_map_cmpkv, "a"));
        xassert(xavltree_map_min_to_max_break_if_false(tree, xavltree_test_map_cmpkv, "b"));

        xavltree_free(&tree);
    }

    /* xavltree_swap */
    {
    }

    /* xavltree_size */
    {
    }

    /* xavltree_is_empty */
    {
    }

    /* xavltree_height */
    {
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
