#include "commands.h"

#include <hardware/flash.h>
#include <hardware/sync.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "clockspeed.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "rambuf.h"
#include "tusb.h"

#define MAX_VALID_INDEX ((_flash_size / 2) / sizeof(SysStateStruct)) - 1

void main_memory_loop(void);

unsigned long _xip_base;
unsigned long _flash_size;
uint32_t ints;

bool load_ptp_error = false;
bool load_ptp_done = false;

void pause() {
  // If the USB is still busy when we enter a critical section,
  // bad things tend to happen, so give time for the USB bursts to
  // to through.

  volatile long long i;
  long long count = 1 * 10000000;

  for (i = 0; i < count; i++) {
    // Empty loop body
  }
}

uintptr_t get_offset(size_t index) {
  size_t size =
      sizeof(SysStateStruct);  // Assuming you want the size of the structure
  return (_flash_size / 2) + (index * size);
}

bool is_safe_to_access(size_t index) {
  if (index > MAX_VALID_INDEX) {
    return false;  // Index is out of valid range
  }

  uintptr_t offset = get_offset(index);
  uintptr_t end_address = XIP_BASE + offset + sizeof(SysStateStruct);
  // Check if end address exceeds flash size or other boundary conditions
  if (end_address > XIP_BASE + _flash_size) {
    return false;
  }

  // Add any additional checks for alignment here if necessary
  // For example, if your system requires 4-byte alignment:
  if ((XIP_BASE + offset) % (4 * 1024) != 0) {
    return false;  // Address is not properly aligned
  }

  return true;  // Safe to access
}

SysStateStruct *get_slot_ptr(size_t index) {
  if (!is_safe_to_access(index)) {
    printf("Error: Unsafe to access the requested memory slot.\n");
    return NULL;  // Use NULL for pointer error signaling
  }

  size_t size = sizeof(SysStateStruct);
  uintptr_t offset = get_offset(index);
  return (SysStateStruct *)(XIP_BASE +
                            offset);  // Correctly cast and return the pointer
}

void load_slot_command(size_t index, SysStateStruct *state) {
  memcpy(state, get_slot_ptr(index), sizeof(SysStateStruct));
}
void crit_start() {
  pause();
  multicore_reset_core1();
  ints = save_and_disable_interrupts();
  set_sys_clock_khz(CLOCK_SPEED_LOW, false);
}

void crit_end() {
  restore_interrupts(ints);
  multicore_launch_core1(main_memory_loop);
  set_sys_clock_khz(CLOCK_SPEED_HIGH, false);
  pause();
}

uint16_t parse_word_from_string(char *line) {
  char temp_s[5];
  temp_s[0] = line[0];
  temp_s[1] = line[1];
  temp_s[2] = line[2];
  temp_s[3] = line[3];
  temp_s[4] = 0;

  return (uint16_t)strtol(temp_s, NULL, 16);
}

uint8_t parse_byte_from_string(char *line) {
  char temp_s[5];

  temp_s[0] = line[0];
  temp_s[1] = line[1];
  temp_s[2] = 0;

  return (uint8_t)strtol(temp_s, NULL, 16);
}

void start_ptp_load(void) {
  load_ptp_error = false;
  load_ptp_done = false;
}

bool load_ptp_line(char *line) {
  int idx = 0;
  uint16_t addr;
  int count;
  uint8_t data;

  uint16_t checksum = 0;
  uint16_t given_checksum = 0;

  // Skip blank/short lines.
  if (strlen(line) < 5) {
    return true;
  }

  // Search until we find the starting ';'
  while (line[idx] != ';') {
    idx += 1;
  }

  // Skip the ';'
  idx += 1;

  // Convert hex string to number
  count = parse_byte_from_string(&line[idx]);
  if (count == 0) {
    load_ptp_done = true;
    return true;
  }

  checksum += count;

  // skip the count
  idx += 2;

  addr = parse_word_from_string(&line[idx]);
  checksum += (int)addr / 256;
  checksum += (int)addr % 256;

  // And skip the address
  idx += 4;

  while (count > 0) {
    data = parse_byte_from_string(&line[idx]);
    checksum += data;
    idx += 2;
    count--;

    pokeram(addr, data);
    addr += 1;
  }

  given_checksum = parse_word_from_string(&line[idx]);

  if (given_checksum == checksum) {
    // printf(" *MATCHED\n");
    return true;
  }
  // printf(" *FAILED\n");

  load_ptp_error = true;
  return false;
}

