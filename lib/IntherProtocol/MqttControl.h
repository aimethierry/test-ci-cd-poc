#ifndef MQTTCONTROL_H
#define MQTTCONTROL_H
#include <stdint.h>
// #include "Arduino.h"
#include <Adafruit_MQTT_Client.h>
#include <SPI.h>
#include <Ethernet.h>


class MqttControl
{
private:
    boolean newData;
    byte ndx;
    int portNumber;
    bool connectionControl;
    Adafruit_MQTT_Client * mqttPtr;
    

public:
    MqttControl();
    ~MqttControl();
    Adafruit_MQTT_Subscribe SetGetSubscribe(char * topic);
    bool PublishMqttMessage(char * topic, char * message, int qos);
    bool GetMessage(Adafruit_MQTT_Subscribe *subName, char * buffer);
    
    bool Init(EthernetClient * ethclient, const char * mqttServer, int mqttPort);
    bool CheckConnection();
    bool MqttConnect();
    Adafruit_MQTT_Client * GetMqttClient();
    void Subscribe(Adafruit_MQTT_Subscribe *subName);
};

#endif