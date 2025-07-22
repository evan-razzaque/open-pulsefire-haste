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

#include "hotplug/hotplug.h"

/**
 * @brief Updates the mouse connection when the device connection changes.
 * 
 * @param mouse A mouse_data struct
 * @param product_id The product id of the mouse
 * @param event The device event that has occured
 *
 */
void update_mouse_connection_type(mouse_data *mouse, uint16_t product_id, int event);

/**
 * @brief Updates the mouse connection status when the device is detached.
 * 
 * @param mouse A mouse_data struct
 */
void update_device_connection_detached(mouse_data *mouse);

/**
 * @brief Updates the mouse connection status when the device is attached.
 * 
 * @param mouse A mouse_data struct
 * @param last_connection_type The last CONNECTION_TYPE of the mouse
 */
void update_device_connection_attached(mouse_data *mouse, CONNECTION_TYPE last_connection_type);

#endif