#include "UartWifi.h"
#include "core.h"

#define DEBUG                     1

#ifdef DEBUG
#define dbg(fmt, args...)         serial_printf_f(F(fmt), ## args)
#else
#define dbg(fmt, args...)
#endif

UartWifi::UartWifi(unsigned int rx_pin, unsigned int tx_pin) :
    esp8266(rx_pin, tx_pin)
{
    pinMode(rx_pin, INPUT);
    pinMode(tx_pin, OUTPUT);
} 

UartWifi::~UartWifi(void)
{
}


bool UartWifi::Begin(unsigned long baud)
{
    // the default baud rate of ESP8266 is 115200
    esp8266.begin(115200);	
    
    esp8266.flush();
    esp8266.setTimeout(3000);

    esp8266.println(F("AT+RST"));
    dbg("AT+RST");

    if(esp8266.find("OK"))
    {
	dbg("Module ESP8266 is ready");
	return true;
    }
    else
    {
	dbg("Module ESP8266 have no response");
	return false;
    }

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
bool UartWifi::Initialize(byte mode, String ssid, String pwd, byte chl, byte ecn)
{
    if (mode == STA)
    {	
	bool b = confMode(mode);
	if (!b)
	{
	    return false;
	}
	Reset();
	confJAP(ssid, pwd);
    }
    else if (mode == AP)
    {
	bool b = confMode(mode);
	if (!b)
	{
	    return false;
	}
	Reset();
	confSAP(ssid, pwd, chl, ecn);
    }
    else if (mode == AP_STA)
    {
	bool b = confMode(mode);
	if (!b)
	{
	    return false;
	}
	Reset();
	confJAP(ssid, pwd);
	confSAP(ssid, pwd, chl, ecn);
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
bool UartWifi::IpConfig(byte type, String addr, int port, bool a, byte id)
{
    bool result = false;
    if (a == 0 )
    {
	confMux(a);
	
	long timeStart = millis();
	while (1)
	{
	    long time0 = millis();
	    if (time0 - timeStart > 5000)
	    {
		break;
	    }
	}
	result = newMux(type, addr, port);
    }
    else if (a == 1)
    {
	confMux(a);
	long timeStart = millis();
	while (1)
	{
	    long time0 = millis();
	    if (time0 - timeStart > 5000)
	    {
		break;
	    }
	}
	result = newMux(id, type, addr, port);
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
    //+IPD,<len>:<data>
    //+IPD,<id>,<len>:<data>
    String data = "";
    
    if (esp8266.available() <= 0)
	return 0;

    unsigned long start;
    start = millis();
    char c0 = esp8266.read();
    if (c0 == '+')
    {	
	while (millis()-start<5000) 
	{
	    if (esp8266.available()>0)
	    {
		char c = esp8266.read();
		data += c;
	    }
	    if (data.indexOf("\nOK")!=-1)
	    {
		break;
	    }
	}
	//Serial.println(data);
	// int sLen = strlen(data.c_str());
	int sLen = data.length();
	int i,j;
	for (i = 0; i <= sLen; i++)
	{
	    if (data[i] == ':')
	    {
		break;
	    }
	    
	}
	bool found = false;
	for (j = 4; j <= i; j++)
	{
	    if (data[j] == ',')
	    {
		found = true;
		break;
	    }
	    
	}
	int iSize;
	//dbg(data);
	//dbg("");
	if(found ==true)
	{
	    String _id = data.substring(4, j);
	    chlID = _id.toInt();
	    String _size = data.substring(j+1, i);
	    iSize = _size.toInt();
	    //dbg(_size);
	    String str = data.substring(i+1, i+1+iSize);
	    // strcpy(buf, str.c_str());
	    str.toCharArray(buf, buf_length);
	    //dbg(str);
	    
	}
	else
	{			
	    String _size = data.substring(4, i);
	    iSize = _size.toInt();
	    //dbg(iSize);
	    //dbg("");
	    String str = data.substring(i+1, i+1+iSize);
	    // strcpy(buf, str.c_str());
	    str.toCharArray(buf, buf_length);
	    //dbg(str);
	}
	return iSize;
    }
	
    return 0;
}

//////////////////////////////////////////////////////////////////////////


/*************************************************************************
//reboot the wifi module



***************************************************************************/
void UartWifi::Reset(void)
{
    esp8266.println(F("AT+RST"));
    unsigned long start;
    start = millis();
    while (millis()-start<5000) {                            
        if(esp8266.find("ready")==true)
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
String UartWifi::showMode()
{
    String data;
    esp8266.println(F("AT+CWMODE?"));  
    unsigned long start;
    start = millis();
    while (millis()-start<2000) {
	if(esp8266.available()>0)
	{
	    char a =esp8266.read();
	    data=data+a;
	}
	if (data.indexOf("OK")!=-1)
	{
	    break;
	}
    }
    if(data.indexOf("1")!=-1)
    {
        return "Station";
    }else if(data.indexOf("2")!=-1)
    {
	return "AP";
    }else if(data.indexOf("3")!=-1)
    {
	return "AP+Station";
    }else
    {
	return "undefined";
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

bool UartWifi::confMode(byte a)
{
    String data;
    esp8266.print(F("AT+CWMODE="));  
    esp8266.println(String(a));
    unsigned long start;
    start = millis();
    while (millis()-start<2000) {
	if(esp8266.available()>0)
	{
	    char a =esp8266.read();
	    data=data+a;
	}
	if (data.indexOf("OK")!=-1 || data.indexOf("no change")!=-1)
	{
	    return true;
	}
	if (data.indexOf("ERROR")!=-1 || data.indexOf("busy")!=-1)
	{
	    return false;
	}
	  
    }

    return false;
}


/*************************************************************************
//show the list of wifi hotspot
		
	return:	string of wifi information
		encryption,SSID,RSSI
		

***************************************************************************/

String UartWifi::showAP(void)
{
    String data;
    esp8266.flush();
    esp8266.print(F("AT+CWLAP\r\n"));  
    delay(1000);
    while(1);
    unsigned long start;
    start = millis();
    while (millis()-start<8000) {
	if(esp8266.available()>0)
	{
	    char a =esp8266.read();
	    data=data+a;
	}
	if (data.indexOf("OK")!=-1 || data.indexOf("ERROR")!=-1 )
	{
	    break;
	}
    }
    if(data.indexOf("ERROR")!=-1)
    {
        return "ERROR";
    }
    else{
	char head[4] = {0x0D,0x0A};   
	char tail[7] = {0x0D,0x0A,0x0D,0x0A};        
	data.replace("AT+CWLAP","");
	data.replace("OK","");
	data.replace("+CWLAP","WIFI");
	data.replace(tail,"");
	data.replace(head,"");

        return data;
    }
}


/*************************************************************************
//show the name of current wifi access port
		
	return:	string of access port name
		AP:<SSID>
		

***************************************************************************/
String UartWifi::showJAP(void)
{
    esp8266.flush();
    esp8266.println(F("AT+CWJAP?"));  
    String data;
    unsigned long start;
    start = millis();
    while (millis()-start<3000) {
	if(esp8266.available()>0)
	{
	    char a =esp8266.read();
	    data=data+a;
	}
	if (data.indexOf("OK")!=-1 || data.indexOf("ERROR")!=-1 )
	{
	    break;
	}
    }
    char head[4] = {0x0D,0x0A};   
    char tail[7] = {0x0D,0x0A,0x0D,0x0A};        
    data.replace("AT+CWJAP?","");
    data.replace("+CWJAP","AP");
    data.replace("OK","");
    data.replace(tail,"");
    data.replace(head,"");
      
    return data;
}


/*************************************************************************
//configure the SSID and password of the access port
		
		return:
		true	-	successfully
		false	-	unsuccessfully
		

***************************************************************************/
bool UartWifi::confJAP(String ssid , String pwd)
{
	
    esp8266.print(F("AT+CWJAP="));
    esp8266.print(F("\""));     //"ssid"
    esp8266.print(ssid);
    esp8266.print(F("\""));

    esp8266.print(F(","));

    esp8266.print(F("\""));      //"pwd"
    esp8266.print(pwd);
    esp8266.println(F("\""));


    unsigned long start;
    start = millis();
    while (millis()-start<3000) {                            
        if(esp8266.find("OK")==true)
        {
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
    unsigned long start;
    start = millis();
    while (millis()-start<3000) {                            
        if(esp8266.find("OK")==true)
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
String UartWifi::showSAP()
{
    esp8266.println(F("AT+CWSAP?"));  
    String data;
    unsigned long start;
    start = millis();
    while (millis()-start<3000) {
	if(esp8266.available()>0)
	{
	    char a =esp8266.read();
	    data=data+a;
	}
	if (data.indexOf("OK")!=-1 || data.indexOf("ERROR")!=-1 )
	{
	    break;
	}
    }
    char head[4] = {0x0D,0x0A};   
    char tail[7] = {0x0D,0x0A,0x0D,0x0A};        
    data.replace("AT+CWSAP?","");
    data.replace("+CWSAP","mySAP");
    data.replace("OK","");
    data.replace(tail,"");
    data.replace(head,"");
      
    return data;
}

/*************************************************************************
//configure the parameter of ssid, password, channel, encryption in AP mode
		
		return:
			true	-	successfully
			false	-	unsuccessfully

***************************************************************************/

bool UartWifi::confSAP(String ssid , String pwd , byte chl , byte ecn)
{
    esp8266.print(F("AT+CWSAP="));  
    esp8266.print(F("\""));     //"ssid"
    esp8266.print(ssid);
    esp8266.print(F("\""));

    esp8266.print(F(","));

    esp8266.print(F("\""));      //"pwd"
    esp8266.print(pwd);
    esp8266.print(F("\""));

    esp8266.print(F(","));
    esp8266.print(String(chl));

    esp8266.print(F(","));
    esp8266.println(String(ecn));
    unsigned long start;
    start = millis();
    while (millis()-start<3000) {                            
        if(esp8266.find("OK")==true )
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

String UartWifi::showStatus(void)
{
    esp8266.println(F("AT+CIPSTATUS"));  
    String data;
    unsigned long start;
    start = millis();
    while (millis()-start<3000) {
	if(esp8266.available()>0)
	{
	    char a =esp8266.read();
	    data=data+a;
	}
	if (data.indexOf("OK")!=-1)
	{
	    break;
	}
    }

    char head[4] = {0x0D,0x0A};   
    char tail[7] = {0x0D,0x0A,0x0D,0x0A};        
    data.replace("AT+CIPSTATUS","");
    data.replace("OK","");
    data.replace(tail,"");
    data.replace(head,"");
          
    return data;
}

/*************************************************************************
//show the current connection mode(sigle or multiple)
		
		return:		string of connection mode
			0	-	sigle
			1	-	multiple

***************************************************************************/
String UartWifi::showMux(void)
{
    String data;
    esp8266.println(F("AT+CIPMUX?"));  

    unsigned long start;
    start = millis();
    while (millis()-start<3000) {
	if(esp8266.available()>0)
	{
	    char a =esp8266.read();
	    data=data+a;
	}
	if (data.indexOf("OK")!=-1)
	{
	    break;
	}
    }
    char head[4] = {0x0D,0x0A};   
    char tail[7] = {0x0D,0x0A,0x0D,0x0A};        
    data.replace("AT+CIPMUX?","");
    data.replace("+CIPMUX","showMux");
    data.replace("OK","");
    data.replace(tail,"");
    data.replace(head,"");
          
    return data;
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
bool UartWifi::confMux(bool a)
{
    esp8266.print(F("AT+CIPMUX="));
    esp8266.println(a);           
    unsigned long start;
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
bool UartWifi::newMux(byte type, String addr, int port)

{
    String data;
    esp8266.print(F("AT+CIPSTART="));
    if(type>0)
    {
        esp8266.print(F("\"TCP\""));
    }else
    {
        esp8266.print(F("\"UDP\""));
    }
    esp8266.print(F(","));
    esp8266.print(F("\""));
    esp8266.print(addr);
    esp8266.print(F("\""));
    esp8266.print(F(","));
//    esp8266.print(F("\""));
    esp8266.println(String(port));
//    esp8266.println(F("\""));
    unsigned long start;
    start = millis();
    while (millis()-start<3000) { 
	if(esp8266.available()>0)
	{
	    char a =esp8266.read();
	    data=data+a;
	}
	if (data.indexOf("OK")!=-1 || data.indexOf("ALREAY CONNECT")!=-1 || data.indexOf("ERROR")!=-1)
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
bool UartWifi::newMux( byte id, byte type, String addr, int port)

{

    esp8266.print(F("AT+CIPSTART="));
    esp8266.print(F("\""));
    esp8266.print(String(id));
    esp8266.print(F("\""));
    if(type>0)
    {
        esp8266.print(F("\"TCP\""));
    }
    else
    {
        esp8266.print(F("\"UDP\""));
    }
    esp8266.print(F(","));
    esp8266.print(F("\""));
    esp8266.print(addr);
    esp8266.print(F("\""));
    esp8266.print(F(","));
//    esp8266.print(F("\""));
    esp8266.println(String(port));
//    esp8266.println(F("\""));
    String data;
    unsigned long start;
    start = millis();
    while (millis()-start<3000) { 
	if(esp8266.available()>0)
	{
	    char a =esp8266.read();
	    data=data+a;
	}
	if (data.indexOf("OK")!=-1 || data.indexOf("ALREAY CONNECT")!=-1 )
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
bool UartWifi::Send(String str)
{
    esp8266.print(F("AT+CIPSEND="));
//    esp8266.print(F("\""));
    esp8266.println(str.length());
//    esp8266.println(F("\""));
    unsigned long start;
    start = millis();
    bool found;
    while (millis()-start<5000) {                            
        if(esp8266.find(">")==true )
        {
	    found = true;
	    break;
        }
    }
    if(found)
	esp8266.print(str);
    else
    {
	closeMux();
	return false;
    }


    String data;
    start = millis();
    while (millis()-start<5000) {
	if(esp8266.available()>0)
	{
	    char a =esp8266.read();
	    data=data+a;
	}
	if (data.indexOf("SEND OK")!=-1)
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
bool UartWifi::Send(byte id, String str)
{
    esp8266.print(F("AT+CIPSEND="));

    esp8266.print(String(id));
    esp8266.print(F(","));
    esp8266.println(str.length());
    unsigned long start;
    start = millis();
    bool found;
    while (millis()-start<5000) {                          
        if(esp8266.find(">")==true )
        {
	    found = true;
	    break;
        }
    }
    if(found)
	esp8266.print(str);
    else
    {
	closeMux(id);
	return false;
    }


    String data;
    start = millis();
    while (millis()-start<5000) {
	if(esp8266.available()>0)
	{
	    char a =esp8266.read();
	    data=data+a;
	}
	if (data.indexOf("SEND OK")!=-1)
	{
	    return true;
	}
    }
    return false;
}

/*************************************************************************
//Close up tcp or udp connection	(sigle connection mode)


***************************************************************************/
void UartWifi::closeMux(void)
{
    esp8266.println(F("AT+CIPCLOSE"));

    String data;
    unsigned long start;
    start = millis();
    while (millis()-start<3000) {
	if(esp8266.available()>0)
	{
	    char a =esp8266.read();
	    data=data+a;
	}
	if (data.indexOf("Linked")!=-1 || data.indexOf("ERROR")!=-1 || data.indexOf("we must restart")!=-1)
	{
	    break;
	}
    }
}


/*************************************************************************
//Set up tcp or udp connection	(multiple connection mode)
		
	id:	id number(0-4)

***************************************************************************/
void UartWifi::closeMux(byte id)
{
    esp8266.print(F("AT+CIPCLOSE="));
    esp8266.println(String(id));
    String data;
    unsigned long start;
    start = millis();
    while (millis()-start<3000) {
	if(esp8266.available()>0)
	{
	    char a =esp8266.read();
	    data=data+a;
	}
	if (data.indexOf("OK")!=-1 || data.indexOf("Link is not")!=-1 || data.indexOf("Cant close")!=-1)
	{
	    break;
	}
    }

}

/*************************************************************************
//show the current ip address
		
	return:	string of ip address

***************************************************************************/
String UartWifi::showIP(void)
{
    String data;
    unsigned long start;
    //dbg("AT+CIFSR");
    for(int a=0; a<3;a++)
    {
	esp8266.println(F("AT+CIFSR"));  
	start = millis();
	while (millis()-start<3000) {
	    while(esp8266.available()>0)
	    {
		char a =esp8266.read();
		data=data+a;
	    }
	    if (data.indexOf("AT+CIFSR")!=-1)
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
    //dbg(data);
    //dbg("");
    char head[4] = {0x0D,0x0A};   
    char tail[7] = {0x0D,0x0D,0x0A};        
    data.replace("AT+CIFSR","");
    data.replace(tail,"");
    data.replace(head,"");
  
    return data;
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

bool UartWifi::confServer(byte mode, int port)
{
    esp8266.print(F("AT+CIPSERVER="));  
    esp8266.print(String(mode));
    esp8266.print(F(","));
    esp8266.println(String(port));

    String data;
    unsigned long start;
    start = millis();
    bool found = false;
    while (millis()-start<3000) {
	if(esp8266.available()>0)
	{
	    char a =esp8266.read();
	    data=data+a;
	}
	if (data.indexOf("OK")!=-1 || data.indexOf("no charge")!=-1)
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
