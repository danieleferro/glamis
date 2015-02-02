#include "ServerManager.h"

#define WIFI_SSID                 "AUCT"
#define WIFI_PASSWORD             "password"

#define DEBUG                     1

#ifdef DEBUG
#define dbg(fmt, args...)         printf("ServerManager: "fmt, ## args)
#else
#define dbg(fmt, args...)
#endif

#define OK_RESP                   "OK"
#define IPD_RESP                  "+IPD"
#define CPI_SEND                  "AT+CIPSEND=%d,%d"


ServerManager::ServerManager(EventManager * event_manager) :
    esp8266(ESP_RX_PIN, ESP_TX_PIN)    
{
    this->event_manager = event_manager;
    memset(esp8266_res_buffer, 0, ESP_RES_BUFFER);
    esp8266_res_buffer_index = 0;
    parser.ReceiverPtr = ExternExcuteCommand;
    parser.SenderPtr = ExternWriteCommand;
}

ServerManager::~ServerManager(void)
{

}

bool ServerManager::StartESP8266(void)
{
    // baud rate might be different
    esp8266.begin(9600);
    
    // reset module
    sendESP8266Command("AT+RST\r\n", 2000);

    // set wifi mode
    sendESP8266Command("AT+CWMODE=3\r\n", 1000);
    if (memcmp(esp8266_res_buffer, OK_RESP, sizeof(OK_RESP)))
	return false;

    // connect to router
    sendESP8266Command("AT+CWJAP=\""WIFI_SSID"\",\""WIFI_PASSWORD"\"\r\n", 2000);
    if (memcmp(esp8266_res_buffer, OK_RESP, sizeof(OK_RESP)))
	return false;

    // get ip address
    sendESP8266Command("AT+CIFSR\r\n", 1000);
    dbg("sever IP is %s\n", esp8266_res_buffer);

    // configure for multiple connections (??)
    sendESP8266Command("AT+CIPMUX=1\r\n", 1000);
    if (memcmp(esp8266_res_buffer, OK_RESP, sizeof(OK_RESP)))
	return false;

     // turn on server on port 80
    sendESP8266Command("AT+CIPSERVER=1,5555\r\n", 1000);
    if (memcmp(esp8266_res_buffer, OK_RESP, sizeof(OK_RESP)))
	return false;

    return true;
}


void ServerManager::sendESP8266Command(const char * command, const int timeout)
{
    unsigned long int time;
    esp8266_res_buffer_index = 0;
    
    // send command to esp8266
    esp8266.print(command);
    
    time = millis();
    
    while( (time+timeout) > millis())
    {
	while(esp8266.available() && esp8266_res_buffer_index < ESP_RES_BUFFER-1)
	{
	    esp8266_res_buffer[esp8266_res_buffer_index] = esp8266.read();
	    esp8266_res_buffer_index++;
	}  
    }
    
    if (esp8266_res_buffer_index)
	dbg("reponse from esp8266: %s\n", esp8266_res_buffer);

    return;
}

unsigned char ServerManager::receiveESP8266Data(void)
{
    unsigned char connection_id;
    unsigned int index;
    String data = esp8266.readString();    
   
    // +IPD,0,n:xxxxxxxxxx
    dbg("Received ");
    for (index = 0; index < data.length(); index++)
	dbg("%c", data[index]);
    dbg("\n");
    
    // get connection ID
    index = data.indexOf(",", 5);
    connection_id = data.substring(5, index).toInt();
    dbg("Connection id is %d\n", connection_id);

    // get length
    index++;
    esp8266_res_buffer_index = data.substring(index, data.indexOf(":")).toInt();
    dbg("Data length is %d\n", esp8266_res_buffer_index);
    
    // get data
    data.substring(data.indexOf(":")).getBytes(esp8266_res_buffer,
					       ESP_RES_BUFFER);

    return connection_id;
    
}

bool ServerManager::ServeESP8266Connection(void)
{
     // check if the esp is sending a message
    if (!esp8266.available()) 
	return false;

    
    if (!esp8266.find(IPD_RESP))
    {
	dbg("Error, no +IPD found\n");
	return false;
    }

    delay(100);

    connection_id = receiveESP8266Data();
    
    // parse and process data
    parser.Receive(esp8266_res_buffer, esp8266_res_buffer_index);

    return true;
}


void ServerManager::ExcuteCommand(byte* buf, byte cmdLength)
{
    char command[128];
    
    if (!cmdLength) {
	goto end;
    }

    // first byte is command
    switch (buf[0])
    {

    case GET_EVENT_LIST:
	// redirect 
	break;
    case DEL_EVENT:
	break;
    case ADD_EVENT:
	break;
    case POWER_ON:
	break;
    case POWER_OFF:
	break;

    default:
	break;
    }

    // close connection
    snprintf(command, 128, "AT+CIPCLOSE=%d\r\n", connection_id);
    sendESP8266Command(command, 1000);

    dbg("close connection res %s\n", esp8266_res_buffer);
  
end:
    return;
}

void ServerManager::powerOnCommand(void)
{
    char command[128];
    unsigned char res[] = "OK";

    // execute command (2 bytes data)
    snprintf(command, 128, "AT+CIPSEND=%d,%d\r\n", connection_id, 2);
    sendESP8266Command(command, 1000);

    // send data
    parser.Send(res, sizeof(res));
    
}
