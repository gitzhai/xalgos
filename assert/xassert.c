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
*       <<C Interfaces and Implementations>> David R. Hanson, chapter 4
*/

#include "../include/xexcept.h"
#include "../include/xassert.h"

const XExcept_T xg_assert_failed = { "Assertion failed" };

void (xassert)(int e) {
    xassert(e);
}

void (xassert_false)(int e) {
    xassert_false(e);
}
