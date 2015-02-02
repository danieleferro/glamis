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

class ParserWriter
{
public:
    // called to send data
    virtual void WriteCallback(byte buf) = 0;
};

class Parser
{

public:
    Parser(ParserWriter * writer);
    Parser(ParserWriter * writer, 
		 byte cmdBeginIndicator, byte cmdEndIndicator);


    unsigned char Read(byte * data, byte dataLength);
    void Write(byte cmd);
    void Write(byte* cmd, byte cmdLength);

private:
	
    byte cmdBeginInd;
    byte cmdEndInd;

    ParserWriter * writer;
};

#endif // __SERIAL_PARSER_HEADER
