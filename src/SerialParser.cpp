#include "SerialParser.h"

SerialParser::SerialParser(void)
{
    cmdBufferIdx = 0;
    cmdBegan = false;
    bufferFull = false;
    
    cmdBeginInd = CMD_BEGIN_IND;
    cmdEndInd = CMD_END_IND;
}

SerialParser::SerialParser(byte cmdBeginIndicator, byte cmdEndIndicator)
{
    cmdBufferIdx = 0;
    cmdBegan = false;
    bufferFull = false;
	
    cmdBeginInd = cmdBeginIndicator;
    cmdEndInd = cmdEndIndicator;
}

void SerialParser::Read(byte data)
{
    byte sum;
    unsigned char i;

    if (data == cmdBeginInd) 
    {
	cmdBegan = true;
	bufferFull = false;
	cmdBufferIdx = 0;                
    }
    else if (data == cmdEndInd)
    {
	cmdBegan = false;
	// check sum
	if (cmdBufferIdx > 1) {
	    sum = 0;
	    for (i = 0; i < cmdBufferIdx - 1; i++) sum += buf[i];
	    
	    if ((sum & 0xFF) == buf[cmdBufferIdx - 1]) {
		// data ok
		ReceiverCallback(buf, cmdBufferIdx - 1);
	    }
	}
    } 
    else
    {
	if (cmdBegan)
	{
	    if (cmdBufferIdx >= BUF_MAX_LENGTH)
	    {
		bufferFull = true;
	    }
	    else
	    {
		buf[cmdBufferIdx] = data;
		cmdBufferIdx++;
	    }
	}
    }
}


void SerialParser::Read(byte * data, byte dataLength)
{
    byte i;
    for (i = 0; i < dataLength; i++)
    {
	Read(data[i]);
    }

}

void SerialParser::Write(byte cmd)
{
    SenderCallback(cmdBeginInd);
    SenderCallback(cmd);
    // send sum
    SenderCallback(cmd);
    SenderCallback(cmdEndInd);
}

void SerialParser::Write(byte* cmd, byte cmdLength)
{
    byte sum = 0;

    SenderCallback(cmdBeginInd);
    for (byte i = 0; i < cmdLength; i++)
    {
	sum += cmd[i];
	SenderCallback(cmd[i]);
    }
    // send sum
    SenderCallback(sum);
    SenderCallback(cmdEndInd);
}

