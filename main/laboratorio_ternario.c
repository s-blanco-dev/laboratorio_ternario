#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "rgb_led.h"
#include "sdkconfig.h"
#include "task_a.h"
#include "task_b.h"
#include "time.h"
#include <stdio.h>

#define QUEUE_LENGTH 10

static char *GAK = "MAIN";

static SemaphoreHandle_t color_mutex = NULL;
static QueueHandle_t command_queue = NULL;

static uint task_a_prio = tskIDLE_PRIORITY + 1;
static uint task_b_prio = tskIDLE_PRIORITY + 3;
static uint task_c_prio = tskIDLE_PRIORITY + 2;

static color_t color = {130, 1, 120};

TaskHandle_t task_a_handler = NULL;
TaskHandle_t task_b_handler = NULL;
TaskHandle_t task_c_handler = NULL;

void app_main(void) {
    /*
     * ===== Mutex =====
     *
     * El mutex es como el defensor de current_color
     * En el caso de task_a, lo toma para leer el color, segun Santiago Blanco
     * En task_c, lo toma para escribir el color.
     * En el caso de task_b, solo Piero sabe.
     */
    color_mutex = xSemaphoreCreateMutex();
    if (color_mutex == NULL) {
        ESP_LOGE(GAK, "No se pudo crear el mutex");
        return;
    }

    /*
     * ==== Queue ====
     *
     * Es la cola que transporta elementos de tipo led_command_t
     *
     * Justamente la idea de task_c es que recibe comandos
     * con xQueueRecieve().
     */
    command_queue = xQueueCreate(QUEUE_LENGTH, sizeof(led_command_t));
    if (command_queue == NULL) {
        ESP_LOGE(GAK, "Fue imposible crear la cola");
        return;
    }

    /*
     * ====== TASK A ======
     */
    led_strip_t *led = NULL;
    ESP_ERROR_CHECK(led_rgb_init(&led));

    static task_a_params_t task_a_params;
    task_a_params.color_mutex = color_mutex;
    task_a_params.led_strip = led;
    task_a_params.current_color = color;
    xTaskCreate(blink_led_task, "TASK_A", CONFIG_ESP32_PTHREAD_TASK_STACK_SIZE_DEFAULT, &task_a_params, task_a_prio,
                &task_a_handler);

    /*
     * ====== TASK B ======
     */
    static task_b_params_t task_b_params;
    task_b_params.command_queue = command_queue;
    xTaskCreate(task_b, "TASK_B", CONFIG_ESP32_PTHREAD_TASK_STACK_SIZE_DEFAULT, &task_b_params, task_b_prio,
                &task_b_handler);
}
