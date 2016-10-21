#include <linux/platform_device.h>
/*Collapse

Here is a simple example to show how you can bind platform driver and device together. Please note that, if you remove the device module, platform_device_unregister will end up an error since no device release method is initialized.

There are two solutions. One is to use dynamic allocation of platform device, i.e., platform_device_alloc and platform_device_add. The other one is to initialize device release method with a nop function such as

static void nop_release(struct device *dev) {}

Also, you can check /sys/bus/platform/devices and /sys/bus/platform/drivers to see the devices and the driver the modules add to the system.
*/

#ifndef __SAMPLE_PLATFORM_H__


  
#define __SAMPLE_PLATFORM_H__

struct P_chip {
		char 			*name;
		int			dev_no;
		struct platform_device 	plf_dev;
};



#endif /* __GPIO_FUNC_H__ */
