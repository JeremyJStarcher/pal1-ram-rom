#pragma once

#include "rambuf.h"
#include "stdbool.h"

typedef struct {
  char description[SYS_DESCRIPTION_SIZE];
  bool primed;
} SlotStateStruct;

void read_string(char *buffer, int max_length);
void command_loop();
void print_binary(unsigned int value, int bits);
void get_slot_state(size_t i, SlotStateStruct *slot_state);
