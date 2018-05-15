#ifdef UTILS_DB
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

static int usage_main_db()
{
	printf("main_db <get/set> <key> [val]\n");
	printf(" main_db get abc\n");
	printf(" main_db set abc {\\\"a\\\":\\\"1\\\",\\\"b\\\":2,\\\"c\\\":[1,2,3]}\n");
	return -1;
}

int main_db(int argc, char** argv)
{
	struct ops_db_t* db = get_db_instance();
	uint8_t* cmd = NULL;
	uint8_t* key = NULL;
	uint8_t val[1024] = { 0 };

	if(argc < 4) {
		return usage_main_db();
	}
	cmd = argv[1];
	key = argv[2];
	memset(&val[0], 0, sizeof(val));

	if(!strcmp(cmd, "get")) {
		db->get_val(key, &val[0]);
	}

	if(!strcmp(cmd, "set")) {
		strcpy(&val[0], argv[3]);
		db->set_val(key, &val[0]);
	}
	printf("key:%s, val: %s\n", key, val);

	return 0;
}
#endif
