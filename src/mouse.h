#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>
#include <hidapi/hidapi.h>

#define VID (0x03F0)
#define PID_WIRELESS (0x028E)
#define PID_WIRED (0x048E)

#define INTERFACE (0x02)
#define PACKET_SIZE (64)

enum SEND_BYTE {
    SEND_POLLING_RATE        = 0xd0,
    SEND_LOWER_POWER_WARNING = 0xd1, // TODO: verify value
    SEND_LED                 = 0xd2,
    SEND_DPI                 = 0xd3,
    SEND_BUTTON_ASSIGNMENT   = 0xd4,
    SEND_MACRO_ASSIGNMENT    = 0xd5,
    SEND_MACRO_DATA          = 0xd6,
    SAVE_SETTINGS            = 0xda
} typedef SEND_BYTE;

enum MOUSE_REPORT {
    REPORT_CONNECTION           = 0x46,
    REPORT_HARDWARE             = 0x50,
    REPORT_HEARTBEAT            = 0x51,
    REPORT_ONBOARD_LED_SETTINGS = 0x52
} typedef MOUSE_REPORT;

/**
 * Opens the mouse.
 * 
 * @return the mouse device handle
 */
hid_device* open_device();

/**
 * Write data to the device.
 * 
 * @param dev The mouse device handle
 * @param data The packet data containing a request bt
 * @return the number of bytes written or -1 on error
 */
int mouse_write(hid_device *dev, uint8_t *data);

/**
 * Read data from the device.
 *
 * @param dev The mouse device handle
 * @param reportType The report to request
 * @param data A buffer to store the output data
 * @return the actual number of bytes read or -1 on error
 */
int mouse_read(hid_device *dev, MOUSE_REPORT reportType, uint8_t *data);

/**
 * Saves the mouse settings to its on-board memory
 * 
 * @param dev The mouse device handle
 */
int save_settings(hid_device *dev);

#endif