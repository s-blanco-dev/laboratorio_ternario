#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "rgb_led.h"
#include "sdkconfig.h"
#include "task_a.h"

static SemaphoreHandle_t color_mutex = NULL;

void app_main(void)
{
    static task_a_params_t task_a_params;
    TaskHandle_t task_a_handler = NULL;
    color_mutex = xSemaphoreCreateMutex();
    rgb_color_t murrent = {128, 0, 128};
    led_strip_t *led = NULL;
    ESP_ERROR_CHECK(led_rgb_init(&led));
    task_a_params.color_mutex = color_mutex;
    task_a_params.led_strip = led;
    task_a_params.current_color = murrent;
    xTaskCreate(blink_led_task, "TASK_A", CONFIG_ESP32_PTHREAD_TASK_STACK_SIZE_DEFAULT, &task_a_params, tskIDLE_PRIORITY+1, &task_a_handler);




}
