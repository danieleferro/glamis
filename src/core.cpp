#include "core.h"
#include <stdio.h>

#define BUF_SIZE              64
void serial_printf(char *fmt, ... )
{
    // resulting string limited to BUF_SIZE chars
    char buf[BUF_SIZE];
    va_list args;
    va_start (args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end (args);
    Serial.println(buf);
}

void serial_printf_f(const __FlashStringHelper *fmt, ... )
{
     // resulting string limited to BUF_SIZE chars
    char buf[BUF_SIZE];
    va_list args;
    va_start (args, fmt);
#ifdef __AVR__
     // progmem for AVR
    vsnprintf_P(buf, sizeof(buf), (const char *)fmt, args);
#else
     // for the rest of the world
    vsnprintf(buf, sizeof(buf), (const char *)fmt, args);
#endif
    va_end(args);
    Serial.println(buf);
}

