// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!

#include <Wire.h>
#include "RTClib.h"
#include <avr/pgmspace.h>
#define WIRE Wire

#define DS1307_ADDRESS  0x68
#define DS1307_CONTROL  0x07
#define DS1307_NVRAM    0x08

#include <Arduino.h>
#define _I2C_WRITE write
#define _I2C_READ  read

static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }

uint8_t RTC_DS1307::begin(void)
{
    return 1;
}

uint8_t RTC_DS1307::isrunning(void)
{
    WIRE.beginTransmission(DS1307_ADDRESS);
    WIRE._I2C_WRITE(0);
    WIRE.endTransmission();
    
    WIRE.requestFrom(DS1307_ADDRESS, 1);
    uint8_t ss = WIRE._I2C_READ();
    return !(ss>>7);
}

void RTC_DS1307::adjust(time_t dt) 
{
    WIRE.beginTransmission(DS1307_ADDRESS);
    WIRE._I2C_WRITE(0);
    WIRE._I2C_WRITE(bin2bcd(second(dt)));
    WIRE._I2C_WRITE(bin2bcd(minute(dt)));
    WIRE._I2C_WRITE(bin2bcd(hour(dt)));
    WIRE._I2C_WRITE(bin2bcd(0));
    WIRE._I2C_WRITE(bin2bcd(day(dt)));
    WIRE._I2C_WRITE(bin2bcd(month(dt)));
    WIRE._I2C_WRITE(bin2bcd(year(dt) - 2000));
    WIRE._I2C_WRITE(0);
    WIRE.endTransmission();
}

time_t RTC_DS1307::now(void) 
{
    tmElements_t tm;

    WIRE.beginTransmission(DS1307_ADDRESS);
    WIRE._I2C_WRITE(0);	
    WIRE.endTransmission();

    WIRE.requestFrom(DS1307_ADDRESS, 7);
    tm.Second = bcd2bin(WIRE._I2C_READ() & 0x7F);
    tm.Minute = bcd2bin(WIRE._I2C_READ());
    tm.Hour = bcd2bin(WIRE._I2C_READ());
    WIRE._I2C_READ();
    tm.Day = bcd2bin(WIRE._I2C_READ());
    tm.Month = bcd2bin(WIRE._I2C_READ());
    tm.Year = bcd2bin(WIRE._I2C_READ()) + 2000;

    return makeTime(tm);
}

Ds1307SqwPinMode RTC_DS1307::readSqwPinMode(void) 
{
    int mode;
    
    WIRE.beginTransmission(DS1307_ADDRESS);
    WIRE._I2C_WRITE(DS1307_CONTROL);
    WIRE.endTransmission();
    
    WIRE.requestFrom((uint8_t)DS1307_ADDRESS, (uint8_t)1);
    mode = WIRE._I2C_READ();
    
    mode &= 0x93;
    return static_cast<Ds1307SqwPinMode>(mode);
}

void RTC_DS1307::writeSqwPinMode(Ds1307SqwPinMode mode)
{
    WIRE.beginTransmission(DS1307_ADDRESS);
    WIRE._I2C_WRITE(DS1307_CONTROL);
    WIRE._I2C_WRITE(mode);
    WIRE.endTransmission();
}

void RTC_DS1307::readnvram(uint8_t* buf, uint8_t size, uint8_t address)
{
    int addrByte = DS1307_NVRAM + address;
    WIRE.beginTransmission(DS1307_ADDRESS);
    WIRE._I2C_WRITE(addrByte);
    WIRE.endTransmission();
    
    WIRE.requestFrom((uint8_t) DS1307_ADDRESS, size);
    for (uint8_t pos = 0; pos < size; ++pos) {
	buf[pos] = WIRE._I2C_READ();
    }
}

void RTC_DS1307::writenvram(uint8_t address, uint8_t* buf, uint8_t size)
{
    int addrByte = DS1307_NVRAM + address;
    WIRE.beginTransmission(DS1307_ADDRESS);
    WIRE._I2C_WRITE(addrByte);
    for (uint8_t pos = 0; pos < size; ++pos) {
	WIRE._I2C_WRITE(buf[pos]);
    }
    WIRE.endTransmission();
}

uint8_t RTC_DS1307::readnvram(uint8_t address)
{
    uint8_t data;
    readnvram(&data, 1, address);
    return data;
}

void RTC_DS1307::writenvram(uint8_t address, uint8_t data)
{
    writenvram(address, &data, 1);
}
