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

#include "esp_common.h"

// AP to connect to.
#define AP_SSID     "PBAP"
#define AP_PASSWORD "passwordpbap"

// Wait time for task.
static const portTickType TASK_WAIT = 60000 / portTICK_RATE_MS;  // 60 s

// Pointer to TCP client's queue.
static xQueueHandle *pxTCPClientTaskQueue;
// Configuration block.
static struct station_config xConfig;
static int retStatusBool;

/**
 * Callback for Wi-Fi events.
 */
void handle_wifi_event(System_Event_t *event) {

	switch(event->event_id) {
	case EVENT_STAMODE_SCAN_DONE:
		printf("*** Scan done.\r\n");
		break;
	case EVENT_STAMODE_CONNECTED:
		printf("*** Connected.\r\n");
		break;
	case EVENT_STAMODE_DISCONNECTED:
		printf("*** Disconnected.\r\n");
		break;
	case EVENT_STAMODE_AUTHMODE_CHANGE:
		printf("*** AuthMode change.\r\n");
		break;
	case EVENT_STAMODE_GOT_IP:
		printf("*** Got IP.\r\n");
		break;
	case EVENT_STAMODE_DHCP_TIMEOUT:
		printf("*** DHCP timeout.\r\n");
		break;
	case EVENT_SOFTAPMODE_STACONNECTED:
	case EVENT_SOFTAPMODE_STADISCONNECTED:
	case EVENT_SOFTAPMODE_PROBEREQRECVED:
		printf("*** Inconsistent Wi-Fi event: %d\r\n", event->event_id);
		break;
	default:
		printf("*** Unknown Wi-Fi event: %d\r\n", event->event_id);
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
 */
void vWiFiStationTask(void *pvParameters) {

	pxTCPClientTaskQueue = pvParameters;

	// Prepare configuration.
	memset(&xConfig, 0, sizeof(xConfig));
	sprintf(xConfig.ssid, AP_SSID);
	sprintf(xConfig.password, AP_PASSWORD);
	retStatusBool = wifi_station_set_config(&xConfig);
	if (!retStatusBool) {
		printf("*** Configuration failed.\r\n");
		vTaskDelete(NULL);
		return;
	}
	retStatusBool = wifi_station_connect();
	if (!retStatusBool) {
		printf("*** Connection failed.\r\n");
		vTaskDelete(NULL);
		return;
	}
	retStatusBool = wifi_set_event_handler_cb(handle_wifi_event);
	if (!retStatusBool) {
		printf("*** Event handler setting failed.\r\n");
		vTaskDelete(NULL);
		return;
	}

	// Do not exit task.
	while(1) {
		vTaskDelay(TASK_WAIT);
		printf("*** Wi-Fi station task loop.\r\n");

	}

}
