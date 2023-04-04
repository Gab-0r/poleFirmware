#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "timers.h"


#define PERIOD_TRIGGER_TASK 5000UL          //Period to launch the next measurement stage

/* Bits to trigger tasks */


/* Tasks */
void periodicTrigger(void *pvParameters);   //Periodic trigger for the measurement stage


int main()
{
    stdio_init_all();

    xTaskCreate(periodicTrigger, "PeriodicTrigger", 1000, NULL, 1, NULL);
    vTaskStartScheduler();

    while(1){};
}

void periodicTrigger(void *pvParameters){
    const TickType_t xDelayTrigger = pdMS_TO_TICKS(PERIOD_TRIGGER_TASK), xDontBLock = 0;
    while (true)
    {
        vTaskDelay(xDelayTrigger);
        printf("<---- LAUNCHING MEASUREMENT STAGE ---->\r\n");
    }
    
}