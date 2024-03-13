#pragma once

#include "rambuf.h"
#include "stdbool.h"

void read_string(char *buffer, int max_length);
void command_loop();
void print_binary(unsigned int value, int bits);


typedef struct {
    char description[SYS_DESCRIPTION_SIZE];
    bool primed;
}  SlotStateStruct;
