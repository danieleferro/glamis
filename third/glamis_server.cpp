/**
 * Glamis Server 
 *
 * Read command from WAN and set relay 
 */

#include <RF24/RF24.h>
#include "printf.h"
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

// Set up nRF24L01 radio on SPI bus plus pins 
// CE = 25, CSN = 0
RF24 radio(25,0);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xE0FDADFD1CLL, 0x5F31AD89B1LL };

//
// Payload
//
#define MAX_PAYLOAD_RESP            32

char receive_payload[MAX_PAYLOAD_RESP];

// The payload will always be the same, what will change is how much of it we send.
static char active_payload[] = "on";
static char disactive_payload[] = "off";

void setup(void)
{
    // Simple codes for UNO nRF adapter that uses pin 10 as Vcc
    //
    // Print preamble
    //

    printf("\n\r** GLAMIS SEVER **\n\r");

    //
    // Setup and configure rf radio
    //

    radio.begin();

    // enable dynamic payloads
    radio.setCRCLength(RF24_CRC_16);
    radio.enableDynamicPayloads();

    // optionally, increase the delay between retries & # of retries
    radio.setAutoAck(true);
    radio.setPALevel(RF24_PA_HIGH);
    radio.setRetries(5,15);

    //
    // Open pipes to other nodes for communication
    //

    // This simple sketch opens two pipes for these two nodes to communicate
    // back and forth.

    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);

    //
    // Start listening
    //

    radio.startListening();

    //
    // Dump the configuration of the rf unit for debugging
    //

    // radio.printDetails();
}

bool sendCommand(bool active)
{
    char * msg_ptr;
    unsigned long started_waiting_at;
    bool timeout, res;
    uint8_t len;

    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it
    // This will block until complete
    if (active)
    {
	printf("Sending ON command\n");
	msg_ptr = active_payload;
	len = sizeof(active_payload);
    }
    else
    {
	printf("Sending OFF command\n");
	msg_ptr = disactive_payload;
	len = sizeof(disactive_payload);
    }
    radio.write(msg_ptr, len, false);

    // Now, continue listening
    radio.startListening();

    // Wait here until we get a response, or timeout
    started_waiting_at = millis();
    timeout = false;
    while ( ! radio.available() && ! timeout )
	if (millis() - started_waiting_at > 1 + 5000/1000 )
	    timeout = true;

    // Describe the results
    if (timeout)
    {
	printf("Failed, response timed out.\n\r");
	res = false;
    }
    else
    {
	// Grab the response, compare, and send to debugging spew
	len = radio.getDynamicPayloadSize();
	radio.read(receive_payload, len);

	// Put a zero at the end for easy printing
	receive_payload[len] = 0;

	// Spew it
	printf("Got response size=%i value=%s\n\r", len, receive_payload);

	res = (strcmp(receive_payload, msg_ptr) == 0);

    }
    
    // Update size for next time.

    // Try again 1s later
    delay(1000);

    return res;
}

int main(int argc, char ** argv)
{
    char * command_str = NULL;
    int c;
    bool command, res;

    opterr = 0;
    while ((c = getopt(argc, argv, "c:")) != -1)
    {
	switch (c)
	{
	case 'c':
	    command_str = optarg;
	    break;

	case '?':
	    if (optopt == 'c')
	    {
		fprintf(stderr, "Option -%c requires an argument.\n", optopt);
	    }
	    else if (isprint (optopt))
	    {
		fprintf(stderr, "Unknown option `-%c'.\n", optopt);
	    }
	    else
	    {
		fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
	    }
	    return 1;

	default:
	    abort ();
	}
    }
    
    // printf("command string = %s\n", command_str);
    if (!strcmp(command_str, "on"))
    {
	command = true;
    }
    else if (!strcmp(command_str, "off"))
    {
	command = false;
    }
    else
    {
	fprintf(stderr, "Unknown option \"%s\"\n", command_str);
	return 1;
    }
	
    setup();
    res = false;
    c = 0;
    do
    {	
        res = sendCommand(command);
	c++;
    } while (!res && c < 3);

    return 1;
}
