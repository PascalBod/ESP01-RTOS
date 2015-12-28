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

#include "c_types.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "esp_common.h"

#include "message.h"

// AP to connect to.
#define AP_SSID     "PBAP"
#define AP_PASSWORD "passwordpbap"

// Automaton states.
typedef enum {
	STA_WAIT_CONNECT,
	STA_WAIT_DISCONN,
	STA_WAIT_TIMEOUT
} STATE;

// Duration for waiting on queue.
static const portTickType QUEUE_WAIT = 180000 / portTICK_RATE_MS; // 3 minutes
// Duration for waiting before trying to reconnect.
static const portTickType RECONN_WAIT = 10000 / portTICK_RATE_MS;  // 10 s

// Current automaton state
static STATE ucCurrentState;
// Pointer to TCP client's queue.
static xQueueHandle *pxTCPClientTaskQueue;
// Our own queue.
static xQueueHandle xWiFiStationTaskQueue;
// Message sent by us from main loop.
static ITMessage_t xSentMessage;
static portBASE_TYPE xRetStatusQueue;
// Message sent to us by Wi-Fi event callback.
static ITMessage_t xWiFiMessage;
static portBASE_TYPE xWiFiRetStatus;
// Message sent to us by timer callback.
static ITMessage_t xTimerMessage;
static portBASE_TYPE xTimerRetStatus;
// Received message.
static ITMessage_t xRecMessage;
// Configuration block.
static struct station_config xConfig;
static uint8 ucRetStatusBool;
// Timer for reconnection attempts.
static xTimerHandle xTimer;
static uint8 ucTimerId;
static portBASE_TYPE xRetStatusTimer;

/**
 * Timer callback function.
 */
static void vTimerCallback(xTimerHandle pxTimer) {

	// Send associated message to our queue.
	xTimerMessage.ucId = MSG_WIFI_ENDW;
	xTimerRetStatus = xQueueSendToBack(
			xWiFiStationTaskQueue,
			&xTimerMessage,
			0
			);
	if (xTimerRetStatus == errQUEUE_FULL) {
		printf("*** Wi-Fi *** Can't write message to queue for timeout.\r\n");
		// In a production-class application, we would put here
		// some code to go back to a consistent global state.
	}

}

/**
 * Callback for Wi-Fi events. For a connection to an access point,
 * events appear in this order:
 * - EVENT_STAMODE_CONNECTED
 * - EVENT_STAMODE_GOT_IP
 * When disconnecting:
 * - EVENT_STAMODE_DISCONNECTED
 * and then the ESP8266 tries to reconnect every few seconds.
 */
void handle_wifi_event(System_Event_t *event) {

	switch(event->event_id) {
	case EVENT_STAMODE_SCAN_DONE:
		printf("*** Wi-Fi *** Scan done.\r\n");
		break;
	case EVENT_STAMODE_CONNECTED:
		printf("*** Wi-Fi *** Connected.\r\n");
		break;
	case EVENT_STAMODE_DISCONNECTED:
		printf("*** Wi-Fi *** Disconnected.\r\n");
		// Send associated message to our queue.
		xWiFiMessage.ucId = MSG_WIFI_DISCONN;
		xWiFiRetStatus = xQueueSendToBack(
				xWiFiStationTaskQueue,
				&xWiFiMessage,
				0
				);
		if (xWiFiRetStatus == errQUEUE_FULL) {
			printf("*** Wi-Fi *** Can't write message to queue for event.\r\n");
			// In a production-class application, we would put here
			// some code to go back to a consistent global state.
		}
		break;
	case EVENT_STAMODE_AUTHMODE_CHANGE:
		printf("*** Wi-Fi *** AuthMode change.\r\n");
		break;
	case EVENT_STAMODE_GOT_IP:
		printf("*** Wi-Fi *** Got IP.\r\n");
		// Send associated message to our queue.
		xWiFiMessage.ucId = MSG_GOT_IP;
		xWiFiRetStatus = xQueueSendToBack(
				xWiFiStationTaskQueue,
				&xWiFiMessage,
				0
				);
		if (xWiFiRetStatus == errQUEUE_FULL) {
			printf("*** Wi-Fi *** Can't write message to queue for event.\r\n");
			// In a production-class application, we would put here
			// some code to go back to a consistent global state.
		}
		break;
	case EVENT_STAMODE_DHCP_TIMEOUT:
		printf("*** Wi-Fi *** DHCP timeout.\r\n");
		break;
	case EVENT_SOFTAPMODE_STACONNECTED:
	case EVENT_SOFTAPMODE_STADISCONNECTED:
	case EVENT_SOFTAPMODE_PROBEREQRECVED:
		printf("*** Wi-Fi *** Inconsistent event: %d\r\n", event->event_id);
		break;
	default:
		printf("*** Wi-Fi *** Unknown event: %d\r\n", event->event_id);
	}
}

