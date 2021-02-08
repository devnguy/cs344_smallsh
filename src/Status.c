#include <stdlib.h>
#include <stdio.h>

struct status_t
{
    int status_no;
    int is_in_fg_mode;
};

struct status_t *status_create()
{
    struct status_t *status = 
        (struct status_t*)malloc(sizeof(struct status_t));
    status->status_no = 0;
    status->is_in_fg_mode = 0;
    return status;
}

void status_print(struct status_t *status)
{
    if (status->status_no <= 1) {
        printf("exit value %d\n", status->status_no);
    } else {
        printf("terminated by signal %d\n", status->status_no);
    }
    fflush(stdout);
}

int status_get_status_no(struct status_t *status)
{
    return status->status_no;
}

void status_set_status_no(struct status_t *status, int status_no)
{
    status->status_no = status_no;
}

int status_get_is_in_fg_mode(struct status_t *status)
{
    return status->is_in_fg_mode;
}

void status_enable_fg_mode(struct status_t *status)
{
    status->is_in_fg_mode = 1;
}

void status_disable_fg_mode(struct status_t *status)
{
    status->is_in_fg_mode = 0;
}
