/**
 * Smart pole firmware
 * By Juan Gabriel Orozco Orozco - 2023
 * 
 * Description.....
 */

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "timers.h"
#include "event_groups.h"
#include "pico/time.h"
#include "lightManager.h"
#include "supplyManager.h"
#include "smartPoleConfig.h"

/* Bits to trigger tasks */
#define READ_ENVIROMENT_SENSORS_TASKS_TRIGGER           (1UL << 0UL)
#define READ_ONBOARD_SENSORS_TASKS_TRIGGER              (1UL << 1UL)
#define READ_EDGE_BOARDS_TASK_TRIGGER                   (1UL << 2UL)
#define LIGHT_MANAGER_TASK_TRIGGER                      (1UL << 3UL)
#define SUPPLY_MANAGER_TASK_TRIGGER                     (1UL << 4UL)    
#define PACKET_MANAGER_TASK_TRIGGER0                    (1UL << 5UL) 
#define PACKET_MANAGER_TASK_TRIGGER1                    (1UL << 6UL) 
#define PACKET_MANAGER_TASK_TRIGGER2                    (1UL << 7UL) 
#define PACKET_MANAGER_TASK_TRIGGER3                    (1UL << 8UL) 
#define COM_MANAGER_TASK_TRIGGER                        (1UL << 9UL)

/***************************************************/
/*                                                 */
/***************************************************/

void hardwareInit();
void pirTriggered(uint /*gpio*/, uint32_t /*event_mask*/);
//TODO: Definir ISR de los sensores de corriente
void OSinit();



bool isPirTriggered = false;
int64_t pirOff(alarm_id_t /*id*/, void* /*user_data*/);
alarm_id_t pirAlarmId = 0;
alarm_id_t relayRelease = 0;

lightManager light_manager(LED_CTRL_PIN);
supplyManager supply_manager;


/***************************************************/
/*           Section related to freeRTOS           */
/***************************************************/

/* Create an event group*/
EventGroupHandle_t xEventGroup;

/* Tasks */
void periodicTriggerTask(void *pvParameters);       //Periodic trigger for the measurement stage
void readEnviromentSensorsTask(void *pvParameters); //Task to read the board with enviroment sensors
void readOnBoardSensorsTask(void *pvParameters);    //Task to read the onboard sensors (Current, voltage and light)
void readEdgeBoardsTask(void *pvParameters);        //Task to read the edge computing boards events and states
void lightManagerTask(void *pvParameters);          //Task to manage the light
void packetManagerTask(void *pvParameters);         //Task to receive the values from the other task and create the telemetry packets
void supplyManagerTask(void *pvParameters);         //Task to manage the power supplies
void comManagerTask(void *pvParameters);            //Tas to manage the communication module


int main()
{
    /* Initialize the RPPico hardware */
    hardwareInit();

    /* Iniziale the OS */
    OSinit();

    /* Initialize scheduler */
    vTaskStartScheduler();

    while(1){};
}

void hardwareInit(){

    stdio_init_all();

    /* Pins declaration */
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    gpio_init(GRID_SUPPLY_RELAY);
    gpio_set_dir(GRID_SUPPLY_RELAY, GPIO_OUT);
    gpio_put(GRID_SUPPLY_RELAY, 1);

    gpio_init(BATTERY_SUPPLY_RELAY);
    gpio_set_dir(BATTERY_SUPPLY_RELAY, GPIO_OUT);

    gpio_init(SOLAR_SUPPLY_RELAY);
    gpio_set_dir(SOLAR_SUPPLY_RELAY, GPIO_OUT);


    /* Pins with IRQ */
    gpio_set_irq_enabled_with_callback(PIR_SENSOR_PIN, GPIO_IRQ_EDGE_RISE, true, &pirTriggered);

}

void OSinit(){

    /* Create tasks */
    xTaskCreate(periodicTriggerTask, "PeriodicTrigger", 1000, NULL, 1, NULL);
    xTaskCreate(readEnviromentSensorsTask, "ReadEnvSensors", 1000, NULL, 2, NULL);
    xTaskCreate(readOnBoardSensorsTask, "ReadOnBoard", 1000, NULL, 2, NULL);
    xTaskCreate(readEdgeBoardsTask, "ReadEdge", 1000, NULL, 2, NULL);
    xTaskCreate(lightManagerTask, "LighManage", 1000, NULL, 2, NULL);
    xTaskCreate(supplyManagerTask, "SupplyManage", 1000, NULL, 2, NULL);
    xTaskCreate(packetManagerTask, "PacketManage", 1000, NULL, 2, NULL);
    xTaskCreate(comManagerTask, "COMManage", 1000, NULL, 2, NULL);

    /* Create event groups*/
    xEventGroup = xEventGroupCreate();

}

