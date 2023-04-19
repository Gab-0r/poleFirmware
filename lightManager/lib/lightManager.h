#include <stdio.h>
#include "pico/stdlib.h"

//TODO: Comment this file
class lightManager{

    private:

        uint32_t measuredLight;
        uint32_t expectedLight;
        bool isPirTriggered;
        uint8_t opMode;
        uint8_t pwmPin;
        uint pwmSlice;
        uint pwmChannel;
        uint8_t pwmDuty;
        //TODO: define telemetry structure atribute
    
    public:
        lightManager(uint16_t pwmPin_);
        uint8_t lightFeedBackCheck(uint32_t measuredLight, uint32_t expectedLight);
        void setPWM(uint8_t opMode, uint16_t duty);
        //TODO: Definition of method to send telemtry data
};