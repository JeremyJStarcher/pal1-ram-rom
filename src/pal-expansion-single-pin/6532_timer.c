#include "6532_timer.h"

#include <stdint.h>
#include <stdio.h>

uint32_t riot_counter = 1;
int32_t period_type = PERIODTYPE_TIM1T;

int32_t __attribute__((section(".data"))) periodtypemap[] = {
    -1,                   // 00
    -1,                   // 01
    -1,                   // 02
    -1,                   // 03
    PERIODTYPE_TIM1T,     // 04
    PERIODTYPE_TIM8T,     // 05
    PERIODTYPE_TIM64T,    // 06
    PERIODTYPE_TIM1024T,  // 07
    -1,                   // 08
    -1,                   // 09
    -1,                   // 0A
    -1,                   // 0B
    PERIODTYPE_TIM1T,     // 0C
    PERIODTYPE_TIM8T,     // 0D
    PERIODTYPE_TIM64T,    // 0E
    PERIODTYPE_TIM1024T   // 0F
};

int32_t __attribute__((section(".data"))) shiftmap[] = {
    -1,  // 00
    -1,  // 01
    -1,  // 02
    -1,  // 03
    0,   // 04
    3,   // 05
    6,   // 06
    10,  // 07
    -1,  // 08
    -1,  // 09
    -1,  // 0A
    -1,  // 0B
    0,   // 0C
    3,   // 0D
    6,   // 0E
    10   // 0F
};

int32_t __attribute__((section(".data"))) shiftmap2[] = {
    0,  // PERIODTYPE_TIM1T
    3,  // PERIODTYPE_TIM8T
    6,  // PERIODTYPE_TIM64T
    10  // PERIODTYPE_TIM1024T
};
