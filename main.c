#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include "evt.h"
#include "signal_handler.h"

// Works for IRC, maybe probably HTTP
void send_msg(int fd, const char *msg, ...) {
   va_list ap;
   char	buf[512];
   int ret;

   va_start(ap, msg);
   memset(buf, 0, sizeof(buf));
   vsnprintf(buf, sizeof(buf) - 3, msg, ap);
   strncat(buf, "\r\n", 2);

   for (ret = 0; ret < strlen(buf); )
      ret += write(fd, buf + ret, strlen(buf) - ret);

   va_end(ap);
}

static void socket_listener_cb(struct ev_loop *loop, ev_io * w, int revents) {
   int fd;

   if ((fd = accept(w->fd, NULL, 0)) < 0) {
      fprintf(stderr, "accept failed for fd %d\n", w->fd);
      exit(1);
   }

   send_msg(fd, "NOTICE AUTH :*** This server only accepts SSL connections");
   send_msg(fd, "NOTICE AUTH :*** Please use port %d to connect.", TARGET_PORT);
   send_msg(fd, "NOTICE AUTH :*** Please see %s for more information!", CONNECT_URL);
   close(fd);
}

int main(int argc, char **argv) {
   int val = 1;
   int fd;
   struct sockaddr_in sin;
   ev_io evt;

   evt_init();

   if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      fprintf(stderr, "creating listener failed %d: %s\n", errno, strerror(errno));
      exit(EXIT_FAILURE);
   }

   memset(&sin, 0, sizeof(sin));
   /*
    * XXX: make this use addr 
    */
   sin.sin_addr.s_addr = INADDR_ANY;
   sin.sin_family = AF_INET;
   sin.sin_port = htons(LISTEN_PORT);
   setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

   if (bind(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
      fprintf(stderr, "cannot bind socket %d: %s\n", errno, strerror(errno));
      exit(EXIT_FAILURE);
   }

   if (listen(fd, 5)) {
      fprintf(stderr, "error listening on *:%d %d: %s\n", LISTEN_PORT, errno, strerror(errno));
      exit(EXIT_FAILURE);
   }

   /* register event for new connections */
   ev_io_init(&evt, socket_listener_cb, fd, EV_READ);
   ev_io_start(evt_loop, &evt);

   while (!dying) {
      ev_loop(evt_loop, 0);
   }

   fprintf(stderr, "shutting down...\n");
  
   return EXIT_SUCCESS;
}
