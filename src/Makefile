
TARGET = iopctest

CFLAGS += -I$(SDKSTAGE)/usr/include/libiopcdao
CFLAGS += -I$(SDKSTAGE)/usr/include/libiopcmisc
CFLAGS += -I$(SDKSTAGE)/usr/include/libiopctask
CFLAGS += -I$(SDKSTAGE)/usr/include/
CFLAGS += -I./include/

LDFLAGS += -L$(CC_SYSROOT)/usr/lib/ 
LDFLAGS += -L$(SDKSTAGE)/usr/lib/ 
LDFLAGS += -L$(SDKSTAGE)/lib/ 
LDFLAGS += -lc -lpthread -lrt -lz -lbz2
LDFLAGS += -ldb -ltokyocabinet -ltokyotyrant -llua5.1
LDFLAGS += -liopcdao -liopcmisc -liopctask


SRC = 
SRC += main.c

ifeq ($(SUPPORT_PLATFORM_AS_HOST),y)
CFLAGS += -DSUPPORT_PLATFORM_AS_HOST
endif
ifeq ($(SUPPORT_PLATFORM_AS_GUEST),y)
endif


include Makefile.include.app
