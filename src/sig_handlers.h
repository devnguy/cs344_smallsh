#ifndef SIG_HANDLERS_H
#define SIG_HANDLERS_H

#include "Status.h"

void send_status_to_sig_handlers(Status *new_status);
void ignore_sigint();
void ignore_sigtstp();
void install_default_sigint_handler();
void install_sigtstp_handler();
void sigtstp_handler();

#endif
