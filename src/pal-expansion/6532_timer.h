
#include <stdint.h>

#pragma once

#define PERIODTYPE_TIM1T 0
#define PERIODTYPE_TIM8T 1
#define PERIODTYPE_TIM64T 2
#define PERIODTYPE_TIM1024T 3

uint8_t riot_read_timer(uint16_t addr);
void riot_write_timer(uint16_t addr, uint8_t data);
void riot_tick(void);
