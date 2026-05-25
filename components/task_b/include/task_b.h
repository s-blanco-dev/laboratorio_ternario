#ifndef TASK_B_H
#define TASK_B_H

#include "stdbool.h"
#include "rgb_led.h"

void task_b(void *pvParameters);
static bool parse_color(const char *token, color_t *out);
static void uart_init(void);

#endif
