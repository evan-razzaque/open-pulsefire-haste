#include <stdio.h>
#include <stdint.h>
#include <hidapi/hidapi.h>

#include "mouse.h"
#include "rgb.h"

int change_color(hid_device *dev, color_options *options) {
	if (options->brightness < 0 || options->brightness > 100) {
		printf("Brightness must be between 0 - 100\n");
		return -1;
	}
	
	float multiplier = options->brightness / 100.0;
	uint8_t red = (uint8_t) (options->red * multiplier);
	uint8_t green = (uint8_t) (options->green * multiplier);
	uint8_t blue = (uint8_t) (options->blue * multiplier);

	uint8_t data[PACKET_SIZE] = {
		SEND_LED, 0x00, 0x00, 0x08,
		red, green, blue,
		red, green, blue,
		options->brightness
	};

	for (int i = 0; i < 14; i++) {
		printf("%#.2x ", data[i]);
	}

	printf("\n");

	return mouse_write(dev, data);
}