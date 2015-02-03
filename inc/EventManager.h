#ifndef __EVENT_HEADER
#define __EVENT_HEADER

#include <Time.h>  

typedef enum
{
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


class EventManager
{

public:
    EventManager(void);
    ~EventManager(void);

    unsigned char EventNumber(void);
    bool AddEvent(time_t time, action_t action);
    bool DelEvent(unsigned char id);
    event_t * PopFirstEvent(void);

    event_t * GetRoot(void);

    void RestoreDay(timeDayOfWeek_t day);

private:
    event_t * first;

    void DelAllEvent(void);

    static unsigned char event_counter;

};

#endif // __EVENT_HEADER
