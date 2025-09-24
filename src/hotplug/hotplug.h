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

#ifndef HOTPLUG_H
#define HOTPLUG_H
#define HOTPLUG_COMMON_PRIVATE

#include <hidapi/hidapi.h>

#include "types.h"

/**
 * @brief An opaque struct for storing platform specific hotplug listener data.
 *
 */
typedef struct hotplug_listener_data hotplug_listener_data;

/**
 * @brief Function pointer type for mouse hotplug events.
 */
typedef void (*hotplug_listener_callback)(bool connected, void *user_data);

/**
 * @brief A struct for storing mouse hotplug data.
 */
struct mouse_hotplug_data {
    hotplug_listener_data *listener_data;
    mouse_data *mouse;
    GThread *hotplug_thread;
    hotplug_listener_callback hotplug_callback;
    void* hotplug_callback_user_data;
} typedef mouse_hotplug_data;

/**
 * @brief Initializes the hotplug listener.
 *
 * @param hotplug_data A pointer to a mouse_hotplug_data struct to store the hotplug listener into
 */
void hotplug_listener_init(mouse_hotplug_data *hotplug_data);

/**
 * @brief Deinitialize the hotplug listener
 * and free any resources associated with it.
 *
 * @param hotplug The mouse_hotplug_data struct containing the hotplug listener
 */
void hotplug_listener_exit(mouse_hotplug_data *listener_data);

/**
 * @brief Initializes the device removal callbacks for each connected device.
 *
 * @param hotplug_data The mouse_hotplug_data struct containing the hotplug listener
 * @param dev_list A list of device info objects for each connected device interface
 */
void setup_mouse_removal_callbacks(mouse_hotplug_data *hotplug_data, struct hid_device_info *dev_list);

#endif
