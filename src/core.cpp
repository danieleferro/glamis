#include <stdarg.h>
#include "Arduino.h"

void printf(char *fmt, ... )
{
    // resulting string limited to 128 chars
    char buf[128];
    va_list args;
    va_start (args, fmt );
    vsnprintf(buf, 128, fmt, args);
    va_end (args);
    Serial.print(buf);
}
