#include <stdio.h>
#include <stdarg.h>
#include "colors.hpp"
#include "log.hpp"

using namespace std;

/**
 * Log an info message to stdout
 * from the manager.
 */
void info(const char *fmt, ...)
{
    va_list vaList;
    cprintf(stdout, BOLD, "[PLR] ");
    va_start(vaList, fmt);
    vfprintf(stdout, fmt, vaList);
    va_end(vaList);
    fprintf(stdout, "\n");
}

/**
 * Log an error message to stderr
 * from the manager.
 */
void error(const char *fmt, ...)
{
    va_list vaList;
    cprintf(stderr, BOLD, "[PLR][ERR] ");
    va_start(vaList, fmt);
    vfprintf(stderr, fmt, vaList);
    va_end(vaList);
    fprintf(stderr, "\n");
}

/**
 * Log a message to a log file
 */
void log(FILE *file, const char *fmt, ...)
{
    va_list vaList;
    fprintf(file, "[PLR] ");
    va_start(vaList, fmt);
    vfprintf(file, fmt, vaList);
    va_end(vaList);
    fprintf(file, "\n");
}
