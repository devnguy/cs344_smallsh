#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_ARGS 512

struct command_t
{
    char *cmd;
    char *args[MAX_ARGS];
    char *input;
    char *output;
    int bg;
    int argc;
};

/**
 * @brief  Allocates and initializes a Command.
 * 
 * @param  char *cmd: Command to run
 * @param  char *args[]: Array of args to run command (optional)
 * @param  char *input: Input redirection (optional)
 * @param  char *output: Output redirection (optional)
 * @param  int bg: Whether to run the command in foreground or background (1
 *                 by default, optional)
 * 
 * @return Command*: Pointer to the constructed Command
 */
struct command_t *command_create(char *cmd, char *args[], char *input, 
    char *output, int bg)
{
    struct command_t *command = 
        (struct command_t*)malloc(sizeof(struct command_t));

    command->cmd = calloc(strlen(cmd) + 1, sizeof(char));
    strcpy(command->cmd, cmd);
    command->argc = 1;

    if (input) {
        command->input = calloc(strlen(input) + 1, sizeof(char));
        strcpy(command->input, input);
        command->argc++;
    } else {
        command->input = NULL;
    }

    if (output) {
        command->output = calloc(strlen(output) + 1, sizeof(char));
        strcpy(command->output, output);
        command->argc++;
    } else {
        command->output = NULL;
    }

    command->bg = bg;

    for (int i = 0; i < MAX_ARGS; i++) {
        if (args[i]) {
            char *arg = calloc(strlen(args[i]) + 1, sizeof(char));
            strcpy(arg, args[i]);
            command->args[i] = arg;
            command->argc++;
        } else {
            command->args[i] = NULL;
        }
    }
    return command;
}

/**
 * @brief  Prints the data members of the Command struct for testing.
 * 
 * @param  Command* command: Pointer to the command to display
 * 
 * @return void
 */
void command_print(struct command_t *command)
{
    printf("%s, %s, %s, %d\n", command->cmd, command->input, command->output,
        command->bg);

    int i = 0;
    while (command->args[i] && i < MAX_ARGS) {
        printf("%s ", command->args[i]);
        i++;
    }
    printf("\n");
}

/**
 * @brief  Frees the allocated memory of the Command.
 * 
 * @param  Command* command: Pointer to the command to free.
 * 
 * @return void
 */
void command_destroy(struct command_t *command)
{
    free(command->cmd);
    free(command->input);
    free(command->output);
    int i = 0;
    while (command->args[i] && i < MAX_ARGS) {
        free(command->args[i]);
        i++;
    }
    free(command);
}

// Get method that returns the command cmd
char* command_get_cmd(struct command_t *command)
{
    return command->cmd;
}

// Get method that returns the command input
char* command_get_input(struct command_t *command)
{
    return command->input;
}

// Get method that returns the command output
char* command_get_output(struct command_t *command)
{
    return command->output;
}

// Get method that returns the command bg
int command_get_fg(struct command_t *command)
{
    return command->bg;
}

// Get method that returns the command args
char** command_get_args(struct command_t *command)
{
    return command->args;
}

// Get method that returns the number of command args
int command_get_argc(struct command_t *command)
{
    return command->argc;
}
