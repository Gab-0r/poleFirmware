/**
 * Supply Manager Class
 * By Juan Gabriel Orozco Orozco - 2023
 * 
 * DESCRIPTION.......
 */

#include <stdio.h>
#include "pico/stdio.h"
#include "pico/time.h"

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

        /*! \brief Class constructor
        *   \param */
        supplyManager();

        /*! \brief  Change the state of a relay
        *   \param switchType Tells what action the relays do (Open/Close)
        *   \param relay2Control The relay pin to control */
        void switchRelay(uint8_t switchType, uint16_t relay2Control);

        /*! \brief Receive an operation mode and change the maximun and minimun power levels for the operation mode.
        *   This levels are defined in the smartPoleConfigFile.
        *   \param opMode Operation mode
        */
        void setOperationMode(uint8_t opMode);


        /*! \brief Return the operation mode */
        inline uint8_t getOperationMode();

        /*! \brief Generates a power alert */
        void powerAlert(uint8_t event);

        void changePowerSupply(uint16_t supplyToDisconnect, uint16_t supplyToConnect);
        
        /* Get methods */
        inline uint8_t getCurrentPowerSupply();
        inline uint8_t getCurrentPower();
        inline uint8_t getLampCurrent();
        inline uint8_t getSensorsCurrent();
        inline uint8_t getSensorsPower();
        inline uint8_t getLampPower();
        inline uint8_t getPowerStatus();
        inline uint8_t getLastAction();
        inline uint8_t* getSuppliesVoltages();

        /* Set methods */
        inline void setSuppliesVoltages(uint8_t* values){
            //TODO: FOR LOOP FOR EXTRACT THE POWER SUPPLIES VOLTAGES
        }
};

/*! \brief  Close a relay. This function need to be call from an alarm interrupt
*   \param id Alarm id
*   \param relayPin Relay pin
*   \return <0 to reschedule the same alarm this many us from the time the alarm was previously sccheduled to fire, 
>0 to reschedule the same alarm this many us from the time this method returns, 0 to not reschdeule the alarm*/
int64_t closeRelay(alarm_id_t id, void* relayPin);

/*! \brief Close a relay. This function cannot be call from an alarm interrupt
*   \param relayPin Relay pin to set */
void closeRelay(uint16_t relayPin);

/*! \brief Release a relay. This function need to be call from an alarm interrupt
*   \param id Alarm id
*   \param relayPin Relay pin
*    \return <0 to reschedule the same alarm this many us from the time the alarm was previously sccheduled to fire, 
>0 to reschedule the same alarm this many us from the time this method returns, 0 to not reschdeule the alarm */
int64_t releaseRelay(alarm_id_t id, void* relayPin);

/*! \brief Release a relay. This function cannot be call from an alarm interrupt
*   \param relayPin Relay pin */
void releaseRelay(uint16_t relayPin);