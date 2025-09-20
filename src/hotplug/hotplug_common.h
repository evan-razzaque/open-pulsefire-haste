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

#ifndef HOTPLUG_COMMON_PRIVATE_H
#define HOTPLUG_COMMON_PRIVATE_H

#ifndef HOTPLUG_COMMON_PRIVATE
#error "You cannot directly include hotplug_common.h"
#endif

#ifdef _WIN32
#include <cfgmgr32.h>
#define DEVICE_ARRIVE (CM_NOTIFY_ACTION_DEVICEINTERFACEARRIVAL)
#define DEVICE_REMOVE (CM_NOTIFY_ACTION_DEVICEREMOVECOMPLETE)
#else
#include <libusb-1.0/libusb.h>
#define DEVICE_ARRIVE (LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED)
#define DEVICE_REMOVE (LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT)
#endif

#include <stdint.h>

#include "types.h"
#include "hotplug/hotplug.h"

/**
 * @brief Updates the mouse connection when the device connection changes.
 *
 * @param mouse A mouse_data struct
 * @param product_id The product id of the mouse
 * @param event The device event that has occured
 *
 */
void update_mouse_connection_type(mouse_hotplug_data *hotplug_data, uint16_t product_id, int event);

/**
 * @brief Updates the mouse connection status when the device is detached.
 *
 * @param mouse A mouse_data struct
 */
void update_device_connection_detached(mouse_hotplug_data *hotplug_data);

/**
 * @brief Updates the mouse connection status when the device is attached.
 *
 * @param mouse A mouse_data struct
 * @param last_connection_type The last CONNECTION_TYPE of the mouse
 */
void update_device_connection_attached(mouse_hotplug_data *hotplug_data, CONNECTION_TYPE last_connection_type);

#endif