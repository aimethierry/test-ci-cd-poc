#include <Arduino.h>
#include <unity.h>
#include <Controllino.h>


void setUp(void)
{
  // set stuff up here
}

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

  UNITY_BEGIN(); // IMPORTANT LINE!
  RUN_TEST(test_led_builtin_pin_number);
}

uint8_t i = 0;
uint8_t max_blinks = 5;

void loop()
{
  if (i < max_blinks)
  {
    RUN_TEST(test_led_state_high);
    delay(500);
    RUN_TEST(test_led_state_low);
    delay(500);
    i++;
  }
  else if (i == max_blinks)
  {
    UNITY_END(); // stop unit testing
  }
}