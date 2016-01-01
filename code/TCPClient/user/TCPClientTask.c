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

#include "lwip/sockets.h"

#include "esp_common.h"

#include "taskMessage.h"
#include "protMessage.h"

// Automaton states.
typedef enum {
	STA_WAIT_WIFI,
	STA_WAIT_COUNTER,
	STA_WAIT_TIMEOUT
} STATE;

// Duration for waiting on queue.
static const portTickType QUEUE_WAIT = 180000 / portTICK_RATE_MS; // 3 minutes
// Duration for waiting before trying to reconnect.
static const portTickType RECONN_WAIT = 10000 / portTICK_RATE_MS;  // 10 s

// Remote host.
#define REMOTE_IP   "127.0.0.1"
#define REMOTE_PORT 50000

// Current automaton state
static STATE ucCurrentState;
// Pointer to our queue.
static xQueueHandle *pxTCPClientTaskQueue;
static portBASE_TYPE xRetStatusQueue;
// Received message.
static ITMessage_t xRecMessage;
// Message sent to us by timer callback.
static ITMessage_t xTimerMessage;
static portBASE_TYPE xTimerRetStatus;
// Socket to remote host.
static int uClientSocket;
static struct sockaddr_in xRemoteHost;
static int uRetStatus;
// Timer for reconnection attempts.
static xTimerHandle xTimer;
static uint8 ucTimerId;
static portBASE_TYPE xRetStatusTimer;
// Buffer for COUNTER protocol message.
static uint8 pucCounterProtMsg[PROT_MSG_COUNTER_L];
static uint8 ucRetStatusProtMsg;
static uint8 ucByteToBeSent;
// Length of sent data.
static int uSentDataL;
// To check task stack.
static portBASE_TYPE xStackMark;

/**
 * Timer callback function.
 */
static void vTimerCallback(xTimerHandle pxTimer) {

	// Send associated message to our queue.
	xTimerMessage.ucId = MSG_TCP_ENDW;
	xTimerRetStatus = xQueueSendToBack(
			*pxTCPClientTaskQueue,
			&xTimerMessage,
			0
			);
	if (xTimerRetStatus == errQUEUE_FULL) {
		printf("*** TCP *** Can't write message to queue for timeout.\r\n");
		// In a production-class application, we would put here
		// some code to go back to a consistent global state.
	}

}

/**
 * Tries to connect to remote host.
 *
 * Side effect: updates ucCurrentState, possibly starts timer.
 *
 */
static void tryToConnect(void) {

	// Create socket.
	uClientSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (uClientSocket == -1) {
		// Well, we are in real trouble.
		vTaskDelete(NULL);
		return;
	}
	// Request connection.
	uRetStatus = connect(uClientSocket, (struct sockaddr *)(&xRemoteHost),
			sizeof(struct sockaddr));
	if (uRetStatus == -1) {
		switch (errno) {
		case ETIMEDOUT:
			printf("*** TCP *** Time out on connect.\r\n");
			break;
		case ECONNRESET:
			printf("*** TCP *** Connection reset by peer.\r\n");
			break;
		case ECONNREFUSED:
			printf("*** TCP *** Connection refused.\r\n");
			break;
		case ENETUNREACH:
			printf("*** TCP *** Remote host unreachable.\r\n");
			break;
		case EADDRINUSE:
			printf("*** TCP *** Address already in use");
			break;
		default:
			printf("*** TCP *** Connection failed: %d.\r\n", errno);
		}
		close(uClientSocket);
		// Start timer.
		xRetStatusTimer = xTimerStart(xTimer, 0);
		if (xRetStatusTimer != pdPASS) {
			printf("*** TCP *** Timer was not started. Exiting.\r\n");
			xTimerDelete(xTimer, 0);
			vTaskDelete(NULL);
			return;
		}
		// Next state.
		ucCurrentState = STA_WAIT_TIMEOUT;
		return;
	}
	// At this stage, we are connected to remote host.
	// Next state.
	printf("*** TCP *** Connected.\r\n");
	ucCurrentState = STA_WAIT_COUNTER;

}

/**
 * Tries to send counter received in task message.
 *
 * Must be called from STA_WAIT_COUNTER state only.
 *
 * Side effects: updates ucCurrentState, ends task on fatal error,
 * possibly starts timer.
 *
 */