void pirTriggered(uint /*gpio*/, uint32_t /*event_mask*/){

    BaseType_t xHigherPriorityTaskWoken, xResult;
    //printf("Movimiento detectado\r\n");
    //gpio_put(PICO_DEFAULT_LED_PIN, 1);
    isPirTriggered = true;
    if (pirAlarmId == 0)
    {   
        pirAlarmId =  add_alarm_in_ms(MAX_BRIGHT_TIME, pirOff, NULL, true);
        xEventGroupSetBitsFromISR(xEventGroup, LIGHT_MANAGER_TASK_TRIGGER, &xHigherPriorityTaskWoken);
    }
    else{
        cancel_alarm(pirAlarmId);
        pirAlarmId =  add_alarm_in_ms(MAX_BRIGHT_TIME, pirOff, NULL, true);
        xEventGroupSetBitsFromISR(xEventGroup, LIGHT_MANAGER_TASK_TRIGGER, &xHigherPriorityTaskWoken);
    }
}

int64_t pirOff(alarm_id_t /*id*/, void* /*user_data*/){
    BaseType_t xHigherPriorityTaskWoken, xResult;
    xEventGroupSetBitsFromISR(xEventGroup, LIGHT_MANAGER_TASK_TRIGGER, &xHigherPriorityTaskWoken);
    isPirTriggered = false;
    //printf("Sin movimiento\r\n");
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    return 0;
}

void periodicTriggerTask(void *pvParameters){
    const TickType_t xDelayTrigger = pdMS_TO_TICKS(PERIOD_TRIGGER_TASK), xDontBLock = 0;
    while (true)
    {
        vTaskDelay(xDelayTrigger);
        printf("<---- LAUNCHING MEASUREMENT STAGE ---->\r\n");
        xEventGroupSetBits(xEventGroup, READ_ENVIROMENT_SENSORS_TASKS_TRIGGER);
        xEventGroupSetBits(xEventGroup, READ_ONBOARD_SENSORS_TASKS_TRIGGER);
        xEventGroupSetBits(xEventGroup, READ_EDGE_BOARDS_TASK_TRIGGER);
    }
}

void readEnviromentSensorsTask(void *pvParameters){
    /* Variable for store event group values */
    EventBits_t xEventGroupValue;

    /* Variable for the bits the task is waiting for */
    const EventBits_t xBitsToWaitFor = READ_ENVIROMENT_SENSORS_TASKS_TRIGGER;

    while (true)
    {
        xEventGroupValue = xEventGroupWaitBits(xEventGroup, xBitsToWaitFor, pdTRUE, pdTRUE, portMAX_DELAY);
        printf("<---- READING ENVIROMENTAL SENSORS ---->\r\n");
        xEventGroupSetBits(xEventGroup, PACKET_MANAGER_TASK_TRIGGER0);
    }
}

void readOnBoardSensorsTask(void *pvParameters){
    /* Variable for store event group values */
    EventBits_t xEventGroupValue;

    /* Variable for the bits the task is waiting for */
    const EventBits_t xBitsToWaitFor = READ_ONBOARD_SENSORS_TASKS_TRIGGER;

    while (true)
    {
        xEventGroupValue = xEventGroupWaitBits(xEventGroup, xBitsToWaitFor, pdTRUE, pdTRUE, portMAX_DELAY);
        printf("<---- READING ON BOARD SENSORS ---->\r\n");
        xEventGroupSetBits(xEventGroup, LIGHT_MANAGER_TASK_TRIGGER);
        xEventGroupSetBits(xEventGroup, SUPPLY_MANAGER_TASK_TRIGGER);
    }
}

