
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../tree_redblack/xtree_redblack_x.h"
#include "../include/xalgos.h"

static
int test_cmpk(void *key1, void *key2, void *cl) {
    return strcmp((char*)key1, (char*)key2);
}

static
bool xrbtree_test_map_applykv_false(void *key, void **value, void *cl) {
    return false;
}

static
bool xrbtree_test_map_applykv_true(void *key, void **value, void *cl) {
    return true;
}

static
bool xrbtree_test_map_applyk_false(void *key, void *cl) {
    return false;
}

static
bool xrbtree_test_map_applyk_true(void *key, void *cl) {
    return true;
}

static
bool xrbtree_test_map_cmpkv(void *key, void **value, void *cl) {
    return strcmp((char*)key, (char*)cl) == 0;
}

static
bool xrbtree_test_map_cmpk(void *key, void *cl) {
    return strcmp((char*)key, (char*)cl) == 0;
}

static
bool xrbtree_test_map_applykv(void *key, void **value, void *cl) {
    printf("%s\n", (char*)key);
    return true;
}

static
bool xrbtree_test_map_applyk(void *key, void *cl) {
    printf("%s\n", (char*)key);
    return true;
}

static
XRBTree_PT xrbtree_random_string(int(*cmp)(void *key1, void *key2, void *cl), void *cl, int total_size, int string_length) {
    xassert(cmp);
    xassert(0 < total_size);
    xassert(0 < string_length);

    if (!cmp || (total_size <= 0) || (string_length <= 0)) {
        return NULL;
    }

    {
        XRBTree_PT tree = xrbtree_new(cmp, cl);
        if (!tree) {
            return NULL;
        }

        {
            const char charsets[] = "0123456789";

            for (int i = 0; i < total_size; i++) {
                char* str = XMEM_MALLOC(string_length + 1);
                if (!str) {
                    xrbtree_deep_free(&tree);
                    return NULL;
                }

                for (int i = 0; i < string_length; ++i) {
                    str[i] = charsets[rand() % (sizeof(charsets) - 1)];
                }
                str[string_length] = '\0';

                if (!xrbtree_put_repeat(tree, str, NULL)) {
                    xrbtree_deep_free(&tree);
                    return NULL;
                }
            }
        }

        return tree;
    }
}

