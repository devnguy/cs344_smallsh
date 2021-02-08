#include <stdlib.h>
#include <stdio.h>

struct status_t
{
    int status_no;
    int is_in_fg_mode;
    int child_status;
    int child_pid;
};

/**
 * @brief  Allocates and initializes a maintainable Status.
 * 
 * @param  None
 * 
 * @return Status*: Pointer to the constructed Status
 */
struct status_t *status_create()
{
    struct status_t *status = 
        (struct status_t*)malloc(sizeof(struct status_t));
    status->status_no = 0;
    status->is_in_fg_mode = 0;
    status->child_status = 0;
    status->child_pid = 0;
    return status;
}

/**
 * @brief  Prints the status based on the value of status_no.
 * 
 * @param  Status *status: The status to display.
 * 
 * @return void
 */
void status_print(struct status_t *status)
{
    if (status->status_no <= 1) {
        printf("exit value %d\n", status->status_no);
    } else {
        printf("terminated by signal %d\n", status->status_no);
    }
    fflush(stdout);
}

// Get method that returns the status number.
int status_get_status_no(struct status_t *status)
{
    return status->status_no;
}

// Set method that sets the status number.
void status_set_status_no(struct status_t *status, int status_no)
{
    status->status_no = status_no;
}

// Get method that returns whether or not the status is_in_fg_mode.
int status_get_is_in_fg_mode(struct status_t *status)
{
    return status->is_in_fg_mode;
}

// Changes is_in_fg_mode to true.
void status_enable_fg_mode(struct status_t *status)
{
    status->is_in_fg_mode = 1;
}

// Changes is_in_fg_mode to false.
void status_disable_fg_mode(struct status_t *status)
{
    status->is_in_fg_mode = 0;
}

// Returns the address of the child_status.
int *status_get_child_status_ptr(struct status_t *status)
{
    return &status->child_status;
}

// Returns the child_status.
int status_get_child_status(struct status_t *status)
{
    return status->child_status;
}

// Returns the child_pid.
int status_get_child_pid(struct status_t *status)
{
    return status->child_pid;
}

// Sets the child_pid.
void status_set_child_pid(struct status_t *status, int child_pid)
{
    status->child_pid = child_pid;
}
