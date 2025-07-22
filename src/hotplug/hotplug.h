#ifndef HOTPLUG_H
#define HOTPLUG_H

#define HOTPLUG_COMMON_PRIVATE

#include <hidapi/hidapi.h>

#include "types.h"
#include "hotplug/hotplug_common.h"

/**
 * @brief An opaque struct for storing platform specific hotplug listener data.
 * 
 */
typedef struct hotplug_listener_data hotplug_listener_data;

/**
 * @brief A struct for storing mouse hotplug data.
 * 
 */
struct mouse_hotplug_data {
    hotplug_listener_data *listener_data;
    mouse_data *mouse;
    GThread *hotplug_thread;
} typedef mouse_hotplug_data;

void setup_mouse_removal_callbacks(mouse_hotplug_data *hotplug_data, struct hid_device_info *dev_list);

/**
 * @brief Initializes the hotplug listener.
 * 
 * @param hotplug_data A pointer to a mouse_hotplug_data struct to store the hotplug listener into
 * @param mouse The mouse_data object to store into the mouse_hotplug_data object 
 */
void hotplug_listener_init(mouse_hotplug_data *hotplug_data, mouse_data *mouse);

/**
 * @brief Deinitialize the hotplug listener 
 * and free any resources associated with it.
 * 
 * @param hotplug The mouse_hotplug_data struct containing the hotplug listener
 */
void hotplug_listener_exit(mouse_hotplug_data *listener_data);

/**
 * @brief Initializes the device removal callbacks for each connected device.
 * Only used on windows.
 * 
 * @param hotplug_data The mouse_hotplug_data struct containing the hotplug listener
 * @param dev_list A list of device info objects for each connected device interface
 */
void setup_mouse_removal_callbacks(mouse_hotplug_data *hotplug_data, struct hid_device_info *dev_list);

#endif
