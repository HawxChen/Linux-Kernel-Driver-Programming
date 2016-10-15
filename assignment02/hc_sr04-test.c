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
typedef struct pdata {
    int fd;
    hcsr_set set;
} pdata;
void* pfunction(void* datain) {
    int i = 0;
    pdata* data = (pdata*) datain;
    printf("mode:%d, freq:%u\n", data->set.mode, data->set.freq);
    ioctl(data->fd, SETMODE,(int)&(data->set), 0);
    i = 1;
    read(data->fd, &i, sizeof(int));
    sleep(1);


    i = 1;//ONE_SHOT: clear data, PEORIDIC: start thread
    write(data->fd, &i, sizeof(int));
    sleep(1);

    i = 0;
    write(data->fd, &i, sizeof(int));
    return NULL;
}

int main(int argc, char*argv[]) {
    //pin_set pins = {38, 40};
    
//    pin_set pins = {10, 1};
    pdata Adata;
    pdata Bdata;
    int i = 1;
    pin_set Apins = {HCSR_A_TRIG_PIN, HCSR_A_ECHO_PIN};
    pin_set Bpins = {HCSR_B_TRIG_PIN, HCSR_B_ECHO_PIN};
    hcsr_set Aset = {0, 7};
    hcsr_set Bset = {1, 7};
    int ret = 0;
    int fdA = open("/dev/HCSR_1", O_RDWR);
    ret = ioctl(fdA, SETPINS, &Apins, 0);
    perror("SETPIN");

    int fdB = open("/dev/HCSR_2", O_RDWR);
    ret = ioctl(fdB, SETPINS, &Bpins, 0);
    perror("SETPIN");


    
    //For single thread test
    ret = ioctl(fdA, SETMODE, &Aset, 0);
    perror("IOCTL");

    i = 0;
    write(fdA, &i, sizeof(int));
    write(fdA, &i, sizeof(int));
    i = 1; //clear
    write(fdA, &i, sizeof(int));

    read(fdA, &i, sizeof(int));
    read(fdA, &i, sizeof(int));

    //B
    ret = ioctl(fdB, SETMODE, &Bset, 0);
    if(0 != ret)
        perror("SETMODE");
    read(fdB, &i, sizeof(int));
    sleep(1);

    i = 1; //start thread
    write(fdB, &i, sizeof(int));
    sleep(1);
    i = 0; //stop thread
    write(fdB, &i, sizeof(int));

    
    Bset.mode = 0;
    ret = ioctl(fdB, SETMODE, &Bset, 0);
    if(0 != ret)
        perror("SETMODE");
    i = 0;

    i = 1; //clear fdA
    write(fdA, &i, sizeof(int));

    i = 1; //clear fdB
    write(fdB, &i, sizeof(int));
   
   //For multi-thread test
   Adata.fd = fdA;
   Adata.set.mode = 1;
   Adata.set.freq = 7;
   Bdata.fd = fdB;
   Bdata.set.mode = 0;
   Bdata.set.freq = 7;
   pthread_t tA, tB;
   pthread_create(&tA, NULL, pfunction, &Adata);
   pthread_create(&tB, NULL, pfunction, &Bdata);

   pthread_join(tA, NULL);
   pthread_join(tB, NULL);

    return 0;
}
