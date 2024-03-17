#include "rambuf.h"

#include <pico/rand.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pin_definitions.h"

// #include "rom_ext.c"

SysStateStruct sys_state;

#define FLASH_SECTOR_SIZE (4 * 1024)

void scpy(char *dest, char *src, size_t len);

void setexcludebyte(uint16_t addr) {
  uint16_t data = 1 << EXCLUDE_BIT;
  sys_state.memory[addr] = data;
}

bool is_excluded(uint16_t addr) {
  return (sys_state.memory[addr] & (1 << EXCLUDE_BIT)) != 0;
}

void pokeram(uint16_t addr, uint8_t value) {
  if (is_excluded(addr)) {
    return;
  }

  uint16_t data = value;

  // There are gaps in the pins, so adjust the data.
  // Any bit position not part of the data mask is a
  // "don't care".

  if (data & 1 << 7) {
    data |= 1 << (D7 - D0);
  }

  data |= 1 << IN_USE_BIT;

  sys_state.memory[addr] = data;
}

void pokerom(uint16_t addr, uint8_t value) {
  if (is_excluded(addr)) {
    return;
  }
  pokeram(addr, value);

  uint16_t data = sys_state.memory[addr];
  data |= 1 << RO_MEMORY_BIT;

  sys_state.memory[addr] = data;
}

void dpokerom(uint16_t addr, uint16_t value) {
  uint16_t addr_low = addr;
  uint16_t addr_high = addr + 1;

  uint8_t data_high = value >> 8;
  uint8_t data_low = value & 0xFF;

  pokerom(addr_low, data_low);
  pokerom(addr_high, data_high);
}

void dpokeram(uint16_t addr, uint16_t value) {
  uint16_t addr_low = addr;
  uint16_t addr_high = addr + 1;

  uint8_t data_high = value >> 8;
  uint8_t data_low = value & 0xFF;

  pokeram(addr_low, data_low);
  pokeram(addr_high, data_high);
}

void scpy(char *dest, char *src, size_t len) {
  strncpy(dest, src, len - 1);
  dest[len - 1] = '\0';
}

void setup_memory_contents() {
  uint32_t idx = 0;
  uint8_t data = 0;

  // memcpy(sys_state.description, const void* src, size_t n);
  // sys_state.memoryp

  scpy(sys_state.description, "EMPTY RAM", SYS_DESCRIPTION_SIZE);
  scpy(sys_state.primed_flag, MAGIC_PRIMED_KEY, SYS_PRIMED_SIZE);
  char primed_flag[SYS_PRIMED_SIZE];

  puts("-------------------------------");
  puts(sys_state.description);
  puts(sys_state.primed_flag);
  puts("\n");

  // Block out the IO/Timer ranges to not be touched
  // $1700 - $173F I/O, timer of 6530-003
  // $1740 - $177F I/O, timer of 6530-002
  for (idx = 0x1700; idx < 0x177F + 1; idx += 1) {
    setexcludebyte(idx);
  }

  // Upper RAM
  for (idx = 0x2000; idx < 0xFFFF; idx += 1) {
    pokeram(idx, get_rand_64());
  }

  // BASE RAM
  // this includes the RRIOT RAM.  The RRIOT I/O and TIMER
  // ranges are excluded.
  for (idx = 0x0000; idx < 0x17FF + 1; idx += 1) {
    pokeram(idx, get_rand_64());
  }

  // While we could replace the entire ROM here, if we do that
  // single step mode will no longer skip the ROM content as it
  // uses physical address lines.
  pokerom(0x1FDD + 0, 'L');
  pokerom(0x1FDD + 1, 'A');
  pokerom(0x1FDD + 2, 'P');

  dpokerom(0xFFFA, 0x1C1C);
  dpokerom(0xFFFC, 0x1C22);
  dpokerom(0xFFFE, 0x1C1F);
}
