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
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "freertos/task.h"

#include "miscPB.h"
#include "message.h"

// Transmit message period.
static const portTickType xTimerDelay = 3000 / portTICK_RATE_MS;  // 3 s

// Task 1 wait period.
static const portTickType xTaskDelay = 60000 / portTICK_RATE_MS;  // 60 s

// Timer used to send a message to task 2 on a periodic basis.
static xTimerHandle xTimer;
static uint8 ucTimerId;
static portBASE_TYPE xRetStatusTimer;

// Pointer to task 2's queue.
static xQueueHandle *pxTaskQueue;

// Message sent to task 2's queue.
static ITMessage_t xMessage;
static portBASE_TYPE xRetStatusQueue;

/**
 * Instead of using an infinite loop and vTaskDelay() as in rtos1 example,
 * we use an auto-reload timer, just to check how to use a FreeRTOS timer.
 */

/**
 * Timer callback function.
 */
static void vTimerCallback(xTimerHandle pxTimer) {

	printf("*** Task 1 timer expired.\r\n");
	//Send message to task 2.
	xRetStatusQueue = xQueueSendToBack(
			*pxTaskQueue,
			&xMessage,
			0
			);
	if (xRetStatusQueue == errQUEUE_FULL) {
		print("*** Can't write message to queue 1.\r\n");
	}
	if (xMessage.xData.xToBeCounted.ucValue == UINT8_MAX) {
		xMessage.xData.xToBeCounted.ucValue = 0;
	} else {
		xMessage.xData.xToBeCounted.ucValue++;
	}

}

/**
 * Task 1.
 *
 * Starts the auto-reload timer which sends a message
 * to task 2 on a periodic basis.
 *
 * Parameter is a pointer to task 2's queue.
 *
 */
void vTask1(void *pvParameters) {

	pxTaskQueue = pvParameters;

	// Initialize message.
	xMessage.ucId = MSG_TOBECOUNTED;
	xMessage.xData.xToBeCounted.ucValue = 0;

	// Create timer.
	ucTimerId = 1;
	xTimer = xTimerCreate(
			"timer1",
			xTimerDelay,
			pdTRUE,					// auto-reload
			(void *)(&ucTimerId),
			vTimerCallback);

	if (xTimer == NULL) {
		printf("*** timer1 was not created.\r\n");
		vTaskDelete(NULL);
		return;
	}
	// At this stage, we got a timer. Try to start it.
	xRetStatusTimer = xTimerStart(xTimer, 0);
	if (xRetStatusTimer != pdPASS) {
		printf("*** timer1 was not started.\r\n");
		xTimerDelete(xTimer, 0);
		vTaskDelete(NULL);
		return;
	}
	// At this stage, the timer has been started.
	printf("*** timer 1 started.\r\n");

	// Do not exit task.
	while(true) {
		vTaskDelay(xTaskDelay);
		// Printing the message below makes the application crash.
		// Increased stack but this did not correct the problem.
		// To be investigated.
		//print("*** Task 1 loop.\r\n");

	}

}