/*****/

void read_string(char *buffer, int max_length) {
  int count = 0;  // Number of characters currently in the buffer
  char ch;        // Character read from stdin

  while (1) {
    ch = getchar();  // Read a character

    ch = toupper(ch);


    // Check for line end: '\n' or '\r'
    if (ch == '\n' || ch == '\r') {
      break;
    }

    // Handle backspace ('\b' or ASCII 127)
    if ((ch == '\b' || ch == 127) && count > 0) {
      // Remove the last character from the buffer
      count--;
      // Echo backspace sequence to terminal (backspace, space, backspace)
      putchar('\b');
      putchar(' ');
      putchar('\b');
      continue;
    }

    // In the printable range?
    if (ch >= 32 && ch <= 126) {
      // If the buffer is not full, add the character to the buffer
      if (count < max_length - 1) {
        buffer[count++] = ch;
        putchar(ch);  // Echo the character
      }
    }
  }

  // Null-terminate the string
  buffer[count] = '\0';
}

void help_command() {
  printf("---- HELP ----- \n");
  printf("L: Load a paper tape file \n");
  printf("MSB: FILL ROM WITH MSB (USED TO VERIFY ADDRESS CODING)\n");
  printf("LSB: FILL ROM WITH LSB (USED TO VERIFY ADDRESS CODING)\n");
  printf("RESET: RESET RAM/ROM TO POWERUP STATE\n");
  printf("SAVE #: SAVE TO A SLOT IN FLASH MEMORY\n");
  printf("LOAD #: LOAD FROM A SLOT IN FLASH MEMORY\n");
  printf("LIST: LIST CONFIGURATIONS TO LOAD\n");
  printf("BLOAD [RAM|ROM] ####: LOAD A BINARY FILE INTO THE GIVEN ADDRESS\n");
  //printf("PAUSE: PAUSE DEMO\n");
}

void fill_rom_msb() {
  uint16_t addr;
  uint8_t data;
  for (addr = 0x0000; addr < 0xFFFF; addr += 1) {
    data = addr / 256;
    pokerom(addr, data);
  }
}

void fill_rom_lsb() {
  uint16_t addr;
  uint8_t data;
  for (addr = 0x0000; addr < 0xFFFF; addr += 1) {
    data = addr % 256;
    pokerom(addr, data);
  }
}

void loadptp_command() {
  char input[200];  // Define the buffer size

  start_ptp_load();

  while (!load_ptp_done) {
    read_string(input, sizeof(input));
    load_ptp_line(input);
  }

  if (load_ptp_error) {
    printf("*ERR*");
  } else {
    printf("*KIM1*");
  }
}

void save_slot_command(size_t index) {
  if (!is_safe_to_access(index)) {
    printf("Error: Unsafe to access the requested memory slot.\n");
    printf("ONLY SLOTS %02x to %02x MAY BE USED\n", 0, MAX_VALID_INDEX);
    return;  // Exit or handle error appropriately
  }

  size_t size =
      sizeof(SysStateStruct);  // Assuming you want the size of the structure
  uintptr_t offset = get_offset(index);

  printf("flash size: %08x\n", _flash_size);
  printf("SAVE SLOT: %d OFFSET %04x  size: %08x\n", index, offset, size);
  if (offset % 4096 == 0) {
    printf("ON THE BOUNDARY\n");
  } else {
    printf("OOPS\n");
  }

  pause();
  size_t idx = offset;

  crit_start();
  flash_range_erase(idx, size);
  flash_range_program(offset, (char *)&sys_state, size);
  crit_end();
}

