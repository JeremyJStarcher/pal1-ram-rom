#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <hardware/sync.h>
#include "hardware/timer.h"
#include <hardware/sync.h>
#include <hardware/flash.h>
#include "hardware/clocks.h"


#include "rambuf.h"
#include "commands.h"

unsigned long _xip_base;
unsigned long _flash_size;

/* Yes I know global variables are evil, but in this case speed trumps
   every other concern.  This check needs done FAST.
*/
int flash_save_index = -1;

bool load_ptp_error = false;
bool load_ptp_done = false;


uint16_t parse_word_from_string(char *line) {
    char temp_s[5];
    temp_s[0] = line[0];
    temp_s[1] = line[1];
    temp_s[2] = line[2];
    temp_s[3] = line[3];
    temp_s[4] = 0;

    return (uint16_t) strtol(temp_s, NULL, 16);
}

uint8_t parse_byte_from_string(char *line) {
    char temp_s[5];

    temp_s[0] = line[0];
    temp_s[1] = line[1];
    temp_s[2] = 0;

    return (uint8_t) strtol(temp_s, NULL, 16);
}



/*** */ 



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
    while(line[idx] != ';') {
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

    //skip the count
    idx += 2;

    addr = parse_word_from_string(&line[idx]);
    checksum += (int) addr / 256;
    checksum += (int) addr % 256;

    // And skip the address
    idx += 4;

    while(count > 0) {
        data = parse_byte_from_string(&line[idx]);
        checksum += data;
        idx += 2;
        count--;

        set_ram_byte(addr, data);
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
    int count = 0; // Number of characters currently in the buffer
    char ch; // Character read from stdin

    while (1) {
        ch = getchar(); // Read a character

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
                putchar(ch); // Echo the character
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
    printf("RESET: RESET RAM/ROM TO POWERUP STATE)\n");
    printf("SAVE #: SAVE TO A SLOT IN FLASH MEMORY)\n");
    printf("LOAD #: LOAD FROM A SLOT IN FLASH MEMORY)\n");
}


void fill_rom_msb() {
    uint16_t addr;
    uint8_t data;
    for(addr = 0x0000; addr < 0xFFFF; addr+=1) {
        data = addr / 256;
        set_rom_byte(addr, data);
    }
}

void fill_rom_lsb() {
    uint16_t addr;
    uint8_t data;
    for(addr = 0x0000; addr < 0xFFFF; addr+=1) {
        data = addr % 256;
        set_rom_byte(addr, data);
    }
}

void loadptp_command() {
    char input[200]; // Define the buffer size

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

void load_slot_command(int index) {
    size_t size = sizeof sys_state;
    unsigned long offset = /*_xip_base +*/ (_flash_size/2) + (index * size);
    const uint8_t *flash_target_contents = (const uint8_t *) (XIP_BASE + offset);

    memcpy(&sys_state, flash_target_contents, size);
}

void save_slot_command(int index) {
    if (index > 10) {
        printf("ONLY SLOTS 0-9 MAY BE USED\n");
        return;
    }


//       flash size: 00200000                                           
// SAVE SLOT: 1 OFFSET 1deffa 

    size_t size = sizeof sys_state;
    unsigned long offset = /*_xip_base +*/ (_flash_size/2) + (index * size);

    printf("flash size: %08x\n", _flash_size);
    printf("SAVE SLOT: %d OFFSET %04x  size: %08x\n", index, offset, size);
    if (offset % 4096 == 0) {
        printf("ON THE BOUNDARY\n");
    } else {
        printf("OOPS\n");
    }

    size_t idx = offset;
    uint32_t ints;

    int cnt = 0;

   for(idx = offset; idx < offset + size; idx += FLASH_SECTOR_SIZE) {
        //ints = save_and_disable_interrupts();
        ints = save_and_disable_interrupts();
        flash_range_erase (idx, size );
        restore_interrupts (ints);

        //restore_interrupts (ints);
        cnt += 1;
    }

    printf("Flashing..... ");
    ints = save_and_disable_interrupts();
    flash_range_program (offset, &sys_state, size);
    restore_interrupts (ints);

    printf("DONE \n");
}
 


void command_loop(unsigned long xip_base, unsigned long flash_size) {
    char input[20]; // Define the buffer size

    _xip_base = xip_base;
    _flash_size = flash_size;

    while(1) {
        printf("\nENTER COMMAND (OR HELP): ");
        read_string(input, sizeof(input));
        //printf("\nYou entered: %s\n", input);

        char *command = strtok(input, " "); // Get the first word as the command
        if (command == NULL) {
            continue; // If no command is entered, skip to the next iteration
        }

        printf("\n");

        // Compare input with known commands and call the corresponding function
        if (strcmp(command, "HELP") == 0) {
            help_command();
        } else if (strcmp(command, "L") == 0) {
            loadptp_command();
        } else if (strcmp(command, "LSB") == 0) {
            fill_rom_lsb();
        } else if (strcmp(command, "MSB") == 0) {
            fill_rom_msb();
        } else if (strcmp(command, "RESET") == 0) {
            setup_memory_contents();

        } else if (strcmp(command, "SAVE") == 0) {
            char *param_str = strtok(NULL, " "); // Try to get the next word as a parameter
            int index = -1; // Default index value if no parameter is provided
            if (param_str != NULL) {
                // index = atoi(param_str); // Convert parameter string to integer
                index = (uint8_t) strtol(param_str, NULL, 16);
                flash_save_index = index;

                printf("!!!!!!\n");

                while(flash_save_index != -1) {
                    // just kill time
                }

                printf("ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ");

            } else {
               printf("MUST SPECIFY AN INDEX NUMBER\n");
            }

        } else if (strcmp(command, "LOAD") == 0) {
            char *param_str = strtok(NULL, " "); // Try to get the next word as a parameter
            int index = -1; // Default index value if no parameter is provided
            if (param_str != NULL) {
                // index = atoi(param_str); // Convert parameter string to integer
                index = (uint8_t) strtol(param_str, NULL, 16);
                load_slot_command(index);

            } else {
               printf("MUST SPECIFY AN INDEX NUMBER\n");
            }


        } else if (strcmp(command, "") == 0) {
            // ignore it.
        } else {
            printf("HUH?\n");
        }
    }
}


void print_binary(unsigned int value, int bits) {
    unsigned int mask = 1 << (bits - 1); // Create a mask for the most significant bit

    for (int i = 0; i < bits; i++) {
        putchar((value & mask) ? '1' : '0'); // Check if the current bit is set
        value <<= 1; // Shift the value left to check the next bit
    }
}
