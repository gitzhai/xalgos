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

#ifndef XBIT_INCLUDED
#define XBIT_INCLUDED

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XBit *XBit_PT;

extern XBit_PT xbit_new    (int length);

extern XBit_PT xbit_copy   (XBit_PT set);

extern int     xbit_put    (XBit_PT set, int n, int XBit);
extern int     xbit_get    (XBit_PT set, int n);

extern bool    xbit_set    (XBit_PT set, int lo, int hi);
extern bool    xbit_not    (XBit_PT set, int lo, int hi);
extern bool    xbit_clear  (XBit_PT set, int lo, int hi);

extern bool    xbit_lt     (XBit_PT s, XBit_PT t);   /* s < t */
extern bool    xbit_leq    (XBit_PT s, XBit_PT t);   /* s <= t */
extern bool    xbit_eq     (XBit_PT s, XBit_PT t);   /* s == t */

extern XBit_PT xbit_union  (XBit_PT s, XBit_PT t);
extern XBit_PT xbit_inter  (XBit_PT s, XBit_PT t);
extern XBit_PT xbit_minus  (XBit_PT s, XBit_PT t);
extern XBit_PT xbit_diff   (XBit_PT s, XBit_PT t);

extern int     xbit_map    (XBit_PT set, bool (*apply)(int n, int bit, void *cl), void *cl);

extern int     xbit_length (XBit_PT set);
extern int     xbit_count  (XBit_PT set);

extern void    xbit_free   (XBit_PT *set);

#ifdef __cplusplus
}
#endif

#endif
