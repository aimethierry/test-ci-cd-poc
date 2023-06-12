#ifndef CLIENTCONTROL_H
#define CLIENTCONTROL_H
#include <stdint.h>
// #include "Arduino.h"

#include <Ethernet.h>
#include <SPI.h>

class ClientControl{
    private: 
        EthernetServer * server;
        bool ServerControl;
        
        IPAddress serverIp; 
        boolean newData;
        byte ndx;
        char endMarker;
        int portNumber;
        IPAddress ip; 
        char * ipAddress;
        EthernetClient * cl;


    public:
        ClientControl(const char * ipAddress, int portNumber);
        ~ClientControl();
        // bool SendMessage(char * message);
        // bool GetMessage(char * buffer);
        void SetUpEthernet();
        bool CheckConnection();
        int GetPortNumber();
        void GetClientIp(char * clientIP);        
        EthernetClient * GetEthernetClient();
};

#endif 