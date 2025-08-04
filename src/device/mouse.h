#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>
#include <hidapi/hidapi.h>

#include "rgb.h"

#define VID (0x03F0)
#define PID_WIRELESS (0x028E)
#define PID_WIRED (0x048E)

#define INTERFACE_NUMBER (0x02)
#define TRUE_PACKET_SIZE (64) // Packet data length without report id

#ifdef _WIN32
// Shifts the packet data over a single byte for the (unused) report id (thanks windows)
#define REPORT_FIRST_BYTE(value) 0x00, (value)
#define PACKET_SIZE (65)
#else
// Everything other platform other than windows doesn't use report ids, so this is used for compatability
#define REPORT_FIRST_BYTE(byte) (byte)
#define PACKET_SIZE (64)
#endif

// The first byte of packet data
#define FIRST_BYTE (PACKET_SIZE - TRUE_PACKET_SIZE)
#define BUTTON_COUNT 6

/**
 * An enum for the status code when performing IO with the mouse.
 * Currently, this is only used for reading from the mouse.
 */
enum MOUSE_IO_STATUS {
    MOUSE_IO_STATUS_DEVICE_ERROR = -1,
    MOUSE_IO_STATUS_READ_MISS = -2
} typedef MOUSE_IO_STATUS;

/**
 * An enum that represents how the mouse is connected.
 * Can be a combination of any of these values,
 * or none to signify that the mouse is disconnected.
 */
enum CONNECTION_TYPE {
    CONNECTION_TYPE_WIRED        = 1,
    CONNECTION_TYPE_WIRELESS     = 2
} typedef CONNECTION_TYPE;

enum SEND_BYTE {
    SEND_BYTE_POLLING_RATE                  = 0xd0,
    SEND_BYTE_LOWER_POWER_WARNING           = 0xd1, // TODO: verify value
    SEND_BYTE_LED                           = 0xd2,
    SEND_BYTE_DPI                           = 0xd3,
    SEND_BYTE_BUTTON_ASSIGNMENT             = 0xd4,
    SEND_BYTE_MACRO_ASSIGNMENT              = 0xd5,
    SEND_BYTE_MACRO_DATA                    = 0xd6,
    
    // Ngenuity sends packets starting with this byte when saving settings, but as far as I can tell it seems to be unessesary
    SEND_BYTE_SAVE_SETTINGS_OLD             = 0xda, 
    SEND_BYTE_SAVE_SETTINGS                 = 0xde
} typedef SEND_BYTE;

/**
 * @brief An enum for the type of save to perform.
 * 
 */
enum SAVE_BYTE {
    SAVE_BYTE_ALL                    = 0xff,
    SAVE_BYTE_DPI_PROFILE_INDICATORS = 0x03
} typedef SAVE_BYTE;

/**
 * @brief An enum for report type for reading data.
 * Note that this is NOT the hid report id, so these
 * values must be wrapped with the REPORT_FIRST_BYTE macro
 * to ensure correct behaviour on both windows and *nix systems.
 */
enum REPORT_TYPE {
    REPORT_TYPE_CONNECTION           = 0x46,
    REPORT_TYPE_HARDWARE             = 0x50,
    REPORT_TYPE_HEARTBEAT            = 0x51,
    REPORT_TYPE_ONBOARD_LED_SETTINGS = 0x52
} typedef REPORT_TYPE;

/**
 * @brief An enum used to extract data from a specific index from read reports.
 */
enum REPORT_INDEX {
    REPORT_INDEX_BATTERY = FIRST_BYTE + 0x04
} typedef REPORT_INDEX;

/**
 * A helper function to print packet data in 16 byte rows.
 * 
 * @param data The packet data
 */
void print_data(byte *data);

/**
 * Gets a list of device info objects for each connection type.
 * 
 * @param connection_type Output location to store the type of connection
 * @return a list of device info objects
 */
struct hid_device_info* get_devices(CONNECTION_TYPE *connection_type);

/**
 * @brief Gets a list of device info objects for the active mouse connection.
 * 
 * @param connection_type The active mouse connection type
 * @return struct hid_device_info* a list of device info objects
 */
struct hid_device_info* get_active_devices(CONNECTION_TYPE connection_type);

/**
 * Opens the mouse device handle.
 * 
 * @param connection_type Output location to store the type of connection
 * @return the mouse device handle
 */
hid_device* open_device(struct hid_device_info *dev_list);

/**
 * Write data to the mouse.
 * 
 * @param dev The mouse device handle
 * @param data The packet data containing a request byte
 * @return the number of bytes written or -1 on error
 */
int mouse_write(hid_device *dev, byte *data);

/**
 * Read data from the mouse.
 *
 * @param dev The mouse device handle
 * @param report_type The type of report to read
 * @param data A buffer to store the data into
 * @return the actual number of bytes read or a MOUSE_IO_STATUS value on error
 */
MOUSE_IO_STATUS mouse_read(hid_device *dev, REPORT_TYPE report_type, byte *data);

/**
 * Send a read request to the mouse.
 *
 * @param dev The mouse device handle
 * @param report_type The type of report to request
 * @return the actual number of bytes read or -1 on error
 */
int mouse_send_read_request(hid_device *dev, REPORT_TYPE report_type);

/**
 * Gets the battery level of the mouse. mouse_send_read_request() must be called before hand in
 * order to eventually recieve data.
 * 
 * @param dev The mouse device handle
 * @param data A buffer to recieve read report data
 * @return the battery level of the mouse, or MOUSE_IO_STATUS value on error
 */
MOUSE_IO_STATUS mouse_get_battery_level(hid_device* dev, byte *data);

/**
 * @brief Set the polling rate for the mouse.
 * 
 * @param dev The mouse device handle
 * @param polling_rate_value The polling rate value
 * @return the number of bytes written or -1 on error
 */
int set_polling_rate(hid_device *dev, byte polling_rate_value);

/**
 * Saves the mouse settings to its on-board memory.
 * 
 * @param dev The mouse device handle
 * @return the number of bytes written or -1 on error
 */
int save_device_settings(hid_device *dev, color_options *color);

#endif
