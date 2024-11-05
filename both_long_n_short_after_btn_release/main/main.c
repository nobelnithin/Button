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
#include "esp_sleep.h"

#define CONFIG_SDA_GPIO 1
#define CONFIG_SCL_GPIO 2
#define CONFIG_RESET_GPIO 0

#define BTN2 GPIO_NUM_21

xQueueHandle BTN2Queue;

uint64_t pre_time = 0;
uint64_t intr_time = 0;
uint64_t curr_time = 0;
int lastState = 1;
int currentState;

void enter_deep_sleep() {
    ESP_LOGI("NO TAG", "Entering deep sleep in 500ms");
    vTaskDelay(500 / portTICK_PERIOD_MS); // Wait for 500ms to ensure button is released
    esp_sleep_enable_ext0_wakeup(BTN2, 0); // Wake up when button is pressed (falling edge)
    esp_deep_sleep_start();
}

void BTN2Task(void *params)
{
    gpio_set_direction(BTN2, GPIO_MODE_INPUT);
    gpio_set_intr_type(BTN2, GPIO_INTR_NEGEDGE); // Trigger on falling edge (button press)
    int BTN_NUMBER = 0;

    while (1)
    {
        if (xQueueReceive(BTN2Queue, &BTN_NUMBER, portMAX_DELAY))
        {
            intr_time = esp_timer_get_time(); // Capture the time when the button is pressed

            // Wait for button release
            while (gpio_get_level(BTN2) == 0)
            {
                vTaskDelay(10 / portTICK_PERIOD_MS); // Small delay to avoid CPU hogging
            }

            // Calculate press duration after button is released
            curr_time = esp_timer_get_time();
            uint64_t press_duration = curr_time - intr_time;

            if (press_duration >= 1000000) // Check for long press duration
            {
                ESP_LOGI("NO TAG", "Long Press Detected");
                enter_deep_sleep();
            }
            else
            {
                ESP_LOGI("NO TAG", "Short Press Detected");
            }

            xQueueReset(BTN2Queue);
        }
    }
}

static void IRAM_ATTR BTN2_interrupt_handler(void *args)
{
    int pinNumber = (int)args;

    if (esp_timer_get_time() - pre_time > 400000)
    {
        xQueueSendFromISR(BTN2Queue, &pinNumber, NULL);
    }

    pre_time = esp_timer_get_time();
}

void app_main(void)
{
    BTN2Queue = xQueueCreate(10, sizeof(int));

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN2, BTN2_interrupt_handler, (void *)BTN2);

    xTaskCreate(BTN2Task, "BTN_Task", 2048, NULL, 1, NULL);
}
