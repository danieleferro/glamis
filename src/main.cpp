#include <EEPROM.h>
#include <Arduino.h>
#include <Wire.h>
#include "RTClib.h"
#include "Time.h"
#include "core.h"
#include "UartWifi.h"
#include "CentralManager.h"
#include "Relay.h"

#define DEBUG                   1
#ifdef DEBUG
#define dbg(fmt, args...)         serial_printf_f(F(fmt), ## args)
#else
#define dbg(fmt, args...)
#endif

RTC_DS1307 rtc;

#define LED_PIN                13
bool led_on = true;

#define RELE_PIN               2
#define ESP_TX_PIN             3
#define ESP_RX_PIN             4
#define ESP_BAUD               115200


#define WIFI_SSID              "AUCT"
#define WIFI_PASSWORD          "inserimentoforzato"
#define WIFI_CHANNEL           1
// UartWifi * wifi;
UartWifi wifi(ESP_RX_PIN, ESP_TX_PIN);


#define BUFFER_SIZE            128
char buffer[BUFFER_SIZE];
// CentralManager * manager;
CentralManager manager(RELE_PIN, RELE_MODE_NC);

void setup(void) 
{
    bool res;
    String ipstring;

    // -- serial Debug
    Serial.begin(115200);

    // -- LED
    pinMode(LED_PIN, OUTPUT);

    // -- TIME

    Wire.begin();
    rtc.begin();
    
    if (! rtc.isrunning()) {
	Serial.println("RTC is NOT running!");
	// following line sets the RTC to the date & time this sketch was compiled
	rtc.adjust(compile_time());
    }

    setTime(compile_time());

    // -- EEPROM
    /*
    for (int i = 0; i < 1024; i++)	
	EEPROM.write(i, 0);
    */

    // -- WIFI
    wifi.Begin(ESP_BAUD);
    res = wifi.Initialize(STA, WIFI_SSID, WIFI_PASSWORD,
			  WIFI_CHANNEL, ENCR_WAP_PSK);

    if (!res)
    {
	dbg("Wifi init error");
    }
    else
    {
	// make sure the module can have enough time to get an IP address
	delay(4000); 
	ipstring = wifi.showIP();
	Serial.println(ipstring);
	
	delay(5000);
	wifi.confMux(1);
	delay(100);
	if (wifi.confServer(OPEN, 8080))
	    dbg("Server is set up");
	else
	    dbg("Server is NOT up");
    }
    // -- MANAGER
    manager.Init();
    manager.SetBuffer(buffer, BUFFER_SIZE);

}

void loop(void)
{
    int iLen, oLen;
    unsigned char chlID;
    time_t time_now;
 
    // event manager
    // 1. check and process event
    manager.ProcessEvent();

    // 2. reload events
    manager.ReloadEvents();

    // network
    // 1. read data from wifi
    iLen = wifi.ReceiveMessage(buffer, BUFFER_SIZE);
    chlID = wifi.GetChlID();
    if (iLen <= 0)
    {
	goto end;
    }

    Serial.print("Get a message from id ");
    Serial.print(chlID);
    Serial.println(":");
    Serial.println(buffer); 
    
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

end:
    // 4. led blink
    digitalWrite(LED_PIN, led_on);

    led_on = !led_on;

    // 5. others
    time_now = now();
    dbg("Time now: %02d.%02d.%04d-%02d:%02d:%02d",
	day(time_now), month(time_now), year(time_now),
	hour(time_now), minute(time_now), second(time_now));

    setTime(time_now + SECS_PER_HOUR);

    // delay
    delay(1000);    
}
