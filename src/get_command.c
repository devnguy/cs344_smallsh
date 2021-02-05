#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Command.h"

#define MAX_ARGS 512


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

void process_optional_commands(char **tokens, char **args, char *input, char *output, int *bg)
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

    // Process optional input/output redirection and background process.
    while (tokens[i]) {
        if (strcmp(tokens[i], "<") == 0) {
            i++;
            input = calloc(strlen(tokens[i]) + 1, sizeof(char));
            strcpy(input, tokens[i]);
        }
        else if (strcmp(tokens[i], ">") == 0) {
            i++;
            output = calloc(strlen(tokens[i]) + 1, sizeof(char));
            strcpy(output, tokens[i]);
        }
        else if (strcmp(tokens[i], "&") == 0 && tokens[i + 1] == NULL) {
            *bg = 1;
        }
        i++;
    }
}

Command *get_command(char* line)
{
    char *tokens[MAX_ARGS + 1] = { NULL };
    char *cmd = NULL, *input = NULL, *output = NULL;
    char *args[MAX_ARGS] = { NULL };
    int bg = 0;

    tokenize_line(line, tokens);

    if (!tokens[0]) {
        return NULL;
    }
    cmd = calloc(strlen(tokens[0]) + 1, sizeof(char));
    strcpy(cmd, tokens[0]);

    process_optional_commands(tokens, args, input, output, &bg);

    Command *command = command_create(cmd, args, input, output, bg);

    free(cmd);
    // if (input) { free(input); }
    // if (output) { free(output); }
    // for (int k = 0; k < i; k++) {
    //     free(tokens[k]);
    // }
    // for (int l = 0; l < j; l++) {
    //     free(args[l]);
    // }

    return command;
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
Command *get_command_old(char* line)
{
    char *save_ptr;
    char *cmd = NULL;
    char *input = NULL;
    char *output = NULL;
    char *args[MAX_ARGS] = {NULL};
    int bg = 0;

    char *token = strtok_r(line, " \n", &save_ptr);

    if (!token) {
        return NULL;
    }

    cmd = calloc(strlen(token) + 1, sizeof(char));
    strcpy(cmd, token);

    // Parse the args into an array.
    int args_index = 0;
    while (save_ptr
            && save_ptr[0] != '&'
            && args_index < MAX_ARGS
            && !((save_ptr[0] == '<' || save_ptr[0] == '>') && save_ptr[1] == ' ')) {
        token = strtok_r(NULL, " ", &save_ptr);
        args[args_index] = calloc(strlen(token) + 1, sizeof(char));
        strcpy(args[args_index], token);
        args_index++;
    }

    // Get input redirection if it exists.
    if (save_ptr && save_ptr[0] == '<') {
        token = strtok_r(NULL, "< \n", &save_ptr);
        printf("input token: %s\n", token);
        input = calloc(strlen(token) + 1, sizeof(char));
        strcpy(input, token);
    }

    // Get output redirection if it exists.
    if (save_ptr && save_ptr[0] == '>') {
        token = strtok_r(NULL, "> \n", &save_ptr);
        printf("output token: %s\n", token);
        output = calloc(strlen(token) + 1, sizeof(char));
        strcpy(output, token);
    }

    // Get background command if it exists.
    if (save_ptr && save_ptr[0] == '&') {
        token = strtok_r(NULL, " \n", &save_ptr);
        if (strcmp(token, "&") == 0) {
            bg = 1;
            printf("bg token found\n");
        }
    }

    Command *command = command_create(cmd, args, input, output, bg);

    free(cmd);
    if (input) { free(input); }
    if (output) { free(output); }
    for (int i = 0; i < args_index; i++) {
        free(args[i]);
    }
    
    return command;
}