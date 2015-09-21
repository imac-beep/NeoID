#ifndef NEOID_NID_SERVER_H
#define NEOID_NID_SERVER_H

struct server_env {
    int32_t server_sockfd;
    int32_t eventfd;
    uint16_t port;
    uint8_t machine_id;
};

int32_t nid_server_start(uint16_t port, uint32_t backlog, uint8_t machine_id);

#endif //NEOID_NID_SERVER_H
