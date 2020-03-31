#ifdef UTILS_UDS

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
#include "ops_net.h"

static int usage_main_uds()
{
	printf("main_uds <out format> <fn> <cmd> <data>\n");
	printf(" out format: string, raw\n");
	printf(" main_uds string 2 2 {\\\"ops\\\":\\\"get\\\",\\\"key\\\":\\\"storage_count\\\"}");
	return -1;
}

int main_uds(int argc, char** argv)
{
	struct msg_t req_msg;
	struct msg_t res_msg;
	uint32_t msg_size = sizeof(struct msg_t);
	struct ops_net_t* net = get_net_instance();
	uint8_t* out_format = NULL;
	int i = 0;

	if(argc < 5) {
		return usage_main_uds();
	}

	memset(&req_msg, 0, msg_size);
	memset(&res_msg, 0, msg_size);

	out_format = argv[1];
	printf("out %s\n", out_format);

	req_msg.data_size = 0;
	req_msg.fn = strtoul(argv[2], NULL, 16);
	req_msg.cmd = strtoul(argv[3], NULL, 16);
	req_msg.data_size = strlen(argv[4]);
	strncpy(req_msg.data, argv[4], req_msg.data_size);

	printf("cli %d, %d\n", req_msg.data_size, argc);

	net->uds_client_send_and_recv(SOCKET_PATH_WWW, &req_msg, &res_msg);

	printf("cli fn : %x\n", res_msg.fn);
	printf("cli cmd : %x\n", res_msg.cmd);
	printf("cli data size : %ld\n", res_msg.data_size);
	if( (strlen(out_format) == strlen("string")) && (memcmp(out_format, "string", strlen("string")) == 0) ) {
		printf("%s", res_msg.data);
	} 
	if( (strlen(out_format) == strlen("raw")) && (memcmp(out_format, "raw", strlen("raw")) == 0) ) {
		for(i=0;i<res_msg.data_size;i++) {
			printf("%x,", res_msg.data[i]);
		}
	}
	printf("\n");
			
	return 0;
}

#endif
