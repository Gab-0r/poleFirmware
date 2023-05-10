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

void supplyManager::setOperationMode(uint8_t opMode)
{

}

int64_t closeRelay(alarm_id_t id, void* data)
{
    #if DEBUGLOG_MODE
        printf("FROM ALARM: CLOSING RELAY ON PIN %u\r\n", *((uint16_t*)data));
    #endif
    gpio_put(*((uint16_t*)data), 1);
    return 0;
}

void closeRelay(uint16_t relayPin)
{
    #if DEBUGLOG_MODE
        printf("CLOSING RELAY ON PIN %u\r\n", relayPin);
    #endif
    gpio_put((uint16_t)relayPin, 1);
}

int64_t releaseRelay(alarm_id_t id, void *data)
{
    #if DEBUGLOG_MODE
        printf("FROM ALARM: RELEASING RELAY ON PIN %d\r\n", *((uint16_t*)data));
    #endif
    gpio_put(*((uint16_t*)data), 0);
    return 0;
}

void releaseRelay(uint16_t relayPin)
{
    #if DEBUGLOG_MODE
        printf("RELEASING RELAY ON PIN %d\r\n", relayPin);
    #endif
    gpio_put(relayPin, 0);
}

void supplyManager::changePowerSupply(uint16_t supplyToDisconnect, uint16_t supplyToConnect)
{
    releaseRelay(supplyToDisconnect);
    static uint16_t data =  supplyToConnect;
    switchAlert = add_alarm_in_ms(RELAY_RELEASE_TIME, closeRelay, &data, true);
}

inline uint8_t supplyManager::getCurrentPowerSupply(){return currentPowerSupply;}
inline uint8_t supplyManager::getCurrentPower(){return currentPower;}
inline uint8_t supplyManager::getLampCurrent(){return lampCurrent;}
inline uint8_t supplyManager::getSensorsCurrent(){return sensorsCurrent;}
inline uint8_t supplyManager::getSensorsPower(){return sensorsPower;}
inline uint8_t supplyManager::getLampPower(){return lampPower;}
inline uint8_t supplyManager::getPowerStatus(){return powerStatus;}
inline uint8_t supplyManager::getLastAction(){return lastAction;}
inline uint8_t* supplyManager::getSuppliesVoltages(){return suppliesVoltages;}