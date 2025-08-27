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
