#ifndef STATUS_H
#define STATUS_H

typedef struct status_t Status;
struct status_t *status_create();
int status_get_status_no(struct status_t *status);
void status_set_status_no(struct status_t *status, int status_no);

#endif