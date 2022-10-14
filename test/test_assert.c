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

#include "../include/xalgos.h"

void test_xassert() {

    /* assert */
    {
        /* assert true */
        xassert(true);

        /* assert false */
        {
            bool except = false;

            XEXCEPT_TRY
                xassert(false);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }
    }

    /* assert_false */
    {
        /* assert_false false */
        xassert_false(false);

        /* assert_false true */
        {
            bool except = false;

            XEXCEPT_TRY
                xassert_false(true);
            XEXCEPT_ELSE
                except = true;
            XEXCEPT_END_TRY

            xassert(except);
        }
    }

}
