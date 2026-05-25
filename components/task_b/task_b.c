#include "../shared_types.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_rom_uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"

#define MAX_LINE_LEN 64

#define UART_TX_PIN 17
#define UART_RX_PIN 18
#define UART_NUM UART_NUM_0

static const char *TAG = "TASK_B";

static void uart_init(void) {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };

    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));

    const int uart_buffer_size = (1024 * 2);
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, uart_buffer_size, uart_buffer_size, 10, NULL, 0));
    ESP_LOGI(TAG, "UART initialized");
}

// <COLOR> <SECONDS>\n
static bool parse_color(const char *token, color_t *out) {
    if (strcmp(token, "ROJO") == 0) {
        *out = (color_t){255, 0, 0};
        return true;
    } else if (strcmp(token, "VERDE") == 0) {
        *out = (color_t){0, 255, 0};
        return true;
    } else if (strcmp(token, "AZUL") == 0) {
        *out = (color_t){0, 0, 255};
        return true;
    } else if (strcmp(token, "BLANCO") == 0) {
        *out = (color_t){255, 255, 255};
        return true;
    }
    return false;
}

void task_b(void *pvParameters) {
    task_b_params_t *params = (task_b_params_t *)pvParameters;
    QueueHandle_t queue = params->command_queue;

    uart_init();

    char line[MAX_LINE_LEN];
    int pos = 0;

    while (1) {
        uint8_t byte;

        // uso delay maximo para bloquear (se mide en RTOS ticks)
        int n = uart_read_bytes(UART_NUM_0, &byte, 1, 0xffffffff);

        if (n <= 0) {
            continue;
        }

        if (byte == '\n' || byte == '\r') {
            // si tengo un \n o \r y la posicion es 0, entonces estoy en una linea vacia
            // la skipeo
            if (pos == 0) {
                continue;
            }

            // si tengo un \n o \r y la posicion NO es 0, entonces estoy en el ultimo caracter
            // agrego el null terminator y reseteo la posicion a 0
            line[pos] = '\0';
            pos = 0;

            // <COLOR> <SECONDS>
            char color_str[16];
            unsigned int delay_s;

            // escaneo, si hay mas de 2 cosas entonces el formato esta mal
            if (sscanf(line, "%15s %u", color_str, &delay_s) != 2) {
                ESP_LOGW(TAG, "bad format: '%s'", line);
                continue;
            }

            // extraigo comando
            led_command_t cmd;
            if (!parse_color(color_str, &cmd.color)) {
                ESP_LOGW(TAG, "unknown color: '%s'", color_str);
                continue;
            }
            cmd.delay_s = delay_s;

            if (xQueueSend(queue, &cmd, pdMS_TO_TICKS(100)) != pdTRUE) {
                ESP_LOGW(TAG, "queue full");
                continue;
            }

            // confirmacion
            char ack[64];
            snprintf(ack, sizeof(ack), "OK: %s en %u\n", color_str, delay_s);
            uart_write_bytes(UART_NUM_0, ack, strlen(ack));

            ESP_LOGI(TAG, "ENQUEUED: %s in %u", color_str, delay_s);
        } else {
            // acumulo
            if (pos < MAX_LINE_LEN - 1) {
                line[pos++] = (char)byte;
            } else {
                // dropeo si es demasiado largo
                ESP_LOGW(TAG, "line too long");
                pos = 0;
            }
        }
    }
}
