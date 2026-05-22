#include "task_c.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"

// etiqueta para identificar mensajes en consola
static const char *BALL = "TASK_C";

// varianles infamememnte privadas para task_c.c
static SemaphoreHandle_t s_color_mutex = NULL;
static rgb_color_t *s_current_color = NULL;

static void color_timer_callback(TimerHandle_t xTimer)
{
  rgb_color_t *new_color = (rgb_color_t *)pvTimerGetTimerID(xTimer); 
}


