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
#include<linux/sysfs.h>
#include"hc_sr04_kernel.h"

static ssize_t trigger_show(struct device *dev,
        struct device_attribute *attr,
        char *buf) {
    int ret = 0;
    int trig_pin = 0;
    struct hcsr_struct* hcsr = (struct hcsr_struct*) dev_get_drvdata(dev);//dev->driver_data;

    spin_lock(&(hcsr->kconfig.kconfig_lock));
    trig_pin = hcsr->kconfig.set.pins.trigger_pin;
    spin_unlock(&(hcsr->kconfig.kconfig_lock));


    printk(KERN_ALERT "trigger_show@ %s\n", hcsr->pplat_dev->plf_dev.name);
    ret = snprintf(buf, 10, "%d\n",trig_pin); 
    printk(KERN_ALERT "trigger_show@ Done: %s", hcsr->pplat_dev->plf_dev.name);
    return ret;
}

static ssize_t trigger_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count) {
    int ret = 0;
    int trig_pin = 0;
    struct hcsr_struct* hcsr = (struct hcsr_struct*) dev_get_drvdata(dev);//dev->driver_data;

    printk(KERN_ALERT "trigger_store@ %s\n", hcsr->pplat_dev->plf_dev.name);

    ret = sscanf(buf,"%d", &trig_pin);
    if(0 >= ret) {
        printk(KERN_ALERT "sscanf error: %d, trig_pin:%d", ret, trig_pin);
        goto FAILED_SET;
    }

    ret = setTrig(hcsr, trig_pin);
    if(0 > ret) {
        printk(KERN_ALERT "setTrigf error: %d, trig_pin:%d", ret, trig_pin);
        goto FAILED_SET;
    }
    
    goto SUCCESS_SET;

FAILED_SET:
    return ret;

SUCCESS_SET:
    spin_lock(&(hcsr->kconfig.kconfig_lock));
    hcsr->kconfig.set.pins.trigger_pin = trig_pin;
    spin_unlock(&(hcsr->kconfig.kconfig_lock));
    printk(KERN_ALERT "trigger_store@ Done: %s", hcsr->pplat_dev->plf_dev.name);
    return count;
}

static ssize_t echo_show(struct device *dev,
        struct device_attribute *attr,
        char *buf) {
    int ret = 0;
    int echo_pin = 0;
    struct hcsr_struct* hcsr = (struct hcsr_struct*) dev_get_drvdata(dev);//dev->driver_data;

    spin_lock(&(hcsr->kconfig.kconfig_lock));
    echo_pin = hcsr->kconfig.set.pins.echo_pin;
    spin_unlock(&(hcsr->kconfig.kconfig_lock));

    printk(KERN_ALERT "echo_show@ %s\n", hcsr->pplat_dev->plf_dev.name);
    ret = snprintf(buf, 10, "%d\n", echo_pin); 
    printk(KERN_ALERT "echo_show@ Done: %s", hcsr->pplat_dev->plf_dev.name);
    return ret;
}

static ssize_t echo_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count) {
    int ret = 0;
    int echo_pin = 0;
    pin_set prev_pins;
    struct hcsr_struct* hcsr = (struct hcsr_struct*) dev_get_drvdata(dev);//dev->driver_data;

    printk(KERN_ALERT "echo_store@ %s\n", hcsr->pplat_dev->plf_dev.name);

    spin_lock(&(hcsr->kconfig.kconfig_lock));
    prev_pins = hcsr->kconfig.set.pins;
    ret = sscanf(buf,"%d", &echo_pin);
    if(0 >= ret) {
        printk(KERN_ALERT "sscanf error: %d, echo_pin:%d", ret, echo_pin);
        goto FAILED_SET;
    }

    ret = setEcho(hcsr, echo_pin);
    if(0 > ret) {
        printk(KERN_ALERT "setEcho error: %d, echo_pin:%d", ret, echo_pin);
        goto FAILED_SET;
    }

    hcsr->kconfig.set.pins.echo_pin = echo_pin;
    ret = set_ISR(hcsr);
    if(0 > ret) {
        printk(KERN_ALERT "set_ISR error: %d, echo_pin:%d", ret, echo_pin);
        goto FAILED_SET;
    }

    spin_lock(&(hcsr->irq_done_lock));
    hcsr->irq_done = IRQ_DONE;
    spin_unlock(&(hcsr->irq_done_lock));

    spin_lock(&(hcsr->irq_done_lock));
    hcsr->irq_done = IRQ_DONE;
    spin_unlock(&(hcsr->irq_done_lock));

    goto SUCCESS_SET;
    

FAILED_SET:
    hcsr->kconfig.set.pins = prev_pins;
    spin_unlock(&(hcsr->kconfig.kconfig_lock));
    return ret;

SUCCESS_SET:
    spin_unlock(&(hcsr->kconfig.kconfig_lock));
    printk(KERN_ALERT "echo_store@ Done: %s", hcsr->pplat_dev->plf_dev.name);
    return count;
}

static ssize_t mode_show(struct device *dev,
        struct device_attribute *attr,
        char *buf) {
    int ret = 0;
    int mode = 0;
    struct hcsr_struct* hcsr = (struct hcsr_struct*) dev_get_drvdata(dev);//dev->driver_data;

    spin_lock(&(hcsr->kconfig.kconfig_lock));
    mode = hcsr->kconfig.set.working_mode.mode;
    spin_unlock(&(hcsr->kconfig.kconfig_lock));

    printk(KERN_ALERT "mode_show@ %s\n", hcsr->pplat_dev->plf_dev.name);
    ret = snprintf(buf, 10, "%d\n", mode); 
    printk(KERN_ALERT "mode_show@ Done: %s", hcsr->pplat_dev->plf_dev.name);
    return ret;
}
static ssize_t mode_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count) {

    int ret = 0;
    int mode = 0;
    struct hcsr_struct* hcsr = (struct hcsr_struct*) dev_get_drvdata(dev);//dev->driver_data;

    printk(KERN_ALERT "mode_store@ %s\n", hcsr->pplat_dev->plf_dev.name);

    ret = sscanf(buf,"%d", &mode);
    if(ONE_SHOT != mode && PERIODIC != mode) return -EINVAL;

    if(0 >= ret) {
        printk(KERN_ALERT "sscanf error: %d, mode:%d", ret, mode);
        goto FAILED_SETMODE;
    }
    goto SUCCESS_SETMODE;

FAILED_SETMODE:
    return ret;

SUCCESS_SETMODE:
    spin_lock(&(hcsr->kconfig.kconfig_lock));
    hcsr->kconfig.set.working_mode.mode = mode;
    spin_unlock(&(hcsr->kconfig.kconfig_lock));
    printk(KERN_ALERT "mode_store@ %s Done\n", hcsr->pplat_dev->plf_dev.name);
    return count;
}

static ssize_t frequency_show(struct device *dev,
        struct device_attribute *attr,
        char *buf) {
    int ret = 0;
    int freq = 0;
    struct hcsr_struct* hcsr = (struct hcsr_struct*) dev_get_drvdata(dev);//dev->driver_data;

    spin_lock(&(hcsr->kconfig.kconfig_lock));
    freq = hcsr->kconfig.set.working_mode.freq;
    spin_unlock(&(hcsr->kconfig.kconfig_lock));

    printk(KERN_ALERT "frequency_show@ %s\n", hcsr->pplat_dev->plf_dev.name);
    ret = snprintf(buf, 10, "%d\n", freq); 
    printk(KERN_ALERT "frequency_show@ Done: %s", hcsr->pplat_dev->plf_dev.name);
    return ret;
}
static ssize_t frequency_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count) {
    int ret = 0;
    int freq = 0;
    struct hcsr_struct* hcsr = (struct hcsr_struct*) dev_get_drvdata(dev);//dev->driver_data;

    printk(KERN_ALERT "freq_store@ %s\n", hcsr->pplat_dev->plf_dev.name);

    ret = sscanf(buf,"%d", &freq);
    if(0 >= ret) {
        printk(KERN_ALERT "sscanf error: %d, freq:%d", ret, freq);
        goto FAILED_SETMODE;
    }
    if(0 == freq) return -EINVAL;

    goto SUCCESS_SETMODE;

FAILED_SETMODE:
    return ret;

SUCCESS_SETMODE:
    spin_lock(&(hcsr->kconfig.kconfig_lock));
    hcsr->kconfig.set.working_mode.freq = freq;
    spin_unlock(&(hcsr->kconfig.kconfig_lock));
    printk(KERN_ALERT "freq_store@ %s Done\n", hcsr->pplat_dev->plf_dev.name);
    return count;
}

static ssize_t enable_show(struct device *dev,
        struct device_attribute *attr,
        char *buf) {
    //SET RUN
    int ret = 0;
    int enable = 0;
    struct hcsr_struct* hcsr = (struct hcsr_struct*) dev_get_drvdata(dev);//dev->driver_data;

    spin_lock(&(hcsr->kconfig.kconfig_lock));
    enable = hcsr->kconfig.enable;
    spin_unlock(&(hcsr->kconfig.kconfig_lock));

    printk(KERN_ALERT "enbale_show@ %s\n", hcsr->pplat_dev->plf_dev.name);
    ret = snprintf(buf, 10, "%d\n", enable); 
    printk(KERN_ALERT "enable_show@ Done: %s", hcsr->pplat_dev->plf_dev.name);

    return ret;
}

