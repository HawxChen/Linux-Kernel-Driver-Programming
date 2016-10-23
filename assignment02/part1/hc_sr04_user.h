#ifndef _HC_SR04_USR_H_
#define _HC_SR04_USR_H_
#include"hc_sr04_common.h"
#include <unistd.h>
#define __HC_DEBUG_USER__
#ifdef __HC_DEBUG_USER__
#define PRINT(name, fd) do {int TMP; read((fd), &TMP,sizeof(int)); printf("%s's distance: %d-\n", name, TMP);}while(0);
#else
#define PRINT(name, fd)
#endif
#endif
