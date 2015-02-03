#ifndef __CENTRAL_MANAGER_HEADER
#define __CENTRAL_MANAGER_HEADER

#define INIT_BYTE              0x5e
#define END_BYTE               0xa3

#include "CommandInterface.h"

class CentralManager
{

public:
    CentralManager(void);
    ~CentralManager(void);

    void SetBuffer(char * buff, unsigned char buff_size);
    unsigned char ProcessData(unsigned char data_len);

private:
    
    bool CheckIntegrity(unsigned char data_len);
    unsigned char PrepareResponse(command_t command);


    char * buffer;
    unsigned char buffer_size;
};

#endif // __CENTRAL_MANAGER_HEADER
