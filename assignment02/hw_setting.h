#ifndef _HW_SETTING_H_
#define PULL_DOWN (0)
#define PULL_UP (1)
#define DIR_IN (1)
#define DIR_OUT (0)
#define TRIGGER_INDEX (0)
#define ECHO_INDEX (1)
#define SET_SIZE (2)
#define PIN_SIZE (5)
#define PIN_INDEX (0)
#define VAL_INDEX (1)
#define HC_GPIO_LINUX (0)
#define HC_GPIO_LEVEL (1)
#define HC_GPIO_PULL  (2)
#define HC_GPIO_MUX0  (3)
#define HC_GPIO_MUX1  (4)

char A_pins[2][5][2] = { 
    //Linux   Level             Pull            Mux0      Mux1
/*IO 4 */    {{HCSR_A_TRIG_PIN, 0}, {36, DIR_OUT}, {-1,-1},          {-1,-1}, {-1,-1}},
/*IO12*/   {{HCSR_A_ECHO_PIN, 0}, {42,  DIR_IN}, { 43, PULL_DOWN}, {-1,-1}, {-1, -1}},

};
char* A_pin_str[2][5] = { 
    {"HCSR_A_TRIG", "HCSR_A_TRIG_LEVEL",  "HCSR_A_TRIG_PULL",  "HCSR_A_TRIG_MUX0", "HCSR_A_TRIG_MUX1"},
    {"HCSR_A_ECHO", "HCSR_A_ECHO_LEVEL",  "HCSR_A_ECHO_PULL",  "HCSR_A_ECHO_MUX0", "HCSR_A_ECHO_MUX1"},
};

char B_pins[2][5][2] = { 
          //Linux   Level             Pull            Mux0      Mux1

//old
/*IO10*/   {{HCSR_B_TRIG_PIN, 0}, {26, DIR_OUT}, {-1,-1},          {74, 0}, {-1,-1}},
/*IO 6*/   {{HCSR_B_ECHO_PIN, 0}, {20,  DIR_IN}, { 21, PULL_DOWN}, {68, 0}, {-1, -1}},
};
char* B_pin_str[2][5] = { 
    {"HCSR_B_TRIG", "HCSR_B_TRIG_LEVEL",  "HCSR_B_TRIG_PULL",  "HCSR_B_TRIG_MUX0", "HCSR_B_TRIG_MUX1"},
    {"HCSR_B_ECHO", "HCSR_B_ECHO_LEVEL",  "HCSR_B_ECHO_PULL",  "HCSR_B_ECHO_MUX0", "HCSR_B_ECHO_MUX1"},
};
#endif
