#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "led_strip.h"
#include "rgb_led.h"
#include <stdint.h>

typedef struct {
    color_t color;
    uint32_t delay_s;
} led_command_t;

// a ruego de piero
typedef struct {
    SemaphoreHandle_t color_mutex;
    color_t current_color;
    led_strip_t *led_strip;
} task_a_params_t;

typedef struct {
    QueueHandle_t command_queue;
} task_b_params_t;

typedef struct {
    QueueHandle_t command_queue;
    SemaphoreHandle_t color_mutex;
    color_t *current_color;
} task_c_params_t;

#endif
