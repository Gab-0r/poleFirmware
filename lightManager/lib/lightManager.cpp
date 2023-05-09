/**
 * Light Manager Class
 * By Juan Gabriel Orozco Orozco - 2023
 * 
 * This class manages the light levels based on a given event, given
 * order or a mode operation of the system. The light levels are controlled by
 * a PWM pin that is specfied when the class is instantiate. This file contains the
 * functiona implementations for the class.
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "lightManager.h"
#include "hardware/pwm.h"
#include "smartPoleConfig.h"


lightManager::lightManager(uint16_t pwmPin_){
    setOperationMode(NORMAL_OPERATION);
    pwmPin = pwmPin_;
    initPWM();
}

uint8_t lightManager::lightFeedBackCheck(uint32_t measuredLight){
    //TODO: Function implementation
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

void lightManager::setOperationMode(uint8_t OP){
    switch (OP)
    {
    case NORMAL_OPERATION:
            #if DEBUGLOG_MODE
                printf("CHANGING OPERATION MODE TO <NORMAL_OPERATION>\r\n");
            #endif
            pwmMaxBrightLevel = MAX_BRIGHT_DUTY*LAMP_PWM_WRAP/100;
            pwmMinBrightLevel = MIN_BRIGHT_DUTY*LAMP_PWM_WRAP/100;
        break;
    
    case POWER_SAVING:
            #if DEBUGLOG_MODE
                printf("CHANGING OPERATION MODE TO <POWER_SAVING>\r\n");
            #endif
            pwmMaxBrightLevel = MAX_BRIGHT_ENERGY_SAVING_DUTY*LAMP_PWM_WRAP/100;
            pwmMinBrightLevel = MIN_BRIGHT_ENERGY_SAVING_DUTY*LAMP_PWM_WRAP/100;
        break;

    case EMERGENCY_OPERATION:
            #if DEBUGLOG_MODE
                printf("CHANGING OPERATION MODE TO <EMERGENCY_OPERATION>\r\n");
            #endif
            pwmMaxBrightLevel = MAX_BRIGHT_EMERGENCY_DUTY*LAMP_PWM_WRAP/100;
            pwmMinBrightLevel = MIN_BRIGHT_EMERGENCY_DUTY*LAMP_PWM_WRAP/100;
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

inline uint32_t lightManager::getMeasuredLight(){return measuredLight;}

inline uint32_t lightManager::getExpectedLight(){return expectedLight;}

inline uint16_t lightManager::getPwmMaxBrightLevel(){return pwmMaxBrightLevel;}

inline uint16_t lightManager::getPwmMinBrightLevel(){return pwmMinBrightLevel;}

inline uint8_t lightManager::getOperationMode(){return operationMode;}

inline uint8_t lightManager::getLastAction(){return lastAction;}

inline uint8_t lightManager::getStatus(){return status;}

inline void lightManager::setMaxBrightLevel(uint16_t value){pwmMaxBrightLevel = value;}

inline void lightManager::setMinBrightLevel(uint16_t value){pwmMinBrightLevel = value;}
