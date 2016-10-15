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

/*
Dynamically define.
wait_queue_head_t my_event;
init_waitqueue_head(&my_event);
*/
DECLARE_WAIT_QUEUE_HEAD(hcsrA_WQ);
DECLARE_WAIT_QUEUE_HEAD(hcsrB_WQ);

static LIST_HEAD(hc_list);
ktime_t kstart;


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

static int do_send(struct hcsr_struct* hcsr){
    unsigned int counter = 0;
    int trig_pin = hcsr->pins[TRIGGER_INDEX][HC_GPIO_LINUX][PIN_INDEX];

    spin_lock(&(hcsr->ongoing_lock));
    hcsr->ongoing = ONGOING;
    spin_unlock(&(hcsr->ongoing_lock));

    gpio_set_value(trig_pin, 1);
    udelay(10);
    gpio_set_value(trig_pin, 0);
    kstart = ktime_get();
    
//    spin_lock(&(hcsr->ongoing_lock));
    while(ONGOING == hcsr->ongoing) {
//        spin_unlock(&(hcsr->ongoing_lock));
        if(++counter > 50000) {
            spin_lock(&(hcsr->ongoing_lock));
            hcsr->ongoing = STOPPING;
            spin_unlock(&(hcsr->ongoing_lock));
            return -EAGAIN;
        }
        udelay(1);
    }
//    spin_unlock(&(hcsr->ongoing_lock));

    return 0;
}

static int send(struct hcsr_struct* hcsr, int retry_cnt) {
    int ret = 0;
    int send_cnt = 0;
    do {
        ret = do_send(hcsr);
    } while( (0 != ret)  && (send_cnt++ < retry_cnt));
    return ret;
}


static irqreturn_t echo_recv_isr(int irq, void *data) {
    //Only one isr_handler in the same time: IRQF_DISABLED, enable it at request_irq.
    ktime_t kend;
    unsigned int diff;
    unsigned int distance;
    struct hcsr_struct* hcsr = (struct hcsr_struct*) data;

    printk(KERN_ALERT "echo_recv_isr\n");
    spin_lock(&(hcsr->kconfig.kconfig_lock));
    if(STOPPING == hcsr->ongoing) {
        spin_unlock(&(hcsr->kconfig.kconfig_lock));
        //printk(KERN_ALERT "!!! Signal from Dark Force !!!");
        goto RETURNED;
    }
    spin_unlock(&(hcsr->kconfig.kconfig_lock));

    spin_lock(&(hcsr->kconfig.kconfig_lock));
    if(-1 == hcsr->kconfig.set.mode) {
        printk(KERN_ALERT "Mode doesn't get initialization-mode;%d\n", hcsr->kconfig.set.mode);
        spin_unlock(&(hcsr->kconfig.kconfig_lock));
        goto RETURNED;
    }
    spin_unlock(&(hcsr->kconfig.kconfig_lock));

    kend = ktime_get();
    diff = ktime_to_ns(ktime_sub(kend, kstart));
    distance = diff/5800;

    spin_lock(&(hcsr->cirb.cir_buf_lock));
        hcsr->cirb.newest = (hcsr->cirb.newest+1)%NUM_DATA;
        hcsr->cirb.buf.data[(hcsr->cirb.newest)%NUM_DATA] = distance;
    spin_unlock(&(hcsr->cirb.cir_buf_lock));

    printk(KERN_ALERT "echo_recv_isr: %d CM, newest:%d\n",distance, hcsr->cirb.newest);

RETURNED:
    hcsr->ongoing = STOPPING;
    printk(KERN_ALERT "echo_recv_isr Done\n");
    return IRQ_HANDLED;
}

