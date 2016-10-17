NAME = hc_sr04
TESTER_NAME = $(NAME)-test
TESTER_FLAGS = -Wall -g -lpthread -std=c99

IOT_HOME = /opt/iot-devkit/1.7.3/sysroots
#KDIR := /root/source/galileo/galileo-linux-stable_backup
KDIR := /root/VM_SHARE/kernel-header-3.8
CORSS_COMPILE = i586-poky-linux-
SROOT  = $(IOT_HOME)/i586-poky-linux-
PWD := $(shell pwd)
ARCH = i386


obj-m := $(NAME).o
#obj-m += $(NAME_MPROBE).o
PWD := $(shell pwd)
XBIN := $(IOT_HOME)/x86_64-pokysdk-linux/usr/bin/i586-poky-linux
PATH := $(PATH):$(XBIN)
XCC = $(XBIN)/i586-poky-linux-gcc

.PHONY: all 
all: $(TESTER_NAME) 
	make ARCH=$(ARCH) CORSS_COMPILE=$(CORSS_COMPILE) -C $(KDIR) SUBDIRS=$(PWD) modules

$(TESTER_NAME): $(TESTER_NAME).c $(NAME)_user.h $(NAME)_common.h $(NAME)_kernel.h common_user.h
	$(XCC) $(TESTER_FLAGS) -o $(TESTER_NAME) $(TESTER_NAME).c -lpthread


.PHONY: install uninstall dellog reinstall
install:
	cp -f `pwd`/hc_sr04.ko /lib/modules/3.8.7-yocto-standard/kernel/drivers/hc_sr04/hc_sr04.ko 
	modprobe hc_sr04
uninstall:
	modprobe -r hc_sr04

reinstall:
	modprobe -r hc_sr04
	modprobe hc_sr04

uninstall_hc_sr04:
	modprobe -r hc_sr04

reinstall_hc_sr04:
	cp -f `pwd`/hc_sr04.ko /lib/modules/3.16.0-4-amd64/kernel/drivers/hc_sr04/hc_sr04.ko 
	modprobe -r hc_sr04
	modprobe --force hc_sr04

install_mprobe:
	cp -f `pwd`/mprobe.ko /lib/modules/3.16.0-4-amd64/kernel/drivers/hc_sr04/mprobe.ko 
	modprobe --force mprobe

uninstall_mprobe:
	modprobe -r mprobe

reinstall_mprobe:
	cp -f `pwd`/mprobe.ko /lib/modules/3.16.0-4-amd64/kernel/drivers/hc_sr04/mprobe.ko 
	modprobe -r mprobe 
	modprobe --force mprobe

install_all: install_hc_sr04 install_mprobe

reinstall_all: reinstall_hc_sr04 reinstall_mprobe

uninstall_all: uninstall_hc_sr04 uninstall_mprobe

dellog:
	cat /dev/null >  /var/log/kern.log
	cat /dev/null >  /var/log/syslog
.PHONY: clean
clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean
	rm -f $(TESTER_NAME)
depmod:
	depmod
