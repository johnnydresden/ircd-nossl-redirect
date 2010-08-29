#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "evt.h"
#include "signal_handler.h"
static ev_signal   signal_die;
static ev_signal   signal_reload;
int dying = 0;

static void signal_handler_die(struct ev_loop *loop, ev_signal * w, int revents) {
   dying = 1;
   exit(EXIT_SUCCESS);
}

static void signal_handler_reload(struct ev_loop *loop, ev_signal * w, int revents) {
}

void signal_init(void) {
   ev_signal_init(&signal_die, signal_handler_die, SIGINT);
   ev_signal_init(&signal_die, signal_handler_die, SIGQUIT);
   ev_signal_init(&signal_die, signal_handler_die, SIGTERM);
   ev_signal_init(&signal_reload, signal_handler_reload, SIGHUP);
   signal(SIGPIPE, SIG_IGN);
}
