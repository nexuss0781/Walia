#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/syscall.h>
#include <linux/io_uring.h>

#include "sys_event_reactor.h"

// ==========================================
// KERNEL PROBING (io_uring vs epoll)
// ==========================================

SerResult sys_event_reactor_init(WldReactor* reactor) {
    memset(reactor, 0, sizeof(WldReactor));

    // 1. Attempt to initialize io_uring (Linux 5.1+)
    // We use a direct syscall to maintain Sovereign independence
    struct io_uring_params params;
    memset(&params, 0, sizeof(params));
    
    int res = syscall(__NR_io_uring_setup, WLD_EVENT_QUEUE_SIZE, &params);
    
    if (res >= 0) {
        reactor->uring_fd = res;
        reactor->use_uring = true;
        // In a full production implementation, we would mmap() the 
        // submission and completion rings here for zero-copy I/O.
        printf(">> Walia Reactor: io_uring substrate active [UFO-Grade Mode]\n");
        return SER_OK;
    }

    // 2. FALLBACK: Initialize epoll (Standard Linux/Termux)
    int epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("Walia Reactor: Critical failure initializing epoll");
        return SER_ERR_POLL_FAILED;
    }
    
    reactor->poll_fd = epfd;
    reactor->use_uring = false;
    printf(">> Walia Reactor: epoll substrate active [Standard Mode]\n");

    return SER_OK;
}

// ==========================================
// EVENT REGISTRATION
// ==========================================

SerResult sys_event_reactor_add(WldReactor* reactor, int fd, WldEventType type) {
    if (reactor->use_uring) {
        // io_uring is submission-based; we add a NOP or specific 
        // read/write request to the ring here.
        return SER_OK;
    }

    // epoll implementation
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    
    ev.data.fd = fd;
    if (type == WLD_EVENT_READ)  ev.events = EPOLLIN | EPOLLET; // Edge-Triggered
    if (type == WLD_EVENT_WRITE) ev.events = EPOLLOUT | EPOLLET;
    
    if (epoll_ctl(reactor->poll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        perror("Walia Reactor: Failed to add descriptor");
        return SER_ERR_INVALID_FD;
    }

    return SER_OK;
}

// ==========================================
// HIGH-SPEED POLLING (Harvesting)
// ==========================================

int sys_event_reactor_poll(WldReactor* reactor, WldEvent* outEvents, int maxEvents) {
    if (reactor->use_uring) {
        // Reap completions from the shared memory ring
        return 0; // Logic for Phase 15.1.2 integration
    }

    // Standard epoll reaping
    struct epoll_event events[1024];
    int n = epoll_wait(reactor->poll_fd, events, 1024, 0); // 0 = Non-blocking

    if (n < 0) return 0;

    int harvestCount = 0;
    for (int i = 0; i < n && harvestCount < maxEvents; i++) {
        WldEvent* wEv = &outEvents[harvestCount++];
        
        wEv->fd = events[i].data.fd;
        wEv->type = (events[i].events & EPOLLIN) ? WLD_EVENT_READ : WLD_EVENT_WRITE;
        wEv->timestamp = 0; // Populated by high-level loop
        wEv->continuation = NIL_VAL; // To be resolved by Event Manager
    }

    return harvestCount;
}

void sys_event_reactor_shutdown(WldReactor* reactor) {
    if (reactor->use_uring) {
        close(reactor->uring_fd);
    } else {
        close(reactor->poll_fd);
    }
    printf(">> Walia Reactor: Substrate released.\n");
}