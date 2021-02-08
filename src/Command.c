#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#include "Command.h"
#include "DynArr.h"
#include "sig_handlers.h"

#define MAX_ARGS 513

struct command_t
{
    char *cmd;
    char *args[MAX_ARGS];
    char *input;
    char *output;
    int bg;
    int argc;
    int pid;
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
    char *output, int bg, int pid)
{
    struct command_t *command = 
        (struct command_t*)malloc(sizeof(struct command_t));

    command->cmd = calloc(strlen(cmd) + 1, sizeof(char));
    strcpy(command->cmd, cmd);
    command->argc = 0;

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
    command->pid = pid;

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
 * @brief  Driver function that handles forking to execute a shell command,
 *         monitoring the forked child process, delegating processes to
 *         foreground/background, and updating status. This function does too
 *         much and has too many dependencies.
 * 
 * @param  Command *command: Pointer to the command to display
 * @param  Status *status: Pointer to the status to maintain
 * @param  DynArr *bg_pids: Array of pids that are currently running in the bg
 * 
 * @return void
 */
void command_fork(struct command_t *command, Status *status, DynArr *bg_pids) 
{
    int *child_status_ptr = status_get_child_status_ptr(status);
    pid_t fork_pid = fork();
    if (fork_pid < 0) {
        perror("fork()");
    }
    // Configure child process signal handling before executing. Process will
    // exit due to exec or exit function.
    if (fork_pid == 0) {
        ignore_sigtstp();
        if (command->bg) {
            ignore_sigint();
        } else {
            install_default_sigint_handler();
        }
        command_exec(command);
        // Clean up before exiting if exec returned due to some failure.
        free(status);
        command_destroy(command);
        dynarr_destroy(bg_pids);
        exit(EXIT_FAILURE);
    }
    // Background process. waitpid will be called with WNOHANG on bg_pids
    // before next prompt.
    if (command->bg && !status_get_is_in_fg_mode(status)) {
        printf("background pid is %d\n", fork_pid);
        fflush(stdout);
        dynarr_append(bg_pids, (int)fork_pid);
    // Wait for foreground child process.
    } else {
        command_resolve_process(&fork_pid, status);
    }
}

/**
 * @brief  Handles input/output redirection if provided before calling execvp.
 *         *Note: Because execvp is called, the calling process will 
 *                terminate on success. On error, this function returns so
 *                error handling can be done by calling function.
 * 
 * @param  Command *command: The command to execute.
 * 
 * @return void
 */
void command_exec(struct command_t *command)
{
    int src_fd;
    int out_fd;
    int result;

    // Redirect stdin/stdout to "/dev/null" for background processes.
    if (command->bg && !command->input) {
        command->input = "/dev/null";
    }
    if (command->bg && !command->output) {
        command->output = "/dev/null";
    }

    // Open source file if provided.
    if (command->input) {
        src_fd = open(command->input, O_RDONLY);
        if (src_fd == -1) {
            perror(command->input);
            return;
        }
        // Redirect stdin to source file.
        result = dup2(src_fd, 0);
        if (result == -1) {
            perror("dup2()");
            return;
        }
    }
    // Open output file if provided.
    if (command->output) {
        out_fd = open(command->output, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (out_fd == -1) {
            perror(command->output);
            return;
        }
        // Redirect stdout to output file.
        result = dup2(out_fd, 1);
        if (result == -1) {
            perror("dup2()");
            return;
        }
    }
    execvp(command->cmd, command->args);
    perror(command->cmd);
}


/**
 * @brief  Takes a process id and waits for it to terminate. Updates the
 *         status based on how the process terminated.
 * 
 * @param  int *pid: pid to wait on
 * @param  Status *status: The Status struct to update.
 * 
 * @return void
 */
void command_resolve_process(int *pid, Status *status)
{
    int *child_status_ptr = status_get_child_status_ptr(status);

    // During waitpid, SIGTSTP signal can be sent to foreground process. 
    // Updating the child pid allows the handler to know which process to
    // continue to wait on.
    status_set_child_pid(status, *pid);
    *pid = waitpid(*pid, child_status_ptr, 0);

    if (WIFEXITED(*child_status_ptr)) {
        status_set_status_no(status, WEXITSTATUS(*child_status_ptr));
    } else {
        status_set_status_no(status, WTERMSIG(*child_status_ptr));
        status_print(status);
    }
}

/**
 * @brief  Changes the current working directory.
 * 
 * @param  Command *command: The command to execute.
 * 
 * @return void
 */
void command_cd(struct command_t *command)
{
    int result;
    // chdir to HOME if no args.
    if (!(command->args[1])) {
        char *path = calloc(strlen(getenv("HOME")) + 1, sizeof(char));
        strcpy(path, getenv("HOME"));
        result = chdir(path);
        free(path);
    } else {
        result = chdir(command->args[1]);
    }

    if (result == -1) {
        perror("Error");
    }
}

/**
 * @brief  Prints the data members of the Command struct for testing.
 * 
 * @param  Command *command: Pointer to the command to display
 * 
 * @return void
 */
void command_print(struct command_t *command)
{
    printf("%s, %s, %s, %d, %d\n", command->cmd, command->input, 
        command->output, command->bg, command->pid);

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
 * @param  Command *command: Pointer to the command to free.
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
int command_get_bg(struct command_t *command)
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

// Get method that returns the pid
int command_get_pid(struct command_t *command)
{
    return command->pid;
}

// Set method that sets the pid
void command_set_pid(struct command_t *command, int pid)
{
    command->pid = pid;
}
