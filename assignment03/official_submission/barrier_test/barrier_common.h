#ifndef _HC_SR04_COMMON_H_
#define _HC_SR04_COMMON_H_
#include<linux/ioctl.h>
#define BARRIER_W_MAGIC ('B')
#define BARRIER_INIT _IOW (BARRIER_W_MAGIC, 1, char*)
#define BARRIER_WAIT _IOR (BARRIER_W_MAGIC, 2, char*)
#define BARRIER_DESTROY _IOW (BARRIER_W_MAGIC, 3, char*)
typedef struct barrier_info {
    int count;
    unsigned int barrier_id;
} barrier_info;
#endif
