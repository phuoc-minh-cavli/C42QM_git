##################################################################
# Copyright (c) $Date: 2009/08/04 21:39:57 $ QUALCOMM INCORPORATED.
# All Rights Reserved.
# Modified by QUALCOMM INCORPORATED on $Date: 2009/08/04 21:39:57 $
##################################################################

SHELL = /bin/bash

empty :=

# Make sure Environment variables are set up
ifndef Q6_RTOS_ROOT
$(error Q6_RTOS_ROOT not defined, please set to qurt install directory)
endif

#Find OS Environment
ifeq (CYGWIN, $(findstring CYGWIN,$(shell uname)))
OSTYPE := cygwin
else
ifeq (Linux, $(findstring Linux,$(shell uname)))
ifeq (x86_64, $(shell arch))
OSTYPE := lnx64
else
$(error Supported OS environments - cygwin, lnx64)
endif
endif
endif

ifeq ($(Q6_TOOLS_ROOT),$(empty))
ifeq (cygwin, $(OSTYPE))
$(error Q6_TOOLS_ROOT not defined)
else
Q6_TOOLS_ROOT := /pkg/qct/software/hexagon/releases/tools/5.0.01
export Q6_TOOLS_ROOT
endif
endif

cur_dir := $(shell echo "$(CURDIR)" | sed 's/^\/cygdrive\/\([a-z]\)\/\(.*\)/\1:\/\2/')
override Q6_RTOS_ROOT := "$(Q6_RTOS_ROOT)/$(TARGET)"
RTOS_LIBS_ROOT := $(Q6_RTOS_ROOT)/lib

LIB_DINKUMWARE_ROOT := "$(Q6_TOOLS_ROOT)"/dinkumware/lib/$(Q6VERSION)/G0
LIB_DSP_ROOT := "$(Q6_TOOLS_ROOT)"/qc/lib/$(Q6VERSION)/G0

CC := "$(Q6_TOOLS_ROOT)"/gnu/bin/qdsp6-gcc
LD := "$(Q6_TOOLS_ROOT)"/gnu/bin/qdsp6-ld

CFLAGS := -m$(Q6VERSION) -g -O2 -G0

INCLUDES :=-I. -I$(Q6_RTOS_ROOT)/include 

LDFLAGS_PREFIX := -m$(Q6VERSION) -g \
			-nostdlib --section-start .start=0xfe000000\
			--start-group \
                        $(LIB_DINKUMWARE_ROOT)/init.o \
                        $(RTOS_LIBS_ROOT)/crt0.o \
                        $(LIB_DINKUMWARE_ROOT)/libc.a \
                        $(LIB_DINKUMWARE_ROOT)/libqcc.a \
                        $(RTOS_LIBS_ROOT)/libqurt.a \
						$(RTOS_LIBS_ROOT)/libqurtcfs.a \
                        $(RTOS_LIBS_ROOT)/libqurtkernel.a \
                        $(RTOS_LIBS_ROOT)/libqube_compat.a \
                        $(RTOS_LIBS_ROOT)/libtimer.a \
                        $(RTOS_LIBS_ROOT)/libposix.a \
                        $(LIB_DSP_ROOT)/libqdsp.a \
                        $(BUILD_DIR)/cust_config.o

LDFLAGS := $(LIB_DINKUMWARE_ROOT)/fini.o --end-group

all: build 

build:
	python $(Q6_RTOS_ROOT)/scripts/qurt_config.py update -T "$(Q6_TOOLS_ROOT)" -o $(BUILD_DIR)/cust_config.o qdsp6_qurt.xml
	$(CC) -c $(CFLAGS) main.c $(INCLUDES) -o $(BUILD_DIR)/qurt.o
	$(LD) $(LDFLAGS_PREFIX) $(BUILD_DIR)/qurt.o $(BUILD_DIR)/cbdrv.o $(BUILD_DIR)/cbdrv_qdi.o $(LDFLAGS) -o $(BUILD_DIR)/qurt.elf

clean:
	rm -rf build

.PHONY: build
