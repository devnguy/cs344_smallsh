/******************************************************************************
** Program name: Smallsh
** Author:       Huy Nguyen
** Date:         02/01/21
** Description:  This is an implementation of a small shell that contains a
**               subset of features of well-known shells, such as bash.
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#include "Command.h"
#include "Status.h"
#include "DynArr.h"
#include "get_command.h"
#include "cmd_constants.h"
#include "sig_handlers.h"

#define _GNU_SOURCE

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/**
 * @brief  Prints the shell prompt.
 * 
 * @param  None
 * 
 * @return void
 */
void show_prompt()
{
    printf(": ");
    fflush(stdout);
}

/**
 * @brief  Checks to see if currently running background processes have
 *         finished and updates the status based on how the background process
 *         ended.
 * 
 * @param  DynArr *bg_pids: Maintained array of pids of currently running
 *         background processes.
 * @param  int *child_status: Address to store child process status.
 * @param  Status *status: Status to update based on child process 
 *         termination.
 * 
 * @return void
 */
void check_bg_pids_status(DynArr *bg_pids, Status *status) {
    int size = dynarr_get_size(bg_pids);
    int i = 0;
    int *child_status = status_get_child_status_ptr(status);
    while (i < size) {
        if (waitpid(dynarr_get_pos(bg_pids, i), child_status, WNOHANG)) {
            printf("background pid %d is done: ", dynarr_get_pos(bg_pids, i));
            fflush(stdout);
            if (WIFEXITED(*child_status)) {
                status_set_status_no(status, WEXITSTATUS(*child_status));
            } else {
                status_set_status_no(status, WTERMSIG(*child_status));
            }
            status_print(status);
            fflush(stdout);
            dynarr_remove(bg_pids, i);
            size = dynarr_get_size(bg_pids);
        } else {
            i++;
        }
    }
}

/**
 * @brief  Kills every background process that is currently running.
 * 
 * @param  DynArr *bg_pids: Maintained array of pids of currently running
 *         background processes.
 * @param  int *status: Address to store process status.
 * 
 * @return void
 */
void kill_bg_pids(DynArr *bg_pids, Status *status) {
    int *child_status_ptr = status_get_child_status_ptr(status);
    int size = dynarr_get_size(bg_pids);
    int i = 0;
    while (i < size) {
        if (waitpid(dynarr_get_pos(bg_pids, i), child_status_ptr, WNOHANG)) {
            dynarr_remove(bg_pids, i);
            size = dynarr_get_size(bg_pids);
        } else {
            kill(dynarr_get_pos(bg_pids, i), SIGKILL);
            i++;
        }
    }
}

/**
 * @brief  Gets a command from the user and dispatches it accordingly.
 * 
 * @return int: Exit status
 */
int main(int argc, char *argv[])
{
    Status *status = status_create();
    DynArr *bg_pids = dynarr_create(10);

    ignore_sigint();
    install_sigtstp_handler();

    while (1) {
        char *current_line = NULL;
        size_t len = 0;
        pid_t pid = getpid();

        // Status is stored in sig_handlers.c as a global variable. Update
        // that variable with each iteration.
        send_status_to_sig_handlers(status);

        check_bg_pids_status(bg_pids, status);

        // Prompt for and get a command from user.
        show_prompt();
        getline(&current_line, &len, stdin);
        Command *command = get_command(current_line, (int)pid);
        free(current_line);

        // Dispatch the given command.
        if (!command) {
            continue;
        } else if (strcmp(command_get_cmd(command), CMD_EXIT) == 0) {
            kill_bg_pids(bg_pids, status);
            command_destroy(command);
            break;
        } else if (strcmp(command_get_cmd(command), CMD_CD) == 0) {
            command_cd(command);
        } else if (strcmp(command_get_cmd(command), CMD_STATUS) == 0) {
            status_print(status);
        } else {
            command_fork(command, status, bg_pids);
        }
        command_destroy(command);
        command = NULL;
    }
    dynarr_destroy(bg_pids);
    free(status);
    return EXIT_SUCCESS;
}
