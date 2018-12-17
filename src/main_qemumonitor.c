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

static int usage_main_qemumonitor()
{
	printf("main_qemumonitor <ip addr> <port> <cmds>\n");
	printf(" main_qemumonitor 192.168.70.11 2300 info\\ kvm");
	return -1;
}

int main_qemumonitor(int argc, char** argv)
{
	int mon_sockfd = -1;
	int rst = -1;
	int len = 0;
	struct sockaddr_in address;
	int wc = 0;
	int rc = 0;
	uint8_t str_output[512] = { 0 };
	uint8_t str_input[512] = { 0 };

	if(argc < 4) {
		return usage_main_qemumonitor();
	}

	uint8_t *str_ip_addr = argv[1];
	uint32_t port = strtoul(argv[2], NULL, 10);
	uint8_t *str_cmd = argv[3];
	memset(&str_input, 0, sizeof(str_input));

	mon_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(str_ip_addr);
	address.sin_port = htons(port);
	len = sizeof(address);

	rst = connect(mon_sockfd, (struct sockaddr *)&address, len);
	if(rst < 0) {
		printf("connection failed...\n");
		return 1;
	}

	memset(&str_output, 0, sizeof(str_output));
	sprintf(str_input, "%s\n", str_cmd);
	rc = read(mon_sockfd, str_output, sizeof(str_output));
	printf("pre %d - %s\n", rc, str_output);
	wc = write(mon_sockfd, str_input, strlen(str_input));
	printf("cmd[%d-%d]: %s\n", wc, strlen(str_input), str_input);
	//uint8_t *ptr = &str_output[strlen(str_output) - 6];
	//printf("CCC %s\n", ptr);
	do {
		sleep(1);
		memset(&str_output, 0, sizeof(str_output));
		rc = read(mon_sockfd, str_output, sizeof(str_output));
		printf("%s", str_output);
		//printf("rc=%d\n%s\n", rc, str_output);
		if(rc >= sizeof(str_output)){
			continue;
		}
		break;
	} while(1);
	printf("\n");

	close(mon_sockfd);

	return 0;
}

