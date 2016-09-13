#include "mprobe_common.h"

static int __init mprobe_init(void);
static void __exit mprobe_exit(void);


static int mprobe_open(struct inode* node, struct file* file);
static int mprobe_release(struct inode* node, struct file* file);
static ssize_t mprobe_read(struct file *file, char *buf, size_t count, loff_t *ptr);
static ssize_t mprobe_write(struct file *file, const char __user *buf, size_t count, loff_t *ptr);
long mprobe_unlocked_ioctl (struct file *file, unsigned int req1, unsigned long req2);
static int __init mprobe_init(void);
static void __exit mprobe_exit(void);

static void bzero(void* sl, unsigned long n) {
    register char *t = sl;
    while(n != 0) {
        *t++ = 0;
        n --;
    }
}
