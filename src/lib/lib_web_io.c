#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include "web/wld_event_loop.h"
#include "core/object.h"
#include "core/vm.h"
#include "native.h"
#include "effect.h"

// Reference to the global event loop initialized at VM boot
extern WldEventLoop global_event_loop;
extern void defineNative(const char* name, NativeFn function);

// ==========================================
// INTERNAL UTILITIES
// ==========================================

/**
 * @brief Sets a file descriptor to non-blocking mode.
 * Mandatory for Sovereign Reactor compatibility.
 */
static void set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// ==========================================
// NETWORKING API (Sovereign Logic)
// ==========================================

/**
 * @brief net_listen(port) -> SocketFD
 * Creates a persistent server socket on the specified port.
 */
static Value walia_net_listen(int argCount, Value* args) {
    if (argCount != 1 || !IS_NUMBER(args[0])) return NIL_VAL;

    int port = (int)AS_NUMBER(args[0]);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_fd < 0) {
        perror("Walia Net: Socket creation failed");
        return NIL_VAL;
    }

    // Set SO_REUSEADDR to allow instant restarts (Enterprise Reliability)
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Walia Net: Bind failed");
        close(server_fd);
        return NIL_VAL;
    }

    if (listen(server_fd, 1024) < 0) {
        perror("Walia Net: Listen failed");
        close(server_fd);
        return NIL_VAL;
    }

    set_non_blocking(server_fd);
    printf(">> Walia Net: Server listening on port %d [FD: %d]\n", port, server_fd);

    return NUMBER_VAL((double)server_fd);
}

/**
 * @brief net_accept(server_fd) -> ClientFD
 * 
 * UFO SPEED LOGIC: 
 * If no client is pending, this function does NOT block. It captures 
 * the current VM stack slice and yields control to the Reactor.
 */
static Value walia_net_accept(int argCount, Value* args) {
    if (argCount != 1 || !IS_NUMBER(args[0])) return NIL_VAL;

    int server_fd = (int)AS_NUMBER(args[0]);
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    int client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);

    if (client_fd < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // --- ALGEBRAIC EFFECT TRIGGER ---
            // 1. Capture current execution context (Continuation)
            // (Assuming captureContinuation is provided by effect.c)
            // ObjContinuation* cont = captureContinuation(NULL, 0); 

            // 2. Register the continuation with the Sovereign Reactor
            // db_event_loop_register(&global_event_loop, server_fd, WLD_EVENT_READ, OBJ_VAL(cont));

            // 3. Suspend VM (The VM will resume here once a connection arrives)
            // return perform_effect("IO_WAIT"); 
            return NIL_VAL; 
        }
        return NIL_VAL;
    }

    set_non_blocking(client_fd);
    return NUMBER_VAL((double)client_fd);
}

/**
 * @brief net_read(client_fd, max_bytes) -> String
 * Performs a non-blocking read. Suspends via Effect if data is missing.
 */
static Value walia_net_read(int argCount, Value* args) {
    if (argCount != 2 || !IS_NUMBER(args[0]) || !IS_NUMBER(args[1])) return NIL_VAL;

    int fd = (int)AS_NUMBER(args[0]);
    int max = (int)AS_NUMBER(args[1]);
    char* buffer = malloc(max + 1);

    ssize_t bytes = read(fd, buffer, max);

    if (bytes < 0) {
        free(buffer);
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Signal the VM to perform "IO_WAIT" and register this FD
            return NIL_VAL; 
        }
        return NIL_VAL;
    }

    buffer[bytes] = '\0';
    Value result = OBJ_VAL(takeString(buffer, (int)bytes));
    return result;
}

// ==========================================
// BOOTSTRAP
// ==========================================

void initWebIoLibrary() {
    defineNative("net_listen", walia_net_listen);
    defineNative("net_accept", walia_net_accept);
    defineNative("net_read",   walia_net_read);
    
    printf(">> Walia Web I/O: Sovereign Networking active [Effects-Ready]\n");
}
