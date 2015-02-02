#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "SerialParser.h"

#define DATA_LEN                          25

static byte buffer[128];
static unsigned char i = 0;
    
void write(byte data)
{
    printf("%02x ", data);
    buffer[i] = data;
    i++;
}

void receive(byte* buf, byte cmdLength)
{
    unsigned char i = 0;
    
    for (i = 0; i < cmdLength; i++)
    {
	printf("%02x ", buf[i]);
    }
}

int main(int argc, char ** argv)
{
    unsigned char i;
    SerialParser parser;
    static byte data[DATA_LEN];
    

    srand(time(NULL));

    parser.SenderPtr = write;
    parser.ReceiverPtr = receive;

    // random data
    printf(">> Generating data\n");
    for (i = 0; i < DATA_LEN; i++)
    {	
	data[i] = rand();
	printf("%02x ", data[i]);
    }
    printf("\n--------\n");

    // sending
    printf(">> Sending data\n");
    parser.Send(data, DATA_LEN);
    printf("\n--------\n");
    
    // receiving
    printf(">> Reading data\n");
    // DATA_LEN plus header, tailer, check sum
    for (i = 0; i < DATA_LEN+3; i++)
    {	
	parser.Receive(buffer[i]);
    }
    printf("\n--------\n");
    

}
