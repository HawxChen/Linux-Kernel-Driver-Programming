/*
 * A sample program to show the binding of platform driver and device.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include "Sample_platform_device.h"


#define DRIVER_NAME		"platform_driver_0"

static const struct platform_device_id P_id_table[] = {
         { "abcd", 0 },
         { "defg", 0 },
         { "ijkl", 0 },
	 { },
};

static int P_driver_probe(struct platform_device *dev_found)
{
	struct P_chip *pchip;
	
	pchip = container_of(dev_found, struct P_chip, plf_dev);
	
	printk(KERN_ALERT "Found the device -- %s  %d \n", pchip->name, pchip->dev_no);
			
	return 0;
};

static int P_driver_remove(struct platform_device *pdev)
{
	
	return 0;
};

static struct platform_driver P_driver = {
	.driver		= {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
	},
	.probe		= P_driver_probe,
	.remove		= P_driver_remove,
	.id_table	= P_id_table,
};

module_platform_driver(P_driver);
MODULE_LICENSE("GPL");
