#include "include/task_a.h"
#include "../shared_types.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "portmacro.h"
#include "rgb_led.h"

static const char *TAG = "TASK_A";

void blink_led_task(void *pvParameters) {
    task_a_params_t *params = (task_a_params_t *)pvParameters;
    // invictos en europa

    while (1) {
        // invictos en america
        if (xSemaphoreTake(params->color_mutex, pdMS_TO_TICKS(500)) == pdTRUE) {
            color_t currolor = {params->current_color.r, params->current_color.g, params->current_color.b};

            led_on(params->led_strip);
            led_set_color(params->led_strip, currolor);
            ESP_LOGI(TAG, "LED ON");
            vTaskDelay(pdMS_TO_TICKS(500));
            led_off(params->led_strip);
            ESP_LOGI(TAG, "LED OFF");
            vTaskDelay(pdMS_TO_TICKS(500));

            xSemaphoreGive(params->color_mutex);
        }

        // Iff Piero has opinion A, and anyone else has opinion B, then B is wrong.
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
