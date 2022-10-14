
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../tree_234b_search/xtree_234b_search_x.h"
#include "../include/xalgos.h"

//extern bool t234bstree_verify(T234BSTree_PT tree);

static
int my_compare(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static 
T234BSTree_PT t234bstree_random_string(int (*cmp)(void *key1, void *key2, void *cl), int total_size, int string_length) {
    xassert(cmp);
    xassert(0 < total_size);
    xassert(0 < string_length);

    if (!cmp || (total_size <= 0) || (string_length <= 0)) {
        return NULL;
    }

    {
        T234BSTree_PT tree = t234bstree_new(cmp, NULL);
        if (!tree) {
            return NULL;
        }

        {
            const char charsets[] = "0123456789";

            for (int i = 0; i < total_size; i++) {
                char* str = XMEM_MALLOC(string_length + 1);
                if (!str) {
                    t234bstree_deep_free(&tree);
                    return NULL;
                }

                for (int i = 0; i < string_length; ++i) {
                    str[i] = charsets[rand() % (sizeof(charsets) - 1)];
                }
                str[string_length] = '\0';

                if (!t234bstree_put_repeat(tree, str, NULL)) {
                    t234bstree_deep_free(&tree);
                    return NULL;
                }
            }
        }

        return tree;
    }
}

static
void t234bstree_printnode(char* str, int h) {
    if (!str) {
        return;
    }

    for (int i = 0; i < h; i++) {
        printf(" ");
    }
    printf("%s\n", str);

    return;
}

static
void t234bstree_print_impl(T234BSTree_Node_PT node, int h) {
    if (!node) {
        t234bstree_printnode("-", h);
        return;
    }

    if (node->key3) {
        t234bstree_print_impl(node->link4, h + 3);

        t234bstree_printnode((char*)node->key3, h);
    }

    if (node->key2) {
        t234bstree_print_impl(node->link3, h + 3);

        t234bstree_printnode((char*)node->key2, h);
    }

    t234bstree_print_impl(node->link2, h + 3);

    t234bstree_printnode((char*)node->key1, h);

    t234bstree_print_impl(node->link1, h + 3);
}

static 
void t234bstree_print(T234BSTree_PT tree, int start_blank) {
    xassert(tree);

    if (!tree) {
        t234bstree_printnode("-", start_blank);
        return;
    }

    t234bstree_print_impl(tree->root, start_blank + 3);
}

void test_t234bstree() {

    T234BSTree_PT tree = t234bstree_new(my_compare, NULL);

    t234bstree_put_repeat(tree, "6", "");
    t234bstree_put_repeat(tree, "4", "");
    t234bstree_put_repeat(tree, "3", "");
    t234bstree_put_repeat(tree, "5", "");
    t234bstree_put_repeat(tree, "1", "");
    t234bstree_put_repeat(tree, "6", "");
    t234bstree_put_repeat(tree, "2", "");
    t234bstree_put_repeat(tree, "7", "");
    t234bstree_put_repeat(tree, "6", "");
    t234bstree_put_repeat(tree, "4", "");
    t234bstree_put_repeat(tree, "3", "");
    t234bstree_put_repeat(tree, "2", "");
    t234bstree_put_repeat(tree, "7", "");
    t234bstree_put_repeat(tree, "6", "");
    t234bstree_put_repeat(tree, "4", "");
    t234bstree_put_repeat(tree, "3", "");
    t234bstree_put_repeat(tree, "1", "");
    t234bstree_put_repeat(tree, "7", "");
    t234bstree_put_repeat(tree, "6", "");
    t234bstree_put_repeat(tree, "2", "");
    t234bstree_put_repeat(tree, "6", "");
    t234bstree_put_repeat(tree, "4", "");
    t234bstree_put_repeat(tree, "3", "");
    t234bstree_put_repeat(tree, "5", "");
    t234bstree_put_repeat(tree, "1", "");
    t234bstree_put_repeat(tree, "4", "");
    t234bstree_put_repeat(tree, "3", "");
    t234bstree_put_repeat(tree, "5", "");

    //t234bstree_print(tree, 5);

    {

        XFifo_PT fifo = t234bstree_keys(tree, "3", "6");
#if 0
        while (true) {
            char *abc = (char*)xfifo_pop(fifo);
            if (abc) {
                printf("%s\n", abc);
            }
            else {
                break;
            }
        }
#endif
        xfifo_free(&fifo);
    }

    //t234bstree_print(tree, 5);

    t234bstree_free(&tree);

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
