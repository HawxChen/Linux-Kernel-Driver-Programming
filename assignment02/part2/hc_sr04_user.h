#ifndef _HC_SR04_USR_H_
#define _HC_SR04_USR_H_
#include"hc_sr04_common.h"
#include"hw_setting.h"
#include <unistd.h>
#include<sys/ioctl.h>
#define __HC_DEBUG_USER__
#ifdef __HC_DEBUG_USER__
#define PRINT(name, fd) do {int TMP; read((fd), &TMP,sizeof(int)); printf("%s's distance: %d-\n", name, TMP);}while(0);
#else
#define PRINT(name, fd)
#endif
typedef struct pdata {
    int fd; char*name;
    int get_i;
    hcsr_set set;
} pdata;
#endif
