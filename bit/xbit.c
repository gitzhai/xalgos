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
*
*   Refer to :
*       <<C Interfaces and Implementations>> David R. Hanson, chapter 13
*/

#include <stdarg.h>
#include <string.h>

#include "../include/xassert.h"
#include "../include/xmem.h"
#include "xbit_x.h"

#define XBIT_BPW (8*sizeof (unsigned long))                                    /* BIT_BPW : bit per words */
#define xbit_nwords(len) ((((len) + XBIT_BPW - 1)&(~(XBIT_BPW-1)))/XBIT_BPW)   /* how many words for len bits */
#define xbit_nbytes(len) ((((len) + 8 - 1)&(~(8-1)))/8)                        /* how many bytes for len bits */

/*   1,  1,  1,  1, |  1,  1,  1,  1
 *  128  64  32  16 |  8   4   2   1
 *------------------|-------------------
 *     high 4 bits  |  low 4 bits
 */
static
unsigned char xbit_msbmask[] = {
    0xFF,   // 1111 1111
    0xFE,   // 1111 1110
    0xFC,   // 1111 1100
    0xF8,   // 1111 1000
    0xF0,   // 1111 0000
    0xE0,   // 1110 0000
    0xC0,   // 1100 0000
    0x80    // 1000 0000
};

static
unsigned char xbit_lsbmask[] = {
    0x01,   // 0000 0001
    0x03,   // 0000 0011
    0x07,   // 0000 0111
    0x0F,   // 0000 1111
    0x1F,   // 0001 1111
    0x3F,   // 0011 1111
    0x7F,   // 0111 1111
    0xFF    // 1111 1111
};

static 
char xbit_bitcount[] = {
        0,  // 0000 0000   : 0
        1,  // 0000 0001   : 1
        1,  // 0000 0010   : 2
        2,  // 0000 0011   : 3
        1,  // 0000 0100   : 4
        2,  // 0000 0101   : 5
        2,  // 0000 0110   : 6
        3,  // 0000 0111   : 7
        1,  // 0000 1000   : 8
        2,  // 0000 1001   : 9
        2,  // 0000 1010   : 10
        3,  // 0000 1011   : 11
        2,  // 0000 1100   : 12
        3,  // 0000 1101   : 13
        3,  // 0000 1110   : 14
        4   // 0000 1111   : 15
};

XBit_PT xbit_new(int length) {
    xassert(0 <= length);

    if (length < 0) {
        return NULL;
    }

    {
        XBit_PT set = XMEM_CALLOC(1, sizeof(*set));
        if (!set) {
            return NULL;
        }

        if (0 < length) {
            set->words = XMEM_CALLOC(xbit_nwords(length), sizeof(unsigned long));
            if (!set->words) {
                XMEM_FREE(set);
                return NULL;
            }

            set->bytes = (unsigned char *)set->words;
        }

        set->length = length;

        return set;
    }
}

XBit_PT xbit_copy(XBit_PT set) {
    xassert(set);

    if (!set) {
        return NULL;
    }

    {
        XBit_PT nset = xbit_new(set->length);
        if (!nset) {
            return NULL;
        }

        if (0 < set->length) {
            memcpy(nset->bytes, set->bytes, xbit_nbytes(set->length));
        }

        return nset;
    }
}

int xbit_put(XBit_PT set, int n, int bit) {
    xassert(set);
    xassert((bit == 0) || (bit == 1));
    xassert((0 <= n) && (n < set->length));

    if (!set || ((bit != 0) && (bit != 1)) || (n < 0) || (set->length <= n)) {
        return 0;
    }

    {
        int prev = ((set->bytes[n / 8] >> (n % 8)) & 1);

        if (bit == 1) {
            set->bytes[n / 8] |= 1 << (n % 8);
        }
        else {
            set->bytes[n / 8] &= ~(1 << (n % 8));
        }

        return prev;
    }
}

int xbit_get(XBit_PT set, int n) {
    xassert(set);
    xassert(0 <= n && n < set->length);

    if (!set || (n < 0) || (set->length <= n)) {
        return 0;
    }

    return ((set->bytes[n / 8] >> (n % 8)) & 1);
}

bool xbit_set(XBit_PT set, int lo, int hi) {
    xassert(set);
    xassert((0 <= lo) && (hi < set->length));
    xassert(lo <= hi);

    if (!set || (lo < 0) || (set->length <= hi) || (hi < lo)) {
        return false;
    }

    if (lo / 8 < hi / 8) {
        set->bytes[lo / 8] |= xbit_msbmask[lo % 8];
        for (int i = lo / 8 + 1; i < hi / 8; i++) {
            set->bytes[i] = 0xFF;
        }
        set->bytes[hi / 8] |= xbit_lsbmask[hi % 8];
    }
    else {
        set->bytes[lo / 8] |= (xbit_msbmask[lo % 8] & xbit_lsbmask[hi % 8]);
    }

    return true;
}

bool xbit_not(XBit_PT set, int lo, int hi) {
    xassert(set);
    xassert((0 <= lo) && (hi < set->length));
    xassert(lo <= hi);

    if (!set || (lo < 0) || (set->length <= hi) || (hi < lo)) {
        return false;
    }

    if (lo / 8 < hi / 8) {
        set->bytes[lo / 8] ^= xbit_msbmask[lo % 8];
        for (int i = lo / 8 + 1; i < hi / 8; i++) {
            set->bytes[i] ^= 0xFF;
        }
        set->bytes[hi / 8] ^= xbit_lsbmask[hi % 8];
    }
    else {
        set->bytes[lo / 8] ^= (xbit_msbmask[lo % 8] & xbit_lsbmask[hi % 8]);
    }

    return true;
}

