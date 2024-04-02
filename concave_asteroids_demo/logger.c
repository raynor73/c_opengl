#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

void error(const char *format, ...) {
	va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);

	exit(-1);
}

void log_error(const char *format, ...) {
	va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
}

#ifdef DEBUG
void log_debug(const char *format, ...) {
	va_list argptr;
    va_start(argptr, format);
    vfprintf(stdout, format, argptr);
    va_end(argptr);
}
#endif
