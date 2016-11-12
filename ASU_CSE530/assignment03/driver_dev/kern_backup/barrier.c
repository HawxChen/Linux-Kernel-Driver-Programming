#include<linux/kernel.h>
#include<linux/syscalls.h>
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
#include"barrier_kernel.h"

static LIST_HEAD(tgid_list);
DEFINE_SPINLOCK(g_tgid_list_lock);

per_tgid* get_ptgid_without_lock(pid_t tgid){
    struct list_head* scan = NULL;
    per_tgid* ptgid = NULL;

    list_for_each(scan, &tgid_list) {
        ptgid = list_entry(scan, per_tgid, list);
        printk(KERN_ALERT "get_ptgid: required_tgid:%d, tgid:%d", tgid, ptgid->tgid);
        if(ptgid->tgid == tgid) {
            return  ptgid;
        }
    }
    return NULL;
}

per_tgid* get_ptgid(pid_t tgid){
    struct list_head* scan = NULL;
    per_tgid* ptgid = NULL;

    spin_lock(&g_tgid_list_lock);
    list_for_each(scan, &tgid_list) {
        ptgid = list_entry(scan, per_tgid, list);
        //printk(KERN_ALERT "get_ptgid: required_tgid:%d, tgid:%d", tgid, ptgid->tgid);
        if(ptgid->tgid == tgid) {
            spin_unlock(&g_tgid_list_lock);
            return  ptgid;
        }
    }

    spin_unlock(&g_tgid_list_lock);
    return NULL;
}

barrier_struct* get_barrier_from_ptgid(per_tgid* ptgid, int barrier_id) {
    struct list_head* scan = NULL;
    barrier_struct* barrier = NULL;

    if(NULL == ptgid) return NULL;
    spin_lock(&(ptgid->barrier_head_lock));
    list_for_each(scan, &(ptgid->barrier_head)) {
        barrier = list_entry(scan, barrier_struct, list);
        //printk(KERN_ALERT "get_barrier:tgid:%d, required_bid:%d, bid:%d", ptgid->tgid, barrier_id, barrier->id);
        if(barrier->id == barrier_id) {
            spin_unlock(&(ptgid->barrier_head_lock));
            return  barrier;
        }
    }

    spin_unlock(&(ptgid->barrier_head_lock));

    return NULL;
}

barrier_struct* get_barrier_only_from_id(int barrier_id) {
    return get_barrier_from_ptgid(get_ptgid(task_tgid_vnr(current)), barrier_id);
}

//barrier_struct* barray[10];
int do_barrier_init(barrier_info* binfo) {
    pid_t tgid = 0;
    per_tgid* ptgid = NULL;
    barrier_struct* barrier = NULL;

    printk(KERN_ALERT "do_barrier_init\n");

    tgid = task_tgid_vnr(current);

    if(0 > binfo->count) goto err_ret;

        //search tgid linkedlist
        //if the tgid block does not exist, then allocate the new one tgid block.
    spin_lock(&g_tgid_list_lock);
    ptgid = get_ptgid_without_lock(tgid);
    if(NULL == ptgid) {
        //allocate, init, insert a new one ptgid
        ptgid = kmalloc(sizeof(per_tgid), GFP_ATOMIC);
        ptgid->tgid = tgid;
        ptgid->bid_count = 0;
        ptgid->barrier_size = 0;

        spin_lock_init(&(ptgid->barrier_head_lock));
        INIT_LIST_HEAD(&(ptgid->barrier_head));

        //list_add_...
        list_add(&(ptgid->list), &tgid_list);
        printk(KERN_ALERT "ptgid LINKED, ptgid->tgid:%d", ptgid->tgid);
    }
    spin_unlock(&g_tgid_list_lock);

    barrier = get_barrier_from_ptgid(ptgid, ptgid->bid_count);
    if(NULL != barrier) {
        //duplicated
        printk("Barrier Init: DUPLATED");
        goto err_ret;
    }

    //allocate, init, insert a new one barrier
    barrier = kmalloc(sizeof(barrier_struct), GFP_KERNEL);

    //link the corresponding barrier block.
    spin_lock(&(ptgid->barrier_head_lock));
    {
        binfo->barrier_id = ptgid->bid_count;
        ptgid->bid_count++;
        init_barrier_struct(barrier, binfo->count, binfo->barrier_id);
        ptgid->barrier_size++;
        list_add(&(barrier->list), &(ptgid->barrier_head));
    }
    spin_unlock(&(ptgid->barrier_head_lock));

    printk(KERN_ALERT "do_barrier_init done: barrier: tgid:%d, id:%d\n", barrier->tgid, barrier->id);

    goto successful_ret;

err_ret:
    return -1;

successful_ret:
    return 0;
}

