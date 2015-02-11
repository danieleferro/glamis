// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!

#ifndef _RTCLIB_H_
#define _RTCLIB_H_

#include "Time.h"

// RTC based on the DS1307 chip connected via I2C and the Wire library
enum Ds1307SqwPinMode 
{ 
    Ds1307_OFF = 0x00, 
    Ds1307_ON = 0x80, 
    SquareWave1HZ = 0x10, 
    SquareWave4kHz = 0x11, 
    SquareWave8kHz = 0x12, 
    SquareWave32kHz = 0x13
};

class RTC_DS1307 
{
public:
    static uint8_t begin(void);
    static void adjust(const time_t dt);
    uint8_t isrunning(void);
    static time_t now();
    static Ds1307SqwPinMode readSqwPinMode();
    static void writeSqwPinMode(Ds1307SqwPinMode mode);
    uint8_t readnvram(uint8_t address);
    void readnvram(uint8_t* buf, uint8_t size, uint8_t address);
    void writenvram(uint8_t address, uint8_t data);
    void writenvram(uint8_t address, uint8_t* buf, uint8_t size);
};

#endif // _RTCLIB_H_
