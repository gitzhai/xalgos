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

#if defined(__linux__)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "../include/xmem.h"
#include "xsocket_pool_static_x.h"

static XSSocket_PT create_socket(char* server_ip, char* server_port);
static bool is_socket_valid(XSSocket_PT socket);

XSSocketPool_PT xssocketpool_init(int pool_size, char* server_ip, char* server_port) {
    XSSocketPool_PT pool = (XSSocketPool_PT)XMEM_CALLOC(1, sizeof(XSSocketPool_T));
    if (!pool) {
        return NULL;
    }

    pool->pool_size = pool_size;
    pool->cur_pool_size = 0;

    pool->is_running = true;

    pool->sockets = xdlist_new();
    if (!pool->sockets) {
        XMEM_FREE(pool);
        return NULL;
    }

    if (pthread_mutex_init(&pool->mutex, NULL) != 0) {
        xdlist_free(&pool->sockets);
        XMEM_FREE(pool);
        return NULL;
    }
    if (pthread_cond_init(&pool->cond, NULL) != 0) {
        pthread_mutex_destroy(&pool->mutex);
        xdlist_free(&pool->sockets);
        XMEM_FREE(pool);
        return NULL;
    }
    if (pthread_cond_init(&pool->shutdown_cond, NULL) != 0) {
        pthread_cond_destroy(&pool->cond);
        pthread_mutex_destroy(&pool->mutex);
        xdlist_free(&pool->sockets);
        XMEM_FREE(pool);
        return NULL;
    }

    strncpy(pool->server_ip, server_ip, sizeof(pool->server_ip) - 1);
    pool->server_ip[sizeof(pool->server_ip) - 1] = '\0';
    strncpy(pool->server_port, server_port, sizeof(pool->server_port) - 1);
    pool->server_port[sizeof(pool->server_port) - 1] = '\0';

    for (int i = 0; i < pool_size; ++i) {
        XSSocket_PT socket = create_socket(server_ip, server_port);
        if (socket) {
            xdlist_push_back_repeat(pool->sockets, socket);  // Add socket to the list
            pool->cur_pool_size++;
        }
    }

    //printf("Total Sockets: %zu\n", pool->cur_pool_size);

    return pool;
}

