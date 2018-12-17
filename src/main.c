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
#include "main.h"

struct sub_fun_t {
	uint8_t name[20];
	int (*fun)(int argc, char** argv);
};

static struct sub_fun_t list[] = {
	MAIN_UDS
	MAIN_DB
	MAIN_INPUT
	MAIN_LXC_CREATE
	MAIN_DRM
	MAIN_RFB
	MAIN_QMP
	MAIN_QEMUMONITOR
	MAIN_END
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

