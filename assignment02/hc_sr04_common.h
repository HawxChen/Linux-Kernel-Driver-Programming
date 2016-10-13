#ifndef _HC_SR04_COMMON_H_
#define _HC_SR04_COMMON_H_
#define __DEUBG__
#include<linux/ioctl.h>
#define ONE_SHOT (0)
#define PERIODIC (1)
#define NUM_ITEM (5) 
#define HCSR_W_MAGIC ('H')
#define SETPINS _IOW (HCSR_W_MAGIC, 1, struct pin_set*)
#define SETMODE _IOW (HCSR_W_MAGIC, 2, struct pin_set*)
#define PIN_SIZE (2)
typedef struct pin_set {
    int trigger_pin;
    int echo_pin;
} pin_set;

typedef struct hcsr_set {
    int mode;
    int freq;
} hcsr_set;

typedef struct record {
    int data[NUM_ITEM];
} record;
#endif
