#include "6532_timer.h"

#include <stdint.h>
#include <stdio.h>

uint32_t riot_counter = 1;
uint8_t period_type = PERIODTYPE_TIM1T;

uint8_t riot_read_timer(uint16_t addr) {
  if (riot_counter & (1 << 18)) {
    // Let the casting get rid of the rest
    return riot_counter;
  }

  if ((addr & 0x000F) == 6) {
    switch (period_type) {
      case PERIODTYPE_TIM1T:
        return riot_counter;
        break;
      case PERIODTYPE_TIM8T:
        return riot_counter >> 3;
        break;
      case PERIODTYPE_TIM64T:
        return riot_counter >> 6;
        break;
      case PERIODTYPE_TIM1024T:
        return riot_counter >> 10;
        break;
    }
  }
}

int periodtypemap[] = {
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

int shiftmap[] = {
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

void riot_write_timer(uint16_t addr, uint8_t data) {
  uint8_t adr = addr & 0x000F;

  if (adr == 0x04) {
    period_type = PERIODTYPE_TIM1T;
    riot_counter = data;
  }
  if (adr == 0x05) {
    period_type = PERIODTYPE_TIM8T;
    riot_counter = data << 3;
  }

  if (adr == 0x06) {
    period_type = PERIODTYPE_TIM64T;
    riot_counter = data << 6;
  }
  if (adr == 0x07) {
    period_type = PERIODTYPE_TIM1024T;
    riot_counter = data << 10;
  }

  //   period_type = periodtypemap[adr];
  //   riot_counter = data << shiftmap[adr];

  //   switch (adr) {
  //     case 0x04:
  //     case 0x0C:
  //       period_type = PERIODTYPE_TIM1T;
  //       riot_counter = data;
  //       break;
  //     case 0x04 + 1:
  //     case 0x0C + 1:
  //       period_type = PERIODTYPE_TIM8T;
  //       riot_counter = data << 3;
  //       break;
  //     case 0x04 + 2:
  //     case 0x0C + 2:
  //       period_type = PERIODTYPE_TIM64T;
  //       riot_counter = data << 6;
  //       break;
  //     case 0x04 + 3:
  //     case 0x0C + 3:
  //       period_type = PERIODTYPE_TIM1024T;
  //       riot_counter = data << 10;
  //       break;
  //   }
}

void riot_tick() { riot_counter -= 1; }
