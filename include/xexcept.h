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

#ifndef XEXCEPT_INCLUDED
#define XEXCEPT_INCLUDED

#include <setjmp.h>
#include <stddef.h>

/* Note :
*   variables of XExcept_T must be global or static.
*
* Usage :
*   XEXCEPT_TRY
*     if(S0)
*       XEXCEPT_RETURN
*     S1
*   XEXCEPT_CATCH(e1)       - optional
*     S2
*   XEXCEPT_CATCH(e2)       - optional
*     S3
*   ...
*   XEXCEPT_CATCH(en)       - optional
*     Sn
*   XEXCEPT_ELSE            - optional
*     S4
*   XEXCEPT_FINALLY         - optional
*     S5
*   XEXCEPT_END_TRY;
*
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XExcept XExcept_T;
struct XExcept {
    char *reason;
};


/*******************internal logic start ***********************************/

typedef struct XExcept_Frame  XExcept_Frame_T;
struct XExcept_Frame {
    XExcept_Frame_T *prev;
    jmp_buf          env;
    const char      *file;
    int              line;
    const XExcept_T *exception;
};

enum { XExcept_entered=0, XExcept_raised, XExcept_handled, XExcept_finalized };

extern const XExcept_T        xg_assert_failed;      // defined in xassert.c
extern const XExcept_T        xg_memory_failed;      // defined in xmem.c
extern const XExcept_T        xg_arena_failed;       // defined in xarena.c

extern       XExcept_Frame_T *xgp_except_stack;

extern void  xexcept_raise    (const XExcept_T *pe, const char *file, int line);

#define XEXCEPT_RAISE(e)      xexcept_raise(&(e), __FILE__, __LINE__)

// Note : why can XEXCEPT_RERAISE throw the exception to outer layer ? 
//     In the inner layer, the xexcept_raise will change xgp_except_stack to point to outer layer except_frame at first,
//     so, after longjmp and return from inner layer setjmp,  the value of xgp_except_stack is the outer layer except_frame now,
//     next xexcept_raise will trigger longjump to return from outer layer setjmp !
#define XEXCEPT_RERAISE       xexcept_raise(except_frame.exception, except_frame.file, except_frame.line)

// Note :
//     just Use XEXCEPT_RETURN instead of return in XEXCEPT_TRY before calling XEXCEPT_RAISE if needed
#define XEXCEPT_RETURN        switch (xgp_except_stack = xgp_except_stack->prev,0) default: return

/*******************internal logic end ***********************************/




/*******************external interfaces start ***********************************/

#define XEXCEPT_TRY do {                     \
    volatile int    except_flag=0;           \
    XExcept_Frame_T except_frame;            \
    except_frame.prev = xgp_except_stack;    \
    except_frame.file = NULL;                \
    except_frame.line = 0;                   \
    except_frame.exception = NULL;           \
    xgp_except_stack = &except_frame;        \
    except_flag = setjmp(except_frame.env);  \
    if (except_flag == XExcept_entered) {


#define XEXCEPT_CATCH(e)                                                               \
        if (except_flag == XExcept_entered) xgp_except_stack = xgp_except_stack->prev; \
    } else if (except_frame.exception == &(e)) {                                       \
        except_flag = XExcept_handled;


#define XEXCEPT_ELSE                                                                   \
        if (except_flag == XExcept_entered) xgp_except_stack = xgp_except_stack->prev; \
    } else {                                                                           \
        except_flag = XExcept_handled;


#define XEXCEPT_FINALLY                                                                \
        if (except_flag == XExcept_entered) xgp_except_stack = xgp_except_stack->prev; \
    }                                                                                  \
    {                                                                                  \
        if (except_flag == XExcept_entered)                                            \
            except_flag = XExcept_finalized;


#define XEXCEPT_END_TRY                                                                \
        if (except_flag == XExcept_entered) xgp_except_stack = xgp_except_stack->prev; \
    }                                                                                  \
    if (except_flag == XExcept_raised) XEXCEPT_RERAISE;                                \
} while (0);

/*******************external interfaces end ***********************************/

#ifdef __cplusplus
}
#endif

#endif
