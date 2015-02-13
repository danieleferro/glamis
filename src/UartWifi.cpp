#include "UartWifi.h"
#include "core.h"

#define DEBUG                     1

#ifdef DEBUG
#define dbg(fmt, args...)         serial_printf_f(F(fmt), ## args)
#else
#define dbg(fmt, args...)
#endif

static const char head[4] = {0x0D,0x0A};   
static const char tail[7] = {0x0D,0x0A,0x0D,0x0A};        


UartWifi::UartWifi(unsigned int rx_pin, unsigned int tx_pin) :
    esp8266(rx_pin, tx_pin)
{
    // pinMode(rx_pin, INPUT);
    // pinMode(tx_pin, OUTPUT);

    // reserve data string
    data.reserve(128);
} 

UartWifi::~UartWifi(void)
{
}


bool UartWifi::Begin(unsigned long baud)
{
    bool res;
    // the default baud rate of ESP8266 is 115200
    esp8266.begin(baud);	
    
    esp8266.flush();
    esp8266.setTimeout(3000);

    esp8266.print(F("AT+RST\r\n"));
    dbg("AT+RST");

    if (esp8266.find("OK"))
    {
	dbg("Module ESP8266 is ready");
	res = true;
    }
    else
    {
	dbg("Module ESP8266 have no response");
	res = false;
    }

    delay(2000);
    esp8266.flush();
    
    dbg("-----------");
    return res;

}


/*************************************************************************
//Initialize port

	mode:	setting operation mode
		STA: 	Station
		AP:	 	Access Point
		AT_STA:	Access Point & Station

	chl:	channel number
	ecn:	encryption
		OPEN          0
		WEP           1
		WAP_PSK       2
		WAP2_PSK      3
		WAP_WAP2_PSK  4		

	return:
		true	-	successfully
		false	-	unsuccessfully

***************************************************************************/
bool UartWifi::Initialize(esp8266_mode_t mode, const char * ssid, const char * pwd, 
			  byte chl, byte ecn)
{
    bool b;

    b = confMode(mode);
    if (!b)
    {
	return false;
    }

    // Reset();

    switch (mode)
    {
    case ESP8266_MODE_STA:
	
	confJAP(ssid, pwd);
	break;
	
    case ESP8266_MODE_AP:
	
	confSAP(ssid, pwd, chl, ecn);
	break;
	
    case ESP8266_MODE_AP_STA:
	confJAP(ssid, pwd);
	confSAP(ssid, pwd, chl, ecn);
	break;
	
    default:
	dbg("Invalid mode");	
    }
    
    return true;
}

/*************************************************************************
//Set up tcp or udp connection

	type:	tcp or udp
	
	addr:	ip address
	
	port:	port number
	
	a:	set multiple connection
		0 for sigle connection
		1 for multiple connection
		
	id:	id number(0-4)

	return:
		true	-	successfully
		false	-	unsuccessfully

***************************************************************************/
bool UartWifi::IpConfig(esp8266_proto_t protocol, const char * addr, int port, 
			esp8266_mux_t mux, byte id)
{
    bool result = false;

    confMux(mux);
	
    delay(5000);

    if (mux == ESP8266_MUX_SING)
    {
	result = newMux(protocol, addr, port);
    }
    else if (mux == ESP8266_MUX_MULT)
    {
	result = newMux(id, protocol, addr, port);
    }
    return result;
}

/*************************************************************************
//receive message from wifi

	buf:	buffer for receiving data
	
	chlID:	<id>(0-4)

	return:	size of the buffer
	

***************************************************************************/
int UartWifi::ReceiveMessage(char *buf, unsigned char buf_length)
{
    unsigned int sLen, i, j, res;
    bool found;

    if (esp8266.available() <= 0)
	return 0;

    data = "";

    start = millis();
    if (esp8266.read() != '+')
    {	
	return 0;
    }


    while ((millis()-start) < 5000) 
    {
	if (esp8266.available() > 0)
	{
	    data = data + esp8266.read();
	}
	if (data.indexOf("\nOK") != -1)
	{
	    break;
	}
    }
    
    dbg("Data from ESP8266:");
#ifdef DEBUG
    while (esp8266.available() > 0)
    {
	dbg("%c", esp8266.read());
    }
#endif

    // int sLen = strlen(data.c_str());
    sLen = data.length();
    for (i = 0; i <= sLen; i++)
    {
	if (data[i] == ':')
	{
	    break;
	}
	
    }
	
    found = false;
    for (j = 4; j <= i; j++)
    {
	if (data[j] == ',')
	{
	    found = true;
	    break;
	}
	
    }

    if (found)
    {
	chlID = data.substring(4, j).toInt();
	dbg("Channel ID is %d", chlID);
	
	res = data.substring(j+1, i).toInt();	    
    }
    else
    {			
	res = data.substring(4, i).toInt();
    }
    
    dbg("Data length is %d", res);
    data.substring(i+1, i+1+res).toCharArray(buf, buf_length);
    dbg("Received data %s", buf);
    
    return res;
}

