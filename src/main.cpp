#include <EEPROM.h>
#include <Arduino.h>
#include "core.h"
#include "UartWifi.h"

#define RELE_PIN            13
#define RELE_MODE_NC         1


UartWifi wifi;
#define WIFI_SSID       "AUCT"
#define WIFI_PASSWORD   "inserimentoforzato"
#define WIFI_CHANNEL    1

void setup(void) 
{
    bool res;
    String ipstring;

    // -- EEPROM
    for (int i = 0; i < 512; i++)	
	EEPROM.write(i, 0);

    // -- GPIO
    pinMode(RELE_PIN, OUTPUT);

    // -- WIFI
    wifi.Begin();
    res = wifi.Initialize(STA, WIFI_SSID, WIFI_PASSWORD,
			  WIFI_CHANNEL, ENCR_WAP_PSK);

    if(!res)
    {
	Serial.println("Main, wifi init error");
    }

    // make sure the module can have enough time to get an IP address
    delay(4000); 
    ipstring = wifi.showIP();
    Serial.println(ipstring);
    
    delay(5000);
    wifi.confMux(1);
    delay(100);
    if (wifi.confServer(OPEN, 8080))
	Serial.println("Server is set up");
	
}

void loop(void)
{
    char buf[128];
    int iLen;
    unsigned char chlID;

    // event manager
    // 1. check if there is event to process
    // TODO
    // 2. execute command
    // TODO
    // 3. reload events
    // TODO

    // network
    // 1. read data from wifi
    iLen = wifi.ReceiveMessage(buf, 128);
    chlID = wifi.GetChlID();
    if (iLen > 0)
    {
	Serial.print("Get a message from id ");
	Serial.print(chlID);
	Serial.println(":");
	Serial.println(buf); 
    }   
    
    // 2. parse data, execute command, prepare response
    // TODO
    // 3. write response
    Serial.print("Send a message back to id ");
    Serial.print(chlID);
    Serial.println(":");
    Serial.println("HELLO BACK");      
    wifi.Send(chlID, "HELLO BACK");

    // delay
    delay(100);
    
}
