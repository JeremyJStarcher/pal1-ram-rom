#include "6532_timer.h"

#include <stdint.h>
#include <stdio.h>

#include "rambuf.h"

uint32_t riot_counter = 1;
int32_t period_type = PERIODTYPE_TIM1T;

void setup_riot_lookup_tables() {
  sys_state.periodtypemap[0x04] = (PERIODTYPE_TIM1T) + 0;
  sys_state.periodtypemap[0x05] = (PERIODTYPE_TIM8T) + 3;
  sys_state.periodtypemap[0x06] = (PERIODTYPE_TIM64T) + 6;
  sys_state.periodtypemap[0x07] = (PERIODTYPE_TIM1024T) + 10;

  sys_state.periodtypemap[0x0C] = (PERIODTYPE_TIM1T) + 0;
  sys_state.periodtypemap[0x0D] = (PERIODTYPE_TIM8T) + 3;
  sys_state.periodtypemap[0x0E] = (PERIODTYPE_TIM64T) + 6;
  sys_state.periodtypemap[0x0F] = (PERIODTYPE_TIM1024T) + 10;

  sys_state.memory[0x1704] = (PERIODTYPE_TIM1T) + 0;
  sys_state.memory[0x1705] = (PERIODTYPE_TIM8T) + 3;
  sys_state.memory[0x1706] = (PERIODTYPE_TIM64T) + 6;
  sys_state.memory[0x1707] = (PERIODTYPE_TIM1024T) + 10;

  sys_state.memory[0x170C] = (PERIODTYPE_TIM1T) + 0;
  sys_state.memory[0x170D] = (PERIODTYPE_TIM8T) + 3;
  sys_state.memory[0x170E] = (PERIODTYPE_TIM64T) + 6;
  sys_state.memory[0x170F] = (PERIODTYPE_TIM1024T) + 10;
 

  sys_state.shiftmap[0x04] = 0;
  sys_state.shiftmap[0x05] = 3;
  sys_state.shiftmap[0x06] = 6;
  sys_state.shiftmap[0x07] = 10;

  sys_state.shiftmap[0x0C] = 0;
  sys_state.shiftmap[0x0D] = 3;
  sys_state.shiftmap[0x0E] = 6;
  sys_state.shiftmap[0x0F] = 10;

  sys_state.shiftmap2[0] = 0;
  sys_state.shiftmap2[1] = 3;
  sys_state.shiftmap2[2] = 6;
  sys_state.shiftmap2[3] = 10;
}
