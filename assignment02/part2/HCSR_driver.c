
/*
 * A sample program to show the binding of platform driver and device.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include "HCSR_kernel_common.h"
#include "hc_sr04.h"

#define MODULE_NAME ("HCSR_MODULE")

static const struct platform_device_id P_id_table[] = {
         { "HCSR_1", 0 },
         { "HCSR_2", 0 },
	 { },
};

static int HCSR_driver_probe(struct platform_device *plf_dev)
{
        HCSR_device* pdev = NULL;
        printk(KERN_ALERT "%s: Probe: INIT\n", MODULE_NAME);
	
	pdev = container_of(plf_dev, HCSR_device, plf_dev);
        hc_sr04_init(pdev);
	printk(KERN_ALERT "Probe device -- %s  %d \n", pdev->name, pdev->dev_no);

        printk(KERN_ALERT "%s: Probe: INIT Done\n", MODULE_NAME);
			
	return 0;
};

static int HCSR_driver_remove(struct platform_device *plf_dev)
{
        HCSR_device* pdev = NULL;

        printk(KERN_ALERT "%s: REMOVE\n", MODULE_NAME);

	pdev = container_of(plf_dev, HCSR_device, plf_dev);
        hc_sr04_exit(pdev);

        printk(KERN_ALERT "%s: REMOVE Done\n", MODULE_NAME);
	return 0; }; 
static struct platform_driver HCSR_driver = {
	.driver		= {
		.name	= MODULE_NAME,
		.owner	= THIS_MODULE,
	},
	.probe		= HCSR_driver_probe,
	.remove		= HCSR_driver_remove,
	.id_table	= P_id_table,
};

module_platform_driver(HCSR_driver);
MODULE_LICENSE("GPL");
