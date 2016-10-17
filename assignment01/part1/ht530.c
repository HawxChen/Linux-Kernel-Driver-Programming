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
#include "ht530_kernel.h"

module_init(ht530_init);
module_exit(ht530_exit);

static unsigned long cur_size;
static int for_mprobe_test1;
static int for_mprobe_test2;
static DEFINE_HASHTABLE(ht530_tbl, HT530_HT_BITS);
static struct ht530_dev *dev_ptr;
static struct class *ht530_class;
static dev_t Ht530_devnum;

static struct file_operations ht530_fops = {
    .owner = THIS_MODULE,
    .open = ht530_open,
    .release = ht530_release,
    .write = ht530_write,
    .read = ht530_read,
    .unlocked_ioctl = ht530_unlocked_ioctl,
    .fsync = ht530_fsync,
    .flock = ht530_flock
};

int ht530_fsync (struct file *fops, loff_t start, loff_t end, int datasync) {
    int xmen2 = 0x123456;
    int xmen3 = 0x654321;
    
    printk(KERN_ALERT "ht530: fsync\n");
    printk(KERN_ALERT "ht530: fsync,xmen2: %p, ht530_fsync: %p\n",&xmen2, ht530_fsync);
    printk(KERN_ALERT "ht530: fsync,xmen3: %p, ht530_fsync: %p\n",&xmen3, ht530_fsync);
    /*
    unsigned long ptr_xmen3 = 0;
    unsigned long ptr_xmen2 = 0;
    ptr_xmen3 = (unsigned long) &xmen3;
    ptr_xmen2 = (unsigned long) &xmen2;
    printk(KERN_ALERT "ht530: fsync\n");
    printk(KERN_ALERT "ht530: fsync,xmen2: %p, ht530_fsync: %p, *ptr_xmen2=0x%x\n",&xmen2, ht530_fsync, *((int*)ptr_xmen2));
    printk(KERN_ALERT "ht530: fsync,xmen3: %p, ht530_fsync: %p, *ptr_xmen3=0x%x\n",&xmen3, ht530_fsync, *((int*)ptr_xmen3));
    */
    printk(KERN_ALERT "ht530: fsync Done\n");
    return 0;
}

int ht530_flock (struct file *fops, int empty1, struct file_lock *empty2){
    int superman2 = 0x1234560;
    int superman3 = 0x6543210;

    printk(KERN_ALERT "ht530: flock\n");
    printk(KERN_ALERT "ht530:, supermen2: %p, ht530_release: %p\n",&superman2, ht530_release);
    printk(KERN_ALERT "ht530:, supermen3: %p, ht530_release: %p\n",&superman3, ht530_release);
    printk(KERN_ALERT "ht530: flock Done\n");

    return 0;
}


static int ht530_open(struct inode* node, struct file* file) {
    for_mprobe_test1 = 0x12345;
    for_mprobe_test2 = 0x54321;
    printk(KERN_ALERT "ht530: Open, mp_t1:0x%x, mp_t2:0x%x\n", for_mprobe_test1, for_mprobe_test2);
    printk(KERN_ALERT "ht530: Open Done\n");
    return 0;
}

static int ht530_release(struct inode* node, struct file* file) {
    printk(KERN_ALERT "ht530: Release\n");
    printk(KERN_ALERT "ht530: Release Done\n");
    return 0;
}

//ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
static ssize_t ht530_read(struct file *file, char *buf, size_t count, loff_t *ptr) {

    int failed_copy  = 0;
    ht_object_t* uitem = (ht_object_t*) buf;
    struct ht530_node *kitem = NULL;

    printk(KERN_ALERT "ht530: Read\n");
    printk(KERN_ALERT "IN: ht530: %p, key:%d , data:%d \n",  uitem, uitem->key, uitem->data);
    
    kitem =  find_the_node(uitem->key, NULL);

    if(NULL == kitem) {
            return -EINVAL;
    }
    
    failed_copy = copy_to_user(buf, (void*) &kitem->pair, count);
    printk(KERN_ALERT "ht530: Read Done\n");

    return count - failed_copy;

    /* Test data transfer between user and kernel
    int ret;
    printk(KERN_ALERT "ht530: Read\n");
    
    failed_copy = copy_to_user(buf, test_data.test_buffer,test_data.len);
    ret = test_data.len - failed_copy;
    test_data.len -= (test_data.len - failed_copy);
    return ret;
    */
}

