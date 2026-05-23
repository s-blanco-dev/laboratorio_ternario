#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "rgb_led.h"
#include "led_strip.h"
#include <stdint.h>

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} rgb_color_t;

typedef struct {
  rgb_color_t color;
  uint32_t delay_s;
} led_command_t;

typedef struct {
  SemaphoreHandle_t color_mutex;
  rgb_color_t *current_color; 
  led_strip_t *led_strip;
} task_a_params_t;

typedef struct {
  QueueHandle_t command_queue;
} task_b_params_t;

typedef struct {
  QueueHandle_t command_queue;
  SemaphoreHandle_t color_mutex;
  rgb_color_t *current_color;
} task_c_params_t;

#endif
