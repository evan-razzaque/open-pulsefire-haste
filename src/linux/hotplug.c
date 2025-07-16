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
    libusb_hotplug_callback_handle hotplug_cb_handle_wired, hotplug_cb_handle_wireless;
    MOUSE_STATE *mouse_state;
    GThread *thread;
} typedef hotplug_listener_data;

/**
 * @brief Handles the attachment and deattachment of the mouse.
 * 
 * @param ctx Unused
 * @param device Unused
 * @param event Whether the mouse was connected or disconnected
 * @param listener_data The hotplug_listener_data object
 * @return indicates that this callback should continue handling hotplug events
 */
static int device_hotplug_callback(libusb_context *ctx, libusb_device *device,
    libusb_hotplug_event event, hotplug_listener_data *listener_data)
{
    if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
        printf("Device disconnected\n");
        hid_close(*listener_data->dev);
        *listener_data->dev = NULL;
    } else {
        printf("Device connected\n");
        *listener_data->mouse_state = RECONNECT;
    }

    return 0;
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

hotplug_listener_data* hotplug_listener_init(hid_device **dev, MOUSE_STATE *mouse_state) {
    int res = libusb_init_context(NULL, NULL, 0);

    if (res != LIBUSB_SUCCESS) {
        printf("Could not initialize libusb context\n");
        printf("Reason: %s\n", libusb_error_name(res));
        exit(-1);
    }

    hotplug_listener_data *listener_data = malloc(sizeof(hotplug_listener_data));
    listener_data->dev = dev;
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