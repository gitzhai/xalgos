/*
*   Copyright (C) 2022, Xiaosan Zhai(tom.zhai@aliyun.com)
*   This file is part of the xalgos library.
*
*   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
*    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*    See the MIT License for more details.
*
*   You should have received a copy of the MIT License along with this program.
*   If not, see <https://mit-license.org/>.
*/

#include <stdbool.h>
#include <stdio.h>

#include "../bit/xbit_x.h"
#include "../include/xalgos.h"

static
void xbit_print(XBit_PT bit) {
    for (int i = 0; i < bit->length; ++i) {
        if ((i != 0) && (i % 8 == 0)) {
            printf(",");
        }
        printf("%d", xbit_get(bit, i));
    }
    printf("\n");
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

void test_xbit() {
    /* xbit_new */
    {
        /* length = 0 */
        {
            XBit_PT bit = xbit_new(0);
            xassert(bit->length == 0);
            xassert_false(bit->bytes);
            xassert_false(bit->words);
            xbit_free(&bit);
        }

        /* 0 < length */
        {
            XBit_PT bit = xbit_new(100);
            xassert(bit->length == 100);
            xassert(bit->bytes);
            xassert((void*)bit->words == (void*)bit->bytes);
            xbit_free(&bit);
        }
    }

    /* xbit_put */
    /* xbit_get */
    {
        XBit_PT bit = xbit_new(100);
        xbit_put(bit, 0, 1);
        xassert(xbit_get(bit, 0) == 1);

        xbit_put(bit, 5, 1);
        xassert(xbit_get(bit, 5) == 1);

        xbit_put(bit, 33, 1);
        xassert(xbit_get(bit, 33) == 1);

        xbit_put(bit, 57, 1);
        xassert(xbit_get(bit, 57) == 1);

        xbit_put(bit, 60, 0);
        xassert(xbit_get(bit, 60) == 0);

        xbit_put(bit, 79, 1);
        xassert(xbit_get(bit, 79) == 1);

        xbit_put(bit, 98, 1);
        xassert(xbit_get(bit, 98) == 1);

        xassert(xbit_count(bit) == 6);
        xbit_clear(bit, 0, 99);
        xassert(xbit_count(bit) == 0);

        xbit_free(&bit);
    }

    /* xbit_set */
    /* xbit_not */
    /* xbit_clear */
    {
        {
            XBit_PT bit = xbit_new(20);
            xbit_set(bit, 11, 14);
            xassert(xbit_count(bit) == 4);

            xbit_not(bit, 11, 14);
            xassert(xbit_count(bit) == 0);

            xbit_not(bit, 11, 14);
            xassert(xbit_count(bit) == 4);

            xbit_clear(bit, 11, 14);
            xassert(xbit_count(bit) == 0);

            xbit_free(&bit);
        }

        {
            XBit_PT bit = xbit_new(100);
            xbit_set(bit, 11, 50);
            xassert(xbit_count(bit) == 40);
            xassert(xbit_get(bit, 10) == 0);
            xassert(xbit_get(bit, 11) == 1);
            xassert(xbit_get(bit, 30) == 1);
            xassert(xbit_get(bit, 50) == 1);
            xassert(xbit_get(bit, 51) == 0);

            xbit_not(bit, 11, 40);
            xassert(xbit_count(bit) == 10);
            xassert(xbit_get(bit, 10) == 0);
            xassert(xbit_get(bit, 11) == 0);
            xassert(xbit_get(bit, 30) == 0);
            xassert(xbit_get(bit, 41) == 1);
            xassert(xbit_get(bit, 50) == 1);
            xassert(xbit_get(bit, 51) == 0);

            xbit_clear(bit, 41, 50);
            xassert(xbit_count(bit) == 0);

            xbit_free(&bit);
        }
    }

    /* xbit_lt */
    {
        XBit_PT bit1 = xbit_new(100);
        XBit_PT bit2 = xbit_new(100);

        xbit_set(bit1, 11, 50);
        xbit_set(bit2, 10, 50);

        xassert(xbit_lt(bit1, bit2));
        xassert_false(xbit_lt(bit2, bit1));

        xbit_free(&bit1);
        xbit_free(&bit2);
    }

    /* xbit_leq */
    {
        {
            XBit_PT bit1 = xbit_new(100);
            XBit_PT bit2 = xbit_new(100);

            xbit_set(bit1, 11, 50);
            xbit_set(bit2, 10, 50);

            xassert(xbit_leq(bit1, bit2));
            xassert_false(xbit_leq(bit2, bit1));

            xbit_free(&bit1);
            xbit_free(&bit2);
        }

        {
            XBit_PT bit1 = xbit_new(100);
            XBit_PT bit2 = xbit_new(100);

            xbit_set(bit1, 11, 50);
            xbit_set(bit2, 11, 50);

            xassert(xbit_leq(bit1, bit2));
            xassert(xbit_leq(bit2, bit1));

            xbit_free(&bit1);
            xbit_free(&bit2);
        }
    }

    /* xbit_eq */
    {
        {
            XBit_PT bit1 = xbit_new(100);
            XBit_PT bit2 = xbit_new(100);

            xbit_set(bit1, 10, 50);
            xbit_set(bit2, 10, 50);

            xassert(xbit_eq(bit1, bit2));
            xassert(xbit_eq(bit2, bit1));

            xbit_free(&bit1);
            xbit_free(&bit2);
        }

        {
            XBit_PT bit1 = xbit_new(100);
            XBit_PT bit2 = xbit_new(100);

            xbit_set(bit1, 10, 50);
            xbit_set(bit2, 11, 50);

            xassert_false(xbit_eq(bit1, bit2));
            xassert_false(xbit_eq(bit2, bit1));

            xbit_free(&bit1);
            xbit_free(&bit2);
        }
    }

    /* xbit_union */
    {
        xassert_false(xbit_union(NULL, NULL));

        {
            XBit_PT bit = xbit_new(100);
            xbit_set(bit, 11, 20);

            XBit_PT nbit = xbit_union(NULL, bit);
            xassert(xbit_count(nbit) == 10);
            xbit_not(nbit, 11, 20);
            xassert(xbit_count(nbit) == 0);

            xbit_free(&bit);
            xbit_free(&nbit);
        }

        {
            XBit_PT bit = xbit_new(100);
            xbit_set(bit, 11, 20);

            XBit_PT nbit = xbit_union(bit, NULL);
            xassert(xbit_count(nbit) == 10);
            xbit_not(nbit, 11, 20);
            xassert(xbit_count(nbit) == 0);

            xbit_free(&bit);
            xbit_free(&nbit);
        }

        {
            XBit_PT bit = xbit_new(100);
            xbit_set(bit, 11, 20);

            XBit_PT nbit = xbit_union(bit, bit);
            xassert(xbit_count(nbit) == 10);
            xbit_not(nbit, 11, 20);
            xassert(xbit_count(nbit) == 0);

            xbit_free(&bit);
            xbit_free(&nbit);
        }

        {
            XBit_PT bit1 = xbit_new(100);
            XBit_PT bit2 = xbit_new(100);
            xbit_set(bit1, 11, 20);
            xbit_set(bit2, 31, 40);

            XBit_PT nbit = xbit_union(bit1, bit2);
            xassert(xbit_count(nbit) == 20);
            xbit_not(nbit, 11, 20);
            xassert(xbit_count(nbit) == 10);
            xbit_not(nbit, 31, 40);
            xassert(xbit_count(nbit) == 0);

            xbit_free(&bit1);
            xbit_free(&bit2);
            xbit_free(&nbit);
        }

        {
            XBit_PT bit1 = xbit_new(100);
            XBit_PT bit2 = xbit_new(100);
            xbit_set(bit1, 11, 20);
            xbit_set(bit2, 15, 30);

            XBit_PT nbit = xbit_union(bit1, bit2);
            xassert(xbit_count(nbit) == 20);
            xbit_not(nbit, 11, 20);
            xassert(xbit_count(nbit) == 10);
            xbit_not(nbit, 21, 30);
            xassert(xbit_count(nbit) == 0);

            xbit_free(&bit1);
            xbit_free(&bit2);
            xbit_free(&nbit);
        }
    }

    /* xbit_inter */
    {
        {
            XBit_PT bit = xbit_new(100);

            xassert_false(xbit_inter(NULL, NULL));
            xassert_false(xbit_inter(bit, NULL));
            xassert_false(xbit_inter(NULL, bit));

            xbit_free(&bit);
        }

        {
            XBit_PT bit = xbit_new(100);
            xbit_set(bit, 11, 20);

            XBit_PT nbit = xbit_inter(bit, bit);
            xassert(xbit_count(nbit) == 10);
            xbit_not(nbit, 11, 20);
            xassert(xbit_count(nbit) == 0);

            xbit_free(&bit);
            xbit_free(&nbit);
        }

        {
            XBit_PT bit1 = xbit_new(100);
            XBit_PT bit2 = xbit_new(100);
            xbit_set(bit1, 11, 20);
            xbit_set(bit2, 31, 40);

            XBit_PT nbit = xbit_inter(bit1, bit2);
            xassert(xbit_count(nbit) == 0);

            xbit_free(&bit1);
            xbit_free(&bit2);
            xbit_free(&nbit);
        }

        {
            XBit_PT bit1 = xbit_new(100);
            XBit_PT bit2 = xbit_new(100);
            xbit_set(bit1, 11, 20);
            xbit_set(bit2, 15, 30);

            XBit_PT nbit = xbit_inter(bit1, bit2);
            xassert(xbit_count(nbit) == 6);
            xbit_not(nbit, 15, 20);
            xassert(xbit_count(nbit) == 0);

            xbit_free(&bit1);
            xbit_free(&bit2);
            xbit_free(&nbit);
        }
    }

    /* xbit_minus */
    {
        {
            XBit_PT bit = xbit_new(100);
            xassert_false(xbit_minus(NULL, bit));
            xbit_free(&bit);
        }

        {
            XBit_PT bit = xbit_new(100);
            xbit_set(bit, 11, 20);

            XBit_PT nbit = xbit_minus(bit, NULL);
            xassert(xbit_count(nbit) == 10);
            xbit_not(nbit, 11, 20);
            xassert(xbit_count(nbit) == 0);

            xbit_free(&bit);
            xbit_free(&nbit);
        }

        {
            XBit_PT bit = xbit_new(100);
            xbit_set(bit, 11, 20);

            XBit_PT nbit = xbit_minus(bit, bit);
            xassert(xbit_count(nbit) == 0);

            xbit_free(&bit);
            xbit_free(&nbit);
        }

        {
            XBit_PT bit1 = xbit_new(100);
            XBit_PT bit2 = xbit_new(100);
            xbit_set(bit1, 11, 20);
            xbit_set(bit2, 31, 40);

            XBit_PT nbit = xbit_minus(bit1, bit2);
            xassert(xbit_count(nbit) == 10);

            xbit_free(&bit1);
            xbit_free(&bit2);
            xbit_free(&nbit);
        }

        {
            XBit_PT bit1 = xbit_new(100);
            XBit_PT bit2 = xbit_new(100);
            xbit_set(bit1, 11, 20);
            xbit_set(bit2, 15, 30);

            XBit_PT nbit = xbit_minus(bit1, bit2);
            xassert(xbit_count(nbit) == 4);
            xbit_not(nbit, 11, 14);
            xassert(xbit_count(nbit) == 0);

            xbit_free(&bit1);
            xbit_free(&bit2);
            xbit_free(&nbit);
        }

        {
            XBit_PT bit1 = xbit_new(100);
            XBit_PT bit2 = xbit_new(100);
            xbit_set(bit1, 11, 20);
            xbit_set(bit2, 11, 20);

            XBit_PT nbit = xbit_minus(bit1, bit2);
            xassert(xbit_count(nbit) == 0);

            xbit_free(&bit1);
            xbit_free(&bit2);
            xbit_free(&nbit);
        }

        {
            XBit_PT bit1 = xbit_new(100);
            XBit_PT bit2 = xbit_new(100);
            xbit_set(bit1, 11, 20);
            xbit_set(bit2, 1, 30);

            XBit_PT nbit = xbit_minus(bit1, bit2);
            xassert(xbit_count(nbit) == 0);

            xbit_free(&bit1);
            xbit_free(&bit2);
            xbit_free(&nbit);
        }
    }

    /* xbit_diff */
    {
        xassert_false(xbit_diff(NULL, NULL));

        {
            XBit_PT bit = xbit_new(100);
            xbit_set(bit, 11, 20);

            XBit_PT nbit = xbit_diff(NULL, bit);
            xassert(xbit_count(nbit) == 10);
            xbit_not(nbit, 11, 20);
            xassert(xbit_count(nbit) == 0);

            xbit_free(&bit);
            xbit_free(&nbit);
        }

        {
            XBit_PT bit = xbit_new(100);
            xbit_set(bit, 11, 20);

            XBit_PT nbit = xbit_diff(bit, NULL);
            xassert(xbit_count(nbit) == 10);
            xbit_not(nbit, 11, 20);
            xassert(xbit_count(nbit) == 0);

            xbit_free(&bit);
            xbit_free(&nbit);
        }

        {
            XBit_PT bit = xbit_new(100);
            xbit_set(bit, 11, 20);

            XBit_PT nbit = xbit_diff(bit, bit);
            xassert(xbit_count(nbit) == 0);

            xbit_free(&bit);
            xbit_free(&nbit);
        }

        {
            XBit_PT bit1 = xbit_new(100);
            XBit_PT bit2 = xbit_new(100);
            xbit_set(bit1, 11, 20);
            xbit_set(bit2, 31, 40);

            XBit_PT nbit = xbit_diff(bit1, bit2);
            xassert(xbit_count(nbit) == 20);

            xbit_clear(nbit, 11, 40);
            xassert(xbit_count(nbit) == 0);

            xbit_free(&bit1);
            xbit_free(&bit2);
            xbit_free(&nbit);
        }

        {
            XBit_PT bit1 = xbit_new(100);
            XBit_PT bit2 = xbit_new(100);
            xbit_set(bit1, 11, 20);
            xbit_set(bit2, 15, 30);

            XBit_PT nbit = xbit_diff(bit1, bit2);
            xassert(xbit_count(nbit) == 14);
            xbit_not(nbit, 11, 14);
            xassert(xbit_count(nbit) == 10);
            xbit_not(nbit, 21, 30);
            xassert(xbit_count(nbit) == 0);

            xbit_free(&bit1);
            xbit_free(&bit2);
            xbit_free(&nbit);
        }
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}
