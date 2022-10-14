
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "../array_int/xarray_int_x.h"
#include "../include/xalgos.h"

static 
bool apply_true(int x, void *cl) {
    return true;
}

static
bool apply_false(int x, void *cl) {
    return false;
}

static
bool apply_compare(int x, void *cl) {    
    return x == *(int*)cl;
}

static
bool apply_print(int x, void *cl) {
    printf("%d\n", x);
    return true;
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

static 
XIArray_PT xiarray_random(int size) {
    XIArray_PT array = xiarray_new(size);
    if (!array) {
        return NULL;
    }

    for (int i = 0; i < array->size; i++) {
        xiarray_put(array, i, rand() % 500, NULL);
    }

    return array;
}

void test_xiarray() {
    /* xiarray_new */
    {
        {
            bool except = false;

            XEXCEPT_TRY
                XIArray_PT array = xiarray_new(-1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        {
            XIArray_PT array = xiarray_new(0);
            xassert(array->size == 0);
            xassert(array->datas == NULL);
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);
            xassert(array->size == 10);
            xassert(array->datas);
            xiarray_free(&array);
        }
    }

    /* xiarray_copy */
    {
        XIArray_PT array = xiarray_new(10);
        XIArray_PT narray = xiarray_copy(array);

        xassert(narray->size == 10);
        xassert(narray->datas);

        xiarray_free(&array);
        xiarray_free(&narray);
    }

    /* xiarray_copyn */
    {
        {
            bool except = false;
            XEXCEPT_TRY
                xiarray_copyn(NULL, 1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        {
            XIArray_PT array = xiarray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xiarray_copyn(array, -1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);
            XIArray_PT narray = xiarray_copyn(array, 0);

            xassert(narray->size == 10);
            xassert(narray->datas);

            xiarray_free(&array);
            xiarray_free(&narray);
        }

        {
            XIArray_PT array = xiarray_new(10);
            XIArray_PT narray = xiarray_copyn(array, 5);

            xassert(narray->size == 10);
            xassert(narray->datas);

            xiarray_free(&array);
            xiarray_free(&narray);
        }
    }

#if 0
    /* xiarray_vload */
    {
        {
            bool except = false;
            XEXCEPT_TRY
                xiarray_vload(NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        {
            XIArray_PT array = xiarray_new(10, 4);

            xassert(xiarray_vload(array, NULL) == 0);

            xassert(xiarray_vload(array, "abc", "def", "xyz", NULL) == 3);

            xassert(strcmp((char*)xiarray_get(array, 2), "xyz") == 0);
            xassert(strcmp((char*)xiarray_get(array, 3), "") == 0);

            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10, 4);

            xassert(xiarray_vload(array, "abc", NULL, "xyz", NULL) == 1);

            xassert(strcmp((char*)xiarray_get(array, 0), "abc") == 0);
            xassert(strcmp((char*)xiarray_get(array, 1), "") == 0);
            xassert(strcmp((char*)xiarray_get(array, 2), "") == 0);

            xiarray_free(&array);
        }
    }

    /* xiarray_aload */
    {
        {
            bool except = false;
            XEXCEPT_TRY
                xiarray_aload(NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        {
            XIArray_PT array = xiarray_new(10, 4);
            XPArray_PT parray = xparray_new(3);

            xassert(xiarray_aload(array, NULL) == 0);

            xparray_put(parray, 0, "abc", NULL);
            xparray_put(parray, 1, "", NULL);
            xparray_put(parray, 2, "xyz", NULL);

            xassert(xiarray_aload(array, parray) == 3);

            xassert(strcmp((char*)xiarray_get(array, 0), "abc") == 0);
            xassert(strcmp((char*)xiarray_get(array, 1), "") == 0);
            xassert(strcmp((char*)xiarray_get(array, 2), "xyz") == 0);

            xiarray_free(&array);
            xparray_free(&parray);
        }
    }
#endif

    /* xiarray_put */
    {
        {
            bool except = false;
            XEXCEPT_TRY
                xiarray_put(NULL, 0, 1, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        {
            XIArray_PT array = xiarray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xiarray_put(array, -1, 1, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xiarray_put(array, 20, 1, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);

            xassert(xiarray_put(array, 0, 1, NULL));
            xassert(xiarray_put(array, 1, 2, NULL));
            xassert(xiarray_put(array, 2, 3, NULL));

            xassert(xiarray_get(array, 0) == 1);
            xassert(xiarray_get(array, 1) == 2);
            xassert(xiarray_get(array, 2) == 3);

            xassert(xiarray_put(array, 1, 0, NULL));
            xassert(xiarray_get(array, 1) == 0);

            int memory = 0;
            xassert(xiarray_put(array, 2, 4, &memory));
            xassert(xiarray_get(array, 2) == 4);
            xassert(memory == 3);

            xiarray_free(&array);
        }
    }

    /* xiarray_put_expand */
    {
        {
            bool except = false;
            XEXCEPT_TRY
                xiarray_put_expand(NULL, 0, 1, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        {
            XIArray_PT array = xiarray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xiarray_put_expand(array, -1, 1, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);

            xassert(array->size == 10);

            xassert(xiarray_put_expand(array, 20, 2, NULL));

            xassert(10 < array->size);

            xassert(xiarray_get(array, 20) == 2);
            xassert(xiarray_get(array, 15) == 0);

            xiarray_free(&array);
        }
    }

    /* xiarray_get */
    {
        {
            bool except = false;
            XEXCEPT_TRY
                xiarray_get(NULL, 0);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        {
            XIArray_PT array = xiarray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xiarray_get(array, -1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xiarray_get(array, 20);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);

            xassert(xiarray_put(array, 0, 1, NULL));
            xassert(xiarray_put(array, 1, 2, NULL));
            xassert(xiarray_put(array, 2, 3, NULL));

            xassert(xiarray_get(array, 1) == 2);
            xassert(xiarray_get(array, 3) == 0);

            xiarray_free(&array);
        }
    }

    /* xiarray_fill */
    {
        XIArray_PT array = xiarray_new(3);

        xassert(xiarray_fill(array, 1));
        xassert(xiarray_get(array, 0) == 1);
        xassert(xiarray_get(array, 1) == 1);
        xassert(xiarray_get(array, 2) == 1);

        xassert(xiarray_fill(array, 0));
        xassert(xiarray_get(array, 0) == 0);
        xassert(xiarray_get(array, 1) == 0);
        xassert(xiarray_get(array, 2) == 0);

        xiarray_free(&array);
    }

    /* xiarray_front */
    /* xiarray_back */

    /* xiarray_min*/
    /* xiarray_max */
    /* xiarray_min_max */
    {
        {
            XIArray_PT array = xiarray_new(10);
            xiarray_put(array, 0, 1, NULL);
            xiarray_put(array, 1, -1, NULL);
            xiarray_put(array, 2, 2, NULL);
            xiarray_put(array, 3, 1, NULL);
            xiarray_put(array, 4, 2, NULL);
            xiarray_put(array, 5, 3, NULL);
            xiarray_put(array, 6, 1, NULL);
            xiarray_put(array, 7, 5, NULL);
            xiarray_put(array, 8, 1, NULL);
            xiarray_put(array, 9, 4, NULL);

            xassert(xiarray_min(array) == -1);
            xassert(xiarray_max(array) == 5);

            {
                int min = 0, max = 0;
                xassert(xiarray_min_max(array, &min, &max) == 0);
                xassert(min == -1);
                xassert(max == 5);
            }

            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);
            xiarray_put(array, 0, 1, NULL);
            xiarray_put(array, 1, -1, NULL);
            xiarray_put(array, 2, 2, NULL);
            xiarray_put(array, 3, 1, NULL);
            xiarray_put(array, 4, -2, NULL);
            xiarray_put(array, 5, 3, NULL);
            xiarray_put(array, 6, 1, NULL);
            xiarray_put(array, 7, 5, NULL);
            xiarray_put(array, 8, 6, NULL);

            int min = 0, max = 0;
            xassert(xiarray_min_max(array, &min, &max) == 0);
            xassert(min == -2);
            xassert(max == 6);

            xiarray_free(&array);
        }
    }

    /* xiarray_map */
    {
        {
            int a = 1;
            XIArray_PT array = xiarray_new(10);
            bool except = false;
            XEXCEPT_TRY
                xiarray_map(array, NULL, &a);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xiarray_free(&array);
        }

        {
            int a = 1, b = 2, c = 3;

            XIArray_PT array = xiarray_new(10);
            xiarray_fill(array, 1);

            xassert(xiarray_map(array, apply_true, &c) == 10);
            xassert(xiarray_map(array, apply_false, &a) == 0);

            xiarray_put(array, 5, 2, NULL);
            xassert(xiarray_map(array, apply_compare, &a) == 9);
            xassert(xiarray_map(array, apply_compare, &b) == 1);

            xiarray_free(&array);
        }
    }

    /* xiarray_map_break_if_true */
    {
        int b = 2, c = 3;

        XIArray_PT array = xiarray_new(10);
        xiarray_fill(array, 1);
        xiarray_put(array, 5, 2, NULL);

        xassert(xiarray_map_break_if_true(array, apply_compare, &b));
        xassert_false(xiarray_map_break_if_true(array, apply_compare, &c));

        xiarray_free(&array);
    }

    /* xiarray_map_break_if_false */
    {
        int a = 1, b = 2;

        XIArray_PT array = xiarray_new(10);

        xiarray_fill(array, 1);
        xassert(xiarray_map_break_if_false(array, apply_compare, &b));
        xassert_false(xiarray_map_break_if_false(array, apply_compare, &a));

        xiarray_put(array, 5, 2, NULL);
        xassert(xiarray_map_break_if_false(array, apply_compare, &a));

        xiarray_free(&array);
    }

    /* xiarray_free */
    {
        {
            XIArray_PT array = NULL;
            xiarray_free(NULL);
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(0);
            xiarray_free(&array);
        }
    }

    /* xiarray_clear */
    {
        {
            bool except = false;
            XEXCEPT_TRY
                xiarray_clear(NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        {
            XIArray_PT array = xiarray_new(10);
            xiarray_fill(array, 1);
            xassert(xiarray_get(array, 2) == 1);

            xiarray_clear(array);
            xassert(xiarray_get(array, 2) == 0);

            xassert(array->size == 10);
            xassert(array->datas);

            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(0);
            xiarray_clear(array);
            xassert(array->size == 0);
            xassert(array->datas == NULL);
            xiarray_free(&array);
        }
    }

    /* xiarray_remove */
    {
        {
            bool except = false;
            XEXCEPT_TRY
                xiarray_remove(NULL, 0, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        {
            XIArray_PT array = xiarray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xiarray_remove(array, -1, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xiarray_remove(array, 20, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);
            xiarray_fill(array, 1);

            xassert(xiarray_get(array, 3) == 1);
            xassert(xiarray_remove(array, 3, NULL));
            xassert(xiarray_get(array, 3) == 0);

            int memory = 0;
            xassert(xiarray_remove(array, 5, &memory));
            xassert(memory == 1);

            xassert(array->size == 10);

            xiarray_free(&array);
        }
    }

    /* xiarray_size */
    {
        {
            XIArray_PT array = xiarray_new(10);
            xiarray_fill(array, 1);
            xassert(xiarray_size(array) == 10);
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(0);
            xassert(xiarray_size(array) == 0);
            xiarray_free(&array);
        }
    }

    /* xiarray_is_empty */
    {
        {
            XIArray_PT array = xiarray_new(10);
            xassert_false(xiarray_is_empty(array));
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(0);
            xassert(xiarray_is_empty(array));
            xiarray_free(&array);
        }
    }

    /* xiarray_resize */
    {
        {
            bool except = false;
            XEXCEPT_TRY
                xiarray_resize(NULL, 1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        {
            XIArray_PT array = xiarray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xiarray_resize(array, -1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);
            xassert(xiarray_resize(array, 0));

            xassert(array->size == 0);
            xassert(array->datas == NULL);

            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(0);
            xassert(xiarray_resize(array, 0));

            xassert(array->size == 0);
            xassert(array->datas == NULL);

            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(0);
            xassert(xiarray_resize(array, 5));

            xassert(array->size == 5);
            xassert(array->datas);

            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);
            xiarray_put(array, 3, 1, NULL);
            xiarray_put(array, 8, 2, NULL);

            xassert(xiarray_resize(array, 10));
            xassert(xiarray_get(array, 8) == 2);

            xassert(xiarray_resize(array, 20));
            xassert(xiarray_get(array, 3) == 1);
            xassert(xiarray_get(array, 5) == 0);
            xassert(xiarray_get(array, 8) == 2);
            xassert(array->size == 20);
            xassert(array->datas);

            xassert(xiarray_resize(array, 5));
            xassert(xiarray_get(array, 1) == 0);
            xassert(xiarray_get(array, 3) == 1);
            xassert(array->size == 5);
            xassert(array->datas);

            xiarray_free(&array);
        }
    }

    /* xiarray_resize */
    {
        {
            XIArray_PT array = xiarray_new(0);
            xassert(xiarray_resize(array, 10));
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);
            xiarray_put(array, 3, 1, NULL);
            xiarray_put(array, 8, 2, NULL);

            xassert(xiarray_resize(array, 10));
            xassert(xiarray_get(array, 8) == 2);

            xassert(xiarray_resize(array, 5));
            xassert(xiarray_get(array, 3) == 1);

            xassert(xiarray_resize(array, 10));
            xassert(xiarray_get(array, 6) == 0);
            xassert(xiarray_get(array, 8) == 0);
            xiarray_free(&array);
        }
    }

    /* xiarray_swap */
    {
        {
            XIArray_PT array = xiarray_new(10);
            bool except = false;
            XEXCEPT_TRY
                xiarray_swap(NULL, array);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);
            bool except = false;
            XEXCEPT_TRY
                xiarray_swap(array, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xiarray_free(&array);
        }

        {
            bool except = false;
            XEXCEPT_TRY
                xiarray_swap(NULL, NULL);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        {
            XIArray_PT array1 = xiarray_new(10);
            XIArray_PT array2 = xiarray_new(15);

            xiarray_put(array1, 5, 1, NULL);
            xiarray_put(array2, 5, 2, NULL);

            xassert(xiarray_swap(array1, array2));

            xassert(xiarray_get(array1, 5) == 2);
            xassert(xiarray_get(array2, 5) == 1);

            xassert(array1->size == 15);

            xassert(array2->size == 10);

            xiarray_free(&array1);
            xiarray_free(&array2);
        }
    }

    /* xiarray_exch */
    {
        {
            bool except = false;
            XEXCEPT_TRY
                xiarray_exch(NULL, 0, 1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
        }

        {
            XIArray_PT array = xiarray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xiarray_exch(array, -1, 1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xiarray_exch(array, 20, 1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xiarray_exch(array, 0, -1);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);

            bool except = false;
            XEXCEPT_TRY
                xiarray_exch(array, 0, 20);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

                xassert(except);
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_new(10);
            xiarray_put(array, 3, 1, NULL);
            xiarray_put(array, 5, 2, NULL);

            xassert(xiarray_exch(array, 5, 5));
            xassert(xiarray_get(array, 5) == 2);

            xassert(xiarray_exch(array, 3, 5));
            xassert(xiarray_get(array, 3) == 2);
            xassert(xiarray_get(array, 5) == 1);

            xiarray_free(&array);
        }
    }

    /* xiarray_is_sorted */
    {
    }
    
    /* xiarray_quick_sort */
    {
        XIArray_PT array = xiarray_random(100);
        xassert(xiarray_quick_sort(array));
        xiarray_free(&array);
    }

    /* xiarray_counting_sort */
    {
        {
            XIArray_PT array = xiarray_new(30);
            xiarray_put(array, 0, 1, NULL);
            xiarray_put(array, 1, 0, NULL);
            xiarray_put(array, 2, 2, NULL);
            xiarray_put(array, 3, 1, NULL);
            xiarray_put(array, 4, 2, NULL);
            xiarray_put(array, 5, 3, NULL);
            xiarray_put(array, 6, 1, NULL);
            xiarray_put(array, 7, 4, NULL);
            xiarray_put(array, 8, 1, NULL);
            xiarray_put(array, 9, 4, NULL);
            xiarray_put(array, 10, 1, NULL);
            xiarray_put(array, 11, 0, NULL);
            xiarray_put(array, 12, 2, NULL);
            xiarray_put(array, 13, 1, NULL);
            xiarray_put(array, 14, 2, NULL);
            xiarray_put(array, 15, 3, NULL);
            xiarray_put(array, 16, 1, NULL);
            xiarray_put(array, 17, 4, NULL);
            xiarray_put(array, 18, 1, NULL);
            xiarray_put(array, 19, 4, NULL);
            xiarray_put(array, 20, 1, NULL);
            xiarray_put(array, 21, 0, NULL);
            xiarray_put(array, 22, 2, NULL);
            xiarray_put(array, 23, 1, NULL);
            xiarray_put(array, 24, 2, NULL);
            xiarray_put(array, 25, 3, NULL);
            xiarray_put(array, 26, 1, NULL);
            xiarray_put(array, 27, 4, NULL);
            xiarray_put(array, 28, 1, NULL);
            xiarray_put(array, 29, 4, NULL);
            xassert(xiarray_counting_sort(array));
            xiarray_free(&array);
        }

        {
            XIArray_PT array = xiarray_random(1000);
            xassert(xiarray_counting_sort(array));
            xiarray_free(&array);
        }
    }

    /* xiarray_bucket_sort */
    {
        {
            XIArray_PT array = xiarray_new(10);
            xiarray_put(array, 0, 1, NULL);
            xiarray_put(array, 1, 0, NULL);
            xiarray_put(array, 2, 2, NULL);
            xiarray_put(array, 3, 1, NULL);
            xiarray_put(array, 4, 2, NULL);
            xiarray_put(array, 5, 3, NULL);
            xiarray_put(array, 6, 1, NULL);
            xiarray_put(array, 7, 4, NULL);
            xiarray_put(array, 8, 1, NULL);
            xiarray_put(array, 9, 4, NULL);
            xassert(xiarray_bucket_sort(array, 3));
            xiarray_free(&array);
        }

        {
            for (int i = 0; i < 50; i++) {
                XIArray_PT array = xiarray_random(300+i);
                xassert(xiarray_bucket_sort(array, 20));
                xiarray_free(&array);
            }
        }
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
