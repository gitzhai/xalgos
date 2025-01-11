
#if defined(__linux__)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../socket_pool_dynamic/xsocket_pool_dynamic_x.h"
#include "../include/xalgos.h"


// Running test_initialization_and_basic_functionality
static
void test_initialization_and_basic_functionality() {
    XDSocketPool_PT pool = xdsocketpool_init(2, 5, 10, "127.0.0.1", "8080");
    xassert(pool != NULL);

    XDSocket_PT socket1 = xdsocket_pool_acquire(pool, 1000);
    xassert(socket1 != NULL);
    xdsocketpool_release(pool, socket1, false);

    XDSocket_PT socket2 = xdsocket_pool_acquire(pool, 1000);
    xassert(socket2 != NULL);
    xdsocketpool_release(pool, socket2, false);

    xdsocketpool_destroy(pool);
}

// Running test_handling_invalid_sockets
static
void test_handling_invalid_sockets() {
    XDSocketPool_PT pool = xdsocketpool_init(2, 5, 10, "127.0.0.1", "8080");
    xassert(pool != NULL);

    XDSocket_PT socket1 = xdsocket_pool_acquire(pool, 1000);
    xassert(socket1 != NULL);
    close(socket1->socket);  // Manually close the socket to simulate an invalid socket
    xdsocketpool_release(pool, socket1, false);

    XDSocket_PT socket2 = xdsocket_pool_acquire(pool, 1000);
    xassert(socket2 != NULL);
    xdsocketpool_release(pool, socket2, false);

    xdsocketpool_destroy(pool);
}

// Running test_idle_timeout
static 
void test_idle_timeout() {
    XDSocketPool_PT pool = xdsocketpool_init(2, 5, 5, "127.0.0.1", "8080");
    xassert(pool != NULL);

    XDSocket_PT socket1 = xdsocket_pool_acquire(pool, 1000);
    xassert(socket1 != NULL);
    xdsocketpool_release(pool, socket1, false);

    sleep(6);  // Wait for idle timeout to exceed

    XDSocket_PT socket2 = xdsocket_pool_acquire(pool, 1000);
    xassert(socket2 != NULL);
    xdsocketpool_release(pool, socket2, false);

    xdsocketpool_destroy(pool);
}

// Running test_shutdown_with_active_sockets
static 
void test_shutdown_with_active_sockets() {
    XDSocketPool_PT pool = xdsocketpool_init(2, 5, 10, "127.0.0.1", "8080");
    xassert(pool != NULL);

    XDSocket_PT socket1 = xdsocket_pool_acquire(pool, 1000);
    xassert(socket1 != NULL);

    xdsocketpool_destroy(pool);  // Shutdown the pool while the socket is still acquired

    xdsocketpool_release(pool, socket1, false);  // Release the socket back to the pool
}

// Running test_force_terminate_on_shutdown
static
void test_force_terminate_on_shutdown() {
    XDSocketPool_PT pool = xdsocketpool_init(2, 5, 10, "127.0.0.1", "8080");
    xassert(pool != NULL);

    XDSocket_PT socket1 = xdsocket_pool_acquire(pool, 1000);
    xassert(socket1 != NULL);

    xdsocketpool_destroy(pool);  // Shutdown the pool and wait for the force termination timeout
}

static
void check_mem_leak(const void *ptr, long size, const char *file, int line, void *cl) {
    (*(int*)cl) += 1;
    printf("file:%s, line:%d, size:%ld\n", file, line, size);
}

void test_xsocket_pool_dynamic() {
    test_initialization_and_basic_functionality();
    test_handling_invalid_sockets();
    test_idle_timeout();
    // test_shutdown_with_active_sockets();
    // test_force_terminate_on_shutdown();

    {
        int count = 0;
        xmem_leak(check_mem_leak, &count);
        xassert(count == 0);
    }    
}

#endif
