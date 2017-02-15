NAME = eosi_barrier
TESTER_FLAGS = -Wall -g -lpthread

IOT_HOME = /opt/iot-devkit/1.7.3/sysroots
#KDIR := /root/source/galileo/galileo-linux-stable_backup
KDIR := /root/VM_SHARE/kernel-header-3.8
CORSS_COMPILE = i586-poky-linux-
SROOT  = $(IOT_HOME)/i586-poky-linux-
PWD := $(shell pwd)
ARCH = x86

obj-m := $(NAME).o
PWD := $(shell pwd)
XBIN := $(IOT_HOME)/x86_64-pokysdk-linux/usr/bin/i586-poky-linux
PATH := $(PATH):$(XBIN)
XCC = $(XBIN)/i586-poky-linux-gcc

.PHONY: all 
all: 
	make ARCH=$(ARCH) CORSS_COMPILE=$(CORSS_COMPILE) -C $(KDIR) SUBDIRS=$(PWD) modules



.PHONY: install uninstall dellog reinstall
install:
	cp -f `pwd`/eosi_barrier.ko /lib/modules/3.8.7-yocto-standard/kernel/drivers/eosi_barrier/eosi_barrier.ko 
	modprobe eosi_barrier
uninstall:
	modprobe -r eosi_barrier

reinstall:
	modprobe -r eosi_barrier
	cp -f `pwd`/eosi_barrier.ko /lib/modules/3.8.7-yocto-standard/kernel/drivers/eosi_barrier/eosi_barrier.ko 
	modprobe eosi_barrier

dellog:
	cat /dev/null >  /var/log/kern.log
	cat /dev/null >  /var/log/syslog
.PHONY: clean
clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean
depmod:
	depmod
