#ifndef __CENTRAL_MANAGER_HEADER
#define __CENTRAL_MANAGER_HEADER

#define INIT_BYTE              0x5e
#define END_BYTE               0xa3

#include "CommandInterface.h"
#include "EventManager.h"
#include "Time.h"

class CentralManager
{

public:
    CentralManager(void);
    ~CentralManager(void);

    void SetBuffer(char * buff, unsigned char buff_size);
    unsigned char ProcessData(unsigned char data_len);

private:
    
    bool CheckIntegrity(unsigned char data_len);

    unsigned char PrepareResponseList(time_t time);


    unsigned char PrepareResponseOK(void);
    unsigned char PrepareResponseKO(void);

    char * buffer;
    unsigned char buffer_size;

    EventManager event_manager;
};

#endif // __CENTRAL_MANAGER_HEADER
