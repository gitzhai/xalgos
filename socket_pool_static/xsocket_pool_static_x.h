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

#ifndef XSOCKET_CLIENT_POOL_STATIC_X_H
#define XSOCKET_CLIENT_POOL_STATIC_X_H

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

typedef struct XSSocket XSSocket_T;
typedef struct XSSocket* XSSocket_PT;

struct XSSocket {
    int socket;
    time_t last_used;
};

typedef struct XSSocketPool XSSocketPool_T;
typedef struct XSSocketPool* XSSocketPool_PT;

struct XSSocketPool {
    XDList_PT sockets;

    int pool_size;
    int cur_pool_size;

    bool is_running;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_cond_t shutdown_cond;

    char server_ip[INET6_ADDRSTRLEN];
    char server_port[6];
};

#endif
#endif
