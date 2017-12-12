#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include "iopcdefine.h"
//#include "iopccfg.h"
#include "iopcops_cfg.h"
#include "iopcops_cfg_platform.h"
#include "iopcops_mq.h"
#include "iopcops_misc.h"

static struct task_t task_table[MAX_TASK_SIZE];
static struct cmd_t cmd_table[MAX_CMD_GROUP_SIZE][MAX_CMD_SIZE];

static struct root_t init_list[] = {
    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/bin", DEFAULT_MODE, 0 ),

    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/dev", DEFAULT_MODE, 0 ),
    INIT_NODE( ROOT_TYPE_MOUNT_DEVTMPFS, "/dev", 0, 0 ),
    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/dev/pts", DEFAULT_MODE, 0 ),
    INIT_NODE( ROOT_TYPE_MOUNT_DEVPTS, "/dev/pts", "newinstance,ptmxmode=0666", 0 ),
    INIT_NODE( ROOT_TYPE_CREATE_SYMBOL, "/dev/pts/ptmx", "/dev/ptmx", 0 ),
    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/dev/shm", DEFAULT_MODE, 0 ),
    INIT_NODE( ROOT_TYPE_MOUNT_TMPFS, "/dev/shm", 0, 0 ),
    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/dev/mqueue", DEFAULT_MODE, 0 ),
    INIT_NODE( ROOT_TYPE_MOUNT_MQUEUE, "/dev/mqueue", 0, 0 ),

    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/etc", DEFAULT_MODE, 0 ),

    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/home", DEFAULT_MODE, 0 ),

    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/lib", DEFAULT_MODE, 0 ),

    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/mnt", DEFAULT_MODE, 0 ),

    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/proc", DEFAULT_MODE, 0 ),
    INIT_NODE( ROOT_TYPE_MOUNT_PROC, "/proc", 0, 0 ),

//    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/root", DEFAULT_MODE, 0 ),

    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/sbin", DEFAULT_MODE, 0 ),

    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/sys", DEFAULT_MODE, 0 ),
    INIT_NODE( ROOT_TYPE_MOUNT_SYSFS, "/sys", 0, 0 ),

    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/tmp", 0x3FF, 0 ),
    INIT_NODE( ROOT_TYPE_MOUNT_TMPFS, "/tmp", 0, 0 ),

    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/usr", DEFAULT_MODE, 0 ),
    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/usr/bin", DEFAULT_MODE, 0 ),
    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/usr/sbin", DEFAULT_MODE, 0 ),

    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/var", DEFAULT_MODE, 0 ),
    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/var/run", DEFAULT_MODE, 0 ),
    INIT_NODE( ROOT_TYPE_CREATE_DIR, "/var/log", DEFAULT_MODE, 0 ),
};

static int init_console(uint8_t* tty_name)
{
    int fd = -1;

    fd = open(tty_name, O_RDWR | O_NONBLOCK | O_NOCTTY);
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);

    return 0;
}

static int setup_zram_swap(uint8_t* disksize)
{
    uint8_t cmd_str[STR_LEN];
    memset(cmd_str, 0, STR_LEN);
    sprintf(cmd_str, "echo %s > /sys/block/zram0/disksize", disksize);
    GET_INSTANCE_MISC_OBJ()->execute_cmd(cmd_str, NULL);

    memset(cmd_str, 0, STR_LEN);
    sprintf(cmd_str, "mkswap /dev/zram0");
    GET_INSTANCE_MISC_OBJ()->execute_cmd(cmd_str, NULL);

    memset(cmd_str, 0, STR_LEN);
    sprintf(cmd_str, "swapon /dev/zram0");
    GET_INSTANCE_MISC_OBJ()->execute_cmd(cmd_str, NULL);

    return 0;
}

