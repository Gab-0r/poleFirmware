/**
 * Supply Manager Class
 * By Juan Gabriel Orozco Orozco - 2023
 * 
 * DESCRIPTION.......
 */

#include <stdio.h>
#include "pico/stdio.h"

class supplyManager{

    private:
        uint8_t currentPowerSupply;
        uint16_t currentPower;
        uint8_t operationMode;
        
    
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

    //TODO: Create a method to send telemetry data to a queue

};