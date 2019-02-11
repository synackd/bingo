#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdarg.h>
#include "colors.hpp"

// Add this if GCC/Clang:
//   __attribute__((format(printf, 3, 4)))
// to get format string validation at compile time.
void cprintf(FILE * stream, const char *color, const char * fmt, ...)
{
    va_list vaList;
    assert(stream != NULL);

    // If printing to a file, don't clutter the output with
    // control characters.
    if (!isatty(fileno(stream)))
    {
        va_start(vaList, fmt);
        vfprintf(stream, fmt, vaList);
        va_end(vaList);
        return;
    }

    // Print to console with color:
    fprintf(stream, "%s", color);
    va_start(vaList, fmt);
    vfprintf(stream, fmt, vaList);
    va_end(vaList);
    fprintf(stream, "%s", RESET);
}

int main (int argc, char const *argv[]) {
    cprintf(stdout, BOLD, "BOLD\n");
    cprintf(stdout, ITALIC, "ITALIC\n");
    cprintf(stdout, UNDERLINE, "UNDERLINE\n");
    cprintf(stdout, STRIKETHROUGH, "STRIKETHROUGH\n");
    cprintf(stdout, REVERSED, "REVERSED\n");

    return 0;
}
