#ifndef RGB_H
#define RGB_H

#include <stdint.h>
#include <hidapi/hidapi.h>

typedef uint8_t byte;

struct color_options {
	byte red; 
	byte green; 
	byte blue;
	byte brightness;
} typedef color_options;

struct mouse_color {
	color_options *prev_color;
	uint64_t new_color;
} typedef mouse_color;

/**
 * A function to set the LED settings for the mouse update thread.
 * 
 * @param app The gtk app instance
 * @param data A color_options object
 */
void set_color(GtkApplication *app, void* data);

/**
 * Changes the LED settings for the mouse.
 * 
 * @param dev The mouse device handle
 * @param options The options to set for the LED configuration
 * @return the number of bytes written or -1 on error
 */
int change_color(hid_device *dev, color_options *options);

#endif