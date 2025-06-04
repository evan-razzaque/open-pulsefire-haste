#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <hidapi/hidapi.h>

#include "enums.h"
#include "options.h"
#include "reports.h"

#define VID (0x03F0)
#define PID_WIRELESS (0x028E)
#define PID_WIRED (0x048E)

#define INTERFACE (0x02)
#define PACKET_SIZE (64)
// #define ENDPOINT_OUT (0x04)
// #define ENDPOINT_IN (0x83)

/**
 * Used for debugging purposes.
 * Logs the last error caused by HIDAPI and exits the program.
 * 
 * @param cond The condition to detect the error
 * @param dev The device to close if not NULL
 */
#define HID_ERROR(cond, dev)\
	if ((cond)) {\
		printf("Error: %S\n", hid_error(NULL));\
		if ((dev)) hid_close((dev));\
		hid_exit();\
		return 1;\
	}

/**
 * Opens the mouse.
 * 
 * @return the mouse device handle
 */
hid_device* open_device() {
	struct hid_device_info *dev_list, *dev_info;
	hid_device *dev;

	dev_list = hid_enumerate(VID, PID_WIRELESS);

	if (!dev_list) {
		dev_list = hid_enumerate(VID, PID_WIRED);
        if (!dev_list) return NULL;
	}

	dev_info = dev_list;

	while (dev_info) {
		// printf("Path:           : %s\n", dev_info->path);
		// printf("- Manufacturer  : %ls\n", dev_info->manufacturer_string);
		// printf("- Product       : %ls\n", dev_info->product_string);
		// printf("- Serial number : %ls\n", dev_info->serial_number);
		// printf("- Release number: %hu\n", dev_info->release_number);
		// printf("- Usage Page    : %#04x\n", dev_info->usage_page);
		// printf("- Usage         : %#04x\n", dev_info->usage);
		// printf("- Interface     : %i\n", dev_info->interface_number);
		// printf("\n");

		if (dev_info->interface_number == INTERFACE) {
			dev = hid_open_path(dev_info->path);
			break;
		}

		dev_info = dev_info->next;
	}

	hid_free_enumeration(dev_list);
	return dev;
}

/**
 * Write data to the device.
 * 
 * @param dev The mouse device handle
 * @param data The packet data containing a request bt
 * @return the number of bytes written or -1 on error
 */
int mouse_write(hid_device *dev, uint8_t *data) {
	return hid_write(dev, data, PACKET_SIZE);
}

/**
 * Read data from the device.
 *
 * @param dev The mouse device handle
 * @param reportType The report to request
 * @param data A buffer to store the output data
 * @return the actual number of bytes read or -1 on error
 */
int mouse_read(hid_device *dev, MOUSE_REPORT reportType, uint8_t *data) {
	int res;

	data[0] = reportType;

	res = hid_write(dev, data, PACKET_SIZE);
	if (res < 0) return res;

	data[0] = 0x00;
	
	return hid_read(dev, data, PACKET_SIZE);
}

/**
 * Changes the LED settings in direct mode.
 * 
 * @param dev The mouse device handle
 * @param options The options to set for the LED configuration
 * @return the number of bytes written or -1 on error
 */
int change_color(hid_device *dev, color_options *options) {
	if (options->brightness < 0 || options->brightness > 100) {
		printf("Brightness must be between 0 - 100\n");
		return -1;
	}
	
	float multiplier = options->brightness / 100.0;

	uint8_t data[PACKET_SIZE] = {
		SEND_LED, 0x00, 0x00, 0x08,
		(int) (options->red * multiplier),
		(int) (options->green * multiplier), 
		(int) (options->blue * multiplier), 
		(int) (options->red * multiplier),
		(int) (options->green * multiplier), 
		(int) (options->blue * multiplier), 
		options->brightness
	};
	
	return mouse_write(dev, data);
}

/**
 * Change a binding for a mouse button.
 * 
 * @param dev The mouse device handle
 * @param options The mouse button to re-assign
 * @param action The action to assign to the button
 * @return the number of bytes written or -1 on error
 */
int assign_button(hid_device *dev, uint8_t button, MOUSE_ACTION action) {
	uint8_t type = action >> 8;

	uint8_t data[PACKET_SIZE] = {SEND_BUTTON_ASSIGNMENT, button, type, 0x02, (uint8_t) action, 0x00}; // TODO: Macro assignment (last byte is different)
	return mouse_write(dev, data);
}

/**
 * Saves the mouse settings to its on-board memory
 * 
 * @param dev The mouse device handle
 */
int save_settings(hid_device *dev) {
	uint8_t data[PACKET_SIZE] = {SAVE_SETTINGS, 0xff};

	return mouse_write(dev, data);
}

int main() {
	int res;

	res = hid_init();
	HID_ERROR(res < 0, NULL);

	hid_device *dev = open_device();
	HID_ERROR(!dev, NULL);
	
	color_options options = {.red = 0xff, .brightness = 50};
	res = change_color(dev, &options);
	HID_ERROR(res < 0, dev);

	res = assign_button(dev, SIDE_BUTTON_FORWARD, FORWARD);
	HID_ERROR(res < 0, dev);

	res = save_settings(dev);
	HID_ERROR(res < 0, dev);

	hid_close(dev);
	hid_exit();
	return 0;
}
