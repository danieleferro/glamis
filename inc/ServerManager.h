#ifndef __SERVER_MANAGER_HEADER
#define __SERVER_MANAGER_HEADER

#include "CommandInterface.h"
#include "EventManager.h"
#include "SerialParser.h"

#ifdef ARDUINO
#include <SoftwareSerial.h>
#include <Arduino.h>
#else
typedef unsigned char       byte;
#endif

#define ESP_TX_PIN             15
#define ESP_RX_PIN             16
#define ESP_RES_BUFFER        128

void ExternExcuteCommand(byte* buf, byte cmdLength)
{
    // ServerManager::ExecuteCommand(buf, cmdLength);
}
void ExternWriteCommand(byte buf)
{
    // ServerManager::WriteCommand(buf);
}

class ServerManager
{
public:
    ServerManager(EventManager * event_manager);
    ~ServerManager(void);

    bool StartESP8266(void);
    bool ServeESP8266Connection(void);

    void ExcuteCommand(byte* buf, byte cmdLength);
    void WriteCommand(byte buf);

private:
    EventManager * event_manager;
    SoftwareSerial esp8266;
    SerialParser parser;

    void clearSerialBuffer(void);
    void sendESP8266Command(const char * command, const int timeout);
    unsigned char receiveESP8266Data(void);


    void powerOnCommand(void);
    
    unsigned char esp8266_res_buffer[ESP_RES_BUFFER];
    unsigned char esp8266_res_buffer_index;
    unsigned char connection_id;
    
};


#endif // __SERVER_MANAGER_HEADER
