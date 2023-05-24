#include <Arduino.h>
#include <Controllino.h>

void setup() {
  pinMode(CONTROLLINO_D4, OUTPUT); // next to the digital output screw termina
}

void loop() {
  // put your main code here, to run repeatedly:
 digitalWrite(CONTROLLINO_D4, HIGH); // turn the LED on (HIGH is the voltage level)
 delay(1000);                         // wait for 100 milliseconds which is 1/10 of a second 
 digitalWrite(CONTROLLINO_D4, LOW);  // turn the LED off by making the voltage LOW
 delay(300);
}

