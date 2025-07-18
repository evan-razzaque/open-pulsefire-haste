#include <libusb-1.0/libusb.h>
#include <hidapi/hidapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "types.h"
#include "device/mouse.h"
#include "hotplug.h"

struct hotplug_listener_data {
    hid_device **dev;
    CONNECTION_TYPE *connection_type; // Current connection type(s) to the mouse
    libusb_hotplug_callback_handle hotplug_cb_handle_wired, hotplug_cb_handle_wireless;
    MOUSE_STATE *mouse_state; // The state of the mouse
    GThread *thread;
} typedef hotplug_listener_data;

static void update_device_connection_detached(
    hotplug_listener_data *listener_data,
    CONNECTION_TYPE last_connection_type
) {
    hid_device **dev = listener_data->dev;

    // Wireless was disconnected with wired plugged in, do nothing
    if (*listener_data->connection_type == CONNECTION_TYPE_WIRED) {
        printf("Wireless unplugged with wired\n");
        return;
    }

    hid_close(*dev);
    *dev = NULL;
    printf("Detach wired\n");

    // Reconnect to wireless
    if (*listener_data->connection_type == CONNECTION_TYPE_WIRELESS) {
        *listener_data->mouse_state = RECONNECT;
    }

}

static void update_device_connection_attached(
    hotplug_listener_data *listener_data,
    CONNECTION_TYPE last_connection_type
) {
    hid_device **dev = listener_data->dev;

    // Wireless was plugged in with wired, do nothing
    if (last_connection_type == CONNECTION_TYPE_WIRED) {
        printf("Wireless plugged in with wired\n");
        return;
    } 

    // Wireless is no longer active, reconnect to wired
    if (last_connection_type == CONNECTION_TYPE_WIRELESS) {
        hid_close(*dev);
        *dev = NULL;
        printf("Wireless inactive\n");
    }

    *listener_data->mouse_state = RECONNECT; 
}


/**
 * @brief Handles the attachment and detachment of the mouse.
 * 
 * @param ctx Unused
 * @param device The device that was attached/detached
 * @param event Whether the mouse was connected or disconnected
 * @param listener_data The hotplug_listener_data object
 * @return indicates that this callback should stop handling hotplug events
 */
static int device_hotplug_callback(libusb_context *ctx, libusb_device *device,
    libusb_hotplug_event event, hotplug_listener_data *listener_data
) {
    struct libusb_device_descriptor device_desc = {0};
    libusb_get_device_descriptor(device, &device_desc);

    CONNECTION_TYPE last_connection_type = *listener_data->connection_type;

    CONNECTION_TYPE connection_type = 
        (device_desc.idProduct == PID_WIRED)?
        CONNECTION_TYPE_WIRED:
        CONNECTION_TYPE_WIRELESS;

    if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
        *listener_data->connection_type -= connection_type;
        update_device_connection_detached(listener_data, last_connection_type);
    } else {
        *listener_data->connection_type += connection_type;
        update_device_connection_attached(listener_data, last_connection_type);
    }

    return false;
}

void* handle_events(hotplug_listener_data *listener_data) {
    while (*listener_data->mouse_state != CLOSED) {
        libusb_handle_events_completed(NULL, NULL);
        g_usleep(1000 * 100);
    }

    printf("hotplug listener exit\n");
    g_thread_exit(NULL);
    return NULL;
}

hotplug_listener_data* hotplug_listener_init(hid_device **dev, MOUSE_STATE *mouse_state, CONNECTION_TYPE *connection_type) {
    int res = libusb_init_context(NULL, NULL, 0);

    if (res != LIBUSB_SUCCESS) {
        printf("Could not initialize libusb context\n");
        printf("Reason: %s\n", libusb_error_name(res));
        exit(-1);
    }

    hotplug_listener_data *listener_data = malloc(sizeof(hotplug_listener_data));
    listener_data->dev = dev;
    listener_data->connection_type = connection_type;
    listener_data->mouse_state = mouse_state;

    libusb_hotplug_register_callback(
        NULL,
        LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED
        | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
        0, VID, PID_WIRED, 
        LIBUSB_HOTPLUG_MATCH_ANY, 
        (libusb_hotplug_callback_fn) device_hotplug_callback,
        listener_data, 
        &listener_data->hotplug_cb_handle_wired
    );

    libusb_hotplug_register_callback(
        NULL,
        LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED
        | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
        0, VID, PID_WIRELESS, 
        LIBUSB_HOTPLUG_MATCH_ANY, 
        (libusb_hotplug_callback_fn) device_hotplug_callback,
        listener_data, 
        &listener_data->hotplug_cb_handle_wireless
    );

    GThread *thread = g_thread_new("handle_events", (GThreadFunc) handle_events, listener_data);
    listener_data->thread = thread;

    return listener_data;
}

void hotplug_listener_exit(hotplug_listener_data *listener_data) {
    libusb_hotplug_deregister_callback(NULL, listener_data->hotplug_cb_handle_wired);
    libusb_hotplug_deregister_callback(NULL, listener_data->hotplug_cb_handle_wireless);

    g_thread_join(listener_data->thread);
	g_thread_unref(listener_data->thread);

    libusb_exit(NULL);
}