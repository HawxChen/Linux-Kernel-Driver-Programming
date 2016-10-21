#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/hashtable.h>
#include<linux/fs.h>
#include<linux/types.h>
#include<linux/uaccess.h>
#include<linux/errno.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/cdev.h>
#include<linux/kdev_t.h>
#include<linux/miscdevice.h>
#include<linux/list.h>
#include<linux/gpio.h>
#include<linux/interrupt.h>
#include<linux/delay.h>
#include<linux/kthread.h>
#include<linux/ktime.h>
#include<linux/sched.h>
#include"plat_device_kernel.h"

static struct file_operations plat_device_fops = {
    .owner = THIS_MODULE,
};

module_init(plat_device_init);
module_exit(plat_device_exit);


static int __init plat_device_init(void) {
    printk(KERN_ALERT "hc_sr05: INIT\n");


    printk(KERN_ALERT "hc_sr04: INIT DONE\n");
    return 0;
}

static void __exit plat_device_exit(void) {
    printk(KERN_ALERT "hc_sr04: GoodBye Kernel World!!!\n");

    printk(KERN_ALERT "hc_sr04: EXIT DONE\n");
    return;
} 

MODULE_LICENSE("GPL");
