#include "task_c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/portable.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "portmacro.h"

// etiqueta para identificar mensajes en consola
static const char *BALL = "TASK_C";

// varianles infamememnte privadas para task_c.c
static SemaphoreHandle_t s_color_mutex = NULL;
static color_t *s_current_color = NULL;

/**
 * Natutalmente el callback es una funcion que freeRTOS llama automaticamente cuando pasa algo
 * En este caso, cuando vence el timer, freeRTOS llama a color_timer_callback.
 * Es lo que debe pasar cuando se cumple el delay del comando.
 */
static void color_timer_callback(TimerHandle_t xTimer) {
    // freeRTOS devuelve un void *, por eso hacemos cast (rgb_color_t *)
    color_t *new_color = (color_t *)pvTimerGetTimerID(xTimer);

    if (new_color == NULL) {
        ESP_LOGE(BALL, "pvTimerID NULL, consultar a Xabi Alonso");
        xTimerDelete(xTimer, 0);
        return;
    }

    if (xSemaphoreTake(s_color_mutex, portMAX_DELAY) == pdTRUE) {
        s_current_color->r = new_color->r;
        s_current_color->g = new_color->g;
        s_current_color->b = new_color->b;

        xSemaphoreGive(s_color_mutex);

        ESP_LOGI(BALL, "Color aplicado: R=%d G=%d B=%d", new_color->r, new_color->g, new_color->b);
    }

    vPortFree(new_color);
    xTimerDelete(xTimer, 0);
}

void task_c(void *pvParameters) {
    task_c_params_t *params = (task_c_params_t *)pvParameters;

    if (params == NULL) {
        ESP_LOGE(BALL, "Parametros NULL, consultar a Nico Calarco");
        vTaskDelete(NULL);
        return;
    }

    s_color_mutex = params->color_mutex;
    s_current_color = params->current_color;

    if (s_color_mutex == NULL || s_current_color == NULL || params->command_queue == NULL) {
        ESP_LOGE(BALL, "Parametros invalidos");
        vTaskDelete(NULL);
        return;
    }

    // esta variable va a guardar el comando recibido desde la consola
    led_command_t command;

    while (1) {
        if (xQueueReceive(params->command_queue, &command, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(BALL, "Comando recibido R=%d G=%d B=%d delay=%lu s", command.color.r, command.color.g,
                     command.color.b, command.delay_s);

            // se reserva un bloque independiente de memoria para cada timer:
            color_t *timer_color = pvPortMalloc(sizeof(color_t));

            if (timer_color == NULL) {
                ESP_LOGE(BALL, "No se pudo reservar memoria para el color.");
                continue;
            }

            // copia el color al bloque reservated
            *timer_color = command.color;

            TickType_t timer_period = pdMS_TO_TICKS(command.delay_s * 1000);

            TimerHandle_t timer = xTimerCreate("color_timer", timer_period, pdFALSE, timer_color, color_timer_callback);

            if (timer == NULL) {
                ESP_LOGE(BALL, "No se pudo crear el timer.");
                vPortFree(timer_color); // importante para no tener fugas de gas (memoria).
                continue;
            }

            if (xTimerStart(timer, 0) != pdPASS) {
                ESP_LOGE(BALL, "Fue imposible inciar el timer, por favor no consultar a Joel Gak.");
                xTimerDelete(timer, 0);
                vPortFree(timer_color);
                continue;
            }

            ESP_LOGI(BALL, "Timer creado para aplicar color en %lu segundos", command.delay_s);
        }
    }
}
