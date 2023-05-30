
/**
 * @file supplyManager.h
 * @author Juan Gabriel Orozco Orozco
 * @brief This class manage the different power supplies, measure their power and select the appropiate one. 
 * In addition the class can report power events that affect the system.
 * @version 1
 * @date 2023-04-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include "pico/stdio.h"
#include "pico/time.h"

/**
 * @brief This class manage the different power supplies, measure their power and select the appropiate one. 
 * In addition the class can report power events that affect the system. 
 */
class supplyManager{

    private:
        uint8_t currentPowerSupply;
        uint16_t currentPower;
        uint8_t operationMode;
        uint8_t lampCurrent;
        uint8_t sensorsCurrent;
        uint8_t sensorsPower;
        uint8_t lampPower;
        uint8_t powerStatus;
        uint8_t lastAction;
        uint8_t suppliesVoltages[3];

        alarm_id_t switchAlert;
    
    public:

       /**
        * @brief Construct a new power supply object
        * 
        */
        supplyManager();

       /**
        * @brief Change the state of a relay
        * 
        * @param switchType Tells what action the relays do (Open/Close)
        * @param relay2Control The relay pin to switch
        */
        void switchRelay(uint8_t switchType, uint16_t relay2Control);

       /**
        * @brief Receive and operation mode and change the maximun and minimun power levels for the operation mode.
        * This levels are defined in the smartPoleConfig file
        * @param opMode Operation mode
        */
        void setOperationMode(uint8_t opMode);

        /**
         * @brief Return the operation mode
         * 
         * @return uint8_t 
         */
        inline uint8_t getOperationMode();

        /**
         * @brief Generates a power alert
         * 
         * @param event Event that happened
         */
        void powerAlert(uint8_t event);

        /**
         * @brief Changes the power supply of the system
         * 
         * @param supplyToDisconnect Power supply to disconect
         * @param supplyToConnect  Power supply to connect
         */
        void changePowerSupply(uint16_t supplyToDisconnect, uint16_t supplyToConnect);
        
        /* Get methods */

        /**
         * @brief Get the current power supply that feeds the system
         * 
         * @return uint8_t 
         */
        inline uint8_t getCurrentPowerSupply();

        /**
         * @brief Get the current power consumption 
         * 
         * @return uint8_t 
         */
        inline uint8_t getCurrentPower();

        /**
         * @brief Get the current that is flowing through the lamp
         * 
         * @return uint8_t 
         */
        inline uint8_t getLampCurrent();

        /**
         * @brief Get the current that is flowing through the sensors
         * 
         * @return uint8_t 
         */
        inline uint8_t getSensorsCurrent();

        /**
         * @brief Get the sensors power consumption
         * 
         * @return uint8_t 
         */
        inline uint8_t getSensorsPower();

        /**
         * @brief Get the power consumption of the lamp
         * 
         * @return uint8_t 
         */
        inline uint8_t getLampPower();

        /**
         * @brief Get the power status of the system
         * 
         * @return uint8_t 
         */
        inline uint8_t getPowerStatus();

        /**
         * @brief Get the last control action
         * 
         * @return uint8_t 
         */
        inline uint8_t getLastAction();

        /**
         * @brief Get the power supplies voltages
         * 
         * @return uint8_t* 
         */
        inline uint8_t* getSuppliesVoltages();

        /**
         * @brief Set the supplies voltages values
         * 
         * @param values Pointer to array for values to set
         */
        inline void setSuppliesVoltages(uint8_t* values){
            //TODO: FOR LOOP FOR EXTRACT THE POWER SUPPLIES VOLTAGES
        }
};

/**
 * @brief Close a relay. This funciton need to be call from an alarm interrupt
 * @param id Alarm id
 * @param relayPin Relay pin to close
 * @return int64_t <0 to reschedule the same alarm this many us from the time the alarm was previously sccheduled to fire, 
>0 to reschedule the same alarm this many us from the time this method returns, 0 to not reschdeule the alarm
 */
int64_t closeRelay(alarm_id_t id, void* relayPin);

/** @brief Close a relay. This function cannot be call from an alarm interrupt
*   @param relayPin Relay pin to set */
void closeRelay(uint16_t relayPin);

/** @brief Release a relay. This function need to be call from an alarm interrupt
*   @param id Alarm id
*   @param relayPin Relay pin
*    @return <0 to reschedule the same alarm this many us from the time the alarm was previously sccheduled to fire, 
>0 to reschedule the same alarm this many us from the time this method returns, 0 to not reschdeule the alarm */
int64_t releaseRelay(alarm_id_t id, void* relayPin);

/** @brief Release a relay. This function cannot be call from an alarm interrupt
*   @param relayPin Relay pin */
void releaseRelay(uint16_t relayPin);