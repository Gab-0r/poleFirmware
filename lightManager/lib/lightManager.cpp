#include <stdio.h>
#include "pico/stdlib.h"
#include "lightManager.h"
#include "hardware/pwm.h"
#include "smartPoleConfig.h"

//TODO: Comment this file

lightManager::lightManager(uint16_t pwmPin_){
    setOperationMode(NORMAL_OPERATION);
    pwmPin = pwmPin_;
    initPWM();
}

uint8_t lightManager::lightFeedBackCheck(uint32_t measuredLight, uint32_t expectedLight){
    //TODO: Function implementation
    return 0;
}

void lightManager::setPWM(uint8_t event){
    switch (event)
    {
    case MOVEMENT_DETECTED:
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
            pwmMaxBrightLevel = MAX_BRIGHT_DUTY*LAMP_PWM_WRAP/100;
            pwmMinBrightLevel = MIN_BRIGHT_DUTY*LAMP_PWM_WRAP/100;
        break;
    
    case POWER_SAVING:
            pwmMaxBrightLevel = MAX_BRIGHT_ENERGY_SAVING_DUTY*LAMP_PWM_WRAP/100;
            pwmMinBrightLevel = MIN_BRIGHT_ENERGY_SAVING_DUTY*LAMP_PWM_WRAP/100;
        break;

    case EMERGENCY_OPEARTION:
            pwmMaxBrightLevel = MAX_BRIGHT_EMERGENCY_DUTY*LAMP_PWM_WRAP/100;
            pwmMinBrightLevel = MIN_BRIGHT_EMERGENCY_DUTY*LAMP_PWM_WRAP/100;
        break;

    default:
        break;
    }
}

void lightManager::initPWM(){
    gpio_set_function(pwmPin, GPIO_FUNC_PWM);
    pwmSlice = pwm_gpio_to_slice_num(pwmPin);
    pwm_set_wrap(pwmSlice, LAMP_PWM_WRAP);
    pwm_set_gpio_level(pwmPin, pwmMinBrightLevel);
    pwm_set_enabled(pwmSlice, true);
}