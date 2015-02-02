#ifndef __EVENT_HEADER
#define __EVENT_HEADER

#include <Time.h>  

typedef enum {
    ON,
    OFF,
    ACTIONS_NUM
}  action_t;

typedef struct event_t event_t;

struct event_t
{
    time_t time;
    unsigned int id;
    action_t action;
    event_t * next;    
};

typedef struct
{
    timeDayOfWeek_t Day;
    uint8_t Minute; 
    uint8_t Hour; 
    action_t action;
} fixedEvent_t;


class EventList {

public:
    EventList(void);

    unsigned int Size(void);
    void AddEvent(event_t * event);
    bool RemEvent(unsigned int id);
    event_t * GetFirstEvent(void);
    event_t * CreateEvent(time_t time, action_t action);

private:
    event_t * first;
    static unsigned int event_counter;
};


class EventManager {

public:
    EventManager(void);
    
private:
    EventList list;

    void RestoreDay(timeDayOfWeek_t day);

};

#endif // __EVENT_HEADER
