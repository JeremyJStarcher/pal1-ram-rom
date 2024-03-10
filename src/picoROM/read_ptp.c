#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "read_ptp.h"

bool load_ptp_error = false;
bool load_ptp_done = false;


uint16_t read_word(char *line) {
    char temp_s[5];
    temp_s[0] = line[0];
    temp_s[1] = line[1];
    temp_s[2] = line[2];
    temp_s[3] = line[3];
    temp_s[4] = 0;

    return (uint16_t) strtol(temp_s, NULL, 16);
}

uint8_t read_byte(char *line) {
    char temp_s[5];

    temp_s[0] = line[0];
    temp_s[1] = line[1];
    temp_s[2] = 0;

    return (uint8_t) strtol(temp_s, NULL, 16);
}

void start_ptp_load(void) {
    load_ptp_error = false;
    load_ptp_done = false;
}

bool load_ptp_line(char *line, unsigned char *buffer) {
    int idx = 0;
    uint16_t addr;
    int count;
    uint8_t data;

    uint16_t checksum = 0;
    uint16_t given_checksum = 0;


    // Search until we find the starting ';'
    while(line[idx] != ';') {
        idx += 1;
    }

    // Skip the ';'
    idx += 1;

    // Convert hex string to number
    count = read_byte(&line[idx]);
    if (count == 0) {
        load_ptp_done = true;
        return true;
    }

    checksum += count;

    //skip the count
    idx += 2;

    addr = read_word(&line[idx]);
    checksum += (int) addr / 256;
    checksum += (int) addr % 256;

    // And skip the address
    idx += 4;

    while(count > 0) {
        data = read_byte(&line[idx]);
        checksum += data;
        idx += 2;
        count--;

        buffer[addr] = data;
        addr += 1;
    }

    given_checksum = read_word(&line[idx]);

    if (given_checksum == checksum) {
        return true;
    }

    load_ptp_error = true;
    return false;
}


#define MAX_LINE_LENGTH 1024 // Define the maximum line length


// void main() {
//     char line[MAX_LINE_LENGTH];
//     FILE *file;

//     printf("Hello world\n");

//     unsigned char buffer[0xFFFF] = {};

//     start_ptp_load();


//     file = fopen("../../basic.pap", "r");
//     if (file == NULL) {
//         perror("Error opening file");
//         // return 1;
//     }

//     while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
//         line[strcspn(line, "\r\n")] = '\0';
//         load_ptp_line(line, buffer);
//     }

//     // Close the file
//     fclose(file);


//     if (load_ptp_error) {
//         printf("Errors were found");
//     }
//     if (load_ptp_done) {
//         printf("Done");
//     }


//     // char str[] = "zzzzzzz;182000E9260726C52B44288C2A9E2EB82AF128F127C9277428CA2609E1\r\n";
//     // load_ptp_line(str, buffer);
// }