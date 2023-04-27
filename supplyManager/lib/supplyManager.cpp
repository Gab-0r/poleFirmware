#include "supplyManager.h"
#include "pico/stdio.h"
#include <stdio.h>
#include "smartPoleConfig.h"
#include "hardware/gpio.h"

supplyManager::supplyManager()
{
}

void supplyManager::switchRelay(uint8_t switchType, uint16_t relay2Control)
{
    switch (switchType)
    {
    case CLOSE_RELAY:
        gpio_put(relay2Control, 1);
        break;

    case RELEASE_RELAY:
        gpio_put(relay2Control, 0);
        break;
    
    default:
        break;
    }
}

void supplyManager::setOperationMode(uint8_t opMode)
{

}
