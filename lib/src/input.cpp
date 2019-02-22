#include <stdio.h>
#include <stdlib.h>
#include "constants.hpp"
#include "cmd.hpp"
#include "colors.hpp"
#include "input.hpp"

using namespace std;

/**
 * Get input from the user and return a string
 */
char *read_line()
{
    int bufsize = INPUT_BUFSIZE;
    int position = 0;
    char *buffer = (char*) malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        cprintf(stderr, BOLD, "[SYS] ");
        fprintf(stderr, "Could not allocate memory for input!\n");
        exit(FAILURE);
    }

    while (1) {
        // Read a character
        c = getchar();

        // If we hit EOF, replace it with a null character and return.
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        // If we have exceeded the buffer, reallocate.
        if (position >= bufsize) {
            bufsize += INPUT_BUFSIZE;
            buffer = (char*) realloc(buffer, bufsize);
            if (!buffer) {
                cprintf(stderr, BOLD, "[SYS] ");
                fprintf(stderr, "Could not allocate memory for input!\n");
                exit(FAILURE);
          }
        }
    }
}