/**
 * Wi-Fi station task.
 *
 * Tries to connect to pre-defined access point. On success, sends an
 * MSG_WIFI_OK message to TCP client task. When failure, sends an
 * MSG_WIFI_KO message to TCP client task.
 *
 * Parameter is a pointer to TCP client's queue.
 *
 * We could have written this code without using a message queue. But
 * program architecture would have been a little more complex. This
 * queue is used for Wi-Fi station task internal events only.
 *
 */
void vWiFiStationTask(void *pvParameters) {

	pxTCPClientTaskQueue = pvParameters;

	ucCurrentState = STA_WAIT_CONNECT;

	ucTimerId = 1;

    // Create our queue. Space for 3 messages only.
    xWiFiStationTaskQueue = xQueueCreate(3, sizeof(ITMessage_t));
    if (xWiFiStationTaskQueue == 0) {
    	printf("*** Wi-Fi *** Can't create station task's queue.\r\n");
    	vTaskDelete(NULL);
    	return;
    }

	// Initialize messages: no data for them.
	xSentMessage.xData.pxNoData = NULL;
	xWiFiMessage.xData.pxNoData = NULL;

	// Create timer we will use to trigger reconnection attempts.
	xTimer = xTimerCreate(
			"WiFiTimer",
			RECONN_WAIT,
			pdFALSE,				// one-shot
			(void *)(&ucTimerId),
			vTimerCallback);
	if (xTimer == NULL) {
		printf("*** Wi-Fi *** Timer was not created.\r\n");
		vTaskDelete(NULL);
		return;
	}

	// Prepare configuration.
	memset(&xConfig, 0, sizeof(xConfig));
	sprintf(xConfig.ssid, AP_SSID);
	sprintf(xConfig.password, AP_PASSWORD);
	ucRetStatusBool = wifi_station_set_config(&xConfig);
	if (ucRetStatusBool != 1) {
		printf("*** Wi-Fi *** Configuration failed.\r\n");
		vTaskDelete(NULL);
		return;
	}
	ucRetStatusBool = wifi_set_event_handler_cb(handle_wifi_event);
	if (ucRetStatusBool != 1) {
		printf("*** Wi-Fi *** Event handler setting failed.\r\n");
		vTaskDelete(NULL);
		return;
	}
	ucRetStatusBool = wifi_station_connect();
	if (ucRetStatusBool != 1) {
		printf("*** Wi-Fi *** Connection request failed.\r\n");
		vTaskDelete(NULL);
		return;
	}

	// Task automaton.
	while(1) {

		// Wait on receiving queue.
		xRetStatusQueue = xQueueReceive(
				xWiFiStationTaskQueue,
				&xRecMessage,
				QUEUE_WAIT
				);
		if (xRetStatusQueue == pdFALSE) {
			printf("*** Wi-Fi *** End of wait on task's queue.\r\n");
			continue;
		}

		switch(ucCurrentState) {

		case STA_WAIT_CONNECT:
			switch(xRecMessage.ucId) {
			case MSG_GOT_IP:
				// Connected and IP available: inform TCP client.
				xSentMessage.ucId = MSG_WIFI_OK;
				xRetStatusQueue = xQueueSendToBack(
						*pxTCPClientTaskQueue,
						&xSentMessage,
						0
						);
				if (xRetStatusQueue == errQUEUE_FULL) {
					printf("*** Wi-Fi *** Can't write message to queue.\r\n");
					// In a production-class application, we would put here
					// some code to go back to a consistent global state.
				}
				ucCurrentState = STA_WAIT_DISCONN;
				break;
			case MSG_WIFI_DISCONN:
				// We didn't get Wi-Fi connectivity.
				// Stop automatic reconnection attemps,
				// and wait a while before trying to reconnect.
				ucRetStatusBool = wifi_station_disconnect();
				if (ucRetStatusBool != 1) {
					printf("*** Wi-Fi *** Disconnection request failed.\r\n");
					vTaskDelete(NULL);
					return;
				}
				// Start timer.
				xRetStatusTimer = xTimerStart(xTimer, 0);
				if (xRetStatusTimer != pdPASS) {
					printf("*** Wi-Fi *** Timer was not started.\r\n");
					xTimerDelete(xTimer, 0);
					vTaskDelete(NULL);
					return;
				}
				// Next state.
				ucCurrentState = STA_WAIT_TIMEOUT;
				break;
			default:
				// Ignore any other message.
				printf("*** Wi-Fi *** Other message for STA_WAIT_CONNECT: %d.\r\n",
						xRecMessage.ucId);
			}
			break;

		case STA_WAIT_DISCONN:
			switch(xRecMessage.ucId) {
			case MSG_WIFI_DISCONN:
				// We lost Wi-Fi connectivity. Inform TCP client,
				// stop automatic reconnection attemps,
				// and wait a while before trying to reconnect.
				xSentMessage.ucId = MSG_WIFI_KO;
				xRetStatusQueue = xQueueSendToBack(
						*pxTCPClientTaskQueue,
						&xSentMessage,
						0
						);
				if (xRetStatusQueue == errQUEUE_FULL) {
					printf("*** Wi-Fi *** Can't write message to queue.\r\n");
					// In a production-class application, we would put here
					// some code to go back to a consistent global state.
				}
				// Stop automatic reconnection.
				ucRetStatusBool = wifi_station_disconnect();
				if (ucRetStatusBool != 1) {
					printf("*** Wi-Fi *** Disconnection request failed.\r\n");
					vTaskDelete(NULL);
					return;
				}
				// Start timer.
				xRetStatusTimer = xTimerStart(xTimer, 0);
				if (xRetStatusTimer != pdPASS) {
					printf("*** Wi-Fi *** Timer was not started.\r\n");
					xTimerDelete(xTimer, 0);
					vTaskDelete(NULL);
					return;
				}
				// Next state.
				ucCurrentState = STA_WAIT_TIMEOUT;
				break;
			default:
				// Ignore any other message.
				printf("*** Wi-Fi *** Other message for STA_WAIT_DISCONN: %d.\r\n",
						xRecMessage.ucId);
			}
			break;

		case STA_WAIT_TIMEOUT:
			switch(xRecMessage.ucId) {
			case MSG_WIFI_ENDW:
				// Request connection.
				ucRetStatusBool = wifi_station_connect();
				if (ucRetStatusBool != 1) {
					printf("*** Wi-Fi *** Connection request failed.\r\n");
					vTaskDelete(NULL);
					return;
				}
				// Next state.
				ucCurrentState = STA_WAIT_CONNECT;
				break;
			default:
				// Ignore any other message.
				printf("*** Wi-Fi *** Other message for STA_WAIT_TIMEOUT: %d.\r\n",
						xRecMessage.ucId);
			}
			break;

		default:
			printf("*** Wi-Fi *** Unknown state: %d.\r\n", ucCurrentState);
			// Go back to known state.
			ucRetStatusBool = wifi_station_connect();
			if (ucRetStatusBool != 1) {
				printf("*** Wi-Fi *** Connection request failed.\r\n");
				vTaskDelete(NULL);
				return;
			}
			ucCurrentState = STA_WAIT_CONNECT;
		}

	}

}
