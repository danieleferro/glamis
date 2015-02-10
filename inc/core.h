#ifndef CORE_HEADER
#define CORE_HEADER

#include <stdarg.h>
#include <Arduino.h>

void serial_printf(char *fmt, ... );
void serial_printf_f(const __FlashStringHelper *fmt, ... );

#endif // CORE_HEADER
