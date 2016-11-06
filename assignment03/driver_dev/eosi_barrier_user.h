#ifndef _HC_SR04_USR_H_
#define _HC_SR04_USR_H_
#include"eosi_barrier_common.h"
#include <unistd.h>
#include<sys/ioctl.h>
int barrier_init(int count, unsigned int* barrier_id) {
    return 0;
}
int barrier_wait(unsigned int barrier_id) {
    return 0;
}

int barrier_destroy(unsigned int barrier_id) {
    return 0;
}

#endif
