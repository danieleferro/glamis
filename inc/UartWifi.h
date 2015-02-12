#ifndef __UARTWIFI_HEADER__
#define __UARTWIFI_HEADR__

#include <SoftwareSerial.h>
#include <Arduino.h>

// The way of encrypstion
#define    ENCR_OPEN          0
#define    ENCR_WEP           1
#define    ENCR_WAP_PSK       2
#define    ENCR_WAP2_PSK      3
#define    ENCR_WAP_WAP2_PSK  4

// Communication mode 
typedef enum
{
    ESP8266_PROTO_UDP = 0,
    ESP8266_PROTO_TCP,
    ESP8266_PROTO_UNDEF

}  esp8266_proto_t;

// Communication mode 
typedef enum
{
    ESP8266_STATUS_CLOSE = 0,
    ESP8266_STATUS_OPEN,
    ESP8266_STATUS_UNDEF

}  esp8266_status_t;


// The type of initialized WIFI
typedef enum
{
    ESP8266_MODE_UNDEF = 0,
    ESP8266_MODE_STA,
    ESP8266_MODE_AP,
    ESP8266_MODE_AP_STA

}  esp8266_mode_t;

// The type of initialized WIFI
typedef enum
{
    ESP8266_MUX_SING = 0,
    ESP8266_MUX_MULT,
    ESP8266_MUX_UNDEF

}  esp8266_mux_t;

class UartWifi
{
public:

    UartWifi(unsigned int rx_pin, unsigned int tx_pin);
    ~UartWifi(void);

    bool Begin(unsigned long baud);
    
    // Initialize port
    bool Initialize(esp8266_mode_t mode, const char *ssid, const char * pwd, 
		    byte chl = 1, byte ecn = 2);
    bool IpConfig(esp8266_proto_t protocol, const char * addr, int port, 
		  esp8266_mux_t mux = ESP8266_MUX_SING, byte id = 0);
    
    // send data in sigle connection mode
    bool Send(const char * in);
    // send data int multiple connection mode
    bool Send(byte id, const char * in);
    
    int ReceiveMessage(char *buf, unsigned char buf_length);
    
    // String begin(void);

    /*=================WIFI Function Command=================*/
    // reset the module   
    void Reset(void);    
    // set the working mode of module
    bool confMode(esp8266_mode_t mode);   
    // set the name and password of wifi
    bool confJAP(const char * ssid , const char * pwd); 
    // set the parametter of SSID, password, channel, encryption in AP mode.
    bool confSAP(const char * ssid , const char * pwd , byte chl, byte ecn);
    
    // inquire the current mode of wifi module
    esp8266_mode_t showMode(void);
    // show the list of wifi hotspot  
    bool showAP(char * out, unsigned int out_len);
    // show the name of current wifi access port
    bool showJAP(char * out, unsigned int out_len);
    // quit the connection of current wifi
    bool quitAP(void);
    // show the parameter of ssid, password, channel, encryption in AP mode
    bool showSAP(char * out, unsigned int out_len);

    /*================TCP/IP commands================*/
    // inquire the connection status
    bool showStatus(char * out, unsigned int out_len);
    // show the current connection mode(sigle or multiple)
    esp8266_mux_t showMux(void);
    // set the connection mode(sigle:0 or multiple:1)
    bool confMux(esp8266_mux_t a);
    // create new tcp or udp connection (sigle connection mode)
    bool newMux(esp8266_proto_t protocol, const char * addr, int port);
    // create new tcp or udp connection (multiple connection mode)(id:0-4) 
    bool newMux(byte id, esp8266_proto_t protocol, const char * addr, int port);
    // close tcp or udp (sigle connection mode)
    bool closeMux(void);
    // close tcp or udp (multiple connection mode)
    bool closeMux(byte id);
    // show the current ip address
    bool showIP(char * out, unsigned int out_len);
    // set the parameter of server
    bool confServer(esp8266_status_t status, int port);
	
    // client id(0-4)
    unsigned char chlID;		
    unsigned char GetChlID(void);

    String m_rev;
    String data;
    unsigned long start;

    SoftwareSerial esp8266;
	
};

#endif // __UARTWIFI_HEADR__
