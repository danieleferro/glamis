#ifndef __SERIAL_PARSER_HEADER
#define __SERIAL_PARSER_HEADER

#define BUF_MAX_LENGTH                   64
#define CMD_BEGIN_IND                    94 // ^
#define CMD_END_IND                      10 // Line Feed (LF)

#ifdef ARDUINO
#include <Arduino.h>
#else
typedef unsigned char       byte;
#endif


class SerialParser
{

public:
    SerialParser(void);
    SerialParser(byte cmdBeginIndicator, byte cmdEndIndicator);

    void (*ReceiverPtr)(byte* buf, byte cmdLength);
    void (*SenderPtr)(byte buf);

    void Receive(byte data);
    void Receive(byte * data, byte dataLength);
    void Send(byte cmd);
    void Send(byte* cmd, byte cmdLength);

private:
	
    byte cmdBufferIdx;
    byte buf[BUF_MAX_LENGTH];
    bool cmdBegan;
    bool bufferFull;

    byte bufMaxLength;
    byte cmdBeginInd;
    byte cmdEndInd;

};

#endif // __SERIAL_PARSER_HEADER
