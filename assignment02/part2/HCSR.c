#include"HCSR_kernel.h"

#define MODULE_NAME ("HCSR")

static dev_t plat_dev_devnum;
static struct file_operations plat_device_fops = {
    .owner = THIS_MODULE,
};

static HCSR_device HCSR_1_dev = {
    .name = "HCSR_1_Device",
		.dev_no 	= 20,
    .plat_dev = {
        .name = "HCSR_1",
        .id = -1,
    }
};

static HCSR_device HCSR_2_dev = {
    .name = "HCSR_2_Device",
		.dev_no 	= 55,
    .plat_dev = {
        .name = "HCSR_2",
        .id = -1,
    }
};

static int __init plat_device_init(void) {
    int ret;
    printk(KERN_ALERT "plat_device_init: INIT\n");
    /*
    if(0 > alloc_chrdev_region(&plat_dev_devnum, MINOR_BASE, MINOR_COUNT, MODULE_NAME)) {
        printk(KERN_ALERT "Error: alloc_chrdev_region");
        return -EINVAL;
    } else {
        printk(KERN_ALERT "plat_dev: devnum:%d, Major:%d, Minor:%d\n", plat_dev_devnum,MAJOR(plat_dev_devnum), MINOR(plat_dev_devnum));
    }

    plat_dev_class = class_create(THIS_MODULE, MODULE_NAME);

    cdev_init(&plat_dev_cdev, &plat_device_fops);
    plat_dev_cdev.owner = THIS_MODULE;

    ret = cdev_add(&plat_dev_cdev, plat_dev_devnum, MINOR_COUNT);
    if(ret) {
        printk("Bad Registration\n");
        return ret;
    }
    */

    //device_create(plat_dev_class, NULL, MKDEV(MAJOR(plat_dev_devnum),0), NULL, DEVICE_NANME);
    //ret = platform_device_register(&(HCSR_1_dev.plat_dev));
    ret = platform_device_register(&(HCSR_1_dev.plat_dev));
    if(ret) {
        printk(KERN_ALERT "Failed device registration: HCSR_1\n");
    }

    ret = platform_device_register(&(HCSR_2_dev.plat_dev));
    if(ret) {
        printk(KERN_ALERT "Failed device registration: HCSR_2\n");
    }


    printk(KERN_ALERT "plat_device_init: INIT DONE\n");
    return 0;
}

static void __exit plat_device_exit(void) {
    printk(KERN_ALERT "plat_device: GoodBye Kernel World!!!\n");

    platform_device_unregister(&(HCSR_1_dev.plat_dev));
    platform_device_unregister(&(HCSR_1_dev.plat_dev));
   // platform_driver_unregister(&(HCSR_1_dev.plat_dev));
   // platform_driver_unregister(&(HCSR_2_dev.plat_dev));

    /*
    unregister_chrdev_region(plat_dev_devnum, MINOR_COUNT);
    //destroy devices
    //device_destroy(plat_dev_class, MKDEV(MAJOR(plat_dev_devnum),0));

    cdev_del(&plat_dev_cdev);

    class_destroy(plat_dev_class);
    */

    printk(KERN_ALERT "plat_device: EXIT DONE\n");
    return;
} 

module_init(plat_device_init);
module_exit(plat_device_exit);
MODULE_LICENSE("GPL");
