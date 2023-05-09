/***************************************************/
/*                   System pins                   */
/***************************************************/
#define PIR_SENSOR_PIN          (uint16_t)15U
#define LED_CTRL_PIN            (uint16_t)7U
#define GRID_SUPPLY_RELAY       (uint16_t)22U
#define BATTERY_SUPPLY_RELAY    (uint16_t)21U
#define SOLAR_SUPPLY_RELAY      (uint16_t)20U
#define MAGNETIC_SWITCH_PIN     (uint16_t)16U

/***************************************************/
/*                 System parameters               */
/***************************************************/

/* Print debug messages */
#define DEBUGLOG_MODE   1
/* Period to launch the next measurement stage in ms*/
#define PERIOD_TRIGGER_TASK 10000UL
/* Time in ms until the lamp decreases the bright after no detect movement */
#define MAX_BRIGHT_TIME  (uint32_t)10000U
/* Duty cycle (0 - 100) for max bright, i.e when movement is detected*/
#define MAX_BRIGHT_DUTY 99U
/* Duty cycle (0 - 100) for min bright, i.e when is no movement*/
#define MIN_BRIGHT_DUTY 20U
/* Duty cycle (0 - 100) for max bright, i.e when movement is detected and system is in power saving mode*/
#define MAX_BRIGHT_ENERGY_SAVING_DUTY 60U
/* Duty cycle (0 - 100) for min bright, i.e when is no movement and system is in power saving mode*/
#define MIN_BRIGHT_ENERGY_SAVING_DUTY 20U
/* Duty cycle (0 - 100) for max bright, i.e when movement is detected and system is in emergency mode*/
#define MAX_BRIGHT_EMERGENCY_DUTY 30U
/* Duty cycle (0 - 100) for min bright, i.e when is no movement* and system is in emergency mode */
#define MIN_BRIGHT_EMERGENCY_DUTY 30U
/* PWM frequency that controls lamp brightness*/
#define LAMP_PWM_FREQUENCY 30000U
/* PWM wrap*/
#define LAMP_PWM_WRAP 4167U
/* Time in ms until the relay release the contact */
#define RELAY_RELEASE_TIME  5000UL



/***************************************************/
/*                    Enumerations                 */
/***************************************************/


/* Operating modes */
enum OP_MODE
{
    NORMAL_OPERATION,
    POWER_SAVING,
    EMERGENCY_OPERATION
};

/* Sensors Events */
enum SENS_EVENTS
{
    MOVEMENT_DETECTED,
    NO_MOVEMENT
};

/* System alerts */
enum SYSTEM_ALERTS{
    NO_ALERT,
    BOX_OPEN
};

/* Light Manager status */
enum LIGHT_MANAGER_STATUS{
    OK,
    UNEXPECTED,
    NOT_WORKING
};

/* Light Manager control actions */
enum LIGHT_MANAGER_CONTROL_ACTIONS{
    INCREASE_BRIGHT,
    DECREASE_BRIGHT,
    OP_MODE_CHANGED
}; 

/* Supply Manager status */
enum SUPPLY_MANAGER_STATUS{
    OK,
    LOW_POWER,
    OVER_VOLTAGE,
    OVER_CURRENT,
    NO_POWER
};

/* Supply Manager actions*/
enum POWER_EVENTS{
    CLOSE_RELAY,
    RELEASE_RELAY,
    SUPPLY_CHANGE,
    MAX_CURRENT_THRESHOLD_CHANGED,
    MIN_CURRENT_THRESHOLD_CHANGED,
    OP_MDOE_CHANGED
};