static
void xrbtree_printnode(char* key, char* value, bool color, int h) {
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
void xrbtree_print_impl(XRBTree_Node_PT node, int h) {
    if (!node) {
        xrbtree_printnode("-", "-", true, h);
        return;
    }

    xrbtree_print_impl(node->right, h + 3);

    xrbtree_printnode((char*)node->key, (char*)node->value, node->color, h);

    xrbtree_print_impl(node->left, h + 3);
}

static
void xrbtree_print(XRBTree_PT tree, int start_blank) {
    xassert(tree);
    xassert(0 <= start_blank);

    if (start_blank < 0) {
        return;
    }

    if (!tree) {
        xrbtree_printnode("-", "-", false, start_blank);
        return;
    }

    xrbtree_print_impl(tree->root, start_blank + 3);
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

void test_xrbtree() {
    /* xrbtree_new */
    {
        /* cmp == NULL */
        {
            bool except = false;

            XEXCEPT_TRY
                xrbtree_new(NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* normal */
        {
            XRBTree_PT tree = NULL;

            tree = xrbtree_new(test_cmpk, NULL);

            xassert(tree);
            xassert(tree->cmp);
            xassert_false(tree->root);

            xassert(xrbtree_size(tree) == 0);

            xrbtree_free(&tree);
        }
    }

    /* xrbtree_copy */
    {
        XRBTree_PT tree = xrbtree_random_string(test_cmpk, NULL, 50, 3);
        XRBTree_PT ntree = xrbtree_copy(tree);

        xassert(xrbtree_is_rbtree(tree));
        xassert(xrbtree_is_rbtree(ntree));

        xrbtree_deep_free(&tree);
        xrbtree_free(&ntree);
    }

    /* xrbtree_deep_copy */
    {
        XRBTree_PT tree = xrbtree_random_string(test_cmpk, NULL, 50, 3);
        XRBTree_PT ntree = xrbtree_deep_copy(tree, 4, 0);

        xassert(xrbtree_is_rbtree(tree));
        xassert(xrbtree_is_rbtree(ntree));

        xrbtree_deep_free(&tree);
        xrbtree_deep_free(&ntree);
    }

    /* xrbtree_put_repeat */
    {
        /* tree == NULL */
        {
            bool except = false;

            XEXCEPT_TRY
                xrbtree_put_repeat(NULL, "abc", "def");
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* key == NULL */
        {
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            bool except = false;

            XEXCEPT_TRY
                xrbtree_put_repeat(tree, NULL, "def");
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xrbtree_free(&tree);
        }

        /* add one (key, value) pair :
        *
        *         5
        */
        {
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 1);
            xassert_false(tree->root->parent);
            xassert_false(tree->root->left);
            xassert_false(tree->root->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
        }

        /* add two (key, value) pairs :
        *     1. second key is less than first key
        *
        *             5
        *   5  ->    /
        *          3(R)
        */
        {
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->right);
            xassert_false(tree->root->parent);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == false);
            xassert(tree->root->left->size == 1);
            xassert(strcmp(tree->root->left->parent->key, "5") == 0);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
        }

        /* add two (key, value) pairs :
        *     2. second key is larger than first key
        *
        *            5 
        *   5  ->     \
        *             8(R)
        */
        {
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->parent);
            xassert_false(tree->root->left);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == false);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));

            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->value, "3v") == 0);
            xassert_false(tree->root->parent);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert(strcmp(tree->root->left->parent->key, "3") == 0);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "5") == 0);
            xassert(strcmp(tree->root->right->value, "5v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert(strcmp(tree->root->right->parent->key, "3") == 0);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "4", "4v"));

            xassert(strcmp(tree->root->key, "4") == 0);
            xassert(strcmp(tree->root->value, "4v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "5") == 0);
            xassert(strcmp(tree->root->right->value, "5v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "6", "6v"));

            xassert(strcmp(tree->root->key, "6") == 0);
            xassert(strcmp(tree->root->value, "6v") == 0);
            xassert_false(tree->root->parent);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "5") == 0);
            xassert(strcmp(tree->root->left->value, "5v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert(strcmp(tree->root->left->parent->key, "6") == 0);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert(strcmp(tree->root->right->parent->key, "6") == 0);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "9", "9v"));

            xassert(strcmp(tree->root->key, "8") == 0);
            xassert(strcmp(tree->root->value, "8v") == 0);
            xassert_false(tree->root->parent);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "5") == 0);
            xassert(strcmp(tree->root->left->value, "5v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert(strcmp(tree->root->left->parent->key, "8") == 0);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->value, "9v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert(strcmp(tree->root->right->parent->key, "8") == 0);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
        }

        /* add three (key, value) pairs :
        *     3. 8 is bigger than 5
        *
        *             5            5             5
        *   5  ->    /     ->    /   \     ->   / \
        *          3(R)        3(R)  8(R)      3   8
        */
        {
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->color == false);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "4", "4v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->left);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->right->key, "4") == 0);
            xassert(strcmp(tree->root->left->right->value, "4v") == 0);
            xassert(tree->root->left->right->color == false);
            xassert(tree->root->left->right->size == 1);
            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "6", "6v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->right->left->key, "6") == 0);
            xassert(strcmp(tree->root->right->left->value, "6v") == 0);
            xassert(tree->root->right->left->color == false);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "9", "9v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->left);

            xassert(strcmp(tree->root->right->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->right->value, "9v") == 0);
            xassert(tree->root->right->right->color == false);
            xassert(tree->root->right->right->size == 1);
            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "0", "0v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 5);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert(tree->root->left->color == false);
            xassert(tree->root->left->size == 3);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "0") == 0);
            xassert(strcmp(tree->root->left->left->value, "0v") == 0);
            xassert(tree->root->left->left->color == true);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "3") == 0);
            xassert(strcmp(tree->root->left->right->value, "3v") == 0);
            xassert(tree->root->left->right->color == true);
            xassert(tree->root->left->right->size == 1);
            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "15", "15v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 5);

            xassert(strcmp(tree->root->left->key, "15") == 0);
            xassert(strcmp(tree->root->left->value, "15v") == 0);
            xassert(tree->root->left->color == false);
            xassert(tree->root->left->size == 3);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->color == true);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "3") == 0);
            xassert(strcmp(tree->root->left->right->value, "3v") == 0);
            xassert(tree->root->left->right->color == true);
            xassert(tree->root->left->right->size == 1);
            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "7", "7v"));
            xassert(xrbtree_put_repeat(tree, "6", "6v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 5);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);

            xassert(strcmp(tree->root->right->key, "7") == 0);
            xassert(strcmp(tree->root->right->value, "7v") == 0);
            xassert(tree->root->right->color == false);
            xassert(tree->root->right->size == 3);

            xassert(strcmp(tree->root->right->left->key, "6") == 0);
            xassert(strcmp(tree->root->right->left->value, "6v") == 0);
            xassert(tree->root->right->left->color == true);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(strcmp(tree->root->right->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->right->value, "8v") == 0);
            xassert(tree->root->right->right->color == true);
            xassert(tree->root->right->right->size == 1);
            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "6", "6v"));
            xassert(xrbtree_put_repeat(tree, "7", "7v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 5);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);

            xassert(strcmp(tree->root->right->key, "7") == 0);
            xassert(strcmp(tree->root->right->value, "7v") == 0);
            xassert(tree->root->right->color == false);
            xassert(tree->root->right->size == 3);

            xassert(strcmp(tree->root->right->left->key, "6") == 0);
            xassert(strcmp(tree->root->right->left->value, "6v") == 0);
            xassert(tree->root->right->left->color == true);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(strcmp(tree->root->right->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->right->value, "8v") == 0);
            xassert(tree->root->right->right->color == true);
            xassert(tree->root->right->right->size == 1);
            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "4", "4v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 5);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == false);
            xassert(tree->root->left->size == 3);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->color == true);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "4") == 0);
            xassert(strcmp(tree->root->left->right->value, "4v") == 0);
            xassert(tree->root->left->right->color == true);
            xassert(tree->root->left->right->size == 1);
            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "6", "6v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 5);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->color == false);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->right->left->key, "6") == 0);
            xassert(strcmp(tree->root->right->left->value, "6v") == 0);
            xassert(tree->root->right->left->color == false);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "9", "9v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 5);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->left);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->color == false);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->right->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->right->value, "9v") == 0);
            xassert(tree->root->right->right->color == false);
            xassert(tree->root->right->right->size == 1);
            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "9", "9v"));
            xassert(xrbtree_put_repeat(tree, "4", "4v"));
            xassert(xrbtree_put_repeat(tree, "6", "6v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 7);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 3);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 3);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->color == true);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "4") == 0);
            xassert(strcmp(tree->root->left->right->value, "4v") == 0);
            xassert(tree->root->left->right->color == true);
            xassert(tree->root->left->right->size == 1);
            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(strcmp(tree->root->right->left->key, "6") == 0);
            xassert(strcmp(tree->root->right->left->value, "6v") == 0);
            xassert(tree->root->right->left->color == true);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(strcmp(tree->root->right->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->right->value, "9v") == 0);
            xassert(tree->root->right->right->color == true);
            xassert(tree->root->right->right->size == 1);
            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "9", "9v"));
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "95", "95v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));

            xassert(strcmp(tree->root->key, "9") == 0);
            xassert(strcmp(tree->root->value, "9v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 6);

            xassert(strcmp(tree->root->left->key, "5") == 0);
            xassert(strcmp(tree->root->left->value, "5v") == 0);
            xassert(tree->root->left->color == false);
            xassert(tree->root->left->size == 4);

            xassert(strcmp(tree->root->right->key, "95") == 0);
            xassert(strcmp(tree->root->right->value, "95v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);

            xassert(strcmp(tree->root->left->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->left->value, "3v") == 0);
            xassert(tree->root->left->left->color == true);
            xassert(tree->root->left->left->size == 2);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "8") == 0);
            xassert(strcmp(tree->root->left->right->value, "8v") == 0);
            xassert(tree->root->left->right->color == true);
            xassert(tree->root->left->right->size == 1);
            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(strcmp(tree->root->left->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->left->value, "1v") == 0);
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
            xassert(xrbtree_put_repeat(tree, "4", "4v"));

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 7);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 3);

            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->value, "9v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 3);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->color == true);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "4") == 0);
            xassert(strcmp(tree->root->left->right->value, "4v") == 0);
            xassert(tree->root->left->right->color == true);
            xassert(tree->root->left->right->size == 1);
            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(strcmp(tree->root->right->left->key, "8") == 0);
            xassert(strcmp(tree->root->right->left->value, "8v") == 0);
            xassert(tree->root->right->left->color == true);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(strcmp(tree->root->right->right->key, "95") == 0);
            xassert(strcmp(tree->root->right->right->value, "95v") == 0);
            xassert(tree->root->right->right->color == true);
            xassert(tree->root->right->right->size == 1);
            xassert_false(tree->root->right->right->left);
            xassert_false(tree->root->right->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
        }
        
        /* add many more (key, value) pairs : */
        {
            XRBTree_PT tree = xrbtree_random_string(test_cmpk, NULL, 5000, 2);
            xassert(xrbtree_is_rbtree(tree));
            xrbtree_deep_free(&tree);
        }
    }

    /* xrbtree_put_unique */
    {
        XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);

        xassert(xrbtree_put_unique(tree, "1", NULL));
        xassert(xrbtree_size(tree) == 1);

        xassert(xrbtree_put_unique(tree, "1", NULL));
        xassert(xrbtree_put_unique(tree, "1", NULL));
        xassert(xrbtree_put_unique(tree, "1", NULL));
        xassert(xrbtree_put_unique(tree, "1", NULL));
        xassert(xrbtree_size(tree) == 1);

        xassert(xrbtree_put_unique(tree, "2", NULL));
        xassert(xrbtree_put_unique(tree, "3", NULL));
        xassert(xrbtree_put_unique(tree, "2", NULL));
        xassert(xrbtree_size(tree) == 3);

        xassert(xrbtree_put_unique(tree, "7", NULL));
        xassert(xrbtree_put_unique(tree, "4", NULL));
        xassert(xrbtree_put_unique(tree, "5", NULL));
        xassert(xrbtree_put_unique(tree, "6", NULL));
        xassert(xrbtree_put_unique(tree, "7", NULL));
        xassert(xrbtree_put_unique(tree, "5", NULL));
        xassert(xrbtree_put_unique(tree, "8", NULL));
        xassert(xrbtree_put_unique(tree, "7", NULL));
        xassert(xrbtree_put_unique(tree, "5", NULL));
        xassert(xrbtree_put_unique(tree, "9", NULL));
        xassert(xrbtree_size(tree) == 9);

        xassert(xrbtree_is_rbtree(tree));
        xrbtree_free(&tree);
    }

    /* xrbtree_put_replace */
    {
        XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);

        xassert(xrbtree_put_replace(tree, "1", NULL, NULL));
        xassert(xrbtree_size(tree) == 1);

        xassert(xrbtree_put_replace(tree, "1", "2", NULL));
        xassert(xrbtree_put_replace(tree, "1", "3", NULL));
        xassert(xrbtree_put_replace(tree, "1", "2", NULL));
        xassert(xrbtree_put_replace(tree, "1", "4", NULL));
        xassert(xrbtree_size(tree) == 1);
        xassert(strcmp(xrbtree_get(tree, "1"), "4") == 0);

        xassert(xrbtree_put_replace(tree, "2", "2", NULL));
        xassert(xrbtree_put_replace(tree, "3", NULL, NULL));
        xassert(xrbtree_put_replace(tree, "2", "1", NULL));
        xassert(strcmp(xrbtree_get(tree, "2"), "1") == 0);
        xassert(xrbtree_size(tree) == 3);

        xassert(xrbtree_put_replace(tree, "4", NULL, NULL));
        xassert(xrbtree_put_replace(tree, "5", NULL, NULL));
        xassert(xrbtree_put_replace(tree, "6", NULL, NULL));
        xassert(xrbtree_put_replace(tree, "7", NULL, NULL));
        xassert(xrbtree_put_replace(tree, "2", "3", NULL));
        xassert(xrbtree_put_replace(tree, "8", NULL, NULL));
        xassert(xrbtree_put_replace(tree, "9", NULL, NULL));
        xassert(strcmp(xrbtree_get(tree, "2"), "3") == 0);
        xassert(xrbtree_size(tree) == 9);

        xassert(xrbtree_is_rbtree(tree));
        xrbtree_free(&tree);
    }

    /* xrbtree_put_deep_replace */
    {
        XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
        char *value1 = XMEM_CALLOC(1, 10);
        char *value2 = XMEM_CALLOC(1, 10);
        char *value3 = XMEM_CALLOC(1, 10);
        memcpy(value1, "1", 1);
        memcpy(value2, "2", 1);
        memcpy(value3, "3", 1);

        xassert(xrbtree_put_deep_replace(tree, "1", NULL));
        xassert(xrbtree_put_deep_replace(tree, "2", NULL));
        xassert(xrbtree_put_deep_replace(tree, "3", NULL));
        xassert(xrbtree_put_deep_replace(tree, "4", NULL));
        xassert(xrbtree_put_deep_replace(tree, "5", NULL));
        xassert(xrbtree_put_deep_replace(tree, "6", NULL));
        xassert(xrbtree_put_deep_replace(tree, "7", NULL));
        xassert(xrbtree_put_deep_replace(tree, "8", NULL));
        xassert(xrbtree_put_deep_replace(tree, "9", NULL));

        xassert(xrbtree_put_deep_replace(tree, "1", value1));
        xassert(strcmp(xrbtree_get(tree, "1"), "1") == 0);
        xassert(xrbtree_put_deep_replace(tree, "1", value2));
        xassert(strcmp(xrbtree_get(tree, "1"), "2") == 0);
        xassert(xrbtree_put_deep_replace(tree, "1", value3));
        xassert(strcmp(xrbtree_get(tree, "1"), "3") == 0);
        xassert(xrbtree_put_deep_replace(tree, "1", "5"));
        xassert(strcmp(xrbtree_get(tree, "1"), "5") == 0);

        xassert(xrbtree_size(tree) == 9);

        xassert(xrbtree_is_rbtree(tree));
        xrbtree_free(&tree);
    }

    {
        XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);

        xassert(xrbtree_put_repeat(tree, "a", NULL));
        xassert(xrbtree_put_repeat(tree, "b", NULL));
        xassert(xrbtree_put_repeat(tree, "c", NULL));
        xassert(xrbtree_put_repeat(tree, "d", NULL));
        xassert(xrbtree_put_repeat(tree, "e", NULL));
        xassert(xrbtree_put_repeat(tree, "f", NULL));
        xassert(xrbtree_put_repeat(tree, "g", "vg"));
        xassert(xrbtree_put_repeat(tree, "h", NULL));
        xassert(xrbtree_put_repeat(tree, "i", NULL));
        xassert(xrbtree_put_repeat(tree, "j", NULL));
        xassert(xrbtree_put_repeat(tree, "k", "vk"));
        xassert(xrbtree_put_repeat(tree, "l", NULL));
        xassert(xrbtree_put_repeat(tree, "m", NULL));
        xassert(xrbtree_put_repeat(tree, "n", NULL));
        xassert(xrbtree_put_repeat(tree, "p", NULL));
        xassert(xrbtree_put_repeat(tree, "q", NULL));
        xassert(xrbtree_put_repeat(tree, "r", "vr"));
        xassert(xrbtree_put_repeat(tree, "t", NULL));
        xassert(xrbtree_put_repeat(tree, "u", NULL));
        xassert(xrbtree_put_repeat(tree, "v", NULL));
        xassert(xrbtree_put_repeat(tree, "w", NULL));
        xassert(xrbtree_put_repeat(tree, "v", NULL));
        xassert(xrbtree_put_repeat(tree, "y", NULL));
        xassert(xrbtree_put_repeat(tree, "z", NULL));

        /* xrbtree_min */
        xassert(strcmp(xrbtree_min(tree), "a") == 0);
        /* xrbtree_max */
        xassert(strcmp(xrbtree_max(tree), "z") == 0);

        /* xrbtree_floor */
        xassert(strcmp(xrbtree_floor(tree, "o"), "n") == 0);
        /* xrbtree_ceiling */
        xassert(strcmp(xrbtree_ceiling(tree, "s"), "t") == 0);

        /* xrbtree_select */
        xassert(strcmp(xrbtree_select(tree, 0), "a") == 0);
        xassert(strcmp(xrbtree_select(tree, 5), "f") == 0);
        xassert(strcmp(xrbtree_select(tree, 15), "q") == 0);
        xassert(strcmp(xrbtree_select(tree, 23), "z") == 0);

        /* xrbtree_rank */
        xassert(xrbtree_rank(tree, "e") == 4);
        xassert(xrbtree_rank(tree, "m") == 12);
        xassert(xrbtree_rank(tree, "y") == 22);

        /* xrbtree_get */
        xassert(strcmp(xrbtree_get(tree, "g"), "vg") == 0);
        xassert(strcmp(xrbtree_get(tree, "k"), "vk") == 0);
        xassert(strcmp(xrbtree_get(tree, "r"), "vr") == 0);

        /* xrbtree_find */
        xassert_false(xrbtree_find(tree, "x"));
        xassert(xrbtree_find(tree, "y"));

        xassert(xrbtree_put_repeat(tree, "g", "vg2"));
        xassert(xrbtree_put_repeat(tree, "k", "vk2"));
        xassert(xrbtree_put_repeat(tree, "g", "vg3"));
        xassert(xrbtree_put_repeat(tree, "k", "vk3"));
        xassert(xrbtree_put_repeat(tree, "g", "vg4"));

        /* xrbtree_get_all */
        {
            XSList_PT list = xrbtree_get_all(tree, "g");
            xassert(xslist_size(list) == 4);
            xslist_free(&list);
        }

        /* xrbtree_keys */
        {
            {
                XSList_PT list = xrbtree_keys(tree, "g", "r");
                xassert(xslist_size(list) == 16);
                xslist_free(&list);
            }

            {
                XSList_PT list = xrbtree_keys(tree, "r", "g");
                xassert(xslist_size(list) == 16);
                xslist_free(&list);
            }
        }

        xassert(xrbtree_is_rbtree(tree));

        /* xrbtree_clear */
        xrbtree_clear(tree);
        xrbtree_free(&tree);
    }

    /* xrbtree_remove_min */
    {
        {
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_remove_min(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
        }

        {
            /*      h
            *      /   ->   h
            *    d(R)
            */
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_put_repeat(tree, "d", NULL);
            xrbtree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 1);
            xassert(strcmp(tree->root->key, "h") == 0);
            xassert_false(tree->root->left);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_min(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
        }

        {
            /*      h
            *        \     ->   m
            *        m(R)
            */
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_put_repeat(tree, "m", NULL);
            xrbtree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 1);
            xassert(strcmp(tree->root->key, "m") == 0);
            xassert_false(tree->root->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_min(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
        }

        {
            /*      h             h          h
            *      / \    ->    /   \    ->   \  
            *     d   m       d(m)  m(R)      m(R)
            */
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_put_repeat(tree, "d", NULL);
            xrbtree_put_repeat(tree, "m", NULL);
            xrbtree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->left);
            xassert_false(tree->root->right->color);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
        }

        {
            /*      h          h
            *      / \        / \
            *     d   m   -> d   m
            *    /
            *   a(R)
            */
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_put_repeat(tree, "d", NULL);
            xrbtree_put_repeat(tree, "m", NULL);
            xrbtree_put_repeat(tree, "a", NULL);
            xrbtree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 3);
            xassert(tree->root->left->size == 1);
            xassert(tree->root->left->color);
            xassert(tree->root->right->color);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
        }

        {
            /*      h            h
            *      / \         /   \
            *     d   m   ->  f     m
            *      \
            *     f(R)
            */
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_put_repeat(tree, "d", NULL);
            xrbtree_put_repeat(tree, "m", NULL);
            xrbtree_put_repeat(tree, "f", NULL);
            xrbtree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 3);
            xassert(tree->root->left->size == 1);
            xassert(tree->root->left->color);
            xassert(tree->root->right->color);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
        }

        {
            /*       h              h             h
            *      /  \           /   \         /   \
            *    d(R)  m   ->    d     m   ->  d     m
            *    /  \          /   \            \
            *   a    f       a(R)  f(R)         f(R) 
            */
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_put_repeat(tree, "d", NULL);
            xrbtree_put_repeat(tree, "m", NULL);
            xrbtree_put_repeat(tree, "a", NULL);
            xrbtree_put_repeat(tree, "f", NULL);
            xrbtree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 4);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->left);
            xassert(tree->root->left->color);
            xassert_false(tree->root->left->right->color);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_put_repeat(tree, "d", NULL);
            xrbtree_put_repeat(tree, "m", NULL);
            xrbtree_put_repeat(tree, "b", NULL);
            xrbtree_put_repeat(tree, "f", NULL);
            xrbtree_put_repeat(tree, "a", NULL);
            xrbtree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 5);
            xassert(tree->root->left->size == 3);
            xassert(tree->root->left->left);
            xassert_false(tree->root->left->color);
            xassert(tree->root->left->left->color);
            xassert(tree->root->left->right->color);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_put_repeat(tree, "d", NULL);
            xrbtree_put_repeat(tree, "m", NULL);
            xrbtree_put_repeat(tree, "b", NULL);
            xrbtree_put_repeat(tree, "f", NULL);
            xrbtree_put_repeat(tree, "j", NULL);
            xrbtree_put_repeat(tree, "p", NULL);
            xrbtree_put_repeat(tree, "a", NULL);
            xrbtree_put_repeat(tree, "c", NULL);
            xrbtree_put_repeat(tree, "e", NULL);
            xrbtree_put_repeat(tree, "g", NULL);
            xrbtree_put_repeat(tree, "i", NULL);
            xrbtree_put_repeat(tree, "k", NULL);
            xrbtree_put_repeat(tree, "o", NULL);
            xrbtree_put_repeat(tree, "p", NULL);
            xrbtree_remove_min(tree);
            xassert(tree->root);
            xassert(tree->root->size == 14);
            xassert(tree->root->left->size == 6);
            xassert(tree->root->left->left->color);
            xassert_false(tree->root->right->color);
            xassert_false(tree->root->left->left->right->color);
            xassert_false(tree->root->left->right->color);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xrbtree_remove_min(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
        }
    }

    /* xrbtree_remove_max */
    {
        {
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_remove_max(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
        }

        {
            /*      h
            *      /   ->   d
            *    d(R)
            */
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_put_repeat(tree, "d", NULL);
            xrbtree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 1);
            xassert(strcmp(tree->root->key, "d") == 0);
            xassert_false(tree->root->left);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_max(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
        }

        {
            /*      h
            *        \     ->   h
            *        m(R)
            */
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_put_repeat(tree, "m", NULL);
            xrbtree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 1);
            xassert(strcmp(tree->root->key, "h") == 0);
            xassert_false(tree->root->right);

            xrbtree_remove_min(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
        }

        {
            /*      h             h            h
            *      / \    ->    /   \    ->   /
            *     d   m       d(m)  m(R)    d(R)
            */
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_put_repeat(tree, "d", NULL);
            xrbtree_put_repeat(tree, "m", NULL);
            xrbtree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->right);
            xassert_false(tree->root->left->color);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
        }

        {
            /*      h          h
            *      / \        / \
            *     d   m   -> d   m
            *          \
            *          n(R)
            */
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_put_repeat(tree, "d", NULL);
            xrbtree_put_repeat(tree, "m", NULL);
            xrbtree_put_repeat(tree, "n", NULL);
            xrbtree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 3);
            xassert(tree->root->right->size == 1);
            xassert(tree->root->left->color);
            xassert(tree->root->right->color);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
        }

        {
            /*      h            h
            *      / \          / \
            *     d   m   ->   d   k
            *        /
            *      k(R)
            */
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_put_repeat(tree, "d", NULL);
            xrbtree_put_repeat(tree, "m", NULL);
            xrbtree_put_repeat(tree, "k", NULL);
            xrbtree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 3);
            xassert(tree->root->right->size == 1);
            xassert(tree->root->left->color);
            xassert(tree->root->right->color);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
        }

        {
            /*      h             h
            *      / \          /   \
            *     d   m   ->   d     m
            *    /     \      /
            *  a(R)    n(R) a(R)
            */
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_put_repeat(tree, "d", NULL);
            xrbtree_put_repeat(tree, "m", NULL);
            xrbtree_put_repeat(tree, "a", NULL);
            xrbtree_put_repeat(tree, "n", NULL);
            xrbtree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 4);
            xassert(tree->root->right->size == 1);
            xassert(tree->root->left->color);
            xassert_false(tree->root->left->left->color);
            xassert(tree->root->right->color);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
        }

        {
            /*       h               h             h
            *      /  \            /   \         /   \
            *     d  m(R)   ->    d     m   ->  d     m
            *        /  \             /   \          /
            *       k    n         k(R)  n(R)      k(R)
            */
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_put_repeat(tree, "d", NULL);
            xrbtree_put_repeat(tree, "m", NULL);
            xrbtree_put_repeat(tree, "k", NULL);
            xrbtree_put_repeat(tree, "n", NULL);
            xrbtree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 4);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->right);
            xassert(tree->root->right->color);
            xassert_false(tree->root->right->left->color);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_put_repeat(tree, "d", NULL);
            xrbtree_put_repeat(tree, "m", NULL);
            xrbtree_put_repeat(tree, "k", NULL);
            xrbtree_put_repeat(tree, "s", NULL);
            xrbtree_put_repeat(tree, "o", NULL);
            xrbtree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 5);
            xassert(tree->root->right->size == 3);
            xassert(tree->root->right->left);
            xassert_false(tree->root->right->color);
            xassert(tree->root->right->left->color);
            xassert(tree->root->right->right->color);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xrbtree_put_repeat(tree, "h", NULL);
            xrbtree_put_repeat(tree, "d", NULL);
            xrbtree_put_repeat(tree, "m", NULL);
            xrbtree_put_repeat(tree, "b", NULL);
            xrbtree_put_repeat(tree, "f", NULL);
            xrbtree_put_repeat(tree, "j", NULL);
            xrbtree_put_repeat(tree, "p", NULL);
            xrbtree_put_repeat(tree, "a", NULL);
            xrbtree_put_repeat(tree, "c", NULL);
            xrbtree_put_repeat(tree, "e", NULL);
            xrbtree_put_repeat(tree, "g", NULL);
            xrbtree_put_repeat(tree, "i", NULL);
            xrbtree_put_repeat(tree, "k", NULL);
            xrbtree_put_repeat(tree, "o", NULL);
            xrbtree_put_repeat(tree, "p", NULL);
            xrbtree_remove_max(tree);
            xassert(tree->root);
            xassert(tree->root->size == 14);
            xassert(tree->root->right->size == 6);
            xassert_false(tree->root->left->color);
            xassert_false(tree->root->right->left->color);
            xassert(tree->root->right->right->color);
            xassert_false(tree->root->right->right->left->color);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xassert(xrbtree_is_rbtree(tree));
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xrbtree_remove_max(tree);
            xassert_false(tree->root);
            xrbtree_free(&tree);
        }
    }

    /* xrbtree_index_replace */
    {
    }

    /* xrbtree_index_deep_replace */
    {
    }

    /* xrbtree_find */
    {
    }

    /* xrbtree_find_replace */
    {
    }

    /* xrbtree_find_deep_replace */
    {
    }

    /* xrbtree_replace_key */
    {
        /*        3                    3
        *      /     \               /   \ 
        *     2       6      ->     2     7
        *    / \     / \           / \   / \
        *   1   2   4   8         2   3 5   9
        */
        XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
        xrbtree_put_repeat(tree, "3", "3");
        xrbtree_put_repeat(tree, "2", "2");
        xrbtree_put_repeat(tree, "6", "6");
        xrbtree_put_repeat(tree, "1", "1");
        xrbtree_put_repeat(tree, "2", "2");
        xrbtree_put_repeat(tree, "4", "4");
        xrbtree_put_repeat(tree, "8", "8");

        xrbtree_replace_key(tree, "4", "5");
        xrbtree_replace_key(tree, "6", "7");
        xrbtree_replace_key(tree, "8", "9");
        xrbtree_replace_key(tree, "1", "2");
        xrbtree_replace_key(tree, "2", "3");

        xassert(strcmp(tree->root->left->left->key, "2") == 0);
        xassert(strcmp(tree->root->left->right->key, "3") == 0);
        xassert(strcmp(tree->root->right->key, "7") == 0);
        xassert(strcmp(tree->root->right->left->key, "5") == 0);
        xassert(strcmp(tree->root->right->right->key, "9") == 0);

        xrbtree_free(&tree);
    }

    /* xrbtree_clear */
    {
    }

    /* xrbtree_deep_clear */
    {
    }

    /* xrbtree_free */
    {
    }

    /* xrbtree_deep_free */
    {
    }
        
    /* xrbtree_remove */
    {
        /* remove one key :
        *
        *         5
        */
        {
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xrbtree_remove(tree, "5");
            xassert(xrbtree_size(tree) == 0);
            xassert_false(tree->root);
            xrbtree_free(&tree);
        }

        /* remove one of two keys :
        *     1. remove the bigger one
        *
        *      5       3
        *     /    ->
        *   3(R)
        */
        {
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xrbtree_remove(tree, "5");
            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->value, "3v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 1);
            xassert_false(tree->root->left);
            xassert_false(tree->root->right);
            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
        }

        /* remove one of two keys :
        *     2. remove the less one
        *
        *      5       5
        *     /    ->
        *   3(R)
        */
        {
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xrbtree_remove(tree, "3");
            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 1);
            xassert_false(tree->root->left);
            xassert_false(tree->root->right);
            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
        }

        /* remove one of three keys :
        *     1. remove 1
        *
        *     3           3
        *    / \    ->     \
        *   1   5         5(R)
        */
        {
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xrbtree_remove(tree, "1");

            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->value, "3v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->left);

            xassert(strcmp(tree->root->right->key, "5") == 0);
            xassert(strcmp(tree->root->right->value, "5v") == 0);
            xassert(tree->root->right->color == false);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);
            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
        }

        /* remove one of three keys :
        *     2. remove 3
        *
        *     3           5
        *    / \    ->   /
        *   1   5      1(R)
        */
        {
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "5", "5v"));

            xrbtree_remove(tree, "3");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->right);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert_false(tree->root->left->color);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
        }

        /* remove one of three keys :
        *     2. remove 5
        *
        *     3           3
        *    / \    ->   /
        *   1   5      1(R)
        */
        {
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "5", "5v"));

            xrbtree_remove(tree, "5");

            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->value, "3v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 2);
            xassert_false(tree->root->right);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert(tree->root->left->color == false);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));

            xrbtree_remove(tree, "1");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));

            xrbtree_remove(tree, "3");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));

            xrbtree_remove(tree, "5");

            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->value, "3v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));

            xrbtree_remove(tree, "8");

            xassert(strcmp(tree->root->key, "3") == 0);
            xassert(strcmp(tree->root->value, "3v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "5") == 0);
            xassert(strcmp(tree->root->right->value, "5v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "7", "7v"));

            xrbtree_remove(tree, "7");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 3);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "0", "0v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));

            xrbtree_remove(tree, "0");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->left);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->right->key, "3") == 0);
            xassert(strcmp(tree->root->left->right->value, "3v") == 0);
            xassert_false(tree->root->left->right->color);
            xassert(tree->root->left->right->size == 1);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "0", "0v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));

            xrbtree_remove(tree, "1");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "0") == 0);
            xassert(strcmp(tree->root->left->left->value, "0v") == 0);
            xassert(tree->root->left->left->color == false);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "0", "0v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));

            xrbtree_remove(tree, "3");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->value, "1v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "0") == 0);
            xassert(strcmp(tree->root->left->left->value, "0v") == 0);
            xassert(tree->root->left->left->color == false);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "0", "0v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));

            xrbtree_remove(tree, "5");

            xassert(strcmp(tree->root->key, "1") == 0);
            xassert(strcmp(tree->root->value, "1v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "0") == 0);
            xassert(strcmp(tree->root->left->value, "0v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->right->left->key, "3") == 0);
            xassert(strcmp(tree->root->right->left->value, "3v") == 0);
            xassert(tree->root->right->left->color == false);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "0", "0v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));

            xrbtree_remove(tree, "8");

            xassert(strcmp(tree->root->key, "1") == 0);
            xassert(strcmp(tree->root->value, "1v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "0") == 0);
            xassert(strcmp(tree->root->left->value, "0v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 1);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "5") == 0);
            xassert(strcmp(tree->root->right->value, "5v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->right->left->key, "3") == 0);
            xassert(strcmp(tree->root->right->left->value, "3v") == 0);
            xassert(tree->root->right->left->color == false);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "6", "6v"));
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "9", "9v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));

            xrbtree_remove(tree, "6");

            xassert(strcmp(tree->root->key, "8") == 0);
            xassert(strcmp(tree->root->value, "8v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "5") == 0);
            xassert(strcmp(tree->root->left->value, "5v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->value, "9v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->color == false);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "6", "6v"));
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "9", "9v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));

            xrbtree_remove(tree, "9");

            xassert(strcmp(tree->root->key, "6") == 0);
            xassert(strcmp(tree->root->value, "6v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "5") == 0);
            xassert(strcmp(tree->root->left->value, "5v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "8") == 0);
            xassert(strcmp(tree->root->right->value, "8v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->color == false);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "6", "6v"));
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "9", "9v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));

            xrbtree_remove(tree, "8");

            xassert(strcmp(tree->root->key, "6") == 0);
            xassert(strcmp(tree->root->value, "6v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 4);

            xassert(strcmp(tree->root->left->key, "5") == 0);
            xassert(strcmp(tree->root->left->value, "5v") == 0);
            xassert(tree->root->left->color == true);
            xassert(tree->root->left->size == 2);
            xassert_false(tree->root->left->right);

            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->value, "9v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 1);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->color == false);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "9", "9v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "4", "4v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "95", "95v"));

            xrbtree_remove(tree, "5");

            xassert(strcmp(tree->root->key, "8") == 0);
            xassert(strcmp(tree->root->value, "8v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 6);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == false);
            xassert(tree->root->left->size == 3);

            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->value, "9v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->left);
            xassert(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->color == true);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "4") == 0);
            xassert(strcmp(tree->root->left->right->value, "4v") == 0);
            xassert(tree->root->left->right->color == true);
            xassert(tree->root->left->right->size == 1);
            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(strcmp(tree->root->right->right->key, "95") == 0);
            xassert(strcmp(tree->root->right->right->value, "95v") == 0);
            xassert(tree->root->right->right->color == false);
            xassert(tree->root->right->right->size == 1);
            xassert_false(tree->root->right->left);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
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
            XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);
            xassert(xrbtree_put_repeat(tree, "5", "5v"));
            xassert(xrbtree_put_repeat(tree, "3", "3v"));
            xassert(xrbtree_put_repeat(tree, "9", "9v"));
            xassert(xrbtree_put_repeat(tree, "1", "1v"));
            xassert(xrbtree_put_repeat(tree, "4", "4v"));
            xassert(xrbtree_put_repeat(tree, "8", "8v"));
            xassert(xrbtree_put_repeat(tree, "95", "95v"));

            xrbtree_remove(tree, "95");

            xassert(strcmp(tree->root->key, "5") == 0);
            xassert(strcmp(tree->root->value, "5v") == 0);
            xassert(tree->root->color == true);
            xassert(tree->root->size == 6);

            xassert(strcmp(tree->root->left->key, "3") == 0);
            xassert(strcmp(tree->root->left->value, "3v") == 0);
            xassert(tree->root->left->color == false);
            xassert(tree->root->left->size == 3);

            xassert(strcmp(tree->root->right->key, "9") == 0);
            xassert(strcmp(tree->root->right->value, "9v") == 0);
            xassert(tree->root->right->color == true);
            xassert(tree->root->right->size == 2);
            xassert_false(tree->root->right->right);

            xassert(strcmp(tree->root->left->left->key, "1") == 0);
            xassert(strcmp(tree->root->left->left->value, "1v") == 0);
            xassert(tree->root->left->left->color == true);
            xassert(tree->root->left->left->size == 1);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);

            xassert(strcmp(tree->root->left->right->key, "4") == 0);
            xassert(strcmp(tree->root->left->right->value, "4v") == 0);
            xassert(tree->root->left->right->color == true);
            xassert(tree->root->left->right->size == 1);
            xassert_false(tree->root->left->right->left);
            xassert_false(tree->root->left->right->right);

            xassert(strcmp(tree->root->right->left->key, "8") == 0);
            xassert(strcmp(tree->root->right->left->value, "8v") == 0);
            xassert(tree->root->right->left->color == false);
            xassert(tree->root->right->left->size == 1);
            xassert_false(tree->root->right->left->left);
            xassert_false(tree->root->right->left->right);

            xassert(xrbtree_is_rbtree(tree));
            xrbtree_free(&tree);
        }
    }

    /* xrbtree_remove_all */
    {
    }

    {
        XRBTree_PT tree = xrbtree_random_string(test_cmpk, NULL, 10, 2);
        //xrbtree_print(tree, 5);

        /* xrbtree_map_preorder */
        //xrbtree_map_preorder(tree, xrbtree_test_map_applykv, NULL);

        /* xrbtree_map_inorder */
        //xrbtree_map_inorder(tree, xrbtree_test_map_applykv, NULL);

        /* xrbtree_map_postorder */
        //xrbtree_map_postorder(tree, xrbtree_test_map_applykv, NULL);

        /* xrbtree_map_levelorder */
        //xrbtree_map_levelorder(tree, xrbtree_test_map_applykv, NULL);

        /* xrbtree_map_min_to_max */
        //xrbtree_map_min_to_max(tree, xrbtree_test_map_applykv, NULL);

        /* xrbtree_map_max_to_min */
        //xrbtree_map_max_to_min(tree, xrbtree_test_map_applykv, NULL);

        /* xrbtree_map_key_min_to_max */
        //xrbtree_map_key_min_to_max(tree, xrbtree_test_map_applyk, NULL);

        /* xrbtree_map_key_max_to_min */
        //xrbtree_map_key_max_to_min(tree, xrbtree_test_map_applyk, NULL);

        xrbtree_deep_free(&tree);
    }

    {
        XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);

        xassert(xrbtree_put_repeat(tree, "a", NULL));
        xassert(xrbtree_put_repeat(tree, "a", NULL));
        xassert(xrbtree_put_repeat(tree, "a", NULL));
        xassert(xrbtree_put_repeat(tree, "a", NULL));
        xassert(xrbtree_put_repeat(tree, "a", NULL));

        /* xrbtree_map_min_to_max_break_if_true */
        xassert_false(xrbtree_map_min_to_max_break_if_true(tree, xrbtree_test_map_applykv_false, NULL));
        xassert_false(xrbtree_map_min_to_max_break_if_true(tree, xrbtree_test_map_cmpkv, "x"));
        xassert(xrbtree_map_min_to_max_break_if_true(tree, xrbtree_test_map_cmpkv, "a"));

        /* xrbtree_map_min_to_max_break_if_false */
        xassert_false(xrbtree_map_min_to_max_break_if_false(tree, xrbtree_test_map_applykv_true, NULL));
        xassert_false(xrbtree_map_min_to_max_break_if_false(tree, xrbtree_test_map_cmpkv, "a"));
        xassert(xrbtree_map_min_to_max_break_if_false(tree, xrbtree_test_map_cmpkv, "b"));

        /* xrbtree_map_key_min_to_max_break_if_true */
        xassert_false(xrbtree_map_key_min_to_max_break_if_true(tree, xrbtree_test_map_applyk_false, NULL));
        xassert_false(xrbtree_map_key_min_to_max_break_if_true(tree, xrbtree_test_map_cmpk, "x"));
        xassert(xrbtree_map_key_min_to_max_break_if_true(tree, xrbtree_test_map_cmpk, "a"));

        /* xrbtree_map_key_min_to_max_break_if_false */
        xassert_false(xrbtree_map_key_min_to_max_break_if_false(tree, xrbtree_test_map_applyk_true, NULL));
        xassert_false(xrbtree_map_key_min_to_max_break_if_false(tree, xrbtree_test_map_cmpk, "a"));
        xassert(xrbtree_map_key_min_to_max_break_if_false(tree, xrbtree_test_map_cmpk, "b"));

        /* xrbtree_map_max_to_min_break_if_true */
        xassert_false(xrbtree_map_max_to_min_break_if_true(tree, xrbtree_test_map_applykv_false, NULL));
        xassert_false(xrbtree_map_max_to_min_break_if_true(tree, xrbtree_test_map_cmpkv, "x"));
        xassert(xrbtree_map_max_to_min_break_if_true(tree, xrbtree_test_map_cmpkv, "a"));

        /* xrbtree_map_max_to_min_break_if_false */
        xassert_false(xrbtree_map_max_to_min_break_if_false(tree, xrbtree_test_map_applykv_true, NULL));
        xassert_false(xrbtree_map_max_to_min_break_if_false(tree, xrbtree_test_map_cmpkv, "a"));
        xassert(xrbtree_map_max_to_min_break_if_false(tree, xrbtree_test_map_cmpkv, "b"));

        /* xrbtree_map_key_max_to_min_break_if_true */
        xassert_false(xrbtree_map_key_max_to_min_break_if_true(tree, xrbtree_test_map_applyk_false, NULL));
        xassert_false(xrbtree_map_key_max_to_min_break_if_true(tree, xrbtree_test_map_cmpk, "x"));
        xassert(xrbtree_map_key_max_to_min_break_if_true(tree, xrbtree_test_map_cmpk, "a"));

        /* xrbtree_map_key_max_to_min_break_if_false */
        xassert_false(xrbtree_map_key_max_to_min_break_if_false(tree, xrbtree_test_map_applyk_true, NULL));
        xassert_false(xrbtree_map_key_max_to_min_break_if_false(tree, xrbtree_test_map_cmpk, "a"));
        xassert(xrbtree_map_key_max_to_min_break_if_false(tree, xrbtree_test_map_cmpk, "b"));

        xrbtree_free(&tree);
    }
        
    /* xrbtree_swap */
    {
    }
        
    /* xrbtree_keys_size */
    {
        XRBTree_PT tree = xrbtree_new(test_cmpk, NULL);

        xassert(xrbtree_put_repeat(tree, "5", "5v"));
        xassert(xrbtree_put_repeat(tree, "5", "5v"));
        xassert(xrbtree_put_repeat(tree, "5", "5v"));
        xassert(xrbtree_put_repeat(tree, "5", "5v"));
        xassert(xrbtree_put_repeat(tree, "5", "5v"));
        xassert(xrbtree_put_repeat(tree, "5", "5v"));
        xassert(xrbtree_put_repeat(tree, "5", "5v"));
        xassert(xrbtree_put_repeat(tree, "5", "5v"));
        xassert(xrbtree_put_repeat(tree, "5", "5v"));
        xassert(xrbtree_put_repeat(tree, "3", "3v"));
        xassert(xrbtree_put_repeat(tree, "8", "8v"));
        xassert(xrbtree_keys_size(tree, "5", "5") == 9);

        xrbtree_free(&tree);
    }
        
    /* xrbtree_size */
    {
    }
        
    /* xrbtree_is_empty */
    {
    }
        
    /* xrbtree_height */
    {
    }

    /* xrbtree_is_rbtree */
    {
        XRBTree_PT tree = xrbtree_random_string(test_cmpk, NULL, 500, 2);
        xassert(xrbtree_is_rbtree(tree));

        xrbtree_deep_remove(tree, "10");
        xrbtree_deep_remove(tree, "11");
        xrbtree_deep_remove(tree, "12");
        xrbtree_deep_remove(tree, "13");
        xrbtree_deep_remove(tree, "14");
        xrbtree_deep_remove(tree, "15");
        xrbtree_deep_remove(tree, "16");
        xrbtree_deep_remove(tree, "17");
        xrbtree_deep_remove(tree, "18");
        xrbtree_deep_remove(tree, "19");
        xassert(xrbtree_is_rbtree(tree));

        xrbtree_deep_remove_min(tree);
        xrbtree_deep_remove_max(tree);
        xrbtree_deep_remove_min(tree);
        xrbtree_deep_remove_max(tree);
        xrbtree_deep_remove_min(tree);
        xrbtree_deep_remove_max(tree);
        xrbtree_deep_remove_min(tree);
        xrbtree_deep_remove_max(tree);
        xrbtree_deep_remove_min(tree);
        xassert(xrbtree_is_rbtree(tree));

        xrbtree_deep_remove(tree, "20");
        xrbtree_deep_remove(tree, "21");
        xrbtree_deep_remove(tree, "22");
        xrbtree_deep_remove(tree, "23");
        xrbtree_deep_remove(tree, "24");
        xrbtree_deep_remove(tree, "25");
        xrbtree_deep_remove(tree, "26");
        xrbtree_deep_remove(tree, "27");
        xrbtree_deep_remove(tree, "28");
        xrbtree_deep_remove(tree, "29");
        xassert(xrbtree_is_rbtree(tree));

        xrbtree_deep_remove_max(tree);
        xrbtree_deep_remove_min(tree);
        xrbtree_deep_remove_max(tree);
        xrbtree_deep_remove_min(tree);
        xrbtree_deep_remove_max(tree);
        xrbtree_deep_remove_min(tree);
        xrbtree_deep_remove_max(tree);
        xrbtree_deep_remove_min(tree);
        xrbtree_deep_remove_max(tree);
        xassert(xrbtree_is_rbtree(tree));

        xrbtree_deep_remove(tree, "50");
        xrbtree_deep_remove(tree, "51");
        xrbtree_deep_remove(tree, "52");
        xrbtree_deep_remove(tree, "53");
        xrbtree_deep_remove(tree, "54");
        xrbtree_deep_remove(tree, "55");
        xrbtree_deep_remove(tree, "56");
        xrbtree_deep_remove(tree, "57");
        xrbtree_deep_remove(tree, "58");
        xrbtree_deep_remove(tree, "59");
        xassert(xrbtree_is_rbtree(tree));

        xrbtree_deep_remove_min(tree);
        xrbtree_deep_remove_max(tree);
        xrbtree_deep_remove_min(tree);
        xrbtree_deep_remove_max(tree);
        xrbtree_deep_remove_min(tree);
        xrbtree_deep_remove_max(tree);
        xrbtree_deep_remove_min(tree);
        xrbtree_deep_remove_max(tree);
        xrbtree_deep_remove_min(tree);
        xrbtree_deep_remove_max(tree);
        xassert(xrbtree_is_rbtree(tree));

        xrbtree_deep_remove(tree, "70");
        xrbtree_deep_remove(tree, "71");
        xrbtree_deep_remove(tree, "72");
        xrbtree_deep_remove(tree, "73");
        xrbtree_deep_remove(tree, "74");
        xrbtree_deep_remove(tree, "75");
        xrbtree_deep_remove(tree, "76");
        xrbtree_deep_remove(tree, "77");
        xrbtree_deep_remove(tree, "78");
        xrbtree_deep_remove(tree, "79");
        xassert(xrbtree_is_rbtree(tree));

        xrbtree_deep_free(&tree);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
