#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "driver/i2c.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"


#define CONFIG_SDA_GPIO 1
#define CONFIG_SCL_GPIO 2
#define CONFIG_RESET_GPIO 0

#define BTN_UP GPIO_NUM_14
#define BTN_DOWN GPIO_NUM_17
#define BTN_PWR GPIO_NUM_15


xQueueHandle BTN_UPQueue;
xQueueHandle BTN_DOWNQueue;
xQueueHandle BTN_PWRQueue;

uint64_t pre_time = 0;
uint64_t intr_time = 0;
uint64_t curr_time = 0;
int lastState = 1;
int currentState;



void BTN_UPTask(void *params)
{
    gpio_set_direction(BTN_UP, GPIO_MODE_INPUT);
    gpio_set_intr_type(BTN_UP, GPIO_INTR_NEGEDGE);
    int BTN_NUMBER = 0;

    while (1)
    {
        if (xQueueReceive(BTN_UPQueue, &BTN_NUMBER, portMAX_DELAY))
        {
            
            printf("Button UP  pressed\n");
            xQueueReset(BTN_UPQueue);
        }
    }
}

void BTN_DOWNTask(void *params)
{
    gpio_set_direction(BTN_DOWN, GPIO_MODE_INPUT);
    gpio_set_intr_type(BTN_DOWN, GPIO_INTR_NEGEDGE);
    int BTN_NUMBER = 0;

    while (1)
    {
        if (xQueueReceive(BTN_DOWNQueue, &BTN_NUMBER, portMAX_DELAY))
        {
            printf("Button DOWN pressed\n");
            xQueueReset(BTN_DOWNQueue);
        }
    }
}

void BTN_PWRTask(void *params)
{
    gpio_set_direction(BTN_PWR, GPIO_MODE_INPUT);
    gpio_set_intr_type(BTN_PWR, GPIO_INTR_NEGEDGE);
    int BTN_NUMBER = 0;

    while (1)
    {
        if (xQueueReceive(BTN_PWRQueue, &BTN_NUMBER, portMAX_DELAY))
        {
            
            printf("Button POWER  pressed\n");
            xQueueReset(BTN_PWRQueue);
        }
    }
}


static void IRAM_ATTR BTN_UP_interrupt_handler(void *args)
{
    int pinNumber = (int)args;

    if (esp_timer_get_time() - pre_time > 400000)
    {
        xQueueSendFromISR(BTN_UPQueue, &pinNumber, NULL);
        intr_time = esp_timer_get_time();
    }

    pre_time = esp_timer_get_time();
}

static void IRAM_ATTR BTN_PWR_interrupt_handler(void *args)
{
    int pinNumber = (int)args;

    if (esp_timer_get_time() - pre_time > 400000)
    {
        xQueueSendFromISR(BTN_PWRQueue, &pinNumber, NULL);
        intr_time = esp_timer_get_time();
    }

    pre_time = esp_timer_get_time();
}

static void IRAM_ATTR BTN_DOWN_interrupt_handler(void *args)
{
    int pinNumber = (int)args;

    if (esp_timer_get_time() - pre_time > 400000)
    {
        xQueueSendFromISR(BTN_DOWNQueue, &pinNumber, NULL);
        intr_time = esp_timer_get_time();
    }

    pre_time = esp_timer_get_time();
}





void app_main(void)
{

    BTN_UPQueue = xQueueCreate(10, sizeof(int));
    BTN_DOWNQueue = xQueueCreate(10, sizeof(int));
    BTN_PWRQueue = xQueueCreate(10, sizeof(int));
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN_UP, BTN_UP_interrupt_handler, (void *)BTN_UP);
    gpio_isr_handler_add(BTN_DOWN, BTN_DOWN_interrupt_handler, (void *)BTN_DOWN);
    gpio_isr_handler_add(BTN_UP, BTN_PWR_interrupt_handler, (void *)BTN_PWR);

    xTaskCreate(BTN_UPTask, "BTN_Task", 2048, NULL, 1, NULL);
    xTaskCreate(BTN_DOWNTask, "BTN_Task", 2048, NULL, 1, NULL);
    xTaskCreate(BTN_PWRTask, "BTN_Task", 2048, NULL, 1, NULL);

}
