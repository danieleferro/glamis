#ifndef __UARTWIFI_HEADER__
#define __UARTWIFI_HEADR__

#include <SoftwareSerial.h>
#include <Arduino.h>

#define    ESP_TX_PIN             15
#define    ESP_RX_PIN             16
// #define    ESP_RES_BUFFER        128


// The way of encrypstion
#define    ENCR_OPEN          0
#define    ENCR_WEP           1
#define    ENCR_WAP_PSK       2
#define    ENCR_WAP2_PSK      3
#define    ENCR_WAP_WAP2_PSK  4

// Communication mode 
#define    TCP     1
#define    tcp     1
#define    UDP     0
#define    udp     0

#define    OPEN    1
#define    CLOSE   0

// The type of initialized WIFI
#define    STA     1
#define    AP      2
#define    AP_STA  3

#define SERIAL_TX_BUFFER_SIZE 128
#define SERIAL_RX_BUFFER_SIZE 128

class UartWifi
{
public:

    UartWifi(void);
    ~UartWifi(void);

    bool Begin(void);
    
    // Initialize port
    bool Initialize(byte mode, String ssid, String pwd, byte chl = 1, byte ecn = 2);
    bool IpConfig(byte type, String addr, int port, bool a = 0, byte id = 0);
    
    // send data in sigle connection mode
    bool Send(String str);
    // send data int multiple connection mode
    bool Send(byte id, String str);
    
    int ReceiveMessage(char *buf, unsigned char buf_length);
    
    // String begin(void);

    /*=================WIFI Function Command=================*/
    // reset the module   
    void Reset(void);    
    // set the working mode of module
    bool confMode(byte a);   
    // set the name and password of wifi
    bool confJAP(String ssid , String pwd); 
    // set the parametter of SSID, password, channel, encryption in AP mode.
    bool confSAP(String ssid , String pwd , byte chl , byte ecn);
    
    // inquire the current mode of wifi module
    String showMode(void);
    // show the list of wifi hotspot  
    String showAP(void);
    // show the name of current wifi access port
    String showJAP(void);
    // quit the connection of current wifi
    bool quitAP(void);
    // show the parameter of ssid, password, channel, encryption in AP mode
    String showSAP(void);

    /*================TCP/IP commands================*/
    // inquire the connection status
    String showStatus(void);
    // show the current connection mode(sigle or multiple)
    String showMux(void);
    // set the connection mode(sigle:0 or multiple:1)
    bool confMux(bool a);
    // create new tcp or udp connection (sigle connection mode)
    bool newMux(byte type, String addr, int port);
    // create new tcp or udp connection (multiple connection mode)(id:0-4) 
    bool newMux(byte id, byte type, String addr, int port);
    // close tcp or udp (sigle connection mode)
    void closeMux(void);
    // close tcp or udp (multiple connection mode)
    void closeMux(byte id);
    // show the current ip address
    String showIP(void);
    // set the parameter of server
    bool confServer(byte mode, int port);
	
    // client id(0-4)
    unsigned char chlID;		
    unsigned char GetChlID(void);

    String m_rev;

    SoftwareSerial esp8266;
	
};

#endif // __UARTWIFI_HEADR__
