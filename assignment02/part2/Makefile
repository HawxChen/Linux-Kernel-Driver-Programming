PLAT = HCSR
DRV = HCSR_driver
TESTER_PLAT := $(PLAT)-test
TESTER_FLAGS = -Wall -g -lpthread -std=c99

IOT_HOME = /opt/iot-devkit/1.7.3/sysroots
#KDIR := /root/source/galileo/galileo-linux-stable_backup
KDIR := /root/VM_SHARE/kernel-header-3.8
CORSS_COMPILE = i586-poky-linux-
SROOT  = $(IOT_HOME)/i586-poky-linux-
PWD := $(shell pwd)
ARCH = i386

obj-m := $(PLAT).o
obj-m += $(DRV).o
obj-m += Sample_platform_device.o
obj-m += Sample_platform_driver.o
PWD := $(shell pwd)
XBIN := $(IOT_HOME)/x86_64-pokysdk-linux/usr/bin/i586-poky-linux
PATH := $(PATH):$(XBIN)
XCC = $(XBIN)/i586-poky-linux-gcc

.PHONY: all 
all: $(TESTER_PLAT)
	make ARCH=$(ARCH) CORSS_COMPILE=$(CORSS_COMPILE) -C $(KDIR) SUBDIRS=$(PWD) modules

$(TESTER_PLAT): $(TESTER_PLAT).c $(PLAT)_user.h $(PLAT)_common.h $(PLAT)_kernel.h
	$(XCC) $(TESTER_FLAGS) -o $(TESTER_PLAT) $(TESTER_PLAT).c -lpthread


.PHONY: install uninstall dellog reinstall
inall:
	cp -f `pwd`/$(PLAT).ko /lib/modules/3.8.7-yocto-standard/kernel/drivers/$(PLAT)/$(PLAT).ko 
	cp -f `pwd`/$(DRV).ko /lib/modules/3.8.7-yocto-standard/kernel/drivers/$(PLAT)/$(DRV).ko 
	modprobe $(DRV) 
	modprobe $(PLAT) 

unall:
	modprobe -r $(DRV) 
	modprobe -r $(PLAT) 
inplat:
	cp -f `pwd`/$(PLAT).ko /lib/modules/3.8.7-yocto-standard/kernel/drivers/$(PLAT)/$(PLAT).ko 
	modprobe $(PLAT) 
unplat:
	modprobe -r $(PLAT) 

replat:
	modprobe -r $(PLAT)
	modprobe $(PLAT) 

indrv:
	cp -f `pwd`/$(DRV).ko /lib/modules/3.8.7-yocto-standard/kernel/drivers/$(DRV)/$(DRV).ko 
	modprobe $(DRV) 
undrv:
	modprobe -r $(DRV) 

redrv:
	modprobe -r $(DRV)
	modprobe $(DRV) 

dellog:
	cat /dev/null >  /var/log/kern.log
	cat /dev/null >  /var/log/syslog
.PHONY: clean
clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean
	rm -f $(TESTER_PLAT)
depmod:
	depmod
