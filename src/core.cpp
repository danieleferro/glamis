#include "core.h"
#include <stdio.h>
#include <avr/pgmspace.h>

#define BUF_SIZE              64

static char buf[BUF_SIZE];

static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
			       "Sep", "Oct", "Nov", "Dec"};

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

time_t compile_time(void)
{ 
    char temp_date[] = __DATE__;
    char temp_time[] = __TIME__;
    unsigned char month, day, min, hour, sec;
    unsigned short year;
    unsigned char i;
    tmElements_t time_data = {0};

    // year
    year = atoi(temp_date + 7);
    // serial_printf_f(F("Year is %d"), year);
    time_data.Year = year - 1970;
    *(temp_date + 6) = 0;

    // day
    day = atoi(temp_date + 4);
    // serial_printf_f(F("Days is %d"), day);
    time_data.Day = day;
    *(temp_date + 3) = 0;
    
    // month
    month = 0;
    for (i = 0; i < 12; i++)
    {
	if (!strcmp(temp_date, months[i]))
	{
	    month = i + 1;
	    // serial_printf_f(F("Month is %d"), month);
	    break;
	}
    }
    time_data.Month = month;

    // serial_printf_f(F("%s"), temp_time);

    // second
    sec = atoi(temp_time + 6);
    // serial_printf_f(F("Second is %d"), sec);
    time_data.Second = sec;
    *(temp_time + 5) = 0;

    // minute
    min = atoi(temp_time + 3);
    // serial_printf_f(F("Minute is %d"), min);
    time_data.Minute = min;
    *(temp_time + 2) = 0;

    // hour
    hour = atoi(temp_time);
    // serial_printf_f(F("Hour is %d"), hour);
    time_data.Hour = hour;
  
    return makeTime(time_data);	
}
