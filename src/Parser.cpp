#include "Parser.h"

Parser::Parser(ParserWriter * writer)
{
    this->writer = writer;
    cmdBeginInd = CMD_BEGIN_IND;
    cmdEndInd = CMD_END_IND;
}

Parser::Parser(ParserWriter * writer, byte cmdBeginIndicator, byte cmdEndIndicator)
{
    this->writer = writer;
    cmdBeginInd = cmdBeginIndicator;
    cmdEndInd = cmdEndIndicator;
}

unsigned char Parser::Read(byte * data, byte dataLength)
{
    byte sum = 0;
    unsigned char i;

    if (dataLength < 4)
    {
	// header, tail and check sum not present
	return 0;
    }

    if (data[0] != cmdBeginInd || data[dataLength-1] != cmdEndInd) 
    {
	// invalid header/tail
	return 0;             
    }
    
    for (i = 1; i < dataLength-2; i++)
    {
 	sum += data[i];
    }

    if ((sum & 0xFF) != data[dataLength-1]) 
    {
	// check sum failed
	return 0;
    }

    // shift data to remove
    for (i = 0; i < dataLength-2; i++)
 	data[i] = data[i+1];

    return (dataLength-3);
    
}



void Parser::Write(byte cmd)
{
    writer->WriteCallback(cmdBeginInd);
    writer->WriteCallback(cmd);
    // send sum
    writer->WriteCallback(cmd);
    writer->WriteCallback(cmdEndInd);
}

void Parser::Write(byte* cmd, byte cmdLength)
{
    byte sum = 0;

    writer->WriteCallback(cmdBeginInd);
    for (byte i = 0; i < cmdLength; i++)
    {
	sum += cmd[i];
	writer->WriteCallback(cmd[i]);
    }
    // send sum
    writer->WriteCallback(sum);
    writer->WriteCallback(cmdEndInd);
}

