
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include"../tree_binary/xtree_binary_x.h"
#include "../include/xalgos.h"

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static 
bool xbintree_print_apply(void *data, void *cl) {
    printf("%s\n", (char*)data);
    return true;
}

static
bool xbintree_print_apply_true(void *data, void *cl) {
    printf("%s\n", (char*)data);
    if (strcmp((char*)data, "5") == 0) {
        return true;
    }
    return false;
}

static
bool xbintree_print_apply_false(void *data, void *cl) {
    printf("%s\n", (char*)data);
    if (strcmp((char*)data, "8") == 0) {
        return false;
    }
    return true;
}

static
void xbintree_printnode(char* data, int h) {
    for (int i = 0; i < h; i++) {
        printf(" ");
    }
    printf("%s\n", data);

    return;
}

static
void xbintree_print_impl(XBinTree_Node_PT node, int h) {
    if (!node) {
        xbintree_printnode("-", h);
        return;
    }

    xbintree_print_impl(node->right, h + 3);

    xbintree_printnode((char*)node->data, h);

    xbintree_print_impl(node->left, h + 3);
}

static
void xbintree_print(XBinTree_PT tree, int start_blank) {
    xassert(tree);
    xassert(0 <= start_blank);

    if (start_blank < 0) {
        return;
    }

    if (!tree) {
        xbintree_printnode("-", start_blank);
        return;
    }

    xbintree_print_impl(tree->root, start_blank + 3);
}

