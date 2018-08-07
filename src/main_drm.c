#ifdef UTILS_DRM

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

#include "ops_db.h"
#include "ops_mq.h"
#include "ops_log.h"
#include "ops_task.h"
#include "ops_net.h"
#include "ops_rfb.h"
#include "ops_drm.h"

static int usage_main_drm()
{
	printf("main_drm </dev/dri/cardX> <host> <port> \n");
	return -1;
}

#define STR_LEN	30
int main_drm(int argc, char** argv)
{
	int drm_fd;
	//int i, j, idx;
	//uint8_t color;
	struct drm_dev_t *dev;
	struct drm_dev_t *drm_head;
	uint8_t dri_path[STR_LEN] = {0};

	int socket_fd = -1;
	struct server_init_t si;
	struct ops_rfb_t *rfb = get_rfb_instance();
	struct ops_drm_t *drm = get_drm_instance();
	struct framebuffer_dev_t fb_dev_rfb;
	struct framebuffer_dev_t fb_dev_phy[6];
	uint16_t pos_x, pos_y;
	uint16_t rfb_x, rfb_y;
	uint8_t idx = 0;

	if(argc < 2) {
		return usage_main_drm();
	}
	sprintf(dri_path, "%s", argv[1]);
	/* init */
	drm_fd = drm->open(dri_path);
	drm_head = drm->find_dev(drm_fd);

#if 1

	if (drm_head == NULL) {
		fprintf(stderr, "available drm_dev not found\n");
		return EXIT_FAILURE;
	}

	printf("available connector(s)\n\n");
	for (dev = drm_head; dev != NULL; dev = dev->next) {
		printf("connector id:%d\n", dev->conn_id);
		printf("\tencoder id:%d crtc id:%d fb id:%d\n", dev->enc_id, dev->crtc_id, dev->fb_id);
		printf("\twidth:%d height:%d\n", dev->width, dev->height);
	}

	/* FIXME: use first drm_dev */
	dev = drm_head;
	drm->setup(drm_fd, dev);
#endif

	printf("drm HxW[%d x %d]\n", dev->height, dev->width);
	socket_fd = rfb->create_socket(argv[2], strtol(argv[3], NULL, 10));
	if (rfb->handshake(socket_fd, &si) < 0) {
	        printf("handleshake failed....\n");
	        close(socket_fd);
	        return -1;
	}
	if (rfb->request_entire_screen(socket_fd, &si) < 0) {
		printf("request content failed....\n");
		close(socket_fd);
		return -1;
	}


	fb_dev_rfb.width = si.fb_width;
	fb_dev_rfb.height = si.fb_height;
	fb_dev_rfb.x_pos = 0;
	fb_dev_rfb.y_pos = 0;
	fb_dev_rfb.fb_ptr = malloc(si.fb_width * si.fb_height * (si.fb_pixel_format.bit_per_pixel / 8));
	fb_dev_rfb.depth = DEPTH;
	fb_dev_rfb.bpp = si.fb_pixel_format.bit_per_pixel;

#define SCALE_FACTOR	0.33
	idx = 0;
	fb_dev_phy[idx].width = (dev->width * SCALE_FACTOR);
	fb_dev_phy[idx].height = (dev->height * SCALE_FACTOR);
	fb_dev_phy[idx].x_pos = idx * (dev->width * SCALE_FACTOR);
	fb_dev_phy[idx].y_pos = 0;
	fb_dev_phy[idx].fb_ptr = dev->buf;
	fb_dev_phy[idx].depth = DEPTH;
	fb_dev_phy[idx].bpp = BPP;

	fb_dev_phy[idx + 3].width = (dev->width * SCALE_FACTOR);
	fb_dev_phy[idx + 3].height = (dev->height * SCALE_FACTOR);
	fb_dev_phy[idx + 3].x_pos = idx * (dev->width * SCALE_FACTOR);
	fb_dev_phy[idx + 3].y_pos = (dev->height * SCALE_FACTOR);
	fb_dev_phy[idx + 3].fb_ptr = dev->buf;
	fb_dev_phy[idx + 3].depth = DEPTH;
	fb_dev_phy[idx + 3].bpp = BPP;

	idx = 1;
	fb_dev_phy[idx].width = (dev->width * SCALE_FACTOR);
	fb_dev_phy[idx].height = (dev->height * SCALE_FACTOR);
	fb_dev_phy[idx].x_pos = idx * (dev->width * SCALE_FACTOR);
	fb_dev_phy[idx].y_pos = 0;
	fb_dev_phy[idx].fb_ptr = dev->buf;
	fb_dev_phy[idx].depth = DEPTH;
	fb_dev_phy[idx].bpp = BPP;

	fb_dev_phy[idx + 3].width = (dev->width * SCALE_FACTOR);
	fb_dev_phy[idx + 3].height = (dev->height * SCALE_FACTOR);
	fb_dev_phy[idx + 3].x_pos = idx * (dev->width * SCALE_FACTOR);
	fb_dev_phy[idx + 3].y_pos = (dev->height * SCALE_FACTOR);
	fb_dev_phy[idx + 3].fb_ptr = dev->buf;
	fb_dev_phy[idx + 3].depth = DEPTH;
	fb_dev_phy[idx + 3].bpp = BPP;

	idx = 2;
	fb_dev_phy[idx].width = (dev->width * SCALE_FACTOR);
	fb_dev_phy[idx].height = (dev->height * SCALE_FACTOR);;
	fb_dev_phy[idx].x_pos = idx * (dev->width * SCALE_FACTOR);
	fb_dev_phy[idx].y_pos = 0;
	fb_dev_phy[idx].fb_ptr = dev->buf;
	fb_dev_phy[idx].depth = DEPTH;
	fb_dev_phy[idx].bpp = BPP;

	fb_dev_phy[idx + 3].width = (dev->width * SCALE_FACTOR);
	fb_dev_phy[idx + 3].height = (dev->height * SCALE_FACTOR);
	fb_dev_phy[idx + 3].x_pos = idx * (dev->width * SCALE_FACTOR);
	fb_dev_phy[idx + 3].y_pos = (dev->height * SCALE_FACTOR);
	fb_dev_phy[idx + 3].fb_ptr = dev->buf;
	fb_dev_phy[idx + 3].depth = DEPTH;
	fb_dev_phy[idx + 3].bpp = BPP;

	do {
		if (rfb->processor(socket_fd, &si, &fb_dev_rfb) < 0)
			break;
		
		for(idx = 0;idx < 6;idx++){
			for(pos_y=0;pos_y<fb_dev_phy[idx].height;pos_y++){
				for(pos_x=0;pos_x<fb_dev_phy[idx].width;pos_x++){
					rfb_x = (uint16_t)(((float)pos_x)/SCALE_FACTOR + 0.5);
					rfb_y = (uint16_t)(((float)pos_y)/SCALE_FACTOR + 0.5);
					uint32_t px = *(fb_dev_rfb.fb_ptr + ((rfb_y * fb_dev_rfb.width) + rfb_x));
					*(fb_dev_phy[idx].fb_ptr + (((fb_dev_phy[idx].y_pos + pos_y) * dev->width) + (fb_dev_phy[idx].x_pos + pos_x))) = px;
				}
			}
		}


		if (rfb->request_changed_screen(socket_fd, &si) < 0) {
			printf("request content failed....\n");
			break;
		}
	} while (1);
	rfb->close_socket(socket_fd);

	/* destroy */
	drm->close(drm_fd, drm_head);

	return 0;
}

#endif
