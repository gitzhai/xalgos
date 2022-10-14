
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../array/xarray_x.h"
#include "../include/xalgos.h"

static 
bool apply_true(void *x, int elem_size, void *cl) {
    return true;
}

static
bool apply_false(void *x, int elem_size, void *cl) {
    return false;
}

static
bool apply_compare(void *x, int elem_size, void *cl) {    
    return strcmp((char*)x, (char*)cl) == 0;
}

static
bool apply_print(void *x, int elem_size, void *cl) {
    printf("%s\n", (char*)x);
    return true;
}

static
bool apply_int_print(int x, void *cl) {
    printf("%d : %s \n", x, (char*)xarray_get((XArray_PT)cl, x));
    return true;
}

static 
int sort_compare(void *x, void *y, int elem_size, void *cl) {
    return strcmp((char*)x, (char*)y);
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static 
XArray_PT xarray_random_string(int size) {
    XArray_PT array = xarray_new(size, 5);
    if (!array) {
        return NULL;
    }

    {
        const char charsets[] = "0123456789";
        const int str_size = 5;
        char str[5];

        for (int i = 0; i < array->size; i++) {
            for (int j = 0; j < str_size - 1; ++j) {
                str[j] = charsets[rand() % (sizeof(charsets) - 1)];
            }
            str[str_size - 1] = '\0';

            xarray_put(array, i, str, NULL);
        }
    }

    return array;
}

void test_xarray() {
    /* xarray_new */
    {
        {
            bool except = false;

            XEXCEPT_TRY
                XArray_PT array = xarray_new(1, 0);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        {
            bool except = false;

            XEXCEPT_TRY
                XArray_PT array = xarray_new(0, -1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        {
            XArray_PT array = xarray_new(0, 1);
            xassert(array->size == 0);
            xassert(array->elem_size == 1);
            xassert(array->datas == NULL);
            xarray_free(&array);
        }

        {
            XArray_PT array = xarray_new(10, 3);
            xassert(array->size == 10);
            xassert(array->elem_size == 3);
            xassert(array->datas);
            xarray_free(&array);
        }
    }

        /* xarray_copy */
        {
            XArray_PT array = xarray_new(10, 4);
            XArray_PT narray = xarray_copy(array);

            xassert(narray->size == 10);
            xassert(narray->elem_size == 4);
            xassert(narray->datas);

            xarray_free(&array);
            xarray_free(&narray);
        }

        /* xarray_copyn */
        {
            {
                bool except = false;
                XEXCEPT_TRY
                    xarray_copyn(NULL, 1);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }

            {
                XArray_PT array = xarray_new(10, 4);

                bool except = false;
                XEXCEPT_TRY
                    xarray_copyn(array, -1);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(10, 4);
                XArray_PT narray = xarray_copyn(array, 0);

                xassert(narray->size == 10);
                xassert(narray->elem_size == 4);
                xassert(narray->datas);

                xarray_free(&array);
                xarray_free(&narray);
            }

            {
                XArray_PT array = xarray_new(10, 4);
                XArray_PT narray = xarray_copyn(array, 5);

                xassert(narray->size == 10);
                xassert(narray->elem_size == 4);
                xassert(narray->datas);

                xarray_free(&array);
                xarray_free(&narray);
            }
        }

        /* xarray_vload */
        {
            {
                bool except = false;
                XEXCEPT_TRY
                    xarray_vload(NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }

            {
                XArray_PT array = xarray_new(10, 4);

                xassert(xarray_vload(array, NULL) == 0);

                xassert(xarray_vload(array, "abc", "def", "xyz", NULL) == 3);

                xassert(strcmp((char*)xarray_get(array, 2), "xyz") == 0);
                xassert(strcmp((char*)xarray_get(array, 3), "") == 0);

                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(10, 4);

                xassert(xarray_vload(array, "abc", NULL, "xyz", NULL) == 1);

                xassert(strcmp((char*)xarray_get(array, 0), "abc") == 0);
                xassert(strcmp((char*)xarray_get(array, 1), "") == 0);
                xassert(strcmp((char*)xarray_get(array, 2), "") == 0);

                xarray_free(&array);
            }
        }

        /* xarray_aload */
        {
            {
                bool except = false;
                XEXCEPT_TRY
                    xarray_aload(NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }

            {
                XArray_PT array = xarray_new(10, 4);
                XPArray_PT parray = xparray_new(3);

                xassert(xarray_aload(array, NULL) == 0);

                xparray_put(parray, 0, "abc", NULL);
                xparray_put(parray, 1, "", NULL);
                xparray_put(parray, 2, "xyz", NULL);

                xassert(xarray_aload(array, parray) == 3);

                xassert(strcmp((char*)xarray_get(array, 0), "abc") == 0);
                xassert(strcmp((char*)xarray_get(array, 1), "") == 0);
                xassert(strcmp((char*)xarray_get(array, 2), "xyz") == 0);

                xarray_free(&array);
                xparray_free(&parray);
            }
        }

        /* xarray_put */
        {
            {
                bool except = false;
                XEXCEPT_TRY
                    xarray_put(NULL, 0, "abc", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }

            {
                XArray_PT array = xarray_new(10, 4);

                bool except = false;
                XEXCEPT_TRY
                    xarray_put(array, -1, "abc", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(10, 4);

                bool except = false;
                XEXCEPT_TRY
                    xarray_put(array, 20, "abc", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(10, 4);

                xassert(xarray_put(array, 0, "abc", NULL));
                xassert(xarray_put(array, 1, "def", NULL));
                xassert(xarray_put(array, 2, "xyz", NULL));

                xassert(strcmp((char*)xarray_get(array, 0), "abc") == 0);
                xassert(strcmp((char*)xarray_get(array, 1), "def") == 0);
                xassert(strcmp((char*)xarray_get(array, 2), "xyz") == 0);

                xassert(xarray_put(array, 1, NULL, NULL));
                xassert(strcmp((char*)xarray_get(array, 1), "") == 0);

                char memory[4] = { 0 };
                xassert(xarray_put(array, 2, "xyz2", memory));
                xassert(strcmp((char*)xarray_get(array, 2), "xyz2") == 0);
                xassert(strcmp(memory, "xyz") == 0);

                xarray_free(&array);
            }
        }

        /* xarray_put_expand */
        {
            {
                bool except = false;
                XEXCEPT_TRY
                    xarray_put_expand(NULL, 0, "abc", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }

            {
                XArray_PT array = xarray_new(10, 4);

                bool except = false;
                XEXCEPT_TRY
                    xarray_put_expand(array, -1, "abc", NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(10, 4);

                xassert(array->size == 10);

                xassert(xarray_put_expand(array, 20, "def", NULL));

                xassert(10 < array->size);

                xassert(strcmp((char*)xarray_get(array, 20), "def") == 0);
                xassert(strcmp((char*)xarray_get(array, 15), "") == 0);

                xarray_free(&array);
            }
        }

        /* xarray_get */
        {
            {
                bool except = false;
                XEXCEPT_TRY
                    xarray_get(NULL, 0);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }

            {
                XArray_PT array = xarray_new(10, 4);

                bool except = false;
                XEXCEPT_TRY
                    xarray_get(array, -1);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(10, 4);

                bool except = false;
                XEXCEPT_TRY
                    xarray_get(array, 20);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(10, 4);

                xassert(xarray_put(array, 0, "abc", NULL));
                xassert(xarray_put(array, 1, "def", NULL));
                xassert(xarray_put(array, 2, "xyz", NULL));

                xassert(strcmp((char*)xarray_get(array, 1), "def") == 0);
                xassert(strcmp((char*)xarray_get(array, 3), "") == 0);

                xarray_free(&array);
            }
        }

        /* xarray_fill */
        {
            xassert_false(xarray_fill(NULL, "abc"));

            {
                XArray_PT array = xarray_new(0, 4);
                xassert_false(xarray_fill(array, "abc"));
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(3, 4);

                xassert(xarray_fill(array, "abc"));
                xassert(strcmp((char*)xarray_get(array, 0), "abc") == 0);
                xassert(strcmp((char*)xarray_get(array, 1), "abc") == 0);
                xassert(strcmp((char*)xarray_get(array, 2), "abc") == 0);

                xassert(xarray_fill(array, NULL));
                xassert(strcmp((char*)xarray_get(array, 0), "") == 0);
                xassert(strcmp((char*)xarray_get(array, 1), "") == 0);
                xassert(strcmp((char*)xarray_get(array, 2), "") == 0);

                xarray_free(&array);
            }
        }

        /* xarray_front */
        {
        }

        /* xarray_back */
        {
            XArray_PT array = xarray_new(0, 4);
            xassert(xarray_back(array) == NULL);
            xarray_free(&array);
        }

        /* xarray_map */
        {
            xassert_false(xarray_map(NULL, apply_true, "abc"));

            {
                XArray_PT array = xarray_new(10, 4);
                bool except = false;
                XEXCEPT_TRY
                    xarray_map(array, NULL, "abc");
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(10, 4);
                xarray_fill(array, "abc");
                xassert(xarray_map(array, apply_true, "xyz") == 10);
                xassert(xarray_map(array, apply_false, "abc") == 0);

                xarray_put(array, 5, "def", NULL);
                xassert(xarray_map(array, apply_compare, "abc") == 9);
                xassert(xarray_map(array, apply_compare, "def") == 1);

                xarray_free(&array);
            }
        }

        /* xarray_map_break_if_true */
        {
            XArray_PT array = xarray_new(10, 4);
            xarray_fill(array, "abc");
            xarray_put(array, 5, "def", NULL);

            xassert(xarray_map_break_if_true(array, apply_compare, "def"));
            xassert_false(xarray_map_break_if_true(array, apply_compare, "xyz"));

            xarray_free(&array);
        }

        /* xarray_map_break_if_false */
        {
            XArray_PT array = xarray_new(10, 4);

            xarray_fill(array, "abc");
            xassert(xarray_map_break_if_false(array, apply_compare, "def"));
            xassert_false(xarray_map_break_if_false(array, apply_compare, "abc"));

            xarray_put(array, 5, "def", NULL);
            xassert(xarray_map_break_if_false(array, apply_compare, "abc"));

            xarray_free(&array);
        }

        /* xarray_free */
        {
            {
                XArray_PT array = NULL;
                xarray_free(NULL);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(10, 4);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(0, 4);
                xarray_free(&array);
            }
        }

        /* xarray_clear */
        {
            {
                bool except = false;
                XEXCEPT_TRY
                    xarray_clear(NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }

            {
                XArray_PT array = xarray_new(10, 4);
                xarray_fill(array, "abc");
                xassert(strcmp((char*)xarray_get(array, 2), "abc") == 0);

                xarray_clear(array);
                xassert(strcmp((char*)xarray_get(array, 2), "") == 0);

                xassert(array->size == 10);
                xassert(array->datas);

                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(0, 4);
                xarray_clear(array);
                xassert(array->size == 0);
                xassert(array->datas == NULL);
                xassert(array->elem_size == 4);
                xarray_free(&array);
            }
        }

        /* xarray_clear_elem */
        {
            {
                bool except = false;
                XEXCEPT_TRY
                    xarray_remove(NULL, 0, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }

            {
                XArray_PT array = xarray_new(10, 4);

                bool except = false;
                XEXCEPT_TRY
                    xarray_remove(array, -1, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(10, 4);

                bool except = false;
                XEXCEPT_TRY
                    xarray_remove(array, 20, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
                xarray_free(&array);
            }

            {
                char memory[4] = { 0 };
                XArray_PT array = xarray_new(10, 4);
                xarray_fill(array, "abc");

                xassert(strcmp((char*)xarray_get(array, 3), "abc") == 0);
                xassert(xarray_remove(array, 3, NULL));
                xassert(strcmp((char*)xarray_get(array, 3), "") == 0);

                xassert(xarray_remove(array, 5, memory));
                xassert(strcmp(memory, "abc") == 0);

                xassert(array->size == 10);

                xarray_free(&array);
            }
        }

        /* xarray_size */
        {
            xassert_false(xarray_size(NULL));

            {
                XArray_PT array = xarray_new(10, 4);
                xarray_fill(array, "abc");
                xassert(xarray_size(array) == 10);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(0, 4);
                xassert(xarray_size(array) == 0);
                xarray_free(&array);
            }
        }

        /* xarray_elem_size */
        {
            {
                XArray_PT array = xarray_new(10, 4);
                xarray_fill(array, "abc");
                xassert(xarray_elem_size(array) == 4);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(0, 4);
                xassert(xarray_elem_size(array) == 4);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(0, 1);
                xassert(xarray_elem_size(array) == 1);
                xarray_free(&array);
            }
        }

        /* xarray_is_empty */
        {
            {
                XArray_PT array = xarray_new(10, 4);
                xassert_false(xarray_is_empty(array));
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(0, 4);
                xassert(xarray_is_empty(array));
                xarray_free(&array);
            }
        }

        /* xarray_resize */
        {
            {
                bool except = false;
                XEXCEPT_TRY
                    xarray_resize(NULL, 1);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }

            {
                XArray_PT array = xarray_new(10, 4);

                bool except = false;
                XEXCEPT_TRY
                    xarray_resize(array, -1);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(10, 4);
                xassert(xarray_resize(array, 0));

                xassert(array->size == 0);
                xassert(array->elem_size == 4);
                xassert(array->datas == NULL);

                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(0, 4);
                xassert(xarray_resize(array, 0));

                xassert(array->size == 0);
                xassert(array->elem_size == 4);
                xassert(array->datas == NULL);

                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(0, 4);
                xassert(xarray_resize(array, 5));

                xassert(array->size == 5);
                xassert(array->elem_size == 4);
                xassert(array->datas);

                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(0, 1);
                xassert(xarray_resize(array, 5));

                xassert(array->size == 5);
                xassert(array->elem_size == 1);
                xassert(array->datas);

                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(10, 4);
                xarray_put(array, 3, "abc", NULL);
                xarray_put(array, 8, "def", NULL);

                xassert(xarray_resize(array, 10));
                xassert(strcmp((char*)xarray_get(array, 8), "def") == 0);

                xassert(xarray_resize(array, 20));
                xassert(strcmp((char*)xarray_get(array, 3), "abc") == 0);
                xassert(strcmp((char*)xarray_get(array, 5), "") == 0);
                xassert(strcmp((char*)xarray_get(array, 8), "def") == 0);
                xassert(array->size == 20);
                xassert(array->elem_size == 4);
                xassert(array->datas);

                xassert(xarray_resize(array, 5));
                xassert(strcmp((char*)xarray_get(array, 1), "") == 0);
                xassert(strcmp((char*)xarray_get(array, 3), "abc") == 0);
                xassert(array->size == 5);
                xassert(array->elem_size == 4);
                xassert(array->datas);

                xarray_free(&array);
            }
        }

        /* xarray_swap */
        {
            {
                XArray_PT array = xarray_new(10, 4);
                bool except = false;
                XEXCEPT_TRY
                    xarray_swap(NULL, array);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(10, 4);
                bool except = false;
                XEXCEPT_TRY
                    xarray_swap(array, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
                xarray_free(&array);
            }

            {
                bool except = false;
                XEXCEPT_TRY
                    xarray_swap(NULL, NULL);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }

            {
                XArray_PT array1 = xarray_new(10, 4);
                XArray_PT array2 = xarray_new(15, 5);

                xarray_put(array1, 5, "abc", NULL);
                xarray_put(array2, 5, "def", NULL);

                xassert(xarray_swap(array1, array2));

                xassert(strcmp((char*)xarray_get(array1, 5), "def") == 0);
                xassert(strcmp((char*)xarray_get(array2, 5), "abc") == 0);

                xassert(array1->size == 15);
                xassert(array1->elem_size == 5);

                xassert(array2->size == 10);
                xassert(array2->elem_size == 4);

                xarray_free(&array1);
                xarray_free(&array2);
            }
        }

        /* xarray_exch */
        {
            {
                bool except = false;
                XEXCEPT_TRY
                    xarray_exch(NULL, 0, 1);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }

            {
                XArray_PT array = xarray_new(10, 4);

                bool except = false;
                XEXCEPT_TRY
                    xarray_exch(array, -1, 1);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(10, 4);

                bool except = false;
                XEXCEPT_TRY
                    xarray_exch(array, 20, 1);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(10, 4);

                bool except = false;
                XEXCEPT_TRY
                    xarray_exch(array, 0, -1);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(10, 4);

                bool except = false;
                XEXCEPT_TRY
                    xarray_exch(array, 0, 20);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
                xarray_free(&array);
            }

            {
                XArray_PT array = xarray_new(10, 4);
                xarray_put(array, 3, "abc", NULL);
                xarray_put(array, 5, "def", NULL);

                xassert(xarray_exch(array, 5, 5));
                xassert(strcmp((char*)xarray_get(array, 5), "def") == 0);

                xassert(xarray_exch(array, 3, 5));
                xassert(strcmp((char*)xarray_get(array, 3), "def") == 0);
                xassert(strcmp((char*)xarray_get(array, 5), "abc") == 0);

                xarray_free(&array);
            }
        }

        /* xarray_sorted */
        {
        }

        /* xarray_quick_sort */
        {
            XArray_PT array = xarray_random_string(100);
            xassert(xarray_quick_sort(array, sort_compare, NULL));
            xarray_free(&array);
        }

        /* xarray_pointer_sort */
        /* xarray_pointer_inplace_sort */
        {
            XArray_PT array = xarray_random_string(100);

            XPArray_PT parray = xarray_pointer_sort(array, sort_compare, NULL);

            xarray_pointer_inplace_sort(array, parray);
            xassert(xarray_is_sorted(array, sort_compare, NULL));

            xarray_free(&array);
            xparray_free(&parray);
        }

        /* xarray_index_sort */
        /* xarray_index_inplace_sort */
        {
            XArray_PT array = xarray_random_string(100);

            XIArray_PT iarray = xarray_index_sort(array, sort_compare, NULL);

            xarray_index_inplace_sort(array, iarray);

            xassert(xarray_is_sorted(array, sort_compare, NULL));

            xarray_free(&array);
            xiarray_free(&iarray);
        }

        {
            int count = 0;
            xmem_leak(check_mem_leak, &count);
            xassert(count == 0);
        }    
}


