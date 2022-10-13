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

#ifndef XIARITH_INCLUDED
#define XIARITH_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* O(1) */
extern int xiarith_max     (int x, int y);
extern int xiarith_min     (int x, int y);

/* O(1) */
extern int xiarith_div     (int x, int y);
extern int xiarith_mod     (int x, int y);

/* O(1) */
extern int xiarith_ceiling (int x, int y);
extern int xiarith_floor   (int x, int y);

/* O(lgN) */
extern int xiarith_lg      (int x);
extern int xiarith_pow2    (int x);

#ifdef __cplusplus
}
#endif

#endif
