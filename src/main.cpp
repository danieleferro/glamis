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
#define RELE_PIN               6
Relay relay(RELE_PIN, RELE_MODE_NC);


// CE = 9, CSN = 10
RF24 radio(9, 10);
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xE0FDADFD1CLL, 0x5F31AD89B1LL };

#define BUTTON_PIN             2
#define BUTTON_DEBOUNCE_DELAY  50 // ms
// Variables will change:
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled

//
// Payload
#define MAX_PAYLOAD_RESP            32

char receive_payload[MAX_PAYLOAD_RESP];

void setup(void)
{
    //
    // PIN
    //
    pinMode(BUTTON_PIN, INPUT);

    //
    // Print preamble
    //
    Serial.begin(115200);
    printf_begin();
    dbg("\n\r** GLAMIS CLIENT **\n\r");

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
    uint8_t len = 0;
    int buttonReading;

    // if there is data ready
    if (radio.available())
    {
    	// Dump the payloads until we've gotten everything
      	while (radio.available())
      	{
	    // Fetch the payload, and see if this was the last one.
	    len = radio.getDynamicPayloadSize();
	    radio.read(receive_payload, len);

	    // Put a zero at the end for easy printing
	    receive_payload[len] = 0;

	    // Spew it
	    dbg("Got payload size=%i value=%s", len, receive_payload);
	    
	    if (!strcmp(receive_payload, "on"))
	    {
		relay.Active();	
	    }
	    else if (!strcmp(receive_payload, "off"))
	    {
		relay.Deactive();		
	    }

      	}

      	// First, stop listening so we can talk
      	radio.stopListening();

      	// Send the final one back.
      	radio.write(receive_payload, len);
      	dbg("Sent response\r\n");

      	// Now, resume listening so we catch the next packets.
      	radio.startListening();
    }


    //  read the state of the switch into a local variable:
    buttonReading = digitalRead(BUTTON_PIN);

    // check to see if you just pressed the button 
    // (i.e. the input went from LOW to HIGH),  and you've waited 
    // long enough since the last press to ignore any noise:  

    // If the switch changed, due to noise or pressing:
    if (buttonReading != lastButtonState)
    {
	// reset the debouncing timer
	lastDebounceTime = millis();
    } 
  
    if ((millis() - lastDebounceTime) > BUTTON_DEBOUNCE_DELAY) 
    {
	// whatever the reading is at, it's been there for longer
	// than the debounce delay, so take it as the actual current state:

	// if the button state has changed:
	if (buttonReading != buttonState) 
	{
	    buttonState = buttonReading;

	    // only toggle the LED if the new button state is HIGH
	    if (buttonState == HIGH) 
	    {
		relay.Switch();	
	    }
	}
    }
  
    // save the reading.  Next time through the loop,
    // it'll be the lastButtonState:
    lastButtonState = buttonReading;

}
