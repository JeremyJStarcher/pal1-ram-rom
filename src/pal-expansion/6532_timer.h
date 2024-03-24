
#include <stdint.h>

#pragma once

#define PERIODTYPE_TIM1T 1
#define PERIODTYPE_TIM8T 2
#define PERIODTYPE_TIM64T 3
#define PERIODTYPE_TIM1024T 4

uint8_t riot_read_timer(uint16_t addr);
void riot_write_timer(uint16_t addr, uint8_t data);
void riot_tick(void);
