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

#ifndef XSOCKET_CLIENT_POOL_DYNAMIC_X_H
#define XSOCKET_CLIENT_POOL_DYNAMIC_X_H

#if defined(__linux__)

#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../include/xlist_d.h"

typedef struct XDSocket XDSocket_T;
typedef struct XDSocket* XDSocket_PT;

struct XDSocket {
    int socket;
    time_t last_used;
};

typedef struct XDSocketPool XDSocketPool_T;
typedef struct XDSocketPool* XDSocketPool_PT;

struct XDSocketPool {
    XDList_PT sockets;

    int min_pool_size;
    int max_pool_size;
    int cur_pool_size;

    time_t idle_timeout;
    bool is_running;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_cond_t shutdown_cond;

    pthread_t status_monitor_thread;

    char server_ip[INET6_ADDRSTRLEN];
    char server_port[6];
};

#endif
#endif
