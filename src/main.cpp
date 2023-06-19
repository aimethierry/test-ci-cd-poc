#include <Controllino.h>
#include <IntherProtocol.h>

#define AIO_SERVER "192.168.127.1"
#define AIO_SERVERPORT 1883
#define IP "192.168.127.3"
#define QOS 0
char *LEDSTRIPTOPIC = "/ledstrip";
char *PICKLIGHTTOPIC = "/picklight";
char *RESPSTATUS = "/status";
char *PUBTOPIC = "/picklight/pub";
char *JsonRespKey = "status";
char *JsonRespval = "true";

// #define ARRAY_size 256
char Mqttbuff[MESSAGE_LENGTH] = {0};
char buffer[MESSAGE_LENGTH] = {0};

// include Intherprotocol namespace
using namespace IntherProtcol;
IntherProtcol::Controller ctrl = IntherProtcol::Controller(IP, AIO_SERVER, AIO_SERVERPORT);
auto picklightTest = ctrl.GetSubscribe(PICKLIGHTTOPIC, QOS);
auto ledstripTest2 = ctrl.GetSubscribe(LEDSTRIPTOPIC, QOS);

#define PICKLIGHT 1 // Control flag for picklight
#define LEDSTRIP 1  // Control flag for LEDSTRIP CONTROLLER
#define DEBUG 1     // Control flag for DEBUG


#if LEDSTRIP
#include <FastLED.h>
#define NUM_LED 100
#define PIN 4
#define DELAY_DURATION 100
#endif


char Resp[] = "{\"status\":\"true\"}";

bool LED_Control(int starLed, int endLed, int r, int g, int b);
void cleanBuffer(char *message);
void LEDstripController();
void PickLightController();

// this function is used for dbuggin to knowthe cause of device rest
void DeviceReset();

// display logs
enum DebugLevel
{
  DEVICERESET,
  CONNECTION,
  LEDSTRIPDEBUG,
  PICKLIGHTDEBUG,
  ADDITIONAL
};

void DisplayLogs(DebugLevel Level, char *message);

#if LEDSTRIP
CRGB leds[NUM_LED];

int r = 0;
int g = 0;
int b = 0;
int start = 0;
int endIndex = 0;
#endif

void setup()
{
  DebugLevel level = CONNECTION;
  Serial.begin(USART_BAUDRATE);
  Serial3.begin(USART_BAUDRATE);

  while (!Serial)
  {
  }
  DisplayLogs(level, "setup ");

  if (ctrl.Init(CONTROLLINO_RS485_nRE, CONTROLLINO_RS485_DE, (&Serial3)))
  {
    DisplayLogs(level, "Ethernet, RS485, MQTT are connected");
  }
  else
  {
    DisplayLogs(level, "Problem in connecting Ethernet, RS485 and MQTT");
  }
  ctrl.MqttSubscribe(&picklightTest);
  ctrl.MqttSubscribe(&ledstripTest2);

#if LEDSTRIP
  FastLED.addLeds<WS2812, PIN, GRB>(leds, NUM_LED);
#endif

  DeviceReset();

  // clear everything in the buffer 
  memset(Mqttbuff, 0, sizeof(Mqttbuff));
  memset(buffer, 0, sizeof(buffer));
}

void loop()
{

  /*
    This part is used to control if the connection to the broker
    is set correctly, if the response shows the connection that there
    is a problem in connection please check if the wiring are correct
    or check if you are connecting to the right ip address of the broker
  */
  if (!ctrl.MqttConnect())
  {
    DebugLevel level = CONNECTION;
    DisplayLogs(level, "problem in connection ");
  }
  else
  {
    DebugLevel level = CONNECTION;
    DisplayLogs(level, "MQTT connection is ready  ");
  }

  LEDstripController();

#if PICKLIGHT
  DebugLevel level = PICKLIGHTDEBUG;
  // send message
  if (ctrl.GetMessage(&picklightTest, Mqttbuff, PICKLIGHTTOPIC) == true)
  {

    DisplayLogs(level, "pick light buffer is");
    DisplayLogs(level, Mqttbuff); 
    ctrl.SendRS485Message(Mqttbuff);
    memset(Mqttbuff, 0, sizeof(Mqttbuff));
  }

  // check if the received message is the response from pick light
  // send the response to the broker
  // if not the response. i.e its the normal message that pick light devices
  // want to send to the broker
  if (ctrl.ReceiveRs485Data(Mqttbuff) == true)
  {
    // check if the message received is pick light response
    if (ctrl.GetString(buffer, Mqttbuff, JsonRespKey))
    {
      if (strcmp((const char *)buffer, (const char *)JsonRespval) == 0)
      {
        DisplayLogs(level, "pick light received message ");
        if (ctrl.SendMessage(Mqttbuff, RESPSTATUS, QOS))
        {
          DisplayLogs(level, "send response to the server");
        }
      }
      else
      {
        DisplayLogs(level, "pick light device did not receive the message");
      }

      // clear the buffer 
      memset(buffer, 0, sizeof(buffer));
    }
    else
    {
      // send the normal message that pick light sent
      if (ctrl.SendMessage(Mqttbuff, PUBTOPIC, QOS) == true)
      {
        DisplayLogs(level, " sent pick light message to the broker ");
        DisplayLogs(level, Mqttbuff);
      }

      memset(Mqttbuff, 0, sizeof(Mqttbuff));
    }
  }
#endif
}

