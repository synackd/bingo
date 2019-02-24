#ifndef _LOG_HPP_
#define _LOG_HPP_

#include <stdio.h>

using namespace std;

void info(const char *fmt, ...);
void error(const char *fmt, ...);
void log(FILE *file, const char *fmt, ...);

#endif
