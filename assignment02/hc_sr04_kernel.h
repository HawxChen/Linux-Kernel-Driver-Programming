#ifndef _HC_SR04_KERNEL_H_
#define _HC_SR04_KERNEL_H_

#include "hc_sr04_common.h"
#include "hw_setting.h"
#include <linux/spinlock.h>
#include <linux/wait.h>
#define BUFF_IN (0x00000001)
#define COUNT_MAX (3)
#define IRQ_FLAG (IRQF_TRIGGER_RISING | IRQF_DISABLED )
//#define IRQ_FLAG (IRQF_TRIGGER_RISING | IRQF_DISABLED | IRQF_TRIGGER_FALLING )

#define ONGOING (1)
#define STOPPING (0)
#define IRQ_DONE (1)
#define IRQ_NOT_DONE (0)

typedef struct cir_buf {
    int newest;
    record buf;
    spinlock_t cir_buf_lock;
} cir_buf;

typedef struct hcsr_kconfig {
    hcsr_set set;
    spinlock_t kconfig_lock;
} hcsr_kconfig;

typedef struct hcsr_struct {
    struct miscdevice* hc_sr04;
    int irq_done;
    int echo_isr_number;
    wait_queue_head_t wq;
    ktime_t kstart;
    struct task_struct* kthread;
    spinlock_t kthread_lock;
    spinlock_t irq_done_lock;
    hcsr_kconfig kconfig;
    cir_buf cirb;
    int ongoing;
    spinlock_t ongoing_lock;
    char(*pins)[5][2];
    char*(*pin_str)[5];
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
