#include "EventManager.h"
#include <Arduino.h>
#include "core.h"

#define MAX_EVENT_LIST                 255

#define DEBUG                1

#ifdef DEBUG
#define dbg(fmt, args...)         serial_printf_f(F(fmt), ## args)
#else
#define dbg(fmt, args...)
#endif

const fixedEvent_t programEvent[] PROGMEM =
{
    {dowSunday, 10, 30, ON},
    {dowSunday, 12, 30, OFF},
    {dowMonday, 9, 10, ON},
    {dowMonday, 18, 30, OFF},
    {dowThursday, 11, 43, ON},
    {dowThursday, 16, 30, OFF},
    {dowFriday, 6, 30, ON},
    {dowFriday, 10, 30, OFF},
    {dowSaturday, 13, 30, ON},
    {dowSaturday, 17, 30, OFF},
};

// -------- EventManager
unsigned char EventManager::event_counter = 0;

EventManager::EventManager(void)
{
    unsigned char i;

    this->first = NULL;

    for (i = 1; i < 8; i++)
    {
	RestoreDay((timeDayOfWeek_t)(i));
    }
}

EventManager::~EventManager(void)
{
    DelAllEvent();
}

unsigned int EventManager::EventNumber(void)
{
    unsigned int count = 0;
    event_t * ptr = first;
    
    while (ptr)
    {		
	/*
	dbg("CounterEvent %02d, %02d.%02d.%04d-%02d:%02d:%02d",
	    ptr->id, day(ptr->time), month(ptr->time), year(ptr->time),
	    hour(ptr->time), minute(ptr->time), second(ptr->time));
	*/
	count++;
	ptr = ptr->next;
    }
  
    return count;
}

bool EventManager::AddEvent(time_t time, action_t action)
{
    event_t * current;
    event_t * event;

    // check max size
    if (EventNumber() >= MAX_EVENT_LIST)
    {
	dbg("EventManager max event reached");
	return false;
    }

    event = (event_t *) malloc(sizeof(event_t)); 
    
    if (!event)
    {
	dbg("EventManager event not created");
	return false;
    }
    
    memset(event, 0, sizeof(event_t));
    event->id = (++event_counter) % 1000;


    // Special case for the head end
    if (first == NULL || first->time >= event->time)
    {
        event->next = first;
        first = event;
    }
    else
    {
        // Locate the node before the point of insertion
        current = first;
        while (current->next != NULL &&
               current->next->time < event->time)
        {
            current = current->next;
        }
        event->next = current->next;
        current->next = event;
    }
    
    if (event->next)
    {
	dbg("Event %02d added before event %02d",
	    event->id, event->next->id);
    }
    else
    {
	dbg("Event %02d added as first",
	    event->id);       
    }

    return true;
}
 
bool EventManager::DelEvent(unsigned char id)
{
    bool res;
    event_t * ptr_pre = NULL;
    event_t * ptr_del = NULL;
    

    // Check whether it is the head node
    if (first->id == id)
    {
        // point to the node to be deleted
        ptr_del = first;
        // update
        first = ptr_del->next;
        delete ptr_del;
        res = true;
	goto end;
    }
    
    ptr_pre = first;
    ptr_del = first->next;
    
    // traverse the list and check the value of each node
    while (ptr_del != NULL)
    {

        if (ptr_del->id == id)
	{
            // Update the list
            ptr_pre->next = ptr_del->next;
            delete ptr_del;
            res = true;
	    goto end;
        }
	
        ptr_pre = ptr_del;
        ptr_del = ptr_del->next;
    }

    res = false;

end:
    if (res)
	dbg("Event %02d removed", id);
    else
	dbg("Event %02d NOT removed", id);
    return res;
}

void EventManager::DelAllEvent(void)
{
    event_t * ptr = NULL;
    event_t * ptr_next = NULL;

    ptr = first;

    if (!ptr)
	return;
    
    do
    {
	ptr_next = ptr->next;
	free (ptr);
	ptr = ptr_next;       
    
    } while (ptr);

    return;
}

event_t * EventManager::GetRoot(void)
{
    return first;
}

event_t * EventManager::PopFirstEvent(void)
{
    event_t * current;

    if (!first)
	return NULL;

    current = first;

    first = first->next;

    dbg("Event %02d enqueued", current->id);
    return current;   

}

void EventManager::RestoreDay(timeDayOfWeek_t input_day)
{
    unsigned char i;
    time_t time_now, time_day, time_event;
    event_t * ptr;

    dbg("EventManager restoring day %s", dayStr(input_day));
    dbg("Event before %d", EventNumber());

    // getting time now
    time_now = now();
    dbg("time_now %02d.%02d.%04d-%02d:%02d:%02d",
	day(time_now), month(time_now), year(time_now),
	hour(time_now), minute(time_now), second(time_now));
    
    // remove hours and minutes
    time_day = previousMidnight(time_now);    

    // add days
    time_day += (input_day - weekday())*SECS_PER_DAY;
    dbg("time_day %02d.%02d.%04d-%02d:%02d:%02d",
	day(time_day), month(time_day), year(time_day),
	hour(time_day), minute(time_day), second(time_day));

    for (i = 0; i < sizeof(programEvent)/sizeof(fixedEvent_t); i++) 
    {

	if (programEvent[i].day != input_day)
	{
	    // wrong day
	    dbg("Program event day %s, input day %s", 
		dayStr(programEvent[i].day),
		dayStr(input_day));
	    continue;
	}

	time_event = time_day
	    + programEvent[i].hour*SECS_PER_HOUR
	    + programEvent[i].minute*SECS_PER_MIN;

	if (time_event < time_now)
	{
	    // elapsed event	
	    dbg("Elapsed event %d\n", i);
	    continue;
	}

	AddEvent(time_event, programEvent[i].action);
    }

    // DEBUG purpose
    dbg("Event List:");
    ptr = first;
    while (ptr)
    {
	dbg("Event %02d, %02d.%02d.%04d-%02d:%02d:%02d",
	    ptr->id, day(ptr->time), month(ptr->time), year(ptr->time),
	    hour(ptr->time), minute(ptr->time), second(ptr->time));
	ptr = ptr->next;
    }
    dbg("");


}

