#ifndef _HC_SR04_KERNEL_H_
#define _HC_SR04_KERNEL_H_

#define _BARRIER_MGMT_
#include "eosi_barrier_common.h"
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <linux/wait.h>
#define BARRIER_FLAG (1ULL<<63)

typedef struct barrier_struct {
    unsigned int id;
    pid_t tgid;
    pid_t tid;
    unsigned long long count;
    unsigned long long total;
    spinlock_t lock_total;
    wait_queue_head_t waitQ;
    struct list_head list;
} barrier_struct;

typedef struct per_tgid {
  pid_t tgid;
  unsigned int bid_count;
  int barrier_size;
  spinlock_t barrier_head_lock;
  struct list_head barrier_head;
  struct list_head list;
} per_tgid;
static int eosi_barrier_open(struct inode* node, struct file* file);
static int eosi_barrier_release(struct inode* node, struct file* file);
static long eosi_barrier_ioctl(struct file* file, unsigned int cmd, unsigned long arg);
static void init_barrier_struct(barrier_struct* barrier, int count, unsigned int id);
static void init_per_tgid(per_tgid* each_tgid);
#endif