static void tryToSendCounter() {

	// Create a COUNTER protocol message, frame it and send it.
	ucRetStatusProtMsg = makeCounter(xRecMessage.xData.xCounter.ucValue,
			pucCounterProtMsg, PROT_MSG_COUNTER_L);
	if (ucRetStatusProtMsg == -1) {
		// We are in real trouble.
		printf("*** TCP *** Can't create COUNTER message. Exiting.\r\n");
		close(uClientSocket);
		vTaskDelete(NULL);
		return;
	}
	// At this stage, we have our message. Frame it and send it.
	ucByteToBeSent = frame1(PROT_MSG_COUNTER_L);
	uSentDataL = write(uClientSocket, &ucByteToBeSent, 1);
	if (uSentDataL != 1) {
		printf("*** TCP *** Frame LSB not sent.\r\n");
		close(uClientSocket);
		// Start timer.
		xRetStatusTimer = xTimerStart(xTimer, 0);
		if (xRetStatusTimer != pdPASS) {
			printf("*** TCP *** Timer was not started. Exiting.\r\n");
			xTimerDelete(xTimer, 0);
			vTaskDelete(NULL);
			return;
		}
		// Next state.
		ucCurrentState = STA_WAIT_TIMEOUT;
		return;
	}
	// At this stage, first frame byte was sent.
	ucByteToBeSent = frame2(PROT_MSG_COUNTER_L);
	uSentDataL = write(uClientSocket, &ucByteToBeSent, 1);
	if (uSentDataL != 1) {
		printf("*** TCP *** Frame MSB not sent.\r\n");
		close(uClientSocket);
		// Start timer.
		xRetStatusTimer = xTimerStart(xTimer, 0);
		if (xRetStatusTimer != pdPASS) {
			printf("*** TCP *** Timer was not started. Exiting.\r\n");
			xTimerDelete(xTimer, 0);
			vTaskDelete(NULL);
			return;
		}
		// Next state.
		ucCurrentState = STA_WAIT_TIMEOUT;
		return;
	}
	// At this stage, second frame byte was sent. Now, send protocol message.
	uSentDataL = write(uClientSocket, pucCounterProtMsg, PROT_MSG_COUNTER_L);
	if (uSentDataL != PROT_MSG_COUNTER_L) {
		printf("*** TCP *** Protocol message not sent.\r\n");
		close(uClientSocket);
		// Start timer.
		xRetStatusTimer = xTimerStart(xTimer, 0);
		if (xRetStatusTimer != pdPASS) {
			printf("*** TCP *** Timer was not started. Exiting.\r\n");
			xTimerDelete(xTimer, 0);
			vTaskDelete(NULL);
			return;
		}
		// Next state.
		ucCurrentState = STA_WAIT_TIMEOUT;
		return;
	}
	// At this stage, counter was sent. Stay in STA_WAIT_COUNTER state.
	printf("*** TCP *** Counter sent to remote host.\r\n");

}

/**
 * TCP client task.
 *
 * Waits for MSG_WIFI_OK from WI-Fi station task. Then tries to open
 * a socket TCP to remote host. If success, waits for MSG_COUNTER
 * periodic messages sent by the counter task.
 *
 * Parameter is a pointer to our message queue.
 *
 */

void vTCPClientTask(void *pvParameters) {

	pxTCPClientTaskQueue = pvParameters;

	ucCurrentState = STA_WAIT_WIFI;

	ucTimerId = 2;

	// Initialize our timer message.
	xTimerMessage.xData.pxNoData = NULL;

	// Prepare address block.
	bzero(&xRemoteHost, sizeof(struct sockaddr_in));
	xRemoteHost.sin_family = AF_INET;
	xRemoteHost.sin_addr.s_addr = inet_addr(REMOTE_IP);
	xRemoteHost.sin_port = htons(REMOTE_PORT);

	// Create timer we will use to trigger reconnection attempts.
	xTimer = xTimerCreate(
			"TCPTimer",
			RECONN_WAIT,
			pdFALSE,				// one-shot
			(void *)(&ucTimerId),
			vTimerCallback);
	if (xTimer == NULL) {
		printf("*** TCP *** Timer was not created. Exiting.\r\n");
		vTaskDelete(NULL);
		return;
	}

	// Task automaton.
	while(1) {

		// Wait on receiving queue.
		xRetStatusQueue = xQueueReceive(
				*pxTCPClientTaskQueue,
				&xRecMessage,
				QUEUE_WAIT
				);
		if (xRetStatusQueue == pdFALSE) {
			printf("*** TCP *** End of wait on task's queue.\r\n");
			continue;
		}
		// Display minimum stack available space.
		xStackMark = uxTaskGetStackHighWaterMark(NULL);
		printf("*** TCP *** HWM: %d.\r\n", xStackMark);

		switch(ucCurrentState) {

		case STA_WAIT_WIFI:
			switch(xRecMessage.ucId) {
			case MSG_WIFI_OK:
				// Wi-Fi connectivity available, let's try to connect.
				tryToConnect();
				break;
			default:
				// Ignore any other message.
				printf("*** TCP *** Other message for STA_WAIT_WIFI: %d.\r\n",
						xRecMessage.ucId);
			}
			break;

		case STA_WAIT_COUNTER:
			switch(xRecMessage.ucId) {
			case MSG_COUNTER:
				tryToSendCounter();
				break;
			case MSG_WIFI_KO:
				close(uClientSocket);
				// Next state.
				ucCurrentState = STA_WAIT_WIFI;
				break;
			default:
				// Ignore any other message.
				printf("*** TCP *** Other message for STA_WAIT_COUNTER: %d.\r\n",
						xRecMessage.ucId);
			}
			break;

		case STA_WAIT_TIMEOUT:
			switch(xRecMessage.ucId) {
			case MSG_TCP_ENDW:
				// Retry connection.
				tryToConnect();
				break;
			case MSG_WIFI_KO:
				// Stop timer, and wait for Wi-Fi.
				xRetStatusTimer = xTimerStop(xTimer, 0);
				if (xRetStatusTimer != pdPASS) {
					printf("*** TCP *** Timer was not stopped. Exiting.\r\n");
					xTimerDelete(xTimer, 0);
					vTaskDelete(NULL);
					return;
				}
				ucCurrentState = STA_WAIT_WIFI;
				break;
			default:
				// Ignore any other message.
				printf("*** TCP *** Other message for STA_WAIT_TIMEOUT: %d.\r\n",
						xRecMessage.ucId);
			}
			break;

		default:
			// We are in trouble.
			printf("*** TCP *** Unknown state: %d. Exiting.\r\n", ucCurrentState);
			xTimerDelete(xTimer, 0);
			vTaskDelete(NULL);
			return;
		}

	}

}
