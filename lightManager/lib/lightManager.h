#include <stdio.h>
#include "pico/stdlib.h"


//TODO: Comment this file
class lightManager{

    private:

        uint32_t measuredLight;
        uint32_t expectedLight;
        uint8_t pwmPin;
        uint pwmSlice;
        uint pwmChannel;
        
        uint16_t pwmMaxBrightLevel;
        uint16_t pwmMinBrightLevel;
        //TODO: define telemetry structure atribute
    
    public:
        lightManager(uint16_t pwmPin_);
        uint8_t lightFeedBackCheck(uint32_t measuredLight, uint32_t expectedLight);
        void initPWM();
        void setPWM(uint8_t event);
        void setOperationMode(uint8_t OP);
        
        //TODO: Definition of method to send telemtry data
};