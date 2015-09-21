#include "nid_core.h"
#include "nid_event.h"

#ifdef NID_HAVE_EPOLL

extern int32_t nid_event_create(void) {
    return epoll_create1(0);
}

extern int32_t nid_event_add(int32_t handle, int32_t fd, int32_t events) {
    struct epoll_event env;
    env.events = (uint32_t)events | EPOLLET;
    env.data.fd = fd;
    
    return epoll_ctl(handle, EPOLL_CTL_ADD, fd, &env);
}

extern int32_t nid_event_mod(int32_t handle, int32_t fd, int32_t events) {
    struct epoll_event env;
    env.events = (uint32_t)events | EPOLLET;
    env.data.fd = fd;
    
    return epoll_ctl(handle, EPOLL_CTL_MOD, fd, &env);
}

extern int32_t nid_event_del(int32_t handle, int32_t fd, int32_t events) {
    /**
     * epoll_ctl(2) BUGS
     * In kernel versions before 2.6.9,
     * the EPOLL_CTL_DEL operation required a non-NULL pointer in event,
     * even though this argument is ignored.
     */
    struct epoll_event env;
    return epoll_ctl(handle, EPOLL_CTL_DEL, fd, &env);
}

#elif defined(NID_HAVE_KQUEUE)

extern int32_t nid_event_create(void) {
    return kqueue();
}

extern int32_t nid_event_add(int32_t handle, int32_t fd, int32_t events) {
    struct kevent env[1];
    EV_SET(env, fd, events, EV_ADD | EV_CLEAR, 0, 0, 0);
    return kevent(handle, env, 1, NULL, 0, NULL);
}

extern int32_t nid_event_mod(int32_t handle, int32_t fd, int32_t events) {
    return nid_event_add(handle, fd, events);
}

extern int32_t nid_event_del(int32_t handle, int32_t fd, int32_t events) {
    struct kevent env[1];
    EV_SET(env, fd, events, EV_DELETE, 0, 0, 0);
    return kevent(handle, env, 1, NULL, 0, NULL);
}

#endif

extern int32_t nid_event_close(int32_t handle) {
    return close(handle);
}

extern int32_t nid_setnonblock(int32_t fd) {
    int32_t opts = fcntl(fd, F_GETFL);
    if (NID_ERROR == opts) {
        return opts;
    }

    opts |= O_NONBLOCK;
    if (NID_ERROR == fcntl(fd, F_SETFL, opts)) {
        return opts;
    }

    return NID_OK;
}
