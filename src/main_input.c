#ifdef UTILS_INPUT

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
#include <sys/ioctl.h>

#include "ops_db.h"
#include "ops_mq.h"
#include "ops_log.h"
#include "ops_task.h"
#include "ops_net.h"

#define SET_KEYBOARD_KEY        0x10
#define SET_MOUSE_KEY           0x11

struct iopc_input_t {
        uint8_t dev_type;
        uint8_t input_key;
};

static int usage_main_input()
{
        printf("main_input <event dev> <cmd> <dev type> <key data>\n");
        printf(" out format: string, raw\n");
        printf(" main_uds string 2 2 {\\\"ops\\\":\\\"get\\\",\\\"key\\\":\\\"storage_count\\\"}");
	return -1;
}

int main_input(int argc, char** argv)
{
	int fd = -1;
	uint8_t* event_dev = NULL;
	uint8_t dev_type = 0;
	uint8_t key_val = 0;
	uint8_t cmd = 0x00;
	if(argc < 5)
		return usage_main_input();

	event_dev = argv[1];
	cmd = strtoul(argv[2], NULL, 16);
	dev_type = strtoul(argv[3], NULL, 16);
	key_val = strtoul(argv[4], NULL, 16);
	input.dev_type = dev_type;
	input.input_key = key_val;
	fd = open(event_dev);
	if(fd <0){
		printf("failed to open %s\n", event_dev);
		return -1;
	}

	if(ioctl(fd, cmd, &input) < 0) {
		printf("ioctl failed \n");
		return -1;
	}
	close(fd);
	return 0;
}

#endif
