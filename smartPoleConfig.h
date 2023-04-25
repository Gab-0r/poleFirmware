/***************************************************/
/*                 System parameters               */
/***************************************************/


/* Period to launch the next measurement stage */
#define PERIOD_TRIGGER_TASK 5000UL
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

/* Events */
enum SENS_EVENTS
{
    MOVEMENT_DETECTED,
    NO_MOVEMENT
};