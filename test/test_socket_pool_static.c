
#if defined(__linux__)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#include "../thread_pool_static/xthread_pool_static_x.h"
#include "../socket_pool_static/xsocket_pool_static_x.h"
#include "../include/xalgos.h"

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

void test_xsocket_pool_static() {

    //Initialization of Socket Pool
    {
        XSSocketPool_PT pool = xssocketpool_init(5, "127.0.0.1", "8080");
        xassert(pool != NULL);
        xassert(pool->pool_size == 5);
        xassert(pool->cur_pool_size == 5);
        xassert(pool->is_running == true);
        xssocketpool_destroy(pool);
    }

    //Initialization with Invalid Parameters
    {
        XSSocketPool_PT pool = xssocketpool_init(5, "invalid_ip", "8080");
        xassert(pool != NULL);
        xassert(pool->pool_size == 5);
        xassert(pool->cur_pool_size == 0);
        xassert(pool->is_running == true);
        xssocketpool_destroy(pool);
    }

    //Acquire Socket from Pool
    {
        XSSocketPool_PT pool = xssocketpool_init(5, "127.0.0.1", "8080");
        XSSocket_PT socket = xssocketpool_acquire(pool, 1000);
        xassert(socket != NULL);
        xassert(pool->cur_pool_size == 5);
        xssocketpool_release(pool, socket, false);
        xassert(pool->cur_pool_size == 5);
        xssocketpool_destroy(pool);
    }

    //Release Socket to Pool
    {
        XSSocketPool_PT pool = xssocketpool_init(5, "127.0.0.1", "8080");
        XSSocket_PT socket = xssocketpool_acquire(pool, 1000);
        xssocketpool_release(pool, socket, false);
        xassert(pool->cur_pool_size == 5);
        xssocketpool_destroy(pool);
    }

    //Destroy Socket Pool
    {
        XSSocketPool_PT pool = xssocketpool_init(5, "127.0.0.1", "8080");
        xssocketpool_destroy(pool);
        // No direct assertions, but ensure no memory leaks or crashes
    }

    //Acquire Socket with Timeout
    {
        XSSocketPool_PT pool = xssocketpool_init(2, "127.0.0.1", "8080");
        XSSocket_PT socket1 = xssocketpool_acquire(pool, 1000);
        XSSocket_PT socket2 = xssocketpool_acquire(pool, 1000);
        xassert(socket1 != NULL);
        xassert(socket2 != NULL);
        XSSocket_PT socket3 = xssocketpool_acquire(pool, 1000);
        xassert(socket3 == NULL);
        xssocketpool_release(pool, socket1, false);
        xssocketpool_release(pool, socket2, false);
        xssocketpool_destroy(pool);
    }

    //Release and Close Socket Directly
    {
        XSSocketPool_PT pool = xssocketpool_init(5, "127.0.0.1", "8080");
        XSSocket_PT socket = xssocketpool_acquire(pool, 1000);
        xssocketpool_release(pool, socket, true);
        xassert(pool->cur_pool_size == 4);
        xssocketpool_destroy(pool);
    }

    //Acquire Invalid Socket
    {
        XSSocketPool_PT pool = xssocketpool_init(5, "127.0.0.1", "8080");
        XSSocket_PT socket = xssocketpool_acquire(pool, 1000);
        close(socket->socket);  // Invalidate the socket
        xssocketpool_release(pool, socket, false);
        XSSocket_PT new_socket = xssocketpool_acquire(pool, 1000);
        xassert(new_socket != NULL);
        xssocketpool_release(pool, new_socket, false);
        xssocketpool_destroy(pool);
    }

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }
}

#endif
