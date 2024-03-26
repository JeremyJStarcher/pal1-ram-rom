#include "rambuf.h"

#include <pico/rand.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
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

uint16_t dpeekram(uint16_t addr) {
  uint16_t r = 0;

  r = peekram(addr + 1) * 256;
  r |= peekram(addr + 0);
  return r;
}

uint8_t data_16_to_8(uint16_t data) {
  uint8_t value = data;
  value |= ((data >> (D7 - D0)) & 1) << 7;
  return value;
}

uint8_t data_16_to_8_slow(uint16_t data) {
  static const uint8_t bit_positions[8] = {D0, D1, D2, D3, D4, D5, D6, D7};

  uint8_t value = 0;

  for (int i = 0; i < 8; ++i) {
    // Extract the bit from data at the position defined by bit_positions[i]
    // and set the corresponding bit in value.
    if (data & (1 << (bit_positions[i] - D0))) {
      value |= 1 << i;
    }
  }

  return value;  // Return the reconstructed original byte value
}

uint8_t peekram(uint16_t addr) {
  uint16_t data = sys_state.memory[addr];  // Retrieve the stored data
  return data_16_to_8(data);
}

uint16_t data_8_to_16(uint8_t value) {
  uint16_t data = value;
  data |= ((value & (1 << 7)) ? 1 : 0) << D7 - D0;
  return data;
}

uint16_t data_8_to_16_slow(uint8_t value) {
  static const uint8_t bit_positions[8] = {D0, D1, D2, D3, D4, D5, D6, D7};
  uint16_t data = 0;

  for (int i = 0; i < 8; ++i) {
    if (value & (1 << i)) {  // Check if the ith bit of value is set
      data |= 1 << (bit_positions[i] -
                    bit_positions[0]);  // Set the corresponding bit in data
    }
  }

  data |= 1 << IN_USE_BIT;  // Set the IN_USE_BIT in data
  return data;
}

void pokeram(uint16_t addr, uint8_t value) {
  if (is_excluded(addr)) {
    return;
  }

  uint16_t data = data_8_to_16(value);
  data |= 1 << IN_USE_BIT;  // Set the IN_USE_BIT in data
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

#if 1
  // Upper RAM
  for (idx = 0x2000; idx < 0xFFFF; idx += 1) {
    uint8_t rnd = get_rand_64();
    pokeram(idx, rnd);
    if (peekram(idx) != rnd) {
      printf("PEEKRAM FAILED at %04X\r\n");
    }
  }

  // BASE RAM
  // this includes the RRIOT RAM.  The RRIOT I/O and TIMER
  // ranges are excluded.
  for (idx = 0x0000; idx < 0x17FF + 1; idx += 1) {
    uint8_t rnd = get_rand_64();
    pokeram(idx, rnd);
  }
#endif

  // While we could replace the entire ROM here, if we do that
  // single step mode will no longer skip the ROM content as it
  // uses physical address lines.
  pokerom(0x1FDD + 0, 'L');
  pokerom(0x1FDD + 1, 'A');
  pokerom(0x1FDD + 2, 'P');

  dpokerom(0xFFFA, 0x1C1C);
  dpokerom(0xFFFC, 0x1C22);
  dpokerom(0xFFFE, 0x1C1F);

  uint16_t dd = dpeekram(0xFFFC);
  if (dd != 0x1C22) {
    printf("DPEEK FAILED %04X\n", dd);
  }

#if 1
  pokeram(0x00F1, 0x00);  // Clear all flags
  pokeram(0x17F9, 0x00);  // Tape load ID
  dpokeram(0x17FA, 0x1C00);
  dpokeram(0x17FC, 0x1C00);
  dpokeram(0x17FE, 0x1C00);
#endif
}
