#ifndef ILAPIN_LOGGER_H
#define ILAPIN_LOGGER_H

void error(const char *format, ...);
void log_error(const char *format, ...);

#ifdef DEBUG
void log_debug(const char *format, ...);
#else
#define log_debug(format, ...)
#endif

#endif
