#include "Rs485Control.h"

Rs485Control::Rs485Control()
{
    this->Ser = NULL;
    this->serialControl = false;
    this->newData = false;
    this->ndx = 0;
    this->endMarker = '}';
    this->startMarker = '{';
}

void Rs485Control::Init(Stream * stream)
{
    while (!(&(*stream)))
    {
    }
    this->Ser = stream;

    if(this->Ser != NULL){
        this->serialControl = true;
    }
}



void Rs485Control::SetRs485Pin(uint8_t pin, uint8_t mode){
    pinMode(pin, mode);
}

void Rs485Control::pinControl(int piNumber, uint8_t val)
{
    digitalWrite(piNumber, val);
}

bool Rs485Control::SendMessage(char *message)
{
    if (this->serialControl == true)
    {
        const char * mess = (const char * )(message);
        Ser->write(mess);
        Ser->flush();
        return true;
    }
    return false;
}


bool Rs485Control::GetMessage(char *buffer)
{
    char rc;
    if (this->serialControl == true)
    {
        while (this->Ser->available() > 0)
        {
            rc = Ser->read();
            if (rc == startMarker)
            {
                this->ndx = 0;
                memset(buffer, 0, sizeof(buffer));
            }

            if (rc == endMarker)
            {
                buffer[this->ndx] = rc;
                this->ndx++;
                this->newData = true;
                break;
            }else{
                buffer[this->ndx] = rc;
                this->ndx++;
            }
        }

        if (this->newData == true)
        {
            // clear everything after the message to avoid overwrighting 
            //the actual  message 
            for(int i = ndx; i < sizeof(buffer); i++){
                buffer[this->ndx] = 0;
            }
            this->ndx = 0;
            this->newData = false;
            int messLen = strlen(buffer);
            if (messLen > 0)
            {
                return true;
            }
        }
    }
    return false;
}


bool Rs485Control::CheckInitialization(){
    return this->serialControl;
}


Rs485Control::~Rs485Control()
{
}