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

void test_xmem() {

    /* xmem.c*/
    {
        /* xmem_malloc */
        {
            /* nbytes < 0 */
            {
                bool except = false;

                XEXCEPT_TRY
                    xmem_malloc(-1, "a", 0);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }

            /* nbytes == 0 */
            {
                bool except = false;

                XEXCEPT_TRY
                    xmem_malloc(0, "a", 0);
                XEXCEPT_ELSE
                    except = true;
                XEXCEPT_END_TRY

                xassert(except);
            }


        }

        /* xmem_calloc */
        {
        }

        /* xmem_resize */
        {
        }

        /* xmem_resize0 */
        {
        }

        /* xmem_free */
        {
        }

        /* XMEM_MALLOC */
        {
        }

        /* XMEM_CALLOC */
        {
        }

        /* XMEM_FREE */
        {
        }

        /* XMEM_NEW */
        {
        }

        /* XMEM_NEWN */
        {
        }

        /* XMEM_NEW0 */
        {
        }

        /* XMEM_NEW0N */
        {
        }

        /* XMEM_RESIZE */
        {
        }

        /* XMEM_RESIZE0 */
        {
        }

        /* xmem_leak */
        {
        }
    }


    /* xmemchk.c*/
    {
        /* xmem_malloc */
        {
        }

        /* xmem_calloc */
        {
        }

        /* xmem_resize */
        {
        }

        /* xmem_resize0 */
        {
        }

        /* xmem_free */
        {
        }

        /* XMEM_MALLOC */
        {
        }

        /* XMEM_CALLOC */
        {
        }

        /* XMEM_FREE */
        {
        }

        /* XMEM_NEW */
        {
        }

        /* XMEM_NEWN */
        {
        }

        /* XMEM_NEW0 */
        {
        }

        /* XMEM_NEW0N */
        {
        }

        /* XMEM_RESIZE */
        {
        }

        /* XMEM_RESIZE0 */
        {
        }

        /* xmem_leak */
        {
        }
    }

}
