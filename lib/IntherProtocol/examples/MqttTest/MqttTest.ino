#include <Controllino.h>
#include <Controller.h>
#define AIO_SERVER "192.168.127.1"
#define AIO_SERVERPORT 1883
#define IP "192.168.127.3"
#define QOS 0
char *LEDSTRIPTOPIC = "/ledstrip";
char *PICKLIGHTTOPIC = "/picklight";
char *RESPSTATUS = "/status";
char *PUBTOPIC = "/picklight/pub";
char *DeviceID = "1000";
// #define ARRAY_size 256
char Mqttbuff[MESSAGE_LENGTH] = {0};
char buffer[MESSAGE_LENGTH] = {0};

Controller ctrl = Controller(IP, AIO_SERVER, AIO_SERVERPORT);
auto picklightTest = ctrl.GetSubscribe(PICKLIGHTTOPIC, QOS);
auto ledstripTest2 = ctrl.GetSubscribe(LEDSTRIPTOPIC, QOS);

#define PICKLIGHT 1 // Control flag for picklight
#define LEDSTRIP 1  // Control flag for LEDSTRIP CONTROLLER
#define DEBUG 1     // Control flag for DEBUG

#if LEDSTRIP
#include <Adafruit_NeoPixel.h>
#define NUM_LED 500
#define PIN 4
#define DELAY_DURATION 100
#endif

String jsonResp = "";
char Resp[] = "{\"status\":\"true\"}";
void SendStatus(bool statusFlag);
bool LED_Control(int starLed, int endLed, int r, int g, int b);

#if LEDSTRIP
  Adafruit_NeoPixel ledstrip = Adafruit_NeoPixel(NUM_LED, PIN, NEO_GRB + NEO_KHZ800);
  int r = 0;
  int g = 0;
  int b = 0;
  int start = 0;
  int endIndex = 0;
#endif

void setup()
{
  Serial.begin(USART_BAUDRATE);
  Serial3.begin(USART_BAUDRATE);

  while (!Serial)
  {
  }

#if DEBUG
  Serial.println("ready");
#endif

  if (ctrl.Init(CONTROLLINO_RS485_nRE, CONTROLLINO_RS485_DE, (&Serial3)))
  {

#if DEBUG
    Serial.println("Ethernet, RS485, MQTT are connected");
#endif
  }
  else
  {

#if DEBUG
    Serial.println("Problem in connecting Ethernet, RS485 and MQTT");
#endif
  }
  ctrl.MqttSubscribe(&picklightTest);
  ctrl.MqttSubscribe(&ledstripTest2);

#if LEDSTRIP
  ledstrip.begin();
#endif
}

void loop()
{
  if (!ctrl.MqttConnect())
  {
#if DEBUG
    Serial.println("problem in connection ");
#endif
  }
  else
  {
#if DEBUG
    Serial.println("MQTT connection is ready  ");
#endif
  }

#if LEDSTRIP
  if (ctrl.GetMessage(&ledstripTest2, Mqttbuff, LEDSTRIPTOPIC))
  {
    SendStatus(true);
#if DEBUG
    Serial.println(Mqttbuff);
#endif
    // these are hardcorded values please make sure you check the returns
    // to avoid issues
    r = ctrl.GetInt(Mqttbuff, "r");
    g = ctrl.GetInt(Mqttbuff, "g");
    b = ctrl.GetInt(Mqttbuff, "b");
    start = ctrl.GetInt(Mqttbuff, "start");
    endIndex = ctrl.GetInt(Mqttbuff, "end");
#if DEBUG
    Serial.println(r);
    Serial.println(g);
    Serial.println(b);
    Serial.println("LEDSTRIP");
#endif
    LED_Control(start, endIndex, r, g, b);
    memset(Mqttbuff, 0, sizeof(Mqttbuff));
  }
#endif

#if PICKLIGHT
  // send message
  if (ctrl.GetMessage(&picklightTest, Mqttbuff, PICKLIGHTTOPIC) == true)
  {
    // send message to pick light
    if (ctrl.SendRS485Message(Mqttbuff) == true)
    {
      SendStatus(true);
    }
    memset(Mqttbuff, 0, sizeof(Mqttbuff));
  }

  // send pick light data
  if (ctrl.ReceiveRs485Data(Mqttbuff) == true)
  {
    if (ctrl.SendMessage(Mqttbuff, PUBTOPIC, QOS) == true)
    {
#if DEBUG
      Serial.println(" sent message to the broker ");
      Serial.println(Mqttbuff);
#endif
    }
    // Clear buffer
    memset(Mqttbuff, 0, sizeof(Mqttbuff));
  }
#endif
}

void SendStatus(bool statusFlag)
{
  if (statusFlag == true)
  {
    if (ctrl.SendMessage(Resp, RESPSTATUS, QOS))
    {
#if DEBUG
      Serial.println("sent response to the broker");
#endif
    }
  }
}
bool LED_Control(int starLed, int endLed, int r, int g, int b)
{
#if LEDSTRIP
  ledstrip.clear();
  for (int i = starLed; i < endLed; i++)
  {
    ledstrip.setPixelColor(i, ledstrip.Color(r, g, b));
    ledstrip.show();
    delay(DELAY_DURATION);
  }
#endif
  return true;
}