void test_xbintree() {

    /* xbintree_new */
    {
        XBinTree_PT tree = xbintree_new();
        xassert(tree);
        xassert(tree->size == 0);
        xassert_false(tree->root);
        xbintree_free(&tree);
    }

    /* xbintree_new_node */
    {
        XBinTree_Node_PT node = xbintree_new_node("abc");
        xassert(node);
        xassert_false(node->parent);
        xassert_false(node->left);
        xassert_false(node->right);
        xassert(strcmp((char*)node->data, "abc") == 0);
        XMEM_FREE(node);
    }

    /* xbintree_copy */
    {
        XBinTree_PT tree = xbintree_new();

        xbintree_insert_left(tree, NULL, "a");
        xbintree_insert_left(tree, tree->root, "b");
        xbintree_insert_left(tree, tree->root->left, "c");
        xbintree_insert_left(tree, tree->root->left->left, "d");

        XBinTree_PT ntree = xbintree_copy(tree);
        xassert(strcmp((char*)ntree->root->data, "a") == 0);
        xassert_false(ntree->root->parent);
        xassert(strcmp((char*)ntree->root->left->data, "b") == 0);
        xassert(strcmp((char*)ntree->root->left->parent->data, "a") == 0);
        xassert(strcmp((char*)ntree->root->left->left->data, "c") == 0);
        xassert(strcmp((char*)ntree->root->left->left->parent->data, "b") == 0);
        xassert(strcmp((char*)ntree->root->left->left->left->data, "d") == 0);
        xassert(strcmp((char*)ntree->root->left->left->left->parent->data, "c") == 0);

        xbintree_free(&tree);
        xbintree_free(&ntree);
    }

    /* xbintree_deep_copy */
    {
        XBinTree_PT tree = xbintree_new();
        char *abc = XMEM_CALLOC(1, 4);
        memcpy(abc, "abc", 3);
        char *bcd = XMEM_CALLOC(1, 4);
        memcpy(bcd, "bcd", 3);
        char *cde = XMEM_CALLOC(1, 4);
        memcpy(cde, "cde", 3);
        char *def = XMEM_CALLOC(1, 4);
        memcpy(def, "def", 3);

        xbintree_insert_right(tree, NULL, abc);
        xbintree_insert_right(tree, tree->root, bcd);
        xbintree_insert_right(tree, tree->root->right, cde);
        xbintree_insert_right(tree, tree->root->right->right, def);
        
        XBinTree_PT ntree = xbintree_deep_copy(tree, 4);
        xassert(strcmp((char*)ntree->root->data, "abc") == 0);
        xassert_false(ntree->root->parent);
        xassert(strcmp((char*)ntree->root->right->data, "bcd") == 0);
        xassert(strcmp((char*)ntree->root->right->parent->data, "abc") == 0);
        xassert(strcmp((char*)ntree->root->right->right->data, "cde") == 0);
        xassert(strcmp((char*)ntree->root->right->right->parent->data, "bcd") == 0);
        xassert(strcmp((char*)ntree->root->right->right->right->data, "def") == 0);
        xassert(strcmp((char*)ntree->root->right->right->right->parent->data, "cde") == 0);

        xbintree_deep_free(&tree);
        xbintree_deep_free(&ntree);
    }

    /* xbintree_insert_left */
    {
        /* tree is null */
        {
            bool except = false;
            XBinTree_Node_PT node = xbintree_new_node("abc");

            XEXCEPT_TRY
                xbintree_insert_left(NULL, NULL, "abc");
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            XMEM_FREE(node);
        }

        /* data is null */
        {
            XBinTree_PT tree = xbintree_new();

            bool except = false;
            XEXCEPT_TRY
                xbintree_insert_left(tree, NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xbintree_free(&tree);
        }

        /* node is null, root is null */
        {
            XBinTree_PT tree = xbintree_new();
            xassert(xbintree_insert_left(tree, NULL, "abc"));
            xassert(tree->size == 1);
            xassert(tree->root);
            xassert_false(tree->root->parent);
            xassert_false(tree->root->left);
            xassert_false(tree->root->right);
            xbintree_free(&tree);
        }

        /* node is null, root is not null */
        {
            XBinTree_PT tree = xbintree_new();
            xassert(xbintree_insert_left(tree, NULL, "abc"));
            xassert_false(xbintree_insert_left(tree, NULL, "def"));
            xassert(tree->size == 1);
            xassert_false(tree->root->parent);
            xassert_false(tree->root->left);
            xassert_false(tree->root->right);
            xbintree_free(&tree);
        }

        {
            XBinTree_PT tree = xbintree_new();
            xbintree_insert_left(tree, NULL, "a");
            xbintree_insert_left(tree, tree->root, "b");
            xbintree_insert_left(tree, tree->root, "c");

            xassert(strcmp((char*)tree->root->data, "a") == 0);
            xassert(strcmp((char*)tree->root->left->data, "c") == 0);
            xassert(strcmp((char*)tree->root->left->left->data, "b") == 0);

            xassert_false(tree->root->right);
            xassert_false(tree->root->left->right);
            xassert_false(tree->root->left->left->right);

            xassert_false(tree->root->parent);
            xassert(tree->root->left->parent = tree->root);
            xassert(tree->root->left->left->parent = tree->root->left);

            xassert(tree->size == 3);
            xbintree_free(&tree);
        }
    }

    /* xbintree_insert_right */
    {
        /* tree is null */
        {
            bool except = false;
            XBinTree_Node_PT node = xbintree_new_node("abc");

            XEXCEPT_TRY
                xbintree_insert_right(NULL, NULL, "abc");
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            XMEM_FREE(node);
        }

        /* data is null */
        {
            XBinTree_PT tree = xbintree_new();

            bool except = false;
            XEXCEPT_TRY
                xbintree_insert_right(tree, NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xbintree_free(&tree);
        }

        /* node is null, root is null */
        {
            XBinTree_PT tree = xbintree_new();
            xassert(xbintree_insert_right(tree, NULL, "abc"));
            xassert(tree->size == 1);
            xassert(tree->root);
            xassert_false(tree->root->parent);
            xassert_false(tree->root->left);
            xassert_false(tree->root->right);
            xbintree_free(&tree);
        }

        /* node is null, root is not null */
        {
            XBinTree_PT tree = xbintree_new();
            xassert(xbintree_insert_right(tree, NULL, "abc"));
            xassert_false(xbintree_insert_right(tree, NULL, "def"));
            xassert(tree->size == 1);
            xassert_false(tree->root->parent);
            xassert_false(tree->root->left);
            xassert_false(tree->root->right);
            xbintree_free(&tree);
        }

        {
            XBinTree_PT tree = xbintree_new();
            xbintree_insert_right(tree, NULL, "a");
            xbintree_insert_right(tree, tree->root, "b");
            xbintree_insert_right(tree, tree->root, "c");

            xassert(strcmp((char*)tree->root->data, "a") == 0);
            xassert(strcmp((char*)tree->root->right->data, "c") == 0);
            xassert(strcmp((char*)tree->root->right->right->data, "b") == 0);

            xassert_false(tree->root->left);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right->left);

            xassert_false(tree->root->parent);
            xassert(tree->root->right->parent = tree->root);
            xassert(tree->root->right->right->parent = tree->root->right);

            xassert(tree->size == 3);
            xbintree_free(&tree);
        }
    }

    /* xbintree_root */
    {
        /* tree is null */
        {
            xassert_false(xbintree_root(NULL));
        }

        /* root is null */
        {
            XBinTree_PT tree = xbintree_new();
            xassert_false(xbintree_root(tree));
            xbintree_free(&tree);
        }

        /* root is not null */
        {
            XBinTree_PT tree = xbintree_new();
            xbintree_insert_right(tree, NULL, "abc");
            xassert(strcmp((char*)xbintree_root(tree), "abc") == 0);
            xbintree_free(&tree);
        }
    }

    /* xbintree_remove_left */
    {
        /* tree is null */
        {
            bool except = false;
            XEXCEPT_TRY
                xbintree_remove_left(NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* node is null */
        {
            XBinTree_PT tree = xbintree_new();

            bool except = false;
            XEXCEPT_TRY
                xbintree_remove_left(tree, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xbintree_free(&tree);
        }

        /* normal case */
        {
            XBinTree_PT tree = xbintree_new();
            xbintree_insert_left(tree, NULL, "a");
            xbintree_insert_left(tree, tree->root, "b");
            xbintree_insert_left(tree, tree->root->left, "c");
            xbintree_insert_left(tree, tree->root->left->left, "d");
            xbintree_remove_left(tree, tree->root->left);
            xassert(tree->size == 2);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);
            xassert(tree->root->left->parent == tree->root);
            xassert(strcmp((char*)tree->root->left->data, "b") == 0);
            xbintree_free(&tree);
        }
    }

    /* xbintree_deep_remove_left */
    {
        /* tree is null */
        /* node is null */
        {
            // all tested by xbintree_remove_left already
        }

        /* normal case */
        {
            XBinTree_PT tree = xbintree_new();
            char *abc = XMEM_CALLOC(1, 4);
            memcpy(abc, "abc", 3);
            char *bcd = XMEM_CALLOC(1, 4);
            memcpy(bcd, "bcd", 3);
            char *cde = XMEM_CALLOC(1, 4);
            memcpy(cde, "cde", 3);
            char *def = XMEM_CALLOC(1, 4);
            memcpy(def, "def", 3);

            xbintree_insert_left(tree, NULL, abc);
            xbintree_insert_left(tree, tree->root, bcd);
            xbintree_insert_left(tree, tree->root->left, cde);
            xbintree_insert_left(tree, tree->root->left->left, def);
            xbintree_deep_remove_left(tree, tree->root->left);
            xassert(tree->size == 2);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);
            xassert(tree->root->left->parent == tree->root);
            xassert(tree->root->left->data == bcd);
            xbintree_deep_free(&tree);
        }
    }

    /* xbintree_remove_right */
    {
        /* tree is null */
        {
            bool except = false;
            XEXCEPT_TRY
                xbintree_remove_right(NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }
        /* node is null */
        {
            XBinTree_PT tree = xbintree_new();

            bool except = false;
            XEXCEPT_TRY
                xbintree_remove_right(tree, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xbintree_free(&tree);
        }

        /* normal case */
        {
            XBinTree_PT tree = xbintree_new();
            xbintree_insert_right(tree, NULL, "a");
            xbintree_insert_right(tree, tree->root, "b");
            xbintree_insert_right(tree, tree->root->right, "c");
            xbintree_insert_right(tree, tree->root->right->right, "d");
            xbintree_remove_right(tree, tree->root->right);
            xassert(tree->size == 2);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);
            xassert(tree->root->right->parent == tree->root);
            xassert(strcmp((char*)tree->root->right->data, "b") == 0);
            xbintree_free(&tree);
        }
    }

    /* xbintree_deep_remove_right */
    {
        /* tree is null */
        /* node is null */
        {
            // all tested by xbintree_remove_right already
        }

        /* normal case */
        {
            XBinTree_PT tree = xbintree_new();
            char *abc = XMEM_CALLOC(1, 4);
            memcpy(abc, "abc", 3);
            char *bcd = XMEM_CALLOC(1, 4);
            memcpy(bcd, "bcd", 3);
            char *cde = XMEM_CALLOC(1, 4);
            memcpy(cde, "cde", 3);
            char *def = XMEM_CALLOC(1, 4);
            memcpy(def, "def", 3);

            xbintree_insert_right(tree, NULL, abc);
            xbintree_insert_right(tree, tree->root, bcd);
            xbintree_insert_right(tree, tree->root->right, cde);
            xbintree_insert_right(tree, tree->root->right->right, def);
            xbintree_deep_remove_right(tree, tree->root->right);
            xassert(tree->size == 2);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);
            xassert(tree->root->right->parent == tree->root);
            xassert(tree->root->right->data == bcd);
            xbintree_deep_free(&tree);
        }
    }

    /* xbintree_clear */
    {
        /* normal case */
        {
            XBinTree_PT tree = xbintree_new();
            xbintree_insert_right(tree, NULL, "abc");
            xbintree_insert_right(tree, tree->root, "bcd");
            xbintree_insert_left(tree, tree->root, "bcd2");
            xbintree_insert_right(tree, tree->root->right, "cde");
            xbintree_insert_left(tree, tree->root->left, "cde");
            xbintree_insert_right(tree, tree->root->right->right, "def");
            xbintree_clear(tree);
            xassert(tree->size == 0);
            xassert_false(tree->root);
            xbintree_free(&tree);
        }
    }

    /* xbintree_deep_clear */
    {
        /* normal case */
        {
            XBinTree_PT tree = xbintree_new();
            char *abc = XMEM_CALLOC(1, 4);
            memcpy(abc, "abc", 3);
            char *bcd = XMEM_CALLOC(1, 4);
            memcpy(bcd, "bcd", 3);
            char *cde = XMEM_CALLOC(1, 4);
            memcpy(cde, "cde", 3);
            char *def = XMEM_CALLOC(1, 4);
            memcpy(def, "def", 3);

            xbintree_insert_right(tree, NULL, abc);
            xbintree_insert_right(tree, tree->root, bcd);
            xbintree_insert_right(tree, tree->root->right, cde);
            xbintree_insert_right(tree, tree->root->right->right, def);
            xbintree_deep_clear(tree);
            xassert(tree->size == 0);
            xassert_false(tree->root);
            xbintree_free(&tree);
        }
    }

    /* xbintree_free */
    {
        /* tree is null */
        {
            xbintree_free(NULL);
        }

        /* *tree is null */
        {
            XBinTree_PT *tree = NULL;
            xbintree_free(tree);
        }

        /* normal case */
        {
            XBinTree_PT tree = xbintree_new();
            xbintree_insert_right(tree, NULL, "abc");
            xbintree_insert_right(tree, tree->root, "bcd");
            xbintree_insert_left(tree, tree->root, "bcd2");
            xbintree_insert_right(tree, tree->root->right, "cde");
            xbintree_insert_left(tree, tree->root->left, "cde");
            xbintree_insert_right(tree, tree->root->right->right, "def");
            xbintree_free(&tree);
        }
    }

    /* xbintree_deep_free */
    {
        /* tree is null */
        {
            xbintree_deep_free(NULL);
        }

        /* *tree is null */
        {
            XBinTree_PT *tree = NULL;
            xbintree_deep_free(tree);
        }

        /* normal case */
        {
            XBinTree_PT tree = xbintree_new();
            char *abc = XMEM_CALLOC(1, 4);
            memcpy(abc, "abc", 3);
            char *bcd = XMEM_CALLOC(1, 4);
            memcpy(bcd, "bcd", 3);
            char *cde = XMEM_CALLOC(1, 4);
            memcpy(cde, "cde", 3);
            char *def = XMEM_CALLOC(1, 4);
            memcpy(def, "def", 3);

            xbintree_insert_right(tree, NULL, abc);
            xbintree_insert_right(tree, tree->root, bcd);
            xbintree_insert_right(tree, tree->root->right, cde);
            xbintree_insert_right(tree, tree->root->right->right, def);
            xbintree_deep_free(&tree);
        }
    }

    /* xbintree_remove_left_most */
    {
        /* root has no left and right */
        {
            XBinTree_PT tree = xbintree_new();
            xbintree_insert_left(tree, NULL, "abc");
            xbintree_remove_left_most(tree);
            xassert(tree->size == 0);
            xassert_false(tree->root);
            xbintree_free(&tree);
        }

        /* root has no left */
        {
            XBinTree_PT tree = xbintree_new();
            xbintree_insert_left(tree, NULL, "abc");
            xbintree_insert_right(tree, tree->root, "bcd");
            xbintree_remove_left_most(tree);
            xassert(tree->size == 1);
            xassert_false(tree->root->left);
            xassert_false(tree->root->right);
            xassert_false(tree->root->parent);
            xassert(strcmp(tree->root->data, "bcd") == 0);
            xbintree_free(&tree);
        }

        /* root has no right */
        {
            XBinTree_PT tree = xbintree_new();
            xbintree_insert_left(tree, NULL, "abc");
            xbintree_remove_left_most(tree);
            xassert(tree->size == 0);
            xassert_false(tree->root);
            xbintree_free(&tree);
        }

        /* node has no branches */
        {
            XBinTree_PT tree = xbintree_new();
            xbintree_insert_left(tree, NULL, "abc");
            xbintree_insert_left(tree, tree->root, "bcd");
            xbintree_insert_left(tree, tree->root->left, "cde");
            xbintree_remove_left_most(tree);
            xassert(tree->size == 2);
            xassert(tree->root->left);
            xassert_false(tree->root->left->left);
            xassert_false(tree->root->left->right);
            xassert(strcmp(tree->root->left->data, "bcd") == 0);
            xbintree_free(&tree);
        }

        /* node has no left branch */
        {
            XBinTree_PT tree = xbintree_new();
            xbintree_insert_left(tree, NULL, "abc");
            xbintree_insert_left(tree, tree->root, "bcd");
            xbintree_insert_left(tree, tree->root->left, "cde");
            xbintree_insert_right(tree, tree->root->left->left, "def");
            xbintree_remove_left_most(tree);
            xassert(tree->size == 3);
            xassert(tree->root->left->left);
            xassert_false(tree->root->left->right);
            xassert(strcmp(tree->root->left->data, "bcd") == 0);
            xassert(strcmp(tree->root->left->left->data, "def") == 0);
            xassert(tree->root->left->left->parent == tree->root->left);
            xassert_false(tree->root->left->left->left);
            xassert_false(tree->root->left->left->right);
            xbintree_free(&tree);
        }
    }

    /* xbintree_remove_right_most */
    {
        /* root has no left and right */
        {
            XBinTree_PT tree = xbintree_new();
            xbintree_insert_right(tree, NULL, "abc");
            xbintree_remove_right_most(tree);
            xassert(tree->size == 0);
            xassert_false(tree->root);
            xbintree_free(&tree);
        }

        /* root has no right */
        {
            XBinTree_PT tree = xbintree_new();
            xbintree_insert_left(tree, NULL, "abc");
            xbintree_insert_left(tree, tree->root, "bcd");
            xbintree_remove_right_most(tree);
            xassert(tree->size == 1);
            xassert_false(tree->root->left);
            xassert_false(tree->root->right);
            xassert_false(tree->root->parent);
            xassert(strcmp(tree->root->data, "bcd") == 0);
            xbintree_free(&tree);
        }

        /* root has no left */
        {
            XBinTree_PT tree = xbintree_new();
            xbintree_insert_left(tree, NULL, "abc");
            xbintree_insert_right(tree, tree->root, "bcd");
            xbintree_remove_right_most(tree);
            xassert(tree->size == 1);
            xassert_false(tree->root->left);
            xassert_false(tree->root->right);
            xassert_false(tree->root->parent);
            xassert(strcmp(tree->root->data, "abc") == 0);
            xbintree_free(&tree);
        }

        /* node has no branches */
        {
            XBinTree_PT tree = xbintree_new();
            xbintree_insert_right(tree, NULL, "abc");
            xbintree_insert_right(tree, tree->root, "bcd");
            xbintree_insert_right(tree, tree->root->right, "cde");
            xbintree_remove_right_most(tree);
            xassert(tree->size == 2);
            xassert(tree->root->right);
            xassert_false(tree->root->right->left);
            xassert_false(tree->root->right->right);
            xassert(strcmp(tree->root->right->data, "bcd") == 0);
            xbintree_free(&tree);
        }

        /* node has no right branch */
        {
            XBinTree_PT tree = xbintree_new();
            xbintree_insert_right(tree, NULL, "abc");
            xbintree_insert_right(tree, tree->root, "bcd");
            xbintree_insert_right(tree, tree->root->right, "cde");
            xbintree_insert_left(tree, tree->root->right->right, "def");
            xbintree_remove_right_most(tree);
            xassert(tree->size == 3);
            xassert(tree->root->right->right);
            xassert_false(tree->root->right->left);
            xassert(tree->root->right->right->parent == tree->root->right);
            xassert(strcmp(tree->root->right->data, "bcd") == 0);
            xassert(strcmp(tree->root->right->right->data, "def") == 0);
            xbintree_free(&tree);
        }
    }

    {
        /*                   6
         *                /     \
         *               4       8 
         *             /   \    /  \   
         *            2     5  7    9
         *           / \ 
         *          1   3
         */
        XBinTree_PT tree = xbintree_new();
        xbintree_insert_left(tree, NULL, "6");
        xbintree_insert_left(tree, tree->root, "4");
        xbintree_insert_left(tree, tree->root->left, "2");
        xbintree_insert_right(tree, tree->root->left, "5");
        xbintree_insert_left(tree, tree->root->left->left, "1");
        xbintree_insert_right(tree, tree->root->left->left, "3");
        xbintree_insert_right(tree, tree->root, "8");
        xbintree_insert_left(tree, tree->root->right, "7");
        xbintree_insert_right(tree, tree->root->right, "9");

        //xbintree_map_preorder(tree, xbintree_print_apply, NULL);
        //xbintree_map_inorder(tree, xbintree_print_apply, NULL);
        //xbintree_map_postorder(tree, xbintree_print_apply, NULL);
        //xbintree_map_levelorder(tree, xbintree_print_apply, NULL);

        //xbintree_map_inorder_break_if_true(tree, xbintree_print_apply_true, NULL);
        //xbintree_map_inorder_break_if_false(tree, xbintree_print_apply_false, NULL);

        xbintree_free(&tree);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }

}
