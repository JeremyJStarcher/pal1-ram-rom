#pragma once

uint16_t read_word(char *line);
uint8_t read_byte(char *line);
void start_ptp_load(void);
bool load_ptp_line(char *line, unsigned char *buffer);
