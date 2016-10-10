#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/ioctl.h>
#include"hc_sr04_user.h"
#include"hc_sr04_common.h"
int main(int argc, char*argv[]) {
    int fd = open("/dev/HCSR_1", O_RDWR);
    int fd2 = open("/dev/HCSR_2", O_RDWR);
    perror("open failed");
    printf("%d-%d\n", fd, fd2);
    return 0;
}
