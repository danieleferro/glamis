#include "Event.h"
#include <Arduino.h>

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
unsigned int EventList::event_counter = 0;

event_t * EventList::CreateEvent(time_t time, action_t action)
{
    
    event_t * event_ptr = (event_t *) malloc(sizeof(event_t)); 
    memset(event_ptr, 0, sizeof(event_t));

    event_ptr->id = (++event_counter) % 1000;
    return event_ptr;

}

EventList::EventList(void)
{
    this->first = NULL;
}

unsigned int EventList::Size(void)
{
    unsigned int count = 0;
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

void EventList::AddEvent(event_t * event)
{
    event_t * current;

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

    return;
}
 
bool EventList::RemEvent(unsigned int id)
{
    bool res;
    event_t * ptr_pre = NULL;
    event_t * ptr_del = NULL;
    

    // Check whether it is the head node
    if (first->id == id) {
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
    while (ptr_del != NULL) {

        if (ptr_del->id == id) {
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

// -------- EventManager

EventManager::EventManager(void)
{
    unsigned int i;
    for (i = 1; i < 8; i++) {
	RestoreDay((timeDayOfWeek_t)(i));
    }
}

void EventManager::RestoreDay(timeDayOfWeek_t day)
{
    unsigned int i;
    time_t time_now, time_day, time_event;

    dbg("EventManager restoring day %s", dayStr(day));
 
    // getting time now
    time_now = now();
    // remove hours and minutes
    time_day = time_now 
	- hour(time_now)*SECS_PER_HOUR
	- minute(time_now)*SECS_PER_MIN;

    // add days
    time_day += abs(weekday(time_now) - day)*SECS_PER_DAY;

    for (i = 0; i < sizeof(programEvent)/sizeof(fixedEvent_t); i++) 
    {

	if (programEvent[i].Day != day) {
	    // wrong day
	    continue;
	}

	time_event = time_day
	    + programEvent[i].Hour*SECS_PER_HOUR
	    + programEvent[i].Minute*SECS_PER_MIN;

	if (time_event < time_now) {
	    // elapsed event
	    continue;
	}

	list.AddEvent(list.CreateEvent(time_event, programEvent[i].action));
    }
}
