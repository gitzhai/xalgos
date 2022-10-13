/*
*   Copyright (C) 2022, Xiaosan Zhai(tom.zhai@aliyun.com)
*   This file is part of the xalgos library.
*
*   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
*   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*   See the MIT License for more details.
*
*   You should have received a copy of the MIT License along with this program.
*   If not, see <https://mit-license.org/>.
*/

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "../include/xmem.h"
#include "xutils.h"

static const int xutils_hash_primes_num = 22;

static const int xutils_hash_primes[] =
{
    53,         97,         193,       389,       769,
    1543,       3079,       6151,      12289,     24593,
    49157,      98317,      196613,    393241,    786433,
    1572869,    3145739,    6291469,   12582917,  25165843,
    50331653,   100663319
};

int xutils_hash_buckets_num(int hint) {
    int i = 1;
    for (; ((i - 1) < xutils_hash_primes_num) && xutils_hash_primes[i - 1] < hint; ) {
        ++i;
    }
    if (i == (xutils_hash_primes_num + 1)) {
        i -= 1;
    }

    return xutils_hash_primes[i - 1];
}

int xutils_max_hash_buckets_size(void) {
    return xutils_hash_primes[xutils_hash_primes_num - 1];
}

int xutils_pointer_equal(void *x, void *y, void *cl) {
    return x == y ? 0 : 1;
}

bool xutils_match(void *x, void *y) {
    return x == y;
}

int xutils_hash_atom(void *x) {
    return (int)((unsigned long long)x>>2);
}

/* Mastering Algorithms with C : Chinese version : P123 */
int xutils_hash_generic(void *key) {
    char *ptr = key;
    int val = 0;

    while (*ptr != '\0') {
        int tmp = 0;

        val = (val << 4) + (*ptr);

        if (tmp = (val & 0xf0000000)) {
            val = val ^ (tmp >> 24);
            val = val ^ tmp;
        }

        ptr++;
    }

    return val;
}

int xutils_hash_pointer(void *pointer) {
    return (int)((unsigned long long)(pointer) >> 3);
}

int xutils_hash_int(int m) {
    return m;
}

int xutils_hash_float(float m) {
    return 1;
}

/* SGI STL stl_hash_fun.h */
int xutils_hash_string(char* str)
{
    int h = 0;
    for (; *str; ++str) {
        h = 5 * h + *str;
    }

    return h;
}

int xutils_hash_const_chars(const char *str, int len) {
    int h = 0;
    for (int i = 0; (i < len) && (*str); ++i, ++str) {
        h = 5 * h + *str;
    }

    return h;
}

bool xutils_generic_swap(void* x, void* y, int size) {
    void* tmp = NULL;
    if ((tmp = XMEM_CALLOC(1, size)) == NULL) {
        return false;
    }

    memcpy(tmp, x, size);
    memcpy(x, y, size);
    memcpy(y, tmp, size);

    XMEM_FREE(tmp);

    return true;
}

void* xutils_deep_copy(void* source, int size) {
    if (!source || (size <= 0)) {
        return NULL;
    }

    void* target = XMEM_CALLOC(1, size);
    if (!target) {
        return NULL;
    }

    memcpy(target, source, size);

    return target;
}
