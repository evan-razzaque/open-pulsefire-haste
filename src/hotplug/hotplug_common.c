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