static struct hlist_head* find_the_bucket(const int key) {
    struct hlist_head* h;
    h = &ht530_tbl[hash_min(key, HT530_HT_BITS)];
    printk(KERN_ALERT "ht530: find:%llu\n", hash_min(((u64)key), HASH_BITS(ht530_tbl)));

    if(hlist_empty(h)) {
        return NULL;
    }

    return h;
}

static struct ht530_node* find_the_node(const int key,struct hlist_head* bucket) {
    struct ht530_node*  node;
    if(!bucket) {
        bucket = find_the_bucket(key);

        if(!bucket) {
            return NULL;
        }
    }

    hlist_for_each_entry(node, bucket, node) {
        if(node->pair.key == key) return node;
    }
    return NULL;
}
//ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
static ssize_t ht530_write(struct file *file, const char __user *buf, size_t count, loff_t *ptr) {
    ht_object_t* uitem = (ht_object_t*) buf;
    struct ht530_node *kitem = NULL;

    printk(KERN_ALERT "ht530: Write\n");
    printk(KERN_ALERT "IN: ht530: %p, key:%d , data:%d , hash_num:%llu \n", uitem, uitem->key, uitem->data, KEY_TO_BUCKET(((u64) uitem->key)));
    
    kitem =  find_the_node(uitem->key, NULL);

    //Here should be a concurrent section protected by spin/sempahore
    //This worload now is dispatched to user mode.
    if(NULL == kitem) {
        if(0 == uitem->data) {
            /*KUSO: Do Nothing*/
            return 0;
        }
        kitem = (struct ht530_node*) kmalloc(sizeof (struct ht530_node), GFP_KERNEL);
        hash_add(ht530_tbl, &kitem->node, uitem->key);
        cur_size++;
    }
    
    if(0 != uitem->data) {
        kitem->pair.key = uitem->key;
        kitem->pair.data = uitem->data;
        printk(KERN_ALERT "ht530: %p, key:%d , data:%d \n", kitem, kitem->pair.key, kitem->pair.data);
    } else {
        printk(KERN_ALERT "ht530: Delete: %p, key:%d , data:%d \n", kitem, kitem->pair.key, kitem->pair.data);
        hash_del(&kitem->node);
        cur_size--;
        kfree(kitem);
    }

    printk(KERN_ALERT "ht530: Write Done\n");
    return 0;

    /* Test data transfer between user and kernel
    bzero(test_data.test_buffer, TEST_SIZE);
    failed_copy = copy_from_user((void*) &test_data.test_buffer, (void __user*) buf, count);
    if(failed_copy != 0) {
        printk(KERN_ALERT "ht531: copy_from_user ERROR\n");
    }

    printk(KERN_ALERT "ht530: data:%s, count:%d, res:%d\n", test_data.test_buffer, (int)count, failed_copy);
    test_data.len = count - failed_copy;
    return count - failed_copy;
    */


}