//////////////////////////////////////////////////////////////////////////


/*************************************************************************
//reboot the wifi module



***************************************************************************/
void UartWifi::Reset(void)
{
    esp8266.println(F("AT+RST"));
    start = millis();

    while ((millis()-start) < 5000) 
    {                            
        if(esp8266.find("ready"))
        {
	    dbg("reboot wifi is OK");
	    break;
        }
    }
}

/*********************************************
 *********************************************
 *********************************************
             WIFI Function Commands
	     *********************************************
	     *********************************************
	     *********************************************
	     */

/*************************************************************************
//inquire the current mode of wifi module

	return:	string of current mode
		Station
		AP
		AP+Station

***************************************************************************/
esp8266_mode_t UartWifi::showMode()
{
    data = "";
    esp8266.println(F("AT+CWMODE?"));  

    start = millis();
    while ((millis()-start) < 2000) 
    {
	if (esp8266.available()>0)
	{
	    data = data + esp8266.read();
	}
	if (data.indexOf("OK") != -1)
	{
	    break;
	}
    }
    
    if (data.indexOf("1") != -1)
    {
        return ESP8266_MODE_STA;
    }
    else if (data.indexOf("2") != -1)
    {
	return ESP8266_MODE_AP;
    }
    else if (data.indexOf("3") != -1)
    {
	return ESP8266_MODE_AP_STA;
    }
    else
    {
	return ESP8266_MODE_UNDEF;
    }
}



/*************************************************************************
//configure the operation mode

	a:	
		1	-	Station
		2	-	AP
		3	-	AP+Station
		
	return:
		true	-	successfully
		false	-	unsuccessfully

***************************************************************************/

bool UartWifi::confMode(esp8266_mode_t mode)
{
    char c;
    data = "";

    esp8266.print(F("AT+CWMODE="));  
    esp8266.println(mode);

    dbg("Mode %d\n", mode);

    start = millis();
    while ((millis()-start) < 2000) 
    {
	if (esp8266.available() > 0)
	{
	    c = esp8266.read();
	    data = data + c;
#ifdef DEBUG
	    Serial.print(c);
#endif
	}
	if (data.indexOf("OK") != -1 || data.indexOf("no change") != -1)
	{
#ifdef DEBUG
	    Serial.println("");
#endif
	    delay(2000);
	    esp8266.flush();
	    
	    dbg("-----------");

	    return true;
	}
	if (data.indexOf("ERROR") != -1 || data.indexOf("busy") != -1)
	{
#ifdef DEBUG
	    Serial.println("");
#endif
	    delay(2000);
	    esp8266.flush();
	    
	    dbg("-----------");
	    return false;
	}
	  
    }

    dbg("No response for confMode");
    return false;
}


/*************************************************************************
//show the list of wifi hotspot
		
	return:	string of wifi information
		encryption,SSID,RSSI
		

***************************************************************************/

bool UartWifi::showAP(char * out, unsigned int out_len)
{
    char c;
    data = "";

    esp8266.flush();
    esp8266.println(F("AT+CWLAP"));  

    start = millis();
    while (1) // (millis()-start) < 20000) 
    {
	delay(10);
	if (esp8266.available() > 0)
	{
	    c = esp8266.read();
	    // data = data + c;
#ifdef DEBUG
	    Serial.print(c);
#endif
	}
	if (data.indexOf("+CWLAP:") != -1)
	{
	    dbg("\n >> \n");
	    data = "";
	}
	
	if (data.indexOf("OK") != -1 || data.indexOf("ERROR") != -1 )
	{
	    dbg("\n received OK or ERROR");
	    break;
	}
    }
    if (data.indexOf("ERROR")!=-1)
    {
	dbg("\n received ERROR");
        return false;
    }
    
    dbg("\n -- DATA is:");
#ifdef DEBUG
    Serial.println(data);
#endif

    data.replace("AT+CWLAP", "");
    data.replace("OK", "");
    data.replace("+CWLAP", "WIFI");
    data.replace(tail, "");
    data.replace(head, "");
    
    data.toCharArray(out, out_len);
    
    return (data.length() != 0);
}


