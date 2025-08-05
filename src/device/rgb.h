#ifndef RGB_H
#define RGB_H

#include <stdint.h>
#include <hidapi/hidapi.h>

typedef uint8_t byte;

/**
 * @brief Color options for the mouse led.
 */
struct color_options {
	byte red; 
	byte green; 
	byte blue;
	byte brightness;
} typedef color_options;

struct rgb {
	byte red; 
	byte green; 
	byte blue;
};

/**
 * Changes the LED settings for the mouse.
 * 
 * @param dev The mouse device handle
 * @param options The options to set for the LED configuration
 * @return the number of bytes written or -1 on error
 */
int change_color(hid_device *dev, color_options *options);

#endif
