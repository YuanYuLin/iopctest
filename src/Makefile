
TARGET = iopctest

CFLAGS += -I$(SDKSTAGE)/usr/include/libiopcdao
CFLAGS += -I$(SDKSTAGE)/usr/include/libiopcmisc
CFLAGS += -I$(SDKSTAGE)/usr/include/libiopctask
CFLAGS += -I$(SDKSTAGE)/usr/include/libiopccmd
CFLAGS += -I$(SDKSTAGE)/usr/include/libiopcnet
CFLAGS += -I$(SDKSTAGE)/usr/include/libjsonc
CFLAGS += -I$(SDKSTAGE)/usr/include/
CFLAGS += -I./include/

LDFLAGS += -L$(CC_SYSROOT)/usr/lib/ 
LDFLAGS += -L$(SDKSTAGE)/usr/lib/ 
LDFLAGS += -L$(SDKSTAGE)/lib/ 
LDFLAGS += -lc -lpthread -lrt -lz -lbz2
LDFLAGS += -ljson-c -lutil
#LDFLAGS += -llua5.1
#LDFLAGS += -ldb -ltokyocabinet -ltokyotyrant 
LDFLAGS += -liopcmisc -liopcnet

ifeq ($(SUPPORT_UDS), y)
CFLAGS += -DUTILS_UDS
endif
ifeq ($(SUPPORT_DB), y)
CFLAGS += -DUTILS_DB
endif
ifeq ($(SUPPORT_INPUT), y)
CFLAGS += -DUTILS_INPUT
endif
ifeq ($(SUPPORT_LXC), y)
CFLAGS += -I$(SDKSTAGE)/usr/include/lxc
CFLAGS += -DUTILS_LXC
LDFLAGS += -llxc -lselinux -lseccomp -lcap -lapparmor
LDFLAGS += -lnettle -lhogweed -lgmp -lidn -ltasn1 -lp11-kit -lffi -lpcre -lgnutls
endif
ifeq ($(SUPPORT_IOPCDRM), y)
CFLAGS += -I$(SDKSTAGE)/usr/include/libdrm
CFLAGS += -I$(SDKSTAGE)/usr/include/libdrm/libdrm
CFLAGS += -I$(SDKSTAGE)/usr/include/libiopcdrm
CFLAGS += -DUTILS_DRM
LDFLAGS += -liopcdrm -ldrm
endif
#ifeq ($(SUPPORT_RFB), y)
#CFLAGS += -I$(SDKSTAGE)/usr/include/libiopcrfb
#CFLAGS += -DUTILS_RFB
#LDFLAGS += -liopcrfb
#endif

ifeq ($(SUPPORT_QMP), y)
CFLAGS += -DUTILS_QMP
endif
CFLAGS += -DUTILS_QEMU

SRC = 
SRC += main.c
SRC += main_uds.c
SRC += main_db.c
SRC += main_input.c
SRC += main_lxc.c
SRC += main_drm.c
SRC += main_rfb.c
SRC += main_qmp.c
SRC += main_qemumonitor.c
SRC += main_unittest.c
SRC += main_md.c

include Makefile.include.app
