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

#ifndef XTHREAD_SEM_INCLUDED
#define XTHREAD_SEM_INCLUDED

#if defined(__linux__)

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XThread_Sem* XThread_Sem_PT;

extern XThread_Sem_PT xthread_sem_init     (int value);

extern bool           xthread_sem_post     (XThread_Sem_PT sem);
extern bool           xthread_sem_post_all (XThread_Sem_PT sem);

extern bool           xthread_sem_wait     (XThread_Sem_PT sem);

extern bool           xthread_sem_reset    (XThread_Sem_PT sem);
extern void           xthread_sem_free     (XThread_Sem_PT *sem);

#ifdef __cplusplus
}
#endif

#endif
#endif
