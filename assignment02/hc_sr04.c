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
#include"hc_sr04_kernel.h"

static struct file_operations hc_sr04_fops = {
    .owner = THIS_MODULE,
    .open = hc_sr04_open,
    .read = hc_sr04_read,
    .write = hc_sr04_write,
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

hc_struct hcA;
hc_struct hcB;

static LIST_HEAD(hc_list);

module_init(hc_sr04_init);
module_exit(hc_sr04_exit);

static int hc_sr04_open(struct inode* node, struct file* file) {
    int minor;
    hc_struct *c;
    printk(KERN_ALERT "hc_sr04: Open\n");
    minor = iminor(node);

    list_for_each_entry(c, &hc_list, list) {
        if(c->hc_sr04->minor == minor) {
            break;
        }
    }
    printk(KERN_ALERT "Open: %s", c->hc_sr04->name);
    printk(KERN_ALERT "hc_sr04: Open Done\n");
    return 0;
}

static int hc_sr04_release(struct inode* node, struct file* file) {
    printk(KERN_ALERT "hc_sr04: Release\n");
    printk(KERN_ALERT "hc_sr04: Release Done\n");
    return 0;
}

static ssize_t hc_sr04_read(struct file *file, char *buf, size_t count, loff_t *ptr) {

    printk(KERN_ALERT "hc_sr04: Read\n");
    printk(KERN_ALERT "hc_sr04: Read Done\n");

    return 0;
}

static ssize_t hc_sr04_write(struct file *file, const char __user *buf, size_t count, loff_t *ptr) {
    printk(KERN_ALERT "hc_sr04: Write\n");
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

static void init_hc_struct(hc_struct* hc, struct miscdevice* md) {
    hc->hc_sr04 = md;
    INIT_LIST_HEAD(&(hc->list));
    list_add(&(hc->list), &hc_list);
}

static int __init hc_sr04_init(void) {
    printk(KERN_ALERT "hc_sr04: INIT\n");
    if( reg_misc(&hc_sr04_A) || reg_misc(&hc_sr04_B) )
        return 0;

    init_hc_struct(&hcA, &hc_sr04_A);
    init_hc_struct(&hcB, &hc_sr04_B);

    printk(KERN_ALERT "hc_sr04: INIT DONE\n");
    return 0;
}

static void __exit hc_sr04_exit(void) {
    printk(KERN_ALERT "hc_sr04: GoodBye Kernel World!!!\n");
    dereg_misc(&hc_sr04_A);
    dereg_misc(&hc_sr04_B);
    printk(KERN_ALERT "hc_sr04: EXIT DONE\n");
    return;
} 

MODULE_LICENSE("GPL");
