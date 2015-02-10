#ifndef __CENTRAL_MANAGER_HEADER
#define __CENTRAL_MANAGER_HEADER

#define INIT_BYTE              0x5e
#define END_BYTE               0xa3

#include "CommandInterface.h"
#include "EventManager.h"
#include "Time.h"
#include "Relay.h"

class CentralManager
{

public:
    CentralManager(unsigned int rele_pin, bool rele_mode_nc);
    ~CentralManager(void);

    void SetBuffer(char * buff, unsigned char buff_size);
    unsigned char ProcessData(unsigned char data_len);
    void ProcessEvent(void);
    void ReloadEvents(void);

private:
    
    bool CheckIntegrity(unsigned char data_len);

    unsigned char PrepareResponseList(time_t time);
    unsigned char PrepareResponseOK(void);
    unsigned char PrepareResponseKO(void);

    char * buffer;
    unsigned char buffer_size;

    EventManager event_manager;
    Relay rele;
    timeDayOfWeek_t day;
};

#endif // __CENTRAL_MANAGER_HEADER