/*************************************************************************
//show the name of current wifi access port
		
	return:	string of access port name
		AP:<SSID>
		

***************************************************************************/
bool UartWifi::showJAP(char * out, unsigned int out_len)
{
    data = "";

    esp8266.flush();
    esp8266.println(F("AT+CWJAP?"));  

    start = millis();

    while ((millis()-start) < 3000) 
    {
	if (esp8266.available() > 0)
	{
	    data = data + esp8266.read();
	}
	if (data.indexOf("OK") !=-1 || data.indexOf("ERROR") != -1)
	{
	    break;
	}
    }

    data.replace("AT+CWJAP?","");
    data.replace("+CWJAP","AP");
    data.replace("OK","");
    data.replace(tail,"");
    data.replace(head,"");
      
    data.toCharArray(out, out_len);

    return (data.length() != 0);
}


/*************************************************************************
//configure the SSID and password of the access port
		
		return:
		true	-	successfully
		false	-	unsuccessfully
		

***************************************************************************/
bool UartWifi::confJAP(const char * ssid , const char * pwd)
{
    esp8266.print(F("AT+CWJAP=\""));
    esp8266.print(ssid);
    esp8266.print(F("\",\""));
    esp8266.print(pwd);
    esp8266.println(F("\""));

    start = millis();
    while ((millis()-start) < 3000) 
    {                            
        if (esp8266.find("OK"))
        {
	    delay(2000);
	    esp8266.flush();
	    
	    dbg("confJAP OK");
	    dbg("-----------");
	    return true;           
        }
    }
    return false;
}
/*************************************************************************
//quite the access port
		
		return:
			true	-	successfully
			false	-	unsuccessfully
		

***************************************************************************/

bool UartWifi::quitAP(void)
{
    esp8266.println(F("AT+CWQAP"));

    start = millis();

    while ((millis()-start) < 3000) 
    {                            
        if(esp8266.find("OK"))
        {
	    return true;           
        }
    }
    return false;
}

/*************************************************************************
//show the parameter of ssid, password, channel, encryption in AP mode
		
		return:
			mySAP:<SSID>,<password>,<channel>,<encryption>

***************************************************************************/
bool UartWifi::showSAP(char * out, unsigned int out_len)
{
    data = "";

    esp8266.println(F("AT+CWSAP?"));  
    start = millis();

    while ((millis()-start) < 3000) 
    {
	if (esp8266.available() > 0)
	{
	    data = data + esp8266.read();
	}
	if (data.indexOf("OK") != -1 || data.indexOf("ERROR") != -1)
	{
	    break;
	}
    }

    data.replace("AT+CWSAP?","");
    data.replace("+CWSAP","mySAP");
    data.replace("OK","");
    data.replace(tail,"");
    data.replace(head,"");
      
    data.toCharArray(out, out_len);

    return (data.length() != 0);
}

/*************************************************************************
//configure the parameter of ssid, password, channel, encryption in AP mode
		
		return:
			true	-	successfully
			false	-	unsuccessfully

***************************************************************************/

bool UartWifi::confSAP(const char * ssid , const char * pwd , byte chl, byte ecn)
{
    esp8266.print(F("AT+CWSAP=\""));  
    esp8266.print(ssid);
    esp8266.print(F("\",\""));
    esp8266.print(pwd);
    esp8266.print(F("\","));
    esp8266.print(chl);	    
    esp8266.print(F(","));
    esp8266.println(ecn);

    start = millis();
    while ((millis()-start) <3000)
    {	
        if(esp8266.find("OK"))
        {
	    return true;
        }
    }
	 
    return false;
}


/*********************************************
 *********************************************
 *********************************************
             TPC/IP Function Command
	     *********************************************
	     *********************************************
	     *********************************************
	     */

/*************************************************************************
//inquire the connection status
		
		return:		string of connection status
			<ID>  0-4
			<type>  tcp or udp
			<addr>  ip
			<port>  port number

***************************************************************************/

bool UartWifi::showStatus(char * out, unsigned int out_len)
{
    data = "";

    esp8266.println(F("AT+CIPSTATUS"));  

    start = millis();
    while ((millis()-start) < 3000)
    {
	if (esp8266.available() > 0)
	{
	    data = data + esp8266.read();
	}
	if (data.indexOf("OK") != -1)
	{
	    break;
	}
    }

    data.replace("AT+CIPSTATUS","");
    data.replace("OK","");
    data.replace(tail,"");
    data.replace(head,"");
          
    data.toCharArray(out, out_len);

    return (data.length() != 0);
}

