/*
 * A sample program to show the binding of platform driver and device.
 */
#include"HCSR_kernel.h"

static struct HCSR_device HCSR_1_dev = {
    .name	= "HCSR_1_Device",
    .dev_no 	= 20,
    .plf_dev = {
        .name	= "HCSR_1",
        .id	= -1,
    }
};

static struct HCSR_device HCSR_2_dev = {
    .name	= "HCSR_2_Device",
    .dev_no 	= 55,
    .plf_dev = {
        .name	= "HCSR_2",
        .id	= -1,
    }
};


/**
 * register the device when module is initiated
 */
static void HCSR_dummy_release(struct device *dev) {}
static int HCSR_device_init(void)
{
    int ret = 0;
    printk(KERN_ALERT "HCSR_device_init: INIT\n");

    /* Register the device */
    HCSR_1_dev.plf_dev.dev.release = HCSR_dummy_release;
    ret = platform_device_register(&HCSR_1_dev.plf_dev);
    if(ret) {
        printk(KERN_ALERT "Failed device registration: HCSR_1\n");
    }
/*
    HCSR_2_dev.plf_dev.dev.release = HCSR_dummy_release;
    ret = platform_device_register(&HCSR_2_dev.plf_dev);
    if(ret) {
        printk(KERN_ALERT "Failed device registration: HCSR_2\n");
    }
    */

    printk(KERN_ALERT "HCSR_device_init: INIT DONE\n");
    return ret;
}

static void HCSR_device_exit(void)
{
    int ret;
    printk(KERN_ALERT "HCSR_device: GoodBye Kernel World!!!\n");
    platform_device_unregister(&HCSR_1_dev.plf_dev);

    //platform_device_unregister(&HCSR_2_dev.plf_dev);

    printk(KERN_ALERT "Goodbye, unregister the device\n");
    printk(KERN_ALERT "HCSR_device: EXIT DONE\n");

}

module_init(HCSR_device_init);
module_exit(HCSR_device_exit);
MODULE_LICENSE("GPL");
