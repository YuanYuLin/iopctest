#ifdef UTILS_RFB

#include <sys/socket.h>
#include <sys/epoll.h>
#include <signal.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "ops_db.h"
#include "ops_mq.h"
#include "ops_log.h"
#include "ops_task.h"
#include "ops_net.h"
#include "ops_rfb.h"

static int usage_main_rfb()
{
    printf("main_rfb <host> <port> \n");
    return -1;
}

int main_rfb(int argc, char **argv)
{
    int socket_fd = -1;
    struct server_init_t si;
    struct ops_rfb_t *rfb = get_rfb_instance();

    if (argc < 3) {
	return usage_main_rfb();
    }

    socket_fd = rfb->create_socket(argv[1], strtol(argv[2], NULL, 10));

    if (rfb->handshake(socket_fd, &si) < 0) {
	printf("handleshake failed....\n");
	close(socket_fd);
	return -1;
    }
    if (rfb->request_entire_screen(socket_fd, &si) < 0) {
	printf("request content failed....\n");
	close(socket_fd);
	return -1;
    }
    do {
	if (rfb->processor(socket_fd, &si ,NULL) < 0)
	    break;
	if (rfb->request_changed_screen(socket_fd, &si) < 0) {
	    printf("request content failed....\n");
	    break;
	}
    } while (1);
    rfb->close_socket(socket_fd);

    return 0;
}

#endif
