#pragma once

#include <ctype.h>
#include <stdint.h>

#define RO_MEMORY_BIT 15
#define IN_USE_BIT 14
// This bit set means that location will never be used as RAM or ROM
// prevent the IO ports from getting stomped on
#define EXCLUDE_BIT 13

#define MAGIC_PRIMED_KEY "Q!Q!"

#define SYS_MEMORY_SIZE 65536  // Number of words
#define SYS_DESCRIPTION_SIZE 20
#define SYS_PRIMED_SIZE 6   // Has this structure been primised yet?
#define SYS_ALIGNMENT 4096  // 4K boundary

// Calculate the total size of 'memory' and 'description' fields in bytes
#define SYS_TOTAL_SIZE                                           \
  ((SYS_MEMORY_SIZE * sizeof(uint16_t)) + SYS_DESCRIPTION_SIZE + \
   SYS_PRIMED_SIZE)

// Calculate padding required to align the total size to the nearest 4K boundary
#define SYS_REQUIRED_PADDING (SYS_ALIGNMENT - (SYS_TOTAL_SIZE % SYS_ALIGNMENT))

typedef struct {
  uint16_t memory[SYS_MEMORY_SIZE];
  char description[SYS_DESCRIPTION_SIZE];
  char primed_flag[SYS_PRIMED_SIZE];
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
