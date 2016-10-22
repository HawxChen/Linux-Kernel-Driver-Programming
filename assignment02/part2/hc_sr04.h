#ifndef _HC_SR_04_H_
#define _HC_SR_04_H_
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/hashtable.h>
#include<linux/fs.h>
#include<linux/types.h>
#include<linux/uaccess.h>
#include<linux/errno.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/cdev.h>
#include<linux/kdev_t.h>
#include<linux/miscdevice.h>
#include<linux/list.h>
#include<linux/gpio.h>
#include<linux/interrupt.h>
#include<linux/delay.h>
#include<linux/kthread.h>
#include<linux/ktime.h>
#include<linux/sched.h>
#include"hc_sr04_kernel.h"

static ssize_t trigger_show(struct device *dev,
        struct device_attribute *attr,
        char *buf) {
    return 0;
}

static ssize_t trigger_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count) {
    //SET_TRIG_PIN
    return 0;
}

static ssize_t echo_show(struct device *dev,
        struct device_attribute *attr,
        char *buf) {
    //SET_ECHO_PIN
    return 0;
}
static ssize_t echo_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count) {
    return 0;
}

static ssize_t mode_show(struct device *dev,
        struct device_attribute *attr,
        char *buf) {
    return 0;
}
static ssize_t mode_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count) {
    return 0;
}

static ssize_t frequency_show(struct device *dev,
        struct device_attribute *attr,
        char *buf) {
    return 0;
}
static ssize_t frequency_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count) {
    //SET_FREQ
    return 0;
}

static ssize_t enable_show(struct device *dev,
        struct device_attribute *attr,
        char *buf) {
    //SET RUN
    return 0;
}
static ssize_t enable_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count) {
    //SET STOP
    return 0;
}



static ssize_t distance_show(struct device *dev,
        struct device_attribute *attr,
        char *buf) {
    //SHOW Result
    return 0;
}



static struct class* HCSR_class = NULL;
static int class_cnt = 0;


static void init_hcsr_struct(hcsr_struct* hcsr, char(*pins)[5][2], char*(*pin_str)[5]) {
    /*!!!!!*/
    //CHECK!!! INIT
    init_waitqueue_head(&(hcsr->wq));
    hcsr->hcsr_class = HCSR_class;
    hcsr->kthread = NULL;
    hcsr->irq_done = IRQ_NOT_DONE;
    hcsr->echo_isr_number = -1;
    hcsr->pins = pins;
    hcsr->kconfig.set.mode = 0;//__DEBUG__ ********
//    hcsr->kconfig.set.mode = -1; 
    hcsr->cirb.newest = -1;
    hcsr->pin_str = pin_str;
    spin_lock_init(&(hcsr->irq_done_lock));
    spin_lock_init(&(hcsr->ongoing_lock));
    spin_lock_init(&(hcsr->cirb.cir_buf_lock));
    spin_lock_init(&(hcsr->kconfig.kconfig_lock));
    spin_lock_init(&(hcsr->kthread_lock));
    INIT_LIST_HEAD(&(hcsr->list));
    list_add(&(hcsr->list), &hcsr_list);
}

static void uninit_hcsr_struct(hcsr_struct* hcsr) {
    /*!!!!!*/
    //CHECK!!! INIT
    list_del(&(hcsr->list));
}
static DEVICE_ATTR(trigger, S_IRUSR | S_IWUSR, trigger_show, trigger_store);
static DEVICE_ATTR(echo, S_IRUSR | S_IWUSR, echo_show, echo_store);

static DEVICE_ATTR(mode, S_IRUSR | S_IWUSR, mode_show, mode_store);
static DEVICE_ATTR(frequency, S_IRUSR | S_IWUSR, frequency_show, frequency_store);
static DEVICE_ATTR(enable, S_IRUSR | S_IWUSR, enable_show, enable_store);

static DEVICE_ATTR(distance, S_IRUSR, distance_show, NULL);

