#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
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
#include <pty.h>

static int usage_main_md()
{
	printf("main_md <device> <offset in hex> <length in dec>\n");
	printf("length : 1 ~ 512\n");
	printf("    main_md /dev/sda 0x100000 64\n");
	return -1;
}

int main_md(int argc, char** argv)
{
	if(argc < 3)
		return usage_main_md();

	uint8_t buf[512] = { 0 };
	uint8_t *dev = argv[1];
	uint32_t offset = strtoul(argv[2], NULL, 16);
	uint32_t length = strtoul(argv[3], NULL, 10);
	if(length >512)
		return usage_main_md();

	int fd = open(dev, O_RDONLY, 0644);
	if(fd < 0) {
		printf("open %s error\n", dev);
		return -1;
	}
	lseek(fd, offset, SEEK_SET);
	read(fd, &buf[0], length);
	close(fd);
	for(int i = 0;i<length;i++) {
		if(i%16 == 0) 
			printf("\n");
		printf("0x%02x,", buf[i]);
	}
	printf("\n");
	return 0;
}
