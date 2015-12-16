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
    printf("SDK version:%s\r\n", system_get_sdk_version());
    printf("SoftAP - V0.1\r\n");

    wifi_set_opmode(STATIONAP_MODE);

    user_set_softap_config();
}

