#include <stdint.h>
#include "DataControl.h"
#include "MqttControl.h"
#include "Rs485Control.h"
#include "ClientControl.h"

namespace IntherProtcol
{
/////////////////////////////////////
// Global variable
/////////////////////////////////////

///////////////////////////////////////
// USART CONFIGURATION
///////////////////////////////////////
#define USART_BAUDRATE 115200 //  BAUD RATE bps
#define MAX_QOS 2             //  BAUD RATE bps


    //////////////////////////////////////////
    ///////// Inherit Data control to use it on pick light device
    ////////  to avoid writing more JSON code
    /////////////////////////////////////////

    class Controller
    {
    private:
        Rs485Control rs;
        MqttControl mqt;
        ClientControl *cl;
        bool connectionControl;
        DataControl dt;

        // MQTT FUNCTION
        bool GetMqttPayload(Adafruit_MQTT_Subscribe *subName, char *bufferReturn);
        Adafruit_MQTT_Client *mqtt;

        // rs485
        void RsInit();

        int rsNre;
        int rsDe;
        Stream *stream;
        bool EthernetSetup();

        // CONTROLLER FUNCTION
        void ClearBuffer(char *buffer);
        bool checkSizePayload(char *buffer);

        Adafruit_MQTT_Subscribe *sub;
        char *ServerPort;
        bool PickLightController;

    public:
        Controller(const char *clientIp, const char *ServerPort, int MqttPort);

        ~Controller();
        bool Init(int nre, int de, Stream *stream);
        bool SendMessage(char *payload, char *topic, int qos);
        bool GetMessage(Adafruit_MQTT_Subscribe *subName, char *buffer, char *topic);
        bool MqttConnect();
        void MqttSubscribe(Adafruit_MQTT_Subscribe *subName);
        Adafruit_MQTT_Client *GetMqttClient();
        bool SendRS485Message(char *message);
        bool ReceiveRs485Data(char *bufferReturn);
        Adafruit_MQTT_Subscribe GetSubscribe(char *topic, int qos);

        bool ValidatePayload(char *buffer);

        // JSON
        bool GetArray(char *bufferesp, char *message, char *keyword);
        bool GetString(char *bufferesp, char *message, char *keyword);
        int GetInt(char *message, char *keyword);
    };
}