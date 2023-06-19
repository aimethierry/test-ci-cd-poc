#include "ClientControl.h"

ClientControl::ClientControl(const char *ipAddress, int portN)
{
    this->server = NULL;
    this->ipAddress = (char *)ipAddress;
    this->ip.fromString(ipAddress);
    this->portNumber = -1;
    this->ServerControl = false;
    this->cl = NULL;
    this->portNumber = portN;
}
void ClientControl::GetClientIp(char * clientIP){
    // empty buffer 
    memset(clientIP, 0, sizeof(clientIP));

    // change ip address back to string 
    strcpy(clientIP, (const char*)this->ipAddress);
}

void ClientControl::SetUpEthernet()
{
    IPAddress gateway(0, 0, 0, 0);
    IPAddress subnet(255, 255, 255, 0);
    byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x1E, 0x51};
    EthernetServer serverAddress = EthernetServer(this->portNumber);
    this->server = &serverAddress;
    Ethernet.begin(mac, this->ip) ;
    this->server->begin();
    this->ServerControl = true;
}


bool ClientControl::CheckConnection(){
    return this->ServerControl;
}


EthernetClient * ClientControl::GetEthernetClient(){
    return this->cl;
}


int ClientControl::GetPortNumber(){
    return this->portNumber;
}

ClientControl::~ClientControl()
{
    
}
