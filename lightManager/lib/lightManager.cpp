#include <stdio.h>
#include "pico/stdlib.h"
#include "lightManager.h"

//TODO: Comment this file

lightManager::lightManager(){}

uint8_t lightManager::lightFeedBackCheck(uint32_t measuredLight, uint32_t expectedLight){
    //TODO: Function implementation
    return 0;
}

void lightManager::setPWM(uint8_t opMode, bool isPirTriggered){
    if(isPirTriggered){
        printf("Increasing duty cycle...\r\n");
    }
    else{
        printf("Decreasing duty cycle...\r\n");
    }
}