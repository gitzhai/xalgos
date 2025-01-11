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

#ifndef XSOCKET_POOL_STATIC_INCLUDED
#define XSOCKET_POOL_STATIC_INCLUDED

#if defined(__linux__)

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XSSocket XSSocket_T;
typedef struct XSSocket* XSSocket_PT;

typedef struct XSSocketPool* XSSocketPool_PT;

XSSocketPool_PT  xssocketpool_init      (int pool_size, char* server_ip, char* server_port);
XSSocket_PT      xssocketpool_acquire   (XSSocketPool_PT pool, long long timeout_ms);
void             xssocketpool_release   (XSSocketPool_PT pool, XSSocket_PT socket, bool close_directly);
void             xssocketpool_destroy   (XSSocketPool_PT pool);
int              xssocketpool_socket_fd (XSSocket_PT socket);

#ifdef __cplusplus
}
#endif

#endif
#endif
