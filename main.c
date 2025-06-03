#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>

#include "enums.h"
#include "options.h"
#include "reports.h"

#define VID (0x03F0)
#define PID (0x028E)

#define INTERFACE (2)
#define ENDPOINT_OUT (0x04)
#define ENDPOINT_IN (0x83)
#define PACKET_SIZE (64)

/**
 * Opens the device handle for the mouse
 * and claims the interface used to change settings.
 * 
 * @return the mouse device handle
 */
libusb_device_handle* open_device() {
	libusb_device_handle *dev = libusb_open_device_with_vid_pid(NULL, VID, PID);
	
	if (!dev) {
		printf("Error opening mouse\n");
		return NULL;
	}
	
	int statusCode;
	statusCode = libusb_set_auto_detach_kernel_driver(dev, 1);
	
	if (statusCode < 0) {
		printf("Warning: Your system does not support kernal driver auto detachment: %s\n", libusb_error_name(statusCode));
	}

	statusCode = libusb_claim_interface(dev, INTERFACE);

	if (statusCode < 0) {
		printf("Error opening mouse interface: %s\n", libusb_error_name(statusCode));
		libusb_release_interface(dev, INTERFACE);
		return NULL;
	}

	return dev;
}

/**
 * Write data to the device.
 * 
 * @param dev The mouse device handle
 * @param data The packet data containing a request bt
 * @return LIBUSB_SUCCESS on success, or LIBUSB_ERROR if unsucessful
 */
int mouse_write(libusb_device_handle *dev,  uint8_t *data) {
	return libusb_interrupt_transfer(dev, ENDPOINT_OUT, data, PACKET_SIZE, NULL, 0);
}

/**
 * Read data from the device.
 *
 * @param dev The mouse device handle
 * @param reportType The report to request
 * @param data A buffer to store the output data
 * @return LIBUSB_SUCCESS on success, or LIBUSB_ERROR if unsucessful
 */
int mouse_read(libusb_device_handle *dev, MOUSE_REPORT reportType, uint8_t *data) {
	int res;

	data[0] = reportType;

	res = libusb_interrupt_transfer(dev, ENDPOINT_OUT, data, PACKET_SIZE, NULL, 0);
	if (res < 0) return res;

	data[0] = 0x00;
	
	return libusb_interrupt_transfer(dev, ENDPOINT_IN, data, PACKET_SIZE, NULL, 0);
}

/**
 * Changes the LED settings in direct mode.
 * 
 * @param dev The mouse device handle
 * @param options The options to set for the LED configuration
 */
void changeColor(libusb_device_handle *dev, color_options *options) {
	if (options->brightness < 0 || options->brightness > 100) {
		printf("Brightness must be between 0 - 100\n");
		return;
	}

	float multiplier = options->brightness / 100.0;

	uint8_t data[PACKET_SIZE] = {
		0xd2, 0x00, 0x00, 0x08, 
		(int) (options->red * multiplier),
		(int) (options->green * multiplier), 
		(int) (options->blue * multiplier), 
		(int) (options->red * multiplier),
		(int) (options->green * multiplier), 
		(int) (options->blue * multiplier), 
		options->brightness
	};

	int result = mouse_write(dev, data);
}

/**
 * Assigns a mouse button an action.
 * 
 * @param dev The mouse device handle
 * @param options The options for the mouse binding being changed
 */
void assignButton(libusb_device_handle *dev, button_options *options) {
	uint8_t data[PACKET_SIZE] = {0xd4, options->button, options->type, 0x02, options->action};
	
	int result = mouse_write(dev, data);
}

int main(int argc, char **argv) {
	int res;

	res = libusb_init_context(NULL, NULL, 0);
	libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_WARNING);

	if (res < 0) {
		printf("Error opening context\n");
		exit(-1);
	}

	libusb_device_handle *dev = open_device();
	if (dev == NULL) exit(-1);

	uint8_t data[PACKET_SIZE];
	mouse_read(dev, REPORT_HEARTBEAT, data);

	for (int i = 0; i < PACKET_SIZE; i++) {
		printf("%#0.2x ", data[i]);
	}
	
	printf("\n");

	libusb_release_interface(dev, INTERFACE);
	libusb_close(dev);
	
	libusb_exit(NULL);
	return 0;
}
