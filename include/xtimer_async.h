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

#ifndef XTIMER_ASYNC_INCLUDED
#define XTIMER_ASYNC_INCLUDED

#if defined(__linux__)

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct XAsyncTimer* XAsyncTimer_PT;

// Define the callback type for timer tasks
typedef void (*XAsyncTimer_Task_Callback)(void* data);

extern XAsyncTimer_PT  xasync_timer_init    (int tick_time, int wheel_size, long start_time, int capacity, int pool_size);
extern bool            xasync_timer_add     (XAsyncTimer_PT timer, XAsyncTimer_Task_Callback callback, void* data, long delay, long recurrence, char* id);
extern bool            xasync_timer_cancel  (XAsyncTimer_PT timer, char* id);
extern void            xasync_timer_destroy (XAsyncTimer_PT timer);

#ifdef __cplusplus
}
#endif

#endif
#endif
