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

#ifndef XASSERT_INCLUDED
#define XASSERT_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#undef xassert

#ifdef XDEBUG

  #include "xexcept.h"

  extern void xassert       (int e);
  extern void xassert_false (int e);

  #define xassert(e)        ( (void)( (e)    || (XEXCEPT_RAISE(xg_assert_failed),0) ) )

  #define xassert_false(e)  ( (void)( (!(e)) || (XEXCEPT_RAISE(xg_assert_failed),0) ) )

#else

  #define xassert(e)       ((void)0)

  #define xassert_false(e) ((void)0)

#endif

#ifdef __cplusplus
}
#endif

#endif
