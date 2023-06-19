#ifndef __RS485CONTROL_H
#define __RS485CONTROL_H
#include <stdint.h>
#include "Arduino.h"

/*
    This class is responsible of handling RS485 communication 
*/
class Rs485Control
{
    private:
        bool serialControl;
        Stream * Ser;
        boolean newData;
        byte ndx;
        char endMarker;
        char startMarker;
    public: 
        Rs485Control();
        ~Rs485Control();
        void Init(Stream * stream);
        void SetRs485Pin(uint8_t pin, uint8_t mode); 
        void pinControl(int piNumber, uint8_t val);
        bool SendMessage(char * message); 
        bool GetMessage(char * buffer);
        bool CheckInitialization();
    

};

#endif