#ifndef WINVER
#define WINVER 0x0A00
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>
#include <cfgmgr32.h>
#include <initguid.h>
#include <usbiodef.h>
#include <hidclass.h>
#include <hidsdi.h>

#include "hotplug/hotplug.h"

struct device_connection_data {
    HANDLE device_handle;
    CM_NOTIFY_FILTER notify_filter_removal;
    HCMNOTIFICATION notify_context_removal;
    
    uint16_t product_id;
    bool device_connected;
    
    mouse_data *mouse;
} typedef device_connection_data;

struct hotplug_listener_data {
    CM_NOTIFY_FILTER notify_filter_arrival;
    HCMNOTIFICATION notify_context_wired;
    HCMNOTIFICATION notify_context_wireless;
    device_connection_data connection_data_wired;
    device_connection_data connection_data_wireless;
};

static HANDLE open_device_handle(void *path, bool is_wide_char) {
    if (is_wide_char) {
        return CreateFileW(
            (wchar_t*) path,
            0,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            0
        );
    } else {
        return CreateFileA(
            (char*) path,
            0,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            0
        );
    }
}

/**
 * @brief Get the device handle from a path and a product id.
 * 
 * @param path The path of the device
 * @param product_id The product id of the device
 * @return the device handle, or `INVALID_HANDLE_VALUE` if not found or an error has occured
 */
static HANDLE get_device_handle(wchar_t *path, uint16_t product_id) {
    HIDD_ATTRIBUTES attrib;
    HANDLE device_handle;

    device_handle = open_device_handle((void*) path, true);

    if (device_handle == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        printf("Could not open device. Error code %lu.\n", error);
        return device_handle;
    }

    BOOLEAN get_attribute_success = HidD_GetAttributes(device_handle, &attrib);

    if (!get_attribute_success) {
        DWORD error = GetLastError();
        printf("Could not read device attributes. Error code %lu.\n", error);
        goto handle_not_found;
    }

    if (attrib.ProductID != product_id) goto handle_not_found;

    return device_handle;

    handle_not_found:
        CloseHandle(device_handle);
        return INVALID_HANDLE_VALUE;
}

/**
 * @brief Handles the detachment of the mouse.
 * 
 * @param notify_handle Unused
 * @param connection_data The connection data for the mouse given its CONNECTION_TYPE
 * @param action The notification action that occured
 * @param event_data Unused
 * @param event_data_size Unused
 * @return Unused
 */
static CALLBACK DWORD device_hotplug_removal(HCMNOTIFICATION notify_handle, device_connection_data* connection_data,
    CM_NOTIFY_ACTION action, PCM_NOTIFY_EVENT_DATA event_data, DWORD event_data_size
) {
    if (action != DEVICE_REMOVE) {
        return 0;
    }

    mouse_data *mouse = connection_data->mouse;
    uint16_t product_id = connection_data->product_id;

    connection_data->device_connected = false;

    update_mouse_connection_type(mouse, product_id, action);
    
    printf("%d\n", CloseHandle(connection_data->device_handle));
    CM_Unregister_Notification(connection_data->notify_context_removal);

    return 0;
}

static void register_device_removal_callback(device_connection_data *connection_data, HANDLE device_handle) {
    connection_data->notify_filter_removal.u.DeviceHandle.hTarget = device_handle;
    connection_data->device_connected = true;
    connection_data->device_handle = device_handle;

    CM_Register_Notification(
        &connection_data->notify_filter_removal,
        connection_data,
        (PCM_NOTIFY_CALLBACK) device_hotplug_removal,
        &connection_data->notify_context_removal
    );
}

/**
 * @brief Handles the attachment of the mouse.
 * 
 * @param notify_handle Unused
 * @param connection_data The connection data for the mouse given its CONNECTION_TYPE
 * @param action The notification action that occured
 * @param event_data Unused
 * @param event_data_size Unused
 * @return Unused
 */
static CALLBACK DWORD device_hotplug_arrival(HCMNOTIFICATION notify_handle, device_connection_data* connection_data,
    CM_NOTIFY_ACTION action, PCM_NOTIFY_EVENT_DATA event_data, DWORD event_data_size
) {
    if (action != DEVICE_ARRIVE) return 0;
    if (connection_data->device_connected) return 0;

    mouse_data *mouse = connection_data->mouse;
    uint16_t product_id = connection_data->product_id;

    HANDLE device_handle = get_device_handle(event_data->u.DeviceInterface.SymbolicLink, product_id);
    if (device_handle == INVALID_HANDLE_VALUE) return 0;

    update_mouse_connection_type(mouse, product_id, action);

    register_device_removal_callback(connection_data, device_handle);

    return 0;
}

