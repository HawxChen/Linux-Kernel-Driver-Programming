#include"HCSR_kernel_common.h"
#define MODULE_NAME ("HCSR_DRIVER")
static const struct platform_device_id HCSR_id_table[] = {
// Name, driver_data,
    {"HCSR_1", 0},
    {"HCSR_2", 0},
    {},
};

static int HCSR_driver_probe(struct platform_device* pplat_dev) {
    HCSR_device* pdev = NULL;
    printk(KERN_ALERT "%s: Probe: INIT\n", MODULE_NAME);
//    pdev = container_of(pplat_dev, HCSR_device, plat_dev);
//    printk(KERN_ALERT "I got the device: %s\n", pdev->name);

    printk(KERN_ALERT "%s: Probe: INIT Done\n", MODULE_NAME);
    return 0;
}

static int HCSR_driver_remove(struct platform_device* pplat_dev) {
    printk(KERN_ALERT "%s: REMOVE\n", MODULE_NAME);

    printk(KERN_ALERT "%s: REMOVE Done\n", MODULE_NAME);
    return 0;
} 

static struct platform_driver HCSR_driver = {
    .driver = {
        .name = MODULE_NAME, 
        .owner = THIS_MODULE,
    },
    .probe = HCSR_driver_probe,
    .remove = HCSR_driver_remove,
    .id_table = HCSR_id_table,
};

moudle_paltform_driver(HCSR_driver);
MODULE_LICENSE("GPL");
