#include <stdio.h>
#include <stdint.h>
#include <hidapi/hidapi.h>
#include <gtk/gtk.h>

#include "mouse.h"
#include "rgb.h"

int change_color(hid_device *dev, color_options *options) {
	if (options->brightness < 0 || options->brightness > 100) {
		printf("Brightness must be between 0 - 100\n");
		return -1;
	}
	
	float multiplier = options->brightness / 100.0;
	byte red = options->red * multiplier;
	byte green = options->green * multiplier;
	byte blue = options->blue * multiplier;

	byte data[PACKET_SIZE] = {
		REPORT_FIRST_BYTE(SEND_BYTE_LED), 0x00, 0x00, 0x08,
		red, green, blue,
		red, green, blue,
		options->brightness
	};

	return mouse_write(dev, data);
}
