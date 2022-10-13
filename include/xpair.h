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

#ifndef XPAIR_INCLUDED
#define XPAIR_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/* define XPair_PT here to make direct access to member "first" and "second" */
typedef struct XPair*  XPair_PT;
struct XPair {
    void *first;
    void *second;
};

/* O(1) */
extern XPair_PT  xpair_new         (void *first, void *second);

/* O(1) */
extern XPair_PT  xpair_copy        (XPair_PT pair);
extern XPair_PT  xpair_deep_copy   (XPair_PT pair, int first_size, int second_size);

/* O(1) */
extern void*     xpair_first       (XPair_PT pair);
extern void*     xpair_second      (XPair_PT pair);

/* O(1) */
extern void      xpair_set_first   (XPair_PT pair, void* first);
extern void      xpair_set_second  (XPair_PT pair, void *second);

/* O(1) */
extern void      xpair_free        (XPair_PT *pair);
extern void      xpair_free_apply  (XPair_PT *pair, bool (*apply)(void *first, void **second, void *cl), void *cl);
extern void      xpair_deep_free   (XPair_PT *pair);

/* O(1) */
extern void      xpair_free_first  (XPair_PT pair);
extern void      xpair_free_second (XPair_PT pair);

#ifdef __cplusplus
}
#endif

#endif
