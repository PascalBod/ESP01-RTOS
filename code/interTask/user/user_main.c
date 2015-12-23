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
#include "esp_common.h"

#include "message.h"
#include "task1.h"
#include "task2.h"

/**
 * Local global variables.
 */
static xQueueHandle xTask2Queue;

void user_init(void)
{

	// Prepare the UART for transmitting trace messages.
	uart_init_new();

    printf("SDK version:%s\r\n", system_get_sdk_version());
    printf("InterTask - V0.2\r\n");

    // Create task 2's queue. Space for 3 messages only.
    xTask2Queue = xQueueCreate(3, sizeof(ITMessage_t));
    if (xTask2Queue == 0) {
    	printf("*** Can't create task 2's queue.\r\n");
    	return;
    }

    // Start tasks.
    xTaskCreate(vTask1, "task1", 512, &xTask2Queue, 2, NULL);
    xTaskCreate(vTask2, "task2", 256, &xTask2Queue, 2, NULL);

}

