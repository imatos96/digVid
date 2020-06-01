CROSS_COMPILE=$(TOOLCHAIN_CROSS_COMPILE)

CC_PREFIX=$(CROSS_COMPILE)-
CC=$(CC_PREFIX)gcc
CXX=$(CC_PREFIX)g++
LD=$(CC_PREFIX)ld

SYSROOT=$(SDK_ROOTFS)
GALOIS_INCLUDE=$(SDK_GALOIS)

INCS =	-I./../tdp_api
INCS += -I./include/ 							\
		-I$(SYSROOT)/usr/include/         \
		-I$(GALOIS_INCLUDE)/Common/include/     \
		-I$(GALOIS_INCLUDE)/OSAL/include/		\
		-I$(GALOIS_INCLUDE)/OSAL/include/CPU1/	\
		-I$(GALOIS_INCLUDE)/PE/Common/include/  \
        -I$(SYSROOT)/usr/include/directfb/

LIBS_PATH = -L./../tdp_api

LIBS_PATH += -L$(SYSROOT)/home/galois/lib/

LIBS_PATH += -L$(SYSROOT)/home/galois/lib/directfb-1.4-6-libs

LIBS := $(LIBS_PATH) -ltdp

LIBS += $(LIBS_PATH) -lOSAL	-lshm -lPEAgent

LIBS += $(LIBS_PATH) -ldirectfb -ldirect -lfusion -lrt

CFLAGS += -D__LINUX__ -O0 -Wno-psabi --sysroot=$(SYSROOT)

CXXFLAGS = $(CFLAGS)

all: clean tv_app

SRCS =  ./main.c
SRCS += ./config.c
SRCS += ./player.c
SRCS += ./parser.c
SRCS += ./remote.c
SRCS += ./graphics.c

tv_app:
	$(CC) -o tv_app $(INCS) $(SRCS) $(CFLAGS) $(LIBS)
	cp tv_app ~/pputvios1/ploca/tv_app
    
clean:
	rm -f tv_app