static void* handle_events(mouse_hotplug_data *hotplug_data) {
    mouse_data *mouse = hotplug_data->mouse;

    while (mouse->state != CLOSED) {
        g_usleep(1000 * 50);
        continue;
    }

    printf("hotplug listener exit\n");
    g_thread_exit(NULL);
    return NULL;
}

void setup_mouse_removal_callbacks(mouse_hotplug_data *hotplug_data, struct hid_device_info *dev_list) {
    struct hid_device_info *dev = dev_list;
    device_connection_data *connection_data;

    while (dev != NULL) {
        if (dev->interface_number != INTERFACE_NUMBER) {
            dev = dev->next;
            continue;
        }

        printf("%d\n", dev->interface_number);

        if (dev->product_id == PID_WIRED) {
            connection_data = &hotplug_data->listener_data->connection_data_wired;
        } else {
            connection_data = &hotplug_data->listener_data->connection_data_wireless;
        }

        HANDLE device_handle = open_device_handle(dev->path, false);
        register_device_removal_callback(connection_data, device_handle);

        dev = dev->next;
    }
}

void hotplug_listener_init(mouse_hotplug_data *hotplug_data, mouse_data *mouse) {
    hotplug_data->mouse = mouse;
    hotplug_data->listener_data = malloc(sizeof(hotplug_listener_data));
    
    hotplug_listener_data *listener_data = hotplug_data->listener_data;

    listener_data->notify_filter_arrival = (CM_NOTIFY_FILTER) {
        .cbSize = sizeof(CM_NOTIFY_FILTER),
        .FilterType = CM_NOTIFY_FILTER_TYPE_DEVICEINTERFACE,
        .u.DeviceInterface.ClassGuid = GUID_DEVINTERFACE_HID
    };

    device_connection_data connection_data = {
        .notify_filter_removal = {
            .cbSize = sizeof(CM_NOTIFY_FILTER),
            .FilterType = CM_NOTIFY_FILTER_TYPE_DEVICEHANDLE
        },
        .mouse = mouse
    };

    listener_data->connection_data_wired = connection_data;
    listener_data->connection_data_wired.product_id = PID_WIRED;

    listener_data->connection_data_wireless = connection_data;
    listener_data->connection_data_wireless.product_id = PID_WIRELESS;

    DWORD res;

    res = CM_Register_Notification(
        &listener_data->notify_filter_arrival,
        &listener_data->connection_data_wired,
        (PCM_NOTIFY_CALLBACK) device_hotplug_arrival,
        &listener_data->notify_context_wired
    );

    if (res != CR_SUCCESS) {
        printf("Could not register wired notification\n");
        exit(-1);
    }

    res = CM_Register_Notification(
        &listener_data->notify_filter_arrival,
        &listener_data->connection_data_wireless,
        (PCM_NOTIFY_CALLBACK) device_hotplug_arrival,
        &listener_data->notify_context_wireless
    );

    if (res != CR_SUCCESS) {
        printf("Could not register wireless notification\n");
        exit(-1);
    }

    GThread *thread = g_thread_new("handle_events", (GThreadFunc) handle_events, hotplug_data);
    hotplug_data->hotplug_thread = thread;
}

void hotplug_listener_exit(mouse_hotplug_data *hotplug_data) {
    device_connection_data *connection_data_wired = 
        &hotplug_data->listener_data->connection_data_wired;

    device_connection_data *connection_data_wireless = 
        &hotplug_data->listener_data->connection_data_wireless;

    if (connection_data_wired->device_connected) {            
        CM_Unregister_Notification(hotplug_data->listener_data->notify_context_wireless);
        CloseHandle(connection_data_wired->device_handle);
    }

    if (connection_data_wireless->device_connected) {
        CM_Unregister_Notification(hotplug_data->listener_data->notify_context_wired);
        CloseHandle(connection_data_wireless->device_handle);
    }

    g_thread_join(hotplug_data->hotplug_thread);
	g_thread_unref(hotplug_data->hotplug_thread);

    free(hotplug_data->listener_data);
}