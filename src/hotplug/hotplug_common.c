/*
 * This file is part of the open-pulsefire-haste project
 * Copyright (C) 2025  Evan Razzaque
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. 
 */

#define HOTPLUG_COMMON_PRIVATE

#include <hidapi/hidapi.h>
#include <adwaita.h>
#include "hotplug/hotplug_common.h"

void update_device_connection_detached(mouse_hotplug_data *hotplug_data) {
    mouse_data *mouse = hotplug_data->mouse;

    if (mouse->connection_type == CONNECTION_TYPE_WIRED) return;

    if (mouse->dev != NULL) {
        printf("disconnect\n");
        hid_close(mouse->dev);
        mouse->dev = NULL;
        mouse->state = DISCONNECTED;
    }
    
    bool device_connected = mouse->connection_type == CONNECTION_TYPE_WIRELESS;
    if (device_connected) mouse->state = RECONNECT;

    hotplug_data->hotplug_callback(device_connected, hotplug_data->hotplug_callback_user_data);
}

void update_device_connection_attached(mouse_hotplug_data *hotplug_data, CONNECTION_TYPE last_connection_type) {
    // Wireless was plugged in with wired, do nothing
    if (last_connection_type == CONNECTION_TYPE_WIRED) return;

    mouse_data *mouse = hotplug_data->mouse;

    // Wired plugged in with wireless, reconnect to wired
    if (last_connection_type == CONNECTION_TYPE_WIRELESS) {
        hid_close(mouse->dev);
        mouse->dev = NULL;
    }

    mouse->state = RECONNECT;
    hotplug_data->hotplug_callback(true, hotplug_data->hotplug_callback_user_data);
}

void update_mouse_connection_type(mouse_hotplug_data *hotplug_data, uint16_t product_id, int event) {
    mouse_data *mouse = hotplug_data->mouse;

    CONNECTION_TYPE connection_type = 
        (product_id == PID_WIRED)?
        CONNECTION_TYPE_WIRED:
        CONNECTION_TYPE_WIRELESS;

    g_mutex_lock(mouse->mutex);

    CONNECTION_TYPE last_connection_type = mouse->connection_type;
    
    if (event == DEVICE_REMOVE) {
        mouse->connection_type -= connection_type;
        update_device_connection_detached(hotplug_data);
    } else {
        mouse->connection_type += connection_type;
        update_device_connection_attached(hotplug_data, last_connection_type);
    }

    g_mutex_unlock(mouse->mutex);
}