#include <libusb-1.0/libusb.h>
#include <hidapi/hidapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "types.h"
#include "device/mouse.h"
#include "hotplug/hotplug.h"

// For compatability with windows
void setup_mouse_removal_callbacks(mouse_hotplug_data *hotplug_data, struct hid_device_info *dev_list) {
    return;
}

struct hotplug_listener_data {
    libusb_hotplug_callback_handle hotplug_cb_handle_wired;
    libusb_hotplug_callback_handle hotplug_cb_handle_wireless;
};

/**
 * @brief Handles the attachment and detachment of the mouse.
 * 
 * @param ctx Unused
 * @param device The device that was attached/detached
 * @param event Whether the mouse was connected or disconnected
 * @param listener_data The mouse_data object
 * @return indicates whether this callback should stop handling hotplug events or not
 */
static int device_hotplug_callback(libusb_context *ctx, libusb_device *device,
    libusb_hotplug_event event, mouse_data *mouse
) {
    struct libusb_device_descriptor device_desc = {0};
    libusb_get_device_descriptor(device, &device_desc);

    update_mouse_connection_type(mouse, device_desc.idProduct, event);
    return false;
}

/**
 * @brief Handles the device hotplug events
 * 
 * @param hotplug_data The mouse_data object
 * @return void* Unused
 */
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