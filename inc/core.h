#ifndef CORE_HEADER
#define CORE_HEADER

#include <stdarg.h>
#include <Arduino.h>
#include "Time.h"

void serial_printf(char *fmt, ... );
void serial_printf_f(const __FlashStringHelper *fmt, ... );
time_t compile_time(void);

#endif // CORE_HEADER
