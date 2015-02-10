#ifndef __RELE_HEADER
#define __RELE_HEADER

#define RELE_MODE_NC         1
#define RELE_MODE_NO         0

class Relay
{

public:
    Relay(unsigned int pin, bool mode_nc);
    ~Relay(void);

    void Active(void);
    void Deactive(void);

private:
    
    unsigned int pin;
    bool mode_nc;

};

#endif // __RELE_HEADER
