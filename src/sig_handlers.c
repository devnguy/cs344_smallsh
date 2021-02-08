#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "Status.h"
#include "sig_handlers.h"

Status *status = NULL;

/**
 * @brief  A hacky workaround that allows a signal handler to have access to
 *         a maintained Status. This is due to custom signal handlers being
 *         unable to take additional params.
 * 
 * @param  Status *new_status: The new status to replace the current global 
 *         variable.
 * 
 * @return void
 */
void send_status_to_sig_handlers(Status *new_status)
{
    status = new_status;
}

/**
 * @brief  Calls sigaction to ignore SIGINT.
 * 
 * @param  None
 * 
 * @return void
 */
void ignore_sigint()
{
    struct sigaction ignore_action = {0};
    ignore_action.sa_handler = SIG_IGN;
    sigaction(SIGINT, &ignore_action, NULL);
}

/**
 * @brief  Calls sigaction to ignore SIGTSTP.
 * 
 * @param  None
 * 
 * @return void
 */
void ignore_sigtstp()
{
    struct sigaction ignore_action = {0};
    ignore_action.sa_handler = SIG_IGN;
    sigaction(SIGTSTP, &ignore_action, NULL);
}

/**
 * @brief  Calls sigaction to set the SIGINT signal handler back to default.
 * 
 * @param  None
 * 
 * @return void
 */
void install_default_sigint_handler()
{
    struct sigaction default_action = {0};
    default_action.sa_handler = SIG_DFL;
    sigaction(SIGINT, &default_action, NULL);
}

/**
 * @brief  Calls sigaction to set the SIGTSTP signal handler to a custom
 *         function.
 * 
 * @param  None
 * 
 * @return void
 */
void install_sigtstp_handler()
{
    struct sigaction sa_sigtstp = {0};
    sa_sigtstp.sa_handler = sigtstp_handler;
    sa_sigtstp.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &sa_sigtstp, NULL);
}

/**
 * @brief  Custom SIGTSTP handler that toggles foreground-only mode status.
 *         Accesses and modifies the global "status" variable. This function
 *         will wait for the pid stored in status.
 * 
 * @param  None
 * 
 * @return void
 */
void sigtstp_handler()
{
    waitpid(status_get_child_pid(status), 
        status_get_child_status_ptr(status), 0);
    if (!status_get_is_in_fg_mode(status)) {
        status_enable_fg_mode(status);
        char *message = 
            "\nEntering foreground-only mode (& is now ignored)\n: ";
        write(STDOUT_FILENO, message, 52);
    } else {
        status_disable_fg_mode(status);
        char *message = "\nExiting foreground-only mode\n: ";
        write(STDOUT_FILENO, message, 32);
    }
    fflush(stdout);
}

