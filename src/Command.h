#ifndef COMMAND_H
#define COMMAND_H

typedef struct command_t Command;
struct command_t *command_create(char *cmd, char *args[], char *input, 
    char *output, int bg, int pid);
void command_cd(Command *command);
void command_exec(Command *command);
void command_print(Command *command);
void command_destroy(Command *command);

char* command_get_cmd(Command *command);
char** command_get_args(Command *command);
char* command_get_input(Command *command);
char* command_get_output(Command *command);
int command_get_fg(Command *command);
int command_get_argc(Command *command);
int command_get_pid(Command *command);
void command_set_pid(Command *command, int pid);

#endif