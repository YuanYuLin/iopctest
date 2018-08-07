#ifdef UTILS_LXC

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
#include <lxccontainer.h>

#include "ops_db.h"
#include "ops_mq.h"
#include "ops_log.h"
#include "ops_task.h"
#include "ops_net.h"

static int usage_main_lxc_create()
{
	printf("main_lxc_create <vm_name> <vm_rootfs> <vm_fstab> <vm_nettype> <vm_nethwlink> <vm_nethwaddr> <vm_ipaddr> <vm_gateway>\n");
	return -1;
}

#define STR_LEN	30
int main_lxc_create(int argc, char** argv)
{
	uint8_t vm_name[STR_LEN] = {0};
	uint8_t vm_rootfs[STR_LEN] = {0};
	uint8_t vm_fstab[STR_LEN] = {0};
	uint8_t vm_nettype[STR_LEN] = {0};
	uint8_t vm_nethwlink[STR_LEN] = {0};
	uint8_t vm_nethwaddr[STR_LEN] = {0};
	uint8_t vm_ipaddress[STR_LEN] = {0};
	uint8_t vm_gateway[STR_LEN] = {0};
	struct lxc_container *container;
	if(argc < 5) {
		return usage_main_lxc_create();
	}

	sprintf(vm_name, "%s", argv[1]);
	sprintf(vm_rootfs, "%s", argv[2]);
	sprintf(vm_fstab, "%s", argv[3]);
	sprintf(vm_nettype, "%s", argv[4]);
	sprintf(vm_nethwlink, "%s", argv[5]);
	sprintf(vm_nethwaddr, "%s", argv[6]);
	sprintf(vm_ipaddress, "%s", argv[7]);
	sprintf(vm_gateway, "%s", argv[8]);
	container = (struct lxc_container*)lxc_container_new(vm_name, "/var/lib/lxc");

	if(container) {
		if(container->is_defined(container)) {
			printf("1.%s thought it was defined\n", vm_name);
			fprintf(stderr, "1.vm%s thought it was defined\n", vm_name);
			return 1;
		}

		container->clear_config(container);
		container->set_config_item(container, "lxc.utsname", vm_name);
		container->set_config_item(container, "lxc.rootfs", vm_rootfs);
		container->set_config_item(container, "lxc.pts", "256");
		container->set_config_item(container, "lxc.autodev", "1");
		container->set_config_item(container, "lxc.init_cmd", "/init");
		container->set_config_item(container, "lxc.mount", vm_fstab);

		container->set_config_item(container, "lxc.network.type", vm_nettype);
		container->set_config_item(container, "lxc.network.flags", "up");
		container->set_config_item(container, "lxc.network.link", vm_nethwlink);
		container->set_config_item(container, "lxc.network.name", "eth0");
		container->set_config_item(container, "lxc.network.hwaddr", vm_nethwaddr);

		container->set_config_item(container, "lxc.network.type", "veth");
		container->set_config_item(container, "lxc.network.flags", "up");
		container->set_config_item(container, "lxc.network.link", "br1");
		container->set_config_item(container, "lxc.network.name", "eth1");
		container->set_config_item(container, "lxc.network.ipv4", vm_ipaddress);
		container->set_config_item(container, "lxc.network.ipv4.gateway", vm_gateway);

		container->save_config(container, NULL);
		if(!container->create(container, NULL, NULL, NULL, 0, NULL)){
			printf("%s creating failed.\n", vm_name);
			lxc_container_put(container);
			return 1;
		}
		lxc_container_put(container);
	}

	return 0;
}

#endif
