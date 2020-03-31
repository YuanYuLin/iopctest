#define UTILS_WWW 1
#ifdef UTILS_WWW

#include <sys/socket.h>
#include <netdb.h>
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

static int usage_main_www()
{
	printf("main_www <host> <port>\n");
	printf(" out format: string, raw\n");
	printf(" main_www string 2 2 {\\\"ops\\\":\\\"get\\\",\\\"key\\\":\\\"storage_count\\\"}");
	return -1;
}

#define WWW_PORT		80
#define MAX_PAYLOAD_SIZE	0xFFF

int main(int argc, char** argv)
//int main_www(int argc, char** argv)
{
	uint8_t request[MAX_PAYLOAD_SIZE] = {0};
	uint8_t response[MAX_PAYLOAD_SIZE] = {0};
	struct addrinfo hints;
	struct addrinfo *res;
	int client_fd = -1;
	int ret = -1;
	uint8_t* host = NULL;
	uint8_t* port = NULL;

	if(argc < 3) {
		return usage_main_www();
	}

	host = argv[1];
	port = argv[2];

	sprintf(request, "GET /api/v1/dao HTTP/1.1\r\nHost: %s\r\n\r\n", host);
	printf("%s\n", host);

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	if((ret = getaddrinfo(host, port, &hints, &res)) != 0) {
		return -1;
	}

	client_fd = socket(res->ai_family, res->ai_socktype, 0);

	ret = connect(client_fd, res->ai_addr, res->ai_addrlen);
	freeaddrinfo(res);

	if(ret == 0) {
		//succeeds
		printf("REQUEST: %d\n", strlen(request));
		printf("%s", request);
		ret = send(client_fd, request, strlen(request), 0);

		if(ret >= 0) {
			ret = recv(client_fd, response, MAX_PAYLOAD_SIZE, 0);
			printf("RESPONSE:\n");
			printf("%s", response);
		}
	}
	shutdown(client_fd, SHUT_RDWR);

	return 0;
}

#endif
