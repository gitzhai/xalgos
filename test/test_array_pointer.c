
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../array_pointer/xarray_pointer_x.h"
#include "../include/xalgos.h"

static
bool apply_true(void *x, void *cl) {
    return true;
}

static
bool apply_false(void *x, void *cl) {
    return false;
}

static
bool apply_compare(void *x, void *cl) {
    return strcmp((char*)x, (char*)cl) == 0;
}

static
bool apply_remove_true(void *x, void *cl) {
    return true;
}

static
bool apply_remove_false(void *x, void *cl) {
    return false;
}

static
bool apply_remove_compare(void *x, void *cl) {
    if (!x) {
        return false;
    }

    return strcmp((char*)x, (char*)cl) == 0;
}

static
int parray_lg_test(int n) {
    int k = 0;
    for (; n != 1; n >>= 1) {
        ++k;
    }
    return k;
}

static
bool apply_print(void *x, void *cl) {
    printf("%s\n", (char*)x);
    return true;
}

static
bool apply_int_print(int x, void *cl) {
    printf("%d : %s \n", x, (char*)xarray_get((XArray_PT)cl, x));
    return true;
}

static
int sort_compare(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static
bool test_xparray_apply(void *x, void *cl) {
    printf("%s\n", (char*)x);
    return true;
}

static
int test_xparray_cmp(void *x, void *y, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static 
XPArray_PT xparray_random_string(int size) {
    XPArray_PT array = xparray_new(size);
    if (!array) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;

        for (int i = 0; i < array->size; i++) {
            char* str = XMEM_CALLOC(1, str_size);
            if (!str) {
                xparray_deep_free(&array);
                return NULL;
            }

            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }

            str[str_size - 1] = '\0';

            xparray_put(array, i, str, NULL);
        }
    }

    return array;
}

