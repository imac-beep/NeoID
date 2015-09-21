#ifndef NEOID_NEOID_H
#define NEOID_NEOID_H

#define NID_MACHINE_ID  0
#define NID_PORT        18989
#define NID_BACKLOG     1024

struct opt_data {
    nid_bool_t      deamonize;
    uint32_t        backlog;
    uint16_t        port;
    uint8_t         machine_id;
};

#endif //NEOID_NEOID_H
