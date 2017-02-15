#On Galileo
TESTER_USER_NAME = barrier_user_sys
#TESTER_FLAGS = -Wall -g -lpthread -D_BARRIER_MODULE_BUILD_
TESTER_FLAGS = -Wall -g -lpthread

IOT_HOME = /opt/iot-devkit/1.7.3/sysroots
#KDIR := /root/source/galileo/galileo-linux-stable_backup
KDIR := /root/VM_SHARE/kernel-header-3.8
CORSS_COMPILE = i586-poky-linux-
SROOT  = $(IOT_HOME)/i586-poky-linux-
PWD := $(shell pwd)
ARCH = x86

PWD := $(shell pwd)
XBIN := $(IOT_HOME)/x86_64-pokysdk-linux/usr/bin/i586-poky-linux
PATH := $(PATH):$(XBIN)
XCC = $(XBIN)/i586-poky-linux-gcc

.PHONY: all 
all: $(TESTER_USER_NAME)

$(TESTER_USER_NAME): $(TESTER_USER_NAME).c $(TESTER_USER_NAME).c
	$(XCC) $(TESTER_FLAGS) -o $(TESTER_USER_NAME) $(TESTER_USER_NAME).c -lpthread

.PHONY: install uninstall dellog reinstall
test:
	./$(TESTER_USER_NAME)
dellog:
	cat /dev/null >  /var/log/kern.log
	cat /dev/null >  /var/log/syslog
.PHONY: clean
clean:
	rm -f $(TESTER_USER_NAME)
depmod:
	depmod
