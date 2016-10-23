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
    char(*pins_default)[5][2];
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

static irqreturn_t echo_recv_isr(int irq, void *data);
int set_ISR(struct hcsr_struct* hcsr) {
    //For Echo ISR
    int ret = 0;
    ret = gpio_to_irq(hcsr->kconfig.set.pins.echo_pin);
    if(0 > ret) {
        printk(KERN_ALERT "SETPIN: gpio_to_irq:%d\n", ret);
        return -EAGAIN;
    } else {
         hcsr->echo_isr_number = ret;
    }
    //ret = request_irq(hcsr->echo_isr_number, echo_recv_isr, IRQ_FLAG, hcsr->pin_str[ECHO_INDEX][HC_GPIO_LINUX], hcsr);
    ret = request_irq(hcsr->echo_isr_number, echo_recv_isr, IRQ_FLAG, "HC-SR04", hcsr);
    if(ret) {
        printk(KERN_ALERT "SETPIN: request_irq:%d\n", ret);
        return -EAGAIN;
    }
    return 0;
}

int freeTrig(int pin) {
    int i, m;
    for(i = 0 ; i < PHY_IO_NUM; i++) {
        if(pin != all_pins[i][HC_GPIO_LINUX][PIN_INDEX])
            continue;

        for(m = 0; m < PIN_SIZE; m++) {
            if(HC_GPIO_PULL == m) continue;

            if(-1 == all_pins[i][m][PIN_INDEX]) continue;

            gpio_free(all_pins[i][m][PIN_INDEX]);
        }
    }
    return 0;
}

int freeEcho(int pin) {
    int i, m;
    for(i = 0 ; i < PHY_IO_NUM; i++) {
        if(pin != all_pins[i][HC_GPIO_LINUX][PIN_INDEX])
            continue;

        for(m = 0; m < PIN_SIZE; m++) {
            if(-1 == all_pins[i][m][PIN_INDEX]) continue;

            gpio_free(all_pins[i][m][PIN_INDEX]);
        }
    }
    return 0;
}

int setTrig (struct hcsr_struct* hcsr, int pin) {
    int i, m;
    int ret;
    for(i = 0 ; i < PHY_IO_NUM; i++) {
        if(pin != all_pins[i][HC_GPIO_LINUX][PIN_INDEX])
            continue;

        for(m = 0; m < PIN_SIZE; m++) {
            if(HC_GPIO_PULL == m) continue;

            if(-1 == all_pins[i][m][PIN_INDEX]) continue;

            ret = gpio_request(all_pins[i][m][PIN_INDEX], hcsr->pin_str[TRIGGER_INDEX][m]);
            if(ret) {
                printk(KERN_ALERT "setTrig: gpio_request,pin:%d, ret=%d\n", all_pins[i][m][PIN_INDEX], ret);
                goto ERR_setTrig_RETURN;
            }

            if(HC_GPIO_MUX0 == m || HC_GPIO_MUX1 == m) {
                gpio_set_value(all_pins[i][m][PIN_INDEX], all_pins[i][m][VAL_INDEX]);
                printk(KERN_ALERT "setTrig: gpio_setValuet");
            } 
            else if(HC_GPIO_LEVEL == m) {
                ret = gpio_direction_output(all_pins[i][m][PIN_INDEX], DIR_OUT);
                if(ret) {
                    printk(KERN_ALERT "setTrig: gpio_dir_output:%d:%d\n", all_pins[i][m][PIN_INDEX],ret);
                    goto ERR_OPER_RETURN;
                }
            }
            else if(HC_GPIO_LINUX == m) {
                ret = gpio_direction_output(all_pins[i][m][PIN_INDEX], 0);
                if(ret) {
                    printk(KERN_ALERT "setTrig: gpio_dir_output:%d:%d\n", all_pins[i][m][PIN_INDEX],ret);
                    goto ERR_OPER_RETURN;
                }
            }
        }
    }
    return 0;

ERR_OPER_RETURN:
    gpio_free(pin);
ERR_setTrig_RETURN:
    return -EINVAL; 
}

int setEcho (struct hcsr_struct* hcsr, int pin) {
    int i, m;
    int ret;
    for(i = 0 ; i < PHY_IO_NUM; i++) {
        if(pin != all_pins[i][HC_GPIO_LINUX][PIN_INDEX])
            continue;

        for(m = 0; m < PIN_SIZE; m++) {
            if(-1 == all_pins[i][m][PIN_INDEX]) continue;

            ret = gpio_request(all_pins[i][m][PIN_INDEX], hcsr->pin_str[ECHO_INDEX][m]);
            if(ret) {
                printk(KERN_ALERT "setEcho: gpio_request,pin:%d, ret=%d\n", all_pins[i][m][PIN_INDEX], ret);
                goto ERR_SETECHO_RETURN;
            }

            if(HC_GPIO_MUX0 == m || HC_GPIO_MUX1 == m) {
                gpio_set_value(all_pins[i][m][PIN_INDEX], all_pins[i][m][VAL_INDEX]);
            } 
            else if(HC_GPIO_PULL == m) {
                 gpio_direction_output(all_pins[i][m][PIN_INDEX], PULL_DOWN);
            }
            else if(HC_GPIO_LEVEL == m) {
                ret = gpio_direction_output(all_pins[i][m][PIN_INDEX], DIR_IN);
                if(ret) {
                    printk(KERN_ALERT "setEcho: gpio_dir_output:%d:%d\n", all_pins[i][m][PIN_INDEX],ret);
                    goto ERR_OPER_RETURN;
                }
            }
            else if(HC_GPIO_LINUX == m) {
                ret = gpio_direction_input(all_pins[i][m][PIN_INDEX]);
                if(ret) {
                    printk(KERN_ALERT "setEcho: gpio_dir_in:%d:%d\n", all_pins[i][m][PIN_INDEX],ret);
                    goto ERR_OPER_RETURN;
                }
            }
        }
    }
    return 0;
ERR_OPER_RETURN:
    gpio_free(pin);
ERR_SETECHO_RETURN:
    return -EINVAL; 
}

#endif
