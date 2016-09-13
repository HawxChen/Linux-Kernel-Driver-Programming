#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/types.h>
#include<linux/uaccess.h>
#include<linux/errno.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/cdev.h>
#include<linux/kdev_t.h>
#include "mprobe_kernel.h"

module_init(mprobe_init);
module_exit(mprobe_exit);

static struct file_operations mprobe_fops = {
    .owner = THIS_MODULE,
    .open = mprobe_open,
    .release = mprobe_release,
    .write = mprobe_write,
    .read = mprobe_read,
    .unlocked_ioctl = mprobe_unlocked_ioctl
};

static int mprobe_open(struct inode* node, struct file* file) {
    printk(KERN_ALERT "mprobe: Open\n");
    return 0;
}

static int mprobe_release(struct inode* node, struct file* file) {
    printk(KERN_ALERT "mprobe: Release\n");
    return 0;
}

//ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
static ssize_t mprobe_read(struct file *file, char *buf, size_t count, loff_t *ptr) {
    printk(KERN_ALERT "mprobe: read\n");
    printk(KERN_ALERT "mprobe: read Done\n");
    return 0;
}

//ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
static ssize_t mprobe_write(struct file *file, const char __user *buf, size_t count, loff_t *ptr) {
    printk(KERN_ALERT "mprobe: write\n");
    printk(KERN_ALERT "mprobe: write Done\n");
    return 0;
}

long mprobe_unlocked_ioctl (struct file *file, unsigned int req1, unsigned long req2) {
    printk(KERN_ALERT "mprobe: unlocked_ioctl\n");
    printk(KERN_ALERT "mprobe: unlocked_ioctl Done\n");
    return 0;
}

static dev_t Ht530_devnum;
static int __init mprobe_init(void) {
    printk(KERN_ALERT "mprobe: init\n");
    printk(KERN_ALERT "mprobe: INIT DONE\n");

    return 0;
}

static void __exit mprobe_exit(void) {
    printk(KERN_ALERT "GoodBye Kernel World!!!\n");
    printk(KERN_ALERT "mprobe: EXIT DONE\n");
    return;
} 

MODULE_LICENSE("GPL");
