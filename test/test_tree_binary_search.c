
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../list_s_raw/xlist_s_raw_x.h"
#include "../list_s/xlist_s_x.h"
#include "../tree_binary_search/xtree_binary_search_x.h"
#include "../include/xalgos.h"

static
int xbstree_compare(void* x, void* y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static
void check_mem_leak(const void* ptr, long size, const char* file, int line, void* cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static
bool xbstree_apply(void* key, void** old_value, void* new_value, void* cl) {
    *old_value = new_value;
    return true;
}

static
bool xbstree_apply_print(void* key, void** value, void* cl) {
    printf("%s:%s\n", (char*)key, (char*)(*value));
    return true;
}

static
bool xbstree_apply_print_break_true(void* key, void** value, void* cl) {
    printf("%s:%s\n", (char*)key, (char*)(*value));
    if (strcmp((char*)key, "6") == 0) {
        return true;
    }
    return false;
}

static
bool xbstree_apply_print_break_false(void* key, void** value, void* cl) {
    printf("%s:%s\n", (char*)key, (char*)(*value));
    if (strcmp((char*)key, "7") == 0) {
        return false;
    }
    return true;
}

static
bool xbstree_apply_print_list_break_false(void* key, XRSList_PT* values, int size, void* cl) {
    printf("%s:%s\n", (char*)key, (char*)((*values)->value));
    if (strcmp((char*)key, "7") == 0) {
        return false;
    }
    return true;
}

static
XBSTree_PT xbstree_random_string(int(*cmp) (void* key1, void* key2, void *cl), int total_size, int string_length) {
    xassert(cmp);
    xassert(0 < total_size);
    xassert(0 < string_length);

    if (!cmp || (total_size <= 0) || (string_length <= 0)) {
        return NULL;
    }

    {
        XBSTree_PT tree = xbstree_new(cmp, NULL);
        if (!tree) {
            return NULL;
        }

        {
            const char charsets[] = "0123456789";

            for (int i = 0; i < total_size; i++) {
                char* str = XMEM_MALLOC(string_length + 1);
                if (!str) {
                    xbstree_deep_free(&tree);
                    return NULL;
                }

                for (int i = 0; i < string_length; ++i) {
                    str[i] = charsets[rand() % (sizeof(charsets) - 1)];
                }
                str[string_length] = '\0';

                if (!xbstree_put_repeat(tree, str, NULL)) {
                    xbstree_deep_free(&tree);
                    return NULL;
                }
            }
        }

        return tree;
    }
}

static
void xbstree_printnode(char* key, char* value, int h) {
    for (int i = 0; i < h; i++) {
        printf(" ");
    }

    printf("%s:", key);
    printf("%s,", (char*)value);
    printf("\n");

    return;
}

static
void xbstree_print_impl(XBSTree_Node_PT node, int h) {
    if (!node) {
        return;
    }

    xbstree_print_impl(node->right, h + 3);

    xbstree_printnode((char*)node->key, node->value, h);

    xbstree_print_impl(node->left, h + 3);
}

static
void xbstree_print(XBSTree_PT tree, int start_blank) {
    xassert(tree);
    xassert(0 <= start_blank);

    if (start_blank < 0) {
        return;
    }

    if (!tree) {
        xbstree_printnode("-", NULL, start_blank);
        return;
    }

    xbstree_print_impl(tree->root, start_blank + 3);
}

void test_xbstree() {
    //xbstree_new
    {
        /* cmp is null */
        {
            bool except = false;

            XEXCEPT_TRY
                xbstree_new(NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        // normal case
        {
            XBSTree_PT tree = xbstree_new(xbstree_compare, NULL);
            xassert(tree);
            xassert(tree->cmp);
            xassert_false(tree->root);
            xbstree_free(&tree);
        }
    }

    //xbstree_copy
    {
        /* tree is null */
        {
            bool except = false;

            XEXCEPT_TRY
                xbstree_copy(NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        //normal case
        {
            //             5
            //          /     \
            //        3        8
            //      /         / \ 
            //     2         7   9
            XBSTree_PT tree = xbstree_new(xbstree_compare, NULL);
            xbstree_put_repeat(tree, "5", NULL);
            xbstree_put_repeat(tree, "3", "");
            xbstree_put_repeat(tree, "2", NULL);
            xbstree_put_repeat(tree, "8", "");
            xbstree_put_repeat(tree, "7", "b");
            xbstree_put_repeat(tree, "9", "c");

            xassert(xbstree_is_bstree(tree));
            //xbstree_print(tree, 5);

            XBSTree_PT ntree = xbstree_copy(tree);
            xassert(xbstree_size(ntree) == 6);

            xassert_false(ntree->root->parent);
            xassert(strcmp("5", (char*)ntree->root->key) == 0);
            xassert_false(ntree->root->value);
            xassert(strcmp("3", (char*)ntree->root->left->key) == 0);
            xassert(strcmp("8", (char*)ntree->root->right->key) == 0);
            xassert(ntree->root->size == 6);

            xassert(strcmp("5", (char*)ntree->root->left->parent->key) == 0);
            xassert(strcmp("3", (char*)ntree->root->left->key) == 0);
            xassert(strcmp("", (char*)ntree->root->left->value) == 0);
            xassert(strcmp("2", (char*)ntree->root->left->left->key) == 0);
            xassert_false(ntree->root->left->right);
            xassert(ntree->root->left->size == 2);

            xassert(strcmp("3", (char*)ntree->root->left->left->parent->key) == 0);
            xassert(strcmp("2", (char*)ntree->root->left->left->key) == 0);
            xassert_false(ntree->root->left->left->value);
            xassert_false(ntree->root->left->left->left);
            xassert_false(ntree->root->left->left->right);
            xassert(ntree->root->left->left->size == 1);

            xassert(strcmp("5", (char*)ntree->root->right->parent->key) == 0);
            xassert(strcmp("8", (char*)ntree->root->right->key) == 0);
            xassert(strcmp("", (char*)ntree->root->right->value) == 0);
            xassert(strcmp("7", (char*)ntree->root->right->left->key) == 0);
            xassert(strcmp("9", (char*)ntree->root->right->right->key) == 0);
            xassert(ntree->root->right->size == 3);

            xassert(strcmp("8", (char*)ntree->root->right->left->parent->key) == 0);
            xassert(strcmp("7", (char*)ntree->root->right->left->key) == 0);
            xassert(strcmp("b", (char*)ntree->root->right->left->value) == 0);
            xassert_false(ntree->root->right->left->left);
            xassert_false(ntree->root->right->left->right);
            xassert(ntree->root->right->left->size == 1);

            xassert(strcmp("8", (char*)ntree->root->right->right->parent->key) == 0);
            xassert(strcmp("9", (char*)ntree->root->right->right->key) == 0);
            xassert(strcmp("c", (char*)ntree->root->right->right->value) == 0);
            xassert_false(ntree->root->right->right->left);
            xassert_false(ntree->root->right->right->right);
            xassert(ntree->root->right->right->size == 1);

            xassert(xbstree_is_bstree(ntree));

            //xbstree_print(ntree, 5);
            xbstree_free(&tree);
            xbstree_free(&ntree);
        }
    }

    /* xbstree_deep_copy */
    {
        XBSTree_PT tree = xbstree_random_string(xbstree_compare, 50, 3);
        XBSTree_PT ntree = xbstree_deep_copy(tree, 4, 0);
        xassert(xbstree_is_bstree(tree));
        xassert(xbstree_is_bstree(ntree));
        xbstree_deep_free(&tree);
        xbstree_deep_free(&ntree);
    }

    //xbstree_put_repeat
    {
        /* tree is NULL */
        {
            bool except = false;

            XEXCEPT_TRY
                xbstree_put_repeat(NULL, "1", NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* key is NULL */
        {
            XBSTree_PT tree = xbstree_new(xbstree_compare, NULL);
            bool except = false;

            XEXCEPT_TRY
                xbstree_put_repeat(tree, NULL, "1");
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xbstree_free(&tree);
        }

        {
            //             5
            //          /     \
            //        3         8
            //      /  \       / \
            //     2    3     7   8
            //      \              \
            //       2              8
            //        \
            //         2

            XBSTree_PT tree = xbstree_new(xbstree_compare, NULL);

            {
                xbstree_put_repeat(tree, "5", NULL);
                xassert(tree->root);
                xassert(tree->root->size == 1);
                xassert(strcmp(tree->root->key, "5") == 0);
                xassert_false(tree->root->value);
                xassert_false(tree->root->parent);
                xassert_false(tree->root->left);
                xassert_false(tree->root->right);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_repeat(tree, "3", "3-1");
                xassert(tree->root->size == 2);
                xassert(tree->root->left->size == 1);
                xassert(strcmp(tree->root->left->key, "3") == 0);
                xassert(strcmp(tree->root->left->value, "3-1") == 0);
                xassert(tree->root->left->parent = tree->root);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_repeat(tree, "2", "2-1");
                xassert(tree->root->size == 3);
                xassert(tree->root->left->size == 2);
                xassert(tree->root->left->left->size == 1);
                xassert(strcmp(tree->root->left->left->key, "2") == 0);
                xassert(strcmp(tree->root->left->left->value, "2-1") == 0);
                xassert(tree->root->left->left->parent = tree->root->left);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_repeat(tree, "2", NULL);
                xassert(tree->root->size == 4);
                xassert(tree->root->left->size == 3);
                xassert(tree->root->left->left->size == 2);
                xassert(tree->root->left->left->right->size == 1);
                xassert(strcmp(tree->root->left->left->right->key, "2") == 0);
                xassert_false(tree->root->left->left->right->value);
                xassert(tree->root->left->left->right->parent = tree->root->left->left);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_repeat(tree, "2", "2-2");
                xassert(tree->root->size == 5);
                xassert(tree->root->left->size == 4);
                xassert(tree->root->left->left->size == 3);
                xassert(tree->root->left->left->right->size == 2);
                xassert(tree->root->left->left->right->right->size == 1);
                xassert(strcmp(tree->root->left->left->right->right->key, "2") == 0);
                xassert(strcmp(tree->root->left->left->right->right->value, "2-2") == 0);
                xassert(tree->root->left->left->right->right->parent = tree->root->left->left->right);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_repeat(tree, "3", NULL);
                xassert(tree->root->size == 6);
                xassert(tree->root->left->size == 5);
                xassert(tree->root->left->right->size == 1);
                xassert(strcmp(tree->root->left->right->key, "3") == 0);
                xassert_false(tree->root->left->right->value);
                xassert(tree->root->left->right->parent = tree->root->left);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_repeat(tree, "8", NULL);
                xassert(tree->root->size == 7);
                xassert(tree->root->right->size == 1);
                xassert(strcmp(tree->root->right->key, "8") == 0);
                xassert_false(tree->root->right->value);
                xassert(tree->root->right->parent = tree->root);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_repeat(tree, "8", "8-1");
                xassert(tree->root->size == 8);
                xassert(tree->root->right->size == 2);
                xassert(tree->root->right->right->size == 1);
                xassert(strcmp(tree->root->right->right->value, "8-1") == 0);
                xassert(tree->root->right->right->parent = tree->root->right);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_repeat(tree, "8", NULL);
                xassert(tree->root->size == 9);
                xassert(tree->root->right->size == 3);
                xassert(tree->root->right->right->size == 2);
                xassert(tree->root->right->right->right->size == 1);
                xassert_false(tree->root->right->right->right->value);
                xassert(tree->root->right->right->right->parent = tree->root->right->right);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_repeat(tree, "7", NULL);
                xassert(tree->root->size == 10);
                xassert(tree->root->right->size == 4);
                xassert(tree->root->right->left->size == 1);
                xassert(strcmp(tree->root->right->left->key, "7") == 0);
                xassert_false(tree->root->right->left->value);
                xassert(tree->root->right->left->parent = tree->root->right);
                xassert(xbstree_is_bstree(tree));
            }

            xbstree_free(&tree);
        }

        /* add many keys */
        {
            XBSTree_PT tree = xbstree_random_string(xbstree_compare, 5000, 2);
            xassert(xbstree_is_bstree(tree));
            xbstree_deep_free(&tree);
        }
    }

    //xbstree_put_unique
    {
        /* tree is NULL */
        {
            bool except = false;

            XEXCEPT_TRY
                xbstree_put_unique(NULL, "1", NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* key is NULL */
        {
            XBSTree_PT tree = xbstree_new(xbstree_compare, NULL);
            bool except = false;

            XEXCEPT_TRY
                xbstree_put_unique(tree, NULL, "1");
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xbstree_free(&tree);
        }

        {
            //             5
            //          /     \
            //        3        8
            //      /         /
            //     2         7

            XBSTree_PT tree = xbstree_new(xbstree_compare, NULL);

            {
                xbstree_put_unique(tree, "5", NULL);
                xassert(tree->root);
                xassert(tree->root->size == 1);
                xassert(strcmp(tree->root->key, "5") == 0);
                xassert_false(tree->root->value);
                xassert_false(tree->root->parent);
                xassert_false(tree->root->left);
                xassert_false(tree->root->right);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_unique(tree, "3", "3-1");
                xassert(tree->root->size == 2);
                xassert(tree->root->left->size == 1);
                xassert(strcmp(tree->root->left->key, "3") == 0);
                xassert(strcmp(tree->root->left->value, "3-1") == 0);
                xassert(tree->root->left->parent = tree->root);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_unique(tree, "2", "2-1");
                xassert(tree->root->size == 3);
                xassert(tree->root->left->size == 2);
                xassert(tree->root->left->left->size == 1);
                xassert(strcmp(tree->root->left->left->key, "2") == 0);
                xassert(strcmp(tree->root->left->left->value, "2-1") == 0);
                xassert(tree->root->left->left->parent = tree->root->left);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_unique(tree, "2", NULL);
                xassert(tree->root->size == 3);
                xassert(tree->root->left->size == 2);
                xassert(tree->root->left->left->size == 1);
                xassert_false(tree->root->left->left->right);
                xassert(strcmp(tree->root->left->left->key, "2") == 0);
                xassert(strcmp(tree->root->left->left->value, "2-1") == 0);
                xassert(tree->root->left->left->parent = tree->root->left);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_unique(tree, "2", "2-2");
                xassert(tree->root->size == 3);
                xassert(tree->root->left->size == 2);
                xassert(tree->root->left->left->size == 1);
                xassert_false(tree->root->left->left->right);
                xassert(strcmp(tree->root->left->left->value, "2-1") == 0);
                xassert(tree->root->left->left->parent = tree->root->left);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_unique(tree, "3", NULL);
                xassert(tree->root->size == 3);
                xassert(tree->root->left->size == 2);
                xassert_false(tree->root->left->right);
                xassert(strcmp(tree->root->left->value, "3-1") == 0);
                xassert(tree->root->left->parent = tree->root);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_unique(tree, "8", NULL);
                xassert(tree->root->size == 4);
                xassert(tree->root->right->size == 1);
                xassert(strcmp(tree->root->right->key, "8") == 0);
                xassert_false(tree->root->right->value);
                xassert(tree->root->right->parent = tree->root);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_unique(tree, "8", "8-1");
                xassert(tree->root->size == 4);
                xassert(tree->root->right->size == 1);
                xassert_false(tree->root->right->right);
                xassert_false(tree->root->right->value);
                xassert(tree->root->right->parent = tree->root);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_unique(tree, "8", NULL);
                xassert(tree->root->size == 4);
                xassert(tree->root->right->size == 1);
                xassert_false(tree->root->right->right);
                xassert(tree->root->right->parent = tree->root);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_unique(tree, "7", NULL);
                xassert(tree->root->size == 5);
                xassert(tree->root->right->size == 2);
                xassert(tree->root->right->left->size == 1);
                xassert(strcmp(tree->root->right->left->key, "7") == 0);
                xassert_false(tree->root->right->left->value);
                xassert(tree->root->right->left->parent = tree->root->right);
                xassert(xbstree_is_bstree(tree));
            }

            xbstree_free(&tree);
        }
    }

    //xbstree_put_replace
    {
        /* tree is NULL */
        {
            bool except = false;

            XEXCEPT_TRY
                xbstree_put_replace(NULL, "1", NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* key is NULL */
        {
            XBSTree_PT tree = xbstree_new(xbstree_compare, NULL);
            bool except = false;

            XEXCEPT_TRY
                xbstree_put_replace(tree, NULL, "1", NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xbstree_free(&tree);
        }

        {
            //             5
            //          /     \
            //        3        8
            //      /         /
            //     2         7

            XBSTree_PT tree = xbstree_new(xbstree_compare, NULL);

            {
                xbstree_put_replace(tree, "5", NULL, NULL);
                xassert(tree->root);
                xassert(tree->root->size == 1);
                xassert(strcmp(tree->root->key, "5") == 0);
                xassert_false(tree->root->value);
                xassert_false(tree->root->parent);
                xassert_false(tree->root->left);
                xassert_false(tree->root->right);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_replace(tree, "3", "3-1", NULL);
                xassert(tree->root->size == 2);
                xassert(tree->root->left->size == 1);
                xassert(strcmp(tree->root->left->key, "3") == 0);
                xassert(strcmp(tree->root->left->value, "3-1") == 0);
                xassert(tree->root->left->parent = tree->root);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_replace(tree, "2", "2-1", NULL);
                xassert(tree->root->size == 3);
                xassert(tree->root->left->size == 2);
                xassert(tree->root->left->left->size == 1);
                xassert(strcmp(tree->root->left->left->key, "2") == 0);
                xassert(strcmp(tree->root->left->left->value, "2-1") == 0);
                xassert(tree->root->left->left->parent = tree->root->left);
                xassert(xbstree_is_bstree(tree));
            }

            {
                char* str = NULL;
                xbstree_put_replace(tree, "2", NULL, (void**)&str);
                xassert(tree->root->size == 3);
                xassert(tree->root->left->size == 2);
                xassert(tree->root->left->left->size == 1);
                xassert(strcmp(tree->root->left->left->key, "2") == 0);
                xassert_false(tree->root->left->left->value);
                xassert_false(tree->root->left->left->right);
                xassert(tree->root->left->left->parent = tree->root->left);
                xassert(strcmp(str, "2-1") == 0);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_replace(tree, "2", "2-2", NULL);
                xassert(tree->root->size == 3);
                xassert(tree->root->left->size == 2);
                xassert(tree->root->left->left->size == 1);
                xassert(strcmp(tree->root->left->left->value, "2-2") == 0);
                xassert_false(tree->root->left->left->right);
                xassert(tree->root->left->left->parent = tree->root->left);
                xassert(xbstree_is_bstree(tree));
            }

            {
                char* str = NULL;
                xbstree_put_replace(tree, "3", NULL, (void**)&str);
                xassert(tree->root->size == 3);
                xassert(tree->root->left->size == 2);
                xassert_false(tree->root->left->value);
                xassert(tree->root->left->parent = tree->root);
                xassert(strcmp(str, "3-1") == 0);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_replace(tree, "8", NULL, NULL);
                xassert(tree->root->size == 4);
                xassert(tree->root->right->size == 1);
                xassert(strcmp(tree->root->right->key, "8") == 0);
                xassert_false(tree->root->right->value);
                xassert(tree->root->right->parent = tree->root);
                xassert(xbstree_is_bstree(tree));
            }

            {
                char* str = NULL;
                xbstree_put_replace(tree, "8", "8-1", (void**)&str);
                xassert(tree->root->size == 4);
                xassert(tree->root->right->size == 1);
                xassert_false(tree->root->right->right);
                xassert(strcmp(tree->root->right->value, "8-1") == 0);
                xassert(tree->root->right->parent = tree->root);
                xassert_false(str);
                xassert(xbstree_is_bstree(tree));
            }

            {
                char* str = NULL;
                xbstree_put_replace(tree, "8", NULL, (void**)&str);
                xassert(tree->root->size == 4);
                xassert(tree->root->right->size == 1);
                xassert_false(tree->root->right->value);
                xassert_false(tree->root->right->right);
                xassert(tree->root->right->parent = tree->root);
                xassert(strcmp(str, "8-1") == 0);
                xassert(xbstree_is_bstree(tree));
            }

            {
                xbstree_put_replace(tree, "7", NULL, NULL);
                xassert(tree->root->size == 5);
                xassert(tree->root->right->size == 2);
                xassert(tree->root->right->left->size == 1);
                xassert(strcmp(tree->root->right->left->key, "7") == 0);
                xassert_false(tree->root->right->left->value);
                xassert(tree->root->right->left->parent = tree->root->right);
                xassert(xbstree_is_bstree(tree));
            }

            xbstree_free(&tree);
        }
    }

    {
        /*            5
        *        /        \
        *       3          7
        *     /  \        /  \
        *    2    3      6    7
        *   /      \           \
        *  1        4           7
        *            \           \
        *             4           8
        *                          \
        *                           9
        */

        XBSTree_PT tree = xbstree_new(xbstree_compare, NULL);
        xbstree_put_repeat(tree, "5", "5");
        xbstree_put_repeat(tree, "3", "3-1");
        xbstree_put_repeat(tree, "3", "3-2");
        xbstree_put_repeat(tree, "2", "2");
        xbstree_put_repeat(tree, "4", "4-1");
        xbstree_put_repeat(tree, "4", "4-2");
        xbstree_put_repeat(tree, "1", "1");
        xbstree_put_repeat(tree, "7", "7-1");
        xbstree_put_repeat(tree, "7", "7-2");
        xbstree_put_repeat(tree, "7", "7-3");
        xbstree_put_repeat(tree, "6", "6");
        xbstree_put_repeat(tree, "8", "8");
        xbstree_put_repeat(tree, "9", "9-1");
        xassert(xbstree_is_bstree(tree));
        //xbstree_print(tree, 5);

        {
            /* xbstree_min */
            /* xbstree_max */
            {
                XBSTree_PT tree1 = xbstree_new(xbstree_compare, NULL);
                xassert_false(xbstree_min(tree1));
                xassert_false(xbstree_max(tree1));
                xbstree_free(&tree1);
            }

            {
                XBSTree_PT tree1 = xbstree_new(xbstree_compare, NULL);
                xbstree_put_repeat(tree1, "5", "5");
                xbstree_put_repeat(tree1, "7", "7-1");
                xbstree_put_repeat(tree1, "6", "6");
                xbstree_put_repeat(tree1, "8", "8");
                xbstree_put_repeat(tree1, "9", "9");

                xassert(strcmp(xbstree_min(tree1), "5") == 0);
                xassert(strcmp(xbstree_max(tree1), "9") == 0);
                xbstree_free(&tree1);
            }

            {
                XBSTree_PT tree1 = xbstree_new(xbstree_compare, NULL);
                xbstree_put_repeat(tree1, "5", "5");
                xbstree_put_repeat(tree1, "3", "3-1");
                xbstree_put_repeat(tree1, "2", "2");
                xbstree_put_repeat(tree1, "4", "4-1");
                xbstree_put_repeat(tree1, "1", "1");

                xassert(strcmp(xbstree_min(tree1), "1") == 0);
                xassert(strcmp(xbstree_max(tree1), "5") == 0);
                xbstree_free(&tree1);
            }

            {
                xassert(strcmp(xbstree_min(tree), "1") == 0);
                xassert(strcmp(xbstree_max(tree), "9") == 0);
            }
        }

        {
            /*            5
             *        /        \
             *       2          5
             *     /  \           \
             *    1    4           8
             *                    / \
             *                   6   9
             */

            XBSTree_PT tree1 = xbstree_new(xbstree_compare, NULL);
            xbstree_put_repeat(tree1, "5", "5");
            xbstree_put_repeat(tree1, "2", "1-1");
            xbstree_put_repeat(tree1, "1", "1-1");
            xbstree_put_repeat(tree1, "4", "4-1");
            xbstree_put_repeat(tree1, "5", "5-1");
            xbstree_put_repeat(tree1, "8", "8-1");
            xbstree_put_repeat(tree1, "6", "6-1");
            xbstree_put_repeat(tree1, "9", "6-1");

            xassert(xbstree_is_bstree(tree1));

            /* xbstree_floor */
            {
                xassert(strcmp(xbstree_floor(tree1, "3"), "2") == 0);
                xassert(strcmp(xbstree_floor(tree1, "7"), "6") == 0);
                xassert(strcmp(xbstree_floor(tree1, "41"), "4") == 0);
                xassert(strcmp(xbstree_floor(tree1, "81"), "8") == 0);
                xassert_false(xbstree_floor(tree1, "0"));
            }

            /* xbstree_ceiling */
            {
                xassert(strcmp(xbstree_ceiling(tree1, "3"), "4") == 0);
                xassert(strcmp(xbstree_ceiling(tree1, "7"), "8") == 0);
                xassert(strcmp(xbstree_ceiling(tree1, "21"), "4") == 0);
                xassert(strcmp(xbstree_ceiling(tree1, "81"), "9") == 0);
                xassert_false(xbstree_ceiling(tree1, "91"));
            }

            /* xbstree_select */
            {
                xassert(strcmp(xbstree_select(tree1, 0), "1") == 0);
                xassert(strcmp(xbstree_select(tree1, 1), "2") == 0);
                xassert(strcmp(xbstree_select(tree1, 2), "4") == 0);
                xassert(strcmp(xbstree_select(tree1, 3), "5") == 0);
                xassert(strcmp(xbstree_select(tree1, 4), "5") == 0);
                xassert(strcmp(xbstree_select(tree1, 5), "6") == 0);
                xassert(strcmp(xbstree_select(tree1, 6), "8") == 0);
                xassert(strcmp(xbstree_select(tree1, 7), "9") == 0);
            }
            {
                xassert(strcmp(xbstree_select(tree, 0), "1") == 0);
                xassert(strcmp(xbstree_select(tree, 1), "2") == 0);
                xassert(strcmp(xbstree_select(tree, 2), "3") == 0);
                xassert(strcmp(xbstree_select(tree, 3), "3") == 0);
                xassert(strcmp(xbstree_select(tree, 4), "4") == 0);
                xassert(strcmp(xbstree_select(tree, 5), "4") == 0);
                xassert(strcmp(xbstree_select(tree, 6), "5") == 0);
                xassert(strcmp(xbstree_select(tree, 7), "6") == 0);
                xassert(strcmp(xbstree_select(tree, 8), "7") == 0);
                xassert(strcmp(xbstree_select(tree, 9), "7") == 0);
                xassert(strcmp(xbstree_select(tree, 10), "7") == 0);
                xassert(strcmp(xbstree_select(tree, 11), "8") == 0);
                xassert(strcmp(xbstree_select(tree, 12), "9") == 0);
            }

            /* xbstree_rank */
            {
                xassert(xbstree_rank(tree1, "1") == 0);
                xassert(xbstree_rank(tree1, "2") == 1);
                xassert(xbstree_rank(tree1, "4") == 2);
                xassert(xbstree_rank(tree1, "5") == 3);
                xassert(xbstree_rank(tree1, "6") == 5);
                xassert(xbstree_rank(tree1, "8") == 6);
                xassert(xbstree_rank(tree1, "9") == 7);
                xassert(xbstree_rank(tree1, "7") == -1);
            }
            {
                xassert(xbstree_rank(tree, "1") == 0);
                xassert(xbstree_rank(tree, "2") == 1);
                xassert(xbstree_rank(tree, "3") == 2);
                xassert(xbstree_rank(tree, "4") == 4);
                xassert(xbstree_rank(tree, "5") == 6);
                xassert(xbstree_rank(tree, "6") == 7);
                xassert(xbstree_rank(tree, "7") == 8);
                xassert(xbstree_rank(tree, "8") == 11);
                xassert(xbstree_rank(tree, "9") == 12);
                xassert(xbstree_rank(tree, "10") == -1);
            }

            xbstree_free(&tree1);
        }

        /* xbstree_get */
        {
            xassert(strcmp(xbstree_get(tree, "3"), "3-1") == 0);
            xassert(strcmp(xbstree_get(tree, "6"), "6") == 0);
            xassert(strcmp(xbstree_get(tree, "7"), "7-1") == 0);
        }

        /* xbstree_get_all */
        {
            XSList_PT list = xbstree_get_all(tree, "3");
            xassert(strcmp(list->head->value, "3-1") == 0);
            xassert(strcmp(list->head->next->value, "3-2") == 0);
            xslist_free(&list);

            list = xbstree_get_all(tree, "6");
            xassert(strcmp(list->head->value, "6") == 0);
            xslist_free(&list);

            list = xbstree_get_all(tree, "7");
            xassert(strcmp(list->head->value, "7-1") == 0);
            xassert(strcmp(list->head->next->value, "7-2") == 0);
            xslist_free(&list);

            /* all equal keys */
            {
                XBSTree_PT tree1 = xbstree_new(xbstree_compare, NULL);
                xbstree_put_repeat(tree1, "5", "1");
                xbstree_put_repeat(tree1, "5", "2");
                xbstree_put_repeat(tree1, "5", "3");
                xbstree_put_repeat(tree1, "5", "4");
                xbstree_put_repeat(tree1, "5", "5");

                XSList_PT list = xbstree_get_all(tree1, "5");
                xassert(xslist_size(list) == 5);
                xslist_free(&list);

                xbstree_free(&tree1);
            }
        }

        /* xbstree_find */
        {
            xassert(xbstree_find(tree, "9"));
            xassert_false(xbstree_find(tree, "10"));
        }

        /* xbstree_find_put_if_not*/
        {
            xassert(xbstree_size(tree) == 13);
            xassert(xbstree_find_put_if_not(tree, "9", "9-3"));
            xassert(xbstree_size(tree) == 13);
            xassert(strcmp(xbstree_get(tree, "9"), "9-1") == 0);
            xassert(xbstree_find_put_if_not(tree, "10", "10"));
            xassert(xbstree_size(tree) == 14);
        }

        /* xbstree_remove_min */
        {
            xbstree_remove_min(tree);
            xassert(xbstree_size(tree) == 13);
            xassert(strcmp(xbstree_ceiling(tree, "1"), "10") == 0);
        }

        /* xbstree_remove_max */
        {
            xbstree_remove_max(tree);
            xassert(xbstree_size(tree) == 12);
            xassert(strcmp(xbstree_floor(tree, "91"), "8") == 0);
        }

        /* xbstree_remove */
        {
            xbstree_remove(tree, "7");
            xassert(xbstree_size(tree) == 11);
            xassert(strcmp(xbstree_get(tree, "7"), "7-2") == 0);
        }

        xassert(xbstree_is_bstree(tree));
        xbstree_free(&tree);
    }

    /* xbstree_map_preorder */
    {
        /*            5
        *        /        \
        *       3          7
        *     /  \        /  \
        *    2    3      6    7
        *   /      \           \
        *  1        4           7
        *            \           \
        *             4           8
        *                          \
        *                           9
        */

        XBSTree_PT tree = xbstree_new(xbstree_compare, NULL);

        xbstree_put_repeat(tree, "5", "5");
        xbstree_put_repeat(tree, "3", "3-1");
        xbstree_put_repeat(tree, "3", "3-2");
        xbstree_put_repeat(tree, "2", "2");
        xbstree_put_repeat(tree, "4", "4-1");
        xbstree_put_repeat(tree, "4", "4-2");
        xbstree_put_repeat(tree, "1", "1");
        xbstree_put_repeat(tree, "7", "7-1");
        xbstree_put_repeat(tree, "7", "7-2");
        xbstree_put_repeat(tree, "7", "7-3");
        xbstree_put_repeat(tree, "6", "6");
        xbstree_put_repeat(tree, "8", "8");
        xbstree_put_repeat(tree, "9", "9");

        xassert(xbstree_is_bstree(tree));
        //xbstree_print(tree, 5);

        {
            //xbstree_map_preorder(tree, xbstree_apply_print, NULL);
            //xbstree_map_preorder_break_if_true(tree, xbstree_apply_print_break_true, NULL);
            //xbstree_map_preorder_break_if_false(tree, xbstree_apply_print_break_false, NULL);
            //xbstree_map_inorder(tree, xbstree_apply_print, NULL);
            //xbstree_map_inorder_break_if_true(tree, xbstree_apply_print_break_true, NULL);
            //xbstree_map_inorder_break_if_false(tree, xbstree_apply_print_break_false, NULL);
            //xbstree_map_postorder(tree, xbstree_apply_print, NULL);
            //xbstree_map_levelorder(tree, xbstree_apply_print, NULL);
            //xbstree_map_min_to_max(tree, xbstree_apply_print, NULL);
            //xbstree_map_max_to_min(tree, xbstree_apply_print, NULL);
        }

        xbstree_free(&tree);
    }

    /* xbstree_map_min_to_max_break_if_true */
    {
        XBSTree_PT tree = xbstree_new(xbstree_compare, NULL);
        xassert_false(xbstree_map_min_to_max_break_if_true(tree, xbstree_apply_print_break_true, NULL));
        xassert_false(xbstree_map_min_to_max_break_if_false(tree, xbstree_apply_print_break_true, NULL));
        xbstree_free(&tree);
    }

    /* xbstree_swap */
    {
        XBSTree_PT tree1 = xbstree_new(xbstree_compare, NULL);
        XBSTree_PT tree2 = xbstree_new(xbstree_compare, NULL);

        xbstree_put_repeat(tree1, "5", "5");
        xbstree_put_repeat(tree1, "3", "3-1");
        xbstree_put_repeat(tree1, "3", "3-2");
        xbstree_put_repeat(tree2, "2", "2");
        xbstree_put_repeat(tree2, "4", "4-1");
        xbstree_put_repeat(tree2, "4", "4-2");

        xbstree_swap(tree1, tree2);

        xassert(strcmp(tree1->root->key, "2") == 0);
        xassert(strcmp(tree2->root->key, "5") == 0);

        xassert(xbstree_is_bstree(tree1));
        xassert(xbstree_is_bstree(tree2));

        xbstree_free(&tree1);
        xbstree_free(&tree2);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }

}