/*************************************************************************
//show the current connection mode(sigle or multiple)
		
		return:		string of connection mode
			0	-	sigle
			1	-	multiple

***************************************************************************/
esp8266_mux_t UartWifi::showMux(void)
{
    data = "";

    esp8266.println(F("AT+CIPMUX?"));  

    start = millis();
    while ((millis()-start) < 3000)
    {
	if (esp8266.available() > 0)
	{
	    data = data + esp8266.read();
	}
	if (data.indexOf("OK") != -1)
	{
	    break;
	}
    }

    data.replace("AT+CIPMUX?","");
    data.replace("+CIPMUX","");
    data.replace("OK","");
    data.replace(tail,"");
    data.replace(head,"");

    if (data == "0")
	return ESP8266_MUX_SING;
    else if (data == "1")
	return ESP8266_MUX_MULT;
    else
	return ESP8266_MUX_UNDEF;
          
}

/*************************************************************************
//configure the current connection mode(sigle or multiple)
		
		a:		connection mode
			0	-	sigle
			1	-	multiple
			
	return:
		true	-	successfully
		false	-	unsuccessfully
***************************************************************************/
bool UartWifi::confMux(esp8266_mux_t a)
{
    esp8266.print(F("AT+CIPMUX="));
    esp8266.println(a);           

    start = millis();
    while (millis()-start<3000) {                            
        if(esp8266.find("OK")==true )
        {
	    return true;
        }
    }
	 
    return false;
}


/*************************************************************************
//Set up tcp or udp connection	(signle connection mode)

	type:	tcp or udp
	
	addr:	ip address
	
	port:	port number
		

	return:
		true	-	successfully
		false	-	unsuccessfully

***************************************************************************/
bool UartWifi::newMux(esp8266_proto_t protocol, const char * addr, int port)
{
    data = "";

    esp8266.print(F("AT+CIPSTART="));
    if (protocol == ESP8266_PROTO_TCP)
    {
        esp8266.print(F("\"TCP\""));
    }
    else
    {
        esp8266.print(F("\"UDP\""));
    }

    esp8266.print(F(",\""));
    esp8266.print(addr);
    esp8266.print(F("\","));
    esp8266.println(port);
    esp8266.println(F("\""));

    start = millis();

    while ((millis()-start) < 3000)
    { 
	if (esp8266.available() > 0)
	{
	    data = data + esp8266.read();
	}
	if (data.indexOf("OK") != -1 || 
	    data.indexOf("ALREAY CONNECT") != -1 ||
	    data.indexOf("ERROR") != -1)
	{
	    return true;
	}
    }

    return false;
}
/*************************************************************************
//Set up tcp or udp connection	(multiple connection mode)

	type:	tcp or udp
	
	addr:	ip address
	
	port:	port number
		
	id:	id number(0-4)

	return:
		true	-	successfully
		false	-	unsuccessfully

***************************************************************************/
bool UartWifi::newMux(byte id, esp8266_proto_t protocol, const char * addr, int port)

{
    data = "";

    esp8266.print(F("AT+CIPSTART=\""));
    esp8266.print(id);
    esp8266.print(F("\""));

    if (protocol == ESP8266_PROTO_TCP)
    {
        esp8266.print(F("\"TCP\""));
    }
    else
    {
        esp8266.print(F("\"UDP\""));
    }

    esp8266.print(F(",\""));
    esp8266.print(addr);
    esp8266.print(F("\","));
    esp8266.println(port);
    esp8266.println(F("\""));

    start = millis();

    while ((millis()-start) < 3000)
    { 
	if (esp8266.available() > 0)
	{
	    data = data + esp8266.read();
	}
	if (data.indexOf("OK") != -1 || 
	    data.indexOf("ALREAY CONNECT") != -1 ||
	    data.indexOf("ERROR") != -1)
	{
	    return true;
	}
    }

    return false;

}
/*************************************************************************
//send data in sigle connection mode

	str:	string of message

	return:
		true	-	successfully
		false	-	unsuccessfully

***************************************************************************/
bool UartWifi::Send(const char * in)
{
    bool found;

    esp8266.print(F("AT+CIPSEND="));
    esp8266.println(strlen(in));

    start = millis();

    while ((millis()-start) < 5000)
    {  
        if (esp8266.find(">"))
        {
	    found = true;
	    break;
        }
    }

    if (found)
    {
	esp8266.print(in);
    }
    else
    {
	closeMux();
	return false;
    }

    data = "";
    start = millis();
    while ((millis()-start) < 5000)
    {
	if (esp8266.available() > 0)
	{
	    data = data + esp8266.read();
	}
	if (data.indexOf("SEND OK") != -1)
	{
	    return true;
	}
    }
    return false;
}

