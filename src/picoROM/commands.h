#pragma once

void read_string(char *buffer, int max_length);
void command_loop();
void print_binary(unsigned int value, int bits);
void save_slot_command(int index);
     
extern  int flash_save_index;
