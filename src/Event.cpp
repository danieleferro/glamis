#include "Event.h"
#include <math.h>
#include <Arduino.h>

const fixedEvent_t sundayEvent[] = { 
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
    unsigned int count = 1;
    Event * ptr = first;
    
    if (!ptr)
	return 0;

    while (ptr->next)
    {
	ptr = ptr->next;
	count++;
    }
    
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

    return;
}
 
bool EventList::RemEvent(unsigned int id)
{
    Event *ptr_pre = NULL;
    Event *ptr_del = NULL;
    
    // Check whether it is the head node
    if (first->id == id) {
        // point to the node to be deleted
        ptr_del = first;
        // update
        first = ptr_del->next;
        delete ptr_del;
        return true;
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
            return true;
        }
        ptr_pre = ptr_del;
        ptr_del = ptr_del->next;
    }

    return false;
}

Event * EventList::GetFirstEvent(void)
{
    Event * current;

    if (!first)
	return NULL;

    current = first;

    first = first->next;

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
    switch (day)
    {

    case dowSunday:
	RestoreSunday();
	break;
    default:
	break;
    }    
}

void EventManager::RestoreSunday(void)
{
    unsigned int i;
    tmElements_t time_elem;
    time_t time_now, time_sunday;
    int elapse_days;

    // getting time now
    time_now = now();
    // time_t for sunday
    elapse_days = fabs(weekday(time_now) - dowSunday);

    time_sunday = time_now + SECS_PER_DAY * elapse_days;
    breakTime(time_now, time_elem);

    for (i = 0; i < sizeof(sundayEvent)/sizeof(fixedEvent_t); i++) 
    {
	// overwrite only second, minute, hour
	time_elem.Second = 0;
	time_elem.Minute = sundayEvent[i].Minute;
	time_elem.Hour = sundayEvent[i].Hour;

	list.AddEvent(new Event(makeTime(time_elem), sundayEvent[i].action));

    }
    
}