static void sysinit() 
{
    int is_init_zram_swap = 0;
    uint8_t zram_disksize[STR_LEN];
    int is_init_console = 0;
    uint8_t platform_name[STR_LEN];
    int is_host = 0;
    uint8_t tty_name[STR_LEN];
    uint16_t str_len = 0;
    int list_size = (sizeof(init_list) / sizeof(init_list[0]));
    uint8_t platform_idx = 0;
    
    //printf("SYSINIT\n");
    putenv((char *) "HOME=/");
    putenv((char *) "PATH=/bin:/sbin:/usr/bin:/usr/sbin/:/usr/local/bin:/usr/local/sbin");
    putenv((char *) "SHELL=/bin/sh");
    putenv((char *) "USER=homeroot"); 
    setsid();

    GET_INSTANCE_CFG_PLATFORM()->loadall();

    is_init_zram_swap = GET_INSTANCE_CFG_PLATFORM()->is_init_zram_swap(platform_idx);
    memset(zram_disksize, 0, STR_LEN);
    str_len = GET_INSTANCE_CFG_PLATFORM()->get_zram_disksize(platform_idx, &zram_disksize[0]);

    is_init_console = GET_INSTANCE_CFG_PLATFORM()->is_init_console(platform_idx);
    memset(platform_name, 0, STR_LEN);
    str_len = GET_INSTANCE_CFG_PLATFORM()->get_platform_name(platform_idx, &platform_name[0]);
    is_host = GET_INSTANCE_CFG_PLATFORM()->is_host(platform_idx);

    memset(tty_name, 0, STR_LEN);
    str_len = GET_INSTANCE_CFG_PLATFORM()->get_tty_name(platform_idx, &tty_name[0]);

    if(0)
        printf("str_len %d\n", str_len);

    if(is_init_console) {
        init_console(tty_name);
    }
    printf("Platform {%s} is running in [%s] mode\n", platform_name, is_host?"host":"guest");
    printf("[%d]tty: %s\n", is_init_console, tty_name);

    GET_INSTANCE_MISC_OBJ()->create_system_by_list(list_size, &init_list[0]);

    if(is_init_zram_swap) {
        setup_zram_swap(zram_disksize);
    }
}

static void callback_task(void* dl_handle, void* task_id, get_name_fn_t get_name_fn, get_help_fn_t get_help_fn)
{
    int i = 0;
    struct task_t* task = NULL;
    printf("register task: %s()\n", get_name_fn());
    for(i=0;i<MAX_TASK_SIZE;i++) {
        task = &task_table[i];
	if(task->isused == 0) {
            task->isused = 1;
	    task->dl_handle = dl_handle;
	    task->task_id = task_id;
	    task->get_name = get_name_fn;
	    task->get_help = get_help_fn;
	    break;
	}
    }
}

static void plugin_fixed_tasks()
{
    printf("Plugin tasks...\n");
    GET_INSTANCE_MISC_OBJ()->plugin_new_task(IOPC_LIBS_PATH(libiopctaskshell.so), callback_task);
    GET_INSTANCE_MISC_OBJ()->plugin_new_task(IOPC_LIBS_PATH(libiopctasktimer.so), callback_task);
//    GET_INSTANCE(ops_misc)->plugin_new_task(IOPC_LIBS_PATH(libiopctaskhttp.so), callback_task);
}

static void callback_cmd(void* dl_handle, uint8_t group, uint8_t cmd, cmd_filter_fn_t cmd_filter, cmd_handler_fn_t cmd_handler)
{
    struct cmd_t *cmd_ptr = &cmd_table[group][cmd];
    cmd_ptr->cmd_filter = cmd_filter;
    cmd_ptr->cmd_handler = cmd_handler;
    printf("register cmd %x,%x\n", group, cmd);
}

#define SYS_CMD(fn_no, cmd_no, name) \
	GET_INSTANCE_MISC_OBJ()->plugin_new_cmd(IOPC_LIBS_PATH(libiopccmd_system.so), callback_cmd, fn_no, cmd_no, name)

