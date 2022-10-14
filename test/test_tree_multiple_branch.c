
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../tree_multiple_branch/xtree_multiple_branch_x.h"
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
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

void test_xmtree() {
    /* xmtree_new */
    {
        XMTree_PT tree = xmtree_new(test_cmpk, NULL);
        xassert(tree->cmp);
        xassert_false(tree->root);
        xassert_false(tree->cl);
        xmtree_free(&tree);
    }

    /* xmtree_put_repeat */
    {
        /*
        *            a
        *     /    /    \      \
        *    b    c      d      e
        *  /  \       /  |  \    \
        * f    g     h   i   j    k
        */
        XMTree_PT tree = xmtree_new(test_cmpk, NULL);
        XMTree_Node_PT a = xmtree_put_repeat(tree, NULL, "a", "a");
        XMTree_Node_PT b = xmtree_put_repeat(tree, a, "b", "b");
        XMTree_Node_PT c = xmtree_put_repeat(tree, a, "c", "c");
        XMTree_Node_PT d = xmtree_put_repeat(tree, a, "d", "d");
        XMTree_Node_PT e = xmtree_put_repeat(tree, a, "e", "e");
        XMTree_Node_PT f = xmtree_put_repeat(tree, b, "f", "f");
        XMTree_Node_PT g = xmtree_put_repeat(tree, b, "g", "g");
        XMTree_Node_PT h = xmtree_put_repeat(tree, d, "h", "h");
        XMTree_Node_PT i = xmtree_put_repeat(tree, d, "i", "i");
        XMTree_Node_PT j = xmtree_put_repeat(tree, d, "j", "j");
        XMTree_Node_PT k = xmtree_put_repeat(tree, e, "k", "k");

        //xmtree_to_string(tree);
        xmtree_free(&tree);
    }

    /* xmtree_clear */
    /* xmtree_deep_clear */
    {
        XMTree_PT tree = xmtree_new(test_cmpk, NULL);
        XMTree_Node_PT a = xmtree_put_repeat(tree, NULL, "a", "a");
        XMTree_Node_PT b = xmtree_put_repeat(tree, a, "b", "b");
        XMTree_Node_PT c = xmtree_put_repeat(tree, a, "c", "c");
        XMTree_Node_PT d = xmtree_put_repeat(tree, a, "d", "d");
        XMTree_Node_PT e = xmtree_put_repeat(tree, a, "e", "e");
        XMTree_Node_PT f = xmtree_put_repeat(tree, b, "f", "f");
        XMTree_Node_PT g = xmtree_put_repeat(tree, b, "g", "g");
        XMTree_Node_PT h = xmtree_put_repeat(tree, d, "h", "h");
        XMTree_Node_PT i = xmtree_put_repeat(tree, d, "i", "i");
        XMTree_Node_PT j = xmtree_put_repeat(tree, d, "j", "j");
        XMTree_Node_PT k = xmtree_put_repeat(tree, e, "k", "k");

        xmtree_clear(tree);
        xassert_false(tree->root);
        xmtree_free(&tree);
    }

    /* xmtree_free */
    /* xmtree_deep_free */

    /* xmtree_map_preorder */
    /* xmtree_map_preorder_break_if_true */
    /* xmtree_map_preorder_break_if_false */
    /* xmtree_map_postorder */
    /* xmtree_map_levelorder */

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
