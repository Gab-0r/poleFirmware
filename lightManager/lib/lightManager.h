#include <stdio.h>
#include "pico/stdlib.h"

//TODO: Comment this file
class lightManager{

    private:

        uint32_t measuredLight;
        uint32_t expectedLight;
        bool isPirTriggered;
        uint8_t opMode;
        //TODO: define telemetry structure atribute
    
    public:
        lightManager();
        uint8_t lightFeedBackCheck(uint32_t measuredLight, uint32_t expectedLight);
        void setPWM(uint8_t opMode, bool isPirTriggered);
        //TODO: Definition of method to send telemtry data
};