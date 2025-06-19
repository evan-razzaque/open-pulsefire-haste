#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>
#include <hidapi/hidapi.h>

#include "rgb.h"

#define VID (0x03F0)
#define PID_WIRELESS (0x028E)
#define PID_WIRED (0x048E)

#define INTERFACE (0x02)
#define TRUE_PACKET_SIZE (64) // Packet data length without report id

#ifdef _WIN32
// Shifts the packet data over a single byte for the report id (thanks windows)
#define REPORT_BYTE(byte) 0x00, (byte)
#define PACKET_SIZE (65)
#else
#define REPORT_BYTE(byte) (byte)
#define PACKET_SIZE (64)
#endif

#define FIRST_BYTE (PACKET_SIZE - TRUE_PACKET_SIZE)
#define BUTTON_COUNT 6

typedef uint8_t byte;

enum CONNECTION_TYPE {
    CONNECTION_TYPE_WIRED,
    CONNECTION_TYPE_WIRELESS
} typedef CONNECTION_TYPE;

enum SEND_BYTE {
    SEND_BYTE_POLLING_RATE                  = 0xd0,
    SEND_BYTE_LOWER_POWER_WARNING           = 0xd1, // TODO: verify value
    SEND_BYTE_LED                           = 0xd2,
    SEND_BYTE_DPI                           = 0xd3,
    SEND_BYTE_BUTTON_ASSIGNMENT             = 0xd4,
    SEND_BYTE_MACRO_ASSIGNMENT              = 0xd5,
    SEND_BYTE_MACRO_DATA                    = 0xd6,
    SEND_BYTE_SAVE_SETTINGS_OLD             = 0xda, // Ngenuity sends packets starting with this byte, but AFAIK it seems to be unessesary
    SEND_BYTE_SAVE_SETTINGS                 = 0xde
} typedef SEND_BYTE;

enum REPORT_BYTE {
    REPORT_BYTE_CONNECTION           = 0x46,
    REPORT_BYTE_HARDWARE             = 0x50,
    REPORT_BYTE_HEARTBEAT            = 0x51,
    REPORT_BYTE_ONBOARD_LED_SETTINGS = 0x52
} typedef REPORT_BYTE;

enum REPORT_INDEX {
    REPORT_INDEX_BATTERY = 0x04
} typedef REPORT_INDEX;

/**
 * A helper function to print packet data
 * 
 * @param data The packet data
 */
void print_data(byte *data);

/**
 * Gets a list of device info objects for the active mouse connection.
 * 
 * @param connection_type Output location to store the type of connection (wired or wireless)
 * @return a list of device info objects
 */
struct hid_device_info* get_devices(CONNECTION_TYPE *connection_type);

/**
 * Opens the mouse.
 * 
 * @param connection_type Output location to store the type of connection (wired or wireless)
 * @return the mouse device handle
 */
hid_device* open_device(CONNECTION_TYPE *connection_type);

/**
 * Write data to the device.
 * 
 * @param dev The mouse device handle
 * @param data The packet data containing a request byte
 * @return the number of bytes written or -1 on error
 */
int mouse_write(hid_device *dev, byte *data);

/**
 * Read data from the device.
 *
 * @param dev The mouse device handle
 * @param reportType The report to request
 * @param data A buffer to store the output data
 * @return the actual number of bytes read or -1 on error
 */
int mouse_read(hid_device *dev, REPORT_BYTE reportType, byte *data);

/**
 * Returns the battery level of the mouse
 * 
 * @param dev The mouse device handle
 * @return the battery level of the mouse
 */
int get_battery_level(hid_device* dev);

/**
 * Saves the mouse settings to its on-board memory
 * 
 * @param dev The mouse device handle
 */
int save_settings(hid_device *dev, color_options *color);

#endif