static ssize_t enable_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count) {
    int ret = 0;
    int enable = 0;
    struct hcsr_struct* hcsr = (struct hcsr_struct*) dev_get_drvdata(dev);//dev->driver_data;

    printk(KERN_ALERT "enable_store@ %s\n", hcsr->pplat_dev->plf_dev.name);


    ret = sscanf(buf,"%d", &enable);
    if(0 >= ret) {
        printk(KERN_ALERT "sscanf error: %d, mode:%d", ret, enable);
        goto FAILED_SETMODE;
    }
    if(0 == enable) {
        //stop kthread
        spin_lock(&(hcsr->kthread_lock));
        if(NULL != hcsr->kthread) {
            kthread_stop(hcsr->kthread);
            hcsr->kthread = NULL;
        }
        spin_unlock(&(hcsr->kthread_lock));

        spin_lock(&(hcsr->ongoing_lock));
        hcsr->ongoing = STOPPING;
        spin_unlock(&(hcsr->ongoing_lock));
    } else if (1 == enable) {
        if(NULL != hcsr->kthread) return count;

        spin_lock(&(hcsr->kconfig.kconfig_lock));
        if(ONE_SHOT == hcsr->kconfig.set.working_mode.mode) {
            spin_unlock(&(hcsr->kconfig.kconfig_lock));
            do {
                spin_lock(&(hcsr->ongoing_lock));
                if(STOPPING == hcsr->ongoing) {
                    spin_unlock(&(hcsr->ongoing_lock));
                    send(hcsr, 5);
                    break;
                } 
                spin_unlock(&(hcsr->ongoing_lock));
            }
            while(0);
        } else{
            spin_lock(&(hcsr->kthread_lock));
            hcsr->kthread = kthread_run(thread_function, hcsr, hcsr->hc_sr04->name);
            spin_unlock(&(hcsr->kthread_lock));
        }
        //kthread , freq some issues
       //if ONE_SHOT
    } else {
        return -EINVAL;
    }
    goto SUCCESS_SETMODE;

FAILED_SETMODE:
    return ret;

SUCCESS_SETMODE:
    spin_lock(&(hcsr->kconfig.kconfig_lock));
    hcsr->kconfig.enable = enable;
    spin_unlock(&(hcsr->kconfig.kconfig_lock));
    printk(KERN_ALERT "enable_store@ %s Done\n", hcsr->pplat_dev->plf_dev.name);
    return count;
}



static ssize_t distance_show(struct device *dev,
        struct device_attribute *attr,
        char *buf) {
    //SHOW Result
    int ret, distance;
    struct hcsr_struct* hcsr = (struct hcsr_struct*) dev_get_drvdata(dev);//dev->driver_data;

    printk(KERN_ALERT "distance_show@ %s\n", hcsr->pplat_dev->plf_dev.name);

    spin_lock(&(hcsr->cirb.cir_buf_lock));
    if (-1 == hcsr->cirb.newest) {
        distance = 0;
    } else {
        distance = hcsr->cirb.buf.data[hcsr->cirb.newest];
    }
    spin_unlock(&(hcsr->cirb.cir_buf_lock));

    ret = snprintf(buf, 10, "%d\n", distance); 
    printk(KERN_ALERT "distance_show@ Done: %s", hcsr->pplat_dev->plf_dev.name);

    return ret;
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
    hcsr->pins_default = pins;
    hcsr->kconfig.set.working_mode.mode = 0;//__DEBUG__ ********
//    hcsr->kconfig.set.mode = -1; 
    hcsr->cirb.newest = -1;
    hcsr->pin_str = pin_str;
    hcsr->kconfig.set.pins.trigger_pin = -1;
    hcsr->kconfig.set.pins.echo_pin = -1;
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
    memset(pdev, 0, sizeof(hcsr_struct));
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
    freeTrig(hcsr->kconfig.set.pins.trigger_pin);
    freeEcho(hcsr->kconfig.set.pins.echo_pin);
}

void hc_sr04_exit(struct HCSR_device* pplat_dev) {

    hcsr_struct *pdev = (hcsr_struct *)(pplat_dev->pdev);
    printk(KERN_ALERT "hc_sr04: GoodBye Kernel World!!!: %s\n", pdev->hc_sr04->name);

    if(NULL != pdev->kthread) 
        kthread_stop(pdev->kthread);

    spin_lock(&(pdev->irq_done_lock));
    if(IRQ_DONE == pdev->irq_done) {
        free_irq(pdev->echo_isr_number, pdev);
        printk(KERN_ALERT "hc_sr04: Free IRQ");

    }
    free_gpio(pdev);  
    spin_unlock(&(pdev->irq_done_lock));

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
