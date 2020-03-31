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
#include <pthread.h>

#include "ops_net.h"
#include "ops_shell.h"

#define TEST_GETGROUPS	0
#define TEST_OPENPTY	0
#define TEST_STRESSUDS	1

static int test_getgroups()
{
	int ret = 0;
#if TEST_GETGROUPS
	gid_t list[500];
	int x = 0;
	if(argc < 2) {
		printf("main_unittest <get/set> <key> [val]\n");
		return 1;
	}
	x = getgroups(0, NULL);
	printf("x=%d\n", x);
	if(x < 0) {
		printf("Error: %d, %s\n", errno, strerror(errno));
		return 1;
	}
	getgroups(x, list);
	for(int i = 0; i < x; i++)
		printf("%d:%d\n", i, list[i]);
#endif
	return ret;
}

static int test_openpty()
{
	int ret = 0;
#if TEST_OPENPTY
#define BUFF_LEN	1024
	int pty_master = -1;
	int pty_slave = -1;
	char pty_name[256] = { 0 };
	char buff[BUFF_LEN] = {0};
	int rd_cnt = 0;
	//char* cmd_args[]={"/bin/sh", NULL};
	char* cmd_args[]={"/sbin/ifconfig", NULL};
	//int isWrite = 0;
	pid_t pid;
	fd_set readfds;
	ret = openpty(&pty_master, &pty_slave, &pty_name[0], 0, 0);
	if(ret < 0) {
		printf("Open PTY Error: %d, %s\n", errno, strerror(errno));
		return -1;
	}
	pid = fork();

	switch(pid) {
	case -1:
	//Error
	close(pty_master);
	close(pty_slave);
	break;
	case 0:
	//Child
	execvp(cmd_args[0], cmd_args);
	//execve("/bin/sh", ["/bin/sh", NULL], NULL);
	break;
	default:
	//Parent
	close(pty_slave);
	FD_ZERO(&readfds);
	FD_SET(pty_master, &readfds);

	for(;;){
		select(pty_master + 1, &readfds, NULL, NULL, NULL);

		if(FD_ISSET(pty_master, &readfds)) {
			memset(buff, 0, BUFF_LEN);
			rd_cnt = read(pty_master, buff, BUFF_LEN);
			printf("%08d[ %s ]\n", rd_cnt, buff);
			//if(isWrite == 2) {
				sleep(1);
			//}
			if((rd_cnt == 12) && (strncmp(buff, "[exit_shell]", 12)==0)) {
				break;
			}
			//if(isWrite == 1) {
			//	write(pty_master, "exit\n", 5);
			//	isWrite = 2;
			//}
			//if(isWrite == 0) {
			//	write(pty_master, "ls\n", 3);
			//	isWrite = 1;
			//}
		}
	}
	close(pty_master);
	break;
	}

	printf("M:%d, S:%d, N:%s\n", pty_master, pty_slave, pty_name);
#endif
	return ret;

}

void* task_stressuds(void *ptr) 
{
	//int idx = *((int*)ptr);
	struct ops_net_t* net = get_net_instance();
	struct msg_t req;
	struct msg_t res;
	struct shell_cmd_t *req_cmd = (struct shell_cmd_t*)&req.data;
	struct shell_cmd_t *res_cmd = (struct shell_cmd_t*)&res.data;
	//int cmd_index = 0;
	memset(&req, 0, sizeof(struct msg_t));
	memset(&res, 0, sizeof(struct msg_t));
	req.fn = 1;
	req.cmd = 2;
	req.data_size = sizeof(struct shell_cmd_t);
//	req_cmd->pty_master = 0;
	req_cmd->action = SHELL_ACTION_CREATE;
	req_cmd->type = SHELL_TYPE_CMDSH;
	req_cmd->instance = 1;
//
	req_cmd->cmdlen = 7;
	memset(&req_cmd->cmd[0], 0, MAX_SHELL_CMDLEN);
	sprintf(&req_cmd->cmd[0], "/bin/sh");
	net->uds_client_send_and_recv(SOCKET_PATH_SHELL, &req, &res);
	printf("Res1 : %d-%d\n", res_cmd->type, res_cmd->instance);

	//req_cmd->pty_master = res_cmd->pty_master;
	req_cmd->action = SHELL_ACTION_EXECUTE;
	req_cmd->type = SHELL_TYPE_CMDSH;
	req_cmd->instance = 1;
//
	req_cmd->cmdlen = 34;
	memset(&req_cmd->cmd[0], 0, MAX_SHELL_CMDLEN);
	sprintf(&req_cmd->cmd[0], "/sbin/ifconfig br1 192.168.155.254");
	net->uds_client_send_and_recv(SOCKET_PATH_SHELL, &req, &res);
	printf("Res2 : %d-%d\n", res_cmd->type, res_cmd->instance);
	//printf("Res2 : %d\n", res_cmd->pty_master);
//
	//req_cmd->pty_master = res_cmd->pty_master;
	req_cmd->action = SHELL_ACTION_TERMINATE;
	req_cmd->type = SHELL_TYPE_CMDSH;
	req_cmd->instance = 1;
	req_cmd->cmdlen = 4;
	memset(&req_cmd->cmd[0], 0, MAX_SHELL_CMDLEN);
	sprintf(&req_cmd->cmd[0], "exit");
	net->uds_client_send_and_recv(SOCKET_PATH_SHELL, &req, &res);
	printf("Res3 : %d-%d\n", res_cmd->type, res_cmd->instance);
	//printf("Res3 : %d\n", res_cmd->pty_master);
//
//	sleep(1);
/*
	req_cmd->pty_master = res_cmd->pty_master;
	req_cmd->status = SHELL_STATUS_TERMINATE;
	req_cmd->cmdlen = 0;
	memset(&req_cmd->cmd[0], 0, MAX_SHELL_CMDLEN);
	net->uds_client_send_and_recv(SOCKET_PATH_SHELL, &req, &res);
	printf("Res4 : %d\n", res_cmd->pty_master);
	*/
	free(ptr);
}

static int test_stressuds()
{
	int ret = 0;
#if TEST_STRESSUDS
	int max_inst = 1;
	pthread_t pids[max_inst];
	for(int i=0;i<max_inst;i++) {
		int *arg = malloc(sizeof(*arg));
		*arg = i;
		pthread_create(&pids[i], NULL, task_stressuds, arg);
	}
	for(int i=0;i<max_inst;i++) {
		pthread_join(pids[i], NULL);
	}
#endif
	return ret;
}


int main_unittest(int argc, char** argv)
{
	int result = 0;
	result = test_getgroups();
	if(result < 0) {
		printf("test_getgroups() : failed\n");
	} else if(result > 0) {
		printf("test_getgroups() : passed\n");
	} else {
	}

	result = test_openpty();
	if(result < 0) {
		printf("test_openpty() : failed\n");
	} else if(result > 0) {
		printf("test_openpty() : passed\n");
	} else {
	}

	result = test_stressuds();
	if(result < 0) {
		printf("test_stressuds() : failed\n");
	} else if(result > 0) {
		printf("test_stressuds() : passed\n");
	} else {
	}
	return 0;
}
