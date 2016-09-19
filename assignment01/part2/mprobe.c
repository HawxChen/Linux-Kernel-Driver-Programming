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
#include<linux/errno.h>
#include<linux/kprobes.h>
#include<asm/timex.h>
#include<linux/io.h>
#include "mprobe_kernel.h"

module_init(mprobe_init);
module_exit(mprobe_exit);

extern int errno;
static struct file_operations mprobe_fops = {
    .owner = THIS_MODULE,
    .open = mprobe_open,
    .release = mprobe_release,
    .write = mprobe_write,
    .read = mprobe_read,
    .unlocked_ioctl = mprobe_unlocked_ioctl
};

static struct kprobe* kp=NULL ;

ringbuffer rbf;


static int handler_pre(struct kprobe *p, struct pt_regs *regs) {
    unsigned long  local_addr = 0;
    unsigned long  g_addr = 0;
    struct debug_result *drst;

    local_addr = ((unsigned long)regs->sp) + (unsigned long)rbf.req.of_local;
    g_addr = ((unsigned long)rbf.req.sect.bss) + 0x440;

    drst = &rbf.rst[rbf.idx++];
    rbf.idx%=RING_SIZE;
    drst->addr = regs->ip;
    drst->pid = current->pid;
    drst->xtc = get_cycles();
    drst->local_var = *((int*)local_addr);
    drst->g_var = *((int*)g_addr);

    printk(KERN_INFO "pre_handler: p->addr = 0x%p, ip = %lx, flags = 0x%lx, bp = 0x%lx, sp = 0x%lx, time = %llu,local_addr:%lx, global_addr:%lx\n", p->addr, regs->ip, regs->flags, regs->bp, regs->sp, get_cycles(), local_addr, g_addr);
    printk(KERN_INFO "bss:%llx,*bp=0x%lx \n", rbf.req.sect.bss,*((unsigned long*)regs->bp));
    return 0;
}

static int handler_fault(struct kprobe *p, struct pt_regs *regs, int trapnr) {
	printk(KERN_INFO "fault_handler: p->addr = 0x%p, trap #%d\n",
		p->addr, trapnr);
	/* Return 0 because we don't handle the fault. */
	return 0;
}

static int handler_break(struct kprobe *p, struct pt_regs *regs) {
	printk(KERN_INFO "break_handler: p->addr = 0x%p\n", p->addr);
	/* Return 0 because we don't handle the fault. */
	return 0;
}

static void handler_post(struct kprobe *p, struct pt_regs *regs, unsigned long flags) {
	printk(KERN_INFO "post_handler: p->addr = 0x%p, flags = 0x%lx\n",
		p->addr, regs->flags);
}
/*set up handlers*/
/*registration*/
size_t strlen(const char* s) {
    const char* sc;
    for(sc = s; *sc != '\0'; ++sc)
        ;
    return sc -s;
}
char* mprobe_strcpy(char *dst, char* src) {
    char* tmp = dst;
    while((*dst++ = *src++) != '\0')
        ;
    return tmp;
}
static int destroy_kp(struct kprobe** kp) {
    if(NULL == *kp) return -EINVAL;

    if(NULL != (*kp)->symbol_name) {
        kfree((*kp)->symbol_name);
    }
    *kp = NULL;
    return 0;
}

static int init_kp(struct kprobe* kp, unsigned long long addr, char* symbol_name) {

    if(addr == 0 && symbol_name == NULL) return -EINVAL;

    kp->addr = 0;
    kp->symbol_name = NULL;

    if(addr != 0) {
        kp->addr =(kprobe_opcode_t*) addr;
    } 
    if (symbol_name != NULL) {
        int len = 0;
        len = strlen(symbol_name);
        kp->symbol_name = kmalloc(len+1,GFP_KERNEL);
        mprobe_strcpy((char*) kp->symbol_name, symbol_name);
    }

    kp->pre_handler    = handler_pre;
    kp->post_handler    = handler_post;
    kp->break_handler    = handler_break;
    kp->fault_handler    = handler_fault;
    return 0;
}

static int mprobe_open(struct inode* node, struct file* file) {
    printk(KERN_ALERT "mprobe: Open\n");
    printk(KERN_ALERT "mprobe: Open Done\n");
    return 0;
}

