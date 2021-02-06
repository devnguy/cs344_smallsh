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

#include "Command.h"
#include "get_command.h"
#include "cmd_constants.h"
#include "cd.h"

#define _GNU_SOURCE

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void show_prompt()
{
    printf(": ");
}

int main(int argc, char *argv[])
{

    while (1) {
        // prompt
        char *current_line = NULL;
        size_t len = 0;
        pid_t pid = getpid();

        show_prompt();
        getline(&current_line, &len, stdin);
        // get_command
        Command *command = get_command(current_line, (int)pid);

        // if (command) { // TESTING
        //     command_print(command);
        // }
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
            // char cwd[256];
            // printf("%s\n", getcwd(cwd, 256));
            run_cd(command);
        } else if (strcmp(command_get_cmd(command), CMD_STATUS) == 0) {
            printf("status running\n");
            perror("error: ");
        } else {
            pid_t fork_pid = fork();
            if (fork_pid < 0) {
                perror("Fork failed");
            }
            if (fork_pid == 0) {
                execvp(command_get_cmd(command), command_get_args(command));
                perror("error: ");
                exit(EXIT_FAILURE);
            }
            // printf("I am the parent.\n");
            wait(NULL);
        }

        free(current_line);
        command_destroy(command);
    }
    return EXIT_SUCCESS;
}
