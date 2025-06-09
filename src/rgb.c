#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <hidapi/hidapi.h>
#include <gtk/gtk.h>

#include "mouse.h"
#include "rgb.h"

void set_color(GtkColorChooser *self, GdkRGBA *color, void *data) {
	mouse_data *mouse = (mouse_data*) data;
	
	mouse->led->red = (byte) (color->red * 255);
	mouse->led->green = (byte) (color->green * 255);
	mouse->led->blue = (byte) (color->blue * 255);
}

int change_color(hid_device *dev, color_options *options) {
	if (options->brightness < 0 || options->brightness > 100) {
		printf("Brightness must be between 0 - 100\n");
		return -1;
	}
	
	float multiplier = options->brightness / 100.0;
	byte red = (byte) (options->red * multiplier);
	byte green = (byte) (options->green * multiplier);
	byte blue = (byte) (options->blue * multiplier);

	byte data[PACKET_SIZE] = {
		REPORT_BYTE(SEND_LED), 0x00, 0x00, 0x08,
		red, green, blue,
		red, green, blue,
		options->brightness
	};

	return mouse_write(dev, data);
}