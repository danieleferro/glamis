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

    // called after a message has been received
    virtual void ReceiverCallback(byte* buf, byte cmdLength) = 0;
    // called to send data
    virtual void SenderCallback(byte buf) = 0;

    void Read(byte data);
    void Read(byte * data, byte dataLength);
    void Write(byte cmd);
    void Write(byte* cmd, byte cmdLength);

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
