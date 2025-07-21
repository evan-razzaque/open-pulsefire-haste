#define HOTPLUG_COMMON_PRIVATE
#include "hotplug/hotplug_common.h"

void update_device_connection_detached(mouse_data *mouse) {
    // Wireless was disconnected with wired plugged in, do nothing
    if (mouse->type == CONNECTION_TYPE_WIRED) return;

    hid_close(mouse->dev);
    mouse->dev = NULL;

    // Reconnect to wireless
    if (mouse->type == CONNECTION_TYPE_WIRELESS) mouse->state = RECONNECT;
}

void update_device_connection_attached(mouse_data *mouse, CONNECTION_TYPE last_connection_type) {
    // Wireless was plugged in with wired, do nothing
    if (last_connection_type == CONNECTION_TYPE_WIRED) return;

    // Wired plugged in with wireless, reconnect to wired
    if (last_connection_type == CONNECTION_TYPE_WIRELESS) {
        hid_close(mouse->dev);
        mouse->dev = NULL;
    }

    mouse->state = RECONNECT;
}