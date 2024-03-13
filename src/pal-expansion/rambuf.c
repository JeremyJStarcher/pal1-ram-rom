#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pin_definitions.h"
#include "rambuf.h"


// #include "rom_ext.c"


SysStateStruct sys_state;

#define FLASH_SECTOR_SIZE (4 * 1024)



void scpy(char *dest, char *src, size_t len);

void set_ram_byte(uint32_t addr, uint8_t value) {
    uint16_t data = value;

    // There are gaps in the pins, so adjust the data.
    // Any bit position not part of the data mask is a
    // "don't care".

    if (data & 1 << 7) {
        data |= 1 << (D7 - D0);
    }
    sys_state.memory[addr] = data;
}

void set_rom_byte(uint32_t addr, uint8_t value) {
    set_ram_byte(addr, value);

    uint16_t data = sys_state.memory[addr];
    data |= 1 << RO_MEMORY_BIT;
    sys_state.memory[addr] = data;
}


void scpy(char *dest, char *src, size_t len) {
    strncpy(dest, src, len - 1);
    dest[len - 1] = '\0';
}


void setup_memory_contents() {
    uint32_t idx = 0;
    uint8_t data = 0;

    //memcpy(sys_state.description, const void* src, size_t n);
    //sys_state.memoryp

    scpy(sys_state.description, "EMPTY RAM", SYS_DESCRIPTION_SIZE);
    scpy(sys_state.primed_flag, MAGIC_PRIMED_KEY, SYS_PRIMED_SIZE);
    char primed_flag[SYS_PRIMED_SIZE];


    puts("-------------------------------");
    puts(sys_state.description);
    puts(sys_state.primed_flag);
    puts("\n");

    for(idx = 0x0000; idx < 0xFFFF; idx+=1) {
        set_ram_byte(idx, 0);
    }


    // for(idx = 0; idx <  rom_extSize; idx += 1) {
    //     data = rom_ext[idx];
    //     set_rom_byte(0xA000+idx, data);
    // }

    set_rom_byte(0xFFFA, 0x1C);
    set_rom_byte(0xFFFB, 0x1C);
    set_rom_byte(0xFFFC, 0x22);
    set_rom_byte(0xFFFD, 0x1C);
    set_rom_byte(0xFFFE, 0x1F);
    set_rom_byte(0xFFFF, 0x1C);
}
