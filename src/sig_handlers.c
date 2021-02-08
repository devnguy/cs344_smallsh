#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "Status.h"

int child_pid = 0;
Status *status = NULL;

void ignore_sigint()
{
    struct sigaction ignore_action = {0};
    ignore_action.sa_handler = SIG_IGN;
    sigaction(SIGINT, &ignore_action, NULL);
}

void handle_sigint(int signo)
{
    // char *message = "Caught SIGINT\n";
    // write(STDOUT_FILENO, message, 14);
    // printf("caught sigint. killing %d\n", child_pid);
    kill(child_pid, SIGINT);
}

void install_default_sigint_handler()
{
    struct sigaction default_action = {0};
    default_action.sa_handler = SIG_DFL;
    sigaction(SIGINT, &default_action, NULL);
}

void init_sigint_handler(int pid)
{
    child_pid = pid;
    struct sigaction sigint_action = {0};
    sigint_action.sa_handler = handle_sigint;
    sigaction(SIGINT, &sigint_action, NULL);
}







// A hack work-around function because args cannot be passed to custom signal
// handler. Update the status global variable.
void send_status_to_sig_handlers(Status *new_status)
{
    status = new_status;
}

void sigtstp_handler()
{
    if (!status_get_is_in_fg_mode(status)) {
        status_enable_fg_mode(status);
        char *message = "\nEntering foreground-only mode (& is now ignored)\n";
        write(STDOUT_FILENO, message, 50);
        fflush(stdout);
    } else {
        status_disable_fg_mode(status);
        char *message = "\nExiting foreground-only mode\n";
        write(STDOUT_FILENO, message, 30);
        fflush(stdout);
    }
}

void install_sgtstp_handler()
{
    struct sigaction sa_sigtstp = {0};
    sa_sigtstp.sa_handler = sigtstp_handler;
    sigfillset(&sa_sigtstp.sa_mask);
    sigaction(SIGTSTP, &sa_sigtstp, NULL);
}
