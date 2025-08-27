/*
 * This file is part of the open-pulsefire-haste project
 * Copyright (C) 2025  Evan Razzaque
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. 
 */

#include <stdio.h>
#include <stdint.h>
#include <hidapi/hidapi.h>

#include "rgb.h"
#include "mouse.h"

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
