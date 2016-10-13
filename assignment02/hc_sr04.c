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
#include<linux/ktime.h>
#include<linux/sched.h>
#include"hc_sr04_kernel.h"

static struct file_operations hc_sr04_fops = {
    .owner = THIS_MODULE,
    .open = hc_sr04_open,
    .read = hc_sr04_read,
    .write = hc_sr04_write,
    .unlocked_ioctl = hc_sr04_ioctl,
    .release = hc_sr04_release,
};
static struct miscdevice hc_sr04_A = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = "HCSR_1",
    .fops = &hc_sr04_fops,
};

static struct miscdevice hc_sr04_B = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = "HCSR_2",
    .fops = &hc_sr04_fops,
};

hcsr_struct hcA;
hcsr_struct hcB;

static LIST_HEAD(hc_list);
static char irq_done = IRQ_NOT_DONE;
static int ongoing;
ktime_t kstart;
static int echo_irq = -1;
static hcsr_kconfig kconfig;


module_init(hc_sr04_init);
module_exit(hc_sr04_exit);

struct hcsr_struct* get_curr_hcsr(struct inode* node) {
    int minor;
    hcsr_struct *c;
    minor = iminor(node);
    list_for_each_entry(c, &hc_list, list) {
        if(c->hc_sr04->minor == minor) {
            return c;
        }
    }
    return NULL;
}

static int hc_sr04_open(struct inode* node, struct file* file) {
    struct hcsr_struct* curr_hcsr;
    printk(KERN_ALERT "hc_sr04: Open\n");
    curr_hcsr = get_curr_hcsr(node);
    printk(KERN_ALERT "Open: %s", curr_hcsr->hc_sr04->name);
    printk(KERN_ALERT "hc_sr04: Open Done\n");
    return 0;
}

static int hc_sr04_release(struct inode* node, struct file* file) {
    printk(KERN_ALERT "hc_sr04: Release\n");
    printk(KERN_ALERT "hc_sr04: Release Done\n");
    return 0;
}

static int send(void) {
    unsigned int counter = 0;
    gpio_set_value(TRIGGER_PIN, 1);
    udelay(10);
    gpio_set_value(TRIGGER_PIN, 0);
    kstart = ktime_get();

    ongoing = 1;
    
    while(1 == ongoing) {
        if(++counter > 50000) {
            ongoing = 0;
            return -EAGAIN;
        }
        udelay(1);
    }
    return 0;
}

static irqreturn_t echo_recv_isr(int irq, void *data) {
    //Only one isr_handler in the same time: IRQF_DISABLED, enable it at request_irq.
    ktime_t kend;
    unsigned long diff;

    if(!ongoing) {
        //printk(KERN_ALERT "!!! Signal from Dark Force !!!");
        return IRQ_HANDLED;
    }
    kend = ktime_get();
    diff = ktime_to_ns(ktime_sub(kend, kstart));
    printk(KERN_ALERT "echo_recv_isr: %ld\n", diff);

    /*
       add to buffer...
     */
    ongoing = 0;
    return IRQ_HANDLED;
}

static long ioctl_SETPINs(struct file* file, unsigned long addr) {
    pin_set pins;
    int ret = 0;
    printk(KERN_ALERT "ioctl_SETPIN\n");
    if(0 != copy_from_user(&pins, (void*)addr, sizeof(pin_set))) {
        printk(KERN_ALERT "SETPIN: copy_from_user ERROR\n");
        ret = -EAGAIN;
        goto ERR_SETPIN_RETURN;
    }

    if(TRIGGER_PIN != pins.trigger_pin  || ECHO_PIN != pins.echo_pin) {
        ret = -EINVAL;
        goto ERR_SETPIN_RETURN;
    }

    if(irq_done) {
        goto SUCCESS_SETPIN_RETURN;
    }

    //For Trigger
    ret = gpio_request(TRIGGER_PIN, "HCSR_TRIGGER");
    if(ret) {
        printk(KERN_ALERT "SETPIN: gpio_request,pin:%d, ret=%d\n", TRIGGER_PIN, ret);
        goto ERR_SETPIN_RETURN;
    }
    ret = gpio_direction_output(TRIGGER_PIN, 0);
    if(ret) {
        printk(KERN_ALERT "SETPIN: gpio_dir_output:%d\n", ret);
        goto ERR_SETPIN_RETURN;
    }

    //For Echo
    ret = gpio_request(ECHO_PIN, "HCSR_ECHO");
    if(ret) {
        printk(KERN_ALERT "SETPIN: gpio_request,pin:%d, ret=%d\n", ECHO_PIN, ret);
        goto ERR_SETPIN_RETURN;
    }
    ret = gpio_direction_input(ECHO_PIN);
    if(ret) {
        printk(KERN_ALERT "SETPIN: gpio_dir_input:%d\n", ret);
        goto ERR_SETPIN_RETURN;
    }

    //For Echo ISR
    ret=gpio_to_irq(ECHO_PIN);
    if(0 > ret) {
        printk(KERN_ALERT "SETPIN: gpio_to_irq:%d\n", ret);
        goto ERR_SETPIN_RETURN;
    } else {
        echo_irq = ret;
    }

    ret = request_irq(echo_irq, echo_recv_isr, IRQ_FLAG, "HC-SR04_Echo", NULL);
    if(ret) {
        printk(KERN_ALERT "SETPIN: request_irq:%d\n", ret);
        goto ERR_SETPIN_RETURN;
    }

    irq_done = IRQ_DONE;
    goto SUCCESS_SETPIN_RETURN;

ERR_SETPIN_RETURN:
        return ret;

SUCCESS_SETPIN_RETURN:
    printk(KERN_ALERT "ioctl_SETPIN DONE\n");
    return 0;
}


