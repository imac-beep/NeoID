#ifndef NEOID_NID_CORE_H
#define NEOID_NID_CORE_H

#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>

typedef int32_t nid_bool_t;

enum NID_BOOL {
    NID_FALSE   =       0,
#define NID_FALSE       NID_FALSE
    NID_TRUE
#define NID_TRUE        NID_TRUE
};

enum NID_STATUS {
    NID_ERROR   =       -1,
#define NID_ERROR       NID_ERROR
    NID_OK
#define NID_OK          NID_OK
};

#if defined(linux) || defined(__linux) || defined(__unix__)

#define NID_HAVE_EPOLL

#elif defined(unix) || defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))

#define NID_HAVE_KQUEUE

#elif defined(_WIN32) || defined(_WIN64)

#define NID_HAVE_IOCP

#endif


#ifdef NID_HAVE_EPOLL

#include <sys/epoll.h>
#define EVENT_READ      EPOLLIN
#define EVENT_ERR       EPOLLERR
#define EVENT_HUP       EPOLLHUP

#elif defined(NID_HAVE_KQUEUE)

#include <sys/event.h>
#define EVENT_READ      EVFILT_READ
#define EVENT_ERR       EV_ERROR
#define EVENT_HUP       EV_EOF

#endif


#endif //NEOID_NID_CORE_H
