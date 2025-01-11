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

#ifndef XSOCKET_POOL_DYNAMIC_INCLUDED
#define XSOCKET_POOL_DYNAMIC_INCLUDED

#if defined(__linux__)

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <time.h>

typedef struct XDSocket* XDSocket_PT;
typedef struct XDSocketPool* XDSocketPool_PT;

XDSocketPool_PT  xdsocketpool_init       (int min_pool_size, int max_pool_size, time_t idle_timeout, char* server_ip, char* server_port);

XDSocket_PT      xdsocket_pool_acquire   (XDSocketPool_PT pool, long long timeout_ms);
int              xdsocketpool_socket_fd  (XDSocket_PT socket);

void             xdsocketpool_release    (XDSocketPool_PT pool, XDSocket_PT socket, bool close_directly);

void             xdsocketpool_destroy    (XDSocketPool_PT pool);

#ifdef __cplusplus
}
#endif

#endif
#endif