long ht530_unlocked_ioctl (struct file *file, unsigned int req1, unsigned long req2) {
    struct dump_org* dump_set = (struct dump_org*) req2;
    struct ht530_node* obj = NULL;
    int acc_n = 0;
    struct hlist_head* h;
    int failed_copy = 0;

    printk(KERN_ALERT "ht530: unlocked_ioctl\n");

    h = &ht530_tbl[dump_set->n];

    if(HASH_DUMP == req1) {

        if(dump_set->n >= BUCKET_SIZE) return -EINVAL;
        bzero(dump_set, sizeof(struct dump_org));
        hlist_for_each_entry(obj, h, node) {
            failed_copy = copy_to_user(&dump_set->object_array[acc_n++], &obj->pair,sizeof(ht_object_t));
            if(acc_n >= MAX_DUMP) break;
        }
    }

    if(RET_CUR_SIZE == req1) {
        printk(KERN_ALERT "ht530: RET_CUR_SIZE:%lu\n", cur_size); 
        failed_copy = copy_to_user((unsigned long*) req2, &cur_size, sizeof(unsigned long));
    }

    //here is protectd by user (Synchronization)
    if(ALL_DUMP == req1) {
        int bkt = 0;
        struct ht530_node *drv_node = NULL;
        ht_object_t* back_storage = (ht_object_t*) req2;
        printk(KERN_ALERT "ht530: ALL_DUMP\n"); 
        hash_for_each(ht530_tbl, bkt, drv_node, node) {
            failed_copy = copy_to_user(back_storage, &drv_node->pair,sizeof(ht_object_t));
            back_storage ++;
        }
    }

    /*
    struct dump_org* dump_set = NULL;
    struct ht530_node* obj = NULL;
    int acc_n = 0;
    struct hlist_head* h;
    int failed_copy = 0;
    int ret = 0;

    printk(KERN_ALERT "ht530: unlocked_ioctl\n");
    if(dump_set->n >= BUCKET_SIZE) return -1;

    switch (req1){
        case HASH_DUMP:
            dump_set = (struct dump_org*) req2;
            h = &ht530_tbl[dump_set->n];

            bzero(dump_set->object_array, MAX_DUMP*sizeof(ht_object_t));
            hlist_for_each_entry(obj, h, node) {
                failed_copy = copy_to_user(&dump_set->object_array[acc_n++], &obj->pair,sizeof(ht_object_t));
                if(acc_n >= MAX_DUMP) break;
            }
            break;

        case RET_CUR_SIZE:
            ret = copy_to_user( (unsigned long*)&req2, &cur_size, sizeof(unsigned long));
            break;

        case ALL_DUMP:
            break;

        default:
            break;
    }
            */

    printk(KERN_ALERT "ht530: unlocked_ioctl Done\n");
    return 0;
}

static int __init ht530_init(void) {
    int ret = 0;
    printk(KERN_ALERT "ht530: init\n");
    hash_init(ht530_tbl);
    cur_size = 0;

    if(0 > alloc_chrdev_region(&Ht530_devnum, MINOR_BASE, MINOR_COUNT, MODULE_NAME)) {
        printk(KERN_ALERT "Error: alloc_chrdev_region");
        return -EINVAL;
    } else {
        printk(KERN_ALERT "ht530: devnum:%d, Major:%d, Minor:%d\n", Ht530_devnum,MAJOR(Ht530_devnum), MINOR(Ht530_devnum));
    }

    ht530_class = class_create(THIS_MODULE, MODULE_NAME);
    dev_ptr = (struct ht530_dev*) kmalloc(sizeof(struct ht530_dev), GFP_KERNEL);

    cdev_init(&dev_ptr->cdev, &ht530_fops);
    dev_ptr->cdev.owner = THIS_MODULE;

    ret = cdev_add(&dev_ptr->cdev, Ht530_devnum, MINOR_COUNT);
    if(ret) {
        printk("Bad Registration\n");
        return ret;
    }

    device_create(ht530_class, NULL, MKDEV(MAJOR(Ht530_devnum),0), NULL, DEVICE_NAME);

    printk(KERN_ALERT "ht530: INIT DONE\n");

    return 0;
}

static void __exit ht530_exit(void) {
    
    struct ht530_node* scan_node = NULL;
    struct hlist_node *tmp_node = NULL;
    int bkt = 0;

    printk(KERN_ALERT "ht530: GoodBye Kernel World!!!\n");
    /*delete hashtable*/
    if(!hash_empty(ht530_tbl)) {
        hash_for_each_safe(ht530_tbl, bkt, tmp_node, scan_node, node) {
            hash_del(&scan_node->node);
            printk(KERN_ALERT "%p, key:%d , data:%d, bucket:%llu\n", scan_node, scan_node->pair.key, scan_node->pair.data, hash_min(scan_node->pair.key, HT530_HT_BITS));
            kfree(scan_node);
        }
    }

    unregister_chrdev_region(Ht530_devnum, MINOR_COUNT);
    /*Destroy device*/
    device_destroy(ht530_class, MKDEV(MAJOR(Ht530_devnum),0));
    cdev_del(&dev_ptr->cdev);
    kfree(dev_ptr);

    class_destroy(ht530_class);

    printk(KERN_ALERT "ht530: EXIT DONE\n");
    return;
} 

MODULE_LICENSE("GPL");
EXPORT_SYMBOL(ht530_fsync);
EXPORT_SYMBOL(ht530_open);
