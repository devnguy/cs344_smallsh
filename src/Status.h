#ifndef STATUS_H
#define STATUS_H

typedef struct status_t Status;
Status *status_create();
void status_print(Status *status);
int status_get_status_no(Status *status);
void status_set_status_no(Status *status, int status_no);
int status_get_is_in_fg_mode(Status *status);
void status_enable_fg_mode(Status *status);
void status_disable_fg_mode(Status *status);
int *status_get_child_status_ptr(Status *status);
int status_get_child_status(Status *status);
int status_get_child_pid(Status *status);
void status_set_child_pid(Status *status, int child_pid);

#endif