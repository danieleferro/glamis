/**
 * Glamis client
 *
 * Receive command from RF24 and move relay
 */

#include <SPI.h>
#include <EEPROM.h>
#include "RF24.h"
#include "printf.h"
#include "Relay.h"
#include "core.h"

#define DEBUG                   1
#ifdef DEBUG
#define dbg(fmt, args...)         serial_printf_f(F(fmt), ## args)
#else
#define dbg(fmt, args...)
#endif

//
// Hardware configuration
//
#define LED_PIN                13
#define RELE_PIN               6
#define RELE_MODE_NC           0

// CE = 9, CSN = 10
RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xE0FDADFD1CLL, 0x5F31AD89B1LL };

bool led_on = true;

Relay relay(RELE_PIN, RELE_MODE_NC);


//
// Payload
#define MAX_PAYLOAD_RESP            32

char receive_payload[MAX_PAYLOAD_RESP];

void setup(void)
{

    // 
    // LED
    //
    pinMode(LED_PIN, OUTPUT);

    //
    // Print preamble
    //

    Serial.begin(115200);
    printf_begin();
    printf("\n\r** GLAMIS CLIENT **/\n\r");

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

    radio.setRetries(5, 15);

    //
    // Open pipes to other nodes for communication
    //

    // This simple sketch opens two pipes for these two nodes to communicate
    // back and forth.
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);

    //
    // Start listening
    //

    radio.startListening();

    //
    // Dump the configuration of the rf unit for debugging
    //

    radio.printDetails();

    //
    // TIME
    // 
    setTime(compile_time());

    //
    // EEPROM
    //
    /*
    for (int i = 0; i < 1024; i++)	
	EEPROM.write(i, 0);
    */


}


void loop(void)
{
    // if there is data ready
    if (radio.available())
    {
    	// Dump the payloads until we've gotten everything
      	uint8_t len = 0;
      	while (radio.available())
      	{
	    // Fetch the payload, and see if this was the last one.
	    len = radio.getDynamicPayloadSize();
	    radio.read(receive_payload, len);

	    // Put a zero at the end for easy printing
	    receive_payload[len] = 0;

	    // Spew it
	    printf("Got payload size=%i value=%s\n\r", len, receive_payload);
      	}

      	// First, stop listening so we can talk
      	radio.stopListening();

      	// Send the final one back.
      	radio.write(receive_payload, len);
      	printf("Sent response.\n\r");

      	// Now, resume listening so we catch the next packets.
      	radio.startListening();
    }
}
