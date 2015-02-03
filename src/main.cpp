#include <EEPROM.h>
#include <Arduino.h>
#include "core.h"
#include "UartWifi.h"
#include "CentralManager.h"
#include "Rele.h"

#define RELE_PIN               13
#define ESP_TX_PIN             15
#define ESP_RX_PIN             16
#define ESP_BAUD               115200


#define WIFI_SSID              "AUCT"
#define WIFI_PASSWORD          "inserimentoforzato"
#define WIFI_CHANNEL           1
UartWifi wifi(ESP_RX_PIN, ESP_TX_PIN);


#define BUFFER_SIZE        128
char buffer[BUFFER_SIZE];
CentralManager manager(RELE_PIN, RELE_MODE_NC);


void setup(void) 
{
    bool res;
    String ipstring;

    // -- EEPROM
    for (int i = 0; i < 512; i++)	
	EEPROM.write(i, 0);

    // -- WIFI
    wifi.Begin(ESP_BAUD);
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

    // -- MANAGER
    manager.SetBuffer(buffer, BUFFER_SIZE);
    
}

void loop(void)
{
    int iLen, oLen;
    unsigned char chlID;
    
    // event manager
    // 1. check and process event
    manager.ProcessEvent();

    // 2. reload events
    manager.ReloadEvents();

    // network
    // 1. read data from wifi
    iLen = wifi.ReceiveMessage(buffer, BUFFER_SIZE);
    chlID = wifi.GetChlID();
    if (iLen > 0)
    {
	Serial.print("Get a message from id ");
	Serial.print(chlID);
	Serial.println(":");
	Serial.println(buffer); 
    }   
    
    // 2. parse data, execute command, prepare response
    oLen = manager.ProcessData(iLen);

    // 3. write response
    if (oLen > 0)
    {
	Serial.print("Send a message back to id ");
	Serial.print(chlID);
	Serial.println(":");
	Serial.println(buffer);      
	wifi.Send(chlID, buffer);
    }
    wifi.closeMux(chlID);
    
    // delay
    delay(1000);    
}
