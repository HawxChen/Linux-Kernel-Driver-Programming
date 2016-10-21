#ifndef _PLAT_DEVICE_COMMON_H_
#define _PLAT_DEVICE_COMMON_H_
#define __DEUBG__
#include<linux/ioctl.h>
#define ONE_SHOT (0)
#define NUM_DATA (5) 
#define HCSR_W_MAGIC ('H')
#define SETPINS _IOW (HCSR_W_MAGIC, 1, struct pin_set*)
#define SETMODE _IOW (HCSR_W_MAGIC, 2, struct pin_set*)
#define U_PIN_SIZE (2)
#define  HCSR_A_TRIG_PIN (6)
#define  HCSR_A_ECHO_PIN (15)
#define  HCSR_B_TRIG_PIN (10)
#define  HCSR_B_ECHO_PIN (1)
#endif
