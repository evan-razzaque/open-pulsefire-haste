#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>
#include <stdbool.h>
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

#define READ_TIMEOUT (10) 

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
 * @brief An enum for the report type of a packet sent by the mouse.
 * Note that this is NOT the hid report id, so these
 * values must be wrapped with the REPORT_FIRST_BYTE macro
 * to ensure correct behaviour on both windows and *nix systems.
 */
enum REPORT_TYPE {
    // Mouse connection status
    REPORT_TYPE_CONNECTION           = 0x46,
    // Mouse hardware information
    REPORT_TYPE_HARDWARE             = 0x50,
    // Mouse battery status
    REPORT_TYPE_HEARTBEAT            = 0x51,
    // Mouse led settings
    REPORT_TYPE_ONBOARD_LED_SETTINGS = 0x52,
    // Mouse state change
    REPORT_TYPE_GENERIC_EVENT        = 0xff
} typedef REPORT_TYPE;

/**
 * @brief A union for reading information from report packets
 * sent by the mouse.
 * 
 */
union report_packet_data {
    byte _data[PACKET_SIZE];

    struct {
        byte _padding[FIRST_BYTE + 3];
        byte type;
        union {
            struct {
                bool is_mouse_active;
            } wireless;
            struct {
                byte unknown[2]; // Maybe charging?
            } wired;
        };
    } connection_status;

    struct {
        byte _padding[FIRST_BYTE + 4];
        uint16_t product_id;
        uint16_t vendor_id;
        uint32_t bcd_release_number;
        byte __padding[8];
        char product_string[32]; // null-terminated
    } hardware_info;

    struct {
        byte _padding[FIRST_BYTE + 4];
        byte battery_level;
        byte charing_status; // 0x00 (wireless) or 0x02 (wired)
    } heartbeat;

    struct {
        byte _padding[FIRST_BYTE + 7];
        byte brightness;
        struct rgb color;
        struct rgb effect_color;
    } led_settings;

    struct {
        byte _padding[FIRST_BYTE + 2];
        byte selected_dpi_profile;
        byte power_state; // 0x00 (wireless) or 0x02 (wired)
        bool is_awake;
        byte __padding[2];
        byte button_bitmask; // GENERIC_EVENT_BUTTON value(s)
    } generic_event;
};

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
 * @return the report type of the report that was read or -1 on error
 */
int mouse_read(hid_device *dev, byte *data);

/**
 * Send a read request to the mouse.
 *
 * @param dev The mouse device handle
 * @param report_type The type of report to request
 * @return the actual number of bytes read or -1 on error
 */
int mouse_send_read_request(hid_device *dev, REPORT_TYPE report_type);

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