/*************************************************************************
//send data in multiple connection mode

	id:		<id>(0-4)
	
	str:	string of message

	return:
		true	-	successfully
		false	-	unsuccessfully

***************************************************************************/
bool UartWifi::Send(byte id, const char * in)
{
    bool found;

    esp8266.print(F("AT+CIPSEND="));
    esp8266.print(id);
    esp8266.print(F(","));
    esp8266.println(strlen(in));

    start = millis();

    while ((millis()-start) < 5000)
    {                          
        if (esp8266.find(">"))
        {
	    found = true;
	    break;
        }
    }

    if (found) 
    {
	esp8266.print(in);
    }
    else
    {
	closeMux(id);
	return false;
    }


    data = "";
    start = millis();
    while ((millis()-start) < 5000)
    {
	if (esp8266.available() > 0)
	{
	    data = data + esp8266.read();
	}
	if (data.indexOf("SEND OK") != -1)
	{
	    return true;
	}
    }
    return false;
}

/*************************************************************************
//Close up tcp or udp connection	(sigle connection mode)


***************************************************************************/
bool UartWifi::closeMux(void)
{
    esp8266.println(F("AT+CIPCLOSE"));

    data = "";

    start = millis();
    while ((millis()-start) < 3000)
    {
	if (esp8266.available() > 0)
	{
	    data = data + esp8266.read();
	}
	if (data.indexOf("Linked") != -1 || 
	    data.indexOf("ERROR") != -1 ||
	    data.indexOf("we must restart") != -1)
	{
	    return false;
	}
    }
    return true;
}


/*************************************************************************
//Set up tcp or udp connection	(multiple connection mode)
		
	id:	id number(0-4)

***************************************************************************/
bool UartWifi::closeMux(byte id)
{
    esp8266.print(F("AT+CIPCLOSE="));
    esp8266.println(id);
    
    data = "";
    start = millis();

    while ((millis()-start) < 3000)
    {
	if (esp8266.available() > 0)
	{
	    data = data + esp8266.read();
	}
	if (data.indexOf("OK") != -1 || 
	    data.indexOf("Link is not") != -1 ||
	    data.indexOf("Cant close") != -1)
	{
	    return false;
	}
    }

    return true;

}

/*************************************************************************
//show the current ip address
		
	return:	string of ip address

***************************************************************************/
bool UartWifi::showIP(char * out, unsigned int out_len)
{
    unsigned int a;
    data = "";
    char c;

    //dbg("AT+CIFSR");
    for (a=0; a < 3; a++)
    {
	esp8266.println(F("AT+CIFSR"));  

	start = millis();

	while ((millis()-start) < 3000) 
	{
	    while(esp8266.available() > 0)
	    {
		c = esp8266.read();
		data = data + c;
#ifdef DEBUG
		Serial.print(c);
#endif

	    }
	    if (data.indexOf("AT+CIFSR") != -1)
	    {
		break;
	    }
	}
	if(data.indexOf(".") != -1)
	{
	    break;
	}
	data = "";
    }

    Serial.println("DATA:");

    Serial.print(data);
    Serial.println("-------");
    data.replace("AT+CIFSR","");
    data.replace(tail,"");
    data.replace(head,"");
  
    data.toCharArray(out, out_len);

    return (data.length() != 0);
}

/*************************************************************************
////set the parameter of server

	mode:
		0	-	close server mode
		1	-	open server mode
		
	port:	<port>
		
	return:
		true	-	successfully
		false	-	unsuccessfully

***************************************************************************/

bool UartWifi::confServer(esp8266_status_t status, int port)
{
    bool found = false;

    esp8266.print(F("AT+CIPSERVER="));  
    esp8266.print(status);
    esp8266.print(F(","));
    esp8266.println(port);

    data = "";
    start = millis();
    while ((millis()-start) < 3000)
    {
	if (esp8266.available() > 0)
	{
	    data = data + esp8266.read();
	}
	if (data.indexOf("OK") != -1 || 
	    data.indexOf("no charge") != -1)
	{
	    found = true;
	    break;
	}
    }
    return found;
}


unsigned char UartWifi::GetChlID(void)
{
    return chlID;
}
