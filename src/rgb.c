#include <stdio.h>
#include <stdint.h>
#include <hidapi/hidapi.h>
#include <gtk/gtk.h>

#include "mouse.h"
#include "rgb.h"

void set_color(GtkApplication *app, void* color_options) {
	mouse_color* color_data = (mouse_color*) color_options;
	uint64_t new_color = color_data->new_color;

	color_data->prev_color->red = new_color >> 24;
	color_data->prev_color->green = new_color >> 16;
	color_data->prev_color->blue = new_color >> 8;
	color_data->prev_color->brightness = new_color;
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