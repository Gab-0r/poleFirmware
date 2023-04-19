#include <stdio.h>
#include "pico/stdlib.h"
#include "lightManager.h"
#include "hardware/pwm.h"

//TODO: Comment this file

lightManager::lightManager(uint16_t pwmPin_){
    //TODO: Create a macro for PWM wrap (4167) in terms of frequency
    pwmPin = pwmPin_;
    gpio_set_function(pwmPin, GPIO_FUNC_PWM);
    pwmSlice = pwm_gpio_to_slice_num(pwmPin);
    pwm_set_wrap(pwmSlice, 4167);
    pwmChannel = pwm_gpio_to_channel(pwmPin);
    //pwm_set_chan_level(pwmSlice, pwmChannel, 2000);
    pwm_set_gpio_level(pwmPin, 4000);
    pwm_set_enabled(pwmSlice, true);
    
}

uint8_t lightManager::lightFeedBackCheck(uint32_t measuredLight, uint32_t expectedLight){
    //TODO: Function implementation
    return 0;
}

void lightManager::setPWM(uint8_t opMode, uint16_t duty){
    pwm_set_gpio_level(pwmPin, duty);
    pwm_set_enabled(pwmSlice, true);
}