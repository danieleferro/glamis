#include "Relay.h"
#include "core.h"
#include <Arduino.h>

#define DEBUG                1

#ifdef DEBUG
#define dbg(fmt, args...)         serial_printf_f(F(fmt), ## args)
#else
#define dbg(fmt, args...)
#endif

Relay::Relay(unsigned int pin, bool mode_nc)
{
    this->pin = pin;
    this->mode_nc = mode_nc;
    pinMode(pin, OUTPUT);
}


Relay::~Relay(void)
{
}


void Relay::Active(void)
{
    if (mode_nc)
    {
	dbg("Activing PIN %d LOW", pin);
	digitalWrite(pin, LOW);
    }
    else    
    {
	dbg("Activing PIN %d HIGH", pin);
	digitalWrite(pin, HIGH);
    }
}

void Relay::Deactive(void)
{
    if (mode_nc)
    {
	dbg("Activing PIN %d HIGH", pin);
	digitalWrite(pin, HIGH);
    }
    else    
    {
	dbg("Activing PIN %d LOW", pin);
	digitalWrite(pin, LOW);
    }
}
