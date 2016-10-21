#ifndef _HCSR_KERNEL_H_
#define _HCSR_KERNEL_H_
#include "HCSR_common.h"
#include "HCSR_kernel_common.h"
#define DEVICE_NAME ("HCSR_DEVICE")
#define MINOR_BASE (0)
#define MINOR_COUNT (5)
static struct class* plat_dev_class;
struct cdev plat_dev_cdev;

static int __init plat_device_init(void);
static void __exit plat_device_exit(void);
#endif
