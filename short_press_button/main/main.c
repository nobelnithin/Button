
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

#define BTN_UP GPIO_NUM_18

xQueueHandle BTN_UPQueue;

uint64_t pre_time = 0;
uint64_t intr_time = 0;
uint64_t curr_time = 0;

void BTN_UPTask(void *params)
{
    gpio_set_direction(BTN_UP, GPIO_MODE_INPUT);
    gpio_set_intr_type(BTN_UP, GPIO_INTR_NEGEDGE);
    int BTN_NUMBER = 0;

    while (1)
    {
        if (xQueueReceive(BTN_UPQueue, &BTN_NUMBER, portMAX_DELAY))
        {
            // Wait for button release
            while (gpio_get_level(BTN_UP) == 0)
            {
                curr_time = esp_timer_get_time();
            }

            // Check if the button was pressed for a short duration
            if (curr_time - intr_time < 1000000) // Adjust the time threshold for short press detection as needed
            {
                ESP_LOGI("NO TAG", "Short Press Detected");
            }

            xQueueReset(BTN_UPQueue);
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

void app_main(void)
{
    BTN_UPQueue = xQueueCreate(10, sizeof(int));

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN_UP, BTN_UP_interrupt_handler, (void *)BTN_UP);

    xTaskCreate(BTN_UPTask, "BTN_Task", 2048, NULL, 1, NULL);
}
