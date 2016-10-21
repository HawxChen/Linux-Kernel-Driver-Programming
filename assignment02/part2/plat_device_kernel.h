#ifndef _PLAT_DEVICE_KERNEL_H_
#define _PLAT_DEVICE_KERNEL_H_
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
#include<linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#include "plat_device_common.h"
#define DEVICE_NAME ("HCSR_DEVICE")
#define MINOR_BASE (0)
#define MINOR_COUNT (5)
static struct class* plat_dev_class;
struct cdev plat_dev_cdev;

typedef struct HCSR_device {
    char* name;
    struct platform_device plat_dev;
} HCSR_device;

static int __init plat_device_init(void);
static void __exit plat_device_exit(void);
#endif
