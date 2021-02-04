/******************************************************************************
** Program name: Smallsh
** Author:       Huy Nguyen
** Date:         02/01/21
** Description:  This is an implementation of a small shell that contains a
**               subset of features of well-known shells, such as bash.
******************************************************************************/
#include <stdio.h>

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
        char input[255];
        show_prompt();
        fgets(input, 255, stdin);
        // prompt
        // get_command
        // if parent
        //   wait for child
        // else
        //   execute child command
    }
    return EXIT_SUCCESS;
}
