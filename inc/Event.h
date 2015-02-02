#ifndef __EVENT_HEADER
#define __EVENT_HEADER

#include <Time.h>  

class EventList;

typedef enum {
    ON,
    OFF,
    ACTIONS_NUM
}  action_t;


class Event
{
public:
    
    Event(time_t time, action_t action);
    action_t GetAction(void);
    bool IsElapsed(void);

private:
    time_t time;
    unsigned int id;
    action_t action;
    bool valid;

    static unsigned int id_counter;
    Event * next;
    
    friend class EventList;
};

typedef struct  { 
    timeDayOfWeek_t Day;
    uint8_t Minute; 
    uint8_t Hour; 
    action_t action;
} fixedEvent_t;


class EventList {

public:
    EventList(void);

    unsigned int Count(void);
    void AddEvent(Event * event);
    bool RemEvent(unsigned int id);
    Event * GetFirstEvent(void);

private:
    Event * first;
};


class EventManager {

public:
    EventManager(void);
    
private:
    EventList list;

    void RestoreDay(timeDayOfWeek_t day);

};

#endif // __EVENT_HEADER
