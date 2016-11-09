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
#include<linux/kernel.h>
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

static LIST_HEAD(tgid_list);


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
    printk(KERN_ALERT "eosi_barrier2: Open\n");
    printk(KERN_ALERT "eosi_barrier: sizeof(unsigned long):%d sizeof(int):%d\n", sizeof(unsigned long long), sizeof(int));
    printk(KERN_ALERT "eosi_barrier: Open Done\n");
    return 0;
}

//destroy
static int eosi_barrier_release(struct inode* node, struct file* file) {
    printk(KERN_ALERT "eosi_barrier: Release\n");
    printk(KERN_ALERT "eosi_barrier: Release Done\n"); return 0;
}

barrier_struct* barray[10];
static unsigned int basic_bid = 0;
int barrier_init(barrier_info* binfo) {
    barrier_struct* barrier = NULL;

    printk(KERN_ALERT "barrier_init\n");

    if(0 > binfo->count) return -1;
    //search tgid linkedlist
    //if the tgid block does not exist, then allocate the new one tgid block.

    binfo->barrier_id = basic_bid;
    basic_bid++;
    barrier = kmalloc(sizeof(barrier_struct), GFP_KERNEL);
    init_barrier_struct(barrier, binfo->count, binfo->barrier_id);

    //link the corresponding barrier block.
    barray[basic_bid-1] = barrier;

    printk(KERN_ALERT "barrier_init done\n");
    return 0;
}

void printSYNC_start(char* fun, barrier_struct* barrier) {
    printk(KERN_ALERT "%s: tgid:%d, tid:%d, id:%d, curr(tgid, tid):(%d,%d)\n", fun, barrier->tgid, barrier->tid, barrier->id, task_tgid_vnr(current), task_pid_vnr(current));
}

void printSYNC_done(char* fun, barrier_struct* barrier) {
    char s[30];
    snprintf(s, 30, "%s Done", fun);

    printk(KERN_ALERT "%s: tgid:%d, tid:%d, id:%d, curr (tgid, tid):(%d, %d)\n", s, barrier->tgid, barrier->tid, barrier->id, task_tgid_vnr(current), task_pid_vnr(current));

}
void wake_up_here (barrier_struct* barrier) {
    char*s = "wake_up_here";

    printSYNC_start(s, barrier);
    wake_up_interruptible_all(&(barrier->waitQ));
    printSYNC_done(s, barrier);

}

void wait_here (barrier_struct* barrier) {
    char*s = "wait_here";
    DEFINE_WAIT(wait);

    printSYNC_start(s, barrier);

    prepare_to_wait(&(barrier->waitQ), &wait, TASK_INTERRUPTIBLE);
    schedule();
    finish_wait(&(barrier->waitQ), &wait);
    printSYNC_done(s, barrier);

}

int barrier_wait(unsigned int barrier_id) {
    barrier_struct* barrier = barray[barrier_id];
    char*s = "barrier_wait";

    printSYNC_start(s, barrier);

    spin_lock(&(barrier->lock_total));

    while(barrier->total > BARRIER_FLAG) {
        spin_unlock(&(barrier->lock_total));
        wait_here(barrier);
        spin_lock(&(barrier->lock_total));
    }

    if(BARRIER_FLAG == barrier->total) barrier->total = 0;

    barrier->total++;

    if(barrier->total == barrier->count) {
        barrier->total += BARRIER_FLAG - 1;
        wake_up_here(barrier);
        //spin_unlock(&(barrier->lock_total));

    } else { //sleep here;
        while(barrier->total < BARRIER_FLAG) {
            spin_unlock(&(barrier->lock_total));
            wait_here(barrier);
            spin_lock(&(barrier->lock_total));
        }

        barrier->total--;

        if(barrier->total == BARRIER_FLAG) {
            wake_up_here(barrier);
        }

        // spin_unlock(&(barrier->lock_total));
    }

    spin_unlock(&(barrier->lock_total));
    printSYNC_done(s, barrier);
    return 0;
}

int barrier_destroy(unsigned int barrier_id) {
    char* s = "barrier_destroy";
    barrier_struct* barrier = barray[barrier_id];

    printSYNC_start(s, barrier);

    spin_lock(&(barrier->lock_total));

    while(barrier->total > BARRIER_FLAG) {
        spin_unlock(&(barrier->lock_total));
        wait_here(barrier);
        spin_lock(&(barrier->lock_total));
    }

    spin_unlock(&(barrier->lock_total));

    //Recycle HERE//

    printSYNC_done(s, barrier);
    return 0;
}

static long eosi_barrier_ioctl(struct file* file, unsigned int cmd, unsigned long arg) {
    int ret = 0;

    printk(KERN_ALERT "ioctl\n");
    switch (cmd) {
        case BARRIER_INIT:
            ret = barrier_init((barrier_info*)arg);
            break;
        case BARRIER_WAIT:
            ret = barrier_wait((unsigned int)arg);
            break;
        case BARRIER_DESTROY:
            ret = barrier_destroy((unsigned int)arg);
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

static void init_barrier_struct(barrier_struct* barrier, int count, unsigned int id) {
    barrier->count = count;
    barrier->total = BARRIER_FLAG;
    barrier->id = id;
    barrier->tgid = task_tgid_vnr(current);
    barrier->tid = task_pid_vnr(current);
    spin_lock_init(&(barrier->lock_total));
    init_waitqueue_head(&(barrier->waitQ));
}

static void init_per_tgid(per_tgid* each_tgid) {
    each_tgid->tgid = task_tgid_vnr(current);
}
#ifndef _BARRIER_MGMT_
#endif

static int __init eosi_barrier_init(void) {
    /*!!!!!*/
    //CHECK!!! INIT
    printk(KERN_ALERT "eosi_barrier: INIT\n");
    if( reg_misc(&eosi_barrier_A) || reg_misc(&eosi_barrier_B) )
        return 0;

//    init_hcsr_struct(&hcA, &eosi_barrier_A, A_pins, A_pin_str);
//    init_hcsr_struct(&hcB, &eosi_barrier_B, B_pins, B_pin_str);

    printk(KERN_ALERT "eosi_barrier: INIT DONE\n");
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