static long ioctl_SETPINs(struct file* filp, unsigned long addr) {
    pin_set pins;
    int ret = 0;
    int i, j;
    struct hcsr_struct* hcsr = NULL; 
    printk(KERN_ALERT "ioctl_SETPIN\n");
    if(0 != copy_from_user(&pins, (void*)addr, sizeof(pin_set))) {
        printk(KERN_ALERT "SETPIN: copy_from_user ERROR\n");
        ret = -EAGAIN;
        goto ERR_SETPIN_RETURN;
    }

    hcsr = get_curr_hcsr(filp->f_dentry->d_inode);
    //if(...)
    if(hcsr->pins[TRIGGER_INDEX][HC_GPIO_LINUX][PIN_INDEX] != pins.trigger_pin  || hcsr->pins[ECHO_INDEX][HC_GPIO_LINUX][PIN_INDEX] != pins.echo_pin) {
        ret = -EINVAL;
        goto ERR_SETPIN_RETURN;
    }
    
    spin_lock(&(hcsr->irq_done_lock));
    if(IRQ_DONE == hcsr->irq_done) {
        goto SUCCESS_SETPIN_RETURN;
    }

    for(i = 0 ; i < SET_SIZE; i++) {
        for(j = 0; j < PIN_SIZE; j++) {
            if(-1 == hcsr->pins[i][j][PIN_INDEX])
                continue;
            ret = gpio_request(hcsr->pins[i][j][PIN_INDEX], hcsr->pin_str[i][j]);
            if(ret) {
                printk(KERN_ALERT "SETPIN: gpio_request,pin:%d, ret=%d\n", hcsr->pins[i][j][PIN_INDEX], ret);
                goto ERR_SETPIN_RETURN;
            }

            //if(unlikely(ECHO_INDEX == i && HC_GPIO_LINUX == j)) 
            if(ECHO_INDEX == i && HC_GPIO_LINUX == j) 
            {
                ret = gpio_direction_input(hcsr->pins[i][j][PIN_INDEX]);
                if(ret) {
                    printk(KERN_ALERT "SETPIN: gpio_dir_input:%d\n", ret);
                    goto ERR_SETPIN_RETURN;
                }
            } 

            else if(HC_GPIO_MUX0 == j || HC_GPIO_MUX1 == j) {
                gpio_set_value(hcsr->pins[i][j][PIN_INDEX], hcsr->pins[i][j][VAL_INDEX]);
            } 
            
            else {
                ret = gpio_direction_output(hcsr->pins[i][j][PIN_INDEX], hcsr->pins[i][j][VAL_INDEX]);
                if(ret) {
                    printk(KERN_ALERT "SETPIN: gpio_dir_output:%d:%d\n", hcsr->pins[i][j][VAL_INDEX],ret);
                    goto ERR_SETPIN_RETURN;
                }
            }
        }
    }

    //For Echo ISR
    ret = gpio_to_irq(hcsr->pins[ECHO_INDEX][HC_GPIO_LINUX][PIN_INDEX]);
    if(0 > ret) {
        printk(KERN_ALERT "SETPIN: gpio_to_irq:%d\n", ret);
        goto ERR_SETPIN_RETURN;
    } else {
         hcsr->echo_isr_number = ret;
    }
    printk("HERE");
    //ret = request_irq(hcsr->echo_isr_number, echo_recv_isr, IRQ_FLAG, hcsr->pin_str[ECHO_INDEX][HC_GPIO_LINUX], hcsr);
    ret = request_irq(hcsr->echo_isr_number, echo_recv_isr, IRQ_FLAG, "HC-SR04", hcsr);
    if(ret) {
        printk(KERN_ALERT "SETPIN: request_irq:%d\n", ret);
        goto ERR_SETPIN_RETURN;
    }

    goto SUCCESS_SETPIN_RETURN;

ERR_SETPIN_RETURN:
    spin_unlock(&(hcsr->irq_done_lock));
    return ret;

SUCCESS_SETPIN_RETURN:
    hcsr->irq_done = IRQ_DONE;
    spin_unlock(&(hcsr->irq_done_lock));
    send(hcsr, 0);
    printk(KERN_ALERT "ioctl_SETPIN DONE\n");
    return 0;
}


static long ioctl_SETMODE(struct file* file, unsigned long addr) {
    int ret = 0;
    hcsr_set set;
    struct hcsr_struct* hcsr = NULL;

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

    hcsr = get_curr_hcsr(file->f_dentry->d_inode);
    spin_lock(&(hcsr->kconfig.kconfig_lock)); {
        // still peeriodical sampling
        //before: one-shot ; after: one-shot
        hcsr->kconfig.set = set;
        spin_unlock(&(hcsr->kconfig.kconfig_lock));
    }
    goto SUCCESS_SETMODE_RETURN;

ERR_SETMODE_RETURN:
        return ret;

SUCCESS_SETMODE_RETURN:
    printk(KERN_ALERT "ioctl_SETMODE DONE\n");
    return 0;
}

static long hc_sr04_ioctl(struct file* file, unsigned int arg1, unsigned long arg2) {
    int ret;

    printk(KERN_ALERT "ioctl\n");
    switch (arg1) {
        case SETPINS:
            ret = ioctl_SETPINs(file, arg2);
            break;
        case SETMODE:
            ret = ioctl_SETMODE(file, arg2);
            break;
        default: 
            return -EINVAL;
    }

    printk(KERN_ALERT "ioctl DONE\n");
    return ret;
}
void spin_ongoing (struct hcsr_struct* hcsr) {
RETEST:
    if(ONGOING == hcsr->ongoing) {
        while(ONGOING == hcsr->ongoing) {
            udelay(10);
        }
        goto RETEST;
    }
    return;
}
static ssize_t hc_sr04_read(struct file *file, char *buf, size_t count, loff_t *ptr) {
    struct hcsr_struct* hcsr = get_curr_hcsr(file->f_dentry->d_inode);

    printk(KERN_ALERT "hc_sr04: Read\n");

    spin_lock(&(hcsr->kconfig.kconfig_lock));
    if(ONE_SHOT == hcsr->kconfig.set.mode) {
        spin_unlock(&(hcsr->kconfig.kconfig_lock));
        if(STOPPING == hcsr->ongoing) {
            send(hcsr, 5);
        } else {
            spin_ongoing(hcsr);
            //get value;
        }
        //take the newest one into its own buffer!
        //copy_to_user
    } else if(PERIODIC == hcsr->kconfig.set.mode) {
            spin_unlock(&(hcsr->kconfig.kconfig_lock));
    }  else {
            spin_unlock(&(hcsr->kconfig.kconfig_lock));
            printk(KERN_ALERT "Read: Mode doesn't get initialization-mode;%d\n", hcsr->kconfig.set.mode);
            return -EINVAL;
    }
    printk(KERN_ALERT "hc_sr04: Read Done\n");
    return 0;
}

