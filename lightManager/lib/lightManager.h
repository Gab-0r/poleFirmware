/**
 * @file lightManager.h
 * @author Juan Gabriel Orozco Orozco
 * @brief This class manage the lamp brightness based in many events, either movement or power. Also, check
 * that the brightness delivered by the lamp is correct.
 * @version 0.1
 * @date 2023-04-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include "pico/stdlib.h"


/**
 * @brief This class manage the lamp brightness based in many events, either movement or power. Also, check
 * that the brightness delivered by the lamp is correct.
 */
class lightManager{

    private:

        /**
         * @brief Light levels measured by light sensor
         * 
         */
        uint8_t measuredLight;

        /**
         * @brief Light level expected for the current PWM duty cycle
         * 
         */
        uint8_t expectedLight;

        /**
         * @brief Pin for the lamp PWM control signal
         * 
         */
        uint8_t pwmPin;

        /**
         * @brief PWM slice to use
         * 
         */
        uint pwmSlice;

        /**
         * @brief PWM channel to use
         * 
         */
        uint pwmChannel;
        
        /**
         * @brief Max PWM level
         * 
         */
        uint16_t pwmMaxBrightLevel;

        /**
         * @brief Min PWM level
         * 
         */
        uint16_t pwmMinBrightLevel;

        /**
         * @brief Operation mode 
         * 
         */
        uint8_t operationMode; 
        
        /**
         * @brief Last control action
         * 
         */
        uint8_t lastAction;

        /**
         * @brief State
         * 
         */
        uint8_t status;

    public:
    
       /**
        * @brief Construct a new light Manager object
        * 
        * @param pwmPin_ Pin where the PWM pin of the lamp dimmer is attached
        */
        lightManager(uint16_t pwmPin_);

       /**
        * @brief Determine if the lighing levels are correct for the curren PWM duty cycle
        * 
        * @return uint8_t Return the state of the lamp brightness
        */
        uint8_t lightFeedBackCheck();

        /**
         * @brief Initialize the PWM with parameters specified in the smarPoleConfig file
         */
        void initPWM();

        /*! \brief Change the PWM duty cycle based on an event 
         *  \param event Event detected
        */

       /**
        * @brief Change the PWM duty cycle based on an event
        * 
        * @param event Event detected
        */
        void setPWM(uint8_t event);

        /* ONLY FOR DEBUG PURPOSES */
        inline bool get_random_bit();
        uint8_t getRandomNumber();

        /* To read light sensor */
        /**
         * @brief Read light sensor  and update the current bright atribute
         * 
         */
        void readSensorsAndUpdate();

        /* Get Methods */

        /**
         * @brief Get the Measured Light value
         * 
         * @return uint8_t 
         */
        uint8_t getMeasuredLight();

        /**
         * @brief Get the Expected Light value
         * 
         * @return uint8_t 
         */
        uint8_t getExpectedLight();

        /**
         * @brief Get the Pwm Max Bright Level value
         * 
         * @return uint16_t 
         */
        uint16_t getPwmMaxBrightLevel();

        /**
         * @brief Get the Pwm Min Bright Level value
         * 
         * @return uint16_t 
         */
        uint16_t getPwmMinBrightLevel();

        /**
         * @brief Get the Operation Mode value
         * 
         * @return uint8_t 
         */
        uint8_t getOperationMode();

        /**
         * @brief Get the Last Action value
         * 
         * @return uint8_t 
         */
        uint8_t getLastAction();

        /**
         * @brief Get the Status value
         * 
         * @return uint8_t 
         */
        uint8_t getStatus();

        /* Set methods */

        /**
         * @brief eceive an operation mode and change the maximun and minimun light levels
        * for the operation mode. This levels are defined in the smartPoleConfig file
         * 
         * @param OP Operation mode
         */
        inline void setOperationMode(uint8_t OP);

        /**
         * @brief Set the Max Bright Level
         * 
         * @param value Value to set
         */
        inline void setMaxBrightLevel(uint16_t value);

        /**
         * @brief Set the Min Bright Level
         * 
         * @param value Value to set
         */
        inline void setMinBrightLevel(uint16_t value);
};