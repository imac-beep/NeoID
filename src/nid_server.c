#include "nid_core.h"
#include "nid_event.h"
#include "nid_genid.h"
#include "nid_server.h"

#define MAX_EVENTS 8

static int32_t nid_server_env_init(struct server_env *env);
static int32_t nid_server_accept_process(const struct server_env *env);
static int32_t nid_server_data_process(const struct server_env *env, int32_t fd);

static inline int32_t nid_server_event_loop(const struct server_env *env);

int32_t nid_server_start(uint16_t port, uint32_t backlog, uint8_t machine_id) {
    struct server_env env;
    env.port = port;
    env.machine_id = machine_id;
    if (NID_ERROR == nid_server_env_init(&env)) {
        return NID_ERROR;
    }

    if (NID_ERROR == listen(env.server_sockfd, backlog)) {
        perror("Listen");
        return NID_ERROR;
    }

    if (NID_ERROR == nid_genid_init(env.machine_id)) {
        perror("NeoID");
        return NID_ERROR;
    }

    if (NID_ERROR == nid_event_add(env.eventfd, env.server_sockfd, EVENT_READ)) {
        perror("EPOLL add");
        return NID_ERROR;
    }

    if (NID_ERROR == nid_server_event_loop(&env)) {
        return NID_ERROR;
    }

    return NID_OK;
}

static int32_t nid_server_env_init(struct server_env *env) {
    env->server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (NID_ERROR == nid_setnonblock(env->server_sockfd)) {
        perror("SetNonBlock");
        return NID_ERROR;
    }
    
    if (NID_ERROR == setsockopt(env->server_sockfd, SOL_SOCKET, SO_REUSEADDR, &(int32_t) {1}, sizeof(int32_t))) {
        perror("Setsockopt");
        return NID_ERROR;
    }
    
    struct sockaddr_in s_address;
    s_address.sin_family = AF_INET;
    s_address.sin_addr.s_addr = htonl(INADDR_ANY);
    s_address.sin_port = htons(env->port);
    
    if (NID_ERROR == bind(env->server_sockfd, (struct sockaddr *) &s_address, sizeof(s_address))) {
        perror("Bind");
        return NID_ERROR;
    }
    
    env->eventfd = nid_event_create();
    if (NID_ERROR == env->eventfd) {
        perror("EPOLL create");
        return NID_ERROR;
    }

    return NID_OK;
}

#ifdef NID_HAVE_EPOLL

static inline int32_t nid_server_event_loop(const struct server_env *env) {
    uint32_t i;
    int32_t e_fdnum;
    struct epoll_event events[MAX_EVENTS];

    while (NID_TRUE) {
        e_fdnum = epoll_wait(env->eventfd, events, MAX_EVENTS, -1);
        if (NID_ERROR == e_fdnum) {
            if (EINTR == errno) {
                continue;
            }
            perror("EVENT wait");
            return NID_ERROR;
        }

        for (i = 0; i < e_fdnum; i++) {
            if (events[i].events & EVENT_READ) {
                if (env->server_sockfd == events[i].data.fd) {
                    if (NID_ERROR == nid_server_accept_process(env)) {
                        return NID_ERROR;
                    }
                } else if (NID_ERROR == nid_server_data_process(env, events[i].data.fd)) {
                    return NID_ERROR;
                }
            } else {
                perror("EPOLL error");
                close(events[i].data.fd);
            }
        }
    }
}

#elif defined(NID_HAVE_KQUEUE)

static inline int32_t nid_server_event_loop(const struct server_env *env) {
    uint32_t i;
    int32_t e_fdnum;
    struct kevent events[MAX_EVENTS];

    while (NID_TRUE) {
        e_fdnum = kevent(env->eventfd, NULL, 0, events, MAX_EVENTS, NULL);
        if (NID_ERROR == e_fdnum) {
            if (EINTR == errno) {
                continue;
            }
            perror("EVENT wait");
            return NID_ERROR;
        }

        for (i = 0; i < e_fdnum; i++) {
            if (events[i].flags & EV_EOF) {
                close((int32_t) events[i].ident);
            } else if (events[i].flags & EVENT_READ) {
                if (env->server_sockfd == events[i].ident) {
                    if (NID_ERROR == nid_server_accept_process(env)) {
                        return NID_ERROR;
                    }
                } else if (NID_ERROR == nid_server_data_process(env, (int32_t) events[i].ident)) {
                    return NID_ERROR;
                }
            } else {
                perror("EVENT error");
                close((int32_t) events[i].ident);
            }
        }
    }
}

#endif

int32_t nid_server_accept_process(register const struct server_env *env) {
    struct sockaddr c_address;
    int32_t c_sockfd;
    socklen_t c_len;

    do {
        c_sockfd = accept(env->server_sockfd, &c_address, &c_len);
        if (NID_ERROR == c_sockfd) {
            if (EAGAIN != errno && EWOULDBLOCK != errno) {
                perror("Accept");
            }
            break;
        }

        if (NID_ERROR == nid_setnonblock(c_sockfd)) {
            perror("SetNonBlock");
            break;
        }

        if (NID_ERROR == nid_event_add(env->eventfd, c_sockfd, EVENT_READ)) {
            perror("EVENT add");
            return NID_ERROR;
        }
    } while (NID_TRUE);

    return NID_OK;
}

static char buffer[BUFSIZ];
static int64_t read_count;
static void *send_data;
static size_t send_data_len;
static nid_bool_t char_mode;
int32_t nid_server_data_process(register const struct server_env *env, register int32_t fd) {
    while ((read_count = read(fd, buffer, BUFSIZ)) > 0) {
        if (read_count < BUFSIZ) {
            break;
        }
    }

    if (1 == read_count && 'b' == buffer[0]) {
        send_data = &(uint64_t){nid_genid_new()};
        send_data_len = sizeof(uint64_t);
        char_mode = NID_FALSE;
    } else {
        sprintf(buffer, "%"PRIu64, nid_genid_new());
        send_data = buffer;
        send_data_len = strlen(buffer);
        char_mode = NID_TRUE;
    }

    if (write(fd, send_data, send_data_len) < 1) {
        perror("Write");
        return NID_ERROR;
    }

    if (char_mode) {
        close(fd);
    }
    return NID_OK;
}
