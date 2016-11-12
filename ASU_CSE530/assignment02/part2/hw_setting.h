#ifndef _HW_SETTING_H_
#define _HW_SETTING_H_
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
#define PHY_IO_NUM    (20)

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

/*IO10*/   {{HCSR_B_TRIG_PIN, 0}, {26, DIR_OUT}, {-1,-1},          {74, 0}, {-1,-1}},
/*IO 6*/   {{HCSR_B_ECHO_PIN, 0}, {20,  DIR_IN}, { 21, PULL_DOWN}, {68, 0}, {-1, -1}},
};
char* B_pin_str[2][5] = { 
    {"HCSR_B_TRIG", "HCSR_B_TRIG_LEVEL",  "HCSR_B_TRIG_PULL",  "HCSR_B_TRIG_MUX0", "HCSR_B_TRIG_MUX1"},
    {"HCSR_B_ECHO", "HCSR_B_ECHO_LEVEL",  "HCSR_B_ECHO_PULL",  "HCSR_B_ECHO_MUX0", "HCSR_B_ECHO_MUX1"},
};

char all_pins[PHY_IO_NUM][5][2] = {
/*IO 0*/    {{11, 0},              {32,      -1}, {33,PULL_DOWN},   {-1,-1}, {-1,-1}},
/*IO 1*/    {{12, 0},              {28,      -1}, {29,PULL_DOWN},   {45, 0}, {-1,-1}},
/*IO 2*/    {{13, 0},              {34,      -1}, {35,PULL_DOWN},   {77, 0}, {-1,-1}},
/*IO 3*/    {{14, 0},              {16,      -1}, {17,PULL_DOWN},   {76, 0}, {64, 0}},
/*IO 4*/    {{ 6, 0},              {36,      -1}, {37,PULL_DOWN},   {-1,-1}, {-1,-1}}, //Defult
/*IO 5*/    {{ 0, 0},              {18,      -1}, {19,PULL_DOWN},   {66, 0}, {-1,-1}},
/*IO 6*/    {{1, 0},               {20,      -1}, {21,PULL_DOWN},   {68, 0}, {-1,-1}}, //Defult
/*IO 7*/    {{38, 0},              {-1, DIR_OUT}, {-1,PULL_DOWN},   {-1,-1}, {-1,-1}},
/*IO 8*/    {{40, 0},              {-1, DIR_OUT}, {-1,PULL_DOWN},   {-1,-1}, {-1,-1}},
/*IO 9*/    {{ 4, 0},              {22,      -1}, {23,PULL_DOWN},   {70, 0}, {-1,-1}},
/*IO10*/    {{10, 0},              {26,      -1}, {27,PULL_DOWN},   {74, 0}, {-1,-1}}, //Defult
/*IO11*/    {{ 5, 0},              {24,      -1}, {25,PULL_DOWN},   {44, 0}, {72, 0}},
/*IO12*/    {{15, 0},              {42,      -1}, {43,PULL_DOWN},   {-1,-1}, {-1,-1}},  //Defult
/*IO13*/    {{ 7, 0},              {30,      -1}, {31,PULL_DOWN},   {46, 0}, {-1,-1}},
/*IO14*/    {{48, 0},              {-1, DIR_OUT}, {-1,PULL_DOWN},   {-1,-1}, {-1,-1}}, //A0
/*IO15*/    {{50, 0},              {-1, DIR_OUT}, {-1,PULL_DOWN},   {-1,-1}, {-1,-1}},
/*IO16*/    {{52, 0},              {-1, DIR_OUT}, {-1,PULL_DOWN},   {-1,-1}, {-1,-1}},
/*IO17*/    {{54, 0},              {-1, DIR_OUT}, {-1,PULL_DOWN},   {-1,-1}, {-1,-1}},
/*IO18*/    {{56, 0},              {-1, DIR_OUT}, {-1,PULL_DOWN},   {-1,-1}, {-1,-1}},
/*IO19*/    {{58, 0},              {-1, DIR_OUT}, {-1,PULL_DOWN},   {-1,-1}, {-1,-1}},
};
#endif