void get_slot_state(size_t i, SlotStateStruct *slot_state) {
  char flag[] = MAGIC_PRIMED_KEY;
  SysStateStruct *state = get_slot_ptr(i);

  unsigned int c0 = state->primed_flag[0];
  unsigned int c1 = state->primed_flag[1];
  unsigned int c2 = state->primed_flag[2];
  unsigned int c3 = state->primed_flag[3];

  bool pass = true;

  // printf("\n:::%c %c %c %c\n", c0, c1, c2, c3);
  // printf(":::%c %c %c %c\n", flag[0], flag[1], flag[2], flag[3]);

  if (c0 != flag[0]) {
    pass = false;
  }
  if (c1 != flag[1]) {
    pass = false;
  }
  if (c2 != flag[2]) {
    pass = false;
  }
  if (c3 != flag[3]) {
    pass = false;
  }

  slot_state->primed = pass;

  if (pass) {
    scpy(slot_state->description, state->description, SYS_DESCRIPTION_SIZE);
  } else {
    scpy(slot_state->description, "<unset>", SYS_DESCRIPTION_SIZE);
  }
}

void list_slots_command() {
  for (size_t i = 0; i < MAX_VALID_INDEX; i++) {
    printf("SLOT: %02x ", i);

    SlotStateStruct slot_state;
    get_slot_state(i, &slot_state);

    puts(slot_state.description);
  }
}

void bload(char *token) {
  printf("BLOAD...\n");
  uint32_t addr;
  bool inrom = false;

  token = strtok(NULL, " ");
  if (strlen(token) == 0) {
    printf("MUST SPECIFY RAM/ROM");
    return;
  }

  if (token[1] == 'O') {
    inrom = true;
  }

  token = strtok(NULL, " ");

  if (strlen(token) == 0) {
    printf("MUST SPECIFY ADDRESS");
    return;
  }

  addr = (uint8_t)strtol(token, NULL, 16);

  printf("RAM/ROM %s\n", inrom ? "ROM" : "RAM" );
  printf("ADDR %04x\n", addr);
}

void command_loop(unsigned long xip_base, unsigned long flash_size) {
  char input[20];  // Define the buffer size

  _xip_base = xip_base;
  _flash_size = flash_size;

  while (1) {
    printf("\nENTER COMMAND (OR HELP): ");
    read_string(input, sizeof(input));
    // printf("\nYou entered: %s\n", input);

    char *command = strtok(input, " ");  // Get the first word as the command
    if (command == NULL) {
      continue;  // If no command is entered, skip to the next iteration
    }

    printf("\n");

    // Compare input with known commands and call the corresponding function
    if (strcmp(command, "HELP") == 0) {
      help_command();
    } else if (strcmp(command, "PAUSE") == 0) {
      puts("LINE 1 - Pausing");
      pause();
      puts("LINE 2 - Pausing");
    } else if (strcmp(command, "L") == 0) {
      loadptp_command();
    } else if (strcmp(command, "LSB") == 0) {
      fill_rom_lsb();
    } else if (strcmp(command, "MSB") == 0) {
      fill_rom_msb();
    } else if (strcmp(command, "RESET") == 0) {
      setup_memory_contents();

    } else if (strcmp(command, "SAVE") == 0) {
      char *param_str =
          strtok(NULL, " ");  // Try to get the next word as a parameter
      int index = -1;         // Default index value if no parameter is provided
      if (param_str != NULL) {
        // index = atoi(param_str); // Convert parameter string to integer
        index = (uint8_t)strtol(param_str, NULL, 16);

        save_slot_command(index);

      } else {
        printf("MUST SPECIFY AN INDEX NUMBER\n");
      }

    } else if (strcmp(command, "LOAD") == 0) {
      char *param_str =
          strtok(NULL, " ");  // Try to get the next word as a parameter
      int index = -1;         // Default index value if no parameter is provided
      if (param_str != NULL) {
        // index = atoi(param_str); // Convert parameter string to integer
        index = (uint8_t)strtol(param_str, NULL, 16);
        load_slot_command(index, &sys_state);

      } else {
        printf("MUST SPECIFY AN INDEX NUMBER\n");
      }

    } else if (strcmp(command, "LIST") == 0) {
      list_slots_command();
    } else if (strcmp(command, "BLOAD") == 0) {
      bload(command);
    } else if (strcmp(command, "") == 0) {
      // ignore it.
    } else {
      printf("HUH?\n");
    }
  }
}

void print_binary(unsigned int value, int bits) {
  unsigned int mask =
      1 << (bits - 1);  // Create a mask for the most significant bit

  for (int i = 0; i < bits; i++) {
    putchar((value & mask) ? '1' : '0');  // Check if the current bit is set
    value <<= 1;  // Shift the value left to check the next bit
  }
}
