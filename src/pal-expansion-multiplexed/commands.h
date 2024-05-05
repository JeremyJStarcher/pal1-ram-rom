#pragma once

#include "rambuf.h"
#include "stdbool.h"

typedef struct {
  char description[SYS_DESCRIPTION_SIZE];
  bool primed;
} SlotStateStruct;

void read_string(char *buffer, int max_length);
void print_binary(unsigned int value, int bits);
void get_slot_state(size_t i, SlotStateStruct *slot_state);

#define XCH_TO_PAL 0x1FF7
#define XCH_FROM_PAL 0x1FF8
#define XCH_CMD_REG 0x1FF9

#define XCH_CMD_NONE 0
#define XCH_CMD_CHOUT 1
#define XCH_CMD_CHIN 2
