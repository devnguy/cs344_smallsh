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
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

#include "Command.h"
#include "Status.h"
#include "DynArr.h"
#include "get_command.h"
#include "cmd_constants.h"
#include "sig_handlers.h"

#define _GNU_SOURCE

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void show_prompt()
{
    printf(": ");
    fflush(stdout);
}

void check_bg_pids_status(DynArr *bg_pids, int *child_status, Status *status) {
    int size = dynarr_get_size(bg_pids);
    int i = 0;
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

void kill_bg_pids(DynArr *bg_pids, int *status) {
    int size = dynarr_get_size(bg_pids);
    int i = 0;
    while (i < size) {
        if (waitpid(dynarr_get_pos(bg_pids, i), status, WNOHANG)) {
            dynarr_remove(bg_pids, i);
            size = dynarr_get_size(bg_pids);
        } else {
            kill(dynarr_get_pos(bg_pids, i), SIGKILL);
            i++;
        }
    }
}

int main(int argc, char *argv[])
{
    Status *status = status_create();
    DynArr *bg_pids = dynarr_create(10);

    ignore_sigint();
    install_sgtstp_handler();

    while (1) {
        char *current_line = NULL;
        size_t len = 0;
        pid_t pid = getpid();
        int child_status = 0;
        
        send_status_to_sig_handlers(status);

        check_bg_pids_status(bg_pids, &child_status, status);
        show_prompt();
        getline(&current_line, &len, stdin);

        Command *command = get_command(current_line, (int)pid);
        free(current_line);

        if (!command) {
            continue;
        } else if (strcmp(command_get_cmd(command), CMD_EXIT) == 0) {
            kill_bg_pids(bg_pids, &child_status);
            command_destroy(command);
            break;
        } else if (strcmp(command_get_cmd(command), CMD_CD) == 0) {
            command_cd(command);
        } else if (strcmp(command_get_cmd(command), CMD_STATUS) == 0) {
            status_print(status);
        } else {
            // command_print(command);
            pid_t fork_pid = fork();
            if (fork_pid < 0) {
                perror("Fork failed");
            }
            // Child process.
            if (fork_pid == 0) {
                install_default_sigint_handler();
                command_exec(command);
                // This only runs if exec returned due to failure.
                free(status);
                command_destroy(command);
                dynarr_destroy(bg_pids);
                exit(EXIT_FAILURE);
            }
            // Parent process.
            if (command_get_bg(command) && !status_get_is_in_fg_mode(status)) {
                // background process
                printf("background pid is %d\n", fork_pid);
                fflush(stdout);
                dynarr_append(bg_pids, (int)fork_pid);
                // fork_pid = waitpid(fork_pid, &child_status, WNOHANG);
            } else {
                fork_pid = waitpid(fork_pid, &child_status, 0);
                // printf("waitpid: %d, wifsignaled = %d\n", fork_pid, WIFSIGNALED(child_status));
                if (WIFEXITED(child_status)) {
                    // printf("child %d exited normally with status %d\n", fork_pid, WEXITSTATUS(child_status));
                    status_set_status_no(status, WEXITSTATUS(child_status));
                } else {
                    // printf("child %d exited abnormally with status %d\n", fork_pid, WTERMSIG(child_status));
                    status_set_status_no(status, WTERMSIG(child_status));
                    status_print(status);
                }
            }
            // status_set_status_no(status, child_status);
        }

        command_destroy(command);
    }
    dynarr_destroy(bg_pids);
    free(status);
    return EXIT_SUCCESS;
}