static
XSSocket_PT create_socket(char* server_ip, char* server_port) {
    XSSocket_PT psocket = (XSSocket_PT)XMEM_CALLOC(1, sizeof(XSSocket_T));
    if (!psocket) {
        return NULL;
    }

    struct addrinfo hints, * res = NULL, * p = NULL;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // AF_UNSPEC to support both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM;

    int status;
    if ((status = getaddrinfo(server_ip, server_port, &hints, &res)) != 0) {
        //fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        XMEM_FREE(psocket);
        return NULL;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        void* addr;
        char* ipver;

        if (p->ai_family == AF_INET) {  // IPv4
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        }
        else {  // IPv6
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        //printf("  %s: %s\n", ipver, ipstr);

        psocket->socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (psocket->socket == -1) {
            continue;
        }

        if (connect(psocket->socket, p->ai_addr, p->ai_addrlen) == -1) {
            close(psocket->socket);
            continue;
        }

        break;  // Successfully connected
    }

    if (p == NULL) {
        //fprintf(stderr, "Failed to connect\n");
        XMEM_FREE(psocket);
        freeaddrinfo(res);
        return NULL;
    }

    freeaddrinfo(res);

    psocket->last_used = time(NULL);
    return psocket;
}

static
bool is_socket_valid(XSSocket_PT socket) {
    char buffer;
    int result = recv(socket->socket, &buffer, 1, MSG_PEEK | MSG_DONTWAIT);
    if (result == 0) {
        return false;  // Connection closed
    }
    else if (result == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
        return false;  // Error other than no data available
    }
    return true;
}

XSSocket_PT xssocketpool_acquire(XSSocketPool_PT pool, long long timeout_ms) {
    pthread_mutex_lock(&pool->mutex);

    if (!pool->is_running) {
        //printf("Socket pool is shut down.\n");
        pthread_mutex_unlock(&pool->mutex);
        return NULL;
    }

    XSSocket_PT socket = NULL;
    while (pool->cur_pool_size > 0) {
        socket = (XSSocket_PT)xdlist_pop_front(pool->sockets);
        if (!socket) {
            break;
        }

        if (is_socket_valid(socket)) {
            socket->last_used = time(NULL);
            pthread_mutex_unlock(&pool->mutex);
            return socket;
        }
        else {
            pool->cur_pool_size--;
            close(socket->socket);
            XMEM_FREE(socket);
            //printf("Invalid socket, discarded: %zu\n", pool->cur_pool_size);
        }
    }

    if (pool->cur_pool_size < pool->pool_size) {
        socket = create_socket(pool->server_ip, pool->server_port);
        if (socket) {
            xdlist_push_back_repeat(pool->sockets, socket);
            pool->cur_pool_size++;
            //printf("New Socket created: %zu\n", pool->cur_pool_size);
            pthread_mutex_unlock(&pool->mutex);
            return socket;
        }
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;

    int rc;
    while ((rc = pthread_cond_timedwait(&pool->cond, &pool->mutex, &ts)) == EINTR);

    if (rc == ETIMEDOUT) {
        //printf("Pool has no socket now: %zu\n", pool->cur_pool_size);
        pthread_mutex_unlock(&pool->mutex);
        return NULL;
    }

    socket = (XSSocket_PT)xdlist_pop_front(pool->sockets);
    if (!socket) {
        pthread_mutex_unlock(&pool->mutex);
        return NULL;
    }

    if (is_socket_valid(socket)) {
        socket->last_used = time(NULL);
        pthread_mutex_unlock(&pool->mutex);
        return socket;
    }
    else {
        pool->cur_pool_size--;
        close(socket->socket);
        XMEM_FREE(socket);
        //printf("Invalid socket, discarded: %zu\n", pool->cur_pool_size);
        pthread_mutex_unlock(&pool->mutex);
        return NULL;
    }
}

void xssocketpool_release(XSSocketPool_PT pool, XSSocket_PT socket, bool close_directly) {
    pthread_mutex_lock(&pool->mutex);

    if (pool->is_running && !close_directly) {
        socket->last_used = time(NULL);
        xdlist_push_back_repeat(pool->sockets, socket);
        pthread_cond_signal(&pool->cond);
    }
    else {
        pool->cur_pool_size--;
        close(socket->socket);
        XMEM_FREE(socket);
        //printf("Socket released: %zu\n", pool->cur_pool_size);
    }

    if (!pool->is_running && pool->cur_pool_size == 0) {
        pthread_cond_signal(&pool->shutdown_cond);
    }

    pthread_mutex_unlock(&pool->mutex);
}

void xssocketpool_destroy(XSSocketPool_PT pool) {
    pthread_mutex_lock(&pool->mutex);

    pool->is_running = false;
    pthread_cond_broadcast(&pool->cond);

    while (!xdlist_is_empty(pool->sockets)) {
        XSSocket_PT socket = (XSSocket_PT)xdlist_pop_front(pool->sockets);
        pool->cur_pool_size--;
        close(socket->socket);
        XMEM_FREE(socket);
    }

    // other sockets are using by someone else, waiting for release
    while (pool->cur_pool_size > 0) {
        pthread_cond_wait(&pool->shutdown_cond, &pool->mutex);
    }

    pthread_mutex_unlock(&pool->mutex);

    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cond);
    pthread_cond_destroy(&pool->shutdown_cond);

    xdlist_free(&pool->sockets);
    XMEM_FREE(pool);
}

int xssocketpool_socket_fd(XSSocket_PT socket) {
    return socket ? socket->socket : -1;
}

#endif
