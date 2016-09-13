NAME = ht530
TESTER_NAME = ht530-test
TESTER_FLAGS = -Wall -g -lpthread -std=c99

obj-m := $(NAME).o
PWD := $(shell pwd)
KDIR := /root/course/eosi/kernel/linux-3.16.4/

.PHONY: all 
all: $(NAME).ko $(TESTER_NAME)

$(NAME).ko: $(NAME).c ht530_kernel.h ht530_common.h
	clear
	make -C $(KDIR) SUBDIRS=$(PWD) modules
	cp -f `pwd`/ht530.ko /lib/modules/3.16.0-4-amd64/kernel/drivers/ht530/ht530.ko 

$(TESTER_NAME): $(TESTER_NAME).c ht530_user.h ht530_common.h
	gcc $(TESTER_FLAGS) -o $(TESTER_NAME) $(TESTER_NAME).c

.PHONY: install uninstall dellog reinstall
install:
	modprobe --force ht530

uninstall:
	modprobe -r ht530

reinstall:
	modprobe -r ht530
	modprobe --force ht530

dellog:
	cat /dev/null >  /var/log/kern.log
	cat /dev/null >  /var/log/syslog
.PHONY: clean
clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean
	rm -f $(TESTER_NAME)
depmode:
	depmod
