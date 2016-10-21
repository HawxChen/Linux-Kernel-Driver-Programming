/*
 * A sample program to show the binding of platform driver and device.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include "Sample_platform_device.h"


static struct P_chip P1_chip = {
		.name	= "xyz01",
		.dev_no 	= 20,
		.plf_dev = {
			.name	= "abcd",
			.id	= -1,
		}
};

static struct P_chip P2_chip = {
		.name	= "xyz02",
		.dev_no 	= 55,
		.plf_dev = {
			.name	= "defg",
			.id	= -1,
		}
};


/**
 * register the device when module is initiated
 */

static int p_device_init(void)
{
	int ret = 0;
	
	

	/* Register the device */
	platform_device_register(&P1_chip.plf_dev);
	
	printk(KERN_ALERT "Platform device 1 is registered in init \n");

	platform_device_register(&P2_chip.plf_dev);

	printk(KERN_ALERT "Platform device 2 is registered in init \n");
	
	return ret;
}

static void p_device_exit(void)
{
    	platform_device_unregister(&P1_chip.plf_dev);

	platform_device_unregister(&P2_chip.plf_dev);

	printk(KERN_ALERT "Goodbye, unregister the device\n");
}

module_init(p_device_init);
module_exit(p_device_exit);
MODULE_LICENSE("GPL");
