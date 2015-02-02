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
    unsigned char id;
    action_t action;
    event_t * next;    
};

typedef struct
{
    timeDayOfWeek_t day;
    uint8_t minute; 
    uint8_t hour; 
    action_t action;
} fixedEvent_t;


class EventList {

public:
    EventList(void);
    ~EventList(void);

    unsigned char Size(void);
    bool AddEvent(event_t * event);
    bool DelEvent(unsigned char id);
    event_t * GetFirstEvent(void);
    event_t * CreateEvent(time_t time, action_t action);
    unsigned char GetEventBefore(time_t time, 
				 event_t ** event_array);    

private:
    event_t * first;
    event_t ** return_array;
    void DelAllEvent(void);
    static unsigned char event_counter;
};


class EventManager {

public:
    EventManager(void);
    ~EventManager(void);

    void RestoreDay(timeDayOfWeek_t day);
    unsigned char GetEventToday(event_t ** event_array);
    unsigned char GetEventTomorrow(event_t ** event_array);
    unsigned char GetEventDayAfterTomorrow(event_t ** event_array);

private:
    EventList list;

};

#endif // __EVENT_HEADER
