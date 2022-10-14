
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "../include/xalgos.h"

static
int test_atom_cmp(void* atom1, void *atom2, void* cl) {    
    return strcmp((char*)atom1, (char*)atom2);
}

static 
bool test_atom_print(void* ptr, void* cl) {
    printf("%s\n", (char*)ptr);
    return true;
}

static 
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static
bool test_atom_find_string(XAtom_PT atom, char *str) {
    return xset_find(atom, str);
}

void test_xatom() {

    /* xatom_new */

    /* xatom_chars */
    {
        XAtom_PT atom = xatom_new(test_atom_cmp, NULL);

        /* add one new */
        {
            char* ptr = xatom_chars(atom, "abcdefg", 3);
            xassert(strlen(ptr) == 3);
            xassert(strcmp(ptr, "abc") == 0);
            xassert(test_atom_find_string(atom, "abc"));
            xassert(test_atom_find_string(atom, "abcdefg") == false);
        }

        /* add equal one */
        {
            char* ptr = xatom_chars(atom, "abc", 3);
            xassert(strcmp(ptr, "abc") == 0);
            xassert(test_atom_find_string(atom, "abc"));
            xassert(xatom_size(atom) == 1);
        }

        xatom_free(&atom);
    }

    /* xatom_string */
    {
        XAtom_PT atom = xatom_new(test_atom_cmp, NULL);
        char* ptr = xatom_string(atom, "abcdefg");

        xassert(strlen(ptr) == 7);
        xassert(strcmp(ptr, "abcdefg") == 0);
        xassert(test_atom_find_string(atom, "abcdefg"));

        xatom_free(&atom);
    }

    /* xatom_int */
    {
        /* == 0 */
        {
            XAtom_PT atom = xatom_new(test_atom_cmp, NULL);
            char* ptr = xatom_int(atom, 0);
            xassert(strlen(ptr) == 1);
            xassert(strcmp(ptr, "0") == 0);
            xassert(test_atom_find_string(atom, "0"));
            xatom_free(&atom);
        }

        /* 0 < */
        {
            XAtom_PT atom = xatom_new(test_atom_cmp, NULL);
            char *ptr = xatom_int(atom, 123456);
            xassert(strlen(ptr) == 6);
            xassert(strcmp(ptr, "123456") == 0);
            xassert(test_atom_find_string(atom, "123456"));
            xatom_free(&atom);
        }

        /* < 0 */
        {
            XAtom_PT atom = xatom_new(test_atom_cmp, NULL);
            char *ptr = xatom_int(atom, -123456);
            xassert(strlen(ptr) == 7);
            xassert(strcmp(ptr, "-123456") == 0);
            xassert(test_atom_find_string(atom, "-123456"));
            xatom_free(&atom);
        }
#if 0
        /* LONG_MIN */
        {
            XAtom_PT atom = xatom_new(0);
            char *ptr = xatom_int(atom, LONG_MIN);
            xassert(strcmp(ptr, "-2147483648") == 0);
            xassert(atom->size == 1);
            xatom_free(&atom);
        }

        /* LONG_MAX */
        {
            XAtom_PT atom = xatom_new(0);
            char *ptr = xatom_int(atom, LONG_MAX);
            xassert(strcmp(ptr, "2147483647") == 0);
            xassert(atom->size == 1);
            xatom_free(&atom);
        }
#endif
    }

    /* xatom_vload */
    {
        XAtom_PT atom = xatom_new(test_atom_cmp, NULL);
        xassert(xatom_vload(atom, "abc1", "abc2", "abc3", NULL));

        xassert(test_atom_find_string(atom, "abc1"));
        xassert(test_atom_find_string(atom, "abc2"));
        xassert(test_atom_find_string(atom, "abc3"));

        xassert(xatom_size(atom) == 3);
        xatom_free(&atom);
    }

    /* xatom_aload */
    {
        XAtom_PT atom = xatom_new(test_atom_cmp, NULL);
        char* arrays[3] = { "abc4", "abc5", "abc6" };
        xassert(xatom_aload(atom, arrays, 3));

        xassert(test_atom_find_string(atom, "abc4"));
        xassert(test_atom_find_string(atom, "abc5"));
        xassert(test_atom_find_string(atom, "abc6"));

        xassert(xatom_size(atom) == 3);
        xatom_free(&atom);
    }

    /* xatom_map */
    {
        XAtom_PT atom = xatom_new(test_atom_cmp, NULL);
        xassert(xatom_vload(atom, "a", "b", "c", "d", "e", "b", "a", "e", NULL));
        xassert(xatom_size(atom) == 5);
        //xatom_map(atom, test_atom_print, NULL);
        xatom_free(&atom);
    }
    
    /* xatom_remove */
    {
        XAtom_PT atom = xatom_new(test_atom_cmp, NULL);
        char *p = xatom_string(atom, "m");
        xassert(xatom_vload(atom, "a", "b", "c", "d", "e", "f", "g", "h", NULL));

        xassert(test_atom_find_string(atom, "m"));
        xassert(xatom_size(atom) == 9);

        /* remove "m" but not atom */
        xatom_remove(atom, "m");
        xassert(!test_atom_find_string(atom, "m"));
        xassert(xatom_size(atom) == 8);

        /* remove atom "m" */
        p = xatom_string(atom, "m");
        xassert(test_atom_find_string(atom, "m"));
        xassert(xatom_size(atom) == 9);

        xatom_remove(atom, p);
        xassert_false(test_atom_find_string(atom, "m"));
        xassert(xatom_size(atom) == 8);

        xatom_free(&atom);
    }

    /* xatom_clear */
    {
        XAtom_PT atom = xatom_new(test_atom_cmp, NULL);
        xassert(xatom_vload(atom, "a", "b", "c", "d", "e", "f", "g", "h", NULL));
        xassert(xatom_size(atom) == 8);

        xatom_clear(atom);
        xassert(xatom_size(atom) == 0);

        xassert(xatom_vload(atom, "a", "b", "c", "d", "e", "f", "g", "h", NULL));
        xassert(xatom_size(atom) == 8);

        xatom_free(&atom);
    }

    /* xatom_free : already tested by other cases */
    
    /* xatom_size */
    /* xatom_is_empty */
    {
        XAtom_PT atom = xatom_new(test_atom_cmp, NULL);
        xassert(xatom_size(atom) == 0);
        xassert(xatom_is_empty(atom));

        xassert(xatom_vload(atom, "a", "b", "c", "d", "e", "f", "g", "h", NULL));
        xassert(xatom_size(atom) == 8);
        xassert_false(xatom_is_empty(atom));

        xatom_clear(atom);
        xassert(xatom_size(atom) == 0);
        xassert(xatom_is_empty(atom));

        xatom_free(&atom);
    }

    /* xatom_is_atom */
    {
        XAtom_PT atom = xatom_new(test_atom_cmp, NULL);
        char *p = xatom_string(atom, "abc");
        xassert(xatom_is_atom(atom, p));
        xassert_false(xatom_is_atom(atom, "abc"));
        xatom_free(&atom);
    }

    /* memory leak check */
    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
