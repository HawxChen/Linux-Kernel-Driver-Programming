#ifndef _HC_SR04_KERNEL_H_
#define _HC_SR04_KERNEL_H_
#include "hc_sr04_common.h"
#define IRQ_FLAG (IRQF_TRIGGER_RISING | IRQF_DISABLED )
//#define IRQ_FLAG (IRQF_TRIGGER_RISING | IRQF_DISABLED | IRQF_TRIGGER_FALLING )
#define TRIGGER_PIN (38)
#define ECHO_PIN (40)
#define IRQ_DONE (1)
#define IRQ_NOT_DONE (0)
typedef struct hcsr_struct {
    struct miscdevice* hc_sr04;
    record record;
    struct list_head list;
} hcsr_struct;
static long hc_sr04_ioctl(struct file* file, unsigned int arg1, unsigned long arg2);
static int hc_sr04_open(struct inode* node, struct file* file);
static int hc_sr04_release(struct inode* node, struct file* file);
static ssize_t hc_sr04_read(struct file *file, char *buf, size_t count, loff_t *ptr);
static ssize_t hc_sr04_write(struct file *file, const char __user *buf, size_t count, loff_t *ptr);
static int __init hc_sr04_init(void);
static void __exit hc_sr04_exit(void);
#endif
