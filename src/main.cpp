#include <EEPROM.h>
#include <Arduino.h>
#include "core.h"

void setup(void) 
{
    for (int i = 0; i < 512; i++)	
	EEPROM.write(i, 0);
    pinMode(13, OUTPUT);

    setup_Mirf();
}

void loop(void)
{
    digitalWrite(13,HIGH);
    delay(1000);
    digitalWrite(13,LOW);
    delay(1000);
    printf("Hello World");

    loop_Mirf();
}
