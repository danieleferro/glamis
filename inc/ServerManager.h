#ifndef __SERVER_MANAGER_HEADER
#define __SERVER_MANAGER_HEADER

#include "CommandInterface.h"
#include "Event.h"

#ifdef ARDUINO
#include <Arduino.h>
#else
typedef unsigned char       byte;
#endif

class ServerManager
{
public:
    ServerManager(EventManager * event_manager);
    ~ServerManager(void);

    bool ExcuteCommand(byte* buf, byte cmdLength);

private:
    EventManager * event_manager;
};

#endif // __SERVER_MANAGER_HEADER
