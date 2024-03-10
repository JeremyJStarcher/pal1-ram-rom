#include <stdlib.h>
#include <stdio.h>

#include "commands.h"

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


void command_loop(uint16_t *rom_contents) {
    char input[10]; // Define the buffer size

    while(1) {
        printf("Enter a string: ");
        read_string(input, sizeof(input));
        printf("\nYou entered: %s\n", input);
    }
}

