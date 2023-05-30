#include <stdio.h>
#include "pico/stdlib.h"
#include "lightManager.h"
#include "hardware/pwm.h"
#include "smartPoleConfig.h"
#include "hardware/structs/rosc.h"


lightManager::lightManager(uint16_t pwmPin_){
    setOperationMode(NORMAL_OPERATION);
    pwmPin = pwmPin_;
    initPWM();
}

uint8_t lightManager::lightFeedBackCheck(){
    //TODO: Function implementation

    if(measuredLight == 0){
        return NOT_WORKING;
    }

    if(measuredLight == expectedLight){
        return EXPECTED;
    }
    else{
        return UNEXPECTED;
    } 
    return 0;
}

void lightManager::setPWM(uint8_t event){
    switch (event)
    {
    case MOVEMENT_DETECTED:
            #if DEBUGLOG_MODE
                printf("INCREASING PWM DUTY CYCLE\r\n");
            #endif
            pwm_set_gpio_level(pwmPin, pwmMaxBrightLevel);
            pwm_set_enabled(pwmSlice, true);    
        break;

    case NO_MOVEMENT:
            pwm_set_gpio_level(pwmPin, pwmMinBrightLevel);
            pwm_set_enabled(pwmSlice, true);
        break;
    
    default:
            pwm_set_gpio_level(pwmPin, pwmMinBrightLevel);
            pwm_set_enabled(pwmSlice, true);    
        break;
    }
}

inline bool lightManager::get_random_bit(){return rosc_hw->randombit;}

uint8_t lightManager::getRandomNumber(){
    uint8_t number = 0;
    for (size_t i = 0; i < 8; i++)
    {
        bool bit = get_random_bit();
        number  = number | (bit << i);
        //printf("Random bit generated: %d\r\n", bit);
    }
    //printf("Random number generated: %d\r\n", number);
    return number;
}

void lightManager::readSensorsAndUpdate(){
    //TODO: Real function implementation to read light sensor

    measuredLight = getRandomNumber();
    expectedLight = measuredLight;
}


void lightManager::setOperationMode(uint8_t OP){
    switch (OP)
    {
    case NORMAL_OPERATION:
            #if DEBUGLOG_MODE
                printf("CHANGING OPERATION MODE TO <NORMAL_OPERATION>\r\n");
            #endif
            pwmMaxBrightLevel = MAX_BRIGHT_DUTY*LAMP_PWM_WRAP/100;
            pwmMinBrightLevel = MIN_BRIGHT_DUTY*LAMP_PWM_WRAP/100;
            operationMode = NORMAL_OPERATION;
        break;
    
    case POWER_SAVING:
            #if DEBUGLOG_MODE
                printf("CHANGING OPERATION MODE TO <POWER_SAVING>\r\n");
            #endif
            pwmMaxBrightLevel = MAX_BRIGHT_ENERGY_SAVING_DUTY*LAMP_PWM_WRAP/100;
            pwmMinBrightLevel = MIN_BRIGHT_ENERGY_SAVING_DUTY*LAMP_PWM_WRAP/100;
            operationMode = POWER_SAVING;
        break;

    case EMERGENCY_OPERATION:
            #if DEBUGLOG_MODE
                printf("CHANGING OPERATION MODE TO <EMERGENCY_OPERATION>\r\n");
            #endif
            pwmMaxBrightLevel = MAX_BRIGHT_EMERGENCY_DUTY*LAMP_PWM_WRAP/100;
            pwmMinBrightLevel = MIN_BRIGHT_EMERGENCY_DUTY*LAMP_PWM_WRAP/100;
            operationMode = EMERGENCY_OPERATION;
        break;

    default:
        break;
    }
}

void lightManager::initPWM(){
    #if DEBUGLOG_MODE
        printf("Initiliazing pwm pin at %d\r\n", pwmPin);
    #endif
    gpio_set_function(pwmPin, GPIO_FUNC_PWM);

    pwmSlice = pwm_gpio_to_slice_num(pwmPin);
    #if DEBUGLOG_MODE
        printf("using %d pwm slice\r\n", pwmSlice);
    #endif

    pwm_set_wrap(pwmSlice, LAMP_PWM_WRAP);
    pwm_set_gpio_level(pwmPin, pwmMinBrightLevel);
    pwm_set_enabled(pwmSlice, true);
    #if DEBUGLOG_MODE
        printf("Starting PWM with a frequeny of %d\r\n", LAMP_PWM_FREQUENCY);
    #endif 
}

inline uint8_t lightManager::getMeasuredLight(){return measuredLight;}

inline uint32_t lightManager::getExpectedLight(){return expectedLight;}

inline uint16_t lightManager::getPwmMaxBrightLevel(){return pwmMaxBrightLevel;}

inline uint16_t lightManager::getPwmMinBrightLevel(){return pwmMinBrightLevel;}

inline uint8_t lightManager::getOperationMode(){return operationMode;}

inline uint8_t lightManager::getLastAction(){return lastAction;}

inline uint8_t lightManager::getStatus(){return status;}

inline void lightManager::setMaxBrightLevel(uint16_t value){pwmMaxBrightLevel = value;}

inline void lightManager::setMinBrightLevel(uint16_t value){pwmMinBrightLevel = value;}