void printSYNC_self(void) {
    printk(KERN_ALERT "curr(tgid, tid):(%d,%d)\n", task_tgid_vnr(current), task_pid_vnr(current));
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

void sleep_here (barrier_struct* barrier) {
    char*s = "sleep_here";
    DEFINE_WAIT(wait);

    printSYNC_start(s, barrier);

    prepare_to_wait(&(barrier->waitQ), &wait, TASK_INTERRUPTIBLE);
    schedule();
    finish_wait(&(barrier->waitQ), &wait);
    printSYNC_done(s, barrier);

}

int do_barrier_wait(unsigned int barrier_id) {
    barrier_struct* barrier = NULL;
    char*s = "do_barrier_wait";

    printSYNC_self();
    barrier = get_barrier_only_from_id(barrier_id);

    printSYNC_start(s, barrier);

    spin_lock(&(barrier->lock_total));

    while(barrier->total > BARRIER_FLAG) {
        spin_unlock(&(barrier->lock_total));
        sleep_here(barrier);
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
            sleep_here(barrier);
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

int do_barrier_destroy(unsigned int barrier_id) {
    char* s = "do_barrier_destroy";
    barrier_struct* barrier = NULL;
    per_tgid* ptgid = NULL;
    pid_t tgid = 0;

    printk(KERN_ALERT "do_barrier_destory");
    tgid = task_tgid_vnr(current);
    ptgid = get_ptgid(tgid);
    if(NULL == ptgid) {
        printk(KERN_ALERT "do_barrier_destroy: NO ptgid !!!");
        return -1;
    }

    barrier = get_barrier_from_ptgid(ptgid, barrier_id);
    if(NULL == barrier) {
        printk(KERN_ALERT "do_barrier_destroy: NO barrier !!!");
        return -1;
    }

    printSYNC_start(s, barrier);

    spin_lock(&(barrier->lock_total));
    {
        while(barrier->total > BARRIER_FLAG) {
            spin_unlock(&(barrier->lock_total));
            sleep_here(barrier);
            spin_lock(&(barrier->lock_total));
        }
    }
    spin_unlock(&(barrier->lock_total));

    //Recycle HERE//
    spin_lock(&(ptgid->barrier_head_lock));
        list_del(&(barrier->list));
        printSYNC_start("barrier_free", barrier);
        kfree(barrier);
        ptgid->barrier_size--;

        if(0 == ptgid->barrier_size) {
            printk(KERN_ALERT "ptgid_free:%d", ptgid->tgid);

            spin_lock(&g_tgid_list_lock);
                list_del(&(ptgid->list));
            spin_unlock(&g_tgid_list_lock);

            kfree(ptgid);
        }
    spin_unlock(&(ptgid->barrier_head_lock));


    //barrier has bee kfreed
    //printSYNC_done(s, barrier);
    printk(KERN_ALERT "barrier_destroy Done\n");
    return 0;
}

asmlinkage long sys_barrier_init(int count, unsigned int *barrier_id) {
    int ret = 0;
    barrier_info binfo;
    binfo.count = count;
    printk(KERN_ALERT "sys_barrier_init\n");
    ret = do_barrier_init(&binfo);
    if(0 > ret) goto failed_ret;

    ret = copy_to_user(barrier_id,&(binfo.barrier_id),sizeof(int));
    if(0 > ret) goto failed_ret;

    printk(KERN_ALERT "sys_barrier_init Done, barrier_id:%u\n", *barrier_id);
    return ret;

failed_ret:
    printk(KERN_ALERT "sys_barrier_init FAILED !!!!!");
    return ret;
}

asmlinkage long sys_barrier_wait(unsigned int barrier_id) {
    int ret = 0;

    printk(KERN_ALERT "sys_barrier_wait");
    ret = do_barrier_wait(barrier_id);
    printk(KERN_ALERT "sys_barrier_wait Done");

    return ret;
}

asmlinkage long sys_barrier_destroy(unsigned int barrier_id) {
    int ret = 0;

    printk(KERN_ALERT "sys_barrier_destroy");
    ret = do_barrier_destroy(barrier_id);
    printk(KERN_ALERT "sys_barrier_destroy Done");

    return ret;
}

static void init_barrier_struct(barrier_struct* barrier, int count, unsigned int id) {
    barrier->id = id;
    barrier->count = count;
    barrier->total = BARRIER_FLAG;
    barrier->tgid = task_tgid_vnr(current);
    barrier->tid = task_pid_vnr(current);
    spin_lock_init(&(barrier->lock_total));
    init_waitqueue_head(&(barrier->waitQ));
}

