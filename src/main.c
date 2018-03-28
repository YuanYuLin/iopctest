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

// ref https://github.com/troydhanson/network/blob/master/unixdomain/05.dgram/recv.c

static int usage_main_uds()
{
	printf("main_uds <out format> <fn> <cmd> <data>\n");
	printf(" out format: string, raw\n");
	printf(" main_uds string 2 2 {\\\"ops\\\":\\\"get\\\",\\\"key\\\":\\\"storage_count\\\"}");
}
#if 0
static int usage_main_db()
{
	printf("main_db <get/set> <key> [val]\n");
	printf(" main_db get abc\n");
	printf(" main_db set abc {\\\"a\\\":\\\"1\\\",\\\"b\\\":2,\\\"c\\\":[1,2,3]}\n");
}
#endif
static int main_uds(int argc, char** argv)
{
	struct msg_t req_msg;
	struct msg_t res_msg;
	uint32_t msg_size = sizeof(struct msg_t);
	struct ops_net_t* net = get_net_instance();
	uint8_t* out_format = NULL;
	int i = 0;

	if(argc < 5) {
		usage_main_uds();
		return -2;
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

	net->uds_client_send_and_recv(&req_msg, &res_msg);

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
#if 0
static int main_db(int argc, char** argv)
{
	struct ops_db_t* db = get_db_instance();
	uint8_t* cmd = NULL;
	uint8_t* key = NULL;
	uint8_t val[1024] = { 0 };

	if(argc < 4) {
		usage_main_db();
		return -2;
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
struct sub_fun_t {
	uint8_t name[20];
	int (*fun)(int argc, char** argv);
};

static struct sub_fun_t list[] = {
	{ "main_uds", main_uds },
//	{ "main_db", main_db },
	{ "", NULL }
};

static void main_usage()
{
    int i = 0;
    int list_size = sizeof(list)/sizeof(struct sub_fun_t);
    printf("\n");
    for(i=0;i<list_size;i++){
	    printf("  %s help\n", list[i].name);
    }
}

int main(int argc, char** argv)
{
    int i = 0;
    int list_size = sizeof(list)/sizeof(struct sub_fun_t);
    uint8_t* main_func = NULL;
    if(argc < 2){
	    main_usage();
	    return 1;
    }
    main_func = argv[1];
    for(i=0;i<list_size;i++){
	    if(!strcmp(main_func, list[i].name)){
		    list[i].fun(argc - 1, &argv[1]);
	    }
    }
    return 0;
}

