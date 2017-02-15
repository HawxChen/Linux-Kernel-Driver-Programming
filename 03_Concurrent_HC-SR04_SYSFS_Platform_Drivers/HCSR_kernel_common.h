#ifndef _HCSR_KERNEL_COMMON_H_
#define _HCSR_KERNEL_COMMON_H_
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
#include <linux/spinlock.h>
#include <linux/wait.h>
#include<linux/platform_device.h>
typedef struct HCSR_device {
    char* name;
    int dev_no;
    //struct class* hcsr_class;
    void* pdev; //HCSR_device
    struct platform_device plf_dev;
} HCSR_device;
#endif
