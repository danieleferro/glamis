#ifndef __RELE_HEADER
#define __RELE_HEADER

#define RELE_MODE_NC         1
#define RELE_MODE_NO         0

class Rele
{

public:
    Rele(unsigned int pin, bool mode_nc);
    ~Rele(void);

    void Active(void);
    void Deactive(void);

private:
    
    unsigned int pin;
    bool mode_nc;

};

#endif // __RELE_HEADER
