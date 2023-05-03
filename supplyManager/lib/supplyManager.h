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

        void test();

    //TODO: Create a method to send telemetry data to a queue

};

/*! \brief  Close a relay. This function need to be call from an alarm interrupt
*   \param id Alarm id
*   \param relayPin Relay pin
*   \return <0 to reschedule the same alarm this many us from the time the alarm was previously sccheduled to fire, 
>0 to reschedule the same alarm this many us from the time this method returns, 0 to not reschdeule the alarm*/
int64_t closeRelay(alarm_id_t id, void* relayPin);

/*! \brief Close a relay. This function cannot be call from an alarm interrupt
*   \param relayPin Relay pin to set */
void closeRelay(uint8_t relayPin);

/*! \brief Release a relay. This function need to be call from an alarm interrupt
*   \param id Alarm id
*   \param relayPin Relay pin
*    \return <0 to reschedule the same alarm this many us from the time the alarm was previously sccheduled to fire, 
>0 to reschedule the same alarm this many us from the time this method returns, 0 to not reschdeule the alarm */
int64_t releaseRelay(alarm_id_t id, void* relayPin);

/*! \brief Release a relay. This function cannot be call from an alarm interrupt
*   \param relayPin Relay pin */
void releaseRelay(uint8_t relayPin);