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

#ifndef XTS_DLISTX_INCLUDED
#define XTS_DLISTX_INCLUDED

#if defined(__linux__)

#include <pthread.h>

#include "../include/xlist_d_raw.h"
#include "../include/xlist_d_thread.h"

struct XTS_DList {
    XRDList_PT       head, tail;

    int              size;
    int              capacity;     /* maximum element numbers : -1 means unlimited */

    pthread_mutex_t  mutex;
    pthread_cond_t   not_empty;
};

#endif
#endif
