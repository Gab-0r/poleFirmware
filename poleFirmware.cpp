#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "timers.h"
#include "event_groups.h"
#include "pico/time.h"
#include "lightManager.h"

#define PERIOD_TRIGGER_TASK 5000UL          //Period to launch the next measurement stage

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
/*                 System parameters               */
/***************************************************/
#define PIR_PULSE_DURATION      (uint32_t)10000U


/***************************************************/
/*    Section related to pico pins and hardware    */
/***************************************************/

/* Pico pins */
#define PIR_SENSOR_PIN  (uint16_t)15U


void hardwareInit();
void pirTriggered(uint /*gpio*/, uint32_t /*event_mask*/);

bool isPirTriggered = false;
int64_t pirOff(alarm_id_t /*id*/, void* /*user_data*/);
alarm_id_t pirAlarmId = 0;


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
    hardwareInit();
    

    xTaskCreate(periodicTriggerTask, "PeriodicTrigger", 1000, NULL, 1, NULL);
    xTaskCreate(readEnviromentSensorsTask, "ReadEnvSensors", 1000, NULL, 2, NULL);
    xTaskCreate(readOnBoardSensorsTask, "ReadOnBoard", 1000, NULL, 2, NULL);
    xTaskCreate(readEdgeBoardsTask, "ReadEdge", 1000, NULL, 2, NULL);
    xTaskCreate(lightManagerTask, "LighManage", 1000, NULL, 2, NULL);
    xTaskCreate(supplyManagerTask, "SupplyManage", 1000, NULL, 2, NULL);
    xTaskCreate(packetManagerTask, "PacketManage", 1000, NULL, 2, NULL);
    xTaskCreate(comManagerTask, "COMManage", 1000, NULL, 2, NULL);

    xEventGroup = xEventGroupCreate();

    vTaskStartScheduler();

    while(1){};
}

void hardwareInit(){

    stdio_init_all();

    /* Pins declaration */
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    /* Pins with IRQ */
    //gpio_set_irq_enabled_with_callback(PIR_SENSOR_PIN, GPIO_IRQ_EDGE_FALL, true, &pirOff);
    gpio_set_irq_enabled_with_callback(PIR_SENSOR_PIN, GPIO_IRQ_EDGE_RISE, true, &pirTriggered);
}

void pirTriggered(uint /*gpio*/, uint32_t /*event_mask*/){
    printf("Movimiento detectado\r\n");
    gpio_put(PICO_DEFAULT_LED_PIN, 1);
    isPirTriggered = true;
    if (pirAlarmId == 0)
    {
        pirAlarmId =  add_alarm_in_ms(PIR_PULSE_DURATION, pirOff, NULL, true);
        //TODO: Set event bits to trigger light manager task
    }
    else{
        cancel_alarm(pirAlarmId);
        pirAlarmId =  add_alarm_in_ms(PIR_PULSE_DURATION, pirOff, NULL, true);
        //TODO: Set event bits to trigger light manager task
    }
}

int64_t pirOff(alarm_id_t id, void* user_data){
    printf("Sin movimiento\r\n");
    isPirTriggered = false;
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
