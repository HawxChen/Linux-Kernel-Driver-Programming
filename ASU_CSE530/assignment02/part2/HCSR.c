/*
 * A sample program to show the binding of platform driver and device.
 */
#include "HCSR_kernel_common.h"

static struct HCSR_device HCSR_1_dev = {
    .name	= "HCSR_1",
    .dev_no 	= 20,
    .plf_dev = {
        .name	= "HCSRdevice1",
        .id	= -1,
    }
};

static struct HCSR_device HCSR_2_dev = {
    .name	= "HCSR_2",
    .dev_no 	= 55,
    .plf_dev = {
        .name	= "HCSRdevice2",
        .id	= -1,
    }
};


/**
 * register the device when module is initiated
 */
#define SUBSYSTEM ("HCSR")
static void HCSR_dummy_release(struct device *dev) {}
static void init_HCSR_dev(struct HCSR_device* hcsr_dev) {
    hcsr_dev->plf_dev.dev.release = HCSR_dummy_release;
    //hcsr_dev->hcsr_class = HCSR_class;
}
static int HCSR_device_init(void)
{
    int ret = 0;
    printk(KERN_ALERT "HCSR_device_init: INIT\n");

    /* Register the device */
//    HCSR_class = class_create(THIS_MODULE, SUBSYSTEM);
    init_HCSR_dev(&HCSR_1_dev);
    ret = platform_device_register(&HCSR_1_dev.plf_dev);
    if(ret) {
        printk(KERN_ALERT "Failed device registration: HCSR_1\n");
    }

    init_HCSR_dev(&HCSR_2_dev);
    ret = platform_device_register(&HCSR_2_dev.plf_dev);
    if(ret) {
        printk(KERN_ALERT "Failed device registration: HCSR_2\n");
    }
    

    printk(KERN_ALERT "HCSR_device_init: INIT DONE\n");
    return ret;
}

static void HCSR_device_exit(void)
{
    printk(KERN_ALERT "HCSR_device: GoodBye Kernel World!!!\n");
    platform_device_unregister(&HCSR_2_dev.plf_dev);
    platform_device_unregister(&HCSR_1_dev.plf_dev);

//    class_destroy(HCSR_class);

    printk(KERN_ALERT "Goodbye, unregister the device\n");
    printk(KERN_ALERT "HCSR_device: EXIT DONE\n");

}

module_init(HCSR_device_init);
module_exit(HCSR_device_exit);
MODULE_LICENSE("GPL");
