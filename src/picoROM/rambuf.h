#pragma once

#define RO_MEMORY_BIT 15

extern uint16_t rom_contents[];


void set_rom_byte(uint32_t addr, uint8_t value);
void setup_rom_contents(void);
