#include "6532_timer.h"

#include <stdint.h>
#include <stdio.h>

uint32_t riot_counter = 1;
int32_t period_type = PERIODTYPE_TIM1T;

// Maybe we have to assign these values manually to get them to be in RAM

int32_t periodtypemap[16];
int32_t shiftmap[16];
int32_t shiftmap2[4];

void setup_riot_lookup_tables() {
  periodtypemap[0x04] = PERIODTYPE_TIM1T;
  periodtypemap[0x05] = PERIODTYPE_TIM8T;
  periodtypemap[0x06] = PERIODTYPE_TIM64T;
  periodtypemap[0x07] = PERIODTYPE_TIM1024T;

  periodtypemap[0x0C] = PERIODTYPE_TIM1T;
  periodtypemap[0x0D] = PERIODTYPE_TIM8T;
  periodtypemap[0x0E] = PERIODTYPE_TIM64T;
  periodtypemap[0x0F] = PERIODTYPE_TIM1024T;

  shiftmap[0x04] = 0;
  shiftmap[0x05] = 3;
  shiftmap[0x06] = 6;
  shiftmap[0x07] = 10;

  shiftmap[0x0C] = 0;
  shiftmap[0x0D] = 3;
  shiftmap[0x0E] = 6;
  shiftmap[0x0F] = 10;

  shiftmap2[0] = 0;
  shiftmap2[1] = 3;
  shiftmap2[2] = 6;
  shiftmap2[3] = 10;
}
