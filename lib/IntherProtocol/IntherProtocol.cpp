#include "IntherProtocol.h"
using namespace IntherProtcol;


IntherProtcol::Controller::Controller(const char *clientIp, const char *ServerPort, int MqttPort, const char * clientName)
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
    this->clientName = (char *)clientName;
    // this->sub = Adafruit_MQTT_Subscribe()
}

////////////////////////////////////////////
// init pin  for rs485
// setup init for ethernet
// connect to mqtt broker
////////////////////////////////////////////
bool IntherProtcol::Controller::Init(int nre, int de, Stream *stream)
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
        this->mqt.Init(&ethClient, (const char *)(this->ServerPort), PORT, (const char *)this->clientName);
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
bool IntherProtcol::Controller::SendMessage(char *payload, char *topic, int qos)
{
    bool resp = false;
    // send JSON message
    if (this->dt.ValidateJsonMessage((payload)) == true && qos <= MAX_QOS)
    {
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
bool IntherProtcol::Controller::GetMessage(Adafruit_MQTT_Subscribe *subName, char *bufferData, char *topic)
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

void IntherProtcol::Controller::MqttSubscribe(Adafruit_MQTT_Subscribe *subName)
{
    this->mqt.Subscribe(subName);
}

void IntherProtcol::Controller::ClearBuffer(char *bufferArray)
{
    memset(bufferArray, 0, sizeof(bufferArray));
}

////////////////////////////////////////////
// check payload size
////////////////////////////////////////////
bool IntherProtcol::Controller::checkSizePayload(char *buffer)
{
    int payloadLen = this->dt.GetPayloadSize(buffer);

    if (payloadLen <= MESSAGE_LENGTH && payloadLen != -1)
    {
        return true;
    }

    return false;
}

bool IntherProtcol::Controller::ValidatePayload(char *buffer)
{
    return this->dt.ValidateJsonMessage((buffer));
}
bool IntherProtcol::Controller::GetMqttPayload(Adafruit_MQTT_Subscribe *subName, char *bufferData)
{
    return (this->mqt.GetMessage(subName, bufferData));
}

void IntherProtcol::Controller::RsInit()
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

bool IntherProtcol::Controller::EthernetSetup()
{
    this->cl->SetUpEthernet();
    if (this->cl->CheckConnection())
    {
        return true;
    }
    return false;
}

bool IntherProtcol::Controller::SendRS485Message(char *message)
{
    bool resp = false;
    this->rs.pinControl(this->rsNre, HIGH);
    this->rs.pinControl(this->rsDe, HIGH);
    resp = this->rs.SendMessage(message);
    this->rs.pinControl(this->rsDe, LOW);
    this->rs.pinControl(this->rsNre, LOW);
    return resp;
}

bool IntherProtcol::Controller::ReceiveRs485Data(char *bufferReturn)
{
    bool resp = false;

    if (this->rs.GetMessage(bufferReturn))
    {
        // check if message received is json
        if (this->dt.ValidateJsonMessage(bufferReturn))
        {
            resp = true;
        }
    }
    return resp;
}

IntherProtcol::Controller::~Controller()
{
    delete (this->cl);
}

Adafruit_MQTT_Client *Controller::GetMqttClient()
{
    return (this->mqt.GetMqttClient());
}

Adafruit_MQTT_Subscribe Controller::GetSubscribe(char *topic, int qos)
{
    int Subqos = qos;
    if (Subqos > MAX_QOS)
    {
        Subqos = 0;
    }
    return (Adafruit_MQTT_Subscribe(this->mqtt, (const char *)topic, Subqos));
}


/*
    This part is used to control if the connection to the broker 
    is set correctly, if return is false check the wiring or check the broker 
    ip address and the port if are corrct 
*/
bool Controller::MqttConnect()
{
    return (this->mqt.MqttConnect());
}

bool Controller::GetArray(char *bufferesp, char *message, char *keyword)
{
    return this->dt.GetArray(bufferesp, message, keyword);
}
bool Controller::GetString(char *bufferesp, char *message, char *keyword)
{
    return this->dt.GetString(bufferesp, message, keyword);
}
int Controller::GetInt(char *message, char *keyword)
{
    return this->dt.GetInt(message, keyword);
}