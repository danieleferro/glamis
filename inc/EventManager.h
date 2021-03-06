#ifndef __EVENT_HEADER
#define __EVENT_HEADER

#include <Time.h>  

typedef enum
{
    OFF,
    ON,
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
    uint8_t hour; 
    uint8_t minute; 
    action_t action;
} fixedEvent_t;


class EventManager
{

public:
    EventManager(void);
    ~EventManager(void);

    unsigned int EventNumber(void);
    bool AddEvent(time_t time, action_t action);
    bool DelEvent(unsigned char id);
    void PopFirstEvent(void);

    event_t * GetRoot(void);

    void RestoreDay(timeDayOfWeek_t day);

private:
    event_t * first;

    void DelAllEvent(void);
    void PrintAllEvent(void);

    static unsigned char event_counter;

};

#endif // __EVENT_HEADER
