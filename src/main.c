/******************************************************************************
** Program name: Smallsh
** Author:       Huy Nguyen
** Date:         02/01/21
** Description:  This is an implementation of a small shell that contains a
**               subset of features of well-known shells, such as bash.
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "Command.h"
#include "get_command.h"

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

        show_prompt();
        getline(&current_line, &len, stdin);
        // get_command
        Command *command = get_command(current_line);
        // if parent
        //   wait for child
        // else
        //   execute child command
        if (command) {
            command_print(command);
        }
        free(current_line);
    }
    return EXIT_SUCCESS;
}
