#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Command.h"

#define MAX_ARGS 513
#define EXP_VAR "$$"

/**
 * @brief  Checks a given string for the occurrence of EXP_VAR. If EXP_VAR is
 *         found, replaces the occurrence with the given replacement string.
 * 
 * @param  char *str: Original string to check and expand.
 * @param  char *repstr: Replacement string if occurrence is found.
 * @return char *newstr: Address of newly allocated string. NULL if an
 *         occurrence was not found.
 */
char *expand_string(char* str, char* repstr)
{
    if (!strstr(str, EXP_VAR)) {
        return NULL;
    }

    char *expstr = NULL;
    char temp[256];
    strcpy(temp, str);

    // Loop until every occurrence is replaced. temp initially holds str. In
    // subsequent iterations, temp holds expstr to check if it still contains
    // EXP_VAR.
    do {
        if (expstr) {
            free(expstr);
        }
        char *occurrence = strstr(temp, EXP_VAR);
        int index = occurrence - temp;

        // Allocate and build the new expanded string.
        expstr = calloc(strlen(temp) - strlen(EXP_VAR) + strlen(repstr) + 1,
            sizeof(char));
        strncpy(expstr, temp, index);
        strcat(expstr, repstr);
        strcat(expstr, &temp[index + strlen(EXP_VAR)]);
        strcpy(temp, expstr);
    } while (strstr(temp, EXP_VAR));

    return expstr;
}

/**
 * @brief  Helper function that tokenizes a line into an array of strings
 *         delimited by spaces. Expands the string if an expansion variable is
 *         found.
 * 
 * @param  char *line: A line in the specified format.
 * @param  char **tokens: Array of strings to store the tokens.
 * @param  char *repstr: Replacement string if EXP_VAR is found in a token.
 * @return void: None
 */
void tokenize_line(char *line, char **tokens, char *repstr)
{
    // Get the first token.
    char *save_ptr;
    char *token = strtok_r(line, " \n", &save_ptr);

    if (!token || token[0] == '#') {
        return;
    }

    // Get the rest of the tokens. Expand string if EXP_VAR is found.
    int i = 0;
    while (token != NULL) {
        if (strstr(token, EXP_VAR)) {
            tokens[i] = expand_string(token, repstr);
        } else {
            tokens[i] = calloc(strlen(token) + 1, sizeof(char));
            strcpy(tokens[i], token);
        }
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
    int i = 0;

    // Process optional args.
    while (tokens[i] 
            && i < MAX_ARGS
            && strcmp(tokens[i], "<") != 0
            && strcmp(tokens[i], ">") != 0
            && strcmp(tokens[i], "&") != 0) {
        args[i] = calloc(strlen(tokens[i]) + 1, sizeof(char));
        strcpy(args[i], tokens[i]);
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
 * @brief  Takes a line in the format: 
 * 
 *         command [arg1 arg2 ...] [< input_file] [> output_file] [&]
 * 
 *         and constructs a Command with that data.
 * 
 * @param  char* line: a line in the specified format.
 * @param  int pid: pid used for variable expansion.
 * @return Command*: Pointer to a constructed Command.
 */
Command *get_command(char* line, int pid)
{
    char *tokens[MAX_ARGS] = { NULL };
    char *cmd = NULL, *input = NULL, *output = NULL;
    char *args[MAX_ARGS] = { NULL };
    int bg = 0;

    char pidstr[11];
    sprintf(pidstr, "%d", pid);

    tokenize_line(line, tokens, pidstr);

    // tokens[0] is the required command.
    if (!tokens[0]) {
        return NULL;
    }
    cmd = calloc(strlen(tokens[0]) + 1, sizeof(char));
    strcpy(cmd, tokens[0]);

    process_optional_commands(tokens, args, &input, &output, &bg);

    Command *command = command_create(cmd, args, input, output, bg, pid);

    free(cmd);
    if (input) { free(input); }
    if (output) { free(output); }
    int i = 0, j = 0;

    while (tokens[i] && i < MAX_ARGS) {
        free(tokens[i]);
        i++;
    }
    while (args[j] && j < MAX_ARGS) {
        free(args[j]);
        j++;
    }

    return command;
}