int hc_sr04_init(struct HCSR_device* pplat_dev) {
    //CHECK!!! INIT
    int rval;
    hcsr_struct* pdev = NULL;
    printk(KERN_ALERT "hc_sr04: INIT: %s\n", pplat_dev->name);

    pdev = (hcsr_struct*) kmalloc(sizeof(hcsr_struct),GFP_KERNEL); 
    pdev->hc_sr04 = (struct miscdevice*) kmalloc(sizeof(struct miscdevice), GFP_KERNEL); 
    memset(pdev->hc_sr04, 0, sizeof(struct miscdevice));

    pplat_dev->pdev = pdev;
    pdev->pplat_dev =  pplat_dev;

    //pdev->hcsr_class = pplat_dev->hcsr_class;

    pdev->hc_sr04->minor = MISC_DYNAMIC_MINOR;
    pdev->hc_sr04->name = pplat_dev->name;
    pdev->hc_sr04->fops = &hc_sr04_fops;

    if(reg_misc(pdev->hc_sr04))
        return 0;

    rval = alloc_chrdev_region(&(pdev->devt), 1, 1, pplat_dev->plf_dev.name);
    if (rval != 0)          /* error */
        goto cdev_alloc_err;

    /* Registring */
    pdev->hcsr_cdev = cdev_alloc();
    if (!pdev->hcsr_cdev) 
        goto cdev_alloc_err;

    /* Init it! */
    cdev_init(pdev->hcsr_cdev, &hc_sr04_fops); 

    /* Tell the kernel "hey, I'm exist" */
    rval = cdev_add(pdev->hcsr_cdev, pdev->devt, 1);
    if (rval < 0) 
        goto cdev_add_out;


    if(0 == class_cnt) {
        HCSR_class = class_create(THIS_MODULE, "HCSR");
    }
    class_cnt+=1;

    //default
    init_hcsr_struct(pdev, A_pins, A_pin_str);

    pdev->dev = device_create(pdev->hcsr_class, NULL, pdev->devt, pdev, pplat_dev->plf_dev.name);

    device_create_file(pdev->dev, &dev_attr_trigger);
    device_create_file(pdev->dev, &dev_attr_echo);

    device_create_file(pdev->dev, &dev_attr_mode);
    device_create_file(pdev->dev, &dev_attr_frequency);
    device_create_file(pdev->dev, &dev_attr_enable);

    device_create_file(pdev->dev, &dev_attr_distance);

    printk(KERN_ALERT "hc_sr04: INIT DONE: %s\n", pplat_dev->name);
    return 0;

cdev_add_out:
cdev_alloc_err:
    printk("------ERROR------\n");
    return -1;
}

void free_gpio(struct hcsr_struct* hcsr) {
    int i, j;
    for(i = 0; i < 2; i++) {
        for(j = 0; j < PIN_SIZE; j++) {
            if(-1 == hcsr->pins[i][j][PIN_INDEX]) {
                continue;
            }
            //gpio_set_value(hcsr->pins[i][j][PIN_INDEX],0);
            printk(KERN_ALERT "gpinfree:%d\n", hcsr->pins[i][j][PIN_INDEX]);
            gpio_free(hcsr->pins[i][j][PIN_INDEX]);
        }
    }
}

void hc_sr04_exit(struct HCSR_device* pplat_dev) {

    hcsr_struct *pdev = (hcsr_struct *)(pplat_dev->pdev);
    printk(KERN_ALERT "hc_sr04: GoodBye Kernel World!!!: %s\n", pdev->hc_sr04->name);

    free_irq(pdev->echo_isr_number, pdev);
    free_gpio(pdev);  

    device_destroy(pdev->hcsr_class, pdev->devt);


    class_cnt--;
    if(class_cnt == 0){
        class_destroy(pdev->hcsr_class);
        printk(KERN_ALERT "Destroy HCSR_class");
    } 

    cdev_del(pdev->hcsr_cdev);

    unregister_chrdev_region(pdev->devt, 1);

    dereg_misc((pdev->hc_sr04));
    uninit_hcsr_struct(pdev);

    kfree(pdev->hc_sr04);
    kfree(pdev);

    printk(KERN_ALERT "hc_sr04: EXIT DONE\n");
    return;
} 
#endif
