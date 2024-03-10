#include <stdlib.h>
#include <stdio.h>

#include "pin_definitions.h"
#include "rambuf.h"
#include "rom_ext.c"

void set_ram_byte(uint32_t addr, uint8_t value) {
    uint16_t data = value;

    // There are gaps in the pins, so adjust the data.
    // Any bit position not part of the data mask is a
    // "don't care".

    if (data & 1 << 7) {
        data |= 1 << (D7 - D0);
    }
    rom_contents[addr] = data;
}

void set_rom_byte(uint32_t addr, uint8_t value) {
    set_ram_byte(addr, value);

    uint16_t data = rom_contents[addr];
    data |= 1 << RO_MEMORY_BIT;
    rom_contents[addr] = data;
}


void setup_rom_contents() {
    uint32_t idx = 0;
    uint8_t data = 0;

    for(idx = 0x0000; idx < 0xFFFF; idx+=1) {
        set_ram_byte(idx, (uint8_t) idx);
    }

    for(idx = 0; idx <  16384; idx += 1) {
        data = rom_ext[idx];
        set_rom_byte(0xA000+idx, data);
    }

    set_rom_byte(0xFFFA, 0x1C);
    set_rom_byte(0xFFFB, 0x1C);
    set_rom_byte(0xFFFC, 0x22);
    set_rom_byte(0xFFFD, 0x1C);
    set_rom_byte(0xFFFE, 0x1F);
    set_rom_byte(0xFFFF, 0x1C);
}