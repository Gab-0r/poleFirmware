//TODO: Comment header

#include "supplyManager.h"
#include "pico/stdio.h"
#include <stdio.h>
#include "smartPoleConfig.h"
#include "hardware/gpio.h"
#include "pico/time.h"

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

int64_t closeRelay(alarm_id_t id, void* data)
{
    uint16_t relayPin = *((uint16_t*)data);
    printf("Cerrando relay %u\r\n", relayPin);
    gpio_put(relayPin, 1);
    //printf("Relay %d encendido\r\n",*reinterpret_cast<int*>(relayPin));
    return 0;
}

void closeRelay(uint8_t relayPin)
{
    gpio_put((uint16_t)relayPin, 1);
}

int64_t releaseRelay(alarm_id_t id, void *relayPin)
{
    gpio_put(*reinterpret_cast<int*>(relayPin), 0);
    return 0;
}

void releaseRelay(uint8_t relayPin)
{
    gpio_put(relayPin, 0);
}

void supplyManager::test()
{
    printf("Desconectando fuente...\r\n");
    releaseRelay(GRID_SUPPLY_RELAY);
    static uint16_t data =  21;
    switchAlert = add_alarm_in_ms(RELAY_RELEASE_TIME, closeRelay, &data, true);
}