void test_xparray() {

    /* xparray_new */
    {
        /* size < 0 */
        {
            bool except = false;

            XEXCEPT_TRY
                XPArray_PT array = xparray_new(-1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* size == 0 */
        {
            XPArray_PT array = xparray_new(0);
            xassert(array->size == 0);
            xassert(array->datas == NULL);
            xparray_free(&array);
        }

        /* 0 < size */
        {
            XPArray_PT array = xparray_new(10);
            xassert(array->size == 10);
            xassert(array->datas);
            xparray_free(&array);
        }
    }

    /* xparray_copyn_impl */
    {
        /* count == 0 */
        {
            XPArray_PT array = xparray_random_string(10);
            XPArray_PT narray = xparray_copyn_impl(array, 0, 0, 0, false);
            xassert(narray->size == 10);
            xassert(narray->datas);
            xparray_free(&narray);
            xparray_deep_free(&array);
        }

        /* array->size == 0  && array->size < count*/
        {
            XPArray_PT array = xparray_new(0);
            XPArray_PT narray = xparray_copyn_impl(array, 0, 20, 10, true);
            xassert(narray->size == 0);
            xassert_false(narray->datas);
            xparray_free(&narray);
            xparray_free(&array);
        }

        /* array->size == 0  && array->size < count + start*/
        {
            XPArray_PT array = xparray_new(0);
            XPArray_PT narray = xparray_copyn_impl(array, 3, 20, 0, false);
            xassert(narray->size == 0);
            xassert_false(narray->datas);
            xparray_free(&narray);
            xparray_free(&array);
        }

        /* array->size < count*/
        {
            XPArray_PT array = xparray_random_string(10);
            XPArray_PT narray = xparray_copyn_impl(array, 0, 20, 10, true);
            xassert(narray->size == 10);
            xassert(narray->datas);

            for (int i = 0; i < 10; i++) {
                xassert(strcmp(xparray_get(array, i), xparray_get(narray, i)) == 0);
            }

            xparray_deep_free(&narray);
            xparray_deep_free(&array);
        }

        /* array->size < count && 0 < start */
        {
            XPArray_PT array = xparray_random_string(10);
            XPArray_PT narray = xparray_copyn_impl(array, 2, 20, 0, false);
            xassert(narray->size == 10);
            xassert(narray->datas);

            for (int i = 0; i < 8; i++) {
                xassert(xparray_get(array, i + 2) == xparray_get(narray, i));
            }

            xparray_free(&narray);
            xparray_deep_free(&array);
        }

        /* count < array->size */
        {
            XPArray_PT array = xparray_random_string(10);
            XPArray_PT narray = xparray_copyn_impl(array, 0, 5, 10, true);
            xassert(narray->size == 10);
            xassert(narray->datas);

            for (int i = 0; i < 5; i++) {
                xassert(strcmp(xparray_get(array, i), xparray_get(narray, i)) == 0);
            }

            xparray_deep_free(&narray);
            xparray_deep_free(&array);
        }

        /* 0 < start && start + count < array->size */
        {
            XPArray_PT array = xparray_random_string(10);
            XPArray_PT narray = xparray_copyn_impl(array, 2, 5, 0, false);
            xassert(narray->size == 10);
            xassert(narray->datas);

            for (int i = 0; i < 5; i++) {
                xassert(xparray_get(array, i + 2) == xparray_get(narray, i));
            }

            xparray_free(&narray);
            xparray_deep_free(&array);
        }

        /* 0 < start && start + count == array->size */
        {
            XPArray_PT array = xparray_random_string(10);
            XPArray_PT narray = xparray_copyn_impl(array, 2, 8, 10, true);
            xassert(narray->size == 10);
            xassert(narray->datas);

            for (int i = 0; i < 8; i++) {
                xassert(strcmp(xparray_get(array, i + 2), xparray_get(narray, i)) == 0);
            }

            xparray_deep_free(&narray);
            xparray_deep_free(&array);
        }

        /* 0 < start && array->size < start + count */
        {
            XPArray_PT array = xparray_random_string(10);
            XPArray_PT narray = xparray_copyn_impl(array, 2, 10, 0, false);
            xassert(narray->size == 10);
            xassert(narray->datas);

            for (int i = 0; i < 8; i++) {
                xassert(xparray_get(array, i + 2) == xparray_get(narray, i));
            }
            xassert(xparray_get(narray, 8) == NULL);
            xassert(xparray_get(narray, 9) == NULL);

            xparray_free(&narray);
            xparray_deep_free(&array);
        }
    }

    /* xparray_copy */
    {
        /* tested by xparray_copyn */
    }

    /* xparray_copyn */
    {
        /* array == NULL */
        {
            bool except = false;
            XEXCEPT_TRY
                xparray_copyn(NULL, 1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* count < 0 */
        {
            XPArray_PT array = xparray_new(10);
            bool except = false;
            XEXCEPT_TRY
                xparray_copyn(array, -1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xparray_free(&array);
        }

        /* all other scenarios will be tested in xparray_copyn_impl */
    }

    /* xparray_deep_copy */
    {
        /* tested by xparray_deep_copyn */
    }

    /* xparray_deep_copyn */
    {
        /* array == NULL */
        {
            bool except = false;
            XEXCEPT_TRY
                xparray_deep_copyn(NULL, 1, 1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* count < 0 */
        {
            XPArray_PT array = xparray_new(10);
            bool except = false;
            XEXCEPT_TRY
                xparray_deep_copyn(array, -1, 1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xparray_free(&array);
        }

        /* elem_size == 0 */
        {
            XPArray_PT array = xparray_new(10);
            bool except = false;
            XEXCEPT_TRY
                xparray_deep_copyn(array, -1, 0);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xparray_free(&array);
        }

        /* elem_size < 0 */
        {
            XPArray_PT array = xparray_new(10);
            bool except = false;
            XEXCEPT_TRY
                xparray_deep_copyn(array, 1, -1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xparray_free(&array);
        }

        /* all other scenarios will be tested in xparray_copyn_if_impl */
    }

    /* xparray_vload */
    {
        /* array == NULL */
        {
            bool except = false;
            XEXCEPT_TRY
                xparray_vload(NULL, "abc", NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* x == NULL */
        {
            XPArray_PT array = xparray_new(10);
            xassert(xparray_vload(array, NULL) == 0);
            xparray_free(&array);
        }

        /* NULL in the middle */
        {
            XPArray_PT array = xparray_new(10);

            xassert(xparray_vload(array, "abc", NULL, "xyz", NULL) == 1);

            xassert(strcmp((char*)xparray_get(array, 0), "abc") == 0);
            xassert_false(xparray_get(array, 1));
            xassert_false(xparray_get(array, 2));

            xparray_free(&array);
        }

        /* normal */
        {
            XPArray_PT array = xparray_new(10);

            xassert(xparray_vload(array, "abc", "def", "xyz", NULL) == 3);
            xassert(strcmp((char*)xparray_get(array, 2), "xyz") == 0);
            xassert_false(xparray_get(array, 3));
            xparray_free(&array);
        }
    }

    /* xparray_aload */
    {
        /* xs == NULL */
        {
            XPArray_PT array = xparray_new(1);
            bool except = false;
            XEXCEPT_TRY
                xparray_aload(array, NULL, 1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xparray_free(&array);
        }

        /* len < 0 */
        {
            XPArray_PT array = xparray_new(1);
            char *temp[2];

            bool except = false;
            XEXCEPT_TRY
                xparray_aload(array, (void**)temp, -1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xparray_free(&array);
        }

        {
            XPArray_PT array = xparray_new(10);
            char *temp[4] = { "abc", NULL, "xyz", NULL};
            xassert(xparray_aload(array, (void**)temp, 4) == 4);

            xassert(strcmp((char*)xparray_get(array, 0), "abc") == 0);
            xassert(xparray_get(array, 1) == NULL);
            xassert(strcmp((char*)xparray_get(array, 2), "xyz") == 0);
            xassert(xparray_get(array, 3) == NULL);
            xassert_false(xparray_get(array, 4));

            xparray_free(&array);
        }
    }

    /* xparray_put */
    {
        /* array == NULL */
        {
            bool except = false;
            XEXCEPT_TRY
                xparray_put(NULL, 0, "abc", NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* i < 0 */
        {
            XPArray_PT array = xparray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xparray_put(array, -1, "abc", NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xparray_free(&array);
        }

        /* array->size < i*/
        {
            XPArray_PT array = xparray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xparray_put(array, 20, "abc", NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xparray_free(&array);
        }

        /* array->size == i*/
        {
            XPArray_PT array = xparray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xparray_put(array, 10, "abc", NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xparray_free(&array);
        }

        /* normal */
        {
            XPArray_PT array = xparray_new(10);

            xassert(xparray_put(array, 0, "abc", NULL));
            xassert(xparray_put(array, 1, "def", NULL));
            xassert(xparray_put(array, 2, "xyz", NULL));

            xassert(strcmp((char*)xparray_get(array, 0), "abc") == 0);
            xassert(strcmp((char*)xparray_get(array, 1), "def") == 0);
            xassert(strcmp((char*)xparray_get(array, 2), "xyz") == 0);

            xassert(xparray_put(array, 1, NULL, NULL));
            xassert_false(xparray_get(array, 1));

            void *memory = NULL;
            xassert(xparray_put(array, 2, "xyz2", &memory));
            xassert(strcmp((char*)xparray_get(array, 2), "xyz2") == 0);
            xassert(strcmp((char*)memory, "xyz") == 0);

            xparray_free(&array);
        }
    }

    /* xparray_put_expand */
    {
        /* array == NULl */
        {
            bool except = false;
            XEXCEPT_TRY
                xparray_put_expand(NULL, 0, "abc", NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* i < 0 */
        {
            XPArray_PT array = xparray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xparray_put_expand(array, -1, "abc", NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xparray_free(&array);
        }

        /* array->size == i */
        {
            XPArray_PT array = xparray_new(10);

            xassert(xparray_put_expand(array, 10, "a", NULL));
            xassert(10 < array->size);
            xassert(strcmp((char*)xparray_get(array, 10), "a") == 0);

            xparray_free(&array);
        }

        /* array->size <= i */
        {
            XPArray_PT array = xparray_new(10);

            xassert(xparray_put_expand(array, 20, "b", NULL));
            xassert(20 < array->size);

            xassert(strcmp((char*)xparray_get(array, 20), "b") == 0);
            xassert_false(xparray_get(array, 15));

            xparray_free(&array);
        }
    }

    /* xparray_get */
    {
        /* array == NULL */
        {
            bool except = false;
            XEXCEPT_TRY
                xparray_get(NULL, 0);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* i < 0 */
        {
            XPArray_PT array = xparray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xparray_get(array, -1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xparray_free(&array);
        }

        /* array->size < i */
        {
            XPArray_PT array = xparray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xparray_get(array, 20);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xparray_free(&array);
        }

        /* normal */
        {
            XPArray_PT array = xparray_new(10);

            xassert(xparray_put(array, 0, "abc", NULL));
            xassert(xparray_put(array, 1, "def", NULL));
            xassert(xparray_put(array, 2, "xyz", NULL));

            xassert(strcmp((char*)xparray_get(array, 1), "def") == 0);
            xassert_false(xparray_get(array, 3));

            xparray_free(&array);
        }
    }

    /* xparray_get_kth_element */
    {
        XPArray_PT array = xparray_new(10);
        xparray_put(array, 0, "a", NULL);
        xparray_put(array, 1, "b", NULL);
        xparray_put(array, 2, "c", NULL);
        xparray_put(array, 3, "d", NULL);
        xparray_put(array, 4, "e", NULL);
        xparray_put(array, 5, "f", NULL);
        xparray_put(array, 6, "g", NULL);
        xparray_put(array, 7, "h", NULL);
        xparray_put(array, 8, "i", NULL);
        xparray_put(array, 9, "j", NULL);

        xassert(strcmp(xparray_get_kth_element_impl_quick_sort(array, 2, test_xparray_cmp, NULL), "c") == 0);
        xassert(strcmp(xparray_get_kth_element_impl_heap_sort(array, 2, test_xparray_cmp, NULL), "c") == 0);

        xassert(strcmp(xparray_get_kth_element_impl_quick_sort(array, 7, test_xparray_cmp, NULL), "h") == 0);
        xassert(strcmp(xparray_get_kth_element_impl_heap_sort(array, 7, test_xparray_cmp, NULL), "h") == 0);

        xparray_free(&array);
    }

    /* xparray_min */
    /* xparray_max */
    {
        XPArray_PT array = xparray_new(10);
        xparray_put(array, 0, "g", NULL);
        xparray_put(array, 1, "d", NULL);
        xparray_put(array, 2, "a", NULL);
        xparray_put(array, 3, "f", NULL);
        xparray_put(array, 4, "h", NULL);
        xparray_put(array, 5, "b", NULL);
        xparray_put(array, 6, "c", NULL);
        xparray_put(array, 7, "e", NULL);
        xparray_put(array, 8, "i", NULL);
        xparray_put(array, 9, "j", NULL);

        xassert(strcmp(xparray_min(array, test_xparray_cmp, NULL), "a") == 0);
        xassert(strcmp(xparray_max(array, test_xparray_cmp, NULL), "j") == 0);

        xparray_free(&array);
    }

    /* xparray_binary_search */
    {
        XPArray_PT array = xparray_new(10);
        xparray_put(array, 0, "a", NULL);
        xparray_put(array, 1, "b", NULL);
        xparray_put(array, 2, "c", NULL);
        xparray_put(array, 3, "d", NULL);
        xparray_put(array, 4, "e", NULL);
        xparray_put(array, 5, "f", NULL);
        xparray_put(array, 6, "g", NULL);
        xparray_put(array, 7, "h", NULL);
        xparray_put(array, 8, "i", NULL);
        xparray_put(array, 9, "j", NULL);

        xassert(xparray_binary_search(array, "c", test_xparray_cmp, NULL) == 2);
        xassert(xparray_binary_search(array, "h", test_xparray_cmp, NULL) == 7);

        xparray_free(&array);
    }

    /* xparray_fill */
    /* xparray_scope_fill */
    {
        /* array == NULL */
        {
            bool except = false;
            XEXCEPT_TRY
                xparray_fill(NULL, "abc");
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* array->size == 0 */
        {
            XPArray_PT array = xparray_new(0);
            bool except = false;
            XEXCEPT_TRY
                xparray_fill(array, "abc");
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xparray_free(&array);
        }

        /* normal */
        {
            XPArray_PT array = xparray_new(3);

            xassert(xparray_fill(array, "abc"));
            xassert(strcmp((char*)xparray_get(array, 0), "abc") == 0);
            xassert(strcmp((char*)xparray_get(array, 1), "abc") == 0);
            xassert(strcmp((char*)xparray_get(array, 2), "abc") == 0);

            xassert(xparray_fill(array, NULL));
            xassert_false(xparray_get(array, 0));
            xassert_false(xparray_get(array, 1));
            xassert_false(xparray_get(array, 2));

            xparray_free(&array);
        }
    }

    /* xparray_front */
    /* xparray_back */

    /* xparray_map */
    {
        /* array == NULL */
        {
            bool except = false;
            XEXCEPT_TRY
                xparray_map(NULL, apply_true, "abc");
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* apply == NULL */
        {
            XPArray_PT array = xparray_new(10);
            bool except = false;
            XEXCEPT_TRY
                xparray_map(array, NULL, "abc");
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xparray_free(&array);
        }

        /* normal */
        {
            XPArray_PT array = xparray_new(10);
            xparray_fill(array, "abc");
            xassert(xparray_map(array, apply_true, "xyz") == 10);
            xassert(xparray_map(array, apply_false, "abc") == 0);

            xparray_put(array, 5, "def", NULL);
            xassert(xparray_map(array, apply_compare, "abc") == 9);
            xassert(xparray_map(array, apply_compare, "def") == 1);

            xparray_free(&array);
        }
    }

    /* xparray_map_break_if_true */
    {
        XPArray_PT array = xparray_new(10);
        xparray_fill(array, "abc");
        xparray_put(array, 5, "def", NULL);

        xassert(xparray_map_break_if_true(array, apply_compare, "def"));
        xassert_false(xparray_map_break_if_true(array, apply_compare, "xyz"));

        xparray_free(&array);
    }

    /* xparray_map_break_if_false */
    {
        XPArray_PT array = xparray_new(10);

        xparray_fill(array, "abc");
        xassert(xparray_map_break_if_false(array, apply_compare, "def"));
        xassert_false(xparray_map_break_if_false(array, apply_compare, "abc"));

        xparray_put(array, 5, "def", NULL);
        xassert(xparray_map_break_if_false(array, apply_compare, "abc"));

        xparray_free(&array);
    }

    /* xparray_free */
    /* xparray_deep_free */
    {
        /* array == NULL */
        {
            XPArray_PT array = NULL;
            xparray_free(NULL);
            xparray_free(&array);
            xassert(array == NULL);
        }

        /* array->size == 0 */
        {
            XPArray_PT array = xparray_new(0);
            xparray_free(&array);
            xassert(array == NULL);
        }

        {
            XPArray_PT array = xparray_new(10);
            xparray_free(&array);
            xassert(array == NULL);
        }

        {
            XPArray_PT array = xparray_random_string(10);
            xparray_deep_free(&array);
            xassert(array == NULL);
        }
    }

    /* xparray_clear */
    /* xparray_deep_clear */
    {
        /* array == NULL */
        {
            XPArray_PT array = NULL;
            xparray_clear(NULL);
            xparray_clear(array);
        }

        /* array->size == 0 */
        {
            XPArray_PT array = xparray_new(0);
            xparray_clear(array);
            xassert(array->size == 0);
            xassert(array->datas == NULL);
            xparray_free(&array);
        }

        /* normal */
        {
            XPArray_PT array = xparray_new(10);
            xparray_fill(array, "abc");
            xassert(strcmp((char*)xparray_get(array, 2), "abc") == 0);

            xparray_clear(array);
            xassert_false(xparray_get(array, 2));

            xassert(array->size == 10);
            xassert(array->datas);

            xparray_free(&array);
        }

        {
            XPArray_PT array = xparray_random_string(10);
            xparray_deep_clear(array);
            xassert(array->size == 10);

            for (int i = 0; i < 10; i++) {
                xassert_false(xparray_get(array, i));
            }

            xparray_free(&array);
        }
    }

    /* xparray_remove */
    /* xparray_deep_remove */
    /* xparray_scope_remove */
    /* xparray_scope_deep_remove */
    {
        /* array == NULL */
        {
            bool except = false;
            XEXCEPT_TRY
                xparray_remove(NULL, 0, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }

        /* i < 0 */
        {
            XPArray_PT array = xparray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xparray_remove(array, -1, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xparray_free(&array);
        }

        /* array->size < i */
        {
            XPArray_PT array = xparray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xparray_remove(array, 20, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
            xparray_free(&array);
        }

        /* normal */
        {
            XPArray_PT array = xparray_new(10);
            xparray_fill(array, "abc");

            xassert(strcmp((char*)xparray_get(array, 3), "abc") == 0);
            xassert(xparray_remove(array, 3, NULL));
            xassert_false(xparray_get(array, 3));

            void *memory = NULL;
            xassert(xparray_remove(array, 5, &memory));
            xassert(strcmp(memory, "abc") == 0);

            xassert(array->size == 10);

            xparray_free(&array);
        }

        {
            XPArray_PT array = xparray_random_string(10);
            xparray_deep_remove(array, 0);
            xparray_scope_deep_remove(array, 1, 9);
            xparray_free(&array);
        }
    }

    /* xparray_size */
    {
        {
            xassert(0 == xparray_size(NULL));
        }

        {
            XPArray_PT array = xparray_new(10);
            xparray_fill(array, "abc");
            xassert(xparray_size(array) == 10);
            xparray_free(&array);
        }

        {
            XPArray_PT array = xparray_new(0);
            xassert(xparray_size(array) == 0);
            xparray_free(&array);
        }
    }

    /* xparray_is_empty */
    {
        {
            XPArray_PT array = xparray_new(10);
            xassert_false(xparray_is_empty(array));
            xparray_free(&array);
        }

        {
            XPArray_PT array = xparray_new(0);
            xassert(xparray_is_empty(array));
            xparray_free(&array);
        }
    }

    /* xparray_resize */
    {
        {
            bool except = false;
            XEXCEPT_TRY
                xparray_resize(NULL, 1);
            XEXCEPT_ELSE
                except = true;
XEXCEPT_END_TRY

xassert(except);
        }

        {
        XPArray_PT array = xparray_new(10);

        bool except = false;
        XEXCEPT_TRY
            xparray_resize(array, -1);
        XEXCEPT_ELSE
            except = true;
        XEXCEPT_END_TRY

            xassert(except);
        xparray_free(&array);
        }

        {
            XPArray_PT array = xparray_new(10);
            xassert(xparray_resize(array, 0));

            xassert(array->size == 0);
            xassert(array->datas == NULL);

            xparray_free(&array);
        }

        {
            XPArray_PT array = xparray_new(0);
            xassert(xparray_resize(array, 0));

            xassert(array->size == 0);
            xassert(array->datas == NULL);

            xparray_free(&array);
        }

        {
            XPArray_PT array = xparray_new(0);
            xassert(xparray_resize(array, 5));

            xassert(array->size == 5);
            xassert(array->datas);
            for (int i = 0; i < 5; i++) {
                xassert_false(xparray_get(array, i));
            }
            xparray_free(&array);
        }

        {
            XPArray_PT array = xparray_new(10);
            xparray_put(array, 3, "abc", NULL);
            xparray_put(array, 8, "def", NULL);

            xassert(xparray_resize(array, 10));
            xassert(strcmp((char*)xparray_get(array, 3), "abc") == 0);
            xassert(strcmp((char*)xparray_get(array, 8), "def") == 0);

            xassert(xparray_resize(array, 20));
            xassert(strcmp((char*)xparray_get(array, 3), "abc") == 0);
            xassert_false(xparray_get(array, 5));
            xassert(strcmp((char*)xparray_get(array, 8), "def") == 0);
            for (int i = 10; i < 20; i++) {
                xassert_false(xparray_get(array, i));
            }
            xassert(array->size == 20);
            xassert(array->datas);

            xassert(xparray_resize(array, 5));
            xassert_false(xparray_get(array, 1));
            xassert(strcmp((char*)xparray_get(array, 3), "abc") == 0);
            xassert(array->size == 5);
            xassert(array->datas);

            xparray_free(&array);
        }

        {
            XPArray_PT array = xparray_random_string(10);
            xassert(xparray_deep_resize(array, 5));
            xassert(array->size == 5);
            xparray_deep_free(&array);
        }

        {
            XPArray_PT array = xparray_random_string(10);
            xassert(xparray_deep_resize(array, 20));
            for (int i = 0; i < 10; i++) {
                xassert(xparray_get(array, i));
            }
            for (int i = 10; i < 20; i++) {
                xassert_false(xparray_get(array, i));
            }
            xassert(array->size == 20);
            xparray_deep_free(&array);
        }
    }

    /* xparray_remove_resize */
    /* xparray_deep_remove_resize */
    {
        {
            XPArray_PT array = xparray_random_string(10);
            xparray_deep_remove_resize(array, 9);
            xassert(array->size == 9);
            xparray_deep_free(&array);
        }

        {
            XPArray_PT array = xparray_random_string(10);
            xparray_deep_remove_resize(array, 5);
            xassert(xparray_get(array, 5));
            xassert(array->size == 9);
            xparray_deep_free(&array);
        }

        {
            XPArray_PT array = xparray_new(7);
            xparray_put(array, 0, "a0", NULL);
            xparray_put(array, 1, "a1", NULL);
            xparray_put(array, 2, "a2", NULL);
            xparray_put(array, 3, "a3", NULL);
            xparray_put(array, 4, "a4", NULL);
            xparray_put(array, 5, "a5", NULL);
            xparray_put(array, 6, "a6", NULL);

            xparray_remove_resize(array, 6, NULL);
            xassert(array->size == 6);
            xassert(strcmp((char*)xparray_get(array, 0), "a0") == 0);
            xassert(strcmp((char*)xparray_get(array, 1), "a1") == 0);
            xassert(strcmp((char*)xparray_get(array, 2), "a2") == 0);
            xassert(strcmp((char*)xparray_get(array, 3), "a3") == 0);
            xassert(strcmp((char*)xparray_get(array, 4), "a4") == 0);
            xassert(strcmp((char*)xparray_get(array, 5), "a5") == 0);

            xparray_remove_resize(array, 2, NULL);
            xassert(array->size == 5);
            xassert(strcmp((char*)xparray_get(array, 0), "a0") == 0);
            xassert(strcmp((char*)xparray_get(array, 1), "a1") == 0);
            xassert(strcmp((char*)xparray_get(array, 2), "a3") == 0);
            xassert(strcmp((char*)xparray_get(array, 3), "a4") == 0);
            xassert(strcmp((char*)xparray_get(array, 4), "a5") == 0);

            xparray_remove_resize(array, 4, NULL);
            xassert(array->size == 4);
            xassert(strcmp((char*)xparray_get(array, 0), "a0") == 0);
            xassert(strcmp((char*)xparray_get(array, 1), "a1") == 0);
            xassert(strcmp((char*)xparray_get(array, 2), "a3") == 0);
            xassert(strcmp((char*)xparray_get(array, 3), "a4") == 0);

            xparray_remove_resize(array, 0, NULL);
            xassert(array->size == 3);
            xassert(strcmp((char*)xparray_get(array, 0), "a1") == 0);
            xassert(strcmp((char*)xparray_get(array, 1), "a3") == 0);
            xassert(strcmp((char*)xparray_get(array, 2), "a4") == 0);

            xparray_free(&array);
        }
    }

    /* xparray_swap */
    {
        {
            XPArray_PT array = xparray_new(10);
            bool except = false;
            XEXCEPT_TRY
                xparray_swap(NULL, array);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xparray_free(&array);
        }

        {
            XPArray_PT array = xparray_new(10);
            bool except = false;
            XEXCEPT_TRY
                xparray_swap(array, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xparray_free(&array);
        }

        {
            bool except = false;
            XEXCEPT_TRY
                xparray_swap(NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        {
            XPArray_PT array1 = xparray_new(10);
            XPArray_PT array2 = xparray_new(15);

            xparray_put(array1, 5, "abc", NULL);
            xparray_put(array2, 5, "def", NULL);

            xassert(xparray_swap(array1, array2));

            xassert(strcmp((char*)xparray_get(array1, 5), "def") == 0);
            xassert(strcmp((char*)xparray_get(array2, 5), "abc") == 0);

            xassert(array1->size == 15);
            xassert(array2->size == 10);

            xparray_free(&array1);
            xparray_free(&array2);
        }
    }

    /* xparray_exch */
    {
        {
            bool except = false;
            XEXCEPT_TRY
                xparray_exch(NULL, 0, 1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        {
            XPArray_PT array = xparray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xparray_exch(array, -1, 1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xparray_free(&array);
        }

        {
            XPArray_PT array = xparray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xparray_exch(array, 20, 1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xparray_free(&array);
        }

        {
            XPArray_PT array = xparray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xparray_exch(array, 0, -1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xparray_free(&array);
        }

        {
            XPArray_PT array = xparray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xparray_exch(array, 0, 20);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xparray_free(&array);
        }

        {
            XPArray_PT array = xparray_new(10);
            xparray_put(array, 3, "abc", NULL);
            xparray_put(array, 5, "def", NULL);

            xassert(xparray_exch(array, 5, 5));
            xassert(strcmp((char*)xparray_get(array, 5), "def") == 0);

            xassert(xparray_exch(array, 3, 5));
            xassert(strcmp((char*)xparray_get(array, 3), "def") == 0);
            xassert(strcmp((char*)xparray_get(array, 5), "abc") == 0);

            xparray_free(&array);
        }
    }

    /* xparray_bubble_sort */
    {
        {
            XPArray_PT array = xparray_random_string(100);
            xassert(xparray_bubble_sort(array, test_xparray_cmp, NULL));
            xparray_deep_free(&array);
        }

        {
            for (int i = 0; i < 50; ++i) {
                XPArray_PT array = xparray_random_string(i);
                xassert(xparray_bubble_sort(array, test_xparray_cmp, NULL));
                xparray_deep_free(&array);
            }
        }
    }

    /* xparray_select_sort */
    {
        {
            XPArray_PT array = xparray_random_string(100);
            xassert(xparray_select_sort(array, test_xparray_cmp, NULL));
            xparray_deep_free(&array);
        }

        {
            for (int i = 0; i < 50; ++i) {
                XPArray_PT array = xparray_random_string(i);
                xassert(xparray_select_sort(array, test_xparray_cmp, NULL));
                xparray_deep_free(&array);
            }
        }
    }

    /* xparray_insert_sort */
    {
        {
            XPArray_PT array = xparray_random_string(100);
            xassert(xparray_insert_sort(array, test_xparray_cmp, NULL));
            xparray_deep_free(&array);
        }

        {
            for (int i = 0; i < 50; ++i) {
                XPArray_PT array = xparray_random_string(i);
                xassert(xparray_insert_sort(array, test_xparray_cmp, NULL));
                xparray_deep_free(&array);
            }
        }
    }

    /* xparray_shell_sort */
    {
        {
            XPArray_PT array = xparray_random_string(1000);
            xassert(xparray_shell_sort(array, test_xparray_cmp, NULL));
            xparray_deep_free(&array);
        }

        {
            for (int i = 0; i < 50; ++i) {
                XPArray_PT array = xparray_random_string(i);
                xassert(xparray_shell_sort(array, test_xparray_cmp, NULL));
                xparray_deep_free(&array);
            }
        }
    }

    /* xparray_merge_sort */
    {
        {
            XPArray_PT array = xparray_random_string(1000);
            XPArray_PT tarray = xparray_new(array->size);

            xparray_merge_sort_impl_up_bottom(array, tarray, 0, array->size - 1, test_xparray_cmp, NULL);
            xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));

            xparray_free(&tarray);
            xparray_deep_free(&array);
        }

        {
            for (int i = 0; i < 50; ++i) {
                XPArray_PT array = xparray_random_string(i);
                XPArray_PT tarray = xparray_new(array->size);

                xparray_merge_sort_impl_up_bottom(array, tarray, 0, array->size - 1, test_xparray_cmp, NULL);
                xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));

                xparray_free(&tarray);
                xparray_deep_free(&array);
            }
        }

        {
            XPArray_PT array = xparray_random_string(1000);
            XPArray_PT tarray = xparray_new(array->size);

            xparray_merge_sort_impl_bottom_up(array, tarray, 0, array->size - 1, test_xparray_cmp, NULL);
            xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));

            xparray_free(&tarray);
            xparray_deep_free(&array);
        }

        {
            for (int i = 0; i < 50; ++i) {
                XPArray_PT array = xparray_random_string(i);
                XPArray_PT tarray = xparray_new(array->size);

                xparray_merge_sort_impl_bottom_up(array, tarray, 0, array->size - 1, test_xparray_cmp, NULL);
                xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));

                xparray_free(&tarray);
                xparray_deep_free(&array);
            }
        }

        {
            XPArray_PT array = xparray_random_string(1000);
            XPArray_PT tarray = xparray_new(array->size);
            memcpy(tarray->datas, array->datas, (array->size * sizeof(void*)));

            xparray_merge_sort_impl_no_copy(array, tarray, 0, array->size - 1, test_xparray_cmp, NULL);
            xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));

            xparray_free(&tarray);
            xparray_deep_free(&array);
        }

        {
            for (int i = 0; i < 50; ++i) {
                XPArray_PT array = xparray_random_string(i);
                XPArray_PT tarray = xparray_new(array->size);
                memcpy(tarray->datas, array->datas, (array->size * sizeof(void*)));

                xparray_merge_sort_impl_no_copy(array, tarray, 0, array->size - 1, test_xparray_cmp, NULL);
                xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));

                xparray_free(&tarray);
                xparray_deep_free(&array);
            }
        }
    }

    /* xparray_quick_sort */
    {
        {
            XPArray_PT array = xparray_random_string(1000);
            xparray_quick_sort_impl_basic_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
            xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
            xparray_quick_sort_impl_basic_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
            xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
            xparray_quick_sort_impl_basic_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
            xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
            xparray_deep_free(&array);
        }

        {
            for (int i = 2; i < 50; ++i) {
                XPArray_PT array = xparray_random_string(i);
                xparray_quick_sort_impl_basic_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
                xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
                xparray_quick_sort_impl_basic_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
                xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
                xparray_deep_free(&array);
            }
        }

        {
            XPArray_PT array = xparray_random_string(1000);
            xparray_quick_sort_impl_random_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
            xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
            xparray_quick_sort_impl_random_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
            xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
            xparray_quick_sort_impl_random_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
            xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
            xparray_deep_free(&array);
        }

        {
            for (int i = 2; i < 50; ++i) {
                XPArray_PT array = xparray_random_string(i);
                xparray_quick_sort_impl_random_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
                xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
                xparray_quick_sort_impl_random_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
                xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
                xparray_deep_free(&array);
            }
        }


        {
            XPArray_PT array = xparray_random_string(1000);
            xparray_quick_sort_impl_median_of_three_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
            xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
            xparray_quick_sort_impl_median_of_three_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
            xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
            xparray_quick_sort_impl_median_of_three_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
            xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
            xparray_deep_free(&array);
        }

        {
            for (int i = 2; i < 50; ++i) {
                XPArray_PT array = xparray_random_string(i);
                xparray_quick_sort_impl_median_of_three_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
                xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
                xparray_quick_sort_impl_median_of_three_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
                xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
                xparray_deep_free(&array);
            }
        }

        {
            XPArray_PT array = xparray_random_string(1000);
            xparray_quick_sort_impl_3_way_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
            xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
            xparray_quick_sort_impl_3_way_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
            xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
            xparray_quick_sort_impl_3_way_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
            xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
            xparray_deep_free(&array);
        }

        {
            for (int i = 2; i < 50; ++i) {
                XPArray_PT array = xparray_random_string(i);
                xparray_quick_sort_impl_3_way_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
                xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
                xparray_quick_sort_impl_3_way_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
                xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
                xparray_deep_free(&array);
            }
        }

        {
            XPArray_PT array = xparray_random_string(1000);
            xparray_quick_sort_impl_quick_3_way_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
            xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
            xparray_quick_sort_impl_quick_3_way_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
            xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
            xparray_quick_sort_impl_quick_3_way_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
            xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
            xparray_deep_free(&array);
        }

        {
            for (int i = 2; i < 50; ++i) {
                XPArray_PT array = xparray_random_string(i);
                xparray_quick_sort_impl_quick_3_way_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
                xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
                xparray_quick_sort_impl_quick_3_way_split(array, 0, array->size - 1, parray_lg_test(array->size - 1) * 2, test_xparray_cmp, NULL);
                xassert(xparray_is_sorted(array, test_xparray_cmp, NULL));
                xparray_deep_free(&array);
            }
        }
    }

    /* xparray_heapify_impl */
    {
        {
            XPArray_PT array = xparray_random_string(1000);
            xassert(xparray_heapify_impl(array, 0, 999, true, test_xparray_cmp, NULL));
            xparray_deep_free(&array);
        }

        {
            for (int i = 100; i < 500; ++i) {
                XPArray_PT array = xparray_random_string(i);
                xassert(xparray_heapify_impl(array, 0, i / 3, true, test_xparray_cmp, NULL));
                xassert(xparray_heapify_impl(array, i / 3, i / 3 * 2, true, test_xparray_cmp, NULL));
                xassert(xparray_heapify_impl(array, i / 3 * 2, array->size - 1, true, test_xparray_cmp, NULL));
                xparray_deep_free(&array);
            }
        }

        {
            XPArray_PT array = xparray_random_string(1000);
            xassert(xparray_heapify_impl(array, 0, 999, false, test_xparray_cmp, NULL));
            xparray_deep_free(&array);
        }

        {
            for (int i = 100; i < 200; ++i) {
                XPArray_PT array = xparray_random_string(i);
                xassert(xparray_heapify_impl(array, 0, i / 3, false, test_xparray_cmp, NULL));
                xassert(xparray_heapify_impl(array, i / 3, i / 3 * 2, false, test_xparray_cmp, NULL));
                xassert(xparray_heapify_impl(array, i / 3 * 2, array->size - 1, false, test_xparray_cmp, NULL));
                xparray_deep_free(&array);
            }
        }
    }

    /* xparray_heapify_min */
    {
        {
            XPArray_PT array = xparray_random_string(1000);
            xassert(xparray_heapify_min(array, test_xparray_cmp, NULL));
            xparray_deep_free(&array);
        }

        {
            for (int i = 0; i < 50; ++i) {
                XPArray_PT array = xparray_random_string(i);
                xassert(xparray_heapify_min(array, test_xparray_cmp, NULL));
                xparray_deep_free(&array);
            }
        }
    }

    /* xparray_heapify_max */
    {
        {
            XPArray_PT array = xparray_random_string(1000);
            xassert(xparray_heapify_max(array, test_xparray_cmp, NULL));
            xparray_deep_free(&array);
        }

        {
            for (int i = 0; i < 50; ++i) {
                XPArray_PT array = xparray_random_string(i);
                xassert(xparray_heapify_max(array, test_xparray_cmp, NULL));
                xparray_deep_free(&array);
            }
        }
    }

    /* xparray_heap_sort_impl */
    {
        for (int i = 100; i < 200; ++i) {
            XPArray_PT array = xparray_random_string(i);
            xassert(xparray_heap_sort_impl(array, 0, i / 3, true, test_xparray_cmp, NULL));
            xassert(xparray_heap_sort_impl(array, i / 3, i / 3 * 2, true, test_xparray_cmp, NULL));
            xassert(xparray_heap_sort_impl(array, i / 3 * 2, array->size - 1, true, test_xparray_cmp, NULL));
            xparray_deep_free(&array);
        }

        for (int i = 100; i < 200; ++i) {
            XPArray_PT array = xparray_random_string(i);
            xassert(xparray_heap_sort_impl(array, 0, i / 3, false, test_xparray_cmp, NULL));
            xassert(xparray_heap_sort_impl(array, i / 3, i / 3 * 2, false, test_xparray_cmp, NULL));
            xassert(xparray_heap_sort_impl(array, i / 3 * 2, array->size - 1, false, test_xparray_cmp, NULL));
            xparray_deep_free(&array);
        }
    }

    /* xparray_heap_sort */
    {
        {
            XPArray_PT array = xparray_random_string(1000);
            xassert(xparray_heap_sort(array, test_xparray_cmp, NULL));
            xparray_deep_free(&array);
        }

        {
            for (int i = 0; i < 200; ++i) {
                XPArray_PT array = xparray_random_string(i);
                xassert(xparray_heap_sort(array, test_xparray_cmp, NULL));
                xparray_deep_free(&array);
            }
        }
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
