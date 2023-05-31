/**
 * @file poleFirmware.cpp
 * @author Juan Gabriel Orozco Orozco
 * @brief Main file of the smart pole firmware. This file initialize the hardware, the operating system
 * and start the scheduler with its tasks
 * @version 1.0
 * @date 2023-04-4
 * 
 * @copyright Copyright (c) 2023
 * 
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
#include "queue.h"

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
void magneticSwitchISR(uint /*gpio*/, uint32_t /*event_mask*/);

//TODO: Definir ISR de los sensores de corriente
void OSinit();


uint8_t systemAlert;
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

/* Queues */
QueueHandle_t xLightData;
QueueHandle_t xPowerData;

/* Data structures for queues */
typedef struct lightData_s{
    uint8_t measuredLight;
    uint8_t expectedLight;
    uint8_t operationMode;
    uint8_t status;
}lightData_t;



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
    gpio_set_irq_enabled_with_callback(MAGNETIC_SWITCH_PIN, GPIO_IRQ_EDGE_FALL, true, &magneticSwitchISR);

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
    xTaskCreate(comManagerTask, "COMManage", 1000, NULL, 3, NULL);

    /* Create Queues */
    xLightData = xQueueCreate(1, sizeof(lightData_t));


    /* Create event groups*/
    xEventGroup = xEventGroupCreate();

}

void magneticSwitchISR(uint /*gpio*/, uint32_t /*event_mask*/){
    BaseType_t xHigherPriorityTaskWoken, xResult;
    systemAlert = BOX_OPEN;
    xEventGroupSetBitsFromISR(xEventGroup, COM_MANAGER_TASK_TRIGGER, &xHigherPriorityTaskWoken);
    //TODO: Manage the timing of generated alerts
}

void pirTriggered(uint /*gpio*/, uint32_t /*event_mask*/){

    BaseType_t xHigherPriorityTaskWoken, xResult;
    #if DEBUGLOG_MODE
        printf("<PIR_TRIGGERED INTERRUPT: MOVEMENT DETECTED>\r\n");
    #endif
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
    #if DEBUGLOG_MODE
        printf("<PIR_OFF INTERRUPT: NO MOVEMENT>");
    #endif
    //printf("Sin movimiento\r\n");
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    return 0;
}

void periodicTriggerTask(void *pvParameters){
    const TickType_t xDelayTrigger = pdMS_TO_TICKS(PERIOD_TRIGGER_TASK), xDontBLock = 0;
    while (true)
    {
        vTaskDelay(xDelayTrigger);
        #if DEBUGLOG_MODE
            printf("<---- LAUNCHING MEASUREMENT STAGE ---->\r\n");
        #endif
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
        #if DEBUGLOG_MODE
            printf("<---- READING ENVIROMENTAL SENSORS ---->\r\n");
        #endif
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
        #if DEBUGLOG_MODE
            printf("<---- READING ON BOARD SENSORS ---->\r\n");
        #endif

        light_manager.readSensorsAndUpdate();

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
        #if DEBUGLOG_MODE
            printf("<---- READING EDGE COMPUTING BOARDS ---->\r\n");
        #endif
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
        #if DEBUGLOG_MODE
            printf("<---- LIGHT MANAGER TASK ---->\r\n");
        #endif 

        if(isPirTriggered){
            #if DEBUGLOG_MODE
                printf("INCREASING LAMP BRIGHTNESS\r\n");
            #endif
            light_manager.setPWM(MOVEMENT_DETECTED);
        }
        else{
            light_manager.setPWM(NO_MOVEMENT);
        }

        //TODO: Comprobar si hay correcta operación, de lo contrario iniciar comunicación de emergencia
        uint8_t result = light_manager.lightFeedBackCheck();
        switch (result)
        {
        case EXPECTED:
            break;
        
        case UNEXPECTED:
            break;
        
        case NOT_WORKING:
            break;

        default:
            break;
        }

        //Set the values of the light telemetry structure
        lightData_t lightData;
        lightData.measuredLight = light_manager.getMeasuredLight();
        lightData.expectedLight = light_manager.getExpectedLight();
        lightData.operationMode = light_manager.getOperationMode();
        lightData.status = light_manager.getStatus();

        //Sent telemtry into a queue
        xQueueSendToBack(xLightData, &lightData, 0);

        #if DEBUGLOG_MODE
            printf("Sending data to light queue\r\n");
            printf("Measured light: %d, Expected light: %d, Operation mode: %d, Status: %d\r\n",
                lightData.measuredLight, lightData.expectedLight, lightData.operationMode, lightData.status  
            );
        #endif

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
        #if DEBUGLOG_MODE
            printf("<---- SUPPLY MANAGER TRIGGERED ---->\r\n");
        #endif
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
        #if DEBUGLOG_MODE
            printf("<---- PACKET MANAGER TRIGGERED ---->\r\n");
        #endif

        //Create the structures to receive the data
        lightData_t lightData;

        //Receive the data form queue's
        xQueueReceive(xLightData, &lightData, 0);

        #if DEBUGLOG_MODE
            printf("Receiving from light queue\r\n");
            printf("Measured light: %d, Expected light: %d, Operation mode: %d, Status: %d\r\n",
                lightData.measuredLight, lightData.expectedLight, lightData.operationMode, lightData.status  
            );
        #endif

        //TODO: Create .JSON

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
        #if DEBUGLOG_MODE
            printf("<---- COM MANAGER TRIGGERED ---->\r\n");
        #endif

        //If the task has been woken by the magnetic switch interrupt
        if(systemAlert){
            switch (systemAlert)
            {
            case BOX_OPEN:
                #if DEBUGLOG_MODE
                    printf("SENDING EMERGENCY PACKET\r\n");
                #endif
                systemAlert = NO_ALERT;
                break;
            
            default:
                break;
            }

        //TODO: Create structures to receive data

        //TODO: Receive data from queue's

        

        }
    }
}
