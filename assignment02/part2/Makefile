NAME = plat_device
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
PWD := $(shell pwd)
XBIN := $(IOT_HOME)/x86_64-pokysdk-linux/usr/bin/i586-poky-linux
PATH := $(PATH):$(XBIN)
XCC = $(XBIN)/i586-poky-linux-gcc

.PHONY: all 
all: $(TESTER_NAME) 
	make ARCH=$(ARCH) CORSS_COMPILE=$(CORSS_COMPILE) -C $(KDIR) SUBDIRS=$(PWD) modules

$(TESTER_NAME): $(TESTER_NAME).c $(NAME)_user.h $(NAME)_common.h $(NAME)_kernel.h
	$(XCC) $(TESTER_FLAGS) -o $(TESTER_NAME) $(TESTER_NAME).c -lpthread


.PHONY: install uninstall dellog reinstall
install:
	cp -f `pwd`/$(NAME).ko /lib/modules/3.8.7-yocto-standard/kernel/drivers/$(NAME)/$(NAME).ko 
	modprobea $(NAME) 
uninstall:
	modprobe -r $(NAME) 

reinstall:
	modprobe -r $(NAME)
	modprobe $(NAME) 

dellog:
	cat /dev/null >  /var/log/kern.log
	cat /dev/null >  /var/log/syslog
.PHONY: clean
clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean
	rm -f $(TESTER_NAME)
depmod:
	depmod
