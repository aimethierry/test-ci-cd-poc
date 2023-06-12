#include "Controller.h"

Controller::Controller(const char *clientIp, const char *ServerPort, int MqttPort)
{
    this->cl = new ClientControl(clientIp, MqttPort);
    this->rs = Rs485Control();
    this->mqt = MqttControl();
    this->dt = DataControl();
    this->connectionControl = false;
    this->rsDe = -1;
    this->rsNre = -1;
    this->stream = NULL;
    this->ServerPort = (char *)ServerPort;
    this->PickLightController = false;
    this->sub = NULL;
    // this->sub = Adafruit_MQTT_Subscribe()
}


////////////////////////////////////////////
// init pin  for rs485
// setup init for ethernet
// connect to mqtt broker
////////////////////////////////////////////
bool Controller::Init(int nre, int de, Stream *stream)
{
    bool resp = false;
    // setup RS485
    this->rsDe = de;
    this->rsNre = nre;
    this->stream = stream;

    // SETUP Ethernet
    if (EthernetSetup())
    {
        
        // setup RS485
        RsInit();
        // check if you can change this to use the one for clientControl
        EthernetClient ethClient;

        int PORT = this->cl->GetPortNumber();
        this->mqt.Init(&ethClient, (const char *)(this->ServerPort), PORT);
        if (this->mqt.CheckConnection())
        {
            resp = true;
            return resp;
        }
    }
    return false;
}

////////////////////////////////////////////
// FUNCTION THAT SEND MESSAGE TO THE BROKER
////////////////////////////////////////////
bool Controller::SendMessage(char *payload, char *topic, int qos)
{
    bool resp = false;
    // send JSON message
    if (this->dt.ValidateJsonMessage((payload)) == true && qos <= MAX_QOS){
        if (this->mqt.PublishMqttMessage(topic, payload, qos))
        {
            resp = true;
            return resp;
        }
    }
    return resp;
}

////////////////////////////////////////////
// FUNCTION THAT GET MESSAGE TO THE BROKER
////////////////////////////////////////////
bool Controller::GetMessage(Adafruit_MQTT_Subscribe *subName, char *bufferData, char *topic)
{
    bool resp = false;
    // clear Buffer to avoid overwriting on existing message
    ClearBuffer(bufferData);
    if (this->mqt.GetMessage(subName, bufferData))
    {
        if (this->dt.ValidateJsonMessage((bufferData)) && checkSizePayload(bufferData))
        {
            resp = true;
            return resp;
        }
    }
    return resp;
}

void Controller::MqttSubscribe(Adafruit_MQTT_Subscribe *subName)
{
    this->mqt.Subscribe(subName);
}

void Controller::ClearBuffer(char *bufferArray)
{
    memset(bufferArray, 0, sizeof(bufferArray));
}

////////////////////////////////////////////
// check payload size
////////////////////////////////////////////
bool Controller::checkSizePayload(char *buffer)
{
    int payloadLen = this->dt.GetPayloadSize(buffer);

    if (payloadLen <= MESSAGE_LENGTH && payloadLen != -1)
    {
        return true;
    }

    return false;
}

bool Controller::ValidatePayload(char *buffer)
{
    return this->dt.ValidateJsonMessage((buffer));
}
bool Controller::GetMqttPayload(Adafruit_MQTT_Subscribe *subName, char *bufferData)
{
    return (this->mqt.GetMessage(subName, bufferData));
}

void Controller::RsInit()
{
    this->rs.Init(this->stream);
    // check if serial data is assigned before doing anything else
    if (this->rs.CheckInitialization())
    {
        this->rs.SetRs485Pin(this->rsNre, OUTPUT);
        this->rs.SetRs485Pin(this->rsDe, OUTPUT);
        this->rs.pinControl(this->rsDe, LOW);
        this->rs.pinControl(this->rsNre, LOW);
    }
}

bool Controller::EthernetSetup()
{
    this->cl->SetUpEthernet();
    if (this->cl->CheckConnection())
    {
        return true;
    }
    return false;
}

bool Controller::SendRS485Message(char *message)
{
    bool resp = false;
    this->rs.pinControl(this->rsNre, HIGH);
    this->rs.pinControl(this->rsDe, HIGH);
    resp = this->rs.SendMessage(message);
    this->rs.pinControl(this->rsDe, LOW);
    this->rs.pinControl(this->rsNre, LOW);
    return resp;
}

bool Controller::ReceiveRs485Data(char *bufferReturn)
{
    bool resp = false;
    // every message received must be sent again
    if (this->rs.GetMessage(bufferReturn))
    {
        resp = this->rs.SendMessage(bufferReturn);
    }
    return resp;
}
Controller::~Controller()
{
    delete (this->cl);
}

Adafruit_MQTT_Client *Controller::GetMqttClient()
{
    return (this->mqt.GetMqttClient());
}


Adafruit_MQTT_Subscribe Controller::GetSubscribe(char * topic, int qos){
    int Subqos = qos;
    if(Subqos > MAX_QOS){
        Subqos = 0;
    }
    return (Adafruit_MQTT_Subscribe(this->mqtt, (const char *)topic, Subqos));
}

bool Controller::MqttConnect()
{
    return (this->mqt.MqttConnect());
}

bool Controller::GetArray(char  * bufferesp, char * message, char * keyword){
    return this->dt.GetArray(bufferesp, message, keyword);
}
bool Controller::GetString(char * bufferesp, char * message, char * keyword){
    return  this->dt.GetString(bufferesp, message, keyword);
}
int  Controller::GetInt(char * message, char * keyword){
    return this->dt.GetInt(message, keyword);
}