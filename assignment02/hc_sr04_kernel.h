#include "hc_sr04_common.h"
typedef struct hc_struct {
    struct miscdevice* hc_sr04;
    struct record* record;
    struct list_head list;
} hc_struct;
static int hc_sr04_open(struct inode* node, struct file* file);
static int hc_sr04_release(struct inode* node, struct file* file);
static ssize_t hc_sr04_read(struct file *file, char *buf, size_t count, loff_t *ptr);
static ssize_t hc_sr04_write(struct file *file, const char __user *buf, size_t count, loff_t *ptr);
static int __init hc_sr04_init(void);
static void __exit hc_sr04_exit(void);
