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

#include <stdlib.h>
#include <stdio.h>

#include "../include/xexcept.h"

XExcept_Frame_T *xgp_except_stack = NULL;

void xexcept_raise(const XExcept_T *pe, const char *file, int line) {
    XExcept_Frame_T *p = xgp_except_stack;

    if (!pe) {
        if (file && (0 < line)) {
            fprintf(stderr, "pointer to XExcept_T is empty, raised at %s:%d\n", file, line);
        }

        fprintf(stderr, "aborting...\n");
        fflush(stderr);

        abort();
    }

    /* when no upper catch call function, abort the program */
    if (!p) {
        fprintf(stderr, "Uncaught exception");

        if (pe->reason) {
            fprintf(stderr, " %s", pe->reason);
        }
        else {
            fprintf(stderr, " at 0x%p", pe);
        }

        if (file && (0 < line)) {
            fprintf(stderr, " raised at %s:%d\n", file, line);
        }

        fprintf(stderr, "aborting...\n");
        fflush(stderr);

        abort();
    }

    p->exception = pe;
    p->file      = file;
    p->line      = line;

    xgp_except_stack = xgp_except_stack->prev;

    longjmp(p->env, XExcept_raised);
}
