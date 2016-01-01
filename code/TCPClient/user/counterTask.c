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
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "esp_common.h"

#include "taskMessage.h"

// Counter message period.
static const portTickType COUNTER_PERIOD = 30000 / portTICK_RATE_MS;  // 30 s
// Task wait period.
static const portTickType TASK_PERIOD = 180000 / portTICK_RATE_MS;  // 3 min

// Pointer to TCP client's queue.
static xQueueHandle *pxTCPClientTaskQueue;
// Message sent by us.
static ITMessage_t xSentMessage;
static portBASE_TYPE xRetStatusQueue;
// Timer.
static xTimerHandle xTimer;
static uint8 ucTimerId;
static portBASE_TYPE xRetStatusTimer;

/**
 * Timer callback function.
 */
static void vTimerCallback(xTimerHandle pxTimer) {

	xRetStatusQueue = xQueueSendToBack(
			*pxTCPClientTaskQueue,
			&xSentMessage,
			0
			);
	if (xRetStatusQueue == errQUEUE_FULL) {
		printf("*** Counter *** Can't write message to queue.\r\n");
	}
	printf("*** Counter *** Message sent to TCP client task: %d.\r\n",
			xSentMessage.xData.xCounter.ucValue);
	// Increment wrapping counter.
	xSentMessage.xData.xCounter.ucValue++;

}

/**
 * Counter task.
 *
 * Increments a wrapping counter from 0 to 255 every 30 seconds. After
 * incrementation, a counter message is sent to TCP client task.
 *
 * Parameter is a pointer to TCP client's queue.
 *
 */
void vCounterTask(void *pvParameters) {

	pxTCPClientTaskQueue = pvParameters;

	// Initialize message.
	xSentMessage.ucId = MSG_COUNTER;
	xSentMessage.xData.xCounter.ucValue = 0;

	// Create timer.
	ucTimerId = 3;
	xTimer = xTimerCreate(
			"counterTimer",
			COUNTER_PERIOD,
			pdTRUE,					// Auto-reload.
			(void *)(&ucTimerId),
			vTimerCallback);

	if (xTimer == NULL) {
		printf("*** Counter *** Timer was not created.\r\n");
		vTaskDelete(NULL);
		return;
	}
	// At this stage, we got a timer. Try to start it.
	xRetStatusTimer = xTimerStart(xTimer, 0);
	if (xRetStatusTimer != pdPASS) {
		printf("*** Counter *** Timer was not started.\r\n");
		xTimerDelete(xTimer, 0);
		vTaskDelete(NULL);
		return;
	}
	// At this stage, the timer has been started.

	// Do not exit task.
	while(1) {
		vTaskDelay(TASK_PERIOD);
	}


}
