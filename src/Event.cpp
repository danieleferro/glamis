#include "Event.h"
#include <Arduino.h>

#define MAX_EVENT_RETURN               128
#define MAX_EVENT_LIST                 255

#define DEBUG                1

#ifdef DEBUG
#define dbg(fmt, args...)         printf(fmt, ## args)
#else
#define dbg(fmt, args...)
#endif

const fixedEvent_t programEvent[] =
{
    {dowSunday, 10, 30, ON},
    {dowSunday, 12, 30, OFF},
    {dowMonday, 9, 10, ON},
    {dowMonday, 18, 30, OFF},
    {dowThursday, 11, 43, ON},
    {dowThursday, 16, 30, OFF},
    {dowFriday, 6, 30, ON},
    {dowFriday, 10, 30, OFF},
    {dowSunday, 13, 30, ON},
    {dowSunday, 17, 30, OFF},
};

// -------- EventList
unsigned char EventList::event_counter = 0;

EventList::EventList(void)
{
    this->first = NULL;
    return_array = (event_t**) malloc(sizeof(event_t*)*MAX_EVENT_RETURN);
}

EventList::~EventList(void)
{
    DelAllEvent();
    free (return_array);
}

event_t * EventList::CreateEvent(time_t time, action_t action)
{
    
    event_t * event_ptr = (event_t *) malloc(sizeof(event_t)); 
    memset(event_ptr, 0, sizeof(event_t));

    event_ptr->id = (++event_counter) % 1000;
    return event_ptr;

}


unsigned char EventList::Size(void)
{
    unsigned char count = 0;
    event_t * ptr = first;
    
    while (ptr)
    {
	dbg("Event %02d, %02d.%02d%04d-%02d:%02d:%02d\n",
	    ptr->id, day(ptr->time), month(ptr->time), day(ptr->time),
	    hour(ptr->time), minute(ptr->time), second(ptr->time));

	count++;
	ptr = ptr->next;
    }
  
    return count;
}

bool EventList::AddEvent(event_t * event)
{
    event_t * current;

    // check max size
    if (Size() >= MAX_EVENT_LIST)
	return false;

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

    dbg("Event %02d added before event %02d\n",
	event->id, event->next->id);

    return true;
}
 
bool EventList::DelEvent(unsigned char id)
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
            // If it is the last node, update the tail
	    /*
            if (ptr_del == _pTail) {
                _pTail = ptr_pre;
            }
	    */
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
	dbg("Event %02d removed\n", id);
    else
	dbg("Event %02d NOT removed\n", id);
    return res;
}

void EventList::DelAllEvent(void)
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

event_t * EventList::GetFirstEvent(void)
{
    event_t * current;

    if (!first)
	return NULL;

    current = first;

    first = first->next;

    dbg("Event %02d enqueued\n", current->id);
    return current;   

}

unsigned char EventList::GetEventBefore(time_t time, 
					event_t ** event_array)
{
    event_t * ptr = first;
    unsigned char i = 0;
    
    while (ptr)
    {
	if (ptr->time <= time)
	{
	    return_array[i] = ptr;
	    i++;
	    ptr = ptr->next;
	}
	else {
	    // event are time ordered
	    break;
	}
    }
  
    event_array = return_array;
    return i;
}

// -------- EventManager

EventManager::EventManager(void)
{
    unsigned char i;

    for (i = 1; i < 8; i++)
    {
	RestoreDay((timeDayOfWeek_t)(i));
    }
}

EventManager::~EventManager(void)
{
}


void EventManager::RestoreDay(timeDayOfWeek_t day)
{
    unsigned char i;
    time_t time_now, time_day, time_event;

    dbg("EventManager restoring day %s", dayStr(day));
 
    // getting time now
    time_now = now();
    // remove hours and minutes
    time_day = previousMidnight(time_now);

    // add days
    time_day += abs(weekday(time_now) - day)*SECS_PER_DAY;

    for (i = 0; i < sizeof(programEvent)/sizeof(fixedEvent_t); i++) 
    {

	if (programEvent[i].day != day)
	{
	    // wrong day
	    continue;
	}

	time_event = time_day
	    + programEvent[i].hour*SECS_PER_HOUR
	    + programEvent[i].minute*SECS_PER_MIN;

	if (time_event < time_now)
	{
	    // elapsed event
	    continue;
	}

	list.AddEvent(list.CreateEvent(time_event, programEvent[i].action));
    }
}

unsigned char EventManager::GetEventToday(event_t ** event_array)
{
    time_t time_searching;

    // remove hours and minutes
    time_searching = previousMidnight(now()) + SECS_PER_DAY;

    return list.GetEventBefore(time_searching, event_array);
}

unsigned char EventManager::GetEventTomorrow(event_t ** event_array)
{
    time_t time_searching;

    // remove hours and minutes
    time_searching = previousMidnight(now()) + 2*SECS_PER_DAY;

    return list.GetEventBefore(time_searching, event_array);
}

unsigned char EventManager::GetEventDayAfterTomorrow(event_t ** event_array)
{
    time_t time_searching;

    // remove hours and minutes
    time_searching = previousMidnight(now()) + 3*SECS_PER_DAY;

    return list.GetEventBefore(time_searching, event_array);
}
