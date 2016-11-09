#ifndef _HC_SR04_USR_H_
#define _HC_SR04_USR_H_
#include"eosi_barrier_common.h"
#include <unistd.h>
#include<sys/ioctl.h>
int eosi_barrer_fd;
barrier_info b1info;
barrier_info b2info;
int barrier_init(int count, unsigned int* barrier_id) {
    b1info.count = count;
    ioctl(eosi_barrer_fd, BARRIER_INIT, (unsigned long)&b1info);
    *barrier_id = b1info.barrier_id;
    return 0;
}

int barrier_wait(unsigned int barrier_id) {
    ioctl(eosi_barrer_fd, BARRIER_WAIT, barrier_id);
    return 0;
}

int barrier_destroy(unsigned int barrier_id) {
    return 0;
}

#endif
