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
#include"eosi_barrier_kernel.h"

static struct file_operations eosi_barrier_fops = {
    .owner = THIS_MODULE,
    .open = eosi_barrier_open,
    .unlocked_ioctl = eosi_barrier_ioctl,
    .release = eosi_barrier_release,
};

static struct miscdevice eosi_barrier_A = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = "eosi_barrier_1",
    .fops = &eosi_barrier_fops,
};

static struct miscdevice eosi_barrier_B = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = "eosi_barrier_2",
    .fops = &eosi_barrier_fops,
};

static LIST_HEAD(barrier_list);


struct barrier_struct* get_curr_barrier(struct inode* node) {
    /*
    int minor;
    barrier_struct *c;
    minor = iminor(node);
    list_for_each_entry(c, &barrier_list, list) { if(c->eosi_barrier->minor == minor) {
            return c;
        }
    }
    */
    return NULL;
}

//init
static int eosi_barrier_open(struct inode* node, struct file* file) {
    printk(KERN_ALERT "eosi_barrier: Open\n");
    printk(KERN_ALERT "eosi_barrier: Open Done\n");
    return 0;
}

//destroy
static int eosi_barrier_release(struct inode* node, struct file* file) {
    printk(KERN_ALERT "eosi_barrier: Release\n");
    printk(KERN_ALERT "eosi_barrier: Release Done\n"); return 0;
}

static long eosi_barrier_ioctl(struct file* file, unsigned int cmd, unsigned long arg) {
    int ret;

    printk(KERN_ALERT "ioctl\n");
    switch (arg) {
        case BARRIER_INIT:
            break;
        case BARRIER_WAIT:
            break;
        case BARRIER_DESTROY:
            break;
        default: 
            return -EINVAL;
    }

    printk(KERN_ALERT "ioctl DONE\n");
    return ret;
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

static void init_barrier_struct(barrier_struct* barrier, int count, int* id) {
}

static int __init eosi_barrier_init(void) {
    /*!!!!!*/
    //CHECK!!! INIT
    printk(KERN_ALERT "hc_sr04: INIT\n");
    if( reg_misc(&eosi_barrier_A) || reg_misc(&eosi_barrier_B) )
        return 0;

//    init_hcsr_struct(&hcA, &hc_sr04_A, A_pins, A_pin_str);
//    init_hcsr_struct(&hcB, &hc_sr04_B, B_pins, B_pin_str);

    printk(KERN_ALERT "hc_sr04: INIT DONE\n");
    return 0;
}

static void __exit eosi_barrier_exit(void) {
    printk(KERN_ALERT "eosi_barrier: GoodBye Kernel World!!!\n");

    dereg_misc(&eosi_barrier_A);
    dereg_misc(&eosi_barrier_B);
    printk(KERN_ALERT "eosi_barrier: EXIT DONE\n");
    return;
} 

module_init(eosi_barrier_init);
module_exit(eosi_barrier_exit);

MODULE_LICENSE("GPL");
