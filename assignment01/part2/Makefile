NAME_MPROBE = mprobe
MTESTER_NAME1 = mprobe-test1
MTESTER_NAME2 = mprobe-test2
TESTER_FLAGS = -Wall -g -lpthread -std=c99

obj-m += $(NAME_MPROBE).o
PWD := $(shell pwd)
KDIR := /usr/src/linux-headers-3.16.0-4-amd64/

.PHONY: all 
all: $(MTESTER_NAME1) $(MTESTER_NAME2)
	make -C $(KDIR) SUBDIRS=$(PWD) modules

$(MTESTER_NAME1): $(MTESTER_NAME1).c mprobe_common.h mprobe_user.h common_user.h
	gcc $(TESTER_FLAGS) -o $(MTESTER_NAME1) $(MTESTER_NAME1).c

$(MTESTER_NAME2): $(MTESTER_NAME2).c mprobe_common.h mprobe_user.h common_user.h
	gcc $(TESTER_FLAGS) -o $(MTESTER_NAME2) $(MTESTER_NAME2).c

.PHONY: install uninstall dellog reinstall

install:
	cp -f `pwd`/mprobe.ko /lib/modules/3.16.0-4-amd64/kernel/drivers/ht530/mprobe.ko 
	modprobe --force mprobe

uninstall:
	modprobe -r mprobe

reinstall:
	cp -f `pwd`/mprobe.ko /lib/modules/3.16.0-4-amd64/kernel/drivers/ht530/mprobe.ko 
	modprobe -r mprobe 
	modprobe --force mprobe

dellog:
	cat /dev/null >  /var/log/kern.log
	cat /dev/null >  /var/log/syslog

.PHONY: clean
clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean
	rm -f $(MTESTER_NAME1) $(MTESTER_NAME2)
depmod:
	depmod