static void plugin_fixed_cmds()
{
    printf("Plugin cmds...%s\n", IOPC_LIBS_PATH(libiopccmd_system.so));

#if 1
    SYS_CMD(CLA_SERVICE, INIT_RAIDDEV, "raiddev");
    SYS_CMD(CLA_SERVICE, INIT_MNTBASE, "mntbase");
    SYS_CMD(CLA_SERVICE, INIT_NETDEV, "netdev");
    SYS_CMD(CLA_SERVICE, INIT_NETSSH, "netssh");
    SYS_CMD(CLA_SERVICE, INIT_NETNTP, "netntp");
    SYS_CMD(CLA_SERVICE, INIT_VM, "vmcontainer");
    SYS_CMD(CLA_SERVICE, INIT_NETHTTP, "nethttp");

    SYS_CMD(CLA_BASE, RAIDDEV_COUNT, "raiddevcount");
    SYS_CMD(CLA_BASE, RAIDDEV_GET, "raiddevget");
    SYS_CMD(CLA_BASE, RAIDDEV_SET, "raiddevset");

    SYS_CMD(CLA_BASE, MNTBASE_COUNT, "mntbasecount");
    SYS_CMD(CLA_BASE, MNTBASE_GET, "mntbaseget");
    SYS_CMD(CLA_BASE, MNTBASE_SET, "mntbaseset");

    SYS_CMD(CLA_BASE, VM_ADD, "vmadd");
    SYS_CMD(CLA_BASE, VM_COUNT, "vmcount");
    SYS_CMD(CLA_BASE, VM_GET, "vmget");
    SYS_CMD(CLA_BASE, VM_SET, "vmset");

    SYS_CMD(CLA_ACTION, OP_MKBTRFS, "mkbtrfs");
    SYS_CMD(CLA_ACTION, OP_MKBTRFS_STATUS, "mkbtrfs_status");

    SYS_CMD(CLA_DB, GET_BOOL_FLASH, "db_getbool_f");
    SYS_CMD(CLA_DB, SET_BOOL_FLASH, "db_setbool_f");
    SYS_CMD(CLA_DB, GET_UINT32_FLASH, "db_getuint32_f");
    SYS_CMD(CLA_DB, SET_UINT32_FLASH, "db_setuint32_f");
    SYS_CMD(CLA_DB, GET_STRING_FLASH, "db_getstring_f");
    SYS_CMD(CLA_DB, SET_STRING_FLASH, "db_setstring_f");
    SYS_CMD(CLA_DB, SAVE2PERSIST, "db_save2persist");

    SYS_CMD(CLA_DB, GET_BOOL_RAM, "db_getbool_r");
    SYS_CMD(CLA_DB, SET_BOOL_RAM, "db_setbool_r");
    SYS_CMD(CLA_DB, GET_UINT32_RAM, "db_getuint32_r");
    SYS_CMD(CLA_DB, SET_UINT32_RAM, "db_setuint32_r");
    SYS_CMD(CLA_DB, GET_STRING_RAM, "db_getstring_r");
    SYS_CMD(CLA_DB, SET_STRING_RAM, "db_setstring_r");
    SYS_CMD(CLA_DB, RESTORE2DEFAULT, "db_restore2default");

#endif
}

int main(int argc, char** argv)
{
    struct msg_t req;
    struct msg_t res;
    uint32_t res_len;
    struct cmd_t *cmd;
    uint8_t group;
    uint8_t cmdno;
    uint8_t *req_data_ptr;
    uint8_t *res_data_ptr;

    GET_INSTANCE_CFG_IFC()->init();

    sysinit();
    if(1) plugin_fixed_cmds();
    if(1) plugin_fixed_tasks();

    for(;;) {
        memset((uint8_t*)&req, 0, sizeof(struct msg_t));
	memset((uint8_t*)&res, 0, sizeof(struct msg_t));

	GET_INSTANCE_MQ_OBJ()->get_from(TASK_IOPCLAUNCHER, &req);
	memcpy(&res.hdr, &req.hdr, sizeof(struct msg_hdr_t));
	strcpy(res.hdr.src, req.hdr.dst);
	strcpy(res.hdr.dst, req.hdr.src);

	group = req.hdr.fn;
	cmdno = req.hdr.cmd;
	printf("------------------------------------\n");
	printf("get MsgQ[%x.%x]\n", group, cmdno);
	printf("------------------------------------\n");

	req_data_ptr = (uint8_t*)&req.data;
	res_data_ptr = (uint8_t*)&res.data;

	cmd = &cmd_table[group][cmdno];
	if(cmd->cmd_handler) {
            printf("Run cmd_handler...\n");
            res_len = cmd->cmd_handler(req_data_ptr, res_data_ptr);
	}
	res.hdr.data_size = res_len;
	res.hdr.fn = 0x80 | group;

	if(strcmp(res.hdr.dst, "") != 0) {
            GET_INSTANCE_MQ_OBJ()->set_to(res.hdr.dst, &res);
	}
    }

    return 0;
}

