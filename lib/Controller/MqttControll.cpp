#include "MqttControl.h"

MqttControl::MqttControl()
{   
    this->mqttPtr = NULL;
    this->connectionControl = false;
    this->newData = false;
    this->ndx = 0;
    this->endMarker = '}';
}

MqttControl::~MqttControl()
{
}



bool MqttControl::Init(EthernetClient * ethclient, const char * mqttServer, int mqttPort){
    bool resp = true;
    Adafruit_MQTT_Client cl = Adafruit_MQTT_Client(ethclient, mqttServer, mqttPort, "IntherProtocol");
    this->mqttPtr =  (&cl);
    if(!this->mqttPtr->ping()){
        this->connectionControl = false;
        resp = false; 
        return resp;
    }
    this->connectionControl = true;
    return resp; 
}



Adafruit_MQTT_Subscribe MqttControl::SetGetSubscribe(char *topic)
{
    Adafruit_MQTT_Subscribe resp = Adafruit_MQTT_Subscribe(this->mqttPtr, (const char *)topic);
    Subscribe(&resp);
    return resp;
}

bool MqttControl::PublishMqttMessage(char *topic, char *message, int qos)
{
    const char *top = (const char *)topic;
    const char *payload = (const char *)message;
    if (this->mqttPtr->publish(top, payload, qos))
    {
        return true;
    }
    return false;
}

bool MqttControl::GetMessage(Adafruit_MQTT_Subscribe *subName, char * buffer)
{
    // clear buffer first 
    memset(buffer, 0, sizeof(buffer));
    Adafruit_MQTT_Subscribe *subscription;
    while ((subscription = this->mqttPtr->readSubscription(1000)))
    {
        if (subscription == &(*subName))
        {
            strcpy(buffer,  (char *)(subName->lastread));
            int messLen = strlen(buffer);
            if (messLen > 0)
            {
                return true;
            }
        }
    }
    return false;
}


bool MqttControl::CheckConnection()
{
    return (this->connectionControl);
}

bool MqttControl::MqttConnect()
{
    int8_t ret;
    if (this->mqttPtr->connected())
    {
        return true;
    }
    while ((ret = this->mqttPtr->connect()) != 0)
    {
        this->mqttPtr->disconnect();
        return false;
    }
   return true;
}



Adafruit_MQTT_Client * MqttControl::GetMqttClient(){
    return this->mqttPtr;
}


void MqttControl::Subscribe(Adafruit_MQTT_Subscribe *subName){
    this->mqttPtr->subscribe(&(*subName));
}