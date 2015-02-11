#include "CentralManager.h"
#include "core.h"
#include "Arduino.h"

#define DEBUG                1

#ifdef DEBUG
#define dbg(fmt, args...)         serial_printf_f(F(fmt), ## args)
#else
#define dbg(fmt, args...)
#endif

CentralManager::CentralManager(unsigned int rele_pin, bool rele_mode) :
    rele(rele_pin, rele_mode)
{
}

CentralManager::~CentralManager(void)
{
}

void CentralManager::Init(void)
{
    day = (timeDayOfWeek_t) weekday();

    unsigned char i;
    for (i = 1; i < 8; i++)
    {
	event_manager.RestoreDay((timeDayOfWeek_t)(i));
    }
}

void CentralManager::SetBuffer(char * buff, unsigned char buff_size)
{
    this->buffer = buff;
    this->buffer_size = buff_size;

    return;
}

// read data and prepare response in the same buffer
unsigned char CentralManager::ProcessData(unsigned char data_len)
{
    unsigned char res_len = 0;
    time_t time;

    if (!CheckIntegrity(data_len))
    {
	// dbg("CheckIntegrity not passed");
	return 0;
    }

    switch (buffer[1])
    {

    case GET_EVENT_LIST:
	// next byte is time lapse
	if (buffer[2] < 0 || buffer[2] > 2)
	{
	    // error
	    dbg("Request list data error (%02x)", buffer[2]);
	    res_len = PrepareResponseKO();
	} 
	else
	{
	    time = previousMidnight(now()) + buffer[2]*SECS_PER_DAY;
	    res_len = PrepareResponseList(time);
	}
	break;

    case DEL_EVENT:
	// next byte is id
	if (event_manager.DelEvent(buffer[2]))
	{
	    res_len = PrepareResponseOK();	    
	}
	else
	{
	    res_len = PrepareResponseKO();
	}	
	break;

    case ADD_EVENT:

	time =  buffer[2];
	time = ((buffer[3] & 0x000000FF) << 8);

	if (buffer[4] >= ACTIONS_NUM)
	{
	    res_len = PrepareResponseKO();
	    break;
	}

	if(event_manager.AddEvent(time, (action_t) buffer[4]))
	{
	    res_len = PrepareResponseOK();	    
	}
	else
	{
	    res_len = PrepareResponseKO();
	}	

	break;

    case POWER_ON:
	// move rele'
	rele.Active();
	// response
	res_len = PrepareResponseOK();
	break;

    case POWER_OFF:
	// move rele'
	rele.Deactive();
	// response
	res_len = PrepareResponseOK();
	break;
	
    default:
	dbg("Command not recognized (0x2d)", buffer[1]);
	res_len = PrepareResponseKO();
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
	dbg("Data are less then 4 bytes");
	return false;
    }

    if (buffer[0] != INIT_BYTE || buffer[data_len-1] != END_BYTE)
    {
	dbg("Header (%02x) or tailer (%02x) not present", buffer[0], buffer[data_len-1]);
	return false;
    }

    for (i = 1; i < (data_len-2); i++)
    {
	sum += (unsigned char) buffer[i];
    }

    if (sum != (unsigned char) buffer[data_len-2])
    {
	dbg("Check sum not passed: calculated %02x, read %02x", sum, buffer[data_len-2]);
	return false;
    }

    // all ok
    return true;

}

unsigned char CentralManager::PrepareResponseOK(void)
{
    unsigned char i = 0;
    unsigned char j = 0;
    unsigned char sum = 0;

    // clean buffer
    memset(buffer, 0, buffer_size);

    // first byte
    buffer[i++] = INIT_BYTE;
    
    buffer[i++] = 'O';
    buffer[i++] = 'K';
    
    // check sum
    for (j = 1; j < i; j++)
    	sum += (unsigned char) buffer[j];
    
    buffer[i++] = (unsigned char) sum;

    // end byte
    buffer[i++] = END_BYTE;

    return i;
    
}

unsigned char CentralManager::PrepareResponseKO(void)
{
    unsigned char i = 0;
    unsigned char j = 0;
    unsigned char sum = 0;

    // clean buffer
    memset(buffer, 0, buffer_size);

    // first byte
    buffer[i++] = INIT_BYTE;
    
    buffer[i++] = 'K';
    buffer[i++] = 'O';
    
    // check sum
    for (j = 1; j < i; j++)
    	sum += (unsigned char) buffer[j];
    
    buffer[i++] = (unsigned char) sum;

    // end byte
    buffer[i++] = END_BYTE;

    return i;
    
}

unsigned char CentralManager::PrepareResponseList(time_t time)
{
    unsigned char i = 0;
    unsigned char j = 0;
    unsigned char sum = 0;
    unsigned char event_counter = 0;
    event_t * event = event_manager.GetRoot();
    time_t event_time;

    // clean buffer
    memset(buffer, 0, buffer_size);

    // first byte
    buffer[i++] = INIT_BYTE;
    
    while (event && event_counter < 16)
    {
	if (event->time <= time)
	{
	    event_time = event->time;
	    buffer[i++] = (event_time & 0x000000FF);
	    buffer[i++] = (event_time & 0x0000FF00) >> 8;
	    // buffer[i++] = (event_time & 0x00FF0000) >> 16;
	    // buffer[i++] = (event_time & 0xFF000000) >> 24;

	    buffer[i++] = event->id;
	    buffer[i++] = event->action;
	    
	    event_counter++;
	    event = event->next;
	}
	else {
	    // event are time ordered
	    break;
	}
    }
  
    // write number of event
    buffer[i++] = event_counter;

    // check sum
    for (j = 1; j < i; j++)
    	sum += (unsigned char) buffer[j];
    
    buffer[i++] = (unsigned char) sum;

    // end byte
    buffer[i++] = END_BYTE;

    return i;

}

void CentralManager::ProcessEvent(void)
{
    event_t * event;
    
    event = event_manager.GetRoot();

    while (event && event->time <= now())
    {
	// remove event from list
	switch (event->action)
	{
	case ON:	
	    dbg("ProcessEvent, action ON");
	    rele.Active();
	    break;
	case OFF:
	    dbg("ProcessEvent, action OFF");
	    rele.Deactive();
	    break;
	default:
	    dbg("ProcessEvent, invalid action %02d", event->action);
	}     

	event_manager.PopFirstEvent();
	event = event_manager.GetRoot();
    }

    return;
}

void CentralManager::ReloadEvents(void)
{
    unsigned char lapse;
    unsigned char i;

    if (day == (timeDayOfWeek_t) weekday())
	return;

    // add days
    lapse = (weekday() - day + 7) % 7;
    
    for (i = 0; i < lapse; i++)
    {
	dbg("ReloadEvent for day %s", dayStr((timeDayOfWeek_t)(day+i)));
	event_manager.RestoreDay((timeDayOfWeek_t)(day+i));
    }

    day = (timeDayOfWeek_t) weekday();

    return;
}
