#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/ioctl.h>
#include"hc_sr04_user.h"
#include"hw_setting.h"
int func(char(*pin)[5][2], char*(*pin_str)[5]) {
    printf("%d, %s\n", pin[1][3][0], pin_str[1][3]); 
    return 0;
}
int main(int argc, char*argv[]) {
    //pin_set pins = {38, 40};
    
    pin_set pins = {10, 1};
//    pin_set pins = {6, 15};
    int ret = 0;
    //int LED = open("/sys/class/gpio/gpio38/value", O_RDWR);
    int fd = open("/dev/HCSR_2", O_RDWR);
//    int fd = open("/dev/HCSR_1", O_RDWR);
    ret = ioctl(fd, SETPINS, &pins, 0 );
    perror("ioctl");
    close(fd);
    
    func(A_pins, A_pin_str);

    return 0;
}
