#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Command.h"

#define MAX_ARGS 513


/**
 * @brief  Helper function that tokenizes a line into an array of strings
 *         delimited by spaces.
 * 
 * @param  char *line: A line in the specified format.
 * @param  char **tokens: Array of strings to store the tokens.
 * @return void: None
 */
void tokenize_line(char *line, char **tokens)
{
    // Get the first token.
    char *save_ptr;
    char *token = strtok_r(line, " \n", &save_ptr);

    if (!token || token[0] == '#') {
        return;
    }

    // Get the rest of the tokens.
    int i = 0;
    while (token != NULL) {
        tokens[i] = calloc(strlen(token) + 1, sizeof(char));
        strcpy(tokens[i], token);
        token = strtok_r(NULL, " \n", &save_ptr);
        i++;
    }
}

/**
 * @brief  Helper function that checks for optional commands given an array of
 *         strings.
 * 
 * @param  char **tokens: Array of strings to process.
 * @param  char **args: Array of strings to store optional args.
 * @param  char **input: Address of a string to store optional input
 *             redirection.
 * @param  char **output: Address of a string to store optional output 
 *             redirection.
 * @param  int *output: Address of an int to store optional background process
 *             command.
 * @return void: None
 */
void process_optional_commands(char **tokens, char **args, char **input,
    char **output, int *bg)
{
    // tokens[0] was already processed, so start i at 1.
    int i = 1;
    int j = 0;

    // Process optional args.
    while (tokens[i] 
            && i < MAX_ARGS
            && strcmp(tokens[i], "<") != 0
            && strcmp(tokens[i], ">") != 0
            && strcmp(tokens[i], "&") != 0) {
        args[j] = calloc(strlen(tokens[i]) + 1, sizeof(char));
        strcpy(args[j], tokens[i]);
        j++;
        i++;
    }

    // Process optional input/output redirection and background process based
    // on presence of special characters.
    while (tokens[i]) {
        if (strcmp(tokens[i], "<") == 0) {
            i++;
            *input = calloc(strlen(tokens[i]) + 1, sizeof(char));
            strcpy(*input, tokens[i]);
        }
        else if (strcmp(tokens[i], ">") == 0) {
            i++;
            *output = calloc(strlen(tokens[i]) + 1, sizeof(char));
            strcpy(*output, tokens[i]);
        }
        else if (strcmp(tokens[i], "&") == 0 && tokens[i + 1] == NULL) {
            *bg = 1;
        }
        i++;
    }
}

/**
 * @brief  Takes a line from in the format: 
 * 
 *         command [arg1 arg2 ...] [< input_file] [> output_file] [&]
 * 
 *         and constructs a Command with that data.
 * 
 * @param  char* line: a line in the specified format.
 * @return Command*: Pointer to a constructed Command.
 */
Command *get_command(char* line)
{
    char *tokens[MAX_ARGS] = { NULL };
    char *cmd = NULL, *input = NULL, *output = NULL;
    char *args[MAX_ARGS] = { NULL };
    int bg = 0;

    tokenize_line(line, tokens);

    // tokens[0] is the required command.
    if (!tokens[0]) {
        return NULL;
    }
    cmd = calloc(strlen(tokens[0]) + 1, sizeof(char));
    strcpy(cmd, tokens[0]);

    process_optional_commands(tokens, args, &input, &output, &bg);

    Command *command = command_create(cmd, args, input, output, bg);

    free(cmd);
    if (input) { free(input); }
    if (output) { free(output); }
    int i = 0, j = 0;

    while (tokens[i] && i < MAX_ARGS) {
        printf("%s\n", tokens[i]);
        free(tokens[i]);
        i++;
    }
    while (args[j] && j < MAX_ARGS) {
        free(args[j]);
        j++;
    }

    return command;
}
