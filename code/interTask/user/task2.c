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

#include "miscPB.h"
#include "message.h"

// Automaton states.
typedef enum {
	STA_IDLE,
	STA_WAIT_MESSAGE,
} STATE;

const portTickType xQueueRecWait = 180000 / portTICK_RATE_MS; // 3 minutes

/**
 * Local global variables.
 */
static STATE ucCurrentState;
static portBASE_TYPE xRetStatusQueue;
static ITMessage_t xMessage;
static portBASE_TYPE xStackMark;

/**
 * Task 2.
 *
 * Parameter is a pointer to receiving message queue.
 *
 */
void vTask2(void *pvParameters) {

	xQueueHandle *pxTaskQueue = pvParameters;

	ucCurrentState = STA_IDLE;

	while(true) {

		// Wait on receiving queue.
		xRetStatusQueue = xQueueReceive(
				*pxTaskQueue,
				&xMessage,
				xQueueRecWait
				);
		if (xRetStatusQueue == pdFALSE) {
			print("*** End of wait on task 2's queue.\r\n");
			continue;
		}

		switch(ucCurrentState) {

		case STA_IDLE:
			printf("*** message value: %d\r\n", xMessage.xData.xToBeCounted.ucValue);
			// Display minimum stack available space.
			xStackMark = uxTaskGetStackHighWaterMark(NULL);
			printf("*** %d\r\n", xStackMark);
			break;

		case STA_WAIT_MESSAGE:
			break;

		default:
			printf("*** Unknown state for task 2: %d\r\n", ucCurrentState);

		}

	}

}
