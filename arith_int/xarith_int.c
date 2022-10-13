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
*       <<C Interfaces and Implementations>> David R. Hanson, chapter 2
*/

#include "../include/xarith_int.h"

int xiarith_max(int x, int y) {
    return x < y ? y : x;
}

int xiarith_min(int x, int y) {
    return x < y ? x : y;
}

int xiarith_div(int x, int y) {
    if ((-13/5 == -2) && ((x < 0) != (y < 0)) && (x%y != 0))
        return x/y - 1;
    else
        return x/y;
}

int xiarith_mod(int x, int y) {
    if ((-13/5 == -2) && ((x < 0) != (y < 0)) && (x%y != 0))
        return x%y + y;
    else
        return x%y;
}

int xiarith_floor(int x, int y) {
    return xiarith_div(x, y);
}

int xiarith_ceiling(int x, int y) {
    return xiarith_div(x, y) + (x%y != 0);
}

int xiarith_lg(int x) {
    int k = 0;
    for (; x != 1; x >>= 1) {
        ++k;
    }
    return k;
}

int xiarith_pow2(int x) {
    int k = 1;
    for (; 1 <= x; --x) {
        k <<= 1;
    }
    return k;
}
