#ifndef HOTPLUG_COMMON_PRIVATE_H
#define HOTPLUG_COMMON_PRIVATE_H

#ifndef HOTPLUG_COMMON_PRIVATE
#error "You cannot directly include hotplug_common.h"
#endif

#include "hotplug/hotplug.h"

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