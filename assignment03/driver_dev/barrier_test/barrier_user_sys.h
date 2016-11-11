#ifndef _HC_SR04_USR_H_
#define _HC_SR04_USR_H_
#include <unistd.h>
#include<sys/ioctl.h>

#ifndef _BARRIER_MODULE_BUILD_
#define BARRIER_INIT_SYS (351)
#define BARRIER_WAIT_SYS (352)
#define BARRIER_DESTROY_SYS (353)

int barrier_init(int count, unsigned int* barrier_id) {
    return syscall(BARRIER_INIT_SYS, count, barrier_id);
}

int barrier_wait(unsigned int barrier_id) {
    return syscall(BARRIER_WAIT_SYS, barrier_id);
}

int barrier_destroy(unsigned int barrier_id) {
    return syscall(BARRIER_DESTROY_SYS, barrier_id);
}
#else
#include"barrier_common.h"
int eosi_barrer_fd;
int barrier_init(int count, unsigned int* barrier_id) {
    barrier_info b1info;
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
    ioctl(eosi_barrer_fd, BARRIER_DESTROY, barrier_id);
    return 0;
}

#endif

#endif
