#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <hidapi/hidapi.h>

#include "mouse.h"
#include "rgb.h"
#include "buttons.h"

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

int main() {
	int res;

	res = hid_init();
	HID_ERROR(res < 0, NULL);

	hid_device *dev = open_device();
	HID_ERROR(!dev, NULL);
	
	color_options options = {.red = 0xff, .green = 0xee, .blue = 0xdd, .brightness = 100};
	res = change_color(dev, &options);
	HID_ERROR(res < 0, dev);

	hid_close(dev);
	hid_exit();
	return 0;
}
