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

#include "esp_common.h"
#include "esp_libc.h"
#include "esp_wifi.h"

/*
 * Global variables used by functions. They are declared here so that they do
 * not consume space from stack.
 */
Event_SoftAPMode_StaConnected_t *info_staConnected_ptr;
Event_SoftAPMode_StaDisconnected_t *info_staDisconnected_ptr;
Event_SoftAPMode_ProbeReqRecved_t *info_probeReqRecved_ptr;

/**
 * Callback for Wi-Fi events.
 */
void handle_wifi_event(System_Event_t *event) {

	switch(event->event_id) {
	case EVENT_STAMODE_SCAN_DONE:
	case EVENT_STAMODE_CONNECTED:
	case EVENT_STAMODE_DISCONNECTED:
	case EVENT_STAMODE_AUTHMODE_CHANGE:
	case EVENT_STAMODE_GOT_IP:
	case EVENT_STAMODE_DHCP_TIMEOUT:
		printf("*** Inconsistent event: %d\r\n", event->event_id);
		break;
	case EVENT_SOFTAPMODE_STACONNECTED:
		info_staConnected_ptr = &(event->event_info.sta_connected);
		printf("*** Connected station: %x.%x.%x.%x.%x.%x - %d\r\n",
				info_staConnected_ptr->mac[0],
				info_staConnected_ptr->mac[1],
				info_staConnected_ptr->mac[2],
				info_staConnected_ptr->mac[3],
				info_staConnected_ptr->mac[4],
				info_staConnected_ptr->mac[5],
				info_staConnected_ptr->aid);
		break;
	case EVENT_SOFTAPMODE_STADISCONNECTED:
		info_staDisconnected_ptr = &(event->event_info.sta_disconnected);
		printf("*** Disconnected station: %x.%x.%x.%x.%x.%x - %d\r\n",
				info_staDisconnected_ptr->mac[0],
				info_staDisconnected_ptr->mac[1],
				info_staDisconnected_ptr->mac[2],
				info_staDisconnected_ptr->mac[3],
				info_staDisconnected_ptr->mac[4],
				info_staDisconnected_ptr->mac[5],
				info_staDisconnected_ptr->aid);
		break;
	case EVENT_SOFTAPMODE_PROBEREQRECVED:
		info_probeReqRecved_ptr = &(event->event_info.ap_probereqrecved);
		printf("*** Probe request received: %x.%x.%x.%x.%x.%x - %d\r\n",
				info_probeReqRecved_ptr->mac[0],
				info_probeReqRecved_ptr->mac[1],
				info_probeReqRecved_ptr->mac[2],
				info_probeReqRecved_ptr->mac[3],
				info_probeReqRecved_ptr->mac[4],
				info_probeReqRecved_ptr->mac[5],
				info_probeReqRecved_ptr->rssi);
		break;
	default:
		printf("*** Unknown event: %d", event->event_id);
	}
}

void user_set_softap_config(void) {
   struct softap_config config;

   wifi_softap_get_config(&config);

   memset(config.ssid, 0, 32);
   memset(config.password, 0, 64);
   memcpy(config.ssid, "SoftAP", 7);
   memcpy(config.password, "12345678", 8);
   config.authmode = AUTH_WPA_WPA2_PSK;
   config.ssid_len = 0;
   config.max_connection = 4;

   wifi_softap_set_config(&config);

}
void user_init(void)
{

	uart_init_new();

    printf("SDK version:%s\r\n", system_get_sdk_version());
    printf("SoftAP - V0.3\r\n");

    wifi_set_opmode(STATIONAP_MODE);

    user_set_softap_config();
    wifi_set_event_handler_cb(handle_wifi_event);

}

