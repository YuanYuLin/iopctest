#ifdef UTILS_QEMU

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

static int usage_main_qmp()
{
	printf("main_qmp <QEMU_index> <script>\n");
	printf(" out format: string, raw\n");
	printf(" main_qmp string 2 2 {\\\"ops\\\":\\\"get\\\",\\\"key\\\":\\\"storage_count\\\"}");
	return -1;
}

int main_qmp(int argc, char** argv)
{
	uint8_t req_msg[BUF_SIZE];
	uint8_t res_msg[BUF_SIZE];
	uint32_t msg_size = BUF_SIZE;
	struct ops_net_t* net = get_net_instance();
	uint8_t qemu_index = 0;

	if(argc < 2) {
		return usage_main_qmp();
	}

	memset(&req_msg, 0, msg_size);
	memset(&res_msg, 0, msg_size);

	qemu_index = (uint8_t)strtoul(argv[1], NULL, 10);
	strcpy(req_msg, argv[2]);

	net->qmp_client_send_and_recv(qemu_index, &req_msg[0], &res_msg[0]);

	printf("qemu[%d] response:%s\n", qemu_index, res_msg);

	return 0;
}

#endif
