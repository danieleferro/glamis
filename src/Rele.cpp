#include "Rele.h"
#include <Arduino.h>

#define DEBUG                1

#ifdef DEBUG
#define dbg(fmt, args...)         printf("Rele: "fmt, ## args)
#else
#define dbg(fmt, args...)
#endif

Rele::Rele(unsigned int pin, bool mode_nc)
{
    this->pin = pin;
    this->mode_nc = mode_nc;
    pinMode(pin, OUTPUT);
}


Rele::~Rele(void)
{
}


void Rele::Active(void)
{
    if (mode_nc)
    {
	dbg("Activing PIN %d LOW\n", pin);
	digitalWrite(pin, LOW);
    }
    else    
    {
	dbg("Activing PIN %d HIGH\n", pin);
	digitalWrite(pin, HIGH);
    }
}

void Rele::Deactive(void)
{
    if (mode_nc)
    {
	dbg("Activing PIN %d HIGH\n", pin);
	digitalWrite(pin, HIGH);
    }
    else    
    {
	dbg("Activing PIN %d LOW\n", pin);
	digitalWrite(pin, LOW);
    }
}
