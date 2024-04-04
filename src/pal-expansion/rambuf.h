#pragma once

#include <ctype.h>
#include <stdint.h>

#include "rambuf.h"

#define RO_MEMORY_BIT 15
#define IN_USE_BIT 14
// This bit set means that location will never be used as RAM or ROM
// prevent the IO ports from getting stomped on
#define EXCLUDE_BIT 13

#define MAGIC_PRIMED_KEY "Q!Q!"

#define SYS_MEMORY_SIZE 65536  // Number of words
#define SYS_DESCRIPTION_SIZE 72
#define SYS_PRIMED_SIZE 6   // Has this structure been primised yet?
#define SYS_ALIGNMENT 4096  // 4K boundary

// Calculate the total size of 'memory' and 'description' fields in bytes
#define SYS_TOTAL_SIZE                                           \
  ((SYS_MEMORY_SIZE * sizeof(uint16_t)) + SYS_DESCRIPTION_SIZE + \
   SYS_PRIMED_SIZE + ((255 + 255 + 4) * sizeof(uint16_t)))

// Calculate padding required to align the total size to the nearest 4K boundary
#define SYS_REQUIRED_PADDING (SYS_ALIGNMENT - (SYS_TOTAL_SIZE % SYS_ALIGNMENT))

typedef struct {
  uint16_t memory[SYS_MEMORY_SIZE];
  char description[SYS_DESCRIPTION_SIZE];
  char primed_flag[SYS_PRIMED_SIZE];

  uint16_t periodtypemap[255];
  uint16_t shiftmap[255];
  uint16_t shiftmap2[4];

  char padding[SYS_REQUIRED_PADDING];  // Padding to align the total size to 4K
                                       // boundary
} /* __attribute__((aligned(SYS_ALIGNMENT))) */ SysStateStruct;

extern SysStateStruct sys_state;

void pokeram(uint16_t addr, uint8_t value);
void pokerom(uint16_t addr, uint8_t value);
void setup_memory_contents(void);
void scpy(char *dest, char *src, size_t len);
void dpokerom(uint16_t addr, uint16_t value);
void dpokeram(uint16_t addr, uint16_t value);
uint16_t dpeekram(uint16_t addr);
uint8_t peekram(uint16_t addr);
uint8_t data_16_to_8(uint16_t data);
uint16_t data_8_to_16(uint8_t value);