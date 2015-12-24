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

// Wait on queue period.
static const portTickType xQueueRecWait = 180000 / portTICK_RATE_MS; // 3 minutes

// Idle state wait time.
static const portTickType xTimerDelay = 10000 / portTICK_RATE_MS;  // 10 s

/**
 * Local global (!) variables.
 */
// Current automaton state
static STATE ucCurrentState;

// Our message queue.
static xQueueHandle *pxTaskQueue;
static portBASE_TYPE xRetStatusQueue;

// Message received from task 1.
static ITMessage_t xRecMessage;

// To check task stack.
static portBASE_TYPE xStackMark;

// Number of MSG_TOBECOUNTED messages received.
static uint8 ucNbRecMsg;

// Timer used to exit from idle state after a while.
static xTimerHandle xTimer;
static uint8 ucTimerId;
static portBASE_TYPE xRetStatusTimer;

// Message sent to our own queue.
static ITMessage_t xSentMessage;
static portBASE_TYPE xRetStatusQueue;

/**
 * Timer callback function.
 */
static void vTimerCallback(xTimerHandle pxTimer) {

	printf("*** Task 2 timer expired.\r\n");
	// Inform task that it must exit from idle state.
	xRetStatusQueue = xQueueSendToBack(
			*pxTaskQueue,
			&xSentMessage,
			0
			);
	if (xRetStatusQueue == errQUEUE_FULL) {
		print("*** Can't write message to queue 2.\r\n");
	}


}

/**
 * Task 2.
 *
 * Stays in idle state for xTimerDelay. Then waits for
 * messages sent by task 1. After three received messages,
 * goes back to idle state.
 *
 * Parameter is a pointer to receiving message queue.
 *
 */
void vTask2(void *pvParameters) {

	// Task initialization.
	pxTaskQueue = pvParameters;

	ucCurrentState = STA_IDLE;

	ucNbRecMsg = 0;

	// Initialize message. No data for it.
	xSentMessage.ucId = MSG_EXITIDLE;
	xSentMessage.xData.pxNoData = NULL;

	// Create timer.
	ucTimerId = 1;
	xTimer = xTimerCreate(
			"timer2",
			xTimerDelay,
			pdFALSE,				// one-shot
			(void *)(&ucTimerId),
			vTimerCallback);

	if (xTimer == NULL) {
		printf("*** timer2 was not created.\r\n");
		vTaskDelete(NULL);
		return;
	}
	// At this stage, we got a timer. Try to start it.
	xRetStatusTimer = xTimerStart(xTimer, 0);
	if (xRetStatusTimer != pdPASS) {
		printf("*** timer2 was not started.\r\n");
		xTimerDelete(xTimer, 0);
		vTaskDelete(NULL);
		return;
	}
	// At this stage, the timer has been started.
	printf("*** timer 2 started.\r\n");

	// Task automaton.
	while(true) {

		// Wait on receiving queue.
		xRetStatusQueue = xQueueReceive(
				*pxTaskQueue,
				&xRecMessage,
				xQueueRecWait
				);
		if (xRetStatusQueue == pdFALSE) {
			print("*** End of wait on task 2's queue.\r\n");
			continue;
		}
		// Display minimum stack available space.
		xStackMark = uxTaskGetStackHighWaterMark(NULL);
		printf("*** %d\r\n", xStackMark);

		switch(ucCurrentState) {

		case STA_IDLE:
			switch (xRecMessage.ucId) {
			case MSG_TOBECOUNTED:
				printf("*** Message to be counted ignored.\r\n");
				break;
			case MSG_EXITIDLE:
				printf("*** Exiting idle state.\r\n");
				ucCurrentState = STA_WAIT_MESSAGE;
				break;
			default:
				print("*** Unknown message for STA_IDLE: %d.\r\n", xRecMessage.ucId);
			}
			break;

		case STA_WAIT_MESSAGE:
			switch (xRecMessage.ucId) {
			case MSG_TOBECOUNTED:
				printf("*** Message value: %d\r\n", xRecMessage.xData.xToBeCounted.ucValue);
				ucNbRecMsg++;
				if (ucNbRecMsg == 3) {
					printf("*** 3 messages received, going back to idle state.\r\n");
					ucNbRecMsg = 0;
					// Restart timer.
					xRetStatusTimer = xTimerStart(xTimer, 0);
					if (xRetStatusTimer != pdPASS) {
						printf("*** timer2 was not started.\r\n");
						xTimerDelete(xTimer, 0);
						vTaskDelete(NULL);
						return;
					}
					// At this stage, the timer has been started.
					printf("*** timer 2 started.\r\n");
					//
					ucCurrentState = STA_IDLE;
				}
				break;
			default:
				printf("*** Unknown message for STA_IDLE: %d.\r\n", xRecMessage.ucId);
			}
			break;

		default:
			printf("*** Unknown state for task 2: %d\r\n", ucCurrentState);
			// Restart timer.
			xRetStatusTimer = xTimerStart(xTimer, 0);
			if (xRetStatusTimer != pdPASS) {
				printf("*** timer2 was not started.\r\n");
				xTimerDelete(xTimer, 0);
				vTaskDelete(NULL);
				return;
			}
			// At this stage, the timer has been started.
			printf("*** timer 2 started.\r\n");
			//
			ucCurrentState = STA_IDLE;

		}

	}

}
