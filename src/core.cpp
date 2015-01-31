
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

void setup_serial(void)
{
   Serial.begin(9600);
}

void setup_Mirf(void)
{
    // Setup pins / SPI.
   
    /* To change CE / CSN Pins:
     * 
     * Mirf.csnPin = 9;
     * Mirf.cePin = 7;
     */
    /*
      Mirf.cePin = 7;
      Mirf.csnPin = 8;
    */
    Mirf.spi = &MirfHardwareSpi;
    Mirf.init();
    
    /*
     * Configure reciving address.
     */
    
    Mirf.setRADDR((byte *)"clie1");
  
    /*
     * Set the payload length to sizeof(unsigned long) the
     * return type of millis().
     *
     * NB: payload on client and server must be the same.
     */
    
    Mirf.payload = sizeof(unsigned long);
    
    /*
     * Write channel and payload config then power up reciver.
     */
    
    /*
     * To change channel:
     * 
     * Mirf.channel = 10;
     *
     * NB: Make sure channel is legal in your area.
     */
    
    Mirf.config();
  
    Serial.println("Mirf setup completed"); 
}

void loop_Mirf(void)
{
    unsigned long time = millis();
    
    Mirf.setTADDR((byte *)"serv1");
    
    Mirf.send((byte *)&time);
    
    while(Mirf.isSending()) {
    }
    Serial.println("Mirf: finished sending");
    delay(10);
    
    while(!Mirf.dataReady()) {
	//Serial.println("Waiting");
	if ( ( millis() - time ) > 1000 ) {
	    Serial.println("Timeout on response from server!");
	    return;
	}
    }
    
    Mirf.getData((byte *) &time);
    
    Serial.print("Ping: ");
    Serial.println((millis() - time));
    
    delay(1000);
} 
