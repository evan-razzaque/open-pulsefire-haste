#ifndef RGB_H
#define RGB_H

#include <stdint.h>
#include <hidapi/hidapi.h>

struct color_options {
	uint8_t red; 
	uint8_t green; 
	uint8_t blue;
	uint8_t brightness;
} typedef color_options;

/**
 * Changes the LED settings in direct mode.
 * 
 * @param dev The mouse device handle
 * @param options The options to set for the LED configuration
 * @return the number of bytes written or -1 on error
 */
int change_color(hid_device *dev, color_options *options);

#endif