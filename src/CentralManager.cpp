#include "CentralManager.h"
#include "core.h"
#include "Arduino.h"

#define DEBUG                1

#ifdef DEBUG
#define dbg(fmt, args...)         printf("CentralManager: "fmt, ## args)
#else
#define dbg(fmt, args...)
#endif

CentralManager::CentralManager(void)
{
}

CentralManager::~CentralManager(void)
{
}

void CentralManager::SetBuffer(char * buff, unsigned char buff_size)
{
    this->buffer = buff;
    this->buffer_size = buff_size;    
}

// read data and prepare response in the same buffer
unsigned char CentralManager::ProcessData(unsigned char data_len)
{
    unsigned char res_len = 0;

    if (!CheckIntegrity(data_len))
    {
	dbg("CheckIntegrity not passed\n");
	return 0;
    }

    switch (buffer[1])
    {
    case GET_EVENT_LIST:
	break;
    case DEL_EVENT:
	break;
    case ADD_EVENT:
	break;
    case POWER_ON:
	// move rele'
	// response
	res_len = PrepareResponse(POWER_ON);
	break;
    case POWER_OFF:
	// move rele'
	// response
	res_len = PrepareResponse(POWER_OFF);
	break;
	
    default:
	dbg("Command not recognized (0x2d)\n", buffer[1]);
	res_len = 0;
    }
    

    return res_len;
}

bool CentralManager::CheckIntegrity(unsigned char data_len)
{
    unsigned char i;
    unsigned char sum = 0;

    if (data_len < 4)
    {
	// header, tailer, data, check sum not present
	dbg("Data are less then 4 bytes\n");
	return false;
    }

    if (buffer[0] != INIT_BYTE || buffer[data_len-1] != END_BYTE)
    {
	dbg("Header (%02x) or tailer (%02x) not present\n", buffer[0], buffer[data_len-1]);
	return false;
    }

    for (i = 1; i < (data_len-2); i++)
    {
	sum += (unsigned char) buffer[i];
    }

    if (sum != (unsigned char) buffer[data_len-2])
    {
	dbg("Check sum not passed: calculated %02x, read %02x\n", sum, buffer[data_len-2]);
	return false;
    }

    // all ok
    return true;

}

unsigned char CentralManager::PrepareResponse(command_t command)
{
    unsigned char i = 0;
    unsigned char j = 0;
    unsigned char sum = 0;

    // clean buffer
    memset(buffer, 0, buffer_size);

    // first byte
    buffer[i++] = INIT_BYTE;
    
    switch (command)
    {
    case GET_EVENT_LIST:
	break;
    case DEL_EVENT:
	break;
    case ADD_EVENT:
	break;

    case POWER_ON:
    case POWER_OFF:
	buffer[i++] = 'O';
	buffer[i++] = 'K';
	break;
	
    default:
	dbg("Command not recognized (0x2d)\n", buffer[1]);
	buffer[i++] = 'K';
	buffer[i++] = 'O';

    }
    
    // check sum
    for (j = 1; j < i; j++)
    	sum += (unsigned char) buffer[j];
    
    buffer[i++] = (unsigned char) sum;

    // end byte
    buffer[i++] = END_BYTE;

    return i;
    
}
