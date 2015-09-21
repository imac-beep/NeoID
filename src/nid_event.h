#ifndef NEOID_NID_EVENT_H
#define NEOID_NID_EVENT_H

extern int32_t nid_event_create(void);

extern int32_t nid_event_add(int32_t handle, int32_t fd, int32_t events);
extern int32_t nid_event_mod(int32_t handle, int32_t fd, int32_t events);
extern int32_t nid_event_del(int32_t handle, int32_t fd, int32_t events);

extern int32_t nid_event_close(int32_t handle);

extern int32_t nid_setnonblock(int32_t fd);

#endif //NEOID_NID_EVENT_H
