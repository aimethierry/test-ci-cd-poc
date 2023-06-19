#ifndef DATACONTROL_H
#define DATACONTROL_H
#include <stdint.h>
#include <Arduino.h>
#include <String.h>
#include <ArduinoJson.h>
#define MESSAGE_LENGTH 256


/*
    For this project we are using ArduinoJson 6.21.2
*/
class DataControl 
{
public:
    DataControl();
    ~DataControl();
    bool GetArray(char  * bufferesp, char * message, char * keyword);

    bool GetString(char bufferesp[], char * message, char * keyword);
    int GetInt(char * message, char * keyword);
    bool ValidateJsonMessage(char * message);
    int GetPayloadSize(char * payload);
};

#endif