/*
  This part is used to command LED strip to turn change colors according to the message received
*/
void LEDstripController()
{
  DebugLevel level = LEDSTRIPDEBUG;
#if LEDSTRIP
  if (ctrl.GetMessage(&ledstripTest2, Mqttbuff, LEDSTRIPTOPIC))
  {
    // send response message to the broker
    DisplayLogs(level, "Received message is ");
    DisplayLogs(level, Mqttbuff);

    // these are hardcorded values please make sure you check the returns
    // to avoid issues
    r = ctrl.GetInt(Mqttbuff, "r");
    g = ctrl.GetInt(Mqttbuff, "g");
    b = ctrl.GetInt(Mqttbuff, "b");
    start = ctrl.GetInt(Mqttbuff, "start");
    endIndex = ctrl.GetInt(Mqttbuff, "end");

    
    itoa(r, buffer, 10);
    DisplayLogs(level, "red value is ");
    DisplayLogs(level, buffer);

    itoa(g, buffer, 10);
    DisplayLogs(level, "green value is ");
    DisplayLogs(level, buffer);

    itoa(b, buffer, 10);
    DisplayLogs(level, "blue value is ");
    DisplayLogs(level, buffer);

    // send status to the broker when the LED strip is done showing colors
    if (LED_Control(start, endIndex, r, g, b))
    {
      if (ctrl.SendMessage(Resp, RESPSTATUS, QOS))
      {
        DisplayLogs(level, "Send status to the server");
      }
    }
    // clean the buffer 
    memset(Mqttbuff, 0, sizeof(Mqttbuff));
    memset(buffer, 0, sizeof(buffer));
  }
#endif
}


bool LED_Control(int starLed, int endLed, int r, int g, int b)
{
bool resp = false;
#if LEDSTRIP
  FastLED.clear();
  if(endLed < NUM_LED){
    for (int i = starLed; i < endLed; i++)
    {
      leds[i] = CRGB(r, g, b);
    }
    FastLED.show();
    resp = true;
  }
#endif
  return resp;
}

void PickLightController()
{
}

// clean the buffer
void cleanBuffer(char *message)
{
  memset(message, 0, sizeof(message));
}

void DeviceReset()
{
  DebugLevel level = DEVICERESET;
#if LEDSTRIP
  if (MCUSR & (1 << PORF))
  { // Power-on Reset
    DisplayLogs(level, "Cause of Reset = Power-on Reset");
    Serial.println();
    leds[0] = CRGB::Blue;
    FastLED.show();
  }
  else if (MCUSR & (1 << EXTRF))
  { // External Reset
    DisplayLogs(level, "Cause of Reset = External Reset");
    leds[0] = CRGB::Red;
    FastLED.show();
  }
  else if (MCUSR & (1 << BORF))
  { // Brown-Out Reset
    DisplayLogs(level, "Cause of Reset = Brown-Out Reset");
    leds[0] = CRGB::Brown;
    FastLED.show();
  }
  else if (MCUSR & (1 << WDRF))
  { // Watchdog Reset
    DisplayLogs(level, "Cause of Reset = Watchdog Reset");
    leds[0] = CRGB::Green;
    FastLED.show();
  }
  MCUSR = 0;
#endif
}

void DisplayLogs(DebugLevel Level, char *message)
{
#if DEBUG
  if (message != NULL)
  {
    switch (Level)
    {
    case DEVICERESET:
      Serial.println("\n Device Reset Debug: ");
      Serial.print("MCUSR: ");
      Serial.println(MCUSR);
      Serial.println("MCUSR result is ");
      Serial.println(message);
      break;

    case CONNECTION:
      Serial.println("\n Connection debug : ");
      Serial.println(message);
      break;

    case LEDSTRIPDEBUG:
      Serial.println("\n LED strip debug");
      Serial.println(message);
      break;

    case PICKLIGHTDEBUG:
      Serial.println("\n PICKLIGHTDEBUG debug");
      Serial.println(message);
      break;

    default:
      // Serial.println("PICKLIGHTDEBUG debug");
      Serial.println();
      Serial.println(message);
      break;
    }
  }
#endif
}