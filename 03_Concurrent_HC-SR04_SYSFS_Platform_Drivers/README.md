*   This work includes two parts:
    *   Two HC-SR04 driver instances registered through miscdevice are equipped with two modes of one-shot and periodical sampling. This kernel module can operate two HC-SR04 sensors in the same time.
    *   The part2 exports HC-SR04 module to the SYSFS interface, implemented through Platform Driver.

*   Both parts have testing programs and README.
# KernelRocks

Setup:
On Gen2 Board:
#mkdir -p /lib/modules/3.8.7-yocto-standard/kernel/drivers/HCSR

Makefile:
KDIR should be the path of kernel source directory

Install it
  #make
  #make inall
  #make depmod
  #make inall

Uninstall:
  #make unall

Demo for part2:
 #./HCSR-test
 #./test.sh
 #dmesg


#Demo Record
#kernel_run.log
