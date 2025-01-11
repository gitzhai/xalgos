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

#ifndef XTHREAD_SEMX_INCLUDED
#define XTHREAD_SEMX_INCLUDED

#if defined(__linux__)

#include <pthread.h>
#include "../include/xthread_sem.h"

struct XThread_Sem {
	pthread_mutex_t  mutex;
	pthread_cond_t   cond;
	int v;
};

#endif
#endif
