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

#ifndef XTIMER_ASYNCX_INCLUDED
#define XTIMER_ASYNCX_INCLUDED

#if defined(__linux__)

#include "../include/xtimer_async.h"

typedef struct TimerTask* TimerTask_PT;
struct TimerTask {
    XAsyncTimer_Task_Callback callback; // Function to call when the task expires
    void*           data;               // Data to pass to the callback function

    long            expiration;         // Expiration time
    long            recurrence;         // Recurrence interval in milliseconds, 0 for non-recurring tasks

    char            id[64];             // Unique identifier for the task
};

typedef struct TimingWheel* TimingWheel_PT;
struct TimingWheel {
    XDList_PT*      buckets;            // Array of buckets, each bucket is a doubly linked list
    int             wheel_size;         // Number of buckets in the timing wheel

    int             tick_time;          // Duration of each tick : basic time span of the current time wheel
    long            interval;           // Total duration of the timing wheel : tick_time * wheel_size

    long            current_time;       // current time of the time wheel : integer multiple of tick_time
                                        // currentTime divides the entire time wheel into expired and unexpired portions
                                        // The time slot currently pointed to by currentTime is also considered part of the expired portion

    int             task_count;         // Current number of scheduled tasks in this wheel

    TimingWheel_PT  overflow_wheel;     // Overflow timing wheel for tasks that exceed the current wheel's interval
};

struct XAsyncTimer {
    TimingWheel_PT   wheel;             // Timing wheel

    int              task_count;        // Current number of scheduled tasks
    int              capacity;          // Maximum number of scheduled tasks

    pthread_t        timer_trigger;     // Timer trigger thread
    XSThreadPool_PT  timeout_handlers;  // Thread pool for processing timeout tasks

    bool             running;           // Flag to control the running state of the timer thread
    pthread_mutex_t  lock;              // Mutex for thread safety
};

#endif
#endif
