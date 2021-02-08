#ifndef COMMAND_H
#define COMMAND_H

#include "Status.h"
#include "DynArr.h"

typedef struct command_t Command;
struct command_t *command_create(char *cmd, char *args[], char *input, 
    char *output, int bg, int pid);
void command_cd(Command *command);
void command_exec(Command *command);
void command_fork(Command *command, Status *status, DynArr *bg_pids);
void command_resolve_process(int *pid, Status *status);
void command_print(Command *command);
void command_destroy(Command *command);

// Getters and setters.
char* command_get_cmd(Command *command);
char** command_get_args(Command *command);
char* command_get_input(Command *command);
char* command_get_output(Command *command);
int command_get_bg(Command *command);
int command_get_argc(Command *command);
int command_get_pid(Command *command);
void command_set_pid(Command *command, int pid);

#endif