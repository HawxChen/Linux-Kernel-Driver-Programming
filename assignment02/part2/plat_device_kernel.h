#ifndef _PLAT_DEVICE_KERNEL_H_
#define _PLAT_DEVICE_KERNEL_H_

#include <linux/spinlock.h>
#include <linux/wait.h>
#include "plat_device_common.h"
static int __init plat_device_init(void);
static void __exit plat_device_exit(void);
#endif
