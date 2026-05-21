#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

SemaphoreHandle_t xMutex = NULL;

void app_main(void)
{
  xMutex = xSemaphoreCreateMutexWithCaps(void);

  if (xMutex != NULL){
    printf("success\n");
  }
  else{
    printf("ball\n");
  }
}
