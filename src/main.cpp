#include <EEPROM.h>
#include <Arduino.h>
#include "core.h"

#define RELE_PIN            13
#define RELE_MODE_NC         1

void setup(void) 
{
    for (int i = 0; i < 512; i++)	
	EEPROM.write(i, 0);
    pinMode(RELE_PIN, OUTPUT);
    // setup wifi
}

void loop(void)
{
    // event manager
    // 1. check if there is event to process
    // 2. execute command
    // 3. reload events

    // network
    // 1. read data from wifi
    // 2. execute command
    // 3. write response

    // delay
    delay(100);
    
}