static ssize_t hc_sr04_write(struct file *file, const char __user *buf, size_t count, loff_t *ptr) {
    struct hcsr_struct*hcsr = NULL;
    int act;
    int ret;

    printk(KERN_ALERT "hc_sr04: Write\n");
    //copy_from_user..
    ret = copy_from_user(&act , buf, sizeof(int));

    hcsr = get_curr_hcsr(file->f_dentry->d_inode);

    spin_lock(&(hcsr->kconfig.kconfig_lock));
    if(ONE_SHOT == hcsr->kconfig.set.mode) {
        spin_unlock(&(hcsr->kconfig.kconfig_lock));
        if(STOPPING == hcsr->ongoing) {
            send(hcsr, 5);
        } 
        if(0 != act) {
            spin_lock(&(hcsr->cirb.cir_buf_lock));
            memset(&(hcsr->cirb.buf), 0, sizeof(record));
            hcsr->cirb.newest = -1;
            spin_unlock(&(hcsr->cirb.cir_buf_lock));
        }
    } else if(PERIODIC == hcsr->kconfig.set.mode) {
        spin_unlock(&(hcsr->kconfig.kconfig_lock));
    } else {
        spin_unlock(&(hcsr->kconfig.kconfig_lock));
        printk(KERN_ALERT "Write: Mode doesn't get initialization-mode;%d\n", hcsr->kconfig.set.mode);
        return -EINVAL;
    }
    spin_unlock(&(hcsr->kconfig.kconfig_lock));
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

static void init_hcsr_struct(hcsr_struct* hc, struct miscdevice* md, char(*pins)[5][2], char*(*pin_str)[5]) {
    /*!!!!!*/
    //CHECK!!! INIT
    hc->hc_sr04 = md;
    hc->irq_done = IRQ_NOT_DONE;
    hc->echo_isr_number = -1;
    hc->pins = pins;
    hc->kconfig.set.mode = 0;//__DEBUG__ ********
//    hc->kconfig.set.mode = -1; 
    hc->pin_str = pin_str;
    spin_lock_init(&(hc->irq_done_lock));
    spin_lock_init(&(hc->ongoing_lock));
    spin_lock_init(&(hc->cirb.cir_buf_lock));
    spin_lock_init(&(hc->kconfig.kconfig_lock));
    INIT_LIST_HEAD(&(hc->list));
    list_add(&(hc->list), &hc_list);
}


static int __init hc_sr04_init(void) {
    /*!!!!!*/
    //CHECK!!! INIT
    printk(KERN_ALERT "hc_sr04: INIT\n");
    if( reg_misc(&hc_sr04_A) || reg_misc(&hc_sr04_B) )
        return 0;

    init_hcsr_struct(&hcA, &hc_sr04_A, A_pins, A_pin_str);
    init_hcsr_struct(&hcB, &hc_sr04_B, B_pins, B_pin_str);

    printk(KERN_ALERT "hc_sr04: INIT DONE\n");
    return 0;
}

void free_gpio(struct hcsr_struct* hcsr) {
    int i, j;
    for(i = 0; i < 2; i++) {
        for(j = 0; j < PIN_SIZE; j++) {
            if(-1 == hcsr->pins[i][j][PIN_INDEX]) {
                continue;
            }
            //gpio_set_value(hcsr->pins[i][j][PIN_INDEX],0);
            gpio_free(hcsr->pins[i][j][PIN_INDEX]);
        }
    }
}

static void __exit hc_sr04_exit(void) {
    printk(KERN_ALERT "hc_sr04: GoodBye Kernel World!!!\n");

    if(IRQ_DONE == hcA.irq_done) {
        if(0 < hcA.echo_isr_number) {
            printk("---release hcA----\n");
            free_irq(hcA.echo_isr_number, NULL);
        }
        free_gpio(&hcA);  
    }

    if(IRQ_DONE == hcB.irq_done) {
        if(0 < hcB.echo_isr_number) {
            printk("---release hcB----\n");
            free_irq(hcB.echo_isr_number, NULL);
        }
        free_gpio(&hcB);  
    }


    dereg_misc(&hc_sr04_A);
    dereg_misc(&hc_sr04_B);
    printk(KERN_ALERT "hc_sr04: EXIT DONE\n");
    return;
} 

MODULE_LICENSE("GPL");