bool xbit_clear(XBit_PT set, int lo, int hi) {
    xassert(set);
    xassert((0 <= lo) && (hi < set->length));
    xassert(lo <= hi);

    if (!set || (lo < 0) || (set->length <= hi) || (hi < lo)) {
        return false;
    }

    if (lo / 8 < hi / 8) {
        set->bytes[lo / 8] &= ~xbit_msbmask[lo % 8];
        for (int i = lo / 8 + 1; i < hi / 8; i++) {
            set->bytes[i] = 0;
        }
        set->bytes[hi / 8] &= ~xbit_lsbmask[hi % 8];
    }
    else {
        set->bytes[lo / 8] &= ~(xbit_msbmask[lo % 8] & xbit_lsbmask[hi % 8]);
    }

    return true;
}

bool xbit_lt(XBit_PT s, XBit_PT t) {
    xassert(s && t);
    xassert(s->length == t->length);

    if (!s || !t || (s->length != t->length)) {
        return false;
    }

    {
        bool lt = false;

        for (int i = xbit_nwords(s->length); 0 <= --i; ) {
            if ((s->words[i] & ~t->words[i]) != 0) {
                return false;
            }
            else if (s->words[i] != t->words[i]) {
                lt = true;
            }
        }

        return lt;
    }
}

bool xbit_leq(XBit_PT s, XBit_PT t) {
    xassert(s && t);
    xassert(s->length == t->length);

    if (!s || !t || (s->length != t->length)) {
        return false;
    }

    for (int i = xbit_nwords(s->length); 0 <= --i; ) {
        if ((s->words[i] & ~t->words[i]) != 0) {
            return false;
        }
    }

    return true;
}

bool xbit_eq(XBit_PT s, XBit_PT t) {
    xassert(s && t);
    xassert(s->length == t->length);

    if (!s || !t || (s->length != t->length)) {
        return false;
    }

    for (int i = xbit_nwords(s->length); 0 <= --i; ) {
        if (s->words[i] != t->words[i]) {
            return false;
        }
    }

    return true;
}

XBit_PT xbit_union(XBit_PT s, XBit_PT t) {
    if (!s) {
        if (!t) {
            return NULL;
        }

        return xbit_copy(t);
    }

    if (!t) {
        return xbit_copy(s);
    }

    if (s == t) {
        return xbit_copy(s);
    }

    xassert(s->length == t->length);

    if (s->length != t->length) {
        return NULL;
    }

    {
        XBit_PT set = xbit_new(s->length);
        if (!set) {
            return NULL;
        }

        for (int i = xbit_nwords(s->length); 0 <= --i; ) {
            set->words[i] = s->words[i] | t->words[i];
        }

        return set;
    }
}

XBit_PT xbit_inter(XBit_PT s, XBit_PT t) {
    if (!s || !t) {
        return NULL;
    }

    if (s == t) {
        return xbit_copy(s);
    }

    xassert(s->length == t->length);

    if (s->length != t->length) {
        return NULL;
    }

    {
        XBit_PT set = xbit_new(s->length);
        if (!set) {
            return NULL;
        }

        for (int i = xbit_nwords(s->length); 0 <= --i; ) {
            set->words[i] = s->words[i] & t->words[i];
        }

        return set;
    }
}

XBit_PT xbit_minus(XBit_PT s, XBit_PT t) {
    if (!s) {
        return NULL;
    }

    if (!t) {
        return xbit_copy(s);
    }

    if (s == t) {
        return xbit_new(s->length);
    }

    xassert(s->length == t->length);

    if (s->length != t->length) {
        return NULL;
    }

    {
        XBit_PT set = xbit_new(s->length);
        if (!set) {
            return NULL;
        }

        for (int i = xbit_nwords(s->length); 0 <= --i; ) {
            set->words[i] = s->words[i] & ~t->words[i];
        }

        return set;
    }
}

XBit_PT xbit_diff(XBit_PT s, XBit_PT t) {
    if (!s) {
        if (!t) {
            return NULL;
        }

        return xbit_copy(t);
    }

    if (!t) {
        return xbit_copy(s);
    }

    if (s == t) {
        return xbit_new(s->length);
    }

    xassert(s->length == t->length);

    if (s->length != t->length) {
        return NULL;
    }

    {
        XBit_PT set = xbit_new(s->length);
        if (!set) {
            return NULL;
        }

        for (int i = xbit_nwords(s->length); 0 <= --i; ) {
            set->words[i] = s->words[i] ^ t->words[i];
        }

        return set;
    }
}

int xbit_map(XBit_PT set, bool (*apply)(int n, int bit, void *cl), void *cl) {
    xassert(set);
    xassert(apply);

    if (!set || !apply) {
        return -1;
    }

    {
        int count = 0;

        for (int n = 0; n < set->length; n++) {
            if (apply(n, ((set->bytes[n / 8] >> (n % 8)) & 1), cl)) {
                ++count;
            }
        }

        return count;
    }
}

int xbit_length(XBit_PT set) {
    return set ? set->length : 0;
}

/*   1,  1,  1,  1, |  1,  1,  1,  1
 *  128  64  32  16 |  8   4   2   1
 *------------------|-------------------
 *     high 4 bits  |  low 4 bits
 */

int xbit_count(XBit_PT set) {
    xassert(set);

    if (!set) {
        return 0;
    }

    {
        int length = 0;

        for (int n = xbit_nbytes(set->length); 0 <= --n; ) {
            unsigned char c = set->bytes[n];
            length += xbit_bitcount[c & 0xF] + xbit_bitcount[c >> 4];
        }

        return length;
    }
}

void xbit_free(XBit_PT *set) {
    xassert(set && *set);

    if (!set || !*set) {
        return;
    }

    XMEM_FREE((*set)->words);
    XMEM_FREE(*set);
}