static long ioctl_SETMODE(struct file* file, unsigned long addr) {
    int ret = 0;
    hcsr_set set;

    printk(KERN_ALERT "ioctl_SETMODE\n");
    if(0 != copy_from_user(&set, (void*)addr, sizeof(hcsr_set))) {
        printk(KERN_ALERT "SETPIN: copy_from_user ERROR\n");
        ret = -EAGAIN;
        goto ERR_SETMODE_RETURN;
    }
    
    switch (set.mode){
        case 0:
        case 1:
            break;
        default:
            ret = -EINVAL;
            goto ERR_SETMODE_RETURN;
    }
    spin_lock(&(kconfig.kconfig_lock)); {
        // still peeriodical sampling
        //before: one-shot ; after: one-shot
        kconfig.set = set;
        spin_unlock(&(kconfig.kconfig_lock));
    }
    goto SUCCESS_SETMODE_RETURN;

ERR_SETMODE_RETURN:
        return ret;

SUCCESS_SETMODE_RETURN:
    printk(KERN_ALERT "ioctl_SETMODE DONE\n");
    return 0;
}

static long hc_sr04_ioctl(struct file* file, unsigned int arg1, unsigned long arg2) {
    printk(KERN_ALERT "ioctl\n");
    switch (arg1) {
        case SETPINS:
            return ioctl_SETPINs(file, arg2);
        case SETMODE:
            return ioctl_SETMODE(file, arg2);
        default: 
            return -EINVAL;
    }

    printk(KERN_ALERT "ioctl DONE\n");
    return 0;
}
static ssize_t hc_sr04_read(struct file *file, char *buf, size_t count, loff_t *ptr) {
    printk(KERN_ALERT "hc_sr04: Read\n");
    spin_lock(&(kconfig.kconfig_lock));
    if(ONE_SHOT == kconfig.set.mode) {
        spin_unlock(&(kconfig.kconfig_lock));
        if(0 == ongoing) {
            send();
        }
        //take the newest one into its own buffer!
        //copy_to_user
    } else if(PERIODIC == kconfig.set.mode) {
        spin_unlock(&(kconfig.kconfig_lock));
    } else {
        spin_unlock(&(kconfig.kconfig_lock));
    }
    printk(KERN_ALERT "hc_sr04: Read Done\n");
    return 0;
}

static ssize_t hc_sr04_write(struct file *file, const char __user *buf, size_t count, loff_t *ptr) {
    printk(KERN_ALERT "hc_sr04: Write\n");
    //copy_from_user..
    spin_lock(&(kconfig.kconfig_lock));
    if(ONE_SHOT == kconfig.set.mode) {
        spin_unlock(&(kconfig.kconfig_lock));
        if(0 == ongoing) {
            send();
        }
    } else if(PERIODIC == kconfig.set.mode) {
        spin_unlock(&(kconfig.kconfig_lock));
    } else {
        spin_unlock(&(kconfig.kconfig_lock));
    }

    printk(KERN_ALERT "hc_sr04: Write Done\n");
    return 0;
}


static int reg_misc(struct miscdevice* md) {
    int error;
    error = misc_register(md);
    if(error)
        printk(KERN_ALERT "Reg md Error:%s\n", md->name);

    return error;
}
static void dereg_misc(struct miscdevice* md) {
    misc_deregister(md);
}

static void init_hcsr_struct(hcsr_struct* hc, struct miscdevice* md) {
    hc->hc_sr04 = md;
    INIT_LIST_HEAD(&(hc->list));
    list_add(&(hc->list), &hc_list);
}

static int __init hc_sr04_init(void) {
    printk(KERN_ALERT "hc_sr04: INIT\n");
    if( reg_misc(&hc_sr04_A) || reg_misc(&hc_sr04_B) )
        return 0;

    init_hcsr_struct(&hcA, &hc_sr04_A);
    init_hcsr_struct(&hcB, &hc_sr04_B);

    spin_lock_init(&(kconfig.kconfig_lock));
    spin_lock_init(&(hcA.record.record_lock));
    spin_lock_init(&(hcB.record.record_lock));
    kconfig.set.mode = -1;
    

    printk(KERN_ALERT "hc_sr04: INIT DONE\n");
    return 0;
}

static void __exit hc_sr04_exit(void) {
    printk(KERN_ALERT "hc_sr04: GoodBye Kernel World!!!\n");

    if(irq_done) {
        if(0 < echo_irq)
            free_irq(echo_irq, NULL);
    }

    gpio_free(TRIGGER_PIN);
    gpio_free(ECHO_PIN);

    dereg_misc(&hc_sr04_A);
    dereg_misc(&hc_sr04_B);
    printk(KERN_ALERT "hc_sr04: EXIT DONE\n");
    return;
} 

MODULE_LICENSE("GPL");
