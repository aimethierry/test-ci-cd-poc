#include <Arduino.h>
#include <unity.h>
#include <Controllino.h>
#include <IntherProtocol.h>

#define AIO_SERVER "192.168.127.1"
#define AIO_SERVERPORT 1883
#define IP "192.168.127.3"
#define QOS 0

char *PUBTESTTOPIC = "/test/pub";


// #define ARRAY_size 256
char Mqttbuff[MESSAGE_LENGTH] = {0};
char buffer[MESSAGE_LENGTH] = {0};


char * JsonTestMessage = "{\"status\":\"unit test\"}"; 

// include Intherprotocol namespace
using namespace IntherProtcol;
IntherProtcol::Controller ctrl = IntherProtcol::Controller(IP, AIO_SERVER, AIO_SERVERPORT);




void tearDown(void)
{
  // clean stuff up here
}

void test_led_builtin_pin_number(void)
{
  TEST_ASSERT_EQUAL(CONTROLLINO_D4, CONTROLLINO_D4);
}

void test_led_state_high(void)
{
  digitalWrite(CONTROLLINO_D4, HIGH);
  TEST_ASSERT_EQUAL(HIGH, digitalRead(CONTROLLINO_D4));
}

void test_led_state_low(void)
{
  digitalWrite(CONTROLLINO_D4, LOW);
  TEST_ASSERT_EQUAL(LOW, digitalRead(CONTROLLINO_D4));
}

void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);
  pinMode(CONTROLLINO_D4, OUTPUT);
   Serial.begin(USART_BAUDRATE);
  Serial3.begin(USART_BAUDRATE);

  while (!Serial)
  {
  }

  ctrl.Init(CONTROLLINO_RS485_nRE, CONTROLLINO_RS485_DE, &(Serial3));
  UNITY_BEGIN(); // IMPORTANT LINE!
  RUN_TEST(test_led_builtin_pin_number);
}

uint8_t i = 0;
uint8_t max_blinks = 5;

void loop()
{
  if (i < max_blinks)
  {
    if (!ctrl.MqttConnect())
    {
      RUN_TEST(test_led_state_high);
      delay(100);
      RUN_TEST(test_led_state_low);
      delay(100);
      i++;
    }
    else
    {
      RUN_TEST(test_led_state_high);
      delay(100);
      ctrl.SendMessage(JsonTestMessage, PUBTESTTOPIC, QOS);
      UNITY_END();
    }
  }
  else if (i == max_blinks)
  {
    UNITY_END(); // stop unit testing
  }

  

}