static int mprobe_release(struct inode* node, struct file* file) {
    printk(KERN_ALERT "mprobe: Release\n");
    printk(KERN_ALERT "mprobe: Release Done\n");
    return 0;
}

//ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
static ssize_t mprobe_read(struct file *file, char *buf, size_t count, loff_t *ptr) {
    int i = 0;
    int failed_copy = 0;
    printk(KERN_ALERT "mprobe: read\n");
    if(0 == rbf.rst[0].xtc) {
        return -EINVAL;
    }

    for(; i < RING_SIZE ;i++) {
       if(0 ==  rbf.rst[i].xtc) break;
    }

    failed_copy = copy_to_user(buf, &(rbf.rst),sizeof(struct debug_result)*i);

    printk(KERN_ALERT "mprobe: read Done\n");
    return sizeof(struct debug_result)*i;
}

//ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
static ssize_t mprobe_write(struct file *file, const char __user *buf, size_t count, loff_t *ptr) {
    struct debug_request *req = (struct debug_request*) buf;

    printk(KERN_ALERT "mprobe: write\n");

    if(kp != NULL) {
        unregister_kprobe(kp);
        destroy_kp(&kp);
        printk(KERN_ALERT "destroy KP\n");
    }
    printk("req->of_line + req->sect.text:%llx\n",req->of_line + req->sect.text);
    kp = (struct kprobe*) kmalloc(sizeof(struct kprobe), GFP_KERNEL);
    init_kp(kp, req->of_line + req->sect.text, NULL);
    rbf.req = *req;
    printk("HERE2");
    register_kprobe(kp);
    

    printk(KERN_ALERT "mprobe: write Done\n");
    return 0;
}

long mprobe_unlocked_ioctl (struct file *file, unsigned int req1, unsigned long req2) {
    printk(KERN_ALERT "mprobe: unlocked_ioctl\n");
    printk(KERN_ALERT "mprobe: unlocked_ioctl Done\n");
    return 0;
}

static struct mprobe_dev *dev_ptr;
static dev_t mprobe_devnum;
static struct class *mprobe_class;
static int __init mprobe_init(void) {
    int ret = 0;
    printk(KERN_ALERT "mprobe: init\n");

    /*
    kp = (struct kprobe*) kmalloc(sizeof(struct kprobe), GFP_KERNEL);
    init_kp(kp, 0, "ht530_open");
    if(kp->addr != 0 || kp->symbol_name != NULL) register_kprobe(kp);
    */

    if(0 > alloc_chrdev_region(&mprobe_devnum, MINOR_BASE, MINOR_COUNT, MODULE_NAME)) {
        printk(KERN_ALERT "Error: alloc_chrdev_region");
        return -EINVAL;
    } else {
        printk(KERN_ALERT "mprobe: Major:%d, Minor:%d\n", MAJOR(mprobe_devnum), MINOR(mprobe_devnum));
    }

    mprobe_class = class_create(THIS_MODULE, MODULE_NAME);
    dev_ptr = (struct mprobe_dev*) kmalloc(sizeof(struct mprobe_dev), GFP_KERNEL);

    cdev_init(&dev_ptr->cdev, &mprobe_fops);
    dev_ptr->cdev.owner = THIS_MODULE;

    ret = cdev_add(&dev_ptr->cdev, mprobe_devnum, MINOR_COUNT);
    if(ret) {
        printk("Bad Registration\n");
        return ret;
    }

    device_create(mprobe_class, NULL, MKDEV(MAJOR(mprobe_devnum),0), NULL, DEVICE_NAME);

    printk(KERN_ALERT "mprobe: INIT DONE\n");

    return 0;
}

static void __exit mprobe_exit(void) {
    printk(KERN_ALERT "mprobe: GoodBye Kernel World!!!\n");

    if((kp != NULL) && (kp->addr != 0 || kp->symbol_name != NULL)) {
        unregister_kprobe(kp);
        destroy_kp(&kp);
    }
    unregister_chrdev_region(mprobe_devnum, MINOR_COUNT);
    /*Destroy device*/
    device_destroy(mprobe_class, MKDEV(MAJOR(mprobe_devnum),0));
    cdev_del(&dev_ptr->cdev);
    kfree(dev_ptr);
    class_destroy(mprobe_class);
    printk(KERN_ALERT "mprobe: EXIT DONE\n");
    return;
} 

MODULE_LICENSE("GPL");
