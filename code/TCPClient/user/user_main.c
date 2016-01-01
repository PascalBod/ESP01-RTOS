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

#include <taskMessage.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_common.h"

#include "WiFiStationTask.h"
#include "TCPClientTask.h"
#include "counterTask.h"


/**
 * Local global variables.
 */
static xQueueHandle xTCPClientTaskQueue;

void user_init(void)
{

	// Prepare the UART for transmitting trace messages.
	uart_init_new();

    printf("SDK version:%s\r\n", system_get_sdk_version());
    printf("TCPClient - V0.5\r\n");

    wifi_set_opmode(STATION_MODE);

    // Create TCP client's queue. Space for 3 messages only.
    xTCPClientTaskQueue = xQueueCreate(3, sizeof(ITMessage_t));
    if (xTCPClientTaskQueue == 0) {
    	printf("*** Can't create TCP client task's queue.\r\n");
    	return;
    }

    // Start tasks.
    xTaskCreate(vWiFiStationTask, "WiFiStationTask", 256, &xTCPClientTaskQueue, 2, NULL);
    xTaskCreate(vTCPClientTask, "TCPClientTask", 256, &xTCPClientTaskQueue, 2, NULL);
    xTaskCreate(vCounterTask, "counterTask", 256, &xTCPClientTaskQueue, 2, NULL);

}

