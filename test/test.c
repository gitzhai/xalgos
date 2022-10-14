#include <stdio.h>

#include "../include/xalgos.h"

extern void test_xexcept();
extern void test_xassert();

extern void test_xbit();
extern void test_xpair();

extern void test_xrslist();
extern void test_xslist();
extern void test_xsclist();
extern void test_xkvslist();

extern void test_xrdlist();
extern void test_xdlist();
extern void test_xdclist();
extern void test_xkvdlist();

extern void test_xatom();

extern void test_xarray();
extern void test_xparray();
extern void test_xiarray();

extern void test_xpseq();
extern void test_xiseq();

extern void test_xdeque();
extern void test_xfifo();
extern void test_xlifo();
extern void test_xstack();
extern void test_xqueue();

extern void test_tmaxheap();
extern void test_tminheap();

extern void test_xfibheap();

extern void test_xmaxbinque();
extern void test_xminbinque();

extern void test_xmaxpq();
extern void test_xminpq();

extern void test_xindexmaxheap();
extern void test_xindexminheap();

extern void test_xindexmaxpq();
extern void test_xindexminpq();

extern void test_xkeyindexminpq();
extern void test_xkeyindexmaxpq();

extern void test_xkvhashtab();
extern void test_xrbtreehash();

extern void test_xbintree();
extern void test_xbstree();
extern void test_t234bstree();
extern void test_xrbtree();
extern void test_xlistrbtree();
extern void test_xavltree();
extern void test_xmtree();

extern void test_xset();
extern void test_xhashset();

extern void test_xgraph();
extern void test_xdigraph();
extern void test_xwgraph();
extern void test_xwdigraph();

int main(void){
    //test_xexcept();

    test_xassert();

    test_xbit();
    test_xpair();

    test_xatom();

    test_xrslist();
    test_xslist();
    test_xsclist();
    test_xkvslist();

    test_xrdlist();
    test_xdlist();
    test_xkvdlist();
    test_xdclist();

    test_xarray();
    test_xparray();
    test_xiarray();

    test_xpseq();
    test_xiseq();

    test_xdeque();
    test_xfifo();
    test_xlifo();
    test_xstack();
    test_xqueue();

    test_tmaxheap();
    test_tminheap();

    test_xfibheap();

    test_xmaxbinque();
    test_xminbinque();

    test_xmaxpq();
    test_xminpq();

    test_xindexmaxheap();
    test_xindexminheap();

    test_xindexmaxpq();
    test_xindexminpq();

    test_xkeyindexminpq();
    test_xkeyindexmaxpq();

    test_xbintree();
    test_xbstree();

    test_t234bstree();
    test_xrbtree();
    test_xlistrbtree();
    test_xavltree();
    test_xmtree();

    test_xset();
    test_xhashset();

    test_xkvhashtab();
    test_xrbtreehash();

    test_xgraph();
    test_xdigraph();
    test_xwgraph();
    test_xwdigraph();

    printf("\n\n   All Pass !!!  \n\n");
    getchar();

    return 0;
}

