#include "ht530_common.h"
#define MODULE_NAME ("ht530")
#define DEVICE_NAME ("ht530_drv")
#define HT530_HT_BITS (7) 
#define BUCKET_SIZE ((1) << HT530_HT_BITS)
#define MINOR_BASE (0)
#define MINOR_COUNT (5)
#define KEY_TO_BUCKET(key) (hash_min((key), HT530_HT_BITS))
#define TEST_SIZE (256)
//#define KEY_TO_BUCKET(key) (hash_min((key), HT530_HT_BITS))

static int __init ht530_init(void);
static void __exit ht530_exit(void);

struct test_set {
    char test_buffer[TEST_SIZE];
    int len;
};


struct ht530_node {
    struct hlist_node node;
    ht_object_t pair;
};

struct ht530_dev {
    struct cdev cdev;
    char name[20];
};

static struct hlist_head* find_the_bucket(const int key); 
static struct ht530_node* find_the_node(const int key, struct hlist_head* bucket); 
static int ht530_open(struct inode* node, struct file* file);
static int ht530_release(struct inode* node, struct file* file);
static ssize_t ht530_read(struct file *file, char *buf, size_t count, loff_t *ptr);
static ssize_t ht530_write(struct file *file, const char __user *buf, size_t count, loff_t *ptr);
long ht530_unlocked_ioctl (struct file *file, unsigned int req1, unsigned long req2);
static int __init ht530_init(void);
static void __exit ht530_exit(void);

static void bzero(void* sl, unsigned long n) {
    register char *t = sl;
    while(n != 0) {
        *t++ = 0;
        n --;
    }
}
