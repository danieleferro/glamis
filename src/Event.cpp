#include "Event.h"
#include <Arduino.h>

#define DEBUG                1

#ifdef DEBUG
#define dbg(fmt, args...)         printf(fmt, ## args)
#else
#define dbg(fmt, args...)
#endif

const fixedEvent_t sundayEvent[] =
{
    {10, 30, ON},
    {12, 30, OFF},
};

const fixedEvent_t mondayEvent[] =
{ 
    {9, 05, ON},
    {12, 30, OFF},
};

const fixedEvent_t tuesdayEvent[] =
{ 
    {10, 30, ON},
    {12, 30, OFF},
};

const fixedEvent_t wednesdayEvent[] =
{ 
    {10, 30, ON},
    {12, 30, OFF},
};

const fixedEvent_t thursdayEvent[] =
{ 
    {10, 30, ON},
    {12, 30, OFF},
};

const fixedEvent_t fridayEvent[] =
{ 
    {10, 30, ON},
    {12, 30, OFF},
};

const fixedEvent_t saturdayEvent[] = { 
    {10, 30, ON},
    {12, 30, OFF},
};



// -------- Event
unsigned int Event::id_counter = 0;

Event::Event(time_t time, action_t action)
{
    this->time = time;
    this->action = action;
    this->id = ++id_counter;
    this->valid = true;
}

action_t Event::GetAction(void)
{
    return action;
}
 
bool Event::IsElapsed(void)
{
    return (now() > time);
}

// -------- EventList

EventList::EventList(void)
{
    this->first = NULL;
}

unsigned int EventList::Count(void)
{
    unsigned int count = 0;
    Event * ptr = first;
    
    if (!ptr)
	return 0;

    do
    {
	dbg("Event %02d, %02d.%02d%04d-%02d:%02d:%02d\n",
	    ptr->id, day(ptr->time), month(ptr->time), day(ptr->time),
	    hour(ptr->time), minute(ptr->time), second(ptr->time));

	count++;
	ptr = ptr->next;
    }  while (ptr);
  
    return count;
}

void EventList::AddEvent(Event * event)
{
    Event * current;

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
    Event * ptr_pre = NULL;
    Event * ptr_del = NULL;
    

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

Event * EventList::GetFirstEvent(void)
{
    Event * current;

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
    for (i = 0; i < 7; i++) {
	RestoreDay((timeDayOfWeek_t)(i+1));
    }
}

void EventManager::RestoreDay(timeDayOfWeek_t day)
{
    unsigned int i;
    unsigned int event_num;
    tmElements_t time_elem;
    time_t time_now, time_sunday, time_event;
    int elapse_days;
    const fixedEvent_t * fixed_event_ptr;

    dbg("EventManager restoring day %s", dayStr(day));
 
    // getting time now
    time_now = now();
    // time_t for sunday
    elapse_days = abs(weekday(time_now) - day);

    time_sunday = time_now + SECS_PER_DAY * elapse_days;
    breakTime(time_sunday, time_elem);

    switch (day)
    {
    case dowSunday:
	event_num = sizeof(sundayEvent)/sizeof(fixedEvent_t);
	fixed_event_ptr = sundayEvent;
	break;

    case dowMonday:
	event_num = sizeof(mondayEvent)/sizeof(fixedEvent_t);
	fixed_event_ptr = mondayEvent;
	break;

    case dowTuesday:
	event_num = sizeof(tuesdayEvent)/sizeof(fixedEvent_t);
	fixed_event_ptr = tuesdayEvent;
	break;

    case dowWednesday:
	event_num = sizeof(wednesdayEvent)/sizeof(fixedEvent_t);
	fixed_event_ptr = wednesdayEvent;
	break;

    case dowThursday:
	event_num = sizeof(thursdayEvent)/sizeof(fixedEvent_t);
	fixed_event_ptr = thursdayEvent;
	break;

    case dowFriday:
	event_num = sizeof(fridayEvent)/sizeof(fixedEvent_t);
	fixed_event_ptr = fridayEvent;
	break;

    case dowSaturday:
	event_num = sizeof(saturdayEvent)/sizeof(fixedEvent_t);
	fixed_event_ptr = saturdayEvent;
	break;

    default:
	return;	
    }

    for (i = 0; i < event_num; i++) 
    {
	// overwrite only second, minute, hour
	time_elem.Second = 0;
	time_elem.Minute = fixed_event_ptr[i].Minute;
	time_elem.Hour = fixed_event_ptr[i].Hour;
	
	time_event = makeTime(time_elem);

	if (time_event < time_now)
	    continue;

	list.AddEvent(new Event(time_event, fixed_event_ptr[i].action));
    }
}
