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

#include "ops_db.h"
#include "ops_mq.h"
#include "ops_log.h"
#include "ops_task.h"

// ref https://github.com/troydhanson/network/blob/master/unixdomain/05.dgram/recv.c
static uint8_t* socket_path = "/var/run/uds.www";
#define MAX_CLIENT	5

static int usage_main_uds()
{
	printf("main_uds <fn> <cmd> <data>\n");
}

static int send_and_recv(struct msg_t* req, struct msg_t* res)
{
	struct sockaddr_un addr;
	uint32_t msg_size = sizeof(struct msg_t);
	struct ops_log_t* log = get_log_instance();
	uint32_t wc = 0;
	uint32_t rc = 0;
	uint32_t i = 0;
	struct sockaddr_un cli_addr;
	int socket_fd = -1;

	socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (socket_fd == -1) {
		log->error(0x01, "cli socket: %s\n", strerror(errno));
		return -1;
	}

	memset(&cli_addr, 0, sizeof(struct sockaddr_un));
	cli_addr.sun_family = AF_UNIX;
	for(i=0;i<MAX_CLIENT;i++) {
		sprintf(cli_addr.sun_path, "%s.cli_%x", socket_path, i);
		if(access(cli_addr.sun_path, F_OK) != -1) {
			// file exist
			continue;
		} else {
			// file not exist
			break;
		}
	}
	if(i >= MAX_CLIENT) {
		log->error(0x01, "can not find slot for clie\n");
	}
	log->debug(0x01, "bind path: %s\n", cli_addr.sun_path);

	if(bind(socket_fd, (struct sockaddr*)&cli_addr, sizeof(struct sockaddr_un)) < 0) {
		log->error(0x01, "cli bind error : %s\n", strerror(errno));
		return -2;
	}

	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, socket_path);
	log->debug(0x01, "cli sending to %s\n", addr.sun_path);
	wc = sendto(socket_fd, (void*)req, msg_size, 0, (struct sockaddr*)&addr, sizeof(struct sockaddr_un));
	log->debug(0x01, "cli write count = %ld\n", wc);

	rc = recvfrom(socket_fd, (void*)res, msg_size, 0, NULL, NULL);
	log->debug(0x01, "cli read count = %ld\n", rc);

	log->debug(0x01, "cli reading from %s\n", cli_addr.sun_path);
	unlink(cli_addr.sun_path);
	return 0;
}

static int main_uds(int argc, char** argv)
{
	struct msg_t req_msg;
	struct msg_t res_msg;
	uint32_t msg_size = sizeof(struct msg_t);
	int i = 0;
	//uint32_t wc = 0;
	//uint32_t rc = 0;

	if(argc < 4) {
		usage_main_uds();
		return -2;
	}

	memset(&req_msg, 0, msg_size);
	memset(&res_msg, 0, msg_size);

	req_msg.data_size = 0;
	req_msg.fn = strtoul(argv[1], NULL, 16);
	req_msg.cmd = strtoul(argv[2], NULL, 16);
	req_msg.data_size = strlen(argv[3]);
	strncpy(req_msg.data, argv[3], req_msg.data_size);

	printf("cli %d, %d\n", req_msg.data_size, argc);

	send_and_recv(&req_msg, &res_msg);

	printf("cli fn : %x\n", res_msg.fn);
	printf("cli cmd : %x\n", res_msg.cmd);
	printf("cli data size : %ld\n", res_msg.data_size);
	for(i=0;i<res_msg.data_size;i++) {
		printf("%x,", res_msg.data[i]);
	}
	printf("\n");
			
	return 0;
}

int main(int argc, char** argv)
{
    uint8_t* main_func = NULL;
    main_func = argv[1];
    printf("%d\n", argc);
    printf("fun: %s\n", main_func);
    if(strcmp(main_func, "main_uds") == 0) {
	    main_uds(argc - 1, &argv[1]);
    }
    return 0;
}

