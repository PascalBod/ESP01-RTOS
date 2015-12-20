/*
 *  Copyright (C) 2015 Pascal Bodin
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#include "miscPB.h"

const portTickType xTimerDelay = 3000 / portTICK_RATE_MS;  // 3 s
const portTickType xTaskDelay = 60000 / portTICK_RATE_MS;  // 60 s

xTimerHandle xTimer;
int timerId;
portBASE_TYPE rsTimer;

/**
 * Instead of using an infinite loop and vTaskDelay() as in rtos1 example,
 * we use an auto-reload timer.
 */

/**
 * Timer callback function.
 */
void vTimerCallback(xTimerHandle pxTimer) {

	printf("*** Task 1 timer expired.\r\n");

}

/**
 * Task 1.
 */
void task1(void *pvParameters) {

	timerId = 1;
	xTimer = xTimerCreate(
			"timer1",
			xTimerDelay,
			pdTRUE,
			(void *)timerId,
			vTimerCallback);

	if (xTimer == NULL) {
		printf("*** timer1 was not created.\r\n");
		return;
	}
	// At this stage, we got a timer. Try to start it.
	rsTimer = xTimerStart(xTimer, 0);
	if (rsTimer != pdPASS) {
		printf("*** timer1 was not started.\r\n");
		xTimerDelete(xTimer, 0);
		return;
	}
	// At this stage, the timer has been started.
	printf("*** timer 1 started.\r\n");

	// Do not exit task.
	while(true) {
		vTaskDelay(xTaskDelay);
	}

}
