#pragma once

#define RO_MEMORY_BIT 15

#include <stdint.h>

#define SYS_MEMORY_SIZE 65535 // Number of words
#define SYS_DESCRIPTION_SIZE 20
#define SYS_ALIGNMENT 4096 // 4K boundary

// Calculate the total size of 'memory' and 'description' fields in bytes
#define SYS_TOTAL_SIZE ((SYS_MEMORY_SIZE * sizeof(uint16_t)) + SYS_DESCRIPTION_SIZE)

// Calculate padding required to align the total size to the nearest 4K boundary
#define SYS_REQUIRED_PADDING (SYS_ALIGNMENT - (SYS_TOTAL_SIZE % SYS_ALIGNMENT))

typedef struct {
    uint16_t memory[SYS_MEMORY_SIZE];           // Array of 65535 words
    char description[SYS_DESCRIPTION_SIZE];     // 20 byte field for description
    char padding[SYS_REQUIRED_PADDING];         // Padding to align the total size to 4K boundary
} /* __attribute__((aligned(SYS_ALIGNMENT))) */ SysStateStruct;


extern SysStateStruct sys_state;

void set_ram_byte(uint32_t addr, uint8_t value);
void set_rom_byte(uint32_t addr, uint8_t value);
void setup_rom_contents(void);
