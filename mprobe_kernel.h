#include "mprobe_common.h"
#define MODULE_NAME ("mprobe")
#define DEVICE_NAME ("mprobe")
#define MINOR_BASE (0)
#define MINOR_COUNT (5)

static int __init mprobe_init(void);
static void __exit mprobe_exit(void);

#define STACK_SIZE_KPROBE (0x100000000)
typedef struct ringbuffer{
    unsigned idx;
    struct debug_request req; 
    struct debug_result rst[RING_SIZE];
} ringbuffer;
struct mprobe_dev {
    struct cdev cdev;
};
static int init_kp(struct kprobe* kp, unsigned long long addr, char* symbol_name);
static int destroy_kp(struct kprobe** kp);
static int handler_pre(struct kprobe *p, struct pt_regs *regs); 
static int handler_fault(struct kprobe *p, struct pt_regs *regs, int trapnr) ;
static int handler_break(struct kprobe *p, struct pt_regs *regs); 
static void handler_post(struct kprobe *p, struct pt_regs *regs, unsigned long flags);
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
