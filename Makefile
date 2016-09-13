NAME = ht530
NAME_MPROBE = mprobe
TESTER_NAME = ht530-test
TESTER_FLAGS = -Wall -g -lpthread -std=c99

obj-m := $(NAME).o $(NAME_MPROBE).o
PWD := $(shell pwd)
KDIR := /root/course/eosi/kernel/linux-3.16.4/

.PHONY: all 
all: 
	make -C $(KDIR) SUBDIRS=$(PWD) modules

$(TESTER_NAME): $(TESTER_NAME).c ht530_user.h ht530_common.h
	gcc $(TESTER_FLAGS) -o $(TESTER_NAME) $(TESTER_NAME).c

.PHONY: install uninstall dellog reinstall
install_ht530:
	cp -f `pwd`/ht530.ko /lib/modules/3.16.0-4-amd64/kernel/drivers/ht530/ht530.ko 
	modprobe --force ht530

uninstall_ht530:
	modprobe -r ht530

reinstall_ht530:
	modprobe -r ht530
	modprobe --force ht530

install_mprobe:
	cp -f `pwd`/mprobe.ko /lib/modules/3.16.0-4-amd64/kernel/drivers/ht530/mprobe.ko 
	modprobe --force mprobe

uninstall_mprobe:
	modprobe -r mprobe

reinstall_mprobe:
	modprobe -r mprobe 
	modprobe --force mprobe

install_all: install_ht530 install_mprobe

reinstall_all: reinstall_ht530 reinstall_mprobe

uninstall_all: uninstall_ht530 uninstall_mprobe

dellog:
	cat /dev/null >  /var/log/kern.log
	cat /dev/null >  /var/log/syslog
.PHONY: clean
clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean
	rm -f $(TESTER_NAME)
depmod:
	depmod
