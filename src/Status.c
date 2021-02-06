#include <stdlib.h>

struct status_t
{
    int status_no;
};

struct status_t *status_create()
{
    struct status_t *status = 
        (struct status_t*)malloc(sizeof(struct status_t));
    status->status_no = 0;
    return status;
}

int status_get_status_no(struct status_t *status)
{
    return status->status_no;
}

void status_set_status_no(struct status_t *status, int status_no)
{
    status->status_no = status_no;
}
