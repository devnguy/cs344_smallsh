#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Command.h"

/**
 * @brief  Changes the current working directory.
 * 
 * @param  Command *command: The command to execute.
 * @return void
 */
void run_cd(Command *command)
{
    int result;
    // chdir to HOME if no args.
    if (!(command_get_args(command)[1])) {
        char *path = calloc(strlen(getenv("HOME")) + 1, sizeof(char));
        strcpy(path, getenv("HOME"));
        result = chdir(path);
        free(path);
    } else {
        result = chdir(command_get_args(command)[1]);
    }

    // TESTING
    char path[256];
    printf("pwd: %s\n", getcwd(path, 256));
    // END TESTING

    if (result == -1) {
        perror("Error");
    }
}