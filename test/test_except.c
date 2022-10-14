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

#include <stdio.h>
#include <stddef.h>

#include "../include/xalgos.h"

const XExcept_T g_except_failed  = { "test except failed" };
const XExcept_T g_except_failed2 = { "test except 2 failed" };

static void test_xexcept_return() {
    XEXCEPT_TRY
        printf("test case TRY - RETURN\n\n");
        XEXCEPT_RETURN;
        printf("test case TRY - RETURN : should never reach here\n");
    XEXCEPT_CATCH(g_except_failed)
        printf("test case TRY - RETURN : CATCH : should never reach here\n");
    XEXCEPT_ELSE
        printf("test case TRY - RETURN : ELSE : should never reach here\n");
    XEXCEPT_END_TRY

    printf("test case TRY - RETURN : should never reach here\n");
}


void test_xexcept() {

    /* test case : TRY - END_TRY */
    XEXCEPT_TRY
        printf("test case : TRY - END_TRY\n\n");
    XEXCEPT_END_TRY

    /* test case : TRY - FINALLY */
    XEXCEPT_TRY
        printf("test case : TRY - FINALLY - END_TRY\n");
    XEXCEPT_FINALLY
        printf("test case : TRY - FINALLY - END_TRY : finally executed\n\n");
    XEXCEPT_END_TRY

    /* test case : TRY - RAISE - CATCH */
    XEXCEPT_TRY
        printf("test case : TRY - RAISE - CATCH\n");
        XEXCEPT_RAISE(g_except_failed);
    XEXCEPT_CATCH(g_except_failed)
        printf("test case : TRY - RAISE - CATCH : except get\n\n");
    XEXCEPT_END_TRY

    /* test case : TRY - RAISE - CATCH - ELSE */
    XEXCEPT_TRY
        printf("test case : TRY - RAISE - CATCH - ELSE\n");
        XEXCEPT_RAISE(g_except_failed);
    XEXCEPT_CATCH(g_except_failed2)
        printf("test case : TRY - RAISE - CATCH - ELSE : except not get\n");
    XEXCEPT_ELSE
        printf("test case : TRY - RAISE - CATCH - ELSE : except else get\n\n");
    XEXCEPT_END_TRY


    /* test case : TRY - (TRY - RAISE - CATCH  - RERAISE - FINALLY) - CATCH - FINALLY */
    XEXCEPT_TRY
        XEXCEPT_TRY
            printf("test case : TRY - (TRY - RAISE - CATCH  - RERAISE - FINALLY) - CATCH - FINALLY\n");
            XEXCEPT_RAISE(g_except_failed);
        XEXCEPT_CATCH(g_except_failed)
            printf("test case : TRY - (TRY - RAISE - CATCH  - RERAISE - FINALLY) - CATCH - FINALLY : except get, reraise it here\n\n");
            XEXCEPT_RERAISE;
        XEXCEPT_FINALLY
            printf("EXCEPT_RERAISE called in last EXCEPT_CATCH, so call EXCEPT_FINALLY have no meaning here, code will not executed here !\n\n");
        XEXCEPT_END_TRY
    XEXCEPT_CATCH(g_except_failed)
        printf("test case : TRY - (TRY - RAISE - CATCH  - RERAISE - FINALLY) - CATCH - FINALLY : except catch get again\n");
    XEXCEPT_FINALLY
        printf("test case : TRY - (TRY - RAISE - CATCH  - RERAISE - FINALLY) - CATCH - FINALLY : except finally get\n\n");
    XEXCEPT_END_TRY


    /* the extended codes for test case : TRY - (TRY - RAISE - CATCH  - RERAISE - FINALLY) - CATCH - FINALLY : */
    do {
        volatile int   except_flag = 0;
        XExcept_Frame_T except_frame;
        except_frame.prev = xgp_except_stack;
        except_frame.file = NULL;
        except_frame.line = 0;
        except_frame.exception = NULL;
        xgp_except_stack = &except_frame;
        except_flag = setjmp(except_frame.env);
        if (except_flag == XExcept_entered) {

            do {
                volatile int   except_flag = 0;
                XExcept_Frame_T except_frame;
                except_frame.prev = xgp_except_stack;
                except_frame.file = NULL;
                except_frame.line = 0;
                except_frame.exception = NULL;
                xgp_except_stack = &except_frame;
                except_flag = setjmp(except_frame.env);
                if (except_flag == XExcept_entered) {

                    printf("TRY - (TRY -\n");
                    xexcept_raise(&g_except_failed, __FILE__, __LINE__);

                    if (except_flag == XExcept_entered) xgp_except_stack = xgp_except_stack->prev;
                }
                else if (except_frame.exception == &(g_except_failed)) {
                    except_flag = XExcept_handled;

                    printf("TRY - (TRY - RAISE - CATCH -\n");
                    xexcept_raise(except_frame.exception, except_frame.file, except_frame.line);

                    if (except_flag == XExcept_entered) xgp_except_stack = xgp_except_stack->prev;
                }
                {
                    if (except_flag == XExcept_entered)
                        except_flag = XExcept_finalized;

                    printf("TRY - (TRY - RAISE - CATCH  - RERAISE - FINALLY : should never reach here\n");

                    if (except_flag == XExcept_entered) xgp_except_stack = xgp_except_stack->prev;
                }
                if (except_flag == XExcept_raised) XEXCEPT_RERAISE;
            } while (0);


            if (except_flag == XExcept_entered) xgp_except_stack = xgp_except_stack->prev;
        }
        else if (except_frame.exception == &(g_except_failed)) {
            except_flag = XExcept_handled;

            printf("TRY - (TRY - RAISE - CATCH  - RERAISE - FINALLY) - CATCH -\n");

            if (except_flag == XExcept_entered) xgp_except_stack = xgp_except_stack->prev;
        }
        {
            if (except_flag == XExcept_entered)
                except_flag = XExcept_finalized;

            printf("TRY - (TRY - RAISE - CATCH  - RERAISE - FINALLY) - CATCH - FINALLY\n");

            if (except_flag == XExcept_entered) xgp_except_stack = xgp_except_stack->prev;
        }
        if (except_flag == XExcept_raised) XEXCEPT_RERAISE;
    } while (0);

    /* test case : TRY - (TRY - RAISE - CATCH)  - (TRY - RAISE - CATCH - RERAISE) - CATCH */
    XEXCEPT_TRY
        XEXCEPT_TRY
            printf("test case : TRY - (TRY - RAISE - CATCH)  - (TRY - RAISE - CATCH - RERAISE) - CATCH\n");
            XEXCEPT_RAISE(g_except_failed);
        XEXCEPT_CATCH(g_except_failed)
            printf("test case : TRY - (TRY - RAISE - CATCH)  - (TRY - RAISE - CATCH - RERAISE) - CATCH : except get for first (TRY - RAISE - CATCH)\n\n");
        XEXCEPT_END_TRY

        XEXCEPT_TRY
            XEXCEPT_RAISE(g_except_failed);
        XEXCEPT_CATCH(g_except_failed)
            printf("test case : TRY - (TRY - RAISE - CATCH)  - (TRY - RAISE - CATCH - RERAISE) - CATCH : except got for second (TRY - RAISE - CATCH)\n\n");
            XEXCEPT_RERAISE;
        XEXCEPT_END_TRY

    XEXCEPT_CATCH(g_except_failed)
        printf("test case : TRY - (TRY - RAISE - CATCH)  - (TRY - RAISE - CATCH - RERAISE) - CATCH : except get in outer catch\n\n");
    XEXCEPT_END_TRY


    /* test case : TRY - RETURN */
    test_xexcept_return();
}
