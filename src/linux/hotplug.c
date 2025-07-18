#include <libusb-1.0/libusb.h>
#include <hidapi/hidapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "types.h"
#include "device/mouse.h"
#include "hotplug.h"

struct hotplug_listener_data {
    libusb_hotplug_callback_handle hotplug_cb_handle_wired;
    libusb_hotplug_callback_handle hotplug_cb_handle_wireless;
} typedef hotplug_listener_data;

static void update_device_connection_detached(
    mouse_data *mouse,
    CONNECTION_TYPE last_connection_type
) {
    // Wireless was disconnected with wired plugged in, do nothing
    if (mouse->type == CONNECTION_TYPE_WIRED) {
        printf("Wireless unplugged\n");
        return;
    }

    hid_close(mouse->dev);
    mouse->dev = NULL;
    printf("Wired detached\n");

    // Reconnect to wireless
    if (mouse->type == CONNECTION_TYPE_WIRELESS) {
        printf("Attaching to mouse\n");
        mouse->state = RECONNECT;
    }

}

static void update_device_connection_attached(
    mouse_data *mouse,
    CONNECTION_TYPE last_connection_type
) {
    // Wireless was plugged in with wired, do nothing
    if (last_connection_type == CONNECTION_TYPE_WIRED) {
        printf("Wireless plugged in\n");
        return;
    } 

    // Wireless is no longer active, reconnect to wired
    if (last_connection_type == CONNECTION_TYPE_WIRELESS) {
        hid_close(mouse->dev);
        mouse->dev = NULL;
    }

    printf("Attaching to mouse\n");
    mouse->state = RECONNECT;
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
    libusb_hotplug_event event, mouse_data *mouse
) {
    struct libusb_device_descriptor device_desc = {0};
    libusb_get_device_descriptor(device, &device_desc);

    CONNECTION_TYPE connection_type = 
        (device_desc.idProduct == PID_WIRED)?
        CONNECTION_TYPE_WIRED:
        CONNECTION_TYPE_WIRELESS;
    
    g_mutex_lock(mouse->mutex);

    CONNECTION_TYPE last_connection_type = mouse->type;

    if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
        mouse->type -= connection_type;
        update_device_connection_detached(mouse, last_connection_type);
    } else {
        mouse->type += connection_type;
        update_device_connection_attached(mouse, last_connection_type);
    }

    g_mutex_unlock(mouse->mutex);

    return false;
}

static void* handle_events(mouse_hotplug_data *hotplug_data) {
    mouse_data *mouse = hotplug_data->mouse;

    while (mouse->state != CLOSED) {
        libusb_handle_events_completed(NULL, NULL);
        g_usleep(1000 * 100);
    }

    printf("hotplug listener exit\n");
    g_thread_exit(NULL);
    return NULL;
}

void hotplug_listener_init(mouse_hotplug_data *hotplug_data, mouse_data *mouse) {
    int res = libusb_init_context(NULL, NULL, 0);

    if (res != LIBUSB_SUCCESS) {
        printf("Could not initialize libusb context\n");
        printf("Reason: %s\n", libusb_error_name(res));
        exit(-1);
    }

    hotplug_data->mouse = mouse;
    hotplug_data->listener_data = malloc(sizeof(hotplug_listener_data));

    libusb_hotplug_register_callback(
        NULL,
        LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED
        | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
        0, VID, PID_WIRED, 
        LIBUSB_HOTPLUG_MATCH_ANY, 
        (libusb_hotplug_callback_fn) device_hotplug_callback,
        hotplug_data->mouse, 
        &hotplug_data->listener_data->hotplug_cb_handle_wired
    );

    libusb_hotplug_register_callback(
        NULL,
        LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED
        | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
        0, VID, PID_WIRELESS, 
        LIBUSB_HOTPLUG_MATCH_ANY, 
        (libusb_hotplug_callback_fn) device_hotplug_callback,
        hotplug_data->mouse, 
        &hotplug_data->listener_data->hotplug_cb_handle_wireless
    );

    GThread *thread = g_thread_new("handle_events", (GThreadFunc) handle_events, hotplug_data);
    hotplug_data->hotplug_thread = thread;
}

void hotplug_listener_exit(mouse_hotplug_data *hotplug_data) {
    libusb_hotplug_deregister_callback(NULL, hotplug_data->listener_data->hotplug_cb_handle_wired);
    libusb_hotplug_deregister_callback(NULL, hotplug_data->listener_data->hotplug_cb_handle_wireless);

    g_thread_join(hotplug_data->hotplug_thread);
	g_thread_unref(hotplug_data->hotplug_thread);

    free(hotplug_data->listener_data);
    
    libusb_exit(NULL);
}