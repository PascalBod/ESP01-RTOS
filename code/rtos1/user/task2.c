/*
 * task2.c
 *
 *  Created on: 6 déc. 2015
 *      Author: pascalbodin
 */

#include "freertos/portmacro.h"

#include "esp_common.h"

void task2(void *pvParameters) {

	const portTickType xDelay = 1000;  // 1 s

    for( ;; ) {
         printf("Hello from task2!\r\n");
         vTaskDelay(xDelay);
     }

}