void readEdgeBoardsTask(void *pvParameters){
    /* Variable for store event group values */
    EventBits_t xEventGroupValue;

    /* Variable for the bits the task is waiting for */
    const EventBits_t xBitsToWaitFor = READ_EDGE_BOARDS_TASK_TRIGGER;

    while (true)
    {
        xEventGroupValue = xEventGroupWaitBits(xEventGroup, xBitsToWaitFor, pdTRUE, pdTRUE, portMAX_DELAY);
        printf("<---- READING EDGE COMPUTING BOARDS ---->\r\n");
        xEventGroupSetBits(xEventGroup, PACKET_MANAGER_TASK_TRIGGER3);
    }
}

void lightManagerTask(void *pvParameters){
    /* Variable for store event group values */
    EventBits_t xEventGroupValue;

    /* Variable for the bits the task is waiting for */
    const EventBits_t xBitsToWaitFor = LIGHT_MANAGER_TASK_TRIGGER;

    while (true)
    {
        xEventGroupValue = xEventGroupWaitBits(xEventGroup, xBitsToWaitFor, pdTRUE, pdTRUE, portMAX_DELAY);
        printf("<---- LIGHT MANAGER TRIGGERED ---->\r\n");

        if(isPirTriggered){
            printf("Aumentando duty\r\n");
            light_manager.setPWM(MOVEMENT_DETECTED);
        }
        else{
            light_manager.setPWM(NO_MOVEMENT);
        }

        /* TODO: Comprobar si hay correcta operación, de lo contrario iniciar comunicación de emergencia
        switch (result)
        {
        case NO_POWER_ALERT:
            /*
            break;
        
        case LOW_POWER_ALERT:
            //Despertar la tarea de comunicación con alta prioridad y que envíe la alerta de potencia
            break;
        
        default:
            break;
        }
        */
        xEventGroupSetBits(xEventGroup, PACKET_MANAGER_TASK_TRIGGER1);
    }
}

void supplyManagerTask(void *pvParameters){
    /* Variable for store event group values */
    EventBits_t xEventGroupValue;

    /* Variable for the bits the task is waiting for */
    const EventBits_t xBitsToWaitFor = SUPPLY_MANAGER_TASK_TRIGGER;

    while (true)
    {
        xEventGroupValue = xEventGroupWaitBits(xEventGroup, xBitsToWaitFor, pdTRUE, pdTRUE, portMAX_DELAY);
        printf("<---- SUPPLY MANAGER TRIGGERED ---->\r\n");
        printf("Abriendo relay\r\n");
        supply_manager.test();

        /* TODO: Comprobar si hay correcta operación, de lo contrario iniciar comunicación de emergencia
        switch (result)
        {
        case NO_POWER_ALERT:
            /*
            break;
        
        case LOW_POWER_ALERT:
            //Despertar la tarea de comunicación con alta prioridad y que envíe la alerta de potencia
            break;
        
        default:
            break;
        }
        */

        xEventGroupSetBits(xEventGroup, PACKET_MANAGER_TASK_TRIGGER2);
    }
}

void packetManagerTask(void *pvParameters){
    /* Variable for store event group values */
    EventBits_t xEventGroupValue;

    /* Variable for the bits the task is waiting for */
    const EventBits_t xBitsToWaitFor = (PACKET_MANAGER_TASK_TRIGGER0 | PACKET_MANAGER_TASK_TRIGGER1 | PACKET_MANAGER_TASK_TRIGGER2 | PACKET_MANAGER_TASK_TRIGGER3);

    while (true)
    {
        xEventGroupValue = xEventGroupWaitBits(xEventGroup, xBitsToWaitFor, pdTRUE, pdTRUE, portMAX_DELAY);
        printf("<---- PACKET MANAGER TRIGGERED ---->\r\n");
        xEventGroupSetBits(xEventGroup, COM_MANAGER_TASK_TRIGGER);
    }
}

void comManagerTask(void *pvParameters){
    /* Variable for store event group values */
    EventBits_t xEventGroupValue;

    /* Variable for the bits the task is waiting for */
    const EventBits_t xBitsToWaitFor = COM_MANAGER_TASK_TRIGGER;

    while (true)
    {
        xEventGroupValue = xEventGroupWaitBits(xEventGroup, xBitsToWaitFor, pdTRUE, pdTRUE, portMAX_DELAY);
        printf("<---- COM MANAGER TRIGGERED ---->\r\n");
    }
}
