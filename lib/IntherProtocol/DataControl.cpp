#include "DataControl.h"

/*
   In implementation of ArduinoJSON avoid using doc as public
   this is because it creates memory leackage
*/
DataControl::DataControl()
{
}

bool DataControl::GetArray(char *bufferesp, char *message, char *keyword)
{
    bool resp = false;
    String mess = String(message);
    String key = String(keyword);
    StaticJsonDocument<MESSAGE_LENGTH> doc;
    auto error = deserializeJson(doc, mess);
    if (error)
    {
        return resp;
    }
    else
    {
        JsonArray arr = doc[key].as<JsonArray>();
        char JsonData[MESSAGE_LENGTH];
        serializeJson(arr, JsonData);

        String val = String(JsonData);
        if (val != "null")
        {
            strcpy(bufferesp, JsonData);
            int bufflen = strlen(bufferesp);
            if (bufflen > 0)
            {
                resp = true;
            }
        }
    }
    return resp;
}

bool DataControl::GetString(char * bufferesp, char *message, char *keyword)
{
    bool resp = false;
    String mess = String(message);
    String key = String(keyword);
    StaticJsonDocument<MESSAGE_LENGTH> doc;
    auto error = deserializeJson(doc, mess);
    if (error)
    {
        return resp;
    }
    else
    {
        String value = doc[key].as<String>();
        const char *char_resp = value.c_str();
        if (value != "null")
        {
            strcpy(bufferesp, char_resp);
            int bufflen = strlen(bufferesp);
            if (bufflen > 0)
            {
                resp = true;
            }
        }
    }
    return resp;
}
int DataControl::GetInt(char *message, char *keyword)
{
    int resp = -1;
    String mess = String(message);
    String key = String(keyword);
    StaticJsonDocument<MESSAGE_LENGTH> doc;
    auto error = deserializeJson(doc, mess);
    if (error)
    {
        return resp;
    }
    else
    {
        int value = doc[key].as<int>();
        resp = value;
        return resp;
    }
}
bool DataControl::ValidateJsonMessage(char *message)
{
    StaticJsonDocument<MESSAGE_LENGTH> doc;
    String mess = String(message);
    auto error = deserializeJson(doc, mess);
    if (error)
    {
        return false;
    }
    return true;
}
int DataControl::GetPayloadSize(char *payload)
{
    int resp = -1; 
    int length = strlen(payload);
    if (length > 0){
        resp = length;
        return resp;
    }
    return resp;
}


DataControl::~DataControl()
{
}