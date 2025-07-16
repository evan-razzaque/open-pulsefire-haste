#ifndef HOTPLUG_H
#define HOTPLUG_H

#include <stdint.h>
#include <hidapi/hidapi.h>

#include "types.h"

/**
 * @brief An opaque struct for storing platform specific hotplug data.
 * 
 */
typedef struct hotplug_listener_data hotplug_listener_data;

/**
 * @brief Initializes the hotplug listener.
 * 
 * @param dev A pointer to a hid_device pointer 
 * @return hotplug_listener_data* the hotplug_listener_data object
 */
hotplug_listener_data* hotplug_listener_init(hid_device **dev, MOUSE_STATE *mouse_state);

/**
 * @brief Deinitialize the hotplug listener 
 * and free any resources associated with it.
 * 
 * @param listener The hotplug_listener_data object associated with the hotplug listener
 */
void hotplug_listener_exit(hotplug_listener_data *listener_data);

#endif
