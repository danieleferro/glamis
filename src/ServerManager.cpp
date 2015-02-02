#include "ServerManager.h"

ServerManager::ServerManager(EventManager * event_manager)
{
    this->event_manager = event_manager;
}

ServerManager::~ServerManager(void)
{

}

bool ServerManager::ExcuteCommand(byte* buf, byte cmdLength)
{
    bool res = false;
    
    if (!cmdLength)
	return false;
 
    // first byte is command
    switch (buf[0])
    {

    case GET_EVENT_LIST:
	// redirect 
	break;
    case DEL_EVENT:
	break;
    case ADD_EVENT:
	break;
    case POWER_ON:
	break;
    case POWER_OFF:
	break;

    default:
	return false;
    }
    

    return res;
}
