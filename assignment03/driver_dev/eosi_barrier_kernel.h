#ifndef _HC_SR04_KERNEL_H_
#define _HC_SR04_KERNEL_H_

#include "eosi_barrier_common.h"
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <linux/wait.h>
static int eosi_barrier_open(struct inode* node, struct file* file);
static int eosi_barrier_release(struct inode* node, struct file* file);
static long eosi_barrier_ioctl(struct file* file, unsigned int cmd, unsigned long arg);

typedef struct barrier_struct {
  pid_t tgid;
  wait_queue_head_t waitQ;
  struct list_head list;
} barrier_struct;


#endif
