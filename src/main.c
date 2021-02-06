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

#include "Command.h"
#include "Status.h"
#include "get_command.h"
#include "cmd_constants.h"

#define _GNU_SOURCE

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void show_prompt()
{
    printf(": ");
}

int main(int argc, char *argv[])
{
    Status *status = status_create();

    while (1) {
        char *current_line = NULL;
        size_t len = 0;
        pid_t pid = getpid();

        show_prompt();
        getline(&current_line, &len, stdin);
        Command *command = get_command(current_line, (int)pid);

        int child_status = 0;

        if (!command) {
            free(current_line);
            continue;
        } else if (strcmp(command_get_cmd(command), CMD_EXIT) == 0) {
            // have a list of all child pids that are still running
            // loop through the list
            //   call kill and waitpid with WNOHANG for all items in it
            free(current_line);
            command_destroy(command);
            break;
        } else if (strcmp(command_get_cmd(command), CMD_CD) == 0) {
            command_cd(command);
        } else if (strcmp(command_get_cmd(command), CMD_STATUS) == 0) {
            printf("exit value %d\n", status_get_status_no(status));
        } else {
            pid_t fork_pid = fork();
            if (fork_pid < 0) {
                perror("Fork failed");
            }
            // Child process.
            if (fork_pid == 0) {
                command_exec(command);
            }
            // Parent process.
            fork_pid = waitpid(fork_pid, &child_status, 0);
            if (WIFEXITED(child_status)) {
                printf("child %d exited normally with status %d\n", fork_pid, WEXITSTATUS(child_status));
                status_set_status_no(status, WEXITSTATUS(child_status));
            } else {
                printf("child %d exited abnormally with status %d\n", fork_pid, WTERMSIG(child_status));
                status_set_status_no(status, WTERMSIG(child_status));
            }
            // status_set_status_no(status, child_status);
        }

        free(current_line);
        command_destroy(command);
    }
    free(status);
    return EXIT_SUCCESS;
}
