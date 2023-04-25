/**
 * Light Manager Class
 * By Juan Gabriel Orozco Orozco - 2023
 * 
 * This class manages the light levels based on a given event, given
 * order or a mode operation of the system. The light levels are controlled by
 * a PWM pin that is specfied when the class is instantiate. This file contains the
 * function definition for the class.
 */

#include <stdio.h>
#include "pico/stdlib.h"

class lightManager{

    private:

        /* Light levels measured by light sensor */
        uint32_t measuredLight;

        /* Light level expected for the current PWM duty cycle */
        uint32_t expectedLight;

        /* Pin for the lamp PWM control signal */
        uint8_t pwmPin;

        /* PWM slice to use */
        uint pwmSlice;

        /* PWM channel to use*/
        uint pwmChannel;
        
        /* Max PWM level */
        uint16_t pwmMaxBrightLevel;

        /* Min PWM level */
        uint16_t pwmMinBrightLevel;

        //TODO: Variable for store the actual PWM duty cycle
        //TODO: define telemetry structure atribute

    public:

        /*! \brief Class constructor 
        *   \param pwmPin_ The lighting control pin for the lamp */
        lightManager(uint16_t pwmPin_);

        /*! \brief Determine if the lighting levels are correct for the current PWM duty cycle
         *  
         *  \param measuredLight Light levels given by the lamp
         *  \return Lamp operation state
         * 
        */
        uint8_t lightFeedBackCheck(uint32_t measuredLight);

        /*! \brief Initialize the PWM with parameters specified in the smarPoleConfig file */
        void initPWM();

        /*! \brief Change the PWM duty cycle based on an event 
         *  \param event Event detected
        */
        void setPWM(uint8_t event);

        /*! \brief Receive an operation mode and change the maximun and minimun light levels
        * for the operation mode. This levels are defined in the smartPoleConfig file
        *   \param OP Operation mode
        */
        void setOperationMode(uint8_t OP);
        
        //TODO: Definition of method to send telemtry data
};