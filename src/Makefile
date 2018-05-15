
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
LDFLAGS += -ljson-c 
#LDFLAGS += -llua5.1
#LDFLAGS += -ldb -ltokyocabinet -ltokyotyrant 
LDFLAGS += -liopcmisc -liopcnet

ifeq (${SUPPORT_UDS},y)
CFLAGS += -DUTILS_UDS
endif
ifeq (${SUPPORT_DB},y)
CFLAGS += -DUTILS_DB
endif
ifeq (${SUPPORT_INPUT},y)
CFLAGS += -DUTILS_INPUT
endif

SRC = 
SRC += main.c
SRC += main_uds.c
SRC += main_db.c
SRC += main_input.c

include Makefile.include.app
