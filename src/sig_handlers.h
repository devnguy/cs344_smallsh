#ifndef SIG_HANDLERS_H
#define SIG_HANDLERS_H

#include "Status.h"

void ignore_sigint();
void ignore_sigtstp();
void init_sigint_handler();

void install_default_sigint_handler();
void send_status_to_sig_handlers(Status *new_status);
void install_sgtstp_handler();